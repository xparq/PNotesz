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

#include <math.h>

#include "notes.h"
#include "global.h"
#include "numericconstants.h"
#include "stringconstants.h"
#include "shared.h"
#include "memorynotes.h"
#include "note.h"

#define	SEARCH_AND				0
#define	SEARCH_OR				1

#define	MATCH_BY_TEXT			1
#define	MATCH_BY_TITLE			2
#define	MATCH_BY_TEXT_AND_TITLE	4

typedef struct _SEARCH_MATCHES {
	wchar_t				matchText[128];
	wchar_t				matchTitle[128];
	wchar_t				matchBoth[128];
}SEARCH_MATCHES;

static BOOL Search_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Search_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Search_OnDestroy(HWND hwnd);
static void ClearTreeView(HWND hwnd);
static void FindProcRegular(HWND hwnd);
static void FindProcANDOR(HWND hList, int searchType);
static void ReplaceProc(void);
static void PrepareResultsList(HWND hwnd);
static void ApplySearchLanguage(HWND hwnd);
static int CountSpaceDelimitedItems(wchar_t * s);
static int CountFoundByTree(HWND hwnd);
static HTREEITEM InsertFoundParent(LPTVINSERTSTRUCTW lptvs, HWND hTree, PMEMNOTE pNote);
static void InsertFoundChild(LPTVINSERTSTRUCTW lptvs, HWND hEdit, HWND hTree, HTREEITEM hItem);
static LRESULT CALLBACK Tree_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static FINDTEXTEXW		m_ft;
static int				m_flags, m_count;
static wchar_t			*m_cols[] = {L"Note", L"Line", L"Column"};
static HIMAGELIST		m_hImlSearch = NULL;
static BOOL				m_hidden;
static wchar_t			m_Line[256], m_Col[256];
static SEARCH_MATCHES	m_SearchMatches;
static int				m_MatchType;

static LRESULT CALLBACK Tree_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg){
		case WM_LBUTTONDBLCLK:{
			PMEMNOTE		pNote;
			HTREEITEM		hItem, hParent;
			TVITEMW			tvi = {0};
			BOOL			fVisible;

			hItem = TreeView_GetSelection(hwnd);
			hParent = TreeView_GetParent(hwnd, hItem);
			if(hParent)
				return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
			else{
				//show note
				tvi.mask = TVIF_PARAM;
				tvi.hItem = hItem;
				TreeView_GetItem(hwnd, &tvi);
				if(tvi.lParam){
					pNote = (PMEMNOTE)tvi.lParam;
					if(pNote->pData->idGroup == GROUP_RECYCLE){
						return FALSE;
					}
					fVisible = pNote->pData->visible;
					if(!fVisible){
						if(UnlockGroup(pNote->pData->idGroup) && UnlockNote(pNote)){
							ShowNote(pNote);
							SendMessageW(GetParent(hwnd), PNM_SHOW_AFTER_SEARCH, (WPARAM)pNote, 0);
							if(g_hCPDialog)
								SendMessageW(g_hCPDialog, PNM_CTRL_UPD, 0, (LPARAM)pNote);
						}
					}
					else{
						ShowNote(pNote);
					}
				}
				//prevent parent item from being collapsed on double click
				return FALSE;
			}
		}
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
	}
}

