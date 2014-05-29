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
#include "numericconstants.h"
#include "stringconstants.h"
#include "shared.h"
#include "memorynotes.h"

static BOOL Search_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Search_OnDestroy(HWND hwnd);
static void Search_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void ApplySearchLanguage(HWND hwnd);
static void PrepareResultsList(HWND hwnd);
static void FindTags(HWND hwnd);

static HIMAGELIST		m_hImlSearch = NULL;
static wchar_t			*m_cols[] = {L"Note", L"Tags"};

BOOL CALLBACK SearchTags_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_INITDIALOG, Search_OnInitDialog);
		HANDLE_MSG (hwnd, WM_COMMAND, Search_OnCommand);
		HANDLE_MSG (hwnd, WM_DESTROY, Search_OnDestroy);
		case WM_NOTIFY:{
			NMHDR *lpnm = (NMHDR *)lParam;
			if(lpnm->code == NM_DBLCLK && lpnm->idFrom == IDC_LST_RESULTS){
				HWND			hList = GetDlgItem(hwnd, IDC_LST_RESULTS);
				PMEMNOTE		pNote;
				NMITEMACTIVATE *lpi = (NMITEMACTIVATE *)lParam;
				LVITEMW			lvi;
				BOOL			fVisible;

				ZeroMemory(&lvi, sizeof(lvi));
				lvi.mask = LVIF_PARAM;
				lvi.iItem = lpi->iItem;
				SendMessageW(hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
				pNote = (PMEMNOTE)lvi.lParam;
				if(pNote->pData->idGroup == GROUP_RECYCLE){
					return TRUE;
				}
				fVisible = pNote->pData->visible;
				if(!fVisible){
					if(UnlockGroup(pNote->pData->idGroup) && UnlockNote(pNote)){
						ShowNote(pNote);
						SendMessageW(hwnd, PNM_SHOW_AFTER_SEARCH, (WPARAM)pNote, 0);
						if(g_hCPDialog)
							SendMessageW(g_hCPDialog, PNM_CTRL_UPD, 0, (LPARAM)pNote);
					}
				}
				else{
					ShowNote(pNote);
				}
			}
			return TRUE;
		}
		case PNM_SHOW_AFTER_SEARCH:{
			HWND	hList = GetDlgItem(hwnd, IDC_LST_RESULTS);
			LVITEMW	lv;
			int		count;

			ZeroMemory(&lv, sizeof(lv));
			lv.mask = LVIF_PARAM | LVIF_IMAGE;
			count = ListView_GetItemCount(hList);
			for(int i = 0; i < count; i++){
				lv.iItem = i;
				SendMessageW(hList, LVM_GETITEMW, 0, (LPARAM)&lv);
				if(lv.lParam == wParam){
					lv.iImage = ItemImageIndex((PMEMNOTE)wParam);
					SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lv);
				}
			}
			return TRUE;
		}
		case PNM_BECOMES_HIDDEN:{
			HWND	hList = GetDlgItem(hwnd, IDC_LST_RESULTS);
			LVITEMW	lv;
			int		count;

			ZeroMemory(&lv, sizeof(lv));
			lv.mask = LVIF_PARAM | LVIF_IMAGE;
			count = ListView_GetItemCount(hList);
			for(int i = 0; i < count; i++){
				lv.iItem = i;
				SendMessageW(hList, LVM_GETITEMW, 0, (LPARAM)&lv);
				if(lv.lParam == wParam){
					lv.iImage = ItemImageIndex((PMEMNOTE)wParam);
					SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lv);
				}
			}
			return TRUE;
		}
		case PNM_CTRL_UPD_LANG:
			ApplySearchLanguage(hwnd);
			return TRUE;
		default: return FALSE;
	}
}

static BOOL Search_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	LPPTAG			pTemp;

	g_hSearchTags = hwnd;
	CenterWindow(hwnd, FALSE);
	PrepareResultsList(hwnd);
	ApplySearchLanguage(hwnd);
	pTemp = g_PTagsPredefined;
	while(pTemp){
		SendDlgItemMessageW(hwnd, IDC_CBO_AVAILABLE_TAGS, CB_ADDSTRING, 0, (LPARAM)pTemp->text);
		pTemp = pTemp->next;
	}
	return FALSE;
}

static void Search_OnDestroy(HWND hwnd)
{
	if(m_hImlSearch){
		ImageList_Destroy(m_hImlSearch);
		m_hImlSearch = NULL;
	}
	g_hSearchTags = NULL;
}

static void Search_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDOK:
			if(IsWindowEnabled(GetDlgItem(hwnd, IDC_CMD_FIND))){
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_FIND, BN_CLICKED), (LPARAM)GetDlgItem(hwnd, IDC_CMD_FIND));
			}
			break;
		case IDCANCEL:
			DestroyWindow(hwnd);
			break;
		case IDC_EDT_TAGS:{
			if(codeNotify == EN_CHANGE){
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), (BOOL)GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_TAGS)));
			}
			break;
		}
		case IDC_CMD_FIND:{
			FindTags(hwnd);
			break;
		}
		case IDC_CBO_AVAILABLE_TAGS:
			if(codeNotify == CBN_SELENDOK){
				int		index = SendDlgItemMessageW(hwnd, IDC_CBO_AVAILABLE_TAGS, CB_GETCURSEL, 0, 0);
				if(index != CB_ERR){
					wchar_t		szBuffer[4096], szText[256];
					SendDlgItemMessageW(hwnd, IDC_CBO_AVAILABLE_TAGS, CB_GETLBTEXT, index, (LPARAM)szText);
					SendDlgItemMessageW(hwnd, IDC_EDT_TAGS, WM_GETTEXT, 4096, (LPARAM)szBuffer);
					_wcstrm(szBuffer);
					if(wcslen(szBuffer) > 0)
						wcscat(szBuffer, L",");
					wcscat(szBuffer, szText);
					SendDlgItemMessageW(hwnd, IDC_EDT_TAGS, WM_SETTEXT, 0, (LPARAM)szBuffer);
				}
			}
			break;
	}
}

