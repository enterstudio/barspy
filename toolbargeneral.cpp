// 31 january 2017
#include "barspy.hpp"

static const uint8_t call386[] = {
	0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x3C, 0x8B, 0x45,
	0x08, 0x89, 0x45, 0xFC, 0x33, 0xC9, 0x89, 0x4D,
	0xE4, 0x89, 0x4D, 0xE8, 0x89, 0x4D, 0xEC, 0xC7,
	0x45, 0xE4, 0x0C, 0x00, 0x00, 0x00, 0x8D, 0x55,
	0xE4, 0x52, 0x6A, 0x00, 0x68, 0x03, 0x20, 0x00,
	0x00, 0x8B, 0x45, 0xFC, 0x8B, 0x48, 0x04, 0x51,
	0x8B, 0x55, 0xFC, 0x8B, 0x02, 0xFF, 0xD0, 0x89,
	0x45, 0xF8, 0x83, 0x7D, 0xF8, 0x00, 0x74, 0x1E,
	0x8B, 0x4D, 0xFC, 0xC7, 0x41, 0x08, 0x01, 0x00,
	0x00, 0x00, 0x8B, 0x55, 0xFC, 0x8B, 0x45, 0xE8,
	0x89, 0x42, 0x0C, 0x8B, 0x4D, 0xFC, 0x8B, 0x55,
	0xEC, 0x89, 0x51, 0x10, 0xEB, 0x0A, 0x8B, 0x45,
	0xFC, 0xC7, 0x40, 0x08, 0x00, 0x00, 0x00, 0x00,
	0x8D, 0x4D, 0xF0, 0x51, 0x6A, 0x00, 0x68, 0x53,
	0x04, 0x00, 0x00, 0x8B, 0x55, 0xFC, 0x8B, 0x42,
	0x04, 0x50, 0x8B, 0x4D, 0xFC, 0x8B, 0x11, 0xFF,
	0xD2, 0x89, 0x45, 0xF8, 0x83, 0x7D, 0xF8, 0x00,
	0x74, 0x1E, 0x8B, 0x45, 0xFC, 0xC7, 0x40, 0x14,
	0x01, 0x00, 0x00, 0x00, 0x8B, 0x4D, 0xFC, 0x8B,
	0x55, 0xF0, 0x89, 0x51, 0x18, 0x8B, 0x45, 0xFC,
	0x8B, 0x4D, 0xF4, 0x89, 0x48, 0x1C, 0xEB, 0x0A,
	0x8B, 0x55, 0xFC, 0xC7, 0x42, 0x14, 0x00, 0x00,
	0x00, 0x00, 0x33, 0xC0, 0x89, 0x45, 0xC4, 0x89,
	0x45, 0xC8, 0x89, 0x45, 0xCC, 0x89, 0x45, 0xD0,
	0x89, 0x45, 0xD4, 0x89, 0x45, 0xD8, 0x89, 0x45,
	0xDC, 0x89, 0x45, 0xE0, 0xC7, 0x45, 0xC4, 0x20,
	0x00, 0x00, 0x00, 0xC7, 0x45, 0xC8, 0x07, 0x00,
	0x00, 0x00, 0x8D, 0x4D, 0xC4, 0x51, 0x6A, 0x00,
	0x68, 0x65, 0x04, 0x00, 0x00, 0x8B, 0x55, 0xFC,
	0x8B, 0x42, 0x04, 0x50, 0x8B, 0x4D, 0xFC, 0x8B,
	0x11, 0xFF, 0xD2, 0x8B, 0x45, 0xFC, 0x8B, 0x4D,
	0xCC, 0x89, 0x48, 0x20, 0x8B, 0x55, 0xFC, 0x8B,
	0x45, 0xD0, 0x89, 0x42, 0x24, 0x8B, 0x4D, 0xFC,
	0x8B, 0x55, 0xD4, 0x89, 0x51, 0x28, 0x8B, 0x45,
	0xFC, 0x8B, 0x4D, 0xD8, 0x89, 0x48, 0x2C, 0x8B,
	0x55, 0xFC, 0x8B, 0x45, 0xDC, 0x89, 0x42, 0x30,
	0x8B, 0x4D, 0xFC, 0x8B, 0x55, 0xE0, 0x89, 0x51,
	0x34, 0x33, 0xC0, 0x8B, 0xE5, 0x5D, 0xC2, 0x04,
	0x00,
};
static const size_t nCall386 = 305;
// TODO static_assert these sizes in all files