static void ApplySearchLanguage(HWND hwnd){
	wchar_t			szBuffer[256], szKey[16];
	int				index;

	_itow(IDM_SEARCH_IN_NOTES, szKey, 10);
	GetPrivateProfileStringW(S_MENU, szKey, L"Search In Notes", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDC_ST_SEARCH, g_NotePaths.CurrLanguagePath, L"Find what:");
	SetDlgCtlText(hwnd, IDC_ST_RESULTS, g_NotePaths.CurrLanguagePath, L"Search results");
	SetDlgCtlText(hwnd, IDC_CHK_MATCH_CASE, g_NotePaths.CurrLanguagePath, L"Match case");
	SetDlgCtlText(hwnd, IDC_CHK_WHOLE_WORD, g_NotePaths.CurrLanguagePath, L"Whole word");
	SetDlgCtlText(hwnd, IDC_CMD_FIND, g_NotePaths.CurrLanguagePath, L"Find");
	SetDlgCtlText(hwnd, IDC_SRCH_IN_HIDDEN, g_NotePaths.CurrLanguagePath, L"Include hidden notes");
	SetDlgCtlText(hwnd, IDC_SRCH_HIST_CLEAR, g_NotePaths.CurrLanguagePath, L"Clear search history");
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, L"Cancel");
	SetDlgCtlText(hwnd, IDC_ST_REPLACE, g_NotePaths.CurrLanguagePath, L"Replace with:");
	SetDlgCtlText(hwnd, IDC_CMD_REPLACE_ALL, g_NotePaths.CurrLanguagePath, L"Replace All");
	SetDlgCtlText(hwnd, IDC_ST_SRCH_CRITERIA, g_NotePaths.CurrLanguagePath, L"Search criteria");
	SetDlgCtlText(hwnd, IDC_ST_SRCH_SCOPE, g_NotePaths.CurrLanguagePath, L"Search scope");

	for(int i = 0; i < NELEMS(m_cols); i++){
		_itow(i, szKey, 10);
		GetPrivateProfileStringW(S_RESULTS_COLS, szKey, m_cols[i], szBuffer, 256, g_NotePaths.CurrLanguagePath);
		if(i == 1)
			wcscpy(m_Line, szBuffer);
		else if(i == 2)
			wcscpy(m_Col, szBuffer);
	}
	//search criteria
	index = SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_CRITERIA, CB_GETCURSEL, 0, 0);
	if(index < 0)
		index = 0;
	SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_CRITERIA, CB_RESETCONTENT, 0, 0);
	GetPrivateProfileStringW(L"search_criteria", L"0", L"Match entirely search string", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_CRITERIA, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	GetPrivateProfileStringW(L"search_criteria", L"1", L"Match every word in search string", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_CRITERIA, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	GetPrivateProfileStringW(L"search_criteria", L"2", L"Match at least one word in search string", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_CRITERIA, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_CRITERIA, CB_SETCURSEL, index, 0);
	//serach scope
	index = SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_SCOPE, CB_GETCURSEL, 0, 0);
	if(index < 0)
		index = 0;
	SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_SCOPE, CB_RESETCONTENT, 0, 0);
	GetPrivateProfileStringW(L"search_scope", L"0", L"Search in text", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_SCOPE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	GetPrivateProfileStringW(L"search_scope", L"1", L"Search in titles", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_SCOPE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	GetPrivateProfileStringW(L"search_scope", L"2", L"Search in text and titles", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_SCOPE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_SCOPE, CB_SETCURSEL, index, 0);
	//search matches
	GetPrivateProfileStringW(L"search_match", L"0", L"(text only)", m_SearchMatches.matchText, 128, g_NotePaths.CurrLanguagePath);
	GetPrivateProfileStringW(L"search_match", L"1", L"(title only)", m_SearchMatches.matchTitle, 128, g_NotePaths.CurrLanguagePath);
	GetPrivateProfileStringW(L"search_match", L"2", L"(text and title)", m_SearchMatches.matchBoth, 128, g_NotePaths.CurrLanguagePath);
}

static void PrepareResultsList(HWND hwnd){
	HWND		hTree;
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
	hTree = GetDlgItem(hwnd, IDC_TVW_SEARCH);
	SetWindowLongPtrW(hTree, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(hTree, GWLP_WNDPROC, (LONG_PTR)Tree_Proc));
	TreeView_SetImageList(hTree, m_hImlSearch, TVSIL_NORMAL);
}

BOOL CALLBACK Search_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_INITDIALOG, Search_OnInitDialog);
		HANDLE_MSG (hwnd, WM_COMMAND, Search_OnCommand);
		HANDLE_MSG (hwnd, WM_DESTROY, Search_OnDestroy);
		case WM_NOTIFY:{
			NMHDR *lpnm = (NMHDR *)lParam;
			if(lpnm->code == NM_DBLCLK && lpnm->idFrom == IDC_TVW_SEARCH){
				HWND			hTree = GetDlgItem(hwnd, IDC_TVW_SEARCH);
				PMEMNOTE		pNote;
				HTREEITEM		hItem, hParent;
				TVITEMW			tvi = {0};
				CHARRANGE		chr;
				BOOL			fVisible;

				hItem = TreeView_GetSelection(hTree);
				hParent = TreeView_GetParent(hTree, hItem);
				if(hParent){
					tvi.mask = TVIF_PARAM;
					tvi.hItem = hParent;
					TreeView_GetItem(hTree, &tvi);
					if(tvi.lParam){
						pNote = (PMEMNOTE)tvi.lParam;
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
							else{
								return TRUE;
							}
						}
						else{
							ShowNote(pNote);
						}
						tvi.hItem = hItem;
						TreeView_GetItem(hTree, &tvi);
						if(tvi.lParam){
							memcpy(&chr, (CHARRANGE *)tvi.lParam, sizeof(CHARRANGE));
							SendMessageW((HWND)GetPropW(pNote->hwnd, PH_EDIT), EM_EXSETSEL, 0, (LPARAM)&chr);
						}
					}
				}
			}
			return TRUE;
		}
		case PNM_SHOW_AFTER_SEARCH:{
			HWND			hTree = GetDlgItem(hwnd, IDC_TVW_SEARCH);
			TVITEMW			tvi = {0};
			HTREEITEM		hItem;

			tvi.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			hItem = TreeView_GetRoot(hTree);
			while(hItem){
				tvi.hItem = hItem;
				TreeView_GetItem(hTree, &tvi);
				if(tvi.lParam == wParam){
					tvi.iImage = ItemImageIndex((PMEMNOTE)wParam);
					tvi.iSelectedImage = tvi.iImage;
					TreeView_SetItem(hTree, &tvi);
					return TRUE;
				}
				hItem = TreeView_GetNextSibling(hTree, hItem);
			}
			return TRUE;
		}
		case PNM_BECOMES_HIDDEN:{
			HWND			hTree = GetDlgItem(hwnd, IDC_TVW_SEARCH);
			TVITEMW			tvi = {0};
			HTREEITEM		hItem;

			tvi.mask = TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			hItem = TreeView_GetRoot(hTree);
			while(hItem){
				tvi.hItem = hItem;
				TreeView_GetItem(hTree, &tvi);
				if(tvi.lParam == wParam){
					tvi.iImage = ItemImageIndex((PMEMNOTE)wParam);
					tvi.iSelectedImage = tvi.iImage;
					TreeView_SetItem(hTree, &tvi);
					return TRUE;
				}
				hItem = TreeView_GetNextSibling(hTree, hItem);
			}
			return TRUE;
		}
		case PNM_CTRL_UPD_LANG:
			ApplySearchLanguage(hwnd);
			return TRUE;
		default: return FALSE;
	}
}

