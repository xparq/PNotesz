// Copyright (C) 2009 Andrey Gruber (aka lamer)

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

#define WIN32_LEAN_AND_MEAN  /* speed up compilations */

#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <wchar.h>

#include "gsearchbox.h"

#define	GCLICK_SEARCH_CLASS			L"__EICMSLWJ_GCLICK_SEARCH_CLASS_PDNTOENY__"
#define	GSEARCH_BOX_CLASS			L"__WIFURJDS_GSEARCH_BOX_CLASS_QJCGBJEH__"

#define	CLICK_ICON					L"CLICK_ICON"
#define	CLICK_ICON_DIS				L"CLICK_ICON_DIS"
#define	IN_SEARCH					L"IN_SEARCH"
#define	FONT_ITALIC					L"FONT_ITALIC"
#define	FONT_NORMAL					L"FONT_NORMAL"
#define	HWND_CLICK					L"HWND_CLICK"
#define	HWND_EDIT					L"HWND_EDIT"
#define	DEF_PROMPT					L"DEF_PROMPT"

static LRESULT CALLBACK Click_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Click_OnPaint(HWND hwnd);
static void Click_OnDestroy(HWND hwnd);
static void Click_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
static LRESULT CALLBACK GSearch_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void GSearch_OnDestroy(HWND hwnd);
static HBRUSH GSearch_OnCtlColorEdit(HWND hwnd, HDC hdc, HWND hwndChild, int type);
static HBRUSH GSearch_OnCtlColorStatic(HWND hwnd, HDC hdc, HWND hwndChild, int type);
LRESULT CALLBACK EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static BOOL RegisterGSearchClasses(HINSTANCE hInstance);
static BOOL CALLBACK PropEnumProc(HWND hwnd, LPCWSTR lpszString, HANDLE hData);

