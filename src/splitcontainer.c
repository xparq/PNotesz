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
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdlib.h>

#include "splitcontainer.h"

#define	SPLIT_MAIN_PANEL_VERTICAL_CLASS		L"__AKDJFGOI_MAIN_SPLIT_PANEL_VERTICAL_RKVYALXL__"
#define	SPLIT_MAIN_PANEL_HORIZONTAL_CLASS	L"__AKDJFGOI_MAIN_SPLIT_PANEL_HORIZONTAL_RKVYALXL__"
#define	SPLIT_SUB_PANEL_CLASS				L"__PQIDHERC_SUB_SPLIT_PANEL_YENAKSID__"

#define	PROP_PARENT							L"PROP_PARENT"
#define	PROP_CHILD							L"PROP_CHILD"
#define	PROP_VISIBLE						L"PROP_VISIBLE"

typedef struct _CHILD_DATA {
	HWND		hwnd;
	RECT		rc;
}CHILD_DATA;

typedef struct _SPLIT_DATA {
	int			splitStyle;
	int			stopper;
	int			marginL;
	int			marginT;
	int			marginR;
	int			marginB;
	CHILD_DATA	cTop;
	CHILD_DATA	cBottom;
	CHILD_DATA	cLeft;
	CHILD_DATA	cRight;
}SPLIT_DATA, *PSPLIT_DATA;

typedef enum _split_offsets {OFF_SP_DATA} split_offsets;

#define	SPLITDATA(hwnd)	(PSPLIT_DATA)GetWindowLongPtrW(hwnd, OFF_SP_DATA)

static BOOL RegisterSplitter(HINSTANCE hInstance, const wchar_t * cpClass, HCURSOR hCursor);
static LRESULT CALLBACK MainPanel_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void MainPanel_OnSize(HWND hwnd, UINT state, int cx, int cy);
static void MainPanel_OnDestroy(HWND hwnd);
static void MainPanel_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
static LRESULT CALLBACK SubPanel_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void SubPanel_OnSize(HWND hwnd, UINT state, int cx, int cy);
static BOOL Subpanel_OnWindowPosChanging(HWND hwnd, LPWINDOWPOS lpwpos);
static BOOL CALLBACK PropEnumProc(HWND hwnd, LPCWSTR lpszString, HANDLE hData);
static void ResizeMain(HWND hMain);
static void AddChild(HWND hMain, HWND hwnd, int position);
static void HideChild(HWND hMain, int position);
static void ShowChild(HWND hMain, int position);

//HWND		m_hMainPanel;

HWND CreateMainPanel(int splitStyle, HINSTANCE hInstance, HWND hParent, HCURSOR hCursor, int nStopper, int nLeft, int nTop, int nRight, int nBottom, int nInit){

	RECT			rc;
	HWND			hwnd = NULL;
	int				width, height, initSize;

	switch(splitStyle){
		case SPLIT_HORZ:
			if(!RegisterSplitter(hInstance, SPLIT_MAIN_PANEL_HORIZONTAL_CLASS, hCursor))
				return NULL;
			break;
		case SPLIT_VERT:
			if(!RegisterSplitter(hInstance, SPLIT_MAIN_PANEL_VERTICAL_CLASS, hCursor))
				return NULL;
			break;
	}
	
	GetClientRect(hParent, &rc);
	width = rc.right - rc.left - (nLeft + nRight);
	height = rc.bottom - rc.top - (nTop + nBottom);
	switch(splitStyle){
		case SPLIT_HORZ:
			hwnd = CreateWindowExW(0, SPLIT_MAIN_PANEL_HORIZONTAL_CLASS, NULL, WS_CHILD | WS_VISIBLE, nLeft, nTop, width, height, hParent, NULL, hInstance, NULL);
			break;
		case SPLIT_VERT:
			hwnd = CreateWindowExW(0, SPLIT_MAIN_PANEL_VERTICAL_CLASS, NULL, WS_CHILD | WS_VISIBLE, nLeft, nTop, width, height, hParent, NULL, hInstance, NULL);
			break;
	}
	if(hwnd){
		PSPLIT_DATA		pData = (PSPLIT_DATA)calloc(1, sizeof(SPLIT_DATA));
		if(!pData)
			return NULL;
		pData->splitStyle = splitStyle;
		pData->stopper = nStopper;
		pData->marginL = nLeft;
		pData->marginT = nTop;
		pData->marginR = nRight;
		pData->marginB = nBottom;
		SetPropW(hwnd, PROP_PARENT, hParent);
		switch(splitStyle){
			case SPLIT_HORZ:
				if(nInit == -1)
					initSize = 3 * height / 4;
				else
					initSize = nInit;
				pData->cTop.hwnd = CreateWindowExW(0, SPLIT_SUB_PANEL_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, width, initSize - 2, hwnd, NULL, hInstance, NULL);
				SetPropW(pData->cTop.hwnd, PROP_VISIBLE, (HANDLE)TRUE);
				pData->cBottom.hwnd = CreateWindowExW(0, SPLIT_SUB_PANEL_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, initSize + 2, width, height - initSize - 2, hwnd, NULL, hInstance, NULL);
				SetPropW(pData->cBottom.hwnd, PROP_VISIBLE, (HANDLE)TRUE);
				break;
			case SPLIT_VERT:
				if(nInit == -1)
					initSize = width / 4;
				else
					initSize = nInit;
				pData->cLeft.hwnd = CreateWindowExW(0, SPLIT_SUB_PANEL_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, initSize - 2, height, hwnd, NULL, hInstance, NULL);
				SetPropW(pData->cLeft.hwnd, PROP_VISIBLE, (HANDLE)TRUE);
				pData->cRight.hwnd = CreateWindowExW(0, SPLIT_SUB_PANEL_CLASS, NULL, WS_CHILD | WS_VISIBLE, initSize + 2, 0, width - initSize - 2, height, hwnd, NULL, hInstance, NULL);
				SetPropW(pData->cRight.hwnd, PROP_VISIBLE, (HANDLE)TRUE);
				break;
		}
		SetWindowLongPtrW(hwnd, OFF_SP_DATA, (LONG_PTR)pData);
	}
	return hwnd;
}

