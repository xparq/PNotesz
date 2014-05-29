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

#include "notes.h"
#include "global.h"
#include "shared.h"
#include "stringconstants.h"
#include "numericconstants.h"
#include "ctreeview.h"
#include "contacts.h"
#include "contgroups.h"
#include "multigroups.h"

static BOOL Multigroups_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Multigroups_OnClose(HWND hwnd);
static void Multigroups_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void ClearDialog(void);
static void PrepareGroupsList(HWND hTree);
static void CT_OnCheckStateChanged(HWND hwnd, HTREEITEM hItem, BOOL fChecked);
static BOOL IsContactChecked(HWND hTree);
static void CreateGroupsContactsList(HWND hTree);

HIMAGELIST			m_hImlDefCheck;

BOOL CALLBACK Multigroups_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, Multigroups_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, Multigroups_OnInitDialog);
		HANDLE_MSG (hwnd, WM_CLOSE, Multigroups_OnClose);
		case WM_NOTIFY:{
			NMHDR	*lpnmh = (NMHDR *)lParam;
			if(lpnmh->idFrom == IDC_LST_SEVERAL_CONT && lpnmh->code == CTVN_CHECKSTATECHANGED){
				LPCTVNCHECKCHANGE	lpctvn = (LPCTVNCHECKCHANGE)lpnmh;
				CT_OnCheckStateChanged(lpnmh->hwndFrom, lpctvn->hItem, lpctvn->fChecked);
			}
			else if(lpnmh->idFrom == IDC_LST_SEVERAL_CONT && lpnmh->code == NM_CUSTOMDRAW){
				LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)lpnmh;
				return CTreeView_OnCustomDraw(hwnd, lpnmh->hwndFrom, pNMTVCD);
			}
			return FALSE;
		}
		default: return FALSE;
	}
}

static void Multigroups_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify){
	switch(id){
		case IDCANCEL:
			ClearDialog();
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDOK:{
			CreateGroupsContactsList(GetDlgItem(hwnd, IDC_LST_SEVERAL_CONT));
			ClearDialog();
			EndDialog(hwnd, IDOK);
			break;
		}
	}
}

static BOOL Multigroups_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t			szBuffer[256], szKey[16];
	HBITMAP			hBmp;
	HMODULE			hIcons;

	g_hMultiGroups = hwnd;
	hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hIcons){
		//prepare image list for check image
		hBmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_CHECKS));
		m_hImlDefCheck = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 0);
		ImageList_AddMasked(m_hImlDefCheck, hBmp, CLR_MASK);
		DeleteBitmap(hBmp);
		DeleteObject((void *)CLR_MASK);
		FreeLibrary(hIcons);
	}
	
	_itow(IDC_ST_CONT_GROUPS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Groups", szBuffer, 255, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, L"OK");
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, L"Cancel");

	CTreeView_Subclass(GetDlgItem(hwnd, IDC_LST_SEVERAL_CONT));
	TreeView_SetImageList(GetDlgItem(hwnd, IDC_LST_SEVERAL_CONT), m_hImlDefCheck, TVSIL_NORMAL);
	PrepareGroupsList(GetDlgItem(hwnd, IDC_LST_SEVERAL_CONT));

	return TRUE;
}

static void Multigroups_OnClose(HWND hwnd)
{
	ClearDialog();
	EndDialog(hwnd, IDCANCEL);
}

static void PrepareGroupsList(HWND hTree){
	TVINSERTSTRUCTW		tvs = {0};
	wchar_t				szBuffer[128];
	LPPCONTGROUP		lpc;
	HTREEITEM			hItem;

	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_LAST;
	tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.lParam = MAKELONG(1, 1);
	tvs.item.pszText = szBuffer;
	lpc = g_PContGroups;
	while(lpc){
		if(PContactsInGroup(g_PContacts, lpc->id) > 0){
			wcscpy(szBuffer, lpc->name);
			hItem = (HTREEITEM)SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
			CTreeView_SetCheckBoxState(hTree, hItem, FALSE);
		}
		lpc = lpc->next;
	}
}

static void ClearDialog(void){
	g_hMultiGroups = NULL;
	if(m_hImlDefCheck){
		ImageList_Destroy(m_hImlDefCheck);
		m_hImlDefCheck = NULL;
	}
}

static void CT_OnCheckStateChanged(HWND hwnd, HTREEITEM hItem, BOOL fChecked){
	if(fChecked){
		EnableWindow(GetDlgItem(g_hMultiGroups, IDOK), TRUE);
	}
	else{
		EnableWindow(GetDlgItem(g_hMultiGroups, IDOK), IsContactChecked(GetDlgItem(g_hMultiGroups, IDC_LST_SEVERAL_CONT)));
	}
}

static BOOL IsContactChecked(HWND hTree){
	HTREEITEM		hItem;

	hItem = TreeView_GetRoot(hTree);
	while(hItem){
		if(CTreeView_GetCheckBoxState(hTree, hItem)){
			return TRUE;
		}
		hItem = TreeView_GetNextSibling(hTree, hItem);
	}

	return FALSE;
}

static void CreateGroupsContactsList(HWND hTree){
	HTREEITEM		hItem;
	TVITEMW			tvi = {0};
	wchar_t			szBuffer[128];

	tvi.mask = TVIF_TEXT;
	tvi.cchTextMax = 128;
	tvi.pszText = szBuffer;

	hItem = TreeView_GetRoot(hTree);
	while(hItem){
		if(CTreeView_GetCheckBoxState(hTree, hItem)){
			tvi.hItem = hItem;
			SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
			LPPCONTGROUP	lpg = PContGroupsItem(g_PContGroups, szBuffer);
			if(lpg){
				for(LPPCONTACT lpc = g_PContacts; lpc; lpc = lpc->next){
					if(lpc->prop.group == lpg->id){
						lpc->prop.send = TRUE;
					}
				}
			}
		}
		hItem = TreeView_GetNextSibling(hTree, hItem);
	}
}
