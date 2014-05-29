// Copyright (C) 2010 Andrey Gruber (aka lamer)

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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#define WIN32_LEAN_AND_MEAN  /* speed up compilations */

#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <wchar.h>

#include "gtoolbar.h"

typedef enum _gtfields {GT_PROP = 0} gtfields;

typedef struct _GTBRPROPS {
	HIMAGELIST			hIml;
	HWND				*pButtons;
	int					count;
	HFONT				hFont;
}GTBRPROPS, *PGTBRPROPS;

typedef struct _BPROPS {
	int					iImage;
	int					iIndex;
	int					id;
	int					state;
	wchar_t				*lpText;
	POINT				ptIcon;
	RECT				rcText;
}BPROPS, *PBPROPS;

#define	GETPROPS(hwnd)			(PGTBRPROPS)(GetWindowLongPtrW(hwnd, GT_PROP))
#define	GETBPROPS(hwnd)			(PBPROPS)(GetWindowLongPtrW(hwnd, GT_PROP))
#define	GTB_CLASS				L"__QINXTEJS_GTB_CLASS_UAREMZPK__"
#define	GTB_BUTTON_CLASS		L"__TSRQBXNA_GTB_BUTTON_CLASS_POIRNALM__"

static BOOL RegisterGTB(HINSTANCE hInstance);
static BOOL RegisterGTBButton(HINSTANCE hInstance);
static LRESULT CALLBACK GTB_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void GTB_OnDestroy(HWND hwnd);
static void GTB_OnPaint(HWND hwnd);
static LRESULT CALLBACK GTButton_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void GTButton_OnDestroy(HWND hwnd);
static void GTButton_OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
static void GTButton_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
static void GTButton_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
static void GTButton_OnPaint(HWND hwnd);
static void AddImageList(HWND hwnd, HIMAGELIST	himl);
static void AddButton(HWND hwnd, HINSTANCE hInstance, LPGTBBUTTON lpgtbb);
static void ClearButtons(HWND hwnd);
static void SelectButton(HWND hwnd, int idButton);
static int GetSelected(HWND hwnd);
static void ButtonClicked(HWND hMain, int idButton);

HWND CreateGTB(HINSTANCE hInstance, HWND hParent, int id, int height){
	HWND		hwnd = NULL;
	RECT		rc;

	if(RegisterGTB(hInstance)){
		GetClientRect(hParent, &rc);
		hwnd = CreateWindowExW(0, GTB_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, rc.right - rc.left, height, hParent, (HMENU)id, hInstance, NULL);
		if(hwnd){
			PGTBRPROPS		pgp = calloc(1, sizeof(GTBRPROPS));
			pgp->hFont = CreateFontW(-MulDiv(8, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Shell Dlg");
			SetWindowLongPtrW(hwnd, GT_PROP, (LONG_PTR)pgp);
		}
	}
	return hwnd;
}

static BOOL RegisterGTB(HINSTANCE hInstance){

	WNDCLASSEXW 	wcx;

	ZeroMemory(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.hInstance = hInstance;
	wcx.lpszClassName = GTB_CLASS;
	wcx.lpfnWndProc = GTB_WndProc;
	wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcx.cbWndExtra = 4;
	if(!RegisterClassExW(&wcx)){
		if(GetLastError() != ERROR_CLASS_ALREADY_EXISTS){
			return FALSE;
		}
	}
	return TRUE;
}

static BOOL RegisterGTBButton(HINSTANCE hInstance){

	WNDCLASSEXW 	wcx;

	ZeroMemory(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.hInstance = hInstance;
	wcx.lpszClassName = GTB_BUTTON_CLASS;
	wcx.lpfnWndProc = GTButton_WndProc;
	wcx.cbWndExtra = 4;
	if(!RegisterClassExW(&wcx)){
		if(GetLastError() != ERROR_CLASS_ALREADY_EXISTS){
			return FALSE;
		}
	}
	return TRUE;
}

static LRESULT CALLBACK GTB_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_DESTROY, GTB_OnDestroy);
		HANDLE_MSG (hwnd, WM_PAINT, GTB_OnPaint);
		case GTBM_SETIMAGELIST:
			AddImageList(hwnd, (HIMAGELIST)lParam);
			return FALSE;
		case GTBM_ADDBUTTON:
			AddButton(hwnd, (HINSTANCE)wParam, (LPGTBBUTTON)lParam);
			return FALSE;
		case GTBM_CLEAR:
			ClearButtons(hwnd);
			return FALSE;
		case GTBM_SELECTBUTTON:
			SelectButton(hwnd, (int)lParam);
			return FALSE;
		case GTBM_GETSELECTED:
			return GetSelected(hwnd);
		default: return DefWindowProc (hwnd, msg, wParam, lParam);
	}
}

static void GTB_OnPaint(HWND hwnd)
{
	PAINTSTRUCT		ps;
	RECT			rc;
	HPEN			hPen, hOldPen;

	BeginPaint(hwnd, &ps);
	GetClientRect(hwnd, &rc);
	hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_ACTIVEBORDER));
	hOldPen = SelectPen(ps.hdc, hPen);
	MoveToEx(ps.hdc, 0, rc.bottom - 1, NULL);
	LineTo(ps.hdc, rc.right, rc.bottom - 1);
	SelectPen(ps.hdc, hOldPen);
	DeletePen(hPen);
	EndPaint(hwnd, &ps);
}

