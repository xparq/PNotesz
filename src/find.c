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

#include "notes.h"
#include "global.h"
#include "numericconstants.h"
#include "stringconstants.h"
#include "shared.h"

typedef struct {
	RECT		rReplace;
	RECT		rCancel;
	RECT		rTextReplace;
	RECT		rCboReplace;
	RECT		rChkMatchCase;
	RECT		rChkWholeWord;
	RECT		rOptUp;
	RECT		rOptDown;
	RECT		rFull;
	RECT		rHalf;
}FIND_RECTS;

static BOOL Find_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Find_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Find_OnClose(HWND hwnd);
static void ApplyFindLanguage(HWND hwnd);
static void ResizeOnInit(HWND hwnd, int iFindReplace);
static void GetSelectedEditText(HWND hwnd);
static void MoveByParent(HWND hwnd);
static void SaveRects(HWND hwnd);
static void PrepareFindFlags(HWND hwnd);

static FIND_RECTS			m_frects;
static HWND					m_hOwner;
static int					m_FindType;

BOOL CALLBACK Find_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_INITDIALOG, Find_OnInitDialog);
	HANDLE_MSG (hwnd, WM_COMMAND, Find_OnCommand);
	HANDLE_MSG (hwnd, WM_CLOSE, Find_OnClose);

	case PNM_CTRL_UPD_LANG:
		ApplyFindLanguage(hwnd);
		return TRUE;
	case PNM_NEW_FIND_PARENT:{
		P_NFIND		lpnf;

		lpnf = (P_NFIND)lParam;
		m_hOwner = (HANDLE)lpnf->hNote;
		if(m_FindType != lpnf->iFindReplace){
			m_FindType = lpnf->iFindReplace;
			ResizeOnInit(hwnd, lpnf->iFindReplace);
		}
		MoveByParent(hwnd);
		GetSelectedEditText(hwnd);
		return TRUE;
	}
	default: return FALSE;
	}
}

static void Find_OnClose(HWND hwnd)
{
	g_hFindDialog = NULL;
	m_FindType = 0;
	SetFocus((HWND)GetPropW(m_hOwner, PH_EDIT));
	m_hOwner = NULL;
	EndDialog(hwnd, 0);
}

static void Find_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
	case IDC_CBO_SEARCH:
		if(codeNotify == CBN_EDITCHANGE){
			int len = SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, WM_GETTEXTLENGTH, 0, 0);
			if(len){
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND_NEXT), TRUE);
				if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, WM_GETTEXTLENGTH, 0, 0) > 0){
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE), TRUE);
				}
			}
			else{
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND_NEXT), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE), FALSE);
			}
		}
		else if(codeNotify == CBN_SELENDOK){
			if(SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_GETCURSEL, 0, 0) != CB_ERR){
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND_NEXT), TRUE);
				if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, WM_GETTEXTLENGTH, 0, 0) > 0){
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE), TRUE);
				}
			}
			else{
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND_NEXT), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE), FALSE);
			}
		}
		break;
	case IDC_CBO_REPLACE:
		if(codeNotify == CBN_EDITCHANGE){
			int len = SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, WM_GETTEXTLENGTH, 0, 0);
			if(len){
				if(IsWindowEnabled(GetDlgItem(hwnd, IDC_CMD_FIND_NEXT))){
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE), TRUE);
				}
			}
			else{
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE), FALSE);
			}
		}
		else if(codeNotify == CBN_SELENDOK){
			if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_GETCURSEL, 0, 0) != CB_ERR){
				if(IsWindowEnabled(GetDlgItem(hwnd, IDC_CMD_FIND_NEXT))){
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE), TRUE);
				}
			}
			else{
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE), FALSE);
			}
		}
		break;
	case IDCANCEL:
		g_hFindDialog = NULL;
		m_FindType = 0;
		SetFocus((HWND)GetPropW(m_hOwner, PH_EDIT));
		m_hOwner = NULL;
		EndDialog(hwnd, 0);
		break;
	case IDC_CMD_FIND_NEXT:
		if(codeNotify == BN_CLICKED){
			PrepareFindFlags(hwnd);
			SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, WM_GETTEXT, 256, (LPARAM)g_SearchString);
			if(SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_FINDSTRINGEXACT, -1, (LPARAM)g_SearchString) == CB_ERR){
				SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_INSERTSTRING, 0, (LPARAM)g_SearchString);
				SaveSearchHistory(hwnd);
			}
			if(SharedFindProc((HWND)GetPropW(m_hOwner, PH_EDIT)) == -1){
				wchar_t		szBuffer[256];
				GetWindowTextW(hwnd, szBuffer, 255);
				MessageBoxW(hwnd, g_Strings.NoMatchesFound, szBuffer, MB_OK | MB_ICONINFORMATION);
			}
		}
		break;
	case IDC_CMD_REPLACE:
		if(codeNotify == BN_CLICKED){
			PrepareFindFlags(hwnd);
			SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, WM_GETTEXT, 256, (LPARAM)g_SearchString);
			SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, WM_GETTEXT, 256, (LPARAM)g_ReplaceString);
			if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_FINDSTRINGEXACT, -1, (LPARAM)g_ReplaceString) == CB_ERR){
				SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_INSERTSTRING, 0, (LPARAM)g_ReplaceString);
				SaveReplaceHistory(hwnd);
			}
			if(SharedReplaceProc((HWND)GetPropW(m_hOwner, PH_EDIT)) == -1){
				wchar_t		szBuffer[256];
				GetWindowTextW(hwnd, szBuffer, 255);
				MessageBoxW(hwnd, g_Strings.NoMatchesFound, szBuffer, MB_OK | MB_ICONINFORMATION);
			}
		}
		break;
	case IDC_CMD_REPLACE_ALL:
		if(codeNotify == BN_CLICKED){
			wchar_t		szBuffer[256], szMessage[512], szTemp[12];
			int 		res = 0, count = 0;
			PrepareFindFlags(hwnd);
			SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, WM_GETTEXT, 256, (LPARAM)g_SearchString);
			SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, WM_GETTEXT, 256, (LPARAM)g_ReplaceString);
			if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_FINDSTRINGEXACT, -1, (LPARAM)g_ReplaceString) == CB_ERR){
				SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_INSERTSTRING, 0, (LPARAM)g_ReplaceString);
				SaveReplaceHistory(hwnd);
			}
			while(res != -1){
				res = SharedReplaceProc((HWND)GetPropW(m_hOwner, PH_EDIT));
				if(res != -1){
					count++;
				}
			}
			GetWindowTextW(hwnd, szBuffer, 255);
			wcscpy(szMessage, g_Strings.ReplaceComplete);
			wcscat(szMessage, L"\n");
			if(count > 0){
				wcscat(szMessage, g_Strings.MatchesFound);
				wcscat(szMessage, L" ");
				_itow(count, szTemp, 10);
				wcscat(szMessage, szTemp);
			}
			else
				wcscat(szMessage, g_Strings.NoMatchesFound);
			MessageBoxW(hwnd, szMessage, szBuffer, MB_OK | MB_ICONINFORMATION);
		}
		break;
	}
}

