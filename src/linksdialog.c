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

#include "shared.h"
#include "stringconstants.h"
#include "memorynotes.h"
#include "global.h"
#include "numericconstants.h"
#include "plinks.h"

#define	NOTE_POINTER_PROP		L"NOTE_POINTER_PROP"

static BOOL ChooseLink_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void ChooseLink_OnClose(HWND hwnd);
static void ChooseLink_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void FreeList(HWND hwnd);
static void ApplyLinks(HWND hwnd);

BOOL CALLBACK ChooseLink_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_INITDIALOG, ChooseLink_OnInitDialog);
	HANDLE_MSG (hwnd, WM_CLOSE, ChooseLink_OnClose);
	HANDLE_MSG (hwnd, WM_COMMAND, ChooseLink_OnCommand);

	default: return FALSE;
	}
}

static BOOL ChooseLink_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t			szBuffer[256];
	PMEMNOTE		pNote, pCurrent;
	LPPLINK			pLink;
	int				index;
	
	g_hLinksDlg = hwnd;
	pCurrent = (PMEMNOTE)lParam;
	GetPrivateProfileStringW(S_OPTIONS, L"1042", L"Links to other notes", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	wcscat(szBuffer, L" [");
	wcscat(szBuffer, pCurrent->pData->szName);
	wcscat(szBuffer, L"]");
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	SetDlgCtlText(hwnd, IDC_ST_LINKS_AV, g_NotePaths.CurrLanguagePath, L"Available notes");
	SetDlgCtlText(hwnd, IDC_ST_LINKS_CURR, g_NotePaths.CurrLanguagePath, L"Linked notes");
	
	SetPropW(hwnd, NOTE_POINTER_PROP, (HANDLE)pCurrent);
	//fill available notes
	pNote = MemoryNotes();
	while(pNote){
		if(pNote->pData->idGroup != GROUP_RECYCLE && pNote != (PMEMNOTE)lParam){
			if(!PLinksContains(pCurrent->pLinks, pNote->pFlags->id)){
				index = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_ADDSTRING, 0, (LPARAM)pNote->pData->szName);
				if(index != LB_ERR){
					wchar_t		*p = calloc(28, sizeof(wchar_t));
					if(p){
						wcscpy(p, pNote->pFlags->id);
						SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETITEMDATA, index, (LPARAM)p);
					}
				}
			}
		}
		pNote = pNote->next;
	}
	//fill linked notes
	pLink = pCurrent->pLinks;
	while(pLink){
		pNote = MemNoteById(pLink->id);
		if(pNote){
			index = SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_ADDSTRING, 0, (LPARAM)pNote->pData->szName);
			if(index != LB_ERR){
				wchar_t		*p = calloc(28, sizeof(wchar_t));
				if(p){
					wcscpy(p, pNote->pFlags->id);
					SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_SETITEMDATA, index, (LPARAM)p);
				}
			}
		}
		pLink = pLink->next;
	}
	return FALSE;
}

static void ChooseLink_OnClose(HWND hwnd)
{
	g_hLinksDlg = NULL;
	RemovePropW(hwnd, NOTE_POINTER_PROP);
	FreeList(hwnd);
	EndDialog(hwnd, IDCANCEL);
}