static void AddChild(HWND hMain, HWND hwnd, int position){
	RECT			rc;
	PSPLIT_DATA		pData = SPLITDATA(hMain);

	switch(position){
		case CHILD_TOP:
			SetParent(hwnd, pData->cTop.hwnd);
			SetPropW(pData->cTop.hwnd, PROP_CHILD, (HANDLE)hwnd);
			GetClientRect(pData->cTop.hwnd, &rc);
			break;
		case CHILD_BOTTOM:
			SetParent(hwnd, pData->cBottom.hwnd);
			SetPropW(pData->cBottom.hwnd, PROP_CHILD, (HANDLE)hwnd);
			GetClientRect(pData->cBottom.hwnd, &rc);
			break;
		case CHILD_LEFT:
			SetParent(hwnd, pData->cLeft.hwnd);
			SetPropW(pData->cLeft.hwnd, PROP_CHILD, (HANDLE)hwnd);
			GetClientRect(pData->cLeft.hwnd, &rc);
			break;
		case CHILD_RIGHT:
			SetParent(hwnd, pData->cRight.hwnd);
			SetPropW(pData->cRight.hwnd, PROP_CHILD, (HANDLE)hwnd);
			GetClientRect(pData->cRight.hwnd, &rc);
			break;
	}
	MoveWindow(hwnd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, FALSE);
}

static void HideChild(HWND hMain, int position){
	PSPLIT_DATA		pData = SPLITDATA(hMain);

	//hide window only if opposite window is visible
	switch(position){
		case CHILD_TOP:
			if(IsWindowVisible(pData->cBottom.hwnd)){
				GetClientRect(pData->cTop.hwnd, &pData->cTop.rc);
				ShowWindow(pData->cTop.hwnd, SW_HIDE);
				SetPropW(pData->cTop.hwnd, PROP_VISIBLE, (HANDLE)FALSE);
			}
			break;
		case CHILD_BOTTOM:
			if(IsWindowVisible(pData->cTop.hwnd)){
				GetClientRect(pData->cBottom.hwnd, &pData->cBottom.rc);
				ShowWindow(pData->cBottom.hwnd, SW_HIDE);
				SetPropW(pData->cBottom.hwnd, PROP_VISIBLE, (HANDLE)FALSE);
			}
			break;
		case CHILD_LEFT:
			if(IsWindowVisible(pData->cRight.hwnd)){
				GetClientRect(pData->cLeft.hwnd, &pData->cLeft.rc);
				ShowWindow(pData->cLeft.hwnd, SW_HIDE);
				SetPropW(pData->cLeft.hwnd, PROP_VISIBLE, (HANDLE)FALSE);
			}
			break;
		case CHILD_RIGHT:
			if(IsWindowVisible(pData->cLeft.hwnd)){
				GetClientRect(pData->cRight.hwnd, &pData->cRight.rc);
				ShowWindow(pData->cRight.hwnd, SW_HIDE);
				SetPropW(pData->cRight.hwnd, PROP_VISIBLE, (HANDLE)FALSE);
			}
			break;
	}
	ResizeMain(hMain);
}

