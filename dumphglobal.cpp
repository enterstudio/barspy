// 4 february 2017
#include "barspy.hpp"

static const uint8_t call386[] = {
	0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x14, 0x8B, 0x45,
	0x08, 0x89, 0x45, 0xFC, 0x8B, 0x4D, 0xFC, 0x8B,
	0x51, 0x18, 0x52, 0x8B, 0x45, 0xFC, 0x8B, 0x08,
	0xFF, 0xD1, 0x8B, 0x55, 0xFC, 0x89, 0x42, 0x20,
	0x8B, 0x45, 0xFC, 0x83, 0x78, 0x20, 0x00, 0x75,
	0x0A, 0xE9, 0xFF, 0x00, 0x00, 0x00, 0xE9, 0xFA,
	0x00, 0x00, 0x00, 0x8B, 0x4D, 0xFC, 0x8B, 0x51,
	0x18, 0x52, 0x8B, 0x45, 0xFC, 0x8B, 0x48, 0x04,
	0xFF, 0xD1, 0x89, 0x45, 0xEC, 0x83, 0x7D, 0xEC,
	0x00, 0x75, 0x0A, 0xE9, 0xDD, 0x00, 0x00, 0x00,
	0xE9, 0xD8, 0x00, 0x00, 0x00, 0x6A, 0x04, 0x68,
	0x00, 0x10, 0x00, 0x00, 0x8B, 0x55, 0xFC, 0x8B,
	0x42, 0x20, 0x50, 0x6A, 0x00, 0x8B, 0x4D, 0xFC,
	0x8B, 0x51, 0x10, 0xFF, 0xD2, 0x8B, 0x4D, 0xFC,
	0x89, 0x41, 0x1C, 0x8B, 0x55, 0xFC, 0x83, 0x7A,
	0x1C, 0x00, 0x75, 0x0A, 0xE9, 0xAC, 0x00, 0x00,
	0x00, 0xE9, 0xA7, 0x00, 0x00, 0x00, 0x8B, 0x45,
	0xFC, 0x8B, 0x48, 0x1C, 0x89, 0x4D, 0xF4, 0x8B,
	0x55, 0xEC, 0x89, 0x55, 0xF0, 0xC7, 0x45, 0xF8,
	0x00, 0x00, 0x00, 0x00, 0xEB, 0x09, 0x8B, 0x45,
	0xF8, 0x83, 0xC0, 0x01, 0x89, 0x45, 0xF8, 0x8B,
	0x4D, 0xFC, 0x8B, 0x55, 0xF8, 0x3B, 0x51, 0x20,
	0x73, 0x1E, 0x8B, 0x45, 0xF4, 0x8B, 0x4D, 0xF0,
	0x8A, 0x11, 0x88, 0x10, 0x8B, 0x45, 0xF4, 0x83,
	0xC0, 0x01, 0x89, 0x45, 0xF4, 0x8B, 0x4D, 0xF0,
	0x83, 0xC1, 0x01, 0x89, 0x4D, 0xF0, 0xEB, 0xCE,
	0x8B, 0x55, 0xFC, 0x8B, 0x42, 0x18, 0x50, 0x8B,
	0x4D, 0xFC, 0x8B, 0x51, 0x08, 0xFF, 0xD2, 0x85,
	0xC0, 0x75, 0x25, 0x8B, 0x45, 0xFC, 0x8B, 0x48,
	0x14, 0xFF, 0xD1, 0x8B, 0x55, 0xFC, 0x89, 0x42,
	0x28, 0x8B, 0x45, 0xFC, 0x83, 0x78, 0x28, 0x00,
	0x74, 0x0E, 0x8B, 0x4D, 0xFC, 0xC7, 0x41, 0x24,
	0x00, 0x00, 0x00, 0x00, 0x33, 0xC0, 0xEB, 0x3F,
	0x8B, 0x55, 0xFC, 0x8B, 0x42, 0x18, 0x50, 0x8B,
	0x4D, 0xFC, 0x8B, 0x51, 0x0C, 0xFF, 0xD2, 0x85,
	0xC0, 0x74, 0x04, 0xEB, 0x10, 0xEB, 0x0E, 0x8B,
	0x45, 0xFC, 0xC7, 0x40, 0x24, 0x01, 0x00, 0x00,
	0x00, 0x33, 0xC0, 0xEB, 0x1A, 0x8B, 0x4D, 0xFC,
	0x8B, 0x51, 0x14, 0xFF, 0xD2, 0x8B, 0x4D, 0xFC,
	0x89, 0x41, 0x28, 0x8B, 0x55, 0xFC, 0xC7, 0x42,
	0x24, 0x00, 0x00, 0x00, 0x00, 0x33, 0xC0, 0x8B,
	0xE5, 0x5D, 0xC2, 0x04, 0x00,
};
static const size_t nCall386 = 333;