static void GTB_OnDestroy(HWND hwnd)
{
	PGTBRPROPS		pgp = GETPROPS(hwnd);

	if(pgp){
		if(pgp->pButtons)
			free(pgp->pButtons);
		DeleteFont(pgp->hFont);
		pgp->pButtons = 0;
		pgp->count = 0;
		free(pgp);
	}
}

static LRESULT CALLBACK GTButton_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_DESTROY, GTButton_OnDestroy);
		HANDLE_MSG (hwnd, WM_KEYDOWN, GTButton_OnKeyDown);
		HANDLE_MSG (hwnd, WM_LBUTTONUP, GTButton_OnLButtonUp);
		HANDLE_MSG (hwnd, WM_MOUSEMOVE, GTButton_OnMouseMove);
		HANDLE_MSG (hwnd, WM_PAINT, GTButton_OnPaint);
		case WM_GETDLGCODE:
			return DLGC_WANTALLKEYS;
		case WM_MOUSELEAVE:{
			PBPROPS 		pbp = GETBPROPS(hwnd);
			if(pbp && pbp->state == GTBSTATE_HIGHLIGHTED){
				pbp->state = GTBSTATE_NORMAL;
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}
			return FALSE;
		}
		default: return DefWindowProc (hwnd, msg, wParam, lParam);
	}
}

static void GTButton_OnDestroy(HWND hwnd)
{
	PBPROPS 		pbp = GETBPROPS(hwnd);

	if(pbp){
		if(pbp->lpText)
			free(pbp->lpText);
		free(pbp);
	}
}

static void GTButton_OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	PBPROPS 		pbp = GETBPROPS(hwnd), pbtemp;
	PGTBRPROPS		pgp = GETPROPS(GetParent(hwnd));
	int 			bIndex = 0;

	if(pbp && pgp){
		bIndex = pbp->iIndex;
		switch(vk){
			case VK_RIGHT:
			case VK_DOWN:
			case VK_LEFT:
			case VK_UP:
				switch(vk){
					case VK_RIGHT:
					case VK_DOWN:
						if(bIndex < (pgp->count - 1))
							bIndex++;
						else
							bIndex = 0;
						break;
					case VK_LEFT:
					case VK_UP:
						if(bIndex > 0)
							bIndex--;
						else
							bIndex = pgp->count - 1;
						break;
				}
				pbtemp = GETBPROPS(pgp->pButtons[bIndex]);
				pbp->state = GTBSTATE_NORMAL;
				pbtemp->state = GTBSTATE_SELECTED;
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
				RedrawWindow(pgp->pButtons[bIndex], NULL, NULL, RDW_INVALIDATE);
				SetFocus(pgp->pButtons[bIndex]);
				ButtonClicked(GetParent(hwnd), pbtemp->id);
				break;
			case VK_ESCAPE:
				SendMessageW(GetParent(GetParent(hwnd)), WM_COMMAND, MAKEWPARAM(IDCANCEL, GetDlgCtrlID(GetParent(hwnd))), (LPARAM)GetParent(hwnd));
				break;
		}
	}
}

