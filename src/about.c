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

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <wchar.h>
#include <commctrl.h>

#include "linklabel.h"
#include "about.h"
#include "shared.h"
#include "numericconstants.h"

/** Defines ********************************************************/
#define	HOME_PAGE				"http://pnotes.sf.net"
#define	EMAIL_ADDRESS			L"andrey.gruber@gmail.com"
#define	PNGLIB_ADDRESS			L"http://www.madwizard.org/programming/projects/pnglib"
#define	HUNSPELL_ADDRESS		L"http://hunspell.sourceforge.net/"
/** Prototypes ********************************************************/
static void CreateAboutString(HINSTANCE hInstance, wchar_t * lpLangFile);
static LRESULT CALLBACK About_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void About_OnClose(HWND hwnd);
static void About_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL About_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void About_OnPaint(HWND hwnd);

#define	FORMAT_STRING		"\\StringFileInfo\\%04x%04x\\%s"
#define	ABOUT_CREDITS		L"Oliver Sahr, Horst Epp, Oliver Crystal, "\
							L"Sergey Hristov, Oleg, nascent-, "\
							L"nomnex, David Naylor, zw963, "\
							L"baggio1987, rickytbf, dintyma, "\
							L"aquabon, juapar2, Vincent Duvernet, "\
							L"jpdag, Raleigh034, Sven, "\
							L"tvrman, sigei, jbsiu, "\
							L"batzilis, pbosoni, patswovelin, "\
							L"tullio99, btriffles, ucr, "\
							L"winds350, snorrt, hwq0086, "\
							L"Tsprajna, tremblsi, Lukas Baron, "\
							L"bejobe, benjamindover, shigeoyoshio, "\
							L"wolfeden3, eran, Christopher Freel, "\
							L"k0shyx, Pavlos Kotronakis, Andrey Tuliev, "\
							L"sh_666, Jose Jamez, Wolfram Schelzel, "\
							L"cominder, gino1307, charles80, kronofogd, "\
							L"Holger Stohr, wieselmann, pnoteslover, "\
							L"thegriff, Manish Kumar"

/** Module variables ********************************************************/
static HICON				m_hAboutIcon;
static HFONT				m_hAboutFont;
static wchar_t				m_sShowLicense[128], m_sName[128], m_sAbout[1024], m_sLangFile[MAX_PATH], m_sPing[512], m_sHunspell[512];
static RECT					m_rcPing = {0}, m_rcHunspell = {0};

static void CreateAboutString(HINSTANCE hInstance, wchar_t * lpLangFile){

	char 		szPath[MAX_PATH], szBuffer[MAX_PATH], szAbout[1024];
	wchar_t		szTemp[1024];
	DWORD 		dwSize, dwBytes = 0;
	HGLOBAL 	hMem;
	UINT 		cbLang;
	LPVOID 		lpt;
	UINT 		cbBuffSize;
	WORD 		* langInfo;

	GetPrivateProfileStringW(L"about", L"show_license", L"Show license", m_sShowLicense, 128, lpLangFile);
	GetPrivateProfileStringW(L"about", L"dialog", L"About", m_sName, 128, lpLangFile);
	wcscat(m_sName, L" - ");

	GetModuleFileName(hInstance, szPath, MAX_PATH);
	dwBytes = GetFileVersionInfoSize(szPath, &dwSize);
	if(dwBytes){
		hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, dwBytes);
		if(GetFileVersionInfo(szPath, 0, dwBytes, hMem)){
			if(VerQueryValue(hMem, _T("\\VarFileInfo\\Translation"), (LPVOID*)&langInfo, &cbLang)){
				//InternalName
				wsprintf(szBuffer, FORMAT_STRING, langInfo[0], langInfo[1], _T("InternalName"));
				if(VerQueryValue(hMem, szBuffer, &lpt, &cbBuffSize)){
					strcpy(szAbout, (LPTSTR)lpt);
					strcat(szAbout, _T(" - "));
					MultiByteToWideChar(CP_ACP, 0, (LPTSTR)lpt, -1, szTemp, 128);
					wcscat(m_sName, szTemp);
				}
				//FileVersion
				wsprintf(szBuffer, FORMAT_STRING, langInfo[0], langInfo[1], _T("FileVersion"));
				if(VerQueryValue(hMem, szBuffer, &lpt, &cbBuffSize)){
					strcat(szAbout, (LPTSTR)lpt);
					strcat(szAbout, _T("\n"));
				}
				//FileDescription
				wsprintf(szBuffer, FORMAT_STRING, langInfo[0], langInfo[1], _T("FileDescription"));
				if(VerQueryValue(hMem, szBuffer, &lpt, &cbBuffSize)){
					strcat(szAbout, (LPTSTR)lpt);
					strcat(szAbout, _T("\n"));
				}
				//LegalCopyright
				wsprintf(szBuffer, FORMAT_STRING, langInfo[0], langInfo[1], _T("LegalCopyright"));
				if(VerQueryValue(hMem, szBuffer, &lpt, &cbBuffSize)){
					strcat(szAbout, (LPTSTR)lpt);
					strcat(szAbout, _T("\n"));
				}
				MultiByteToWideChar(CP_ACP, 0, szAbout, -1, szTemp, 1024);
				wcscpy(m_sAbout, szTemp);
				//License
				GetPrivateProfileStringW(L"about", L"license", L"This program is distributed under the terms of the GNU General Public License version 2 or later", szTemp, 512, lpLangFile);
				wcscat(m_sAbout, szTemp);
			}
		}
		GlobalFree(hMem);
	}
}