static void Search_OnDestroy(HWND hwnd)
{
	ClearTreeView(hwnd);
	if(m_hImlSearch){
		ImageList_Destroy(m_hImlSearch);
		m_hImlSearch = NULL;
	}
	g_hSearchDialog = NULL;
}

static void Search_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDC_CBO_SEARCH:
			if(codeNotify == CBN_EDITCHANGE){
				int len = SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, WM_GETTEXTLENGTH, 0, 0);
				if(len){
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), TRUE);
					len = SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, WM_GETTEXTLENGTH, 0, 0);
					if(len){
						int	index = SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_SCOPE, CB_GETCURSEL, 0, 0);
						if(index == 0)
							EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), TRUE);
						else
							EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
					}
					else{
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
					}
				}
				else{
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
				}
			}
			else if(codeNotify == CBN_SELENDOK){
				if(SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_GETCURSEL, 0, 0) != CB_ERR){
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), TRUE);
					int len = SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, WM_GETTEXTLENGTH, 0, 0);
					if(len){
						int	index = SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_SCOPE, CB_GETCURSEL, 0, 0);
						if(index == 0)
							EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), TRUE);
						else
							EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
					}
					else{
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
					}
				}
				else{
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
				}
			}
			break;
		case IDC_CBO_REPLACE:
			if(codeNotify == CBN_EDITCHANGE){
				int len = SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, WM_GETTEXTLENGTH, 0, 0);
				if(len){
					int	index = SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_SCOPE, CB_GETCURSEL, 0, 0);
					if(index == 0)
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), TRUE);
					else
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
				}
				else{
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
				}
			}
			else if(codeNotify == CBN_SELENDOK){
				if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_GETCURSEL, 0, 0) != CB_ERR){
					int	index = SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_SCOPE, CB_GETCURSEL, 0, 0);
					if(index == 0)
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), TRUE);
					else
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
				}
				else{
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
				}
			}
			break;
		case IDC_SRCH_HIST_CLEAR:
			if(codeNotify == BN_CLICKED){
				WritePrivateProfileSectionW(S_SEARCH_HIST, NULL, g_NotePaths.INIFile);
				SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_RESETCONTENT, 0, 0);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), FALSE);
				WritePrivateProfileSectionW(S_REPLACE_HIST, NULL, g_NotePaths.INIFile);
				SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_RESETCONTENT, 0, 0);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
			}
			break;
		case IDC_CBO_SRCH_SCOPE:
			if(codeNotify == CBN_SELENDOK){
				int	index = SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_SCOPE, CB_GETCURSEL, 0, 0);
				if(index != CB_ERR){
					m_MatchType = (int)pow(2, index);
					if(m_MatchType == MATCH_BY_TEXT){
						int len = SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, WM_GETTEXTLENGTH, 0, 0);
						if(len){
							EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), TRUE);
						}
						else{
							EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
						}
					}
					else{
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
					}
				}
			}
			break;
		case IDOK:
			if(IsWindowEnabled(GetDlgItem(hwnd, IDC_CMD_FIND))){
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_FIND, BN_CLICKED), (LPARAM)GetDlgItem(hwnd, IDC_CMD_FIND));
			}
			break;
		case IDCANCEL:
			DestroyWindow(hwnd);
			break;
		case IDC_CMD_REPLACE_ALL:{
			wchar_t		szBuffer[256], szMessage[1024], szTemp[12];
			int			res, index;

			index = SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_CRITERIA, CB_GETCURSEL, 0, 0);
			if(index > 0){
				wcscpy(szMessage, g_Strings.ReplaceWarning1);
				wcscat(szMessage, L" \"");
				SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_CRITERIA, CB_GETLBTEXT, 0, (LPARAM)szBuffer);
				wcscat(szMessage, szBuffer);
				wcscat(szMessage, L"\"\n");
				wcscat(szMessage, g_Strings.ReplaceWarning2);
				wcscat(szMessage, L" \"");
				SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_CRITERIA, CB_GETLBTEXT, index, (LPARAM)szBuffer);
				wcscat(szMessage, szBuffer);
				wcscat(szMessage, L"\"\n");
				wcscat(szMessage, g_Strings.ContinueQuestion);
				if(MessageBoxW(hwnd, szMessage, PROG_NAME, MB_YESNO | MB_ICONQUESTION) == IDNO){
					break;
				}
			}
			m_count = 0;
			SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, WM_GETTEXT, 256, (LPARAM)szBuffer);
			if(SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_FINDSTRINGEXACT, -1, (LPARAM)szBuffer) == CB_ERR){
				SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_INSERTSTRING, 0, (LPARAM)szBuffer);
				SaveSearchHistory(hwnd);
			}
			SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, WM_GETTEXT, 256, (LPARAM)szBuffer);
			if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_FINDSTRINGEXACT, -1, (LPARAM)szBuffer) == CB_ERR){
				SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_INSERTSTRING, 0, (LPARAM)szBuffer);
				SaveReplaceHistory(hwnd);
			}
			//perform serach and replace
			res = IsDlgButtonChecked(hwnd, IDC_SRCH_IN_HIDDEN);
			if(res == BST_CHECKED)
				m_hidden = TRUE;
			else
				m_hidden = FALSE;
			ZeroMemory(&m_ft, sizeof(m_ft));
			m_flags = FR_DOWN;
			
			res = IsDlgButtonChecked(hwnd, IDC_CHK_MATCH_CASE);
			if(res == BST_CHECKED)
				m_flags |= FR_MATCHCASE;
			res = IsDlgButtonChecked(hwnd, IDC_CHK_WHOLE_WORD);
			if(res == BST_CHECKED)
				m_flags |= FR_WHOLEWORD;
			m_ft.lpstrText = g_SearchString;
			GetDlgItemTextW(hwnd, IDC_CBO_SEARCH, g_SearchString, 255);
			GetDlgItemTextW(hwnd, IDC_CBO_REPLACE, g_ReplaceString, 255);
			ReplaceProc();
			ClearTreeView(hwnd);
			GetWindowTextW(hwnd, szBuffer, 255);
			wcscpy(szMessage, g_Strings.ReplaceComplete);
			wcscat(szMessage, L"\n");
			if(m_count > 0){
				wcscat(szMessage, g_Strings.MatchesFound);
				wcscat(szMessage, L" ");
				_itow(m_count, szTemp, 10);
				wcscat(szMessage, szTemp);
			}
			else
				wcscat(szMessage, g_Strings.NoMatchesFound);
			MessageBoxW(hwnd, szMessage, szBuffer, MB_OK | MB_ICONINFORMATION);
			break;
		}
		case IDC_CMD_FIND:{
			wchar_t		szBuffer[256], szMessage[512], szTemp[12];
			int			res, condition;

			m_count = 0;
			condition = SendDlgItemMessageW(hwnd, IDC_CBO_SRCH_CRITERIA, CB_GETCURSEL, 0, 0);
			SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, WM_GETTEXT, 256, (LPARAM)szBuffer);
			if(SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_FINDSTRINGEXACT, -1, (LPARAM)szBuffer) == CB_ERR){
				SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_INSERTSTRING, 0, (LPARAM)szBuffer);
				SaveSearchHistory(hwnd);
			}
			//perform search
			res = IsDlgButtonChecked(hwnd, IDC_SRCH_IN_HIDDEN);
			if(res == BST_CHECKED)
				m_hidden = TRUE;
			else
				m_hidden = FALSE;
			ZeroMemory(&m_ft, sizeof(m_ft));
			m_flags = FR_DOWN;
			
			res = IsDlgButtonChecked(hwnd, IDC_CHK_MATCH_CASE);
			if(res == BST_CHECKED)
				m_flags |= FR_MATCHCASE;
			res = IsDlgButtonChecked(hwnd, IDC_CHK_WHOLE_WORD);
			if(res == BST_CHECKED)
				m_flags |= FR_WHOLEWORD;
			switch(condition){
				case 0:		//regular search - whole search string
					m_ft.lpstrText = g_SearchString;
					GetDlgItemTextW(hwnd, IDC_CBO_SEARCH, g_SearchString, 255);
					FindProcRegular(hwnd);
					break;
				case 1:		//AND search - every word from search string
					GetDlgItemTextW(hwnd, IDC_CBO_SEARCH, g_SearchString, 255);
					FindProcANDOR(hwnd, SEARCH_AND);
					break;
				case 2:		//OR search - at least one word from search string
					GetDlgItemTextW(hwnd, IDC_CBO_SEARCH, g_SearchString, 255);
					FindProcANDOR(hwnd, SEARCH_OR);
					break;
			}
			m_count = CountFoundByTree(hwnd);
			//EnumThreadWindows(GetCurrentThreadId(), FindProcRegular, (LPARAM)hwnd);
			GetWindowTextW(hwnd, szBuffer, 255);
			wcscpy(szMessage, g_Strings.SearchComplete);
			wcscat(szMessage, L"\n");
			if(m_count > 0){
				wcscat(szMessage, g_Strings.MatchesFound);
				wcscat(szMessage, L" ");
				_itow(m_count, szTemp, 10);
				wcscat(szMessage, szTemp);
			}
			else
				wcscat(szMessage, g_Strings.NoMatchesFound);
			MessageBoxW(hwnd, szMessage, szBuffer, MB_OK | MB_ICONINFORMATION);
			break;
		}
	}
}