HWND CreateGSearchBoxWindow(HINSTANCE hInstance, HWND hParent, HICON hIcon, HICON hIconDisabled, wchar_t * lpPrompt, int left, int top, int width, int height){
	HWND		hGSearch, hClick, hEdit;
	HFONT		hFont;

	if(!RegisterGSearchClasses(hInstance))
		return NULL;
	hGSearch = CreateWindowExW(0, GSEARCH_BOX_CLASS, NULL, WS_VISIBLE | WS_CHILD, left, top, width, height, hParent, NULL, hInstance, NULL);
	if(hGSearch){
		SetPropW(hGSearch, IN_SEARCH, FALSE);
		hClick = CreateWindowExW(0, GCLICK_SEARCH_CLASS, NULL, WS_CHILD | WS_VISIBLE, width - 16 - 1, (height - 16) / 2, 16, 16, hGSearch, NULL, hInstance, NULL);
		SetPropW(hClick, CLICK_ICON, (HANDLE)hIcon);
		SetPropW(hClick, CLICK_ICON_DIS, (HANDLE)hIconDisabled);
		hEdit = CreateWindowExW(0, L"EDIT", lpPrompt, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_WANTRETURN, 0, 0, width - 16 - 1, height, hGSearch, NULL, hInstance, NULL);
		SetWindowLongPtrW(hEdit, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(hEdit, GWLP_WNDPROC, (LONG_PTR)EditProc));
		SetWindowPos(hEdit, 0, 0, 0, width - 16 - 1, height, SWP_FRAMECHANGED);
		hFont = CreateFontW(-MulDiv(8, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 1, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Shell Dlg");
		SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
		SetPropW(hGSearch, FONT_ITALIC, (HANDLE)hFont);
		hFont = CreateFontW(-MulDiv(8, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Shell Dlg");
		SetPropW(hGSearch, FONT_NORMAL, (HANDLE)hFont);
		SetPropW(hGSearch, HWND_CLICK, (HANDLE)hClick);
		SetPropW(hGSearch, HWND_EDIT, (HANDLE)hEdit);
		wchar_t *	pPrompt = calloc(wcslen(lpPrompt) + 1, sizeof(wchar_t));
		wcscpy(pPrompt, lpPrompt);
		SetPropW(hGSearch, DEF_PROMPT, (HANDLE)pPrompt);
	}
	return hGSearch;
}

static BOOL RegisterGSearchClasses(HINSTANCE hInstance){

	WNDCLASSEXW 	wcx;

	ZeroMemory(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.hInstance = hInstance;
	wcx.lpszClassName = GSEARCH_BOX_CLASS;
	wcx.lpfnWndProc = GSearch_WndProc;
	wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	if(!RegisterClassExW(&wcx)){
		if(GetLastError() != ERROR_CLASS_ALREADY_EXISTS){
			return FALSE;
		}
	}
	wcx.lpszClassName = GCLICK_SEARCH_CLASS;
	wcx.lpfnWndProc = Click_WndProc;
	wcx.hCursor = LoadCursor(NULL, IDC_HAND);
	if(!RegisterClassExW(&wcx)){
		if(GetLastError() != ERROR_CLASS_ALREADY_EXISTS){
			return FALSE;
		}
	}
	return TRUE;
}

static LRESULT CALLBACK GSearch_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		HANDLE_MSG (hwnd, WM_DESTROY, GSearch_OnDestroy);
		HANDLE_MSG (hwnd, WM_CTLCOLOREDIT, GSearch_OnCtlColorEdit);
		HANDLE_MSG (hwnd, WM_CTLCOLORSTATIC, GSearch_OnCtlColorStatic);

		case WM_ENABLE:
			if(!wParam){
				HWND			hEdit = (HWND)GetPropW(hwnd, HWND_EDIT);
				GUITHREADINFO	gti;
				gti.cbSize = sizeof(gti);
				GetGUIThreadInfo(0, &gti);
				if(gti.hwndCaret == hEdit)
					SendMessageW(hEdit, WM_KILLFOCUS, 0, 0);
			}
			RedrawWindow((HWND)GetPropW(hwnd, HWND_CLICK), NULL, NULL, RDW_INVALIDATE);
			return DefWindowProc (hwnd, msg, wParam, lParam);
		case GSBM_GET_STATUS:
			if(GetPropW(hwnd, IN_SEARCH))
				return TRUE;
			else
				return FALSE;
		case GSBM_SET_DEF_PROMT:{
			wchar_t * pPrompt = (wchar_t *)GetPropW(hwnd, DEF_PROMPT);
			if(pPrompt)
				free(pPrompt);
			int		length = wcslen((wchar_t *)lParam);
			pPrompt = calloc(length + 1, sizeof(wchar_t));
			wcscpy(pPrompt, (wchar_t *)lParam);
			SetPropW(hwnd, DEF_PROMPT, (HANDLE)pPrompt);
			if(!GetPropW(hwnd, IN_SEARCH))
				SetWindowTextW((HWND)GetPropW(hwnd, HWND_EDIT), pPrompt);
			return FALSE;
		}
		case GSBM_CLEAR:{
			HWND		hEdit = (HWND)GetPropW(hwnd, HWND_EDIT);
			NMHDR		nmh;

			SendMessageW(hEdit, WM_SETFONT, (WPARAM)(HFONT)GetPropW(hwnd, FONT_ITALIC), TRUE);
			SetWindowTextW(hEdit, (wchar_t *)GetPropW(hwnd, DEF_PROMPT));
			SetPropW(hwnd, IN_SEARCH, (HANDLE)FALSE);
			SetFocus((HWND)GetPropW(hwnd, HWND_CLICK));
			nmh.hwndFrom = hwnd;
			nmh.code = GSBNC_CLEAR;
			nmh.idFrom = 0;
			SendMessageW(GetParent(hwnd), WM_NOTIFY, (WPARAM)hwnd, (LPARAM)&nmh);
			return FALSE;
		}
		default: return DefWindowProc (hwnd, msg, wParam, lParam);
	}
}

static HBRUSH GSearch_OnCtlColorStatic(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
	return GetSysColorBrush(COLOR_WINDOW);
}

static HBRUSH GSearch_OnCtlColorEdit(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
	if(GetPropW(hwnd, IN_SEARCH))
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
	else
		SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
	return GetSysColorBrush(COLOR_WINDOW);
}

static void GSearch_OnDestroy(HWND hwnd)
{
	DeleteFont((HFONT)GetPropW(hwnd, FONT_NORMAL));
	DeleteFont((HFONT)GetPropW(hwnd, FONT_ITALIC));
	wchar_t * pPrompt = (wchar_t *)GetPropW(hwnd, DEF_PROMPT);
	if(pPrompt)
		free(pPrompt);
	EnumPropsW(hwnd, PropEnumProc);
}

static LRESULT CALLBACK Click_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		HANDLE_MSG (hwnd, WM_PAINT, Click_OnPaint);
		HANDLE_MSG (hwnd, WM_DESTROY, Click_OnDestroy);
		HANDLE_MSG (hwnd, WM_LBUTTONUP, Click_OnLButtonUp);

		default: return DefWindowProc (hwnd, msg, wParam, lParam);
	}
}

static void Click_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	HWND		hParent = GetParent(hwnd);
	HWND		hEdit = (HWND)GetPropW(hParent, HWND_EDIT);
	int			length;

	if(GetPropW(hParent, IN_SEARCH)){
		length = GetWindowTextLengthW(hEdit);
		if(length == 0){
			SetPropW(hParent, IN_SEARCH, (HANDLE)FALSE);
			SendMessageW(hEdit, WM_SETFONT, (WPARAM)(HFONT)GetPropW(hParent, FONT_ITALIC), TRUE);
			SetWindowTextW(hEdit, (wchar_t *)GetPropW(hParent, DEF_PROMPT));
			SetFocus(hwnd);
		}
		else{
			GSBNTF		nf;
			wchar_t		szBuffer[256];

			nf.nmh.hwndFrom = hParent;
			nf.nmh.code = GSBNC_STRING;
			nf.nmh.idFrom = 0;
			GetWindowTextW(hEdit, szBuffer, 256);
			nf.lpString = szBuffer;
			SendMessageW(GetParent(hParent), WM_NOTIFY, (WPARAM)hParent, (LPARAM)&nf);
		}
	}
}