void CreateAboutDialog(HWND hwnd, HINSTANCE hInstance, HICON hIcon, wchar_t * lpLangFile){
	wcscpy(m_sLangFile, lpLangFile);
	CreateAboutString(hInstance, lpLangFile);
	DialogBoxParamW(hInstance, MAKEINTRESOURCEW(IDD_ABOUT), hwnd, (DLGPROC)About_DlgProc, (LPARAM)hIcon);
}

static LRESULT CALLBACK About_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_CLOSE, About_OnClose);
	HANDLE_MSG (hwnd, WM_COMMAND, About_OnCommand);
	HANDLE_MSG (hwnd, WM_INITDIALOG, About_OnInitDialog);
	HANDLE_MSG (hwnd, WM_PAINT, About_OnPaint);

	case WM_NOTIFY:{
		LPNMHDR lpnmh = (LPNMHDR)lParam;
		if(lpnmh->idFrom == IDC_EMAIL && lpnmh->code == NM_CLICK){
			char	szAddress[128];
			strcpy(szAddress, "mailto:");
			strcat(szAddress, LLGetText(GetDlgItem(hwnd, IDC_EMAIL)));
			ShellExecute(hwnd, _T("open"), szAddress, NULL, NULL, SW_SHOWDEFAULT);
		}
		else if(lpnmh->idFrom == IDC_PNGLIB_ADDRESS && lpnmh->code == NM_CLICK){
			wchar_t	szPage[512];
			wcscpy(szPage, LLGetTextW(GetDlgItem(hwnd, IDC_PNGLIB_ADDRESS)));
			SendMessageW(g_hMain, PNM_OPEN_PAGE, 0, (LPARAM)szPage);
		}
		else if(lpnmh->idFrom == IDC_HUNSPELL_ADDRESS && lpnmh->code == NM_CLICK){
			wchar_t	szPage[512];
			wcscpy(szPage, LLGetTextW(GetDlgItem(hwnd, IDC_HUNSPELL_ADDRESS)));
			SendMessageW(g_hMain, PNM_OPEN_PAGE, 0, (LPARAM)szPage);
		}
		return TRUE;
	}
	default: return FALSE;
	}
}

static void About_OnPaint(HWND hwnd)
{
	PAINTSTRUCT		ps;
	HFONT 			hOldFont;

	BeginPaint(hwnd, &ps);
	hOldFont = SelectFont(ps.hdc, GetWindowFont(hwnd));
	SetBkMode(ps.hdc, TRANSPARENT);
	DrawTextW(ps.hdc, m_sPing, -1, &m_rcPing, DT_CENTER | DT_WORDBREAK);
	DrawTextW(ps.hdc, m_sHunspell, -1, &m_rcHunspell, DT_CENTER | DT_WORDBREAK);
	SelectFont(ps.hdc, hOldFont);
	EndPaint(hwnd, &ps);
}

static void About_OnClose(HWND hwnd)
{
	g_hAbout = NULL;
	DeleteFont(m_hAboutFont);
	if(g_hLastModal == hwnd)
		EndDialog(hwnd, 0);
	else
		DestroyWindow(hwnd);
}

static void About_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id){
		case IDOK:
		case IDCANCEL:
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDC_SHOW_LICENSE:
			ShellExecute(hwnd, "open", "License.txt", NULL, NULL, SW_SHOWNORMAL);
			break;
	}
}