static BOOL Search_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	g_hSearchDialog = hwnd;
	CenterWindow(hwnd, FALSE);
	PrepareResultsList(hwnd);
	ApplySearchLanguage(hwnd);
	CheckDlgButton(hwnd, IDC_SRCH_IN_HIDDEN, BST_CHECKED);
	//limit combo box
	SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_LIMITTEXT, 255, 0);
	SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_LIMITTEXT, 255, 0);
	//load search history
	LoadSearchHistory(hwnd);
	//load replace history
	LoadReplaceHistory(hwnd);
	m_MatchType = MATCH_BY_TEXT;
	SetFocus(GetDlgItem(hwnd, IDC_CBO_SEARCH));
	return FALSE;
}

static void ReplaceProc(void){
	PMEMNOTE		pNote = MemoryNotes();
	HWND			hEdit;
	int				result;
	
	while(pNote){
		hEdit = NULL;
		if(!pNote->pData->visible && m_hidden){
			hEdit = GetUnvisibleNoteEdit(pNote);
		}
		else{
			hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
		}
		if(hEdit){
			result = 0;
			m_ft.chrg.cpMin = 0;
			m_ft.chrg.cpMax = -1;
			while(result != -1){
				result = SendMessageW(hEdit, EM_FINDTEXTEXW, m_flags, (LPARAM)&m_ft);
				if(result != -1){
					SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&m_ft.chrgText);
					SendMessageW(hEdit, EM_REPLACESEL, TRUE, (LPARAM)g_ReplaceString);
					m_ft.chrg.cpMin = m_ft.chrgText.cpMin + wcslen(g_ReplaceString);
					// m_ft.chrg.cpMin = m_ft.chrgText.cpMax;
					m_ft.chrg.cpMax = -1;

					m_count++;
				}
			}
			if(!pNote->pData->visible){
				SendMessageW(hEdit, WM_DESTROY, 0, 0);
			}
		}
		pNote = pNote->next;
	}
}