static void GTButton_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	PBPROPS 		pbp = GETBPROPS(hwnd), pbtemp;
	PGTBRPROPS		pgp = GETPROPS(GetParent(hwnd));

	if(pbp && pgp){
		SetFocus(hwnd);
		if(pbp->state != GTBSTATE_SELECTED){
			pbp->state = GTBSTATE_SELECTED;
			for(int i = 0; i < pgp->count; i++){
				if(pgp->pButtons[i] != hwnd){
					pbtemp = GETBPROPS(pgp->pButtons[i]);
					if(pbtemp->state != GTBSTATE_NORMAL){
						pbtemp->state = GTBSTATE_NORMAL;
						RedrawWindow(pgp->pButtons[i], NULL, NULL, RDW_INVALIDATE);
					}
				}
			}
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			ButtonClicked(GetParent(hwnd), pbp->id);
		}
	}
}

static void GTButton_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	PBPROPS 		pbp = GETBPROPS(hwnd);
	TRACKMOUSEEVENT	tme;

	if(pbp){
		if(pbp->state == GTBSTATE_NORMAL){
			pbp->state = GTBSTATE_HIGHLIGHTED;
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = hwnd;
			tme.dwFlags = TME_LEAVE;
			TrackMouseEvent(&tme);
		}
	}
}

static void GTButton_OnPaint(HWND hwnd)
{
	PBPROPS 		pbp = GETBPROPS(hwnd);
	PGTBRPROPS		pgp = GETPROPS(GetParent(hwnd));
	PAINTSTRUCT		ps;
	HDC				hdc;
	RECT			rc;
	HBITMAP			hbmNew, hbmOld;
	HFONT			hf;
	COLORREF		clrOld = 0;

	if(pbp && pgp){
		BeginPaint(hwnd, &ps);

		hdc = CreateCompatibleDC(ps.hdc);
		SetBkMode(hdc, TRANSPARENT);
		GetClientRect(hwnd, &rc);
		hbmNew = CreateCompatibleBitmap(ps.hdc, rc.right - rc.left, rc.bottom - rc.top);
		hbmOld = SelectBitmap(hdc, hbmNew);
		switch(pbp->state){
			case GTBSTATE_NORMAL:
				FillRect(hdc, &rc, GetSysColorBrush(COLOR_WINDOW));
				clrOld = SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
				break;
			case GTBSTATE_HIGHLIGHTED:
				FillRect(hdc, &rc, GetSysColorBrush(COLOR_BTNFACE));
				clrOld = SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
				break;
			case GTBSTATE_SELECTED:
				FillRect(hdc, &rc, GetSysColorBrush(COLOR_HIGHLIGHT));
				clrOld = SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
				break;
		}
		ImageList_Draw(pgp->hIml, pbp->iImage, hdc, pbp->ptIcon.x, pbp->ptIcon.y, ILD_TRANSPARENT);
		hf = SelectFont(hdc, pgp->hFont);
		DrawTextW(hdc, pbp->lpText, -1, &pbp->rcText, DT_SINGLELINE | DT_CENTER);
		BitBlt(ps.hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdc, 0, 0, SRCCOPY);
		SelectBitmap(hdc, hbmOld);
		SelectFont(hdc, hf);
		SetTextColor(hdc, clrOld);
		DeleteDC(hdc);
		DeleteBitmap(hbmNew);
		EndPaint(hwnd, &ps);
	}
}

static void AddImageList(HWND hwnd, HIMAGELIST	himl){
	PGTBRPROPS		pgp = GETPROPS(hwnd);

	if(pgp){
		pgp->hIml = himl;
	}
}

