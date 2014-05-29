// Copyright (C) 2007 Andrey Gruber (aka lamer)

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
#include "glistbox.h"
#include "gllist.h"
#include "transparentbitmap.h"

#define	LST_PROC_PROP		"_GList_ListProc_Name"		// property for storing listbox window procedure address
#define	LST_PROC_PROP_W		L"_GList_ListProc_Name"		// property for storing listbox window procedure address (unicode) 

static LRESULT CALLBACK ListProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK ParentProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void GListOnDrawItem(LPDRAWITEMSTRUCT lpd);
static LRESULT AddGListEntryW(HWND hList, PGLIST_IMAGE pi, LPCWSTR lpString);

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: CreateGListWindowW
 Created  : Mon Jul 16 13:14:37 2007
 Modified : Mon Jul 16 13:24:19 2007

 Synopsys : Creates glist Unicode window
 Input    : dwExStyle - extended window style
            dwStyle - window style
            x - the initial horizontal position of the window
            y - the initial vertical position of the window
            w - the width of the window
            h - the height of the window
            hParent - handle to the parent window
 Output   : If the function succeeds, the return value is a handle to the 
            new glist window.
            If the function fails, the return value is NULL. To get 
            extended error information, call GetLastError
             
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

HWND CreateGListWindowW(DWORD dwExStyle, DWORD dwStyle, int x, int y, int w, int h, HWND hParent){
	HWND			hList = NULL;
	WNDPROC			hProc, hListProc;

	// add some listbox styles if they are missing
	dwStyle |= (LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | LBS_OWNERDRAWVARIABLE | LBS_NOTIFY | WS_VSCROLL);
	// create listbox window
	hList = CreateWindowExW(dwExStyle, L"listbox", NULL, dwStyle, x, y, w, h, hParent, NULL, (HINSTANCE)GetWindowLongPtrW(hParent, GWLP_HINSTANCE), NULL);
	if(hList == NULL)
		return NULL;
	// get listbox window procedure address and subclass it (must clear errors before call to SetWindowLongPtrW)
	SetLastError(0);
	hListProc = (WNDPROC)SetWindowLongPtrW(hList, GWLP_WNDPROC, (LONG_PTR)ListProc);
	if(hListProc == 0 && GetLastError() != 0)
		return NULL;
	// store listbox window procedure address
	if(SetPropW(hList, LST_PROC_PROP_W, hListProc) == 0)
		return NULL;
	// add parent window data to inner collection
	if(!GLStructExists(hParent)){
		// get parent window procedure address and subclass it (must clear errors before call to SetWindowLongPtrW)
		SetLastError(0);
		hProc = (WNDPROC)SetWindowLongPtrW(hParent, GWLP_WNDPROC, (LONG_PTR)ParentProc);
		if(hProc == 0 && GetLastError() != 0)
			return NULL;
		// add parent handle and proc address to inner collection
		if(AddNewGLStruct(hParent, hProc) == NULL)
			return NULL;
	}
	// increase counter of glist windows belong to parent
	IncGListCount(hParent);
	return hList;
}

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: AddGListEntryW
 Created  : Mon Jul 16 14:16:58 2007
 Modified : Mon Jul 16 14:18:21 2007

 Synopsys : Adds new entry to Unicode glist window
 Input    : hList - glist window handle
            lpString - item text
            pi - pointer to GLIST_IMAGE structure containing image/color 
            information
 Output   : Zero based index of newly added entry on success or -1 
            otherwise
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static LRESULT AddGListEntryW(HWND hList, PGLIST_IMAGE pi, LPCWSTR lpString){

	LRESULT				res;
	PGLIST_IMAGE		pData;

	// check for valid image/color type
	if(pi->nType < GLIST_I_COLOR || pi->nType > GLIST_I_ICON)
		return -1;
	// add string to listbox
	res = SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)lpString);
	if(res == LB_ERR || res == LB_ERRSPACE)
		return -1;
	// allocate memory for image/color infomation
	pData = (PGLIST_IMAGE)calloc(1, sizeof(GLIST_IMAGE));
	if(pData == NULL)
		return -1;
	// store image/color information
	memcpy(pData, pi, sizeof(GLIST_IMAGE));
	// store pointer to image/color information as item data
	if(SendMessageW(hList, LB_SETITEMDATA, res, (LPARAM)pData) == LB_ERR)
		return -1;
	// if individual item height is specified
	if(pi->itemHeight > 0)
		// set listbox item height
		if(SendMessageW(hList, LB_SETITEMHEIGHT, res, pi->itemHeight) == LB_ERR)
			return -1;
	return res;
}