static int CountSpaceDelimitedItems(wchar_t * s){
	int				result = 0;

	while(*s){
		if(*s == ' '){
			result++;
		}
		s++;
	}
	return ++result;
}

static int CountFoundByTree(HWND hwnd){
	HWND			hTree = GetDlgItem(hwnd, IDC_TVW_SEARCH);
	HTREEITEM		hParent, hChild;
	int				result = 0;
	TVITEMW			tvi = {0};
	wchar_t			szTemp[256];

	tvi.mask = TVIF_TEXT;
	tvi.pszText = szTemp;
	tvi.cchTextMax = 256;

	hParent = TreeView_GetRoot(hTree);
	while(hParent){
		hChild = TreeView_GetChild(hTree, hParent);
		if(hChild){
			while(hChild){
				result++;
				hChild = TreeView_GetNextSibling(hTree, hChild);
			}
			tvi.hItem = hParent;
			SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
			if(_wcsistr(szTemp, m_SearchMatches.matchBoth))
				result++;
		}
		else{
			//in case of found title only
			result++;
		}
		hParent = TreeView_GetNextSibling(hTree, hParent);
	}
	return result;
}

static void FindProcANDOR(HWND hwnd, int searchType){
	wchar_t			*pSearch, *pTemp, *w, *ptr, szTemp[256], *titleResult;
	PMEMNOTE		pNote = MemoryNotes();
	HWND			hEdit, hTree;
	int				result, count, index;
	TVINSERTSTRUCTW	tvs = {0};
	HTREEITEM		hItem;
	BOOL			*pFoundText, *pFoundTitle, findResult;
	int				matched;
	BOOL			wholeWord;

	pSearch = calloc(wcslen(g_SearchString) + 2, sizeof(wchar_t));
	if(!pSearch)
		return;
	wcscpy(pSearch, g_SearchString);
	RemoveDoubleSpace(pSearch);
	pTemp = _wcsdup(pSearch);

	count = CountSpaceDelimitedItems(pSearch);

	hTree = GetDlgItem(hwnd, IDC_TVW_SEARCH);
	ClearTreeView(hwnd);

	tvs.item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_TEXT | TVIF_SELECTEDIMAGE;
	tvs.hInsertAfter = TVI_LAST;
	while(pNote){
		hEdit = NULL;
		if(!pNote->pData->visible && m_hidden){
			hEdit = GetUnvisibleNoteEdit(pNote);
		}
		else{
			hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
		}
		hItem = NULL;
		if(hEdit){
			pFoundText = calloc(count, sizeof(BOOL));
			pFoundTitle = calloc(count, sizeof(BOOL));
			if(!pFoundText || !pFoundTitle){
				if(pSearch)
					free(pSearch);
				if(pFoundText)
					free(pFoundText);
				if(pFoundTitle)
					free(pFoundTitle);
				return;
			}
			matched = 0;
			index = 0;
			wcscpy(pSearch, pTemp);

			if(searchType == SEARCH_AND)
				findResult = TRUE;
			else
				findResult = FALSE;
			w = wcstok(pSearch, L" ", &ptr);
			while(w){
				m_ft.lpstrText = w;
				result = 0;
				wholeWord = TRUE;
				m_ft.chrg.cpMin = 0;
				m_ft.chrg.cpMax = -1;
				//check title
				if(m_MatchType == MATCH_BY_TEXT_AND_TITLE || m_MatchType == MATCH_BY_TITLE){
					if((m_flags & FR_MATCHCASE) != FR_MATCHCASE)
						titleResult = _wcsistr(pNote->pData->szName, m_ft.lpstrText);
					else
						titleResult = wcsstr(pNote->pData->szName, m_ft.lpstrText);
					if(titleResult){
						if((m_flags & FR_WHOLEWORD) == FR_WHOLEWORD){
							//initially set wholeWord to FALSE
							wholeWord = FALSE;
							if(titleResult == pNote->pData->szName){
								//if string has been found at the beginning of title
								if(wcslen(m_ft.lpstrText) == wcslen(pNote->pData->szName))
									//if the title is exactly as string
									wholeWord = TRUE;
								else if(*(pNote->pData->szName + wcslen(m_ft.lpstrText)) == ' ')
									//if the next character after found string in title is space
									wholeWord = TRUE;
							}
							else{
								//if string has been found in the middle of title
								if(*(titleResult - 1) == ' '){
									//if previous character is space
									if(*(titleResult + wcslen(m_ft.lpstrText)) == ' ' || *(titleResult + wcslen(m_ft.lpstrText)) == '\0')
										//if the next character after found string in title is space or null symbol (end of string)
										wholeWord = TRUE;
								}
							}
						}
						if(wholeWord){
							matched |= MATCH_BY_TITLE;
							if(hItem == NULL){
								hItem = InsertFoundParent(&tvs, hTree, pNote);
							}
							pFoundTitle[index] = TRUE;
						}
						if(m_MatchType == MATCH_BY_TITLE)
							//continue to next note
							goto _next;
					}
				}
				while(result != -1){
					result = SendMessageW(hEdit, EM_FINDTEXTEXW, m_flags, (LPARAM)&m_ft);
					if(result != -1){
						//matched by text
						matched |= MATCH_BY_TEXT;
						pFoundText[index] = TRUE;
							tvs.item.lParam = 0;
						if(hItem == NULL){
							//insert found parent
							hItem = InsertFoundParent(&tvs, hTree, pNote);
						}
						//insert found child
						InsertFoundChild(&tvs, hEdit, hTree, hItem);
						//prepare for next search
						m_ft.chrg.cpMin = m_ft.chrgText.cpMax;
						m_ft.chrg.cpMax = -1;
					}
				}
				_next:
				w = wcstok(NULL, L" ", &ptr);
				index++;
			}
			
			if(hItem){
				if(searchType == SEARCH_AND){
					if(m_MatchType == MATCH_BY_TEXT){
						for(int i = 0; i < count; i++){
							findResult &= pFoundText[i];
						}
					}
					else if(m_MatchType == MATCH_BY_TITLE){
						for(int i = 0; i < count; i++){
							findResult &= pFoundTitle[i];
						}
					}
					else{
						for(int i = 0; i < count; i++){
							findResult &= pFoundText[i];
						}
						BOOL	findTitle = TRUE;
						for(int i = 0; i < count; i++){
							findTitle &= pFoundTitle[i];
						}
						findResult |= findTitle; 
					}
				}
				else{
					if(m_MatchType == MATCH_BY_TEXT){
						for(int i = 0; i < count; i++){
							findResult |= pFoundText[i];
						}
					}
					else if(m_MatchType == MATCH_BY_TITLE){
						for(int i = 0; i < count; i++){
							findResult |= pFoundTitle[i];
						}
					}
					else{
						for(int i = 0; i < count; i++){
							findResult |= pFoundText[i];
						}
						for(int i = 0; i < count; i++){
							findResult |= pFoundTitle[i];
						}
					}
				}
				if(findResult){
					if(m_MatchType == MATCH_BY_TEXT_AND_TITLE){
						TVITEMW			tvi = {0};

						tvi.mask = TVIF_TEXT;
						tvi.hItem = hItem;
						tvi.pszText = szTemp;
						tvi.cchTextMax = 256;
						SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
						wcscat(szTemp, L" (");
						if(matched == MATCH_BY_TITLE)
							wcscat(szTemp, m_SearchMatches.matchTitle);
						else if(matched == MATCH_BY_TEXT)
							wcscat(szTemp, m_SearchMatches.matchText);
						else
							wcscat(szTemp, m_SearchMatches.matchBoth);
						wcscat(szTemp, L")");
						SendMessageW(hTree, TVM_SETITEMW, 0, (LPARAM)&tvi);
					}
					TreeView_Expand(hTree, hItem, TVE_EXPAND);
				}
				else
					TreeView_DeleteItem(hTree, hItem);
			}
			if(!pNote->pData->visible){
				SendMessageW(hEdit, WM_DESTROY, 0, 0);
			}
			free(pFoundText);
			free(pFoundTitle);
		}
		pNote = pNote->next;
	}
	free(pTemp);
	free(pSearch);
}

