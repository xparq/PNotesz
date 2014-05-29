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
#include "contacts.h"
#include "contgroups.h"

#define	CONTACTS_PROP		L"CONT_PROP"

static void Hosts_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Hosts_OnClose(HWND hwnd);
static BOOL Hosts_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void HostsEnableOK(HWND hwnd);

BOOL CALLBACK Hosts_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_COMMAND, Hosts_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, Hosts_OnInitDialog);
		HANDLE_MSG (hwnd, WM_CLOSE, Hosts_OnClose);
		case WM_NOTIFY:{
			LPNMHDR		lpnmh = (LPNMHDR)lParam;
			if(lpnmh->idFrom == IDC_IP_ADDRESS && lpnmh->code == IPN_FIELDCHANGED){
				HostsEnableOK(hwnd);
			}
			return TRUE;
		}
		default: return FALSE;
	}
}

static void Hosts_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDCANCEL:
			RemovePropW(hwnd, CONTACTS_PROP);
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDOK:{
			wchar_t			szBuffer[256];
			LPPCONTACT		pConstants = NULL;
			int				mode = (int)GetPropW(hwnd, CONTACTS_PROP), index = 0;

			pConstants = (mode == 0 ? g_TempContacts : (mode == 2 ? g_PContacts : NULL));
			GetDlgItemTextW(hwnd, IDC_EDT_CONTACT_NAME, szBuffer, 255);
			if((mode == 0 || mode == 2) && PContactsContains(pConstants, szBuffer)){
				GetPrivateProfileStringW(S_MESSAGES, L"contact_exists", L"Contact with the same name already exists", szBuffer, 255, g_NotePaths.CurrLanguagePath);
				MessageBoxW(hwnd, szBuffer, PROG_NAME, MB_OK | MB_ICONERROR);
				break;
			}
			wcscpy(g_Contact.name, szBuffer);
			GetDlgItemTextW(hwnd, IDC_EDT_HOST_NAME, szBuffer, 255);
			wcscpy(g_Contact.host, szBuffer);
			if(!SendDlgItemMessageW(hwnd, IDC_IP_ADDRESS, IPM_ISBLANK, 0, 0)){
				SendDlgItemMessageW(hwnd, IDC_IP_ADDRESS, IPM_GETADDRESS, 0, (LPARAM)&g_Contact.address);
			}
			if(IsDlgButtonChecked(hwnd, IDC_OPT_HOST_NAME) == BST_CHECKED){
				g_Contact.usename = true;
			}
			else{
				g_Contact.usename = false;
			}
			index = SendDlgItemMessageW(hwnd, IDC_CBO_CONT_GROUPS, CB_GETCURSEL, 0, 0);
			g_Contact.group = SendDlgItemMessageW(hwnd, IDC_CBO_CONT_GROUPS, CB_GETITEMDATA, index, 0); 
			RemovePropW(hwnd, CONTACTS_PROP);
			if(mode != 2)
				EndDialog(hwnd, IDOK);
			else{
				if(IsDlgButtonChecked(hwnd, IDC_CHK_SEND_NOW) == BST_CHECKED)
					EndDialog(hwnd, IDYES);
				else
					EndDialog(hwnd, IDOK);
			}
			break;
		}
		case IDC_EDT_CONTACT_NAME:
			if(codeNotify == EN_CHANGE){
				HostsEnableOK(hwnd);
			}
			break;
		case IDC_EDT_HOST_NAME:
			if(codeNotify == EN_CHANGE){
				HostsEnableOK(hwnd);
			}
			break;
		case IDC_OPT_HOST_NAME:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_OPT_HOST_NAME) == BST_CHECKED){
					EnableWindow(GetDlgItem(hwnd, IDC_IP_ADDRESS), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_EDT_HOST_NAME), TRUE);
				}
				HostsEnableOK(hwnd);
			}
			break;
		case IDC_OPT_HOST_IP:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_OPT_HOST_IP) == BST_CHECKED){
					EnableWindow(GetDlgItem(hwnd, IDC_IP_ADDRESS), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_EDT_HOST_NAME), FALSE);
				}
				HostsEnableOK(hwnd);
			}
			break;
	}
}

static void Hosts_OnClose(HWND hwnd)
{
	RemovePropW(hwnd, CONTACTS_PROP);
	EndDialog(hwnd, 0);
}

