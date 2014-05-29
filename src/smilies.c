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
#include "stringconstants.h"
#include "shared.h"
#include "memorynotes.h"
#include "smilies.h"
#include "dataobject.h"
#include "note.h"

#define		PROP_IML		L"PROP_IML"
#define		PROP_TBR		L"PROP_TBR"

static void Smilies_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Smilies_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Smilies_OnClose(HWND hwnd);
static void Smilies_OnSize(HWND hwnd, UINT state, int cx, int cy);
static HBITMAP GetImlBitmapWithSolidBrush(HIMAGELIST hIml, HDC hdc, COLORREF clr, int index, int cx, int cy);
static HBITMAP GetImlBitmapWithPatternBrush(HIMAGELIST hIml, HWND hNote, HWND hEdit, int index, int cx, int cy);
static COLORREF REBackColor(HWND hNote);

BOOL CALLBACK Smilies_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, Smilies_OnCommand);
	HANDLE_MSG (hwnd, WM_INITDIALOG, Smilies_OnInitDialog);
	HANDLE_MSG (hwnd, WM_CLOSE, Smilies_OnClose);
	HANDLE_MSG (hwnd, WM_SIZE, Smilies_OnSize);

	default: return FALSE;
	}
}

static void Smilies_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	MoveWindow((HWND)GetPropW(hwnd, PROP_TBR), 0, 0, cx, cy, TRUE);
}

static void Smilies_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDCANCEL:
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		default:
			if(id >= 8000 && id <= 8122){
				HWND	hNote = GetParent(hwnd);
				HWND	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
				if(hNote && hEdit){
					LPRICHEDITOLE pREOle = NULL;
					SendMessageW(hEdit, EM_GETOLEINTERFACE, 0, (LPARAM)&pREOle);
					if(pREOle != NULL){
						if(g_RTHandles.hbSkin == NULL){
							HDC		hdc = GetDC(hNote);
							HBITMAP hbmp = GetImlBitmapWithSolidBrush((HIMAGELIST)GetPropW(hwnd, PROP_IML), hdc, REBackColor(hNote), id - 8000, 18, 18);
							InsertBitmap(pREOle, hbmp);
							pREOle->lpVtbl->Release(pREOle);
							DeleteBitmap(hbmp);
							ReleaseDC(hNote, hdc);
						}
						else{
							HBITMAP hbmp = GetImlBitmapWithPatternBrush((HIMAGELIST)GetPropW(hwnd, PROP_IML), hNote, hEdit, id - 8000, 18, 18);
							InsertBitmap(pREOle, hbmp);
							pREOle->lpVtbl->Release(pREOle);
							DeleteBitmap(hbmp);
						}
						//save edit box contents to temporary file and load from it
						ExchangeSameRE(hEdit);
						SendMessageW(hwnd, WM_CLOSE, 0, 0);
					}
				}
			}
			break;
	}
}

static BOOL Smilies_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT		rc;
	HWND		hTbr = GetDlgItem(hwnd, IDC_TBR_SMILIES);		
	HIMAGELIST	hIml = ImageList_Create(18, 18, ILC_COLOR32 | ILC_MASK, 0, 1);
	wchar_t 	szBuffer[256];

	//set dialog and controls text
	GetPrivateProfileStringW(S_OPTIONS, L"1033", L"Smilies", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);

	SetPropW(hwnd, PROP_IML, (HANDLE)hIml);
	SetPropW(hwnd, PROP_TBR, (HANDLE)hTbr);
	SendMessage(hTbr, TB_SETIMAGELIST, 0, (LPARAM)hIml);
	EnumResourceNames(g_hSmilies, RT_BITMAP, EnumResNameProc, (LPARAM)hIml);
	TBBUTTON	tbb;
	int count = ImageList_GetImageCount(hIml);
	for(int i = 0; i < count; i++){
		ZeroMemory(&tbb, sizeof(tbb));
		tbb.iBitmap = i;
		tbb.fsStyle = TBSTYLE_BUTTON;
		tbb.fsState = TBSTATE_ENABLED;
		tbb.idCommand = 8000 + i;
		SendMessage(hTbr, TB_ADDBUTTONS, 1 , (LPARAM)&tbb);
	}
	DWORD		size = SendMessage(hTbr, TB_GETBUTTONSIZE, 0, 0);
	WORD		width = LOWORD(size), height = HIWORD(size);
	SetRect(&rc, 0, 0, width * 11, height * 11);
	AdjustWindowRectEx(&rc, GetWindowLongPtr(hwnd, GWL_STYLE), FALSE, GetWindowLongPtr(hwnd, GWL_EXSTYLE));
	SetWindowPos(hwnd, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
	return FALSE;
}

