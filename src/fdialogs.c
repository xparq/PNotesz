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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winbase.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <dlgs.h>

#define	ENC1				L"ANSI"
#define	ENC2				L"Unicode"
#define	ENC3				L"Unicode big endian"
#define	ENC4				L"UTF-8"

static UINT_PTR CALLBACK OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK OFNParentProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static HWND					m_hSelCombo;
static int					m_CurrType;

BOOL ShowOpenFileDlg(HWND hwnd, wchar_t *pstrFileName, wchar_t *pstrTitleName, wchar_t *pstrFilter, wchar_t *pstrTitle, wchar_t *pstrInitialDir){

	OPENFILENAMEW 		ofn;

	pstrFileName[0] = '\0';
	pstrTitleName[0] = '\0';
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = GetModuleHandle(0);
	ofn.lpstrFilter	= pstrFilter;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.lpstrTitle = pstrTitle;
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH; 
	if(pstrInitialDir)
		ofn.lpstrInitialDir = pstrInitialDir;
	ofn.Flags =	OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;

	return GetOpenFileNameW(&ofn);
}

BOOL ShowSaveFileDlg(HWND hwnd, wchar_t *pstrFileName, wchar_t *pstrTitleName, wchar_t *pstrFilter, wchar_t *pstrTitle, wchar_t *pstrInitialDir, wchar_t *pstrDefExt){

	OPENFILENAMEW 		ofn;

	pstrFileName[0] = '\0';
	pstrTitleName[0] = '\0';
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = GetModuleHandle(0);
	ofn.lpstrFilter	= pstrFilter;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.lpstrTitle = pstrTitle;
	ofn.lpstrDefExt = pstrDefExt;
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH; 
	if(pstrInitialDir)
		ofn.lpstrInitialDir = pstrInitialDir;
	ofn.Flags =	OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

	return GetSaveFileNameW(&ofn);
}

int ShowCustomSaveFileDlg(HWND hwnd, wchar_t *pstrFileName, wchar_t *pstrTitleName, wchar_t *pstrFilter, wchar_t *pstrTitle, wchar_t *pstrDefExt){

	OPENFILENAMEW 		ofn;

	//pstrFileName[0] = '\0';
	pstrTitleName[0] = '\0';
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = GetModuleHandle(0);
	ofn.lpstrFilter	= pstrFilter;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.lpstrTitle = pstrTitle;
	ofn.lpstrDefExt = pstrDefExt;
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH; 
	ofn.Flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_OVERWRITEPROMPT;
	ofn.lpfnHook = OFNHookProc;

	if(GetSaveFileNameW(&ofn))
		return m_CurrType;
	else
		return -1;
}

static UINT_PTR CALLBACK OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam){

	HWND				hParent, hStatic, hw;
	WINDOWPLACEMENT		wp;
	RECT				rc, rc1, rc2;
	int					of, h, exStyle, style, top, width, height;
	HFONT				hFont;

	if(uiMsg == WM_INITDIALOG){
		hParent = GetParent(hdlg);
		ZeroMemory(&wp, sizeof(wp));
		wp.length = sizeof(wp);
		GetWindowPlacement(hParent, &wp);
		GetWindowRect(GetDlgItem(hParent, cmb1), &rc);
		hw = GetDlgItem(hParent, edt1);
		if(!hw)
			hw = GetDlgItem(hParent, cmb13);
		GetWindowRect(hw, &rc1);
		of = rc.top - rc1.bottom;
		h = rc.bottom - rc.top;

		GetClientRect(hParent, &rc2);
		rc2.bottom += h;
		exStyle = GetWindowLongPtrW(hParent, GWL_EXSTYLE);
		style = GetWindowLongPtrW(hParent, GWL_STYLE);
		AdjustWindowRectEx(&rc2, style, FALSE, exStyle);
		wp.rcNormalPosition.bottom = wp.rcNormalPosition.top;
		wp.rcNormalPosition.bottom += rc2.bottom - rc2.top;// + 8;

		MapWindowPoints(HWND_DESKTOP, hParent, (LPPOINT)&rc, 2);
		top = rc.bottom + of;
		width = rc.right - rc.left;
		height = rc.bottom - rc.top;
		m_hSelCombo = CreateWindowExW(WS_EX_CLIENTEDGE, L"COMBOBOX", NULL, CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_TABSTOP, rc.left, top, width, 400, hParent, NULL, GetModuleHandle(0), NULL);
		GetWindowRect(GetDlgItem(hParent, stc2), &rc);
		MapWindowPoints(HWND_DESKTOP, hParent, (LPPOINT)&rc, 2);
		width = rc.right - rc.left;
		hStatic = CreateWindowExW(0, L"STATIC", L"Encoding:", SS_CENTERIMAGE | WS_CHILD | WS_VISIBLE, rc.left, top, width, height, hParent, NULL, GetModuleHandle(0), NULL);
		hFont = (HFONT)SendMessageW(hParent, WM_GETFONT, 0, 0);
		SendMessageW(m_hSelCombo, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessageW(hStatic, WM_SETFONT, (WPARAM)hFont, TRUE);
		SetWindowPlacement(hParent, &wp);
		SendMessageW(m_hSelCombo, CB_ADDSTRING, 0, (LPARAM)ENC1);
		SendMessageW(m_hSelCombo, CB_ADDSTRING, 0, (LPARAM)ENC2);
		SendMessageW(m_hSelCombo, CB_ADDSTRING, 0, (LPARAM)ENC3);
		SendMessageW(m_hSelCombo, CB_ADDSTRING, 0, (LPARAM)ENC4);
		SendMessageW(m_hSelCombo, CB_SETCURSEL, 0, 0);
		SetWindowLongPtrW(hParent, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(hParent, GWLP_WNDPROC, (LONG_PTR)OFNParentProc));
	}
	return FALSE;
}

static LRESULT CALLBACK OFNParentProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	if(msg == WM_COMMAND){
		if(HIWORD(wParam) == CBN_SELENDOK){
			if((HWND)lParam == m_hSelCombo){
				m_CurrType = SendMessageW(m_hSelCombo, CB_GETCURSEL, 0, 0);
			}
		}
	}
	return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
}
