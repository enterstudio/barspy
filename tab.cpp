// 29 january 2017
#include "barspy.hpp"

Tab::Tab(HWND parent, int id)
{
	this->parent = parent;
	this->id = id;
	this->hwnd = CreateWindowExW(0,
		WC_TABCONTROLW, L"",
		WS_CHILD | WS_VISIBLE | TCS_TOOLTIPS,
		0, 0,
		100, 100,
		this->parent, (HMENU) id, hInstance, NULL);
	if (this->hwnd == NULL)
		panic(L"error creating tab: %I32d", GetLastError());
	SendMessageW(this->hwnd, WM_SETFONT, (WPARAM) hMessageFont, (LPARAM) TRUE);
	this->id++;
}

void Tab::rearrangeZOrder(void)
{
	HWND after;

	after = this->hwnd;
	for (auto hwnd : this->pages) {
		if (SetWindowPos(hwnd, after,
			0, 0,
			0, 0,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE) == 0)
			panic(L"error reordering tab pages: %I32d", GetLastError());
		after = hwnd;
	}
}

// from http://msdn.microsoft.com/en-us/library/windows/desktop/bb226818%28v=vs.85%29.aspx
#define tabMargin 7

INT_PTR CALLBACK Tab::dlgproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Tab *t;

	if (uMsg == WM_INITDIALOG) {
		t = (Tab *) lParam;
		SetWindowLongPtrW(hwnd, DWLP_USER, (LONG_PTR) t);
		return TRUE;
	}
	if (uMsg == WM_WINDOWPOSCHANGED) {
		WINDOWPOS *wp = (WINDOWPOS *) lParam;
		RECT client;
		Layouter *d;
		HDWP dwp;

		if ((wp->flags & SWP_NOSIZE) != 0)
			return FALSE;
		if (GetClientRect(hwnd, &client) == 0)
			panic(L"error getting client rect of tab page for relayout: %I32d", GetLastError());
		t = (Tab *) GetWindowLongPtrW(hwnd, DWLP_USER);
		d = new Layouter(hwnd);
		// TODO move the claculations directly into Layouter
		client.left += d->X(tabMargin);
		client.top += d->Y(tabMargin);
		client.right -= d->X(tabMargin);
		client.bottom -= d->Y(tabMargin);
		dwp = beginDeferWindowPos(64);
		dwp = t->RelayoutChild(dwp, hwnd, &client, d);
		endDeferWindowPos(dwp);
		delete d;
		// pretend the dialog hasn't handled this just in case the system needs to do something special
		return FALSE;
	}
	if (uMsg == WM_CTLCOLORSTATIC) {
		HBRUSH brush;

		t = (Tab *) GetWindowLongPtrW(hwnd, DWLP_USER);
		if (t->OnCtlColorStatic((HDC) wParam, (HWND) lParam, &brush))
			return (INT_PTR) brush;
		return FALSE;
	}
	// TODO WM_PRINTCLIENT unthemed hack?
	return FALSE;
}

HWND Tab::Add(const WCHAR *name)
{
	HWND hwnd;
	TCITEMW item;
	HRESULT hr;

	// unfortunately this needs to be a proper dialog for EnableThemeDialogTexture() to work; CreateWindowExW() won't suffice
	hwnd = CreateDialogParamW(hInstance, MAKEINTRESOURCE(tabPageDialogTemplate),
		this->parent, Tab::dlgproc, (LPARAM) this);
	if (hwnd == NULL)
		panic(L"error creating tab page: %I32d", GetLastError());
	hr = EnableThemeDialogTexture(hwnd, ETDT_ENABLE | ETDT_USETABTEXTURE | ETDT_ENABLETAB);
	if (hr != S_OK)
		panic(L"error setting tab page background: 0x%08I32X", hr);
	this->pages.push_back(hwnd);
	this->rearrangeZOrder();

	ZeroMemory(&item, sizeof (TCITEMW));
	item.mask = TCIF_TEXT;
	item.pszText = (WCHAR *) name;
	if (SendMessageW(this->hwnd, TCM_INSERTITEMW, (WPARAM) (this->pages.size()), (LPARAM) (&item)) == (LRESULT) -1)
		panic(L"error adding tab page: %I32d", GetLastError());

	// the first page has to be explicitly shown because it does not send a notification
	if (this->pages.size() == 0) {
		Layouter *d;

		ShowWindow(hwnd, SW_SHOW);
		// and resize it
		d = new Layouter(this->hwnd);
		// no need for a HDWP; only one thing is being moved in this first iteration
		this->relayoutCurrentPage(NULL, NULL, d);
		delete d;
	} else
		// otherwise we have to hide the newly created dialog
		ShowWindow(hwnd, SW_HIDE);
	return hwnd;
}

void Tab::Show(int cmd)
{
	HWND page;

	ShowWindow(this->hwnd, cmd);
	page = this->curpage();
	if (page != NULL)
		ShowWindow(page, cmd);
}

LRESULT Tab::curpagenum(void)
{
	return SendMessageW(this->hwnd, TCM_GETCURSEL, 0, 0);
}

HWND Tab::curpage(void)
{
	LRESULT n;

	n = this->curpagenum();
	if (n == (LRESULT) (-1))
		return NULL;
	return this->pages[n];
}

bool Tab::HandleNotify(NMHDR *nm, LRESULT *lResult)
{
	HWND page;
	int cmd;

	switch (nm->code) {
	case TCN_SELCHANGING:
		cmd = SW_HIDE;
		*lResult = FALSE;
		break;
	case TCN_SELCHANGE:
		cmd = SW_SHOW;
		*lResult = 0;
		break;
	default:
		return false;
	}
	page = this->curpage();
	if (page == NULL)
		return false;
	ShowWindow(page, cmd);
	if (cmd == SW_SHOW) {
		Layouter *d;

		// we only resize the current page, so we have to resize the new current page once the current page changes
		d = new Layouter(this->hwnd);
		// no need for a HDWP; only one thing is being moved in this first iteration
		this->relayoutCurrentPage(NULL, NULL, d);
		delete d;
	}
	return true;
}

HDWP Tab::relayoutCurrentPage(HDWP dwp, RECT *fill, Layouter *d)
{
	HWND page;
	RECT r;

	page = this->curpage();
	if (page == NULL)
		return dwp;
	if (fill != NULL) {
		r = *fill;
		// we need screen coordinates for TCM_ADJUSTRECT
		// TODO error check
		MapWindowRect(this->parent, NULL, &r);
	} else
		if (GetWindowRect(this->hwnd, &r) == 0)
			panic(L"error getting tab window rect for page relayout: %I32d", GetLastError());
	SendMessageW(this->hwnd, TCM_ADJUSTRECT, (WPARAM) FALSE, (LPARAM) (&r));
	// we need parent coordinates for deferWindowPos()
	// TODO error check
	MapWindowRect(NULL, this->parent, &r);
	dwp = deferWindowPos(dwp, page,
		r.left, r.top,
		r.right - r.left, r.bottom - r.top,
		0);
	// the dialog procedure above will relayout the page contents
	return dwp;
}

HDWP Tab::Relayout(HDWP dwp, RECT *fill, Layouter *d)
{
	dwp = deferWindowPos(dwp, this->hwnd,
		fill->left, fill->top,
		fill->right - fill->left, fill->bottom - fill->top,
		0);
	dwp = this->relayoutCurrentPage(dwp, fill, d);
	return dwp;
}
