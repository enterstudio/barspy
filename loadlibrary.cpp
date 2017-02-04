// 3 february 2017
#include "barspy.hpp"

static const uint8_t call386[] = {
	0x55, 0x8B, 0xEC, 0x51, 0x8B, 0x45, 0x08, 0x89,
	0x45, 0xFC, 0x8B, 0x4D, 0xFC, 0x8B, 0x51, 0x08,
	0x52, 0x8B, 0x45, 0xFC, 0x8B, 0x08, 0xFF, 0xD1,
	0x8B, 0x55, 0xFC, 0x89, 0x42, 0x0C, 0x8B, 0x45,
	0xFC, 0x8B, 0x48, 0x04, 0xFF, 0xD1, 0x8B, 0x55,
	0xFC, 0x89, 0x42, 0x10, 0x33, 0xC0, 0x8B, 0xE5,
	0x5D, 0xC2, 0x04, 0x00,
};
static const size_t nCall386 = 52;

static const uint8_t callAMD64[] = {
	0x48, 0x89, 0x4C, 0x24, 0x08, 0x48, 0x83, 0xEC,
	0x38, 0x48, 0x8B, 0x44, 0x24, 0x40, 0x48, 0x89,
	0x44, 0x24, 0x20, 0x48, 0x8B, 0x44, 0x24, 0x20,
	0x48, 0x8B, 0x48, 0x10, 0x48, 0x8B, 0x44, 0x24,
	0x20, 0xFF, 0x10, 0x48, 0x8B, 0x4C, 0x24, 0x20,
	0x48, 0x89, 0x41, 0x18, 0x48, 0x8B, 0x44, 0x24,
	0x20, 0xFF, 0x50, 0x08, 0x48, 0x8B, 0x4C, 0x24,
	0x20, 0x89, 0x41, 0x20, 0x33, 0xC0, 0x48, 0x83,
	0xC4, 0x38, 0xC3,
};
static const size_t nCallAMD64 = 67;

static ProcessHelper *mkProcessHelper(Process *p, const WCHAR *name)
{
	ProcessHelper *ph;

	ph = new ProcessHelper(p);
	ph->SetCode(call386, nCall386, callAMD64, nCallAMD64);
	ph->AddField("LoadLibraryWPtr", fieldPointer, 0, 4, 0, 8);
	ph->AddField("GetLastErrorPtr", fieldPointer, 4, 4, 8, 8);
	ph->AddField("name", fieldPointer, 8, 4, 16, 8);
	ph->AddField("module", fieldPointer, 12, 4, 24, 8);
	ph->AddField("lastError", fieldDWORD, 16, 4, 32, 4);
	// extra just to be safe
	ph->SetExtraDataSize((wcslen(name) + 1) * sizeof (WCHAR));
	ph->WriteExtraData(name);
	return ph;
}

static HMODULE runThread(ProcessHelper *ph)
{
	HMODULE ret;
	DWORD lastError;

	ph->Run();

	ret = (HMODULE) ph->ReadFieldPointer("module");
	ph->ReadField("lastError", &lastError);
	// TODO allow this to fail gracefully
	if (ret == NULL)
		panic(L"error loading OLE32 into process: %I32d", lastError);
	return ret;
}

HMODULE loadLibrary(Process *p, const WCHAR *name)
{
	HMODULE module;
	ProcessHelper *ph;
	void *pkernel32;

	ph = mkProcessHelper(p, name);

	pkernel32 = p->GetModuleBase(L"kernel32.dll");

	ph->WriteFieldProcAddress("LoadLibraryWPtr", pkernel32, "LoadLibraryW");
	ph->WriteFieldProcAddress("GetLastErrorPtr", pkernel32, "GetLastError");
	ph->WriteFieldPointer("name", ph->ExtraDataPtr());

	module = runThread(ph);
	delete ph;
	return module;
}
