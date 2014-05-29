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

#include "notes.h"
#include "combodialog.h"
#include "global.h"
#include "numericconstants.h"
#include "shared.h"
#include "note.h"
#include "enums.h"

/** Prototypes ********************************************************/
static LRESULT CALLBACK Combo_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Combo_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Combo_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Combo_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static int CALLBACK EnumFontFamProcW(
  ENUMLOGFONTW *lpelf,    // logical-font data
  NEWTEXTMETRICW *lpntm,  // physical-font data
  DWORD FontType,        // type of font
  LPARAM lParam          // application-defined data
);
static LRESULT CALLBACK CFont_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void CFont_OnKillFocus(HWND hwnd, HWND hwndNewFocus);
static void ShowFontCombo(HWND hDlg);
static void CloseDialogCombo(HWND hwnd, int result);

static CBCREATION		m_cbr;

void CreateComboDialog(P_CBCREATION pcbr, HINSTANCE hInstance){
	DialogBoxParamW(hInstance, MAKEINTRESOURCEW(DLG_COMBO), pcbr->hParent, (DLGPROC)Combo_DlgProc, (LPARAM)pcbr);
}

static LRESULT CALLBACK Combo_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, Combo_OnCommand);
	HANDLE_MSG (hwnd, WM_INITDIALOG, Combo_OnInitDialog);
	HANDLE_MSG (hwnd, WM_DRAWITEM, Combo_OnDrawItem);
	default: return FALSE;
	}
}

static void Combo_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
	case IDCANCEL:
		CloseDialogCombo(hwnd, 0);
		break;
	case IDC_COMBO_FONT:
		if(codeNotify == CBN_SELENDOK){
			//get selected font name
			int index = SendDlgItemMessageW(hwnd, IDC_COMBO_FONT, CB_GETCURSEL, 0, 0);
			SendDlgItemMessageW(hwnd, IDC_COMBO_FONT, CB_GETLBTEXT, index, (LPARAM)g_SelectedFont.szFace);
			g_SelectedFont.nCharSet = SendDlgItemMessageW(hwnd, IDC_COMBO_FONT, CB_GETITEMDATA, index, 0);
			//if(wcscmp(g_CurrentFontFace, g_SelectedFont.szFace))
				//change font
				SendMessageW(GetParent(m_cbr.hButton), PNM_NEW_FONT_FACE, 0, 0);
			CloseDialogCombo(hwnd, 0);
		}
	}
}