static void ChooseLink_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	int			count, index, data;
	wchar_t		szBuffer[128];

	switch(id){
		case IDOK:
			ApplyLinks(hwnd);
			g_hLinksDlg = NULL;
			RemovePropW(hwnd, NOTE_POINTER_PROP);
			FreeList(hwnd);
			EndDialog(hwnd, IDOK);
			break;
		case IDCANCEL:
			g_hLinksDlg = NULL;
			RemovePropW(hwnd, NOTE_POINTER_PROP);
			FreeList(hwnd);
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDC_LST_PRE_TAGS:
			if(codeNotify == LBN_SELCHANGE){
				if(SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCURSEL, 0, 0) >= 0)
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_PRETOCURR), TRUE);
				else
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_PRETOCURR), FALSE);
			}
			else if(codeNotify == LBN_DBLCLK){
				if(SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCURSEL, 0, 0) >= 0){
					SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_TAGS_PRETOCURR, BN_CLICKED), (LPARAM)GetDlgItem(hwnd, IDC_CMD_TAGS_PRETOCURR));
				}
			}
			break;
		case IDC_LST_CURR_TAGS:
			if(codeNotify == LBN_SELCHANGE){
				if(SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETCURSEL, 0, 0) >= 0)
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_CURRTOPRE), TRUE);
				else
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_CURRTOPRE), FALSE);
			}
			else if(codeNotify == LBN_DBLCLK){
				if(SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETCURSEL, 0, 0) >= 0){
					SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_TAGS_CURRTOPRE, BN_CLICKED), (LPARAM)GetDlgItem(hwnd, IDC_CMD_TAGS_CURRTOPRE));
				}
			}
			break;
		case IDC_CMD_TAGS_PRETOCURR:
			if(codeNotify == BN_CLICKED){
				count = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCOUNT, 0, 0);
				SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_SETSEL, FALSE, -1);
				for(int i = count - 1; i >= 0; i--){
					if(SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETSEL, i, 0) > 0){
						SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETTEXT, i, (LPARAM)szBuffer);
						data = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETITEMDATA, i, 0);
						SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_DELETESTRING, i, 0);
						index = SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_ADDSTRING, 0, (LPARAM)szBuffer);
						SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_SETSEL, TRUE, index);
						SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_SETITEMDATA, index, data);
					}
				}
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_LST_CURR_TAGS, LBN_SELCHANGE), (LPARAM)GetDlgItem(hwnd, IDC_LST_CURR_TAGS));
				SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETSEL, FALSE, -1);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_PRETOCURR), FALSE);
			}
			break;
		case IDC_CMD_TAGS_CURRTOPRE:
			if(codeNotify == BN_CLICKED){
				count = SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETCOUNT, 0, 0);
				SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETSEL, FALSE, -1);
				for(int i = count - 1; i >= 0; i--){
					if(SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETSEL, i, 0) > 0){
						SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETTEXT, i, (LPARAM)szBuffer);
						data = SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETITEMDATA, i, 0);
						SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_DELETESTRING, i, 0);
						index = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_ADDSTRING, 0, (LPARAM)szBuffer);
						SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETSEL, TRUE, index);
						SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_SETITEMDATA, index, data);
					}
				}
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_LST_PRE_TAGS, LBN_SELCHANGE), (LPARAM)GetDlgItem(hwnd, IDC_LST_PRE_TAGS));
				SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_SETSEL, FALSE, -1);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_TAGS_CURRTOPRE), FALSE);
			}
			break;
	}
}

static void ApplyLinks(HWND hwnd){
	int			count, pointer;
	PMEMNOTE	pNote;

	pNote = (PMEMNOTE)GetPropW(hwnd, NOTE_POINTER_PROP);
	if(pNote){
		count = SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETCOUNT, 0, 0);
		if(pNote->pLinks){
			PLinksFree(pNote->pLinks);
			pNote->pLinks = NULL;
		}
		for(int i = 0; i < count; i++){
			pointer = SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETITEMDATA, i, 0);
			if(pointer && pointer != LB_ERR){
				pNote->pLinks = PLinksAdd(pNote->pLinks, (wchar_t *)pointer);
			}
		}
	}
	// int			index = SendDlgItemMessageW(hwnd, IDC_LST_LINKS, LB_GETCURSEL, 0, 0);

	// if(index >= 0){
		// pointer = SendDlgItemMessageW(hwnd, IDC_LST_LINKS, LB_GETITEMDATA, index, 0);
		// if(pointer && pointer != LB_ERR){
			// wcscpy(g_TempLink, (wchar_t *)pointer);
		// }
	// }
}

static void FreeList(HWND hwnd){
	int			count, pointer;

	count = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETCOUNT, 0, 0);
	for(int i = 0; i < count; i++){
		pointer = SendDlgItemMessageW(hwnd, IDC_LST_PRE_TAGS, LB_GETITEMDATA, i, 0);
		if(pointer && pointer != LB_ERR){
			free((void *)pointer);
		}
	}
	count = SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETCOUNT, 0, 0);
	for(int i = 0; i < count; i++){
		pointer = SendDlgItemMessageW(hwnd, IDC_LST_CURR_TAGS, LB_GETITEMDATA, i, 0);
		if(pointer && pointer != LB_ERR){
			free((void *)pointer);
		}
	}
}