static void Click_OnDestroy(HWND hwnd)
{
	EnumPropsW(hwnd, PropEnumProc);
}

static void Click_OnPaint(HWND hwnd)
{
	PAINTSTRUCT		ps;
	RECT			rc;
	HDC				hdc;
	HBITMAP			hbmNew, hbmOld;

	BeginPaint(hwnd, &ps);
	hdc = CreateCompatibleDC(ps.hdc);
	hbmNew = CreateCompatibleBitmap(ps.hdc, 16, 16);
	GetClientRect(hwnd, &rc);
	hbmOld = SelectBitmap(hdc, hbmNew);
	FillRect(hdc, &rc, GetSysColorBrush(COLOR_WINDOW));
	if(IsWindowEnabled(GetParent(hwnd)))
		DrawIconEx(hdc, 0, 0, (HICON)GetPropW(hwnd, CLICK_ICON), 16, 16, 0, NULL, DI_NORMAL);
	else
		DrawIconEx(hdc, 0, 0, (HICON)GetPropW(hwnd, CLICK_ICON_DIS), 16, 16, 0, NULL, DI_NORMAL);
	BitBlt(ps.hdc, 0, 0, 16, 16, hdc, 0, 0, SRCCOPY);
	SelectBitmap(hdc, hbmOld);
	DeleteDC(hdc);
	DeleteBitmap(hbmNew);
	EndPaint(hwnd, &ps);
}

