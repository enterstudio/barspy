// 4 february 2017
#include "barspy.hpp"

static const uint8_t call386[] = {
	0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x08, 0x8B, 0x45,
	0x08, 0x89, 0x45, 0xFC, 0xC7, 0x45, 0xF8, 0x00,
	0x00, 0x00, 0x00, 0x8D, 0x4D, 0xF8, 0x51, 0x6A,
	0x00, 0x6A, 0x00, 0x8B, 0x55, 0xFC, 0x8B, 0x02,
	0xFF, 0xD0, 0x8B, 0x4D, 0xFC, 0x89, 0x41, 0x14,
	0x8B, 0x55, 0xFC, 0x83, 0x7A, 0x14, 0x00, 0x74,
	0x04, 0xEB, 0x43, 0xEB, 0x41, 0x8B, 0x45, 0xF8,
	0x50, 0x6A, 0x00, 0x8B, 0x4D, 0xFC, 0x8B, 0x51,
	0x0C, 0x52, 0x8B, 0x45, 0xFC, 0x8B, 0x48, 0x08,
	0xFF, 0xD1, 0x8B, 0x55, 0xFC, 0x89, 0x42, 0x14,
	0x8B, 0x45, 0xFC, 0x83, 0x78, 0x14, 0x00, 0x74,
	0x04, 0xEB, 0x1B, 0xEB, 0x19, 0x8B, 0x4D, 0xFC,
	0x83, 0xC1, 0x10, 0x51, 0x8B, 0x55, 0xF8, 0x52,
	0x8B, 0x45, 0xFC, 0x8B, 0x48, 0x04, 0xFF, 0xD1,
	0x8B, 0x55, 0xFC, 0x89, 0x42, 0x14, 0x83, 0x7D,
	0xF8, 0x00, 0x74, 0x0E, 0x8B, 0x45, 0xF8, 0x8B,
	0x08, 0x8B, 0x55, 0xF8, 0x52, 0x8B, 0x41, 0x08,
	0xFF, 0xD0, 0x33, 0xC0, 0x8B, 0xE5, 0x5D, 0xC2,
	0x04, 0x00,
};
static const size_t nCall386 = 146;

static const uint8_t callAMD64[] = {
	0x48, 0x89, 0x4C, 0x24, 0x08, 0x48, 0x83, 0xEC,
	0x38, 0x48, 0x8B, 0x44, 0x24, 0x40, 0x48, 0x89,
	0x44, 0x24, 0x20, 0x48, 0xC7, 0x44, 0x24, 0x28,
	0x00, 0x00, 0x00, 0x00, 0x4C, 0x8D, 0x44, 0x24,
	0x28, 0x33, 0xD2, 0x33, 0xC9, 0x48, 0x8B, 0x44,
	0x24, 0x20, 0xFF, 0x10, 0x48, 0x8B, 0x4C, 0x24,
	0x20, 0x89, 0x41, 0x28, 0x48, 0x8B, 0x44, 0x24,
	0x20, 0x83, 0x78, 0x28, 0x00, 0x74, 0x04, 0xEB,
	0x52, 0xEB, 0x50, 0x4C, 0x8B, 0x44, 0x24, 0x28,
	0x33, 0xD2, 0x48, 0x8B, 0x44, 0x24, 0x20, 0x48,
	0x8B, 0x48, 0x18, 0x48, 0x8B, 0x44, 0x24, 0x20,
	0xFF, 0x50, 0x10, 0x48, 0x8B, 0x4C, 0x24, 0x20,
	0x89, 0x41, 0x28, 0x48, 0x8B, 0x44, 0x24, 0x20,
	0x83, 0x78, 0x28, 0x00, 0x74, 0x04, 0xEB, 0x23,
	0xEB, 0x21, 0x48, 0x8B, 0x44, 0x24, 0x20, 0x48,
	0x83, 0xC0, 0x20, 0x48, 0x8B, 0xD0, 0x48, 0x8B,
	0x4C, 0x24, 0x28, 0x48, 0x8B, 0x44, 0x24, 0x20,
	0xFF, 0x50, 0x08, 0x48, 0x8B, 0x4C, 0x24, 0x20,
	0x89, 0x41, 0x28, 0x48, 0x83, 0x7C, 0x24, 0x28,
	0x00, 0x74, 0x10, 0x48, 0x8B, 0x44, 0x24, 0x28,
	0x48, 0x8B, 0x00, 0x48, 0x8B, 0x4C, 0x24, 0x28,
	0xFF, 0x50, 0x10, 0x33, 0xC0, 0x48, 0x83, 0xC4,
	0x38, 0xC3,
};
static const size_t nCallAMD64 = 178;

static ProcessHelper *mkProcessHelper(Process *p)
{
	ProcessHelper *ph;

	ph = new ProcessHelper(p);
	ph->SetCode(call386, nCall386, callAMD64, nCallAMD64);
	ph->AddField("CreateStreamOnHGlobalPtr", fieldPointer, 0, 4, 0, 8);
	ph->AddField("GetHGlobalFromStreamPtr", fieldPointer, 4, 4, 8, 8);
	ph->AddField("ImageList_WriteExPtr", fieldPointer, 8, 4, 16, 8);
	ph->AddField("imglist", fieldPointer, 12, 4, 24, 8);
	ph->AddField("hGlobal", fieldPointer, 16, 4, 32, 8);
	ph->AddField("hr", fieldHRESULT, 20, 4, 40, 4);
	return ph;
}

static HGLOBAL runThread(ProcessHelper *ph, HIMAGELIST imglist)
{
	HGLOBAL ret;
	HRESULT hr;

	ph->WriteFieldPointer("imglist", imglist);
	ph->Run();

	ret = (HGLOBAL) ph->ReadFieldPointer("hGlobal");
	ph->ReadField("hr", &hr);
	// TODO BreadcrumbBar in Windows Explorer returns E_NOINTERFACE
	if (hr != S_OK)
		panic(L"error serializing V6 HIMAGELIST: 0x%08I32X", hr);
	return ret;
}

HGLOBAL writeImageListV6(HWND hwnd, Process *p, HIMAGELIST imglist, void *pole32)
{
	HGLOBAL hGlobal;
	ProcessHelper *ph;
	void *pcomctl32;

	ph = mkProcessHelper(p);

	pcomctl32 = (void *) GetClassLongPtrW(hwnd, GCLP_HMODULE);

	ph->WriteFieldProcAddress("CreateStreamOnHGlobalPtr", pole32, "CreateStreamOnHGlobal");
	ph->WriteFieldProcAddress("GetHGlobalFromStreamPtr", pole32, "GetHGlobalFromStream");
	ph->WriteFieldProcAddress("ImageList_WriteExPtr", pcomctl32, "ImageList_WriteEx");

	hGlobal = runThread(ph, imglist);
	delete ph;
	return hGlobal;
}
