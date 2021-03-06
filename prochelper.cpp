// 31 january 2017
#include "barspy.hpp"

struct structField {
	int type;
	size_t off386;
	size_t size386;
	size_t offAMD64;
	size_t sizeAMD64;
};

// TODO convert everything to use these private types
struct ProcessHelperPriv {
	bool is64Bit;
	LPVOID pCode;
	LPVOID pData;

	size_t structSize386;
	size_t structSizeAMD64;
	std::map<std::string, struct structField *> *fields;
	size_t extraDataSize;
};

ProcessHelper::ProcessHelper(Process *p)
{
	this->p = p;

	this->priv = new struct ProcessHelperPriv;
	ZeroMemory(this->priv, sizeof (struct ProcessHelperPriv));
	this->priv->is64Bit = this->p->Is64Bit();
	this->priv->fields = new std::map<std::string, struct structField *>;
}

ProcessHelper::~ProcessHelper(void)
{
	Process *p = this->p;
	struct ProcessHelperPriv *priv = this->priv;

	for (auto f : *(priv->fields))
		delete f.second;
	delete priv->fields;
	if (priv->pData != NULL)
		p->FreeBlock(priv->pData);
	if (priv->pCode != NULL)
		p->FreeBlock(priv->pCode);
	delete priv;
}

void ProcessHelper::SetCode(const uint8_t *code386, size_t n386, const uint8_t *codeAMD64, size_t nAMD64)
{
	Process *p = this->p;
	struct ProcessHelperPriv *priv = this->priv;
	const uint8_t *code;
	size_t n;

	if (priv->pCode != NULL)
		panic(L"cannot set code twice");
	if (priv->is64Bit) {
		code = codeAMD64;
		n = nAMD64;
	} else {
		code = code386;
		n = n386;
	}
	priv->pCode = p->AllocBlock(n);
	p->Write(priv->pCode, 0, code, n);
	p->MakeExecutable(priv->pCode, n);
}

void ProcessHelper::AddField(const std::string &name, int type, size_t off386, size_t size386, size_t offAMD64, size_t sizeAMD64)
{
	struct ProcessHelperPriv *priv = this->priv;
	struct structField *f;

	if (priv->pData != NULL)
		panic(L"cannot add field to finalized data struct");
	f = (*(priv->fields))[name];
	if (f != NULL)
		panic(L"cannot add field with duplicate name");
	f = new struct structField;
	f->type = type;
	f->off386 = off386;
	f->size386 = size386;
	if (priv->structSize386 < (f->off386 + f->size386))
		priv->structSize386 = f->off386 + f->size386;
	f->offAMD64 = offAMD64;
	f->sizeAMD64 = sizeAMD64;
	if (priv->structSizeAMD64 < (f->offAMD64 + f->sizeAMD64))
		priv->structSizeAMD64 = f->offAMD64 + f->sizeAMD64;
	(*(priv->fields))[name] = f;
}

// catchall
template<typename T>
static bool compatibleTypes(int type)
{
	return false;
}

// note: this all assumes that the sizes of these types are the same across platforms; this is true for 386 and AMD64
// oh and we need to do instantiation because cross-object-file references
#define INSTANTIATE(T) \
	template void ProcessHelper::ReadField<T>(const std::string &field, T *out); \
	template void ProcessHelper::WriteField<T>(const std::string &field, T out);
#define COMPATIBLE(T, U) template<> static bool compatibleTypes<T>(int type) { return type == U; } INSTANTIATE(T)
#define COMPATIBLE2(T, U, U2) template<> static bool compatibleTypes<T>(int type) { return type == U || type == U2; } INSTANTIATE(T)
COMPATIBLE(ATOM, fieldATOM)
COMPATIBLE(UINT, fieldUINT)
COMPATIBLE2(DWORD,
	fieldDWORD,
	fieldCOLORREF)		// defined as typedef to DWORD
COMPATIBLE2(LONG,
	fieldLONG,
	fieldHRESULT)			// defined as typedef to LONG
COMPATIBLE2(int,
	fieldInt,
	fieldBOOL)

void ProcessHelper::finalizeData(void)
{
	Process *p = this->p;
	struct ProcessHelperPriv *priv = this->priv;
	size_t size;

	if (priv->pData != NULL)
		return;		// already set
	size = priv->structSize386;
	if (priv->is64Bit)
		size = priv->structSizeAMD64;
	priv->pData = p->AllocBlock(size + priv->extraDataSize);
}

template<typename T>
void ProcessHelper::ReadField(const std::string &field, T *out)
{
	Process *p = this->p;
	struct ProcessHelperPriv *priv = this->priv;
	struct structField *f;
	size_t off, size;

	this->finalizeData();
	f = (*(priv->fields))[field];
	if (!compatibleTypes<T>(f->type))
		panic(L"cannot read field of incompatible type");
	off = f->off386;
	size = f->size386;
	if (priv->is64Bit) {
		off = f->offAMD64;
		size = f->sizeAMD64;
	}
	if (sizeof (T) != size)
		panic(L"types incompatible by size when reading");
	p->Read(priv->pData, off, out, size);
}