static void Combo_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	LOGFONTW		lf;
	HFONT			hFont, hFontOld;
	wchar_t			szBuffer[256];
	RECT			rc;

	//draw fonts
	SetBkMode(lpDrawItem->hDC, TRANSPARENT);
	if(lpDrawItem->itemID != -1){
		CopyRect(&rc, &lpDrawItem->rcItem);
		if((lpDrawItem->itemState & ODS_SELECTED) == ODS_SELECTED){
			FillRect(lpDrawItem->hDC, &rc, GetSysColorBrush(COLOR_HIGHLIGHT));
			SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		}
		else{
			FillRect(lpDrawItem->hDC, &rc, GetSysColorBrush(COLOR_WINDOW));
			SetTextColor(lpDrawItem->hDC, GetSysColor(COLOR_WINDOWTEXT));
		}
		
		SendDlgItemMessageW(hwnd, IDC_COMBO_FONT, CB_GETLBTEXT, (WPARAM)lpDrawItem->itemID, (LPARAM)szBuffer);
		OffsetRect(&rc, 4, 0);
		DrawTextW(lpDrawItem->hDC, szBuffer, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		ZeroMemory(&lf, sizeof(lf));
		lf.lfHeight = -MulDiv(8, GetDeviceCaps(lpDrawItem->hDC, LOGPIXELSY), 72);
		wcscpy(lf.lfFaceName, szBuffer);
		lf.lfQuality = PROOF_QUALITY;
		lf.lfPitchAndFamily = VARIABLE_PITCH;
		lf.lfOutPrecision = OUT_TT_PRECIS;
		lf.lfCharSet = DEFAULT_CHARSET;
		hFont = CreateFontIndirectW(&lf);
		hFontOld = SelectFont(lpDrawItem->hDC, hFont);
		OffsetRect(&rc, -8, 0);
		DrawTextW(lpDrawItem->hDC, L"Sample", -1, &rc, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
		SelectFont(lpDrawItem->hDC, hFontOld);
		DeleteFont(hFont);
	}
}

static BOOL Combo_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{

	HWND		hComboFont;
	HDC			hdc;
	WNDPROC		hProc;

	*g_SelectedFont.szFace = '\0';
	g_SelectedFont.nCharSet = 0;
	memcpy(&m_cbr, (P_CBCREATION)lParam, sizeof(m_cbr));
	g_hComboDialog = hwnd;
	hdc = GetDC(hwnd);
	hComboFont = GetDlgItem(hwnd, IDC_COMBO_FONT);
	hProc = (WNDPROC)SetWindowLongPtrW(hComboFont, GWLP_WNDPROC, (LONG_PTR)CFont_WndProc);
	SetWindowLongPtrW(hComboFont, GWLP_USERDATA, (LONG_PTR)hProc);
	//store fonts
	EnumFontFamiliesW(hdc, NULL, (FONTENUMPROCW)EnumFontFamProcW, (LPARAM)hComboFont);
	ReleaseDC(hwnd, hdc);
	if(GetDlgCtrlID(m_cbr.hButton) == CMD_FONT + 5000)
		ShowFontCombo(hwnd);
	return TRUE;
}

static int CALLBACK EnumFontFamProcW(
  ENUMLOGFONTW *lpelf,    // logical-font data
  NEWTEXTMETRICW *lpntm,  // physical-font data
  DWORD FontType,        // type of font
  LPARAM lParam          // application-defined data
){
	int		item;

	item = SendMessageW((HWND)lParam, CB_ADDSTRING, 0, (LPARAM)lpelf->elfLogFont.lfFaceName);
	SendMessageW((HWND)lParam, CB_SETITEMDATA, (WPARAM)item, (LPARAM)lpelf->elfLogFont.lfCharSet);
	return 1;
}

static LRESULT CALLBACK CFont_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_KILLFOCUS, CFont_OnKillFocus);
	default: return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
	}
}

static void CFont_OnKillFocus(HWND hwnd, HWND hwndNewFocus)
{
	if(hwndNewFocus != GetParent(m_cbr.hButton) && hwndNewFocus != m_cbr.hParent && hwndNewFocus != m_cbr.hEdit)
		ShowWindow(GetParent(m_cbr.hButton), SW_HIDE);
	CloseDialogCombo(GetParent(hwnd), 0);
}

static void CloseDialogCombo(HWND hwnd, int result){
	RedrawWindow(GetParent(GetParent(m_cbr.hButton)), NULL, NULL, RDW_INVALIDATE);
	ShowNoteMarks(GetParent(GetParent(m_cbr.hButton)));
	EndDialog(hwnd, result);
}

static void ShowFontCombo(HWND hDlg){
	HWND		hCombo;
	RECT		rcCombo, rcButton;
	int			index;

	hCombo = GetDlgItem(hDlg, IDC_COMBO_FONT);
	GetWindowRect(hCombo, &rcCombo);
	GetWindowRect(m_cbr.hButton, &rcButton);
	MoveWindow(hDlg, rcButton.left, rcButton.bottom, rcCombo.right - rcCombo.left, rcCombo.bottom - rcCombo.top, TRUE);
	index = SendMessageW(hCombo, CB_FINDSTRINGEXACT, -1, (LPARAM)g_CurrentFontFace);
	if(index != CB_ERR)
		SendMessageW(hCombo, CB_SETCURSEL, index, 0);
	ShowWindow(hDlg, SW_SHOW);
}
