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
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>

#include "ctreeview.h"

static LRESULT CALLBACK CTreeView_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK CTreeView_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_LBUTTONDBLCLK:
			return FALSE;
		case WM_KEYDOWN:
			if(wParam == VK_SPACE){
				HTREEITEM		hItem = TreeView_GetSelection(hwnd);
				if(CTreeView_IsCheckBoxItem(hwnd, hItem)){
					if(!CTreeView_IsItemEnabled(hwnd, hItem))
						return FALSE;
					CTreeView_ToggleCheckState(hwnd, hItem);
					CTVNCHECKCHANGE		tvcc;
					tvcc.nmhdr.hwndFrom = hwnd;
					tvcc.nmhdr.idFrom = GetDlgCtrlID(hwnd);
					tvcc.nmhdr.code = CTVN_CHECKSTATECHANGED;
					tvcc.hItem = hItem;
					tvcc.fChecked = CTreeView_GetCheckBoxState(hwnd, hItem);
					SendMessageW(GetParent(hwnd), WM_NOTIFY, (WPARAM)hwnd, (LPARAM)&tvcc);
				}
				return FALSE;
			}
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
		case WM_LBUTTONDOWN:{
			TVHITTESTINFO	tvh;
			tvh.pt.x = LOWORD(lParam);
			tvh.pt.y = HIWORD(lParam);

			if(CTreeView_IsLabelClicked(hwnd, &tvh)){
				if(CTreeView_IsCheckBoxItem(hwnd, tvh.hItem)){
					if(!CTreeView_IsItemEnabled(hwnd, tvh.hItem))
						return FALSE;
					CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
					CTVNCHECKCHANGE		tvcc;
					tvcc.nmhdr.hwndFrom = hwnd;
					tvcc.nmhdr.idFrom = GetDlgCtrlID(hwnd);
					tvcc.nmhdr.code = CTVN_CHECKSTATECHANGED;
					tvcc.hItem = tvh.hItem;
					tvcc.fChecked = CTreeView_ToggleCheckState(hwnd, tvh.hItem);
					SendMessageW(GetParent(hwnd), WM_NOTIFY, (WPARAM)hwnd, (LPARAM)&tvcc);
					return FALSE;
				}
			}
			else if(CTreeView_IsCheckboxClicked(hwnd, &tvh)){
				if(CTreeView_IsCheckBoxItem(hwnd, tvh.hItem)){
					if(!CTreeView_IsItemEnabled(hwnd, tvh.hItem))
						return FALSE;
					CTreeView_ToggleCheckState(hwnd, tvh.hItem);
					// CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
					CTVNCHECKCHANGE		tvcc;
					tvcc.nmhdr.hwndFrom = hwnd;
					tvcc.nmhdr.idFrom = GetDlgCtrlID(hwnd);
					tvcc.nmhdr.code = CTVN_CHECKSTATECHANGED;
					tvcc.hItem = tvh.hItem;
					tvcc.fChecked = CTreeView_GetCheckBoxState(hwnd, tvh.hItem);
					if(tvh.hItem != TreeView_GetSelection(hwnd))
						TreeView_SelectItem(hwnd, tvh.hItem);
					SendMessageW(GetParent(hwnd), WM_NOTIFY, (WPARAM)hwnd, (LPARAM)&tvcc);
					return FALSE;
				}
			}
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
		}
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
	}
}

void CTreeView_Subclass(HWND hTree){
	SetWindowLongPtrW(hTree, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(hTree, GWLP_WNDPROC, (LONG_PTR)CTreeView_WndProc));
}

BOOL CTreeView_GetCheckBoxState(HWND hTree, HTREEITEM hItem)
{
    TVITEM 	tvi;
	BOOL	fEnabled;

	fEnabled = CTreeView_IsItemEnabled(hTree, hItem);

    tvi.mask = TVIF_HANDLE | TVIF_IMAGE;
    tvi.hItem = hItem;

    // Request the information.
    TreeView_GetItem(hTree, &tvi);

    // Return zero if it's not checked, or nonzero otherwise.
	if(fEnabled)
    	return tvi.iImage == 1 ? TRUE : FALSE;
	else
		return tvi.iImage == 3 ? TRUE : FALSE;
}

BOOL CTreeView_SetCheckBoxState(HWND hTree, HTREEITEM hItem, BOOL fCheck)
{
    TVITEM 	tvi;
	BOOL	fEnabled;

	fEnabled = CTreeView_IsItemEnabled(hTree, hItem);

    tvi.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvi.hItem = hItem;

	if(fEnabled)
    	tvi.iImage = fCheck ? 1 : 0;
	else
		tvi.iImage = fCheck ? 3 : 2;

	tvi.iSelectedImage = tvi.iImage;

    return TreeView_SetItem(hTree, &tvi);
}

