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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <windows.h>

#include "global.h"
#include "numericconstants.h"
#include "shared.h"
#include "enums.h"
#include "skin.h"
#include "notes.h"
#include "docking.h"

#define	_AG_DOCK_ARROW_CLASS		L"_ag_dock_arrow_class"

typedef enum _TrayPosition {
	TRP_LEFT,
	TRP_TOP,
	TRP_RIGHT,
	TRP_BOTTOM
}TrayPosition;

static LRESULT CALLBACK DArrow_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void DArrow_OnDestroy(HWND hwnd);
static void DArrow_OnPaint(HWND hwnd);
static void DArrow_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
static BOOL RegisterDArrow(HINSTANCE hInstance);
static void MoveArrow(HWND hwnd, int pos, HBITMAP hbmp);
static void ShiftNotesOnArrowClick(HWND hwnd);
static TrayPosition GetTrayPosition(void);

HWND CreateDArrow(HINSTANCE hInstance, HWND hParent, int pos){
	HWND		hDArrow = NULL;
	HBITMAP		hbmp = NULL;
	HMODULE		hIcons;

	hDArrow = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED, _AG_DOCK_ARROW_CLASS, NULL, WS_POPUP, 0, 0, 48, 48, hParent, NULL, hInstance, NULL);
	if(hDArrow == NULL){
		if(GetLastError() == ERROR_CANNOT_FIND_WND_CLASS){
			if(!RegisterDArrow(hInstance)){
				return NULL;
			}
			hDArrow = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED, _AG_DOCK_ARROW_CLASS, NULL, WS_POPUP, 0, 0, 48, 48, hParent, NULL, hInstance, NULL);
		}
		else{
			return NULL;
		}
	}
	
	if(hDArrow){
		hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
		SetWindowLongPtrW(hDArrow, GWLP_USERDATA, pos);
		switch(pos){
			case DA_LEFT_UP:
			case DA_RIGHT_UP:
				hbmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_ARROW_TOP));
				break;
			case DA_LEFT_DOWN:
			case DA_RIGHT_DOWN:
				hbmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_ARROW_BOTTOM));
				break;
			case DA_TOP_LEFT:
			case DA_BOTTOM_LEFT:
				hbmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_ARROW_LEFT));
				break;
			case DA_TOP_RIGHT:
			case DA_BOTTOM_RIGHT:
				hbmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_ARROW_RIGHT));
				break;
		}
		if(hIcons)
			FreeLibrary(hIcons);
		SetWindowLongPtrW(hDArrow, 0, (LONG_PTR)hbmp);
		SetLayeredWindowAttributes(hDArrow, CLR_MASK, 160, LWA_ALPHA);
		SetWindowRgn(hDArrow, RegionFromSkin(hbmp, CLR_MASK), TRUE);
		MoveArrow(hDArrow, pos, hbmp);
		ShowWindow(hDArrow, SW_HIDE);
	}
	return hDArrow;
}

static TrayPosition GetTrayPosition(void){
	HWND			hTray = NULL;
	RECT			rcTray;

	hTray = FindWindow("Shell_TrayWnd", NULL);
	GetWindowRect(hTray, &rcTray);
	if(rcTray.left == 0 && rcTray.top == 0){
		if((rcTray.right - rcTray.left) > (rcTray.bottom - rcTray.top))
			return TRP_TOP;
		else
			return TRP_LEFT;
	}
	else if(rcTray.left == 0 && rcTray.top != 0)
		return TRP_BOTTOM;
	else
		return TRP_RIGHT;
}