static const uint8_t callAMD64[] = {
	0x48, 0x89, 0x4C, 0x24, 0x08, 0x57, 0x48, 0x83,
	0xEC, 0x70, 0x48, 0x8B, 0x84, 0x24, 0x80, 0x00,
	0x00, 0x00, 0x48, 0x89, 0x44, 0x24, 0x20, 0x48,
	0x8D, 0x44, 0x24, 0x38, 0x48, 0x8B, 0xF8, 0x33,
	0xC0, 0xB9, 0x0C, 0x00, 0x00, 0x00, 0xF3, 0xAA,
	0xC7, 0x44, 0x24, 0x38, 0x0C, 0x00, 0x00, 0x00,
	0x4C, 0x8D, 0x4C, 0x24, 0x38, 0x45, 0x33, 0xC0,
	0xBA, 0x03, 0x20, 0x00, 0x00, 0x48, 0x8B, 0x44,
	0x24, 0x20, 0x48, 0x8B, 0x48, 0x08, 0x48, 0x8B,
	0x44, 0x24, 0x20, 0xFF, 0x10, 0x48, 0x89, 0x44,
	0x24, 0x28, 0x48, 0x83, 0x7C, 0x24, 0x28, 0x00,
	0x74, 0x26, 0x48, 0x8B, 0x44, 0x24, 0x20, 0xC7,
	0x40, 0x10, 0x01, 0x00, 0x00, 0x00, 0x48, 0x8B,
	0x44, 0x24, 0x20, 0x8B, 0x4C, 0x24, 0x3C, 0x89,
	0x48, 0x14, 0x48, 0x8B, 0x44, 0x24, 0x20, 0x8B,
	0x4C, 0x24, 0x40, 0x89, 0x48, 0x18, 0xEB, 0x0C,
	0x48, 0x8B, 0x44, 0x24, 0x20, 0xC7, 0x40, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x4C, 0x8D, 0x4C, 0x24,
	0x30, 0x45, 0x33, 0xC0, 0xBA, 0x53, 0x04, 0x00,
	0x00, 0x48, 0x8B, 0x44, 0x24, 0x20, 0x48, 0x8B,
	0x48, 0x08, 0x48, 0x8B, 0x44, 0x24, 0x20, 0xFF,
	0x10, 0x48, 0x89, 0x44, 0x24, 0x28, 0x48, 0x83,
	0x7C, 0x24, 0x28, 0x00, 0x74, 0x26, 0x48, 0x8B,
	0x44, 0x24, 0x20, 0xC7, 0x40, 0x1C, 0x01, 0x00,
	0x00, 0x00, 0x48, 0x8B, 0x44, 0x24, 0x20, 0x8B,
	0x4C, 0x24, 0x30, 0x89, 0x48, 0x20, 0x48, 0x8B,
	0x44, 0x24, 0x20, 0x8B, 0x4C, 0x24, 0x34, 0x89,
	0x48, 0x24, 0xEB, 0x0C, 0x48, 0x8B, 0x44, 0x24,
	0x20, 0xC7, 0x40, 0x1C, 0x00, 0x00, 0x00, 0x00,
	0x48, 0x8D, 0x44, 0x24, 0x48, 0x48, 0x8B, 0xF8,
	0x33, 0xC0, 0xB9, 0x20, 0x00, 0x00, 0x00, 0xF3,
	0xAA, 0xC7, 0x44, 0x24, 0x48, 0x20, 0x00, 0x00,
	0x00, 0xC7, 0x44, 0x24, 0x4C, 0x07, 0x00, 0x00,
	0x00, 0x4C, 0x8D, 0x4C, 0x24, 0x48, 0x45, 0x33,
	0xC0, 0xBA, 0x65, 0x04, 0x00, 0x00, 0x48, 0x8B,
	0x44, 0x24, 0x20, 0x48, 0x8B, 0x48, 0x08, 0x48,
	0x8B, 0x44, 0x24, 0x20, 0xFF, 0x10, 0x48, 0x8B,
	0x44, 0x24, 0x20, 0x8B, 0x4C, 0x24, 0x50, 0x89,
	0x48, 0x28, 0x48, 0x8B, 0x44, 0x24, 0x20, 0x8B,
	0x4C, 0x24, 0x54, 0x89, 0x48, 0x2C, 0x48, 0x8B,
	0x44, 0x24, 0x20, 0x8B, 0x4C, 0x24, 0x58, 0x89,
	0x48, 0x30, 0x48, 0x8B, 0x44, 0x24, 0x20, 0x8B,
	0x4C, 0x24, 0x5C, 0x89, 0x48, 0x34, 0x48, 0x8B,
	0x44, 0x24, 0x20, 0x8B, 0x4C, 0x24, 0x60, 0x89,
	0x48, 0x38, 0x48, 0x8B, 0x44, 0x24, 0x20, 0x8B,
	0x4C, 0x24, 0x64, 0x89, 0x48, 0x3C, 0x33, 0xC0,
	0x48, 0x83, 0xC4, 0x70, 0x5F, 0xC3,
};
static const size_t nCallAMD64 = 374;

static ProcessHelper *mkProcessHelper(Process *p)
{
	ProcessHelper *ph;

	ph = new ProcessHelper(p);
	ph->SetCode(call386, nCall386, callAMD64, nCallAMD64);
	ph->AddField("SendMessageWPtr", fieldPointer, 0, 4, 0, 8);
	ph->AddField("hwnd", fieldPointer, 4, 4, 8, 8);
	ph->AddField("gsResultNonzero", fieldDWORD, 8, 4, 16, 4);
	ph->AddField("shadow", fieldCOLORREF, 12, 4, 20, 4);
	ph->AddField("highlight", fieldCOLORREF, 16, 4, 24, 4);
	ph->AddField("msResultNonzero", fieldDWORD, 20, 4, 28, 4);
	ph->AddField("maxWidth", fieldLONG, 24, 4, 32, 4);
	ph->AddField("maxHeight", fieldLONG, 28, 4, 36, 4);
	ph->AddField("gmCXPad", fieldInt, 32, 4, 40, 4);
	ph->AddField("gmCYPad", fieldInt, 36, 4, 44, 4);
	ph->AddField("gmCXBarPad", fieldInt, 40, 4, 48, 4);
	ph->AddField("gmCYBarPad", fieldInt, 44, 4, 52, 4);
	ph->AddField("gmCXButtonSpacing", fieldInt, 48, 4, 56, 4);
	ph->AddField("gmCYButtonSpacing", fieldInt, 52, 4, 60, 4);
	return ph;
}

ProcessHelper *getToolbarGeneral(HWND hwnd, Process *p)
{
	ProcessHelper *ph;
	void *puser32;

	ph = mkProcessHelper(p);

	// TODO add error checks to all these calls
	puser32 = p->GetModuleBase(L"kernel32.dll");

	ph->WriteFieldProcAddress("SendMessageWPtr", puser32, "SendMessageW");
	ph->WriteFieldPointer("hwnd", hwnd);

	ph->Run();
	return ph;
}