static LRESULT CALLBACK ParentProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

	WNDPROC					hProc;
	LPDRAWITEMSTRUCT		lpd;

	// get original window procedure from inner collection
	hProc = GetGLProc(hwnd);
	switch(msg){
		case WM_DESTROY:
			// if window still is in inner collection
			if(GLStructExists(hwnd)){
				// delete it
				DeleteGLStruct(hwnd);
			}
			break;
		case WM_DRAWITEM:
			lpd = (LPDRAWITEMSTRUCT)lParam;
			// draw listbox item
			if(lpd->CtlType == ODT_LISTBOX){
				GListOnDrawItem(lpd);
				return TRUE;
			}
			// otherwise call to original window procedure
			else{
				return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
			}
				
		default:
			// call to original window procedure
			return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
	}
	// call to original window procedure
	return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK ListProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	DWORD			nEntries = 0;
	PGLIST_IMAGE	pData;
	HWND			hParent;
	WNDPROC			hProc;

	// get original glist procedure
	hProc = (WNDPROC)GetPropW(hwnd, LST_PROC_PROP_W);

	switch(msg)
	{
	case WM_DESTROY:
		// get number of listbox items
		nEntries = SendMessageW(hwnd, LB_GETCOUNT, 0 , 0);
		// free memory associated with each item
		for(int i = 0; i < nEntries; i++){
			pData = (PGLIST_IMAGE)SendMessageW(hwnd, LB_GETITEMDATA, i, 0);
			if(pData != 0)
				free(pData);
		}
		// get parent window
		hParent = GetParent(hwnd);
		// if parent window is still in innner collection
		if(GLStructExists(hParent)){
			// decrease number of glist windows belong to parent
			DecGListCount(hParent);
			// if there no glist windows left
			if(GetGListCount(hParent) == 0)
				// remove the parent window from innner collection
				DeleteGLStruct(hParent);
		}
		// remove the property associated with glist window
		RemovePropW(hwnd, LST_PROC_PROP_W);
		return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
	case GLM_ADDENTRY:
		return AddGListEntryW(hwnd, (PGLIST_IMAGE)wParam, (LPCWSTR)lParam);
	default:
		// call to original glist window procedure
		return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
	}
}

static void GListOnDrawItem(LPDRAWITEMSTRUCT lpd){
	wchar_t			szUBuffer[256];
	RECT			rc, rci;
	PGLIST_IMAGE	pData;
	HBRUSH			hbr;
	COLORREF		crText;

	// copy drawing rectangle
	CopyRect(&rc, &lpd->rcItem);
	// get image/color data associated with listbox item
	pData = (PGLIST_IMAGE)SendMessageW(lpd->hwndItem, LB_GETITEMDATA, lpd->itemID, 0);
	// make rectangle for image/color
	if(pData != 0){
		if(pData->szImage.cx ==0 || pData->szImage.cy == 0)
			SetRect(&rci, rc.left + 1, rc.top, rc.left + 1 + (rc.bottom - rc.top), rc.bottom);
		else if(pData->szImage.cy >= rc.bottom - rc.top)
			SetRect(&rci, rc.left + 1, rc.top, rc.left + 1 + (rc.bottom - rc.top), rc.bottom);
		else
			SetRect(&rci, rc.left + 1, rc.top + ((rc.bottom - rc.top) - pData->szImage.cy) / 2, rc.left + 1 + pData->szImage.cx, rc.bottom - ((rc.bottom - rc.top) - pData->szImage.cy) / 2);
	}
	// set back mode to transparent
	SetBkMode(lpd->hDC, TRANSPARENT);
	// fill background and set text color depending on item state
	if((lpd->itemState & ODS_SELECTED) == ODS_SELECTED){
		// define selected back and text color
		crText = (pData->crSelectedText == 0) ? GetSysColor(COLOR_HIGHLIGHTTEXT) : pData->crSelectedText;
		if(pData->crSelectedBack == 0)
			hbr = GetSysColorBrush(COLOR_HIGHLIGHT);
		else
			hbr = CreateSolidBrush(pData->crSelectedBack);
		SetTextColor(lpd->hDC, crText);
		FillRect(lpd->hDC, &rc, hbr);
		if(pData->crSelectedBack != 0)
			DeleteBrush(hbr);
		if((lpd->itemState & ODS_FOCUS) == ODS_FOCUS){
			DrawFocusRect(lpd->hDC, &rc);
		}
	}
	else{
		crText = (pData->crText == 0) ? GetSysColor(COLOR_WINDOWTEXT) : pData->crText;
		SetTextColor(lpd->hDC, crText);
		if(pData->crBack == 0)
			hbr = GetSysColorBrush(COLOR_WINDOW);
		else
			hbr = CreateSolidBrush(pData->crBack);
		FillRect(lpd->hDC, &rc, hbr);
		if(pData->crBack != 0)
			DeleteBrush(hbr);
	}
	if(pData != 0){
		switch(pData->nType){
		case GLIST_I_COLOR:
			// fill rectangle with color
			hbr = CreateSolidBrush((COLORREF)pData->dwValue);
			FillRect(lpd->hDC, &rci, hbr);
			DeleteBrush(hbr);
			break;
		case GLIST_I_ICON:
			// draw icon
			DrawIconEx(lpd->hDC, rci.left, rci.top, (HICON)pData->dwValue, rci.right - rci.left, rci.bottom - rci.top, 0, 0, DI_NORMAL);
			break;
		case GLIST_I_BITMAP:
			// draw bitmap transparent
			DrawTransparentBitmap((HBITMAP)pData->dwValue, lpd->hDC, rci.left, rci.top, rci.right - rci.left, rci.bottom - rci.top, pData->xSrc, pData->ySrc, pData->crMask);
			break;
		}
		// draw frame arround the image/color rectangle, if specified
		if(pData->frame)
			FrameRect(lpd->hDC, &rci, GetStockObject(BLACK_BRUSH));
		// offset text rectangle
		OffsetRect(&rc, rci.right + 4, 0);
	}
	else{
		// offset text rectangle
		OffsetRect(&rc, 1, 0);
	}
	// get item text and draw it
	SendMessageW(lpd->hwndItem, LB_GETTEXT, lpd->itemID, (LPARAM)szUBuffer);
	DrawTextExW(lpd->hDC, szUBuffer, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, NULL);
}