void CTreeView_SetEnable(HWND hTree, HTREEITEM hItem, BOOL fEnable){
	TVITEMW tvi;
	BOOL	fChecked = FALSE, isCheckBox;

	if(CTreeView_IsItemEnabled(hTree, hItem) == fEnable)
		return;

	isCheckBox = CTreeView_IsCheckBoxItem(hTree, hItem);

	if(isCheckBox){
		fChecked = CTreeView_GetCheckBoxState(hTree, hItem);
	    tvi.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	}
	else{
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	}
    tvi.hItem = hItem;

    TreeView_GetItem(hTree, &tvi);

	if(fEnable){
		tvi.lParam = MAKELONG(1, HIWORD(tvi.lParam));
		if(isCheckBox)
			tvi.iImage = fChecked ? 1 : 0;
	}
	else{
		tvi.lParam = MAKELONG(0, HIWORD(tvi.lParam));
		if(isCheckBox)
			tvi.iImage = fChecked ? 3 : 2;
	}
	if(isCheckBox)
		tvi.iSelectedImage = tvi.iImage;

	TreeView_SetItem(hTree, &tvi);
}

BOOL CTreeView_IsItemEnabled(HWND hTree, HTREEITEM hItem){
	TVITEMW tvi;

    tvi.mask = TVIF_HANDLE | TVIF_PARAM;
    tvi.hItem = hItem;

    TreeView_GetItem(hTree, &tvi);

	return (BOOL)(LOWORD(tvi.lParam));
}

BOOL CTreeView_ToggleCheckState(HWND hTree, HTREEITEM hItem)
{
	BOOL	fChecked;

	if(CTreeView_GetCheckBoxState(hTree, hItem))
		fChecked = FALSE;
	else
		fChecked = TRUE;
    
	CTreeView_SetCheckBoxState(hTree, hItem, fChecked);

    return fChecked;
}

BOOL CTreeView_IsCheckboxClicked(HWND hTree, LPTVHITTESTINFO lptvh){
	TreeView_HitTest(hTree, lptvh);
	if((lptvh->flags & TVHT_ONITEMICON) == TVHT_ONITEMICON)
		return TRUE;
	return FALSE;
}

BOOL CTreeView_IsLabelClicked(HWND hTree, LPTVHITTESTINFO lptvh){
	TreeView_HitTest(hTree, lptvh);
	if((lptvh->flags & TVHT_ONITEMLABEL) == TVHT_ONITEMLABEL)
		return TRUE;
	return FALSE;
}

BOOL CTreeView_IsCheckBoxItem(HWND hTree, HTREEITEM hItem)
{
    TVITEMW tvi;

    // Prepare to receive the desired information.
    tvi.mask = TVIF_HANDLE | TVIF_IMAGE;
    tvi.hItem = hItem;

    // Request the information.
    TreeView_GetItem(hTree, &tvi);

    // Return zero if it's not check box item (state image index = 0), or nonzero otherwise.
    return (tvi.iImage >= 0 && tvi.iImage <= 3);
}

LRESULT CTreeView_OnCustomDraw(HWND hParent, HWND hwnd, LPNMTVCUSTOMDRAW pNMTVCD){
	if (pNMTVCD==NULL)
	{
		return -1;
	}
	switch (pNMTVCD->nmcd.dwDrawStage){
		case CDDS_PREPAINT:
			SetWindowLongPtrW(hParent, DWLP_MSGRESULT, (LONG_PTR)CDRF_NOTIFYSUBITEMDRAW);
        	return TRUE;
		case CDDS_ITEMPREPAINT:
			// if(CTreeView_IsCheckBoxItem(hwnd, (HTREEITEM)pNMTVCD->nmcd.dwItemSpec)){
				if(LOWORD(pNMTVCD->nmcd.lItemlParam) == 0){
					pNMTVCD->clrText = GetSysColor(COLOR_GRAYTEXT);
					pNMTVCD->clrTextBk = GetSysColor(COLOR_WINDOW);
					if((pNMTVCD->nmcd.uItemState & CDIS_HOT) == CDIS_HOT){
						SetWindowLongPtrW(hParent, DWLP_MSGRESULT, (LONG_PTR)CDRF_SKIPDEFAULT);
						return TRUE;
					}
				}
			// }
			SetWindowLongPtrW(hParent, DWLP_MSGRESULT, (LONG_PTR)CDRF_NOTIFYPOSTPAINT);
			return TRUE;
		case CDDS_ITEMPOSTPAINT:
			SetWindowLongPtrW(hParent, DWLP_MSGRESULT, (LONG_PTR)CDRF_DODEFAULT);
			return TRUE;
	}
	SetWindowLongPtrW(hParent, DWLP_MSGRESULT, (LONG_PTR)CDRF_DODEFAULT);
	return TRUE;
}