static void ShowChild(HWND hMain, int position){
	PSPLIT_DATA		pData = SPLITDATA(hMain);

	switch(position){
		case CHILD_TOP:
			if(!IsWindowVisible(pData->cTop.hwnd)){
				RECT		rc;

				GetClientRect(hMain, &rc);
				ShowWindow(pData->cTop.hwnd, SW_SHOW);
				SetPropW(pData->cTop.hwnd, PROP_VISIBLE, (HANDLE)TRUE);
				MoveWindow(pData->cBottom.hwnd, 0, (rc.bottom - rc.top) - (pData->cTop.rc.bottom - pData->cTop.rc.top) + 4, rc.right - rc.left, (rc.bottom - rc.top) - (pData->cTop.rc.bottom - pData->cTop.rc.top) - 4, TRUE);
			}
			break;
		case CHILD_BOTTOM:
			if(!IsWindowVisible(pData->cBottom.hwnd)){
				SetPropW(pData->cBottom.hwnd, PROP_VISIBLE, (HANDLE)TRUE);
				ShowWindow(pData->cBottom.hwnd, SW_SHOW);
			}
			break;
		case CHILD_LEFT:
			if(!IsWindowVisible(pData->cLeft.hwnd)){
				SetPropW(pData->cLeft.hwnd, PROP_VISIBLE, (HANDLE)TRUE);
				ShowWindow(pData->cLeft.hwnd, SW_SHOW);
			}
			break;
		case CHILD_RIGHT:{
			if(!IsWindowVisible(pData->cRight.hwnd)){
				RECT		rc;

				GetClientRect(hMain, &rc);
				ShowWindow(pData->cRight.hwnd, SW_SHOW);
				SetPropW(pData->cRight.hwnd, PROP_VISIBLE, (HANDLE)TRUE);
				MoveWindow(pData->cLeft.hwnd, 0, 0, (rc.right - rc.left) - (pData->cRight.rc.right - pData->cRight.rc.left) - 4, rc.bottom - rc.top, TRUE);
			}
			break;
		}
	}
	ResizeMain(hMain);
}

static void ResizeMain(HWND hMain){
	RECT			rc;

	GetClientRect(hMain, &rc);
	SendMessageW(hMain, WM_SIZE, 0, MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top));
}

static BOOL RegisterSplitter(HINSTANCE hInstance, const wchar_t * cpClass, HCURSOR hCursor){

	WNDCLASSEXW 	wcx;

	ZeroMemory(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.hInstance = hInstance;
	wcx.lpszClassName = cpClass;
	wcx.lpfnWndProc = MainPanel_WndProc;
	wcx.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcx.hCursor = hCursor;
	wcx.cbWndExtra = 4;
	if(!RegisterClassExW(&wcx)){
		if(GetLastError() != ERROR_CLASS_ALREADY_EXISTS){
			return FALSE;
		}
	}

	wcx.lpszClassName = SPLIT_SUB_PANEL_CLASS;
	wcx.lpfnWndProc = SubPanel_WndProc;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.cbWndExtra = 0;
	if(!RegisterClassExW(&wcx)){
		if(GetLastError() != ERROR_CLASS_ALREADY_EXISTS){
			return FALSE;
		}
	}
	return TRUE;
}

static LRESULT CALLBACK MainPanel_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_SIZE, MainPanel_OnSize);
		HANDLE_MSG (hwnd, WM_DESTROY, MainPanel_OnDestroy);
		HANDLE_MSG (hwnd, WM_MOUSEMOVE, MainPanel_OnMouseMove);
		//case WM_ERASEBKGND:
			//return TRUE;
		case SPM_ADD_CHILD:
			AddChild(hwnd, (HWND)wParam, lParam);
			return TRUE;
		case SPM_HIDE_CHILD:
			HideChild(hwnd, lParam);
			return TRUE;
		case SPM_SHOW_CHILD:
			ShowChild(hwnd, lParam);
			return TRUE;
		case SPM_SET_NEW_TOP:{
			PSPLIT_DATA		pData = SPLITDATA(hwnd);
			if(pData){
				pData->marginT = wParam;
			}
			return TRUE;
		}
		case SPM_MAIN_RESIZED:{
			PSPLIT_DATA		pData = SPLITDATA(hwnd);
			if(pData){
				MoveWindow(hwnd, pData->marginL, pData->marginT, LOWORD(lParam) - (pData->marginL + pData->marginR), HIWORD(lParam) - (pData->marginT + pData->marginB), FALSE);
				switch(pData->splitStyle){
					case SPLIT_HORZ:
						RedrawWindow(pData->cBottom.hwnd, NULL, NULL, RDW_INVALIDATE);
						RedrawWindow(pData->cTop.hwnd, NULL, NULL, RDW_INVALIDATE);
						break;
					case SPLIT_VERT:
						RedrawWindow(pData->cLeft.hwnd, NULL, NULL, RDW_INVALIDATE);
						RedrawWindow(pData->cRight.hwnd, NULL, NULL, RDW_INVALIDATE);
						break;
				}
			}
			return TRUE;
		}
		case SPM_TOP_HEIGHT:{
			PSPLIT_DATA		pData = SPLITDATA(hwnd);
			RECT			rc;
			if(pData){
				GetWindowRect(pData->cTop.hwnd, &rc);
				return(rc.bottom - rc.top + 2);
			}
			return 4;
		}
		case SPM_LEFT_WIDTH:{
			PSPLIT_DATA		pData = SPLITDATA(hwnd);
			RECT			rc;
			if(pData){
				GetWindowRect(pData->cLeft.hwnd, &rc);
				return(rc.right - rc.left + 2);
			}
			return 4;
		}
		case WM_NOTIFY:
			return SendMessageW((HWND)GetPropW(hwnd, PROP_PARENT), WM_NOTIFY, wParam, lParam);
		default: return DefWindowProc (hwnd, msg, wParam, lParam);
	}
}