static void PrepareFindFlags(HWND hwnd){

	int			res;

	g_Flags = 0;
	res = IsDlgButtonChecked(hwnd, IDC_CHK_MATCH_CASE);
	if(res == BST_CHECKED)
		g_Flags |= FR_MATCHCASE;
	res = IsDlgButtonChecked(hwnd, IDC_CHK_WHOLE_WORD);
	if(res == BST_CHECKED)
		g_Flags |= FR_WHOLEWORD;
	res = IsDlgButtonChecked(hwnd, IDC_OPT_FIND_DOWN);
	if(res == BST_CHECKED)
		g_Flags |= FR_DOWN;
	else
		g_Flags &= ~FR_DOWN;
}

static BOOL Find_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	P_NFIND		lpnf;

	g_hFindDialog = hwnd;
	SaveRects(hwnd);
	lpnf = (P_NFIND)lParam;
	ApplyFindLanguage(hwnd);
	m_hOwner = lpnf->hNote;
	m_FindType = lpnf->iFindReplace;
	ResizeOnInit(hwnd, lpnf->iFindReplace);
	MoveByParent(hwnd);
	if((g_Flags & FR_DOWN) == FR_DOWN)
		CheckDlgButton(hwnd, IDC_OPT_FIND_DOWN, BST_CHECKED);
	else
		CheckDlgButton(hwnd, IDC_OPT_FIND_UP, BST_CHECKED);
	//limit combo box
	SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_LIMITTEXT, 255, 0);
	SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_LIMITTEXT, 255, 0);
	//load search history
	LoadSearchHistory(hwnd);
	//load replace history
	LoadReplaceHistory(hwnd);
	GetSelectedEditText(hwnd);
	// SetFocus(GetDlgItem(hwnd, IDC_CBO_SEARCH));
	return FALSE;
}