static void MoveArrow(HWND hwnd, int pos, HBITMAP hbmp){
	RECT			rcDesktop, rcWorkArea;
	int				sx = 0, sy = 0, mcount;
	BITMAP			bm;
	TrayPosition	tp;

	tp = GetTrayPosition();
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	mcount = GetSystemMetrics(SM_CMONITORS);
	if(mcount == 1){
		GetClientRect(GetDesktopWindow(), &rcDesktop);
		sx = rcDesktop.right - rcDesktop.left;
		sy = rcDesktop.bottom - rcDesktop.top;
	}
	else{
		sx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		sy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}
	GetObject(hbmp, sizeof(bm), &bm);

	switch(pos){
		case DA_LEFT_UP:
			switch(tp){
				case TRP_BOTTOM:
				case TRP_RIGHT:
					SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_TOP:
					SetWindowPos(hwnd, HWND_TOPMOST, 0, rcWorkArea.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_LEFT:
					SetWindowPos(hwnd, HWND_TOPMOST, rcWorkArea.left, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
			}
			break;
		case DA_LEFT_DOWN:
			switch(tp){
				case TRP_TOP:
				case TRP_RIGHT:
					SetWindowPos(hwnd, HWND_TOPMOST, 0, sy - bm.bmHeight, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_LEFT:
					SetWindowPos(hwnd, HWND_TOPMOST, rcWorkArea.left, sy - bm.bmHeight, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_BOTTOM:
					SetWindowPos(hwnd, HWND_TOPMOST, 0, rcWorkArea.bottom - bm.bmHeight, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
			}
			break;
		case DA_TOP_LEFT:
			switch(tp){
				case TRP_BOTTOM:
				case TRP_RIGHT:
					SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_TOP:
					SetWindowPos(hwnd, HWND_TOPMOST, 0, rcWorkArea.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_LEFT:
					SetWindowPos(hwnd, HWND_TOPMOST, rcWorkArea.left, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
			}
			break;
		case DA_TOP_RIGHT:
			switch(tp){
				case TRP_BOTTOM:
				case TRP_LEFT:
					SetWindowPos(hwnd, HWND_TOPMOST, sx - bm.bmWidth, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_TOP:
					SetWindowPos(hwnd, HWND_TOPMOST, sx - bm.bmWidth, rcWorkArea.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_RIGHT:
					SetWindowPos(hwnd, HWND_TOPMOST, rcWorkArea.right - bm.bmWidth, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
			}
			break;
		case DA_BOTTOM_LEFT:
			switch(tp){
				case TRP_TOP:
				case TRP_RIGHT:
				case TRP_BOTTOM:
					SetWindowPos(hwnd, HWND_TOPMOST, 0, rcWorkArea.bottom - bm.bmHeight, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_LEFT:
					SetWindowPos(hwnd, HWND_TOPMOST, rcWorkArea.left, rcWorkArea.bottom - bm.bmHeight, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
			}
			break;
		case DA_BOTTOM_RIGHT:
			switch(tp){
				case TRP_TOP:
				case TRP_LEFT:
				case TRP_BOTTOM:
					SetWindowPos(hwnd, HWND_TOPMOST, sx - bm.bmWidth, rcWorkArea.bottom - bm.bmHeight, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_RIGHT:
					SetWindowPos(hwnd, HWND_TOPMOST, rcWorkArea.right - bm.bmWidth, rcWorkArea.bottom - bm.bmHeight, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
			}
			break;
		case DA_RIGHT_UP:
			switch(tp){
				case TRP_LEFT:
				case TRP_BOTTOM:
					SetWindowPos(hwnd, HWND_TOPMOST, sx - bm.bmWidth, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_TOP:
					SetWindowPos(hwnd, HWND_TOPMOST, sx - bm.bmWidth, rcWorkArea.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_RIGHT:
					SetWindowPos(hwnd, HWND_TOPMOST, rcWorkArea.right - bm.bmWidth, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
			}
			break;
		case DA_RIGHT_DOWN:
			switch(tp){
				case TRP_TOP:
				case TRP_LEFT:
					SetWindowPos(hwnd, HWND_TOPMOST, sx - bm.bmWidth, sy - bm.bmHeight, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_RIGHT:
					SetWindowPos(hwnd, HWND_TOPMOST, rcWorkArea.right - bm.bmWidth, sy - bm.bmHeight, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
				case TRP_BOTTOM:
					SetWindowPos(hwnd, HWND_TOPMOST, sx - bm.bmWidth, rcWorkArea.bottom - bm.bmHeight, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
					break;
			}
			break;
	}
}

static BOOL RegisterDArrow(HINSTANCE hInstance){
	WNDCLASSEXW		wcx;

	ZeroMemory(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.hInstance = hInstance;
	wcx.cbWndExtra = 4;
	wcx.lpszClassName = _AG_DOCK_ARROW_CLASS;
	wcx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcx.lpfnWndProc = DArrow_WndProc;
	wcx.hCursor = LoadCursor(NULL, IDC_HAND);
	if(RegisterClassExW(&wcx)){
		return TRUE;
	}
	else{
		if(GetLastError() == ERROR_CLASS_ALREADY_EXISTS){
			return TRUE;
		}
		return FALSE;
	}
}

static LRESULT CALLBACK DArrow_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_DESTROY, DArrow_OnDestroy);
	HANDLE_MSG (hwnd, WM_PAINT, DArrow_OnPaint);
	HANDLE_MSG (hwnd, WM_LBUTTONUP, DArrow_OnLButtonUp);

	default: return DefWindowProc (hwnd, msg, wParam, lParam);
	}
}

static void DArrow_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	ShowWindow(hwnd, SW_HIDE);
	ShiftNotesOnArrowClick(hwnd);
}

static void DArrow_OnDestroy(HWND hwnd)
{
	HBITMAP		hbmp = (HBITMAP)GetWindowLongPtrW(hwnd, 0);
	if(hbmp)
		DeleteBitmap(hbmp);
}

static void DArrow_OnPaint(HWND hwnd)
{
	PAINTSTRUCT		ps;
	RECT			rc;
	HBITMAP			hbmp;

	BeginPaint(hwnd, &ps);
	hbmp = (HBITMAP)GetWindowLongPtrW(hwnd, 0);
	if(hbmp){
		GetClientRect(hwnd, &rc);
		DrawState(ps.hdc, NULL, NULL, (LPARAM)hbmp, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, DST_BITMAP);
	}
	EndPaint(hwnd, &ps);
}

static void ShiftNotesOnArrowClick(HWND hwnd){
	PDOCKITEM		pItem;
	PDOCKHEADER 	pHeader = NULL;
	int				pos = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
	int				offset = 0, dockType = 0;
	// HWND			hTray = NULL;

	switch(pos){
	case DA_LEFT_UP:
		pHeader = DHeader(DOCK_LEFT);
		dockType = DOCK_LEFT;
		offset = 1;
		break;
	case DA_LEFT_DOWN:
		pHeader = DHeader(DOCK_LEFT);
		dockType = DOCK_LEFT;
		offset = -1;
		break;
	case DA_TOP_LEFT:
		pHeader = DHeader(DOCK_TOP);
		dockType = DOCK_TOP;
		offset = 1;
		break;
	case DA_TOP_RIGHT:
		pHeader = DHeader(DOCK_TOP);
		dockType = DOCK_TOP;
		offset = -1;
		break;
	case DA_RIGHT_UP:
		pHeader = DHeader(DOCK_RIGHT);
		dockType = DOCK_RIGHT;
		offset = 1;
		break;
	case DA_RIGHT_DOWN:
		pHeader = DHeader(DOCK_RIGHT);
		dockType = DOCK_RIGHT;
		offset = -1;
		break;
	case DA_BOTTOM_LEFT:
		pHeader = DHeader(DOCK_BOTTOM);
		dockType = DOCK_BOTTOM;
		offset = 1;
		break;
	case DA_BOTTOM_RIGHT:
		pHeader = DHeader(DOCK_BOTTOM);
		dockType = DOCK_BOTTOM;
		offset = -1;
		break;
	}
	pItem = pHeader->pNext;
	while(pItem){
		pItem->index += offset;
		MoveDockWindow(pItem->hwnd, dockType, pItem->index);
		pItem = pItem->pNext;
	}
	// if(hTray)
		// SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
}