static void PrepareResultsList(HWND hwnd){
	HWND		hList;
	LVCOLUMNW	lvc;
	HBITMAP		hBmp;
	HMODULE		hIcons;

	hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hIcons){
		//prepare and set list view image lists
		hBmp = LoadBitmapW(hIcons, MAKEINTRESOURCEW(IDB_CTRL_SMALL));
		m_hImlSearch = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 1);
		ImageList_AddMasked(m_hImlSearch, hBmp, CLR_MASK);
		DeleteBitmap(hBmp);
		DeleteObject((void *)CLR_MASK);
		FreeLibrary(hIcons);
	}
	hList = GetDlgItem(hwnd, IDC_LST_RESULTS);
	ListView_SetImageList(hList, m_hImlSearch, LVSIL_SMALL);
	SendMessageW(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES, LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES);
	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	for(int i = 0; i < NELEMS(m_cols); i++){
		if(i > 0){
			lvc.cx = 240;
		}
		else{
			lvc.cx = 180;
		}
		lvc.iSubItem = i;
		lvc.pszText = m_cols[i];
		SendMessageW(hList, LVM_INSERTCOLUMNW, i, (LPARAM)&lvc);
	}
}

static void ApplySearchLanguage(HWND hwnd){
	wchar_t			szBuffer[256], szKey[16];
	LVCOLUMNW		lvc;

	_itow(IDM_SEARCH_BY_TAGS, szKey, 10);
	GetPrivateProfileStringW(S_MENU, szKey, L"Search By Tags", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDC_ST_PRE_TAGS, g_NotePaths.CurrLanguagePath, L"Available tags");
	SetDlgCtlText(hwnd, IDC_ST_RESULTS, g_NotePaths.CurrLanguagePath, L"Search results");
	SetDlgCtlText(hwnd, IDC_CMD_FIND, g_NotePaths.CurrLanguagePath, L"Find");
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, L"Cancel");
	SetDlgCtlText(hwnd, IDC_ST_TAGS, g_NotePaths.CurrLanguagePath, L"Tags separated by commas (e.g. \"one,two,three\")");
	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.cchTextMax = 256;
	lvc.pszText = szBuffer;
	SendDlgItemMessageW(hwnd, IDC_LST_RESULTS, LVM_GETCOLUMNW, 0, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"0", m_cols[0], szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_LST_RESULTS, LVM_SETCOLUMNW, 0, (LPARAM)&lvc);
	SendDlgItemMessageW(hwnd, IDC_LST_RESULTS, LVM_GETCOLUMNW, 1, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, L"7", m_cols[1], szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_LST_RESULTS, LVM_SETCOLUMNW, 1, (LPARAM)&lvc);
}

static void FindTags(HWND hwnd){
	PMEMNOTE		pNote = MemoryNotes();
	wchar_t			*pText, *pSub, *pt, *w, *ptr;
	LPPTAG			p1, p2, p3, pTags = NULL;
	int				len, count, index = 0;
	LVITEMW			lvi = {0}, lvsub = {0};

	ListView_DeleteAllItems(GetDlgItem(hwnd, IDC_LST_RESULTS));
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvsub.mask = LVIF_TEXT;
	lvsub.iSubItem = 1;

	len = GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_TAGS));
	pText = calloc(len + 2, sizeof(wchar_t));
	if(pText){
		GetDlgItemTextW(hwnd, IDC_EDT_TAGS, pText, len + 1);
		RemoveDoubleComma(pText);
		
		w = wcstok(pText, L",", &ptr);
		while(w){
			pt = _wcsdup(w);
			_wcstrm(pt);
			if(*pt){
				pTags = TagsAdd(pTags, pt, NULL);
			}
			free(pt);
			w = wcstok(NULL, L",", &ptr);
		}

		free(pText);

		while(pNote){
			if(pNote->pData->idGroup != GROUP_RECYCLE){
				count = TagsCount(pNote->pTags);
				if(count == 0)
					goto _next;
				p1 = pNote->pTags;
				while(p1){
					p2 = pTags;
					while(p2){
						if(_wcsicmp(p2->text, p1->text) == 0){
							lvi.iItem = index;
							lvi.lParam = (int)pNote;
							lvi.pszText = pNote->pData->szName;
							lvi.iImage = ItemImageIndex(pNote);
							SendDlgItemMessageW(hwnd, IDC_LST_RESULTS, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
							pSub = calloc(count * 128, sizeof(wchar_t));
							if(pSub){
								lvsub.iItem = index;
								lvsub.pszText = pSub;
								p3 = pNote->pTags;
								while(p3){
									wcscat(pSub, p3->text);
									wcscat(pSub, L",");
									p3 = p3->next;
								}
								if(*pSub){
									wchar_t		*p = wcsrchr(pSub, ',');
									if(p)
										*p = '\0';
								}
								SendDlgItemMessageW(hwnd, IDC_LST_RESULTS, LVM_SETITEMW, 0, (LPARAM)&lvsub);
								index++;
								free(pSub);
								goto _next;
							}
						}
						p2 = p2->next;
					}
					p1 = p1->next;
				}
			}
			_next:
			pNote = pNote->next;
		}
		TagsFree(pTags);
	}
}
