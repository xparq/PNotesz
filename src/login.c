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

#include "notes.h"
#include "global.h"
#include "stringconstants.h"
#include "login.h"
#include "encryption.h"
#include "shared.h"
#include "enums.h"
#include "groups.h"

static BOOL CALLBACK Login_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Login_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Login_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Login_OnDestroy(HWND hwnd);
static BOOL IsPasswordValid(HWND hwnd);
static void EnableOKButton(HWND hwnd);

static int					m_DlgId;
static wchar_t				m_InvalidPassword[256], m_InvalidOldPassword[256], m_BothPasswords[256];
static wchar_t				*lpNewPwrd;
static login_dialog_type	m_LoginType;
static LPVOID				m_pParam;

int ShowLoginDialog(HINSTANCE hInstance, HWND hParent, int idDlg, wchar_t * pNewPwrd, int eType, LPVOID lpParam){
	m_DlgId = idDlg;
	m_LoginType = eType;
	m_pParam = lpParam;
	
	lpNewPwrd = pNewPwrd;
	if(lpNewPwrd)
		*lpNewPwrd = '\0';
	return DialogBoxParamW(hInstance, MAKEINTRESOURCEW(idDlg), hParent, Login_DlgProc, 0);
}

static BOOL CALLBACK Login_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_INITDIALOG, Login_OnInitDialog);
	HANDLE_MSG (hwnd, WM_COMMAND, Login_OnCommand);
	HANDLE_MSG (hwnd, WM_DESTROY, Login_OnDestroy);

	default: return FALSE;
	}
}

static void Login_OnDestroy(HWND hwnd)
{
	lpNewPwrd = NULL;
	g_hLoginDialog = NULL;
	DisableInput(false);
}

static void Login_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDOK:
			if(IsPasswordValid(hwnd)){
				EndDialog(hwnd, IDOK);
			}
			break;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDC_EDT_PWRD_MAIN:
		case IDC_EDT_PWRD_SECOND:
		case IDC_EDT_PWRD_NEW:
			if(codeNotify == EN_CHANGE)
				EnableOKButton(hwnd);
			break;

	}
}