static void AddButton(HWND hwnd, HINSTANCE hInstance, LPGTBBUTTON lpgtbb){
	PGTBRPROPS		pgp = GETPROPS(hwnd);
	PBPROPS			pbp;
	HWND			hButton = NULL;
	HDC				hdc;
	RECT			rc;
	SIZE			sz;
	HFONT			hf;
	int				cx, cy, left, width, height;

	if(pgp && pgp->hIml){
		if(RegisterGTBButton(hInstance)){
			GetClientRect(hwnd, &rc);
			height = rc.bottom - rc.top - 1;
			hdc = GetDC(hwnd);
			hf = SelectFont(hdc, pgp->hFont);
			GetTextExtentPoint32W(hdc, lpgtbb->lpText, wcslen(lpgtbb->lpText), &sz);
			SelectFont(hdc, hf);
			ReleaseDC(hwnd, hdc);
			ImageList_GetIconSize(pgp->hIml, &cx, &cy);
			if(sz.cx > cx)
				width = sz.cx + 12;
			else
				width = cx + 12;
			left = 6;
			if(pgp->pButtons){
				for(int i = 0; i < pgp->count; i++){
					GetClientRect(pgp->pButtons[i], &rc);
					left += (rc.right - rc.left) + 6;
				}
			}
			
			hButton = CreateWindowExW(0, GTB_BUTTON_CLASS, NULL, WS_CHILD | WS_VISIBLE, left, 0, width, height, hwnd, NULL, hInstance, NULL);
			if(pgp->count == 0){
				pgp->pButtons = (HWND *)calloc(1, sizeof(HWND));
				pgp->pButtons[0] = hButton;
			}
			else{
				pgp->pButtons = (HWND *)realloc(pgp->pButtons, (pgp->count + 1) * sizeof(HWND));
				pgp->pButtons[pgp->count] = hButton;
			}
			pgp->count++;
			pbp = (PBPROPS)calloc(1, sizeof(BPROPS));
			pbp->ptIcon.x = (width - cx) / 2;
			pbp->ptIcon.y = 4;
			SetRect(&pbp->rcText, 0, height - 4 - sz.cy, width, height - 4);
			pbp->id = lpgtbb->id;
			pbp->iImage = lpgtbb->index;
			pbp->iIndex = pgp->count - 1;
			pbp->lpText = (wchar_t *)calloc(wcslen(lpgtbb->lpText) + 1, sizeof(wchar_t));
			wcscpy(pbp->lpText, lpgtbb->lpText);
			SetWindowLongPtrW(hButton, GT_PROP, (LONG_PTR)pbp);
		}
	}
}

static void ClearButtons(HWND hwnd){
	PGTBRPROPS		pgp = GETPROPS(hwnd);
			
	if(pgp){
		if(pgp->pButtons){
			for(int i = pgp->count - 1; i >= 0; i--){
				ShowWindow(pgp->pButtons[i], SW_HIDE);
				DestroyWindow(pgp->pButtons[i]);
			}
			free(pgp->pButtons);
			pgp->pButtons = 0;
			pgp->count = 0;
		}
	}
}

static void SelectButton(HWND hwnd, int idButton){
	PGTBRPROPS		pgp = GETPROPS(hwnd);

	if(pgp && pgp->pButtons){
		for(int i = 0; i < pgp->count; i++){
			PBPROPS 		pbp = GETBPROPS(pgp->pButtons[i]);
			if(pbp->id == idButton){
				if(pbp->state != GTBSTATE_SELECTED)
					SendMessageW(pgp->pButtons[i], WM_LBUTTONUP, 0, 0);
				break;
			}
		}
	}
}

static int GetSelected(HWND hwnd){
	PGTBRPROPS		pgp = GETPROPS(hwnd);

	if(pgp && pgp->pButtons){
		for(int i = 0; i < pgp->count; i++){
			PBPROPS 		pbp = GETBPROPS(pgp->pButtons[i]);
			if(pbp->state == GTBSTATE_SELECTED)
				return pbp->id;
		}
	}
	return -1;
}

static void ButtonClicked(HWND hMain, int idButton){
	GTBNMHEADER			gtbnmh;

	gtbnmh.nmh.hwndFrom = hMain;
	gtbnmh.nmh.idFrom = GetDlgCtrlID(hMain);
	gtbnmh.nmh.code = GTBN_BUTTONCLICKED;
	gtbnmh.idButton = idButton;
	SendMessageW(GetParent(hMain), WM_NOTIFY, (WPARAM)gtbnmh.nmh.idFrom, (LPARAM)&gtbnmh);
}
