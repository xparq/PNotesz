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
#include "multicontacts.h"

static BOOL Multicontacts_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Multicontacts_OnClose(HWND hwnd);
static void Multicontacts_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void ClearDialog(void);
static void PrepareContactsList(HWND hTree);
static void CT_OnCheckStateChanged(HWND hwnd, HTREEITEM hItem, BOOL fChecked);
static BOOL IsContactChecked(HWND hTree);
static void CreateContactsList(HWND hTree);

HIMAGELIST			m_hImlDefCheck;

BOOL CALLBACK Multicontacts_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, Multicontacts_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, Multicontacts_OnInitDialog);
		HANDLE_MSG (hwnd, WM_CLOSE, Multicontacts_OnClose);
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

static void Multicontacts_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify){
	switch(id){
		case IDCANCEL:
			ClearDialog();
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDOK:{
			CreateContactsList(GetDlgItem(hwnd, IDC_LST_SEVERAL_CONT));
			ClearDialog();
			EndDialog(hwnd, IDOK);
			break;
		}
	}
}

static BOOL Multicontacts_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t			szBuffer[256], szKey[16];
	HBITMAP			hBmp;
	HMODULE			hIcons;

	g_hSeveralContacts = hwnd;
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
	
	_itow(IDC_ST_CONTACTS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Contacts", szBuffer, 255, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, L"OK");
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, L"Cancel");

	CTreeView_Subclass(GetDlgItem(hwnd, IDC_LST_SEVERAL_CONT));
	TreeView_SetImageList(GetDlgItem(hwnd, IDC_LST_SEVERAL_CONT), m_hImlDefCheck, TVSIL_NORMAL);
	PrepareContactsList(GetDlgItem(hwnd, IDC_LST_SEVERAL_CONT));

	return TRUE;
}

static void Multicontacts_OnClose(HWND hwnd)
{
	ClearDialog();
	EndDialog(hwnd, IDCANCEL);
}

static void ClearDialog(void){
	g_hSeveralContacts = NULL;
	if(m_hImlDefCheck){
		ImageList_Destroy(m_hImlDefCheck);
		m_hImlDefCheck = NULL;
	}
}

static void PrepareContactsList(HWND hTree){
	TVINSERTSTRUCTW		tvs = {0};
	wchar_t				szBuffer[128];
	LPPCONTACT			lpc;
	HTREEITEM			hItem;

	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_LAST;
	tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.lParam = MAKELONG(1, 1);
	tvs.item.pszText = szBuffer;
	lpc = g_PContacts;
	while(lpc){
		wcscpy(szBuffer, lpc->prop.name);
		hItem = (HTREEITEM)SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
		CTreeView_SetCheckBoxState(hTree, hItem, FALSE);
		lpc = lpc->next;
	}
}

static void CT_OnCheckStateChanged(HWND hwnd, HTREEITEM hItem, BOOL fChecked){
	if(fChecked){
		EnableWindow(GetDlgItem(g_hSeveralContacts, IDOK), TRUE);
	}
	else{
		EnableWindow(GetDlgItem(g_hSeveralContacts, IDOK), IsContactChecked(GetDlgItem(g_hSeveralContacts, IDC_LST_SEVERAL_CONT)));
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

static void CreateContactsList(HWND hTree){
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
			PContactsItem(g_PContacts, szBuffer)->prop.send = TRUE;
		}
		hItem = TreeView_GetNextSibling(hTree, hItem);
	}
}

void ClearMultipleNames(void){
	LPPCONTACT		lpc;

	lpc = g_PContacts;
	while(lpc){
		lpc->prop.send = FALSE;
		lpc = lpc->next;
	}
}