static void ClearTreeView(HWND hwnd){
	HWND			hTree;
	HTREEITEM		hParent, hChild;
	TVITEMW			tvi = {0};

	tvi.mask = TVIF_PARAM;
	hTree = GetDlgItem(hwnd, IDC_TVW_SEARCH);
	hParent = TreeView_GetRoot(hTree);
	while(hParent){
		hChild = TreeView_GetChild(hTree, hParent);
		while(hChild){
			tvi.hItem = hChild;
			TreeView_GetItem(hTree, &tvi);
			if(tvi.lParam){
				free((CHARRANGE *)tvi.lParam);
				tvi.lParam = 0;
			}
			hChild = TreeView_GetNextSibling(hTree, hChild);
		}
		hParent = TreeView_GetNextSibling(hTree, hParent);
	}
	TreeView_DeleteAllItems(hTree);
}

static HTREEITEM InsertFoundParent(LPTVINSERTSTRUCTW lptvs, HWND hTree, PMEMNOTE pNote){
	lptvs->hParent = TVI_ROOT;
	lptvs->item.iImage = ItemImageIndex(pNote);
	lptvs->item.iSelectedImage = lptvs->item.iImage;
	lptvs->item.lParam = (int)pNote;
	lptvs->item.pszText = pNote->pData->szName;
	return (HTREEITEM)SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)lptvs);
}