static void MainPanel_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	if(keyFlags == MK_LBUTTON){
		PSPLIT_DATA		pData = SPLITDATA(hwnd);
		BOOL			fFullDrag;

		ReleaseCapture();
		SystemParametersInfoW(SPI_GETDRAGFULLWINDOWS, 0, &fFullDrag, 0);
		SystemParametersInfoW(SPI_SETDRAGFULLWINDOWS, TRUE, 0, 0);
		switch(pData->splitStyle){
			case SPLIT_HORZ:
				SendMessageW(pData->cBottom.hwnd, WM_NCLBUTTONDOWN, HTTOP, 0);
				SystemParametersInfoW(SPI_SETDRAGFULLWINDOWS, fFullDrag, 0, 0);
				break;
			case SPLIT_VERT:
				SendMessageW(pData->cLeft.hwnd, WM_NCLBUTTONDOWN, HTRIGHT, 0);
				SystemParametersInfoW(SPI_SETDRAGFULLWINDOWS, fFullDrag, 0, 0);
				break;
		}
	}
}

static void MainPanel_OnDestroy(HWND hwnd)
{
	PSPLIT_DATA		pData = SPLITDATA(hwnd);
	if(pData){
		EnumPropsW(pData->cLeft.hwnd, PropEnumProc);
		EnumPropsW(pData->cBottom.hwnd, PropEnumProc);
		EnumPropsW(pData->cRight.hwnd, PropEnumProc);
		EnumPropsW(pData->cTop.hwnd, PropEnumProc);
		free(pData);
	}
	
	EnumPropsW(hwnd, PropEnumProc);
}

static void MainPanel_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	RECT			rc;
	PSPLIT_DATA		pData = SPLITDATA(hwnd);

	if(pData){
		switch(pData->splitStyle){
			case SPLIT_HORZ:
				if(GetPropW(pData->cBottom.hwnd, PROP_VISIBLE) && GetPropW(pData->cTop.hwnd, PROP_VISIBLE)){
					GetClientRect(pData->cBottom.hwnd, &rc);
					MoveWindow(pData->cBottom.hwnd, 0, cy - (rc.bottom - rc.top), cx, rc.bottom - rc.top, TRUE);
					MoveWindow(pData->cTop.hwnd, 0, 0, cx, cy - (rc.bottom - rc.top) - 4, TRUE);
				}
				else if(GetPropW(pData->cBottom.hwnd, PROP_VISIBLE)){
					MoveWindow(pData->cBottom.hwnd, 0, 0, cx, cy, TRUE);
				}
				else if(GetPropW(pData->cTop.hwnd, PROP_VISIBLE)){
					MoveWindow(pData->cTop.hwnd, 0, 0, cx, cy, TRUE);
				}
				break;
			case SPLIT_VERT:
				if(GetPropW(pData->cLeft.hwnd, PROP_VISIBLE) && GetPropW(pData->cRight.hwnd, PROP_VISIBLE)){
					GetClientRect(pData->cLeft.hwnd, &rc);
					MoveWindow(pData->cLeft.hwnd, 0, 0, rc.right - rc.left, cy, TRUE);
					MoveWindow(pData->cRight.hwnd, rc.right - rc.left + 4, 0, cx - (rc.right - rc.left) - 4, cy, TRUE);
				}
				else if(GetPropW(pData->cLeft.hwnd, PROP_VISIBLE)){
					MoveWindow(pData->cLeft.hwnd, 0, 0, cx, cy, TRUE);
				}
				else if(GetPropW(pData->cRight.hwnd, PROP_VISIBLE)){
					MoveWindow(pData->cRight.hwnd, 0, 0, cx, cy, TRUE);
				}
				break;
		}
	}
}

