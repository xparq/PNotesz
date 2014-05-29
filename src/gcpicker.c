// Copyright (C) 2008 Andrey Gruber (aka lamer)

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "gcpicker.h"


#define	GCPICKER_CLASS_W			L"GCPICKER_CLASS_Wnd"
#define	GCP_ID_W					L"GCP_ID"

#define	GetButtonW(hwnd)			(HWND)GetWindowLongPtrW(hwnd, O_CMDHWND)
#define	GetOwnerW(hwnd)				(HWND)GetWindowLongPtrW(hwnd, O_PHWND)
#define	GetColorsW(hwnd)			(HWND)GetWindowLongPtrW(hwnd, O_CLRWND)
#define	GetCurrentColor(hwnd)		(COLORREF)GetWindowLongPtrW(hwnd, O_CCOLOR)

typedef enum {O_CCOLOR = 0, O_PHWND = 4, O_CMDHWND = 8, O_CLRWND = 12};

static BOOL RegisterGCPickerWindowW(void);
static LRESULT CALLBACK GCPicker_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void GCPicker_OnPaint(HWND hwnd);
static BOOL GCPicker_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
static void GCPicker_OnDestroy(HWND hwnd);
static void GCPicker_OnSetFocus(HWND hwnd, HWND hwndOldFocus);
static void GCPicker_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void GCPicker_OnSetFont(HWND hwndCtl, HFONT hfont, BOOL fRedraw);
static void GCPicker_OnEnable(HWND hwnd, BOOL fEnable);
static void GCPicker_OnLButtonDblClk(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);

HWND CreateGCPickerWindowW(int x, int y, int cx, int cy, HWND hParentWnd, int id, COLORREF crColor){
	HWND			hwnd, hColors;
	RECT			rc;

	if(!RegisterGCPickerWindowW()){
		return NULL;
	}
	hwnd = CreateWindowExW(0, GCPICKER_CLASS_W, NULL, WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, x, y, cx, cy, hParentWnd, (HMENU)id, GetModuleHandleW(0), 0);
	if(!hwnd)
		return NULL;
	SetPropW(hwnd, GCP_ID_W, (HANDLE)id);

	SetWindowLongPtrW(hwnd, O_CCOLOR, (LONG_PTR)crColor);
	SetWindowLongPtrW(hwnd, O_PHWND, (LONG_PTR)hParentWnd);
	
	GetWindowRect(hwnd, &rc);
	hColors = CreateGColorsWindowW(rc.left, rc.bottom, hwnd, crColor);
	SetWindowLongPtrW(hwnd, O_CLRWND, (LONG_PTR)hColors);

	return hwnd;
}

static BOOL RegisterGCPickerWindowW(void){

    WNDCLASSEXW 			wcx;

	ZeroMemory(&wcx, sizeof(wcx));
    wcx.cbSize = sizeof(wcx);
	wcx.cbWndExtra = 16;
    wcx.hInstance = GetModuleHandleW(0);
	wcx.lpfnWndProc = GCPicker_WndProc;
    wcx.lpszClassName = GCPICKER_CLASS_W;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    if (!RegisterClassExW(&wcx)){
		if(GetLastError() == ERROR_CLASS_ALREADY_EXISTS)
			return TRUE;
		return FALSE;
	}
	return TRUE;
}

static LRESULT CALLBACK GCPicker_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_PAINT, GCPicker_OnPaint);
	HANDLE_MSG (hwnd, WM_CREATE, GCPicker_OnCreate);
	HANDLE_MSG (hwnd, WM_DESTROY, GCPicker_OnDestroy);
	HANDLE_MSG (hwnd, WM_SETFOCUS, GCPicker_OnSetFocus);
	HANDLE_MSG (hwnd, WM_COMMAND, GCPicker_OnCommand);
	HANDLE_MSG (hwnd, WM_SETFONT, GCPicker_OnSetFont);
	HANDLE_MSG (hwnd, WM_ENABLE, GCPicker_OnEnable);
	HANDLE_MSG (hwnd, WM_LBUTTONDBLCLK, GCPicker_OnLButtonDblClk);

	case GCM_NEW_COLOR:{
		HWND	hColors = GetColorsW(hwnd);
		SetWindowLongPtrW(hwnd, O_CCOLOR, (COLORREF)lParam);
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
		SendMessageW(hColors, GCM_NEW_COLOR, 0, lParam);
		return TRUE;
	}
	case GCN_COLOR_SELECTED:{
		COLORREF		crColor = (COLORREF)lParam;

		SetWindowLongPtrW(hwnd, O_CCOLOR, crColor);
		PostMessageW(GetOwnerW(hwnd), GCN_COLOR_SELECTED, (WPARAM)GetPropW(hwnd, GCP_ID_W), (LPARAM)crColor);
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
		return TRUE;
	}
	case GCM_CLOSE:
		SendMessageW(GetColorsW(hwnd), GCM_CLOSE, 0, 0);
		return TRUE;
	default: return DefWindowProc (hwnd, msg, wParam, lParam);
	}
}