static BOOL Login_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t 		szBuffer[256], szTemp[128];

	g_hLoginDialog = hwnd;
	DisableInput(true);
	GetPNotesMessage(L"invalid_pwrd", L"Incorrect password", m_InvalidPassword);
	GetPNotesMessage(L"invalid_old_pwrd", L"Incorrect old password", m_InvalidOldPassword);
	GetPNotesMessage(L"both_pwrd", L"Both password strings should be identical. Please, check the spelling.", m_BothPasswords);
	switch(m_DlgId){
		case DLG_LOGIN_MAIN:{
			GetPrivateProfileStringW(S_LOGIN_CAPTIONS, IK_ENTER_PWRD, L"Enter password", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			SetDlgItemTextW(hwnd, IDC_ST_PWRD_MAIN, szBuffer);
			GetPrivateProfileStringW(L"captions", L"enter_pwrd", L"Enter Password", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			if(m_LoginType == LDT_NOTE){
				PMEMNOTE		pNote = (PMEMNOTE)m_pParam;

				GetPrivateProfileStringW(L"captions", L"note", L"note", szTemp, 128, g_NotePaths.CurrLanguagePath);
				wcscat(szBuffer, L" [");
				wcscat(szBuffer, szTemp);
				wcscat(szBuffer, L" '");
				wcscat(szBuffer, pNote->pData->szName);
				wcscat(szBuffer, L"']");
			}
			else if(m_LoginType == LDT_GROUP){
				LPPNGROUP		pGroup = (LPPNGROUP)m_pParam;

				GetPrivateProfileStringW(L"captions", L"group", L"group", szTemp, 128, g_NotePaths.CurrLanguagePath);
				wcscat(szBuffer, L" [");
				wcscat(szBuffer, szTemp);
				wcscat(szBuffer, L" '");
				wcscat(szBuffer, pGroup->szName);
				wcscat(szBuffer, L"']");
			}
			else if(m_LoginType == LDT_DIARY){
				GetPrivateProfileStringW(L"captions", L"group", L"group", szTemp, 128, g_NotePaths.CurrLanguagePath);
				wcscat(szBuffer, L" [");
				wcscat(szBuffer, szTemp);
				wcscat(szBuffer, L" '");
				GetPrivateProfileStringW(L"left_panel", L"209", L"Diary", szTemp, 128, g_NotePaths.CurrLanguagePath);
				wcscat(szBuffer, szTemp);
				wcscat(szBuffer, L"']");
			}
			break;
		}
		case DLG_LOGIN_CREATE:
			GetPrivateProfileStringW(S_LOGIN_CAPTIONS, IK_ENTER_PWRD, L"Enter password", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			SetDlgItemTextW(hwnd, IDC_ST_PWRD_MAIN, szBuffer);
			GetPrivateProfileStringW(S_LOGIN_CAPTIONS, IK_CONF_PWRD, L"Confirm password", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			SetDlgItemTextW(hwnd, IDC_ST_PWRD_SECOND, szBuffer);
			GetPrivateProfileStringW(L"captions", L"create_pwrd", L"Password Setting", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			if(m_LoginType == LDT_NOTE){
				PMEMNOTE		pNote = (PMEMNOTE)m_pParam;

				GetPrivateProfileStringW(L"captions", L"note", L"note", szTemp, 128, g_NotePaths.CurrLanguagePath);
				wcscat(szBuffer, L" [");
				wcscat(szBuffer, szTemp);
				wcscat(szBuffer, L" '");
				wcscat(szBuffer, pNote->pData->szName);
				wcscat(szBuffer, L"']");
			}
			else if(m_LoginType == LDT_GROUP){
				LPPNGROUP		pGroup = (LPPNGROUP)m_pParam;

				GetPrivateProfileStringW(L"captions", L"group", L"group", szTemp, 128, g_NotePaths.CurrLanguagePath);
				wcscat(szBuffer, L" [");
				wcscat(szBuffer, szTemp);
				wcscat(szBuffer, L" '");
				wcscat(szBuffer, pGroup->szName);
				wcscat(szBuffer, L"']");
			}
			else if(m_LoginType == LDT_DIARY){
				GetPrivateProfileStringW(L"captions", L"group", L"group", szTemp, 128, g_NotePaths.CurrLanguagePath);
				wcscat(szBuffer, L" [");
				wcscat(szBuffer, szTemp);
				wcscat(szBuffer, L" '");
				GetPrivateProfileStringW(L"left_panel", L"209", L"Diary", szTemp, 128, g_NotePaths.CurrLanguagePath);
				wcscat(szBuffer, szTemp);
				wcscat(szBuffer, L"']");
			}
			break;
		case DLG_LOGIN_CHANGE:
			GetPrivateProfileStringW(S_LOGIN_CAPTIONS, IK_OLD_PWRD, L"Old password", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			SetDlgItemTextW(hwnd, IDC_ST_PWRD_MAIN, szBuffer);
			GetPrivateProfileStringW(S_LOGIN_CAPTIONS, IK_NEW_PWRD, L"New password", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			SetDlgItemTextW(hwnd, IDC_ST_PWRD_NEW, szBuffer);
			GetPrivateProfileStringW(S_LOGIN_CAPTIONS, IK_CONF_PWRD, L"Confirm password", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			SetDlgItemTextW(hwnd, IDC_ST_PWRD_SECOND, szBuffer);
			GetPrivateProfileStringW(L"captions", L"change_pwrd", L"Password Changing", szBuffer, 256, g_NotePaths.CurrLanguagePath);
			break;
		default:
			break;
	}
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, DS_CANCEL);
	// SetForegroundWindow(hwnd);
	SetFocus(GetDlgItem(hwnd, IDC_EDT_PWRD_MAIN));
	return FALSE;
}

static BOOL IsPasswordValid(HWND hwnd){
	wchar_t			szPwrdMain[256], szPwrdNew[256], szPwrdConfirm[256], szHash[256], szTemp[256];

	switch(m_DlgId){
		case DLG_LOGIN_MAIN:
			GetDlgItemTextW(hwnd, IDC_EDT_PWRD_MAIN, szPwrdMain, 256);
			CreateHashString(szHash, szPwrdMain);
			if(m_LoginType == LDT_MAIN)
				GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", szTemp, 256, g_NotePaths.INIFile);
			else if(m_LoginType == LDT_GROUP){
				LPPNGROUP		pGroup = (LPPNGROUP)m_pParam;
				wcscpy(szTemp, pGroup->szLock);
			}
			else if(m_LoginType == LDT_NOTE){
				PMEMNOTE		pNote = (PMEMNOTE)m_pParam;
				GetPrivateProfileStringW(pNote->pFlags->id, IK_LOCK, L"N/A", szTemp, 256, g_NotePaths.DataFile);
			}
			else if(m_LoginType == LDT_DIARY){
				GetPrivateProfileStringW(S_DIARY, IK_DIARY_LOCK, L"N/A", szTemp, 256, g_NotePaths.INIFile);
			}
			if(wcscmp(szHash, szTemp) == 0)
				return TRUE;
			else{
				MessageBoxW(hwnd, m_InvalidPassword, 0, MB_OK | MB_ICONERROR);
				SendDlgItemMessageW(hwnd, IDC_EDT_PWRD_MAIN, EM_SETSEL, 0, -1);
			}
			break;
		case DLG_LOGIN_CREATE:
			GetDlgItemTextW(hwnd, IDC_EDT_PWRD_MAIN, szPwrdMain, 256);
			GetDlgItemTextW(hwnd, IDC_EDT_PWRD_SECOND, szPwrdConfirm, 256);
			if(wcscmp(szPwrdMain, szPwrdConfirm) == 0){
				CreateHashString(szHash, szPwrdMain);
				if(lpNewPwrd)
					wcscpy(lpNewPwrd, szHash);
				// WritePrivateProfileStringW(S_NOTE, IK_PASSWORD, szHash, g_NotePaths.INIFile);
				return TRUE;
			}
			else{
				MessageBoxW(hwnd, m_BothPasswords, 0, MB_OK | MB_ICONERROR);
				SetFocus(GetDlgItem(hwnd, IDC_EDT_PWRD_MAIN));
				SendDlgItemMessageW(hwnd, IDC_EDT_PWRD_MAIN, EM_SETSEL, 0, -1);
			}
			break;
		case DLG_LOGIN_CHANGE:
			GetDlgItemTextW(hwnd, IDC_EDT_PWRD_MAIN, szPwrdMain, 256);
			CreateHashString(szHash, szPwrdMain);
			if(m_LoginType == LDT_MAIN)
				GetPrivateProfileStringW(S_NOTE, IK_PASSWORD, L"N/A", szTemp, 256, g_NotePaths.INIFile);
			else if(m_LoginType == LDT_GROUP){
				LPPNGROUP		pGroup = (LPPNGROUP)m_pParam;
				wcscpy(szTemp, pGroup->szLock);
			}
			else if(m_LoginType == LDT_NOTE){
				PMEMNOTE		pNote = (PMEMNOTE)m_pParam;
				GetPrivateProfileStringW(pNote->pFlags->id, IK_LOCK, L"N/A", szTemp, 256, g_NotePaths.DataFile);
			}
			else if(m_LoginType == LDT_DIARY){
				GetPrivateProfileStringW(S_DIARY, IK_DIARY_LOCK, L"N/A", szTemp, 256, g_NotePaths.INIFile);
			}
			if(wcscmp(szHash, szTemp) == 0){
				GetDlgItemTextW(hwnd, IDC_EDT_PWRD_NEW, szPwrdNew, 256);
				GetDlgItemTextW(hwnd, IDC_EDT_PWRD_SECOND, szPwrdConfirm, 256);
				if(wcscmp(szPwrdNew, szPwrdConfirm) == 0){
					CreateHashString(szHash, szPwrdNew);
					//store hash of new password
					if(lpNewPwrd)
						wcscpy(lpNewPwrd, szHash);
					// WritePrivateProfileStringW(S_NOTE, IK_PASSWORD, szHash, g_NotePaths.INIFile);
					return TRUE;
				}
				else{
					MessageBoxW(hwnd, m_BothPasswords, 0, MB_OK | MB_ICONERROR);
					SetFocus(GetDlgItem(hwnd, IDC_EDT_PWRD_NEW));
					SendDlgItemMessageW(hwnd, IDC_EDT_PWRD_NEW, EM_SETSEL, 0, -1);
				}
			}
			else{
				MessageBoxW(hwnd, m_InvalidOldPassword, 0, MB_OK | MB_ICONERROR);
				SetFocus(GetDlgItem(hwnd, IDC_EDT_PWRD_MAIN));
				SendDlgItemMessageW(hwnd, IDC_EDT_PWRD_MAIN, EM_SETSEL, 0, -1);
			}
			break;
	}
	return FALSE;
}

static void EnableOKButton(HWND hwnd){
	switch(m_DlgId){
		case DLG_LOGIN_MAIN:
			if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_PWRD_MAIN)) > 0)
				EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
			else
				EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
			break;
		case DLG_LOGIN_CREATE:
			if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_PWRD_MAIN)) > 0 && GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_PWRD_SECOND)) > 0)
				EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
			else
				EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
			break;
		case DLG_LOGIN_CHANGE:
			if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_PWRD_MAIN)) > 0 && GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_PWRD_SECOND)) > 0 && GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_PWRD_NEW)) > 0)
				EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
			else
				EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
			break;
	}
}