static void Smilies_OnClose(HWND hwnd)
{
	RemovePropW(hwnd, PROP_IML);
	RemovePropW(hwnd, PROP_TBR);
	EndDialog(hwnd, 0);
}

static HBITMAP GetImlBitmapWithPatternBrush(HIMAGELIST hIml, HWND hNote, HWND hEdit, int index, int cx, int cy){
	HDC			hdcTemp, hdcBack, hdcNote, hdcSkin;
	HBITMAP		hBmpTemp, hBmpOld, hBmpBack, hBmpPattern, hBmpSkinOld;
	RECT		rc;
	HBRUSH		hbr;
	PMEMNOTE	pNote;
	CHARRANGE	cr;
	POINT		pt;

	//get MEMNOTE object
	pNote = MemNoteByHwnd(hNote);
	if(pNote == NULL)
		return NULL;
	//get current rich edit box selection index
	cr.cpMax = cr.cpMin = 0;
	SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&cr);
	//get coordinates of current rich edit box selection
	pt.x = pt.y = 0;
	SendMessageW(hEdit, EM_POSFROMCHAR, (WPARAM)&pt, cr.cpMin);
	//prepare smiley rectangle
	SetRect(&rc, 0, 0, cx, cy);
	//get DC of note
	hdcNote = GetDC(hNote);
	//create temporary DCs
	hdcTemp = CreateCompatibleDC(hdcNote);
	hdcBack = CreateCompatibleDC(hdcNote);
	hdcSkin = CreateCompatibleDC(hdcNote);
	//create temporary bitmaps
	hBmpTemp = CreateCompatibleBitmap(hdcNote, cx, cy);
	hBmpPattern = CreateCompatibleBitmap(hdcNote, cx, cy);
	//release note DC
	ReleaseDC(hNote, hdcNote);
	//select pattern bitmap in temporary DC
	hBmpBack = SelectBitmap(hdcBack, hBmpPattern);
	//select note skin in temporary DC
	hBmpSkinOld = SelectBitmap(hdcSkin, pNote->pRTHandles->hbSkin);
	//copy needed portion of note skin to temporary bitmap
	BitBlt(hdcBack, 0, 0, cx, cy, hdcSkin, pt.x + pNote->pRTHandles->rcEdit.left, pt.y + pNote->pRTHandles->rcEdit.top, SRCCOPY);
	//restore temporary skin DC
	SelectBitmap(hdcSkin, hBmpSkinOld);
	//select pattern bitmap back
	SelectBitmap(hdcBack, hBmpBack);
	//select smiley bitmap in temporary DC
	hBmpOld = SelectBitmap(hdcTemp, hBmpTemp);
	//create brush from pattern bitmap
	hbr = CreatePatternBrush(hBmpPattern);
	//fill rectangle with pattern brush
	FillRect(hdcTemp, &rc, hbr);
	//draw smiley onto psmiley bitmap
	ImageList_Draw(hIml, index, hdcTemp, 0, 0, ILD_TRANSPARENT);
	//select smiley bitmap back
	SelectBitmap(hdcTemp, hBmpOld);
	//delete unneded objects
	DeleteBrush(hbr);
	DeleteBitmap(hBmpPattern);
	DeleteDC(hdcSkin);
	DeleteDC(hdcBack);
	DeleteDC(hdcTemp);
	//return smiley bitmap
	return hBmpTemp;
}

static HBITMAP GetImlBitmapWithSolidBrush(HIMAGELIST hIml, HDC hdc, COLORREF clr, int index, int cx, int cy){
	HDC			hdcTemp;
	HBITMAP		hBmpTemp, hBmpOld;
	RECT		rc;
	HBRUSH		hbr;
	
	//prepare smiley rectangle
	SetRect(&rc, 0, 0, cx, cy);
	//create temporary DC
	hdcTemp = CreateCompatibleDC(hdc);
	//create smiley bitmap
	hBmpTemp = CreateCompatibleBitmap(hdc, cx, cy);
	//select smiley bitmap in temporary DC
	hBmpOld = SelectBitmap(hdcTemp, hBmpTemp);
	//create solid brush
	hbr = CreateSolidBrush(clr);
	//fill smiley rectangle
	FillRect(hdcTemp, &rc, hbr);
	//draw smiley onto smiley bitmap
	ImageList_Draw(hIml, index, hdcTemp, 0, 0, ILD_TRANSPARENT);
	//select smiley bitmap back
	SelectBitmap(hdcTemp, hBmpOld);
	//delete unneded object
	DeleteBrush(hbr);
	DeleteDC(hdcTemp);
	//return smiley bitmap
	return hBmpTemp;
}

static COLORREF REBackColor(HWND hNote){
	PMEMNOTE		pNote = MemNoteByHwnd(hNote);

	if(pNote)
		return pNote->pAppearance->crWindow;
	else
		return RGB(255, 255, 255);
}