static void InsertFoundChild(LPTVINSERTSTRUCTW lptvs, HWND hEdit, HWND hTree, HTREEITEM hItem){
	wchar_t			szNumber[16], szBuffer[128];
	int				line, col;
	CHARRANGE		* lpchr;

	line = SendMessageW(hEdit, EM_EXLINEFROMCHAR, 0, m_ft.chrgText.cpMin);
	_itow(++line, szNumber, 10);
					
	lptvs->item.lParam = 0;
	lptvs->hParent = hItem;
	lptvs->item.iImage = 22;
	lptvs->item.iSelectedImage = 22;
	lptvs->item.pszText = szBuffer;

	wcscpy(szBuffer, m_ft.lpstrText);
	wcscat(szBuffer, L" (");
	wcscat(szBuffer, m_Line);
	wcscat(szBuffer, L": ");
	wcscat(szBuffer, szNumber);
	wcscat(szBuffer, L", ");
	wcscat(szBuffer, m_Col);
	wcscat(szBuffer, L": ");

	col = m_ft.chrgText.cpMin - SendMessageW(hEdit, EM_LINEINDEX, --line, 0);
	_itow(++col, szNumber, 10);
	wcscat(szBuffer, szNumber);
	wcscat(szBuffer, L")");

	lpchr = calloc(1, sizeof(CHARRANGE));
	memcpy(lpchr, &m_ft.chrgText.cpMin, sizeof(CHARRANGE));
	lptvs->item.lParam = (int)lpchr;

	SendMessageW(hTree, TVM_INSERTITEMW, 0, (LPARAM)lptvs);
}