static void GCPicker_OnEnable(HWND hwnd, BOOL fEnable)
{
	EnableWindow(GetButtonW(hwnd), fEnable);
	InvalidateRect(hwnd, NULL, FALSE);
}

static void GCPicker_OnSetFont(HWND hwndCtl, HFONT hfont, BOOL fRedraw)
{
	SendMessageW(GetButtonW(hwndCtl), WM_SETFONT, (WPARAM)hfont, TRUE);
}

static void GCPicker_OnLButtonDblClk(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	RECT			rc;
	COLORREF		crColor;

	GetWindowRect(hwnd, &rc);
	crColor = GetCurrentColor(hwnd);
	ShowColorsWindow(rc.left, rc.bottom, GetColorsW(hwnd), crColor);
}

static void GCPicker_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	RECT			rc;
	COLORREF		crColor;

	GetWindowRect(hwnd, &rc);
	if(hwndCtl == GetButtonW(hwnd)){
		crColor = GetCurrentColor(hwnd);
		ShowColorsWindow(rc.left, rc.bottom, GetColorsW(hwnd), crColor);
	}
}

static void GCPicker_OnSetFocus(HWND hwnd, HWND hwndOldFocus)
{
	SetFocus(GetButtonW(hwnd));
}

static BOOL GCPicker_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	RECT			rc;
	HWND			hCmd;
	HRGN			hRgn;

	GetClientRect(hwnd, &rc);
	hRgn = CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 3, 3);
	SetWindowRgn(hwnd, hRgn, TRUE);
	hCmd = CreateWindowExW(0, L"BUTTON", L"...", BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, rc.right - (rc.bottom - rc.top - 2), rc.top + 2, rc.bottom - rc.top - 4, rc.bottom - rc.top - 4, hwnd, NULL, GetModuleHandleW(0), 0);
	SetWindowLongPtrW(hwnd, O_CMDHWND, (LONG_PTR)hCmd);
	return TRUE;
}

static void GCPicker_OnDestroy(HWND hwnd)
{
	RemovePropW(hwnd, GCP_ID_W);
}

static void GCPicker_OnPaint(HWND hwnd)
{
	PAINTSTRUCT		ps;
	RECT			rc;
	COLORREF		crColor;
	HBRUSH			hBrush = NULL;
	int				state;
	HPEN			hPen;

	BeginPaint(hwnd, &ps);
	state = SaveDC(ps.hdc);
	GetWindowRect(hwnd, &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	crColor = (COLORREF)GetWindowLongPtrW(hwnd, O_CCOLOR);
	hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	SelectPen(ps.hdc, hPen);
	SelectBrush(ps.hdc, GetSysColorBrush(COLOR_BTNFACE));
	RoundRect(ps.hdc, rc.left, rc.top, rc.right - 1, rc.bottom - 1, 3, 3);
	rc.left += 2;
	rc.top += 2;
	rc.right -=3;
	rc.bottom -=3;
	if(IsWindowEnabled(hwnd)){
		hBrush = CreateSolidBrush(crColor);
		FillRect(ps.hdc, &rc, hBrush);
		DeleteBrush(hBrush);
	}
	else{
		FillRect(ps.hdc, &rc, GetSysColorBrush(COLOR_BTNFACE));
	}
	RestoreDC(ps.hdc, state);
	EndPaint(hwnd, &ps);
}