template<typename T>
void ProcessHelper::WriteField(const std::string &field, T val)
{
	Process *p = this->p;
	struct ProcessHelperPriv *priv = this->priv;
	struct structField *f;
	size_t off, size;

	this->finalizeData();
	f = (*(this->priv->fields))[field];
	if (!compatibleTypes<T>(f->type))
		panic(L"cannot write field of incompatible type");
	// TODO change all if-elses that can be changed to be like this one
	off = f->off386;
	size = f->size386;
	if (priv->is64Bit) {
		off = f->offAMD64;
		size = f->sizeAMD64;
	}
	if (sizeof (T) != size)
		panic(L"types incompatible by size when writing");
	p->Write(priv->pData, off, &val, size);
}

void *ProcessHelper::ReadFieldPointer(const std::string &field)
{
	Process *p = this->p;
	struct ProcessHelperPriv *priv = this->priv;
	struct structField *f;
	uint32_t p32;
	uint64_t p64;

	this->finalizeData();
	f = (*(this->priv->fields))[field];
	if (f->type != fieldPointer)
		panic(L"cannot read pointer to field of incompatible type");
	if (priv->is64Bit) {
		p->Read(priv->pData, f->offAMD64, &p64, f->sizeAMD64);
		return (void *) p64;
	}
	p->Read(priv->pData, f->off386, &p32, f->size386);
	return (void *) p32;
}

void ProcessHelper::WriteFieldPointer(const std::string &field, void *ptr)
{
	Process *p = this->p;
	struct ProcessHelperPriv *priv = this->priv;
	struct structField *f;
	uint32_t p32;
	uint64_t p64;

	this->finalizeData();
	f = (*(this->priv->fields))[field];
	if (f->type != fieldPointer)
		panic(L"cannot write pointer to field of incompatible type");
	p32 = (uint32_t) ptr;
	p64 = (uint64_t) ptr;
	if (priv->is64Bit)
		p->Write(priv->pData, f->offAMD64, &p64, f->sizeAMD64);
	else
		p->Write(priv->pData, f->off386, &p32, f->size386);
}

void ProcessHelper::WriteFieldProcAddress(const std::string &field, void *modbase, const char *name)
{
	Process *p = this->p;
	void *proc;

	proc = p->GetProcAddress(modbase, name);
	if (proc == NULL)
		panic(L"procedure not found in module");
	this->WriteFieldPointer(field, proc);
}

void ProcessHelper::SetExtraDataSize(size_t n)
{
	struct ProcessHelperPriv *priv = this->priv;

	if (priv->pData != NULL)
		panic(L"cannot set extra data size of finalized data");
	priv->extraDataSize = n;
}

void *ProcessHelper::ExtraDataPtr(void)
{
	struct ProcessHelperPriv *priv = this->priv;
	uint8_t *d;
	size_t off;

	this->finalizeData();
	d = (uint8_t *) (priv->pData);
	off = priv->structSize386;
	if (priv->is64Bit)
		off = priv->structSizeAMD64;
	return d + off;
}

void *ProcessHelper::ReadExtraData(void)
{
	Process *p = this->p;
	struct ProcessHelperPriv *priv = this->priv;
	uint8_t *d;
	size_t off;

	this->finalizeData();
	d = new uint8_t[priv->extraDataSize];
	off = priv->structSize386;
	if (priv->is64Bit)
		off = priv->structSizeAMD64;
	p->Read(priv->pData, off, d, priv->extraDataSize);
	return d;
}

void ProcessHelper::WriteExtraData(const void *data)
{
	struct ProcessHelperPriv *priv = this->priv;

	this->WriteExtraData(data, priv->extraDataSize);
}

void ProcessHelper::WriteExtraData(const void *data, size_t size)
{
	Process *p = this->p;
	struct ProcessHelperPriv *priv = this->priv;
	size_t off;

	this->finalizeData();
	off = priv->structSize386;
	if (priv->is64Bit)
		off = priv->structSizeAMD64;
	p->Write(priv->pData, off, data, size);
}

void ProcessHelper::Run(void)
{
	Process *p = this->p;
	struct ProcessHelperPriv *priv = this->priv;
	HANDLE hThread;

	hThread = p->CreateThread(priv->pCode, priv->pData);
	// TODO switch to MsgWaitForMultipleObjectsEx()? this code assumes it is atomic with regards to the UI
	if (WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED)
		panic(L"error waiting for thread to run: %I32d", GetLastError());
	if (CloseHandle(hThread) == 0)
		panic(L"error closing thread: %I32d", GetLastError());
}