static LRESULT CALLBACK SubPanel_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_SIZE, SubPanel_OnSize);
		HANDLE_MSG (hwnd, WM_WINDOWPOSCHANGING, Subpanel_OnWindowPosChanging);
		//HANDLE_MSG (hwnd, WM_WINDOWPOSCHANGED, Subpanel_OnWindowPosChanged);

		case WM_ERASEBKGND:
			return TRUE;
		case WM_NOTIFY:
			return SendMessageW(GetParent(hwnd), WM_NOTIFY, wParam, lParam);
		default: return DefWindowProc (hwnd, msg, wParam, lParam);
	}
}

static BOOL Subpanel_OnWindowPosChanging(HWND hwnd, LPWINDOWPOS lpwpos)
{
	RECT			rc;
	PSPLIT_DATA		pData = SPLITDATA(GetParent(hwnd));

	if(pData){
		if(hwnd == pData->cBottom.hwnd && GetPropW(pData->cTop.hwnd, PROP_VISIBLE)){	
			GetClientRect(GetParent(hwnd), &rc);
			if(lpwpos->y < pData->stopper + 4){
				lpwpos->y = pData->stopper + 4;
				lpwpos->cy = rc.bottom - pData->stopper - 4;
				MoveWindow(pData->cTop.hwnd, 0, 0, lpwpos->cx, (rc.bottom - rc.top) - lpwpos->cy - 4, TRUE);
				return FALSE;
			}
			else if(lpwpos->cy < pData->stopper){
				lpwpos->y = rc.bottom - pData->stopper;
				lpwpos->cy = pData->stopper;
				MoveWindow(pData->cTop.hwnd, 0, 0, lpwpos->cx, (rc.bottom - rc.top) - lpwpos->cy - 4, TRUE);
				return FALSE;
			}
			MoveWindow(pData->cTop.hwnd, 0, 0, lpwpos->cx, (rc.bottom - rc.top) - lpwpos->cy - 4, TRUE);
		}
		if(hwnd == pData->cLeft.hwnd && GetPropW(pData->cRight.hwnd, PROP_VISIBLE)){
			GetClientRect(GetParent(hwnd), &rc);
			if(lpwpos->cx < pData->stopper){
				lpwpos->cx = pData->stopper;
				MoveWindow(pData->cRight.hwnd, lpwpos->cx + 4, 0, (rc.right - rc.left) - (lpwpos->cx + 4), lpwpos->cy, TRUE);
				return TRUE;
			}
			else if((rc.right - rc.left) - lpwpos->cx - 4 < pData->stopper){
				lpwpos->cx = (rc.right - rc.left) - pData->stopper - 4;
				MoveWindow(pData->cRight.hwnd, lpwpos->cx + 4, 0, (rc.right - rc.left) - (lpwpos->cx + 4), lpwpos->cy, TRUE);
				return TRUE;
			}
			MoveWindow(pData->cRight.hwnd, lpwpos->cx + 4, 0, (rc.right - rc.left) - (lpwpos->cx + 4), lpwpos->cy, TRUE);
		}
		else{
			return TRUE;
		}
	}
	return TRUE;
}

static void SubPanel_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	HWND			hChild;
	PSPLIT_DATA		pData = SPLITDATA(GetParent(hwnd));

	if(pData){
		hChild = (HWND)GetPropW(hwnd, PROP_CHILD);
		MoveWindow(hChild, 0, 0, cx, cy, TRUE);
	}
}

static BOOL CALLBACK PropEnumProc(HWND hwnd, LPCWSTR lpszString, HANDLE hData){
	if((DWORD)(VOID *)hData != 0xffffffff){
		RemovePropW(hwnd, lpszString);
	}
	return TRUE;
}