static const uint8_t callAMD64[] = {
	0x48, 0x89, 0x4C, 0x24, 0x08, 0x48, 0x83, 0xEC,
	0x58, 0x48, 0x8B, 0x44, 0x24, 0x60, 0x48, 0x89,
	0x44, 0x24, 0x20, 0x48, 0x8B, 0x44, 0x24, 0x20,
	0x48, 0x8B, 0x48, 0x30, 0x48, 0x8B, 0x44, 0x24,
	0x20, 0xFF, 0x10, 0x48, 0x8B, 0x4C, 0x24, 0x20,
	0x48, 0x89, 0x41, 0x40, 0x48, 0x8B, 0x44, 0x24,
	0x20, 0x48, 0x83, 0x78, 0x40, 0x00, 0x75, 0x0A,
	0xE9, 0x40, 0x01, 0x00, 0x00, 0xE9, 0x3B, 0x01,
	0x00, 0x00, 0x48, 0x8B, 0x44, 0x24, 0x20, 0x48,
	0x8B, 0x48, 0x30, 0x48, 0x8B, 0x44, 0x24, 0x20,
	0xFF, 0x50, 0x08, 0x48, 0x89, 0x44, 0x24, 0x40,
	0x48, 0x83, 0x7C, 0x24, 0x40, 0x00, 0x75, 0x0A,
	0xE9, 0x18, 0x01, 0x00, 0x00, 0xE9, 0x13, 0x01,
	0x00, 0x00, 0x41, 0xB9, 0x04, 0x00, 0x00, 0x00,
	0x41, 0xB8, 0x00, 0x10, 0x00, 0x00, 0x48, 0x8B,
	0x44, 0x24, 0x20, 0x48, 0x8B, 0x50, 0x40, 0x33,
	0xC9, 0x48, 0x8B, 0x44, 0x24, 0x20, 0xFF, 0x50,
	0x20, 0x48, 0x8B, 0x4C, 0x24, 0x20, 0x48, 0x89,
	0x41, 0x38, 0x48, 0x8B, 0x44, 0x24, 0x20, 0x48,
	0x83, 0x78, 0x38, 0x00, 0x75, 0x0A, 0xE9, 0xDA,
	0x00, 0x00, 0x00, 0xE9, 0xD5, 0x00, 0x00, 0x00,
	0x48, 0x8B, 0x44, 0x24, 0x20, 0x48, 0x8B, 0x40,
	0x38, 0x48, 0x89, 0x44, 0x24, 0x30, 0x48, 0x8B,
	0x44, 0x24, 0x40, 0x48, 0x89, 0x44, 0x24, 0x38,
	0x48, 0xC7, 0x44, 0x24, 0x28, 0x00, 0x00, 0x00,
	0x00, 0xEB, 0x0D, 0x48, 0x8B, 0x44, 0x24, 0x28,
	0x48, 0xFF, 0xC0, 0x48, 0x89, 0x44, 0x24, 0x28,
	0x48, 0x8B, 0x44, 0x24, 0x20, 0x48, 0x8B, 0x40,
	0x40, 0x48, 0x39, 0x44, 0x24, 0x28, 0x73, 0x2B,
	0x48, 0x8B, 0x44, 0x24, 0x30, 0x48, 0x8B, 0x4C,
	0x24, 0x38, 0x0F, 0xB6, 0x09, 0x88, 0x08, 0x48,
	0x8B, 0x44, 0x24, 0x30, 0x48, 0xFF, 0xC0, 0x48,
	0x89, 0x44, 0x24, 0x30, 0x48, 0x8B, 0x44, 0x24,
	0x38, 0x48, 0xFF, 0xC0, 0x48, 0x89, 0x44, 0x24,
	0x38, 0xEB, 0xB8, 0x48, 0x8B, 0x44, 0x24, 0x20,
	0x48, 0x8B, 0x48, 0x30, 0x48, 0x8B, 0x44, 0x24,
	0x20, 0xFF, 0x50, 0x10, 0x85, 0xC0, 0x75, 0x2B,
	0x48, 0x8B, 0x44, 0x24, 0x20, 0xFF, 0x50, 0x28,
	0x48, 0x8B, 0x4C, 0x24, 0x20, 0x89, 0x41, 0x4C,
	0x48, 0x8B, 0x44, 0x24, 0x20, 0x83, 0x78, 0x4C,
	0x00, 0x74, 0x10, 0x48, 0x8B, 0x44, 0x24, 0x20,
	0xC7, 0x40, 0x48, 0x00, 0x00, 0x00, 0x00, 0x33,
	0xC0, 0xEB, 0x48, 0x48, 0x8B, 0x44, 0x24, 0x20,
	0x48, 0x8B, 0x48, 0x30, 0x48, 0x8B, 0x44, 0x24,
	0x20, 0xFF, 0x50, 0x18, 0x48, 0x85, 0xC0, 0x74,
	0x04, 0xEB, 0x12, 0xEB, 0x10, 0x48, 0x8B, 0x44,
	0x24, 0x20, 0xC7, 0x40, 0x48, 0x01, 0x00, 0x00,
	0x00, 0x33, 0xC0, 0xEB, 0x1E, 0x48, 0x8B, 0x44,
	0x24, 0x20, 0xFF, 0x50, 0x28, 0x48, 0x8B, 0x4C,
	0x24, 0x20, 0x89, 0x41, 0x4C, 0x48, 0x8B, 0x44,
	0x24, 0x20, 0xC7, 0x40, 0x48, 0x00, 0x00, 0x00,
	0x00, 0x33, 0xC0, 0x48, 0x83, 0xC4, 0x58, 0xC3,
};
static const size_t nCallAMD64 = 416;