static BOOL Hosts_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT			rc;
	HWND			hAdd;
	wchar_t			szBuffer[256];
	int				index;
	LPPCONTGROUP	lpGroups;

	GetWindowRect(GetDlgItem(hwnd, IDC_PLACEHOLDER), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	hAdd = CreateWindowExW(WS_EX_CLIENTEDGE, WC_IPADDRESSW, NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, 0, 0, 0, hwnd, (HMENU)IDC_IP_ADDRESS, g_hInstance, NULL);
	MoveWindow(hAdd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
	SendMessageW(hAdd, WM_SETFONT, (WPARAM)SendMessageW(hwnd, WM_GETFONT, 0, 0), TRUE);
	SetPropW(hwnd, CONTACTS_PROP, (HANDLE)lParam);
	//fill contacts groups
	if(lParam == 0 || lParam == 1)
		lpGroups = g_TempContGroups;
	else
		lpGroups = g_PContGroups;
	for(LPPCONTGROUP pTemp = lpGroups; pTemp; pTemp = pTemp->next){
		index = SendDlgItemMessageW(hwnd, IDC_CBO_CONT_GROUPS, CB_ADDSTRING, 0, (LPARAM)pTemp->name);
		SendDlgItemMessageW(hwnd, IDC_CBO_CONT_GROUPS, CB_SETITEMDATA, index, pTemp->id);
	}
	
	if(lParam == 0 || lParam == 2){
		GetPrivateProfileStringW(S_CAPTIONS, L"new_contact", L"New contact", szBuffer, 255, g_NotePaths.CurrLanguagePath);
		CheckDlgButton(hwnd, IDC_OPT_HOST_NAME, BST_CHECKED);
		EnableWindow(GetDlgItem(hwnd, IDC_IP_ADDRESS), FALSE);
		if(lParam == 2){
			ShowWindow(GetDlgItem(hwnd, IDC_CHK_SEND_NOW), SW_SHOW);
			CheckDlgButton(hwnd, IDC_CHK_SEND_NOW, BST_CHECKED);
		}
		SendDlgItemMessageW(hwnd, IDC_CBO_CONT_GROUPS, CB_SETCURSEL, 0, 0);
	}
	else if (lParam == 1){
		GetPrivateProfileStringW(S_CAPTIONS, L"edit_contact", L"Edit contact", szBuffer, 255, g_NotePaths.CurrLanguagePath);
		SetDlgItemTextW(hwnd, IDC_EDT_CONTACT_NAME, g_Contact.name);
		SetDlgItemTextW(hwnd, IDC_EDT_HOST_NAME, g_Contact.host);
		SendDlgItemMessageW(hwnd, IDC_IP_ADDRESS, IPM_SETADDRESS, 0, (LPARAM)g_Contact.address);
		if(g_Contact.usename){
			CheckDlgButton(hwnd, IDC_OPT_HOST_NAME, BST_CHECKED);
			EnableWindow(GetDlgItem(hwnd, IDC_IP_ADDRESS), FALSE);
		}
		else{
			CheckDlgButton(hwnd, IDC_OPT_HOST_IP, BST_CHECKED);
			EnableWindow(GetDlgItem(hwnd, IDC_EDT_HOST_NAME), FALSE);
		}
		EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
		for(int i = 0; i < SendDlgItemMessageW(hwnd, IDC_CBO_CONT_GROUPS, CB_GETCOUNT, 0, 0); i++){
			index = SendDlgItemMessageW(hwnd, IDC_CBO_CONT_GROUPS, CB_GETITEMDATA, i, 0);
			if(index == g_Contact.group){
				index = SendDlgItemMessageW(hwnd, IDC_CBO_CONT_GROUPS, CB_SETCURSEL, i, 0);
				break;
			}
		}
	}
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDC_OPT_HOST_NAME, g_NotePaths.CurrLanguagePath, L"Use computer name");
	SetDlgCtlText(hwnd, IDC_OPT_HOST_IP, g_NotePaths.CurrLanguagePath, L"Use IP address");
	SetDlgCtlText(hwnd, IDC_ST_CONTACT_NAME, g_NotePaths.CurrLanguagePath, L"Name");
	SetDlgCtlText(hwnd, IDC_CHK_SEND_NOW, g_NotePaths.CurrLanguagePath, L"Send now");
	SetDlgCtlText(hwnd, IDC_ST_CONT_GROUPS, g_NotePaths.CurrLanguagePath, L"Groups");
	SetDlgCtlText(hwnd, IDOK, g_NotePaths.CurrLanguagePath, L"OK");
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, L"Cancel");

	return TRUE;
}

static void HostsEnableOK(HWND hwnd){
	if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_CONTACT_NAME)) == 0){
		EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
		return;
	}
	if(IsDlgButtonChecked(hwnd, IDC_OPT_HOST_NAME) == BST_CHECKED){
		if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_HOST_NAME)) == 0){
			EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
			return;
		}
	}
	else{
		if(SendDlgItemMessageW(hwnd, IDC_IP_ADDRESS, IPM_ISBLANK, 0, 0)){
			EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
			return;
		}
	}
	EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
}