static BOOL About_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT		rc, rc2, rcText;
	HWND		hLabel;
	wchar_t		szTemp[512];
	int			height;
	HDC 		hdc;
	HFONT 		hOldFont;

	g_hAbout = hwnd;
	g_hLastModal = hwnd;
	m_hAboutIcon = (HICON)lParam;
	m_hAboutFont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
	SetWindowTextW(hwnd, m_sName);
	GetPrivateProfileStringW(L"options", L"1", L"OK", szTemp, 512, m_sLangFile);
	SetDlgItemTextW(hwnd, IDOK, szTemp);
	SetDlgItemTextW(hwnd, IDC_SHOW_LICENSE, m_sShowLicense);
	CenterWindow(hwnd, FALSE);
	SendDlgItemMessageW(hwnd, IDC_ST_ABOUT_ICON, STM_SETICON, (WPARAM)(HICON)lParam, 0);
	SetDlgItemTextW(hwnd, IDC_ST_ABOUT_STRING, m_sAbout);
	GetClientRect(hwnd, &rc);
	hLabel = CreateLinkLableW(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), EMAIL_ADDRESS, 0, 0, 120, 20, IDC_EMAIL, FALSE);
	GetWindowRect(hLabel, &rc2);
	GetWindowRect(GetDlgItem(hwnd, IDC_ST_ABOUT_STRING), &rcText);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rcText, 2);
	SetWindowPos(hLabel, 0, ((rc.right - rc.left) - (rc2.right - rc2.left)) / 2, rcText.bottom, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	SetDlgItemTextW(hwnd, IDC_EDI_CREDITS, ABOUT_CREDITS);

	GetPrivateProfileStringW(L"about", L"thanks_1", L"Special thanks for significant remarks, fresh ideas, suggestions and selfless testing to:", szTemp, 512, m_sLangFile);
	SetDlgItemTextW(hwnd, IDC_ST_CREDITS_1, szTemp);
	GetPrivateProfileStringW(L"about", L"thanks_2", L"Thanks to all over the world who tests, translates and simply uses the program.", szTemp, 512, m_sLangFile);
	SetDlgItemTextW(hwnd, IDC_ST_CREDITS_2, szTemp);
	// GetPrivateProfileStringW(L"about", L"png_lib", L"PNG images decoding - PNGlib, a free PNG decoder library by Thomas Bleeker.", szTemp, 512, m_sLangFile);
	// SetDlgItemTextW(hwnd, IDC_ST_PNGLIB, szTemp);
	// GetPrivateProfileStringW(L"about", L"hunspell", L"Spell checking - Hunspell library by Hunspell.", szTemp, 512, m_sLangFile);
	// SetDlgItemTextW(hwnd, IDC_ST_HUNSPELL, szTemp);
	
	GetPrivateProfileStringW(L"about", L"png_lib", L"PNG images decoding - PNGlib, a free PNG decoder library by Thomas Bleeker.", m_sPing, 512, m_sLangFile); 
	GetPrivateProfileStringW(L"about", L"hunspell", L"Spell checking - Hunspell library by Hunspell.", m_sHunspell, 512, m_sLangFile);
	
	GetWindowRect(GetDlgItem(hwnd, IDC_ST_PNGLIB), &m_rcPing);
	GetWindowRect(GetDlgItem(hwnd, IDC_ST_HUNSPELL), &m_rcHunspell);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&m_rcPing, 2);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&m_rcHunspell, 2);
	hdc = GetDC(hwnd);
	hOldFont = SelectFont(hdc, GetWindowFont(hwnd));
	CopyRect(&rcText, &m_rcPing);
	height = DrawTextW(hdc, m_sPing, -1, &m_rcPing, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
	m_rcPing.bottom = rcText.bottom;
	m_rcPing.top = m_rcPing.bottom - height;
	m_rcPing.right = rcText.right;
	CopyRect(&rcText, &m_rcHunspell);
	height = DrawTextW(hdc, m_sHunspell, -1, &m_rcHunspell, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
	m_rcHunspell.bottom = rcText.bottom;
	m_rcHunspell.top = m_rcHunspell.bottom - height;
	m_rcHunspell.right = rcText.right;
	SelectFont(hdc, hOldFont);
	ReleaseDC(hwnd, hdc);

	hLabel = CreateLinkLableW(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), PNGLIB_ADDRESS, 0, 0, rc.right - rc.left, 20, IDC_PNGLIB_ADDRESS, FALSE);
	GetWindowRect(hLabel, &rc2);
	GetWindowRect(GetDlgItem(hwnd, IDC_ST_PNGLIB), &rcText);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rcText, 2);
	SetWindowPos(hLabel, 0, ((rc.right - rc.left) - (rc2.right - rc2.left)) / 2, rcText.bottom, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

	hLabel = CreateLinkLableW(hwnd, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), HUNSPELL_ADDRESS, 0, 0, rc.right - rc.left, 20, IDC_HUNSPELL_ADDRESS, FALSE);
	GetWindowRect(hLabel, &rc2);
	GetWindowRect(GetDlgItem(hwnd, IDC_ST_HUNSPELL), &rcText);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rcText, 2);
	SetWindowPos(hLabel, 0, ((rc.right - rc.left) - (rc2.right - rc2.left)) / 2, rcText.bottom, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

	return TRUE;
}