static void GetSelectedEditText(HWND hwnd){
	HWND		hEdit;
	TEXTRANGEW	tr;

	//get selected text from parent note
	hEdit = (HWND)GetPropW(m_hOwner, PH_EDIT);
	SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&tr.chrg);
	if(tr.chrg.cpMax != tr.chrg.cpMin){
		wchar_t	*pTemp;
		wchar_t *tmp; //szTemp[tr.chrg.cpMax - tr.chrg.cpMin + 1]

		pTemp = (wchar_t *)calloc(tr.chrg.cpMax - tr.chrg.cpMin + 1, sizeof(wchar_t));
		if(pTemp){
			tr.lpstrText = pTemp; // szTemp;
			SendMessageW(hEdit, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
			tmp = pTemp; //szTemp;
			while(*tmp++){
				if(*tmp == '\n' || *tmp == '\r'){
					*tmp = '\0';
					break;
				}
			}
			SendMessageW(GetDlgItem(hwnd, IDC_CBO_SEARCH), WM_SETTEXT, 0, (LPARAM)pTemp);
			EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND_NEXT), TRUE);
			free(pTemp);
		}
		
	}
	else{
		if(wcslen(g_SearchString) > 0){
			SendMessageW(GetDlgItem(hwnd, IDC_CBO_SEARCH), WM_SETTEXT, 0, (LPARAM)g_SearchString);
			EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND_NEXT), TRUE);
		}
	}

	if(wcslen(g_ReplaceString) > 0){
		SendMessageW(GetDlgItem(hwnd, IDC_CBO_REPLACE), WM_SETTEXT, 0, (LPARAM)g_ReplaceString);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), TRUE);
	}
}

static void ResizeOnInit(HWND hwnd, int iFindReplace){

	if(iFindReplace == IDM_SEARCH_NOTE){	//find
		ShowWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE), SW_HIDE);
		ShowWindow(GetDlgItem(hwnd, IDC_ST_REPLACE), SW_HIDE);
		ShowWindow(GetDlgItem(hwnd, IDC_CBO_REPLACE), SW_HIDE);
		ShowWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), SW_HIDE);
		MoveWindow(GetDlgItem(hwnd, IDCANCEL), m_frects.rReplace.left, m_frects.rReplace.top, m_frects.rReplace.right - m_frects.rReplace.left, m_frects.rReplace.bottom - m_frects.rReplace.top, TRUE);
		MoveWindow(GetDlgItem(hwnd, IDC_CHK_MATCH_CASE), m_frects.rChkMatchCase.left, m_frects.rTextReplace.top, m_frects.rChkMatchCase.right - m_frects.rChkMatchCase.left, m_frects.rChkMatchCase.bottom - m_frects.rChkMatchCase.top, TRUE);
		MoveWindow(GetDlgItem(hwnd, IDC_CHK_WHOLE_WORD), m_frects.rChkWholeWord.left, m_frects.rCboReplace.top, m_frects.rChkWholeWord.right - m_frects.rChkWholeWord.left, m_frects.rChkWholeWord.bottom - m_frects.rChkWholeWord.top, TRUE);
		MoveWindow(GetDlgItem(hwnd, IDC_OPT_FIND_UP), m_frects.rOptUp.left, m_frects.rTextReplace.top, m_frects.rOptUp.right - m_frects.rOptUp.left, m_frects.rOptUp.bottom - m_frects.rOptUp.top, TRUE);
		MoveWindow(GetDlgItem(hwnd, IDC_OPT_FIND_DOWN), m_frects.rOptDown.left, m_frects.rCboReplace.top, m_frects.rOptDown.right - m_frects.rOptDown.left, m_frects.rOptDown.bottom - m_frects.rOptDown.top, TRUE);
		MoveWindow(hwnd, m_frects.rHalf.left, m_frects.rHalf.top, m_frects.rHalf.right - m_frects.rHalf.left, m_frects.rHalf.bottom - m_frects.rHalf.top, TRUE);
	}
	else{		//replace
		ShowWindow(GetDlgItem(hwnd, IDC_ST_REPLACE), SW_SHOW);
		ShowWindow(GetDlgItem(hwnd, IDC_CBO_REPLACE), SW_SHOW);
		ShowWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE), SW_SHOW);
		ShowWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), SW_SHOW);
		MoveWindow(GetDlgItem(hwnd, IDCANCEL), m_frects.rCancel.left, m_frects.rCancel.top, m_frects.rCancel.right - m_frects.rCancel.left, m_frects.rCancel.bottom - m_frects.rCancel.top, TRUE);
		MoveWindow(GetDlgItem(hwnd, IDC_CHK_MATCH_CASE), m_frects.rChkMatchCase.left, m_frects.rChkMatchCase.top, m_frects.rChkMatchCase.right - m_frects.rChkMatchCase.left, m_frects.rChkMatchCase.bottom - m_frects.rChkMatchCase.top, TRUE);
		MoveWindow(GetDlgItem(hwnd, IDC_CHK_WHOLE_WORD), m_frects.rChkWholeWord.left, m_frects.rChkWholeWord.top, m_frects.rChkWholeWord.right - m_frects.rChkWholeWord.left, m_frects.rChkWholeWord.bottom - m_frects.rChkWholeWord.top, TRUE);
		MoveWindow(GetDlgItem(hwnd, IDC_OPT_FIND_UP), m_frects.rOptUp.left, m_frects.rOptUp.top, m_frects.rOptUp.right - m_frects.rOptUp.left, m_frects.rOptUp.bottom - m_frects.rOptUp.top, TRUE);
		MoveWindow(GetDlgItem(hwnd, IDC_OPT_FIND_DOWN), m_frects.rOptDown.left, m_frects.rOptDown.top, m_frects.rOptDown.right - m_frects.rOptDown.left, m_frects.rOptDown.bottom - m_frects.rOptDown.top, TRUE);
		MoveWindow(hwnd, m_frects.rFull.left, m_frects.rFull.top, m_frects.rFull.right - m_frects.rFull.left, m_frects.rFull.bottom - m_frects.rFull.top, TRUE);
	}
}