static void FindProcRegular(HWND hwnd){
	wchar_t			szTemp[256], *titleResult;
	PMEMNOTE		pNote = MemoryNotes();
	HWND			hEdit, hTree;
	int				result;
	TVINSERTSTRUCTW	tvs = {0};
	HTREEITEM		hItem;
	int				matched;
	BOOL			wholeWord;

	hTree = GetDlgItem(hwnd, IDC_TVW_SEARCH);
	ClearTreeView(hwnd);

	tvs.item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_TEXT | TVIF_SELECTEDIMAGE;
	tvs.hInsertAfter = TVI_LAST;
	while(pNote){
		hEdit = NULL;
		if(!pNote->pData->visible && m_hidden){
			hEdit = GetUnvisibleNoteEdit(pNote);
		}
		else{
			hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
		}
		if(hEdit){
			hItem = NULL;
			matched = 0;
			result = 0;
			wholeWord = TRUE;
			m_ft.chrg.cpMin = 0;
			m_ft.chrg.cpMax = -1;
			//check title
			if(m_MatchType == MATCH_BY_TEXT_AND_TITLE || m_MatchType == MATCH_BY_TITLE){
				if((m_flags & FR_MATCHCASE) != FR_MATCHCASE)
					titleResult = _wcsistr(pNote->pData->szName, m_ft.lpstrText);
				else
					titleResult = wcsstr(pNote->pData->szName, m_ft.lpstrText);
				if(titleResult){
					if((m_flags & FR_WHOLEWORD) == FR_WHOLEWORD){
						//initially set wholeWord to FALSE
						wholeWord = FALSE;
						if(titleResult == pNote->pData->szName){
							//if string has been found at the beginning of title
							if(wcslen(m_ft.lpstrText) == wcslen(pNote->pData->szName))
								//if the title is exactly as string
								wholeWord = TRUE;
							else if(*(pNote->pData->szName + wcslen(m_ft.lpstrText)) == ' ')
								//if the next character after found string in title is space
								wholeWord = TRUE;
						}
						else{
							//if string has been found in the middle of title
							if(*(titleResult - 1) == ' '){
								//if previous character is space
								if(*(titleResult + wcslen(m_ft.lpstrText)) == ' ' || *(titleResult + wcslen(m_ft.lpstrText)) == '\0')
									//if the next character after found string in title is space or null symbol (end of string)
									wholeWord = TRUE;
							}
						}
					}
					if(wholeWord){
						matched |= MATCH_BY_TITLE;
						if(hItem == NULL){
							hItem = InsertFoundParent(&tvs, hTree, pNote);
						}
					}
				}
				if(m_MatchType == MATCH_BY_TITLE)
					//continue to next note
					goto _next;
			}
			while(result != -1){
				result = SendMessageW(hEdit, EM_FINDTEXTEXW, m_flags, (LPARAM)&m_ft);
				if(result != -1){
					//matched by text
					matched |= MATCH_BY_TEXT;
					tvs.item.lParam = 0;
					if(hItem == NULL){
						hItem = InsertFoundParent(&tvs, hTree, pNote);
					}
					//insert found child
					InsertFoundChild(&tvs, hEdit, hTree, hItem);
					//prepare for next search
					m_ft.chrg.cpMin = m_ft.chrgText.cpMax;
					m_ft.chrg.cpMax = -1;
				}
			}
			if(hItem){
				if(m_MatchType == MATCH_BY_TEXT_AND_TITLE){
					TVITEMW			tvi = {0};

					tvi.mask = TVIF_TEXT;
					tvi.hItem = hItem;
					tvi.pszText = szTemp;
					tvi.cchTextMax = 256;
					SendMessageW(hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
					wcscat(szTemp, L" (");
					if(matched == MATCH_BY_TITLE)
						wcscat(szTemp, m_SearchMatches.matchTitle);
					else if(matched == MATCH_BY_TEXT)
						wcscat(szTemp, m_SearchMatches.matchText);
					else
						wcscat(szTemp, m_SearchMatches.matchBoth);
					wcscat(szTemp, L")");
					SendMessageW(hTree, TVM_SETITEMW, 0, (LPARAM)&tvi);
				}
				TreeView_Expand(hTree, hItem, TVE_EXPAND);
			}
			if(!pNote->pData->visible){
				SendMessageW(hEdit, WM_DESTROY, 0, 0);
			}
		}
		_next:
		pNote = pNote->next;
	}
}