static ProcessHelper *mkProcessHelper(Process *p)
{
	ProcessHelper *ph;

	ph = new ProcessHelper(p);
	ph->SetCode(call386, nCall386, callAMD64, nCallAMD64);
	ph->AddField("GlobalSizePtr", fieldPointer, 0, 4, 0, 8);
	ph->AddField("GlobalLockPtr", fieldPointer, 4, 4, 8, 8);
	ph->AddField("GlobalUnlockPtr", fieldPointer, 8, 2, 16, 8);
	ph->AddField("GlobalFreePtr", fieldPointer, 12, 4, 24, 8);
	ph->AddField("VirtualAllocPtr", fieldPointer, 16, 4, 32, 8);
	ph->AddField("GetLastErrorPtr", fieldPointer, 20, 4, 40, 8);
	ph->AddField("hGlobal", fieldPointer, 24, 4, 48, 8);
	ph->AddField("memory", fieldPointer, 28, 4, 56, 8);
	ph->AddField("size", fieldPointer, 32, 4, 64, 8);		// pointer-sized
	ph->AddField("success", fieldBOOL, 36, 4, 72, 4);
	ph->AddField("lastError", fieldDWORD, 40, 4, 76, 4);
	return ph;
}

static uint8_t *runThread(ProcessHelper *ph, Process *p, HGLOBAL hGlobal, SIZE_T *size)
{
	uint8_t *buf;
	void *off;
	BOOL success;
	DWORD lastError;

	ph->WriteFieldPointer("hGlobal", hGlobal);
	ph->Run();

	ph->ReadField("success", &success);
	ph->ReadField("lastError", &lastError);
	if (!success)
		panic(L"error reading stream output: %I32d", lastError);

	off = ph->ReadFieldPointer("memory");
	*size = (SIZE_T) ph->ReadFieldPointer("size");
	buf = new uint8_t[*size];
	p->Read(off, 0, buf, *size);
	p->FreeBlock(off);
	return buf;
}

uint8_t *dumpHGLOBALStreamData(Process *p, HGLOBAL hGlobal, SIZE_T *size)
{
	uint8_t *buf;
	ProcessHelper *ph;
	void *pkernel32;

	ph = mkProcessHelper(p);

	pkernel32 = p->GetModuleBase(L"kernel32.dll");

	ph->WriteFieldProcAddress("GlobalSizePtr", pkernel32, "GlobalSize");
	ph->WriteFieldProcAddress("GlobalLockPtr", pkernel32, "GlobalLock");
	ph->WriteFieldProcAddress("GlobalUnlockPtr", pkernel32, "GlobalUnlock");
	ph->WriteFieldProcAddress("GlobalFreePtr", pkernel32, "GlobalFree");
	ph->WriteFieldProcAddress("VirtualAllocPtr", pkernel32, "VirtualAlloc");
	ph->WriteFieldProcAddress("GetLastErrorPtr", pkernel32, "GetLastError");

	buf = runThread(ph, p, hGlobal, size);
	delete ph;
	return buf;
}