static void MoveByParent(HWND hwnd){
	RECT				rc1, rc2;

	GetWindowRect(m_hOwner, &rc1);
	GetWindowRect(hwnd, &rc2);
	MoveWindow(hwnd, rc1.left + ((rc1.right - rc1.left) - (rc2.right - rc2.left)) / 2, rc1.top + ((rc1.bottom - rc1.top) - (rc2.bottom - rc2.top)) / 2, rc2.right - rc2.left, rc2.bottom - rc2.top, TRUE);
}

static void ApplyFindLanguage(HWND hwnd){
	wchar_t			szBuffer[256], szKey[16];

	_itow(DLG_FIND_SINGLE, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Find", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDC_ST_SEARCH, g_NotePaths.CurrLanguagePath, L"Find what:");
	SetDlgCtlText(hwnd, IDC_CHK_MATCH_CASE, g_NotePaths.CurrLanguagePath, L"Match case");
	SetDlgCtlText(hwnd, IDC_CHK_WHOLE_WORD, g_NotePaths.CurrLanguagePath, L"Whole word");
	SetDlgCtlText(hwnd, IDC_CMD_FIND_NEXT, g_NotePaths.CurrLanguagePath, L"Find Next");
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, L"Cancel");
	SetDlgCtlText(hwnd, IDC_ST_REPLACE, g_NotePaths.CurrLanguagePath, L"Replace with:");
	SetDlgCtlText(hwnd, IDC_CMD_REPLACE_ALL, g_NotePaths.CurrLanguagePath, L"Replace All");
	SetDlgCtlText(hwnd, IDC_CMD_REPLACE, g_NotePaths.CurrLanguagePath, L"Replace");
	SetDlgCtlText(hwnd, IDC_OPT_FIND_UP, g_NotePaths.CurrLanguagePath, L"Up");
	SetDlgCtlText(hwnd, IDC_OPT_FIND_DOWN, g_NotePaths.CurrLanguagePath, L"Down");
}

static void SaveRects(HWND hwnd){
	RECT		rc;

	GetWindowRect(GetDlgItem(hwnd, IDC_CMD_REPLACE), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	CopyRect(&m_frects.rReplace, &rc);
	GetWindowRect(GetDlgItem(hwnd, IDCANCEL), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	CopyRect(&m_frects.rCancel, &rc);
	GetWindowRect(GetDlgItem(hwnd, IDC_ST_REPLACE), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	CopyRect(&m_frects.rTextReplace, &rc);
	GetWindowRect(GetDlgItem(hwnd, IDC_CBO_REPLACE), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	CopyRect(&m_frects.rCboReplace, &rc);
	GetWindowRect(GetDlgItem(hwnd, IDC_CBO_REPLACE), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	CopyRect(&m_frects.rCboReplace, &rc);
	GetWindowRect(GetDlgItem(hwnd, IDC_CHK_MATCH_CASE), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	CopyRect(&m_frects.rChkMatchCase, &rc);
	GetWindowRect(GetDlgItem(hwnd, IDC_CHK_WHOLE_WORD), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	CopyRect(&m_frects.rChkWholeWord, &rc);
	GetWindowRect(GetDlgItem(hwnd, IDC_OPT_FIND_UP), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	CopyRect(&m_frects.rOptUp, &rc);
	GetWindowRect(GetDlgItem(hwnd, IDC_OPT_FIND_DOWN), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	CopyRect(&m_frects.rOptDown, &rc);
	GetWindowRect(hwnd, &rc);
	CopyRect(&m_frects.rFull, &rc);
	rc.bottom -= 2 * (m_frects.rCancel.bottom - m_frects.rCancel.top);
	CopyRect(&m_frects.rHalf, &rc);
}
