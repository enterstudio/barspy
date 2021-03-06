// 26 january 2017
#include "barspy.hpp"

#define swtpszSubAppName 0
#define swtpszSubIdList 1
#define stylesStyles 0
#define stylesExStyles 1

// TODO rename idoff to itemid
// TODO find a sensible layout for the colon or switch to a pointer
Common::Common(HWND parent, int idoff) :
	version(parent),
	unicode(parent),
	setWindowTheme(parent),
	styles(parent)
{
	this->version.SetID(idoff);
	this->version.SetMinEditWidth(35);
	this->version.Add(L"comctl32.dll Version");
	idoff = this->version.ID();

	this->unicode.SetID(idoff);
	this->unicode.AddCheckmark(L"Unicode");
	idoff = this->unicode.ID();

	this->setWindowTheme.SetID(idoff);
	this->setWindowTheme.SetMinEditWidth(50);
	this->setWindowTheme.SetPadded(false);
	this->setWindowTheme.Add(L"SetWindowTheme(");
	this->setWindowTheme.Add(L", ");
	this->setWindowTheme.AddTrailingLabel(L")");
	idoff = this->setWindowTheme.ID();

	this->styles.SetMinEditWidth(100);
	this->styles.SetID(idoff);
	this->styles.Add(L"Styles");
	this->styles.Add(L"Extended Styles");
	idoff = this->styles.ID();
}

void Common::Reset(void)
{
	this->version.SetText(0, L"N/A");
	this->unicode.SetCheckmark(0, hIconUnknown);
	this->setWindowTheme.SetText(swtpszSubAppName, L"N/A");
	this->setWindowTheme.SetText(swtpszSubIdList, L"N/A");
	this->styles.SetText(stylesStyles, L"N/A");
	this->styles.SetText(stylesExStyles, L"N/A");
}

static WCHAR *nullCopy(void)
{
	WCHAR *s;

	s = new WCHAR[5];
	s[0] = L'N';
	s[1] = L'U';
	s[2] = L'L';
	s[3] = L'L';
	s[4] = L'\0';
	return s;
}

void Common::Reflect(HWND hwnd, Process *p)
{
	int wc;
	WCHAR *pszSubAppName, *pszSubIdList;

	this->Reset();

	// only get these if this is a known control
	wc = windowClassOf(hwnd, DESIREDCLASSES, NULL);
	if (wc != -1) {
		WCHAR *dgv;
		HICON iconToUse;
		std::wstring s;

		dgv = getDLLVersion(hwnd, p);
		this->version.SetText(0, dgv);
		delete[] dgv;

		iconToUse = hIconNo;
		if (SendMessageW(hwnd, CCM_GETUNICODEFORMAT, 0, 0) != 0)
			iconToUse = hIconYes;
		this->unicode.SetCheckmark(0, iconToUse);

		switch (wc) {
		case DESIREDTOOLBAR:
			s = toolbarStyleString(hwnd);
			this->styles.SetText(stylesStyles, s.c_str());
			s = toolbarExStyleString(hwnd);
			this->styles.SetText(stylesExStyles, s.c_str());
			break;
		case DESIREDREBAR:
			// TODO
			break;
		}
	}

	// always set this
	getWindowTheme(hwnd, p, &pszSubAppName, &pszSubIdList);
	if (pszSubAppName == NULL)
		pszSubAppName = nullCopy();
	if (pszSubIdList == NULL)
		pszSubIdList = nullCopy();
	this->setWindowTheme.SetText(swtpszSubAppName, pszSubAppName);
	this->setWindowTheme.SetText(swtpszSubIdList, pszSubIdList);
	delete[] pszSubIdList;
	delete[] pszSubAppName;
}

SIZE Common::MinimumSize(Layouter *d)
{
	SIZE ret;
	SIZE sUnicode;		// has an icon; use it for the height
	SIZE sStyles;

	ret.cx = 0;
	ret.cy = 0;

	ret.cx = this->version.MinimumSize(d).cx;
	ret.cx += d->PaddingX();
	sUnicode = this->unicode.MinimumSize(d);
	ret.cx += sUnicode.cx;
	ret.cx += d->PaddingX();
	ret.cx += this->setWindowTheme.MinimumSize(d).cx;
	ret.cy = sUnicode.cy;

	ret.cy += d->PaddingY();
	sStyles = this->styles.MinimumSize(d);
	if (ret.cx < sStyles.cx)
		ret.cx = sStyles.cx;
	ret.cy += sStyles.cy;

	return ret;
}

HDWP Common::Relayout(HDWP dwp, RECT *fill, Layouter *d)
{
	POINT pVersion, pUnicode, pSWT;
	SIZE sVersion, sUnicode, sSWT;
	struct RowYMetrics m;		// of the Unicode one, since it has an icon

	sVersion = this->version.MinimumSize(d);
	sUnicode = this->unicode.MinimumSize(d);
	sSWT = this->setWindowTheme.MinimumSize(d);
	this->unicode.RowYMetrics(&m, d);

	pVersion.x = fill->left;
	pVersion.y = fill->top + m.LabelEditY;
	pSWT.x = fill->right - sSWT.cx;
	pSWT.y = fill->top + m.LabelEditY;
	// center Unicode in the remaining space, not the available space
	pUnicode.x = pVersion.x + sVersion.cx;
	pUnicode.x = pUnicode.x + (pSWT.x - pUnicode.x - sUnicode.cx) / 2;
	pUnicode.y = fill->top;		// TODO what do we add to this to align it properly?

	dwp = this->version.Relayout(dwp,
		pVersion.x, pVersion.y,
		d);
	dwp = this->unicode.Relayout(dwp,
		pUnicode.x, pUnicode.y,
		d);
	dwp = this->setWindowTheme.Relayout(dwp,
		pSWT.x, pSWT.y,
		d);

	dwp = this->styles.RelayoutWidth(dwp,
		fill->left, fill->top + m.TotalHeight + d->PaddingY(),
		fill->right - fill->left,
		d);

	return dwp;
}

#if 0
// TODO move to flags.cpp when the time is right
std::wstring drawTextFlagsString(UINT relevant)
{
	const WCHAR *prefix = L"";
	std::wostringstream ss;

	TRY(DT_CENTER)
	TRY(DT_RIGHT)
	TRY(DT_VCENTER)
	TRY(DT_BOTTOM)
	TRY(DT_WORDBREAK)
	TRY(DT_SINGLELINE)
	TRY(DT_EXPANDTABS)
	TRY(DT_TABSTOP)
	TRY(DT_NOCLIP)
	TRY(DT_EXTERNALLEADING)
	TRY(DT_CALCRECT)
	TRY(DT_NOPREFIX)
	TRY(DT_INTERNAL)
	TRY(DT_EDITCONTROL)
	TRY(DT_PATH_ELLIPSIS)
	TRY(DT_END_ELLIPSIS)
	TRY(DT_MODIFYSTRING)
	TRY(DT_RTLREADING)
	TRY(DT_WORD_ELLIPSIS)
	TRY(DT_NOFULLWIDTHCHARBREAK)
	TRY(DT_HIDEPREFIX)
	TRY(DT_PREFIXONLY)
	if (relevant != 0) {
		ss << prefix << L"0x";
		ss.fill(L'0');
		ss.setf(ss.hex | ss.uppercase, ss.basefield | ss.uppercase);
		ss.width(8);
		ss << relevant;
	}
	return ss.str();
}
#endif