LRESULT CALLBACK EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	HWND		hParent = GetParent(hwnd);
	int			length;

	switch(uMsg){
		case WM_NCCALCSIZE:{
			HDC		hdc;
			RECT	rcClient, rcWnd, rcText;
			HFONT	hFontOld;
			UINT	uCHeight, uOffset;

			if(!wParam)
				return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
			hdc = GetDC(hwnd);
			if(!GetPropW(hParent, IN_SEARCH))
				hFontOld = SelectFont(hdc, (HFONT)GetPropW(hParent, FONT_ITALIC));
			else
				hFontOld = SelectFont(hdc, (HFONT)GetPropW(hParent, FONT_NORMAL));
			SetRectEmpty(&rcText);
			DrawTextW(hdc, L"WWW", -1, &rcText, DT_CALCRECT | DT_LEFT);
			uCHeight = rcText.bottom - rcText.top;
			SelectFont(hdc, hFontOld);
			ReleaseDC(hwnd, hdc);
			GetWindowRect(hwnd, &rcWnd);
			GetClientRect(hwnd, &rcClient);
			MapWindowPoints(hwnd, HWND_DESKTOP, (LPPOINT)&rcClient, 2);
			uOffset = rcClient.bottom - rcClient.top;
			uOffset -= uCHeight;
			uOffset >>= 1;
			NCCALCSIZE_PARAMS * pncp = (NCCALCSIZE_PARAMS *)lParam;
			pncp->rgrc[0].top += uOffset;
			pncp->rgrc[0].bottom -= uOffset;
			return WVR_REDRAW;
		}
		case WM_LBUTTONDOWN:
			if(!GetPropW(hParent, IN_SEARCH)){
				SetWindowTextW(hwnd, NULL);
				SendMessageW(hwnd, WM_SETFONT, (WPARAM)(HFONT)GetPropW(hParent, FONT_NORMAL), TRUE);
				SetPropW(hParent, IN_SEARCH, (HANDLE)TRUE);
				SetFocus(hwnd);
				return FALSE;
			}
			else
				return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
		case WM_KILLFOCUS:
			if(GetPropW(hParent, IN_SEARCH)){
				length = GetWindowTextLengthW(hwnd);
				if(length == 0){
					SetPropW(hParent, IN_SEARCH, (HANDLE)FALSE);
					SendMessageW(hwnd, WM_SETFONT, (WPARAM)(HFONT)GetPropW(hParent, FONT_ITALIC), TRUE);
					SetWindowTextW(hwnd, (wchar_t *)GetPropW(hParent, DEF_PROMPT));
				}
				return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
			}
			else
				return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
		case WM_GETDLGCODE:
			return DLGC_WANTALLKEYS;
		case WM_KEYUP:
			if(GetWindowTextLengthW(hwnd) == 0){
				SendMessageW(hParent, GSBM_CLEAR, 0, 0);
			}
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
		case WM_KEYDOWN:{
			if(wParam == VK_RETURN){
				if(GetPropW(hParent, IN_SEARCH)){
					length = GetWindowTextLengthW(hwnd);
					if(length == 0){
						return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
					}
					else{
						GSBNTF		nf;
						wchar_t		szBuffer[256];

						nf.nmh.hwndFrom = hParent;
						nf.nmh.code = GSBNC_STRING;
						nf.nmh.idFrom = 0;
						GetWindowTextW(hwnd, szBuffer, 256);
						nf.lpString = szBuffer;
						SendMessageW(GetParent(hParent), WM_NOTIFY, (WPARAM)hParent, (LPARAM)&nf);
					}
				}
				else
					return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
			}
			else
				return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
		}
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
	}
}

static BOOL CALLBACK PropEnumProc(HWND hwnd, LPCWSTR lpszString, HANDLE hData){
	if((DWORD)(VOID *)hData != 0xffffffff){
		RemovePropW(hwnd, lpszString);
	}
	return TRUE;
}
