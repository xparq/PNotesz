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
#include "global.h"
#include "numericconstants.h"
#include "skinlessdrawing.h"
#include "shared.h"
#include "docking.h"
#include "memorynotes.h"
#include "enums.h"

/** Prototypes ********************************************************/
static void CDrawFont(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawFontSize(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawFontColor(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawBold(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawItalic(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawUnderline(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawStrikethrough(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawLeftAlign(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawRightAlign(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawCenterAlign(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawHighlight(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawBullets(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void SysDrawHide(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void SysDrawDelete(HDC hdc, P_NOTE_APPEARANCE pA, int offset);

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateSkinlessSysBitmap
 Created  : Tue Jan  8 12:11:19 2008
 Modified : Tue Jan  8 12:11:19 2008

 Synopsys : Creates bitmap for skinless "delete" and "hide" buttons
 Input    : hwnd - note window handle
            pA - note appearance
            pH - note run-time handles
            phbmp - pointer to HBITMAP will be created
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void CreateSkinlessSysBitmap(HWND hwnd, P_NOTE_APPEARANCE pA, P_NOTE_RTHANDLES pH, HBITMAP * phbmp){
	RECT		rc;
	HDC			hdcMem, hdcSrc;
	HBITMAP		hBmpOld, hBmpMem;
	HBRUSH		hBrush;
	int			offset = 0;

	hdcSrc = GetDC(hwnd);
	hdcMem = CreateCompatibleDC(hdcSrc);
	SetRect(&rc, 0, 0, SMALL_RECT * CMD_SYS_BUTTONS, SMALL_RECT);
	hBmpMem = CreateCompatibleBitmap(hdcSrc, SMALL_RECT * CMD_SYS_BUTTONS, SMALL_RECT);
	SetBkMode(hdcMem, TRANSPARENT);
	hBmpOld = SelectBitmap(hdcMem, hBmpMem);
	hBrush = CreateSolidBrush(pH->crMask);
	FillRect(hdcMem, &rc, hBrush);
	DeleteBrush(hBrush);
	for(int i = 0; i < CMD_SYS_BUTTONS; i++){
		switch(i + IDM_HIDE){
		case IDM_HIDE:
			SysDrawHide(hdcMem, pA, offset);
			break;
		case IDM_DEL:
			SysDrawDelete(hdcMem, pA, offset);
			break;
		}
		offset += SMALL_RECT;
	}
	hBmpMem = SelectBitmap(hdcMem, hBmpOld);
	if(*phbmp)
		DeleteBitmap(*phbmp);
	*phbmp = hBmpMem;
	DeleteDC(hdcMem);
	ReleaseDC(hwnd, hdcSrc);
}

void CreateSimpleMarksBitmap(HWND hwnd, HBITMAP * phbmp, int nMarksCount){
	HBITMAP				hbmp, hbmpOld;
	HDC					hdcSrc, hdcDest;
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	COLORREF			crWindow, crCaption;
	HBRUSH				hBrush, hTextBrush, hBrushOld;
	HPEN				hPen, hOldPen;
	RECT				rc, rc1;

	//get needed parameters
	if(DockType(pNote->pData->dockData) != DOCK_NONE && g_DockSettings.fCustColor)
		crWindow = g_DockSettings.crWindow;
	else
		crWindow = pNote->pAppearance->crWindow;
	if(DockType(pNote->pData->dockData) != DOCK_NONE && g_DockSettings.fCustCaption){
		crCaption = g_DockSettings.crCaption;
	}
	else{
		crCaption = pNote->pAppearance->crCaption;
	}
	//create solid brush with needed color
	hBrush = CreateSolidBrush(crWindow);
	//create solid pen with caption font color
	hPen = CreatePen(PS_SOLID, 1, crCaption);
	//create text brush
	hTextBrush = CreateSolidBrush(crCaption);

	hdcSrc = GetDC(hwnd);
	hbmp = CreateCompatibleBitmap(hdcSrc, SMALL_RECT * nMarksCount, SMALL_RECT);
	hdcDest = CreateCompatibleDC(hdcSrc);
	SetBkMode(hdcDest, TRANSPARENT);
	hbmpOld = SelectBitmap(hdcDest, hbmp);
	hOldPen = SelectPen(hdcDest, hPen);
	SetRect(&rc, 0, 0, SMALL_RECT * nMarksCount, SMALL_RECT);
	FillRect(hdcDest, &rc, hBrush);
	hBrushOld = SelectBrush(hdcDest, hBrush);
	//draw "scheduled" sign
	SetRect(&rc, 0, 0, SMALL_RECT, SMALL_RECT);
	Ellipse(hdcDest, rc.left, rc.top, rc.right, rc.bottom);
	MoveToEx(hdcDest, rc.left + 5, rc.top + 2, NULL);
	LineTo(hdcDest, rc.left + 5, rc.top + 6);
	LineTo(hdcDest, rc.left + 7, rc.top + 6);
	//draw "changed" sign
	OffsetRect(&rc, SMALL_RECT, 0);
	CopyRect(&rc1, &rc);
	rc1.left += 4;
	rc1.top += 1;
	FrameRect(hdcDest, &rc1, hTextBrush);
	MoveToEx(hdcDest, rc.left, rc.top, NULL);
	LineTo(hdcDest, rc.left + 7, rc.top + 7);
	//draw "protected" sign
	OffsetRect(&rc, SMALL_RECT, 0);
	MoveToEx(hdcDest, rc.left + 4, rc.top, NULL);
	LineTo(hdcDest, rc.left + 8, rc.top);
	MoveToEx(hdcDest, rc.left + 2, rc.top + 2, NULL);
	LineTo(hdcDest, rc.left + 5, rc.top - 1);
	MoveToEx(hdcDest, rc.left + 7, rc.top, NULL);
	LineTo(hdcDest, rc.left + 10, rc.top + 3);
	MoveToEx(hdcDest, rc.left + 2, rc.top + 2, NULL);
	LineTo(hdcDest, rc.left + 2, rc.top + 6);
	MoveToEx(hdcDest, rc.left + 9, rc.top + 2, NULL);
	LineTo(hdcDest, rc.left + 9, rc.top + 6);
	MoveToEx(hdcDest, rc.left + 5, rc.top + 2, NULL);
	LineTo(hdcDest, rc.left + 7, rc.top + 2);
	MoveToEx(hdcDest, rc.left + 4, rc.top + 3, NULL);
	LineTo(hdcDest, rc.left + 4, rc.top + 6);
	MoveToEx(hdcDest, rc.left + 7, rc.top + 3, NULL);
	LineTo(hdcDest, rc.left + 7, rc.top + 6);
	MoveToEx(hdcDest, rc.left + 1, rc.top + 5, NULL);
	LineTo(hdcDest, rc.left + 3, rc.top + 5);
	MoveToEx(hdcDest, rc.left + 5, rc.top + 5, NULL);
	LineTo(hdcDest, rc.left + 8, rc.top + 5);
	MoveToEx(hdcDest, rc.left + 9, rc.top + 5, NULL);
	LineTo(hdcDest, rc.left + 11, rc.top + 5);
	MoveToEx(hdcDest, rc.left + 0, rc.top + 6, NULL);
	LineTo(hdcDest, rc.left + 0, rc.top + 11);
	MoveToEx(hdcDest, rc.left + 11, rc.top + 6, NULL);
	LineTo(hdcDest, rc.left + 11, rc.top + 11);
	MoveToEx(hdcDest, rc.left + 1, rc.top + 11, NULL);
	LineTo(hdcDest, rc.left + 11, rc.top + 11);
	MoveToEx(hdcDest, rc.left + 4, rc.top + 8, NULL);
	LineTo(hdcDest, rc.left + 8, rc.top + 8);
	MoveToEx(hdcDest, rc.left + 5, rc.top + 9, NULL);
	LineTo(hdcDest, rc.left + 7, rc.top + 9);
	//draw priority sign
	OffsetRect(&rc, SMALL_RECT, 0);
	MoveToEx(hdcDest, rc.left + 1, rc.top + 1, NULL);
	LineTo(hdcDest, rc.left + 7, rc.top + 12);
	MoveToEx(hdcDest, rc.left + 4, rc.top + 7, NULL);
	LineTo(hdcDest, rc.left + 12, rc.top + 3);
	MoveToEx(hdcDest, rc.left + 1, rc.top + 1, NULL);
	LineTo(hdcDest, rc.left + 12, rc.top + 2);
	//draw completed sign
	OffsetRect(&rc, SMALL_RECT, 0);
	MoveToEx(hdcDest, rc.left + 1, rc.top + 6, NULL);
	LineTo(hdcDest, rc.left + 7, rc.top + 12);
	MoveToEx(hdcDest, rc.left + 6, rc.top + 11, NULL);
	LineTo(hdcDest, rc.left + 12, rc.top);
	//draw key sign
	OffsetRect(&rc, SMALL_RECT, 0);
	MoveToEx(hdcDest, rc.left + 1, rc.top, NULL);
	LineTo(hdcDest, rc.left + 6, rc.top);
	MoveToEx(hdcDest, rc.left, rc.top + 1, NULL);
	LineTo(hdcDest, rc.left, rc.top + 6);
	MoveToEx(hdcDest, rc.left + 1, rc.top + 6, NULL);
	LineTo(hdcDest, rc.left + 4, rc.top + 6);
	LineTo(hdcDest, rc.left + 4, rc.top + 8);
	LineTo(hdcDest, rc.left + 6, rc.top + 8);
	LineTo(hdcDest, rc.left + 6, rc.top + 10);
	LineTo(hdcDest, rc.left + 8, rc.top + 10);
	LineTo(hdcDest, rc.left + 8, rc.top + 11);
	LineTo(hdcDest, rc.left + 11, rc.top + 11);
	LineTo(hdcDest, rc.left + 11, rc.top + 10);
	LineTo(hdcDest, rc.left + 6, rc.top + 5);
	LineTo(hdcDest, rc.left + 6, rc.top);
	MoveToEx(hdcDest, rc.left + 2, rc.top + 2, NULL);
	LineTo(hdcDest, rc.left + 4, rc.top + 2);
	LineTo(hdcDest, rc.left + 4, rc.top + 4);
	LineTo(hdcDest, rc.left + 2, rc.top + 4);
	LineTo(hdcDest, rc.left + 2, rc.top + 2);
	//draw pin
	OffsetRect(&rc, SMALL_RECT, 0);
	MoveToEx(hdcDest, rc.left + 7, rc.top, NULL);
	LineTo(hdcDest, rc.left + 11, rc.top);
	MoveToEx(hdcDest, rc.left + 6, rc.top + 1, NULL);
	LineTo(hdcDest, rc.left + 6, rc.top + 4);
	MoveToEx(hdcDest, rc.left + 11, rc.top + 1, NULL);
	LineTo(hdcDest, rc.left + 11, rc.top + 5);
	// MoveToEx(hdcDest, rc.left + 10, rc.top + 3, NULL);
	// LineTo(hdcDest, rc.left + 10, rc.top + 6);
	MoveToEx(hdcDest, rc.left + 3, rc.top + 3, NULL);
	LineTo(hdcDest, rc.left + 6, rc.top + 3);
	MoveToEx(hdcDest, rc.left + 8, rc.top + 5, NULL);
	LineTo(hdcDest, rc.left + 11, rc.top + 5);
	// MoveToEx(hdcDest, rc.left + 2, rc.top + 4, NULL);
	// LineTo(hdcDest, rc.left + 5, rc.top + 4);
	MoveToEx(hdcDest, rc.left + 2, rc.top + 4, NULL);
	LineTo(hdcDest, rc.left + 2, rc.top + 7);
	MoveToEx(hdcDest, rc.left + 8, rc.top + 5, NULL);
	LineTo(hdcDest, rc.left + 8, rc.top + 9);
	// MoveToEx(hdcDest, rc.left + 7, rc.top + 7, NULL);
	// LineTo(hdcDest, rc.left + 7, rc.top + 10);
	MoveToEx(hdcDest, rc.left + 5, rc.top + 9, NULL);
	LineTo(hdcDest, rc.left + 8, rc.top + 9);
	// MoveToEx(hdcDest, rc.left + 7, rc.top, NULL);
	// LineTo(hdcDest, rc.left + 10, rc.top + 3);
	// MoveToEx(hdcDest, rc.left + 7, rc.top + 1, NULL);
	// LineTo(hdcDest, rc.left + 10, rc.top + 4);
	// MoveToEx(hdcDest, rc.left + 6, rc.top + 3, NULL);
	// LineTo(hdcDest, rc.left + 8, rc.top + 5);
	// MoveToEx(hdcDest, rc.left + 4, rc.top + 4, NULL);
	// LineTo(hdcDest, rc.left + 7, rc.top + 7);
	// MoveToEx(hdcDest, rc.left + 4, rc.top + 5, NULL);
	// LineTo(hdcDest, rc.left + 7, rc.top + 8);

	// MoveToEx(hdcDest, rc.left, rc.top + 10, NULL);
	// LineTo(hdcDest, rc.left + 4, rc.top + 6);
	MoveToEx(hdcDest, rc.left, rc.top + 11, NULL);
	LineTo(hdcDest, rc.left + 4, rc.top + 7);
	// MoveToEx(hdcDest, rc.left + 1, rc.top + 11, NULL);
	// LineTo(hdcDest, rc.left + 5, rc.top + 7);

	MoveToEx(hdcDest, rc.left + 2, rc.top + 6, NULL);
	LineTo(hdcDest, rc.left + 6, rc.top + 10);

	// MoveToEx(hdcDest, rc.left + 7, rc.top, NULL);
	// LineTo(hdcDest, rc.left + 11, rc.top);
	// MoveToEx(hdcDest, rc.left + 6, rc.top + 1, NULL);
	// LineTo(hdcDest, rc.left + 12, rc.top + 1);
	// MoveToEx(hdcDest, rc.left + 6, rc.top + 2, NULL);
	// LineTo(hdcDest, rc.left + 12, rc.top + 2);
	// MoveToEx(hdcDest, rc.left + 3, rc.top + 3, NULL);
	// LineTo(hdcDest, rc.left + 12, rc.top + 3);
	// MoveToEx(hdcDest, rc.left + 2, rc.top + 4, NULL);
	// LineTo(hdcDest, rc.left + 12, rc.top + 4);
	// MoveToEx(hdcDest, rc.left + 2, rc.top + 5, NULL);
	// LineTo(hdcDest, rc.left + 11, rc.top + 5);
	// MoveToEx(hdcDest, rc.left + 2, rc.top + 6, NULL);
	// LineTo(hdcDest, rc.left + 9, rc.top + 6);
	// MoveToEx(hdcDest, rc.left + 3, rc.top + 7, NULL);
	// LineTo(hdcDest, rc.left + 9, rc.top + 7);
	// MoveToEx(hdcDest, rc.left + 2, rc.top + 8, NULL);
	// LineTo(hdcDest, rc.left + 9, rc.top + 8);
	// MoveToEx(hdcDest, rc.left + 1, rc.top + 9, NULL);
	// LineTo(hdcDest, rc.left + 4, rc.top + 9);
	// MoveToEx(hdcDest, rc.left + 5, rc.top + 9, NULL);
	// LineTo(hdcDest, rc.left + 8, rc.top + 9);
	// MoveToEx(hdcDest, rc.left, rc.top + 10, NULL);
	// LineTo(hdcDest, rc.left + 3, rc.top + 10);
	// MoveToEx(hdcDest, rc.left + 0, rc.top + 11, NULL);
	// LineTo(hdcDest, rc.left + 2, rc.top + 11);


	hbmp = SelectBitmap(hdcDest, hbmpOld);
	SelectPen(hdcDest, hOldPen);
	SelectBrush(hdcDest, hBrushOld);
	DeletePen(hPen);
	DeleteBrush(hBrush);
	DeleteBrush(hTextBrush);
	if(*phbmp)
		DeleteBitmap(*phbmp);
	*phbmp = hbmp;
	DeleteDC(hdcDest);
	ReleaseDC(hwnd, hdcSrc);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateSkinlessToolbarBitmap
 Created  : Tue Jan  8 12:11:19 2008
 Modified : Tue Jan  8 12:11:19 2008

 Synopsys : Creates bitmap for skinless toolbar
 Input    : hwnd - note window handle
            pA - note appearance
            pH - note run-time handles
            phbmp - pointer to HBITMAP will be created
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void CreateSkinlessToolbarBitmap(HWND hwnd, P_NOTE_APPEARANCE pA, P_NOTE_RTHANDLES pH, HBITMAP * phbmp){

	RECT		rc;
	HDC			hdcMem, hdcSrc;
	HBITMAP		hBmpOld, hBmpMem;
	int			offset = 0;

	hdcSrc = GetDC(hwnd);
	hdcMem = CreateCompatibleDC(hdcSrc);
	SetRect(&rc, 0, 0, SMALL_RECT * CMD_TBR_BUTTONS, SMALL_RECT);
	hBmpMem = CreateCompatibleBitmap(hdcSrc, SMALL_RECT * CMD_TBR_BUTTONS, SMALL_RECT);
	SetBkMode(hdcMem, TRANSPARENT);
	hBmpOld = SelectBitmap(hdcMem, hBmpMem);
	FillSkinlessNoteCaption(hdcMem, &rc, pA->crWindow, NULL);
	for(int i = CMD_FONT; i < CMD_TBR_BUTTONS; i++){
		switch(i){
		case CMD_FONT:
			CDrawFont(hdcMem, pA, offset);
			break;
		case CMD_SIZE:
			CDrawFontSize(hdcMem, pA, offset);
			break;
		case CMD_COLOR:
			CDrawFontColor(hdcMem, pA, offset);
			break;
		case CMD_BOLD:
			CDrawBold(hdcMem, pA, offset);
			break;
		case CMD_ITALIC:
			CDrawItalic(hdcMem, pA, offset);
			break;
		case CMD_UNDERLINE:
			CDrawUnderline(hdcMem, pA, offset);
			break;
		case CMD_STRIKETHROUGH:
			CDrawStrikethrough(hdcMem, pA, offset);
			break;
		case CMD_BULLETS:
			CDrawBullets(hdcMem, pA, offset);
			break;
		case CMD_HIGHLIGHT:
			CDrawHighlight(hdcMem, pA, offset);
			break;
		case CMD_A_LEFT:
			CDrawLeftAlign(hdcMem, pA, offset);
			break;
		case CMD_A_CENTER:
			CDrawCenterAlign(hdcMem, pA, offset);
			break;
		case CMD_A_RIGHT:
			CDrawRightAlign(hdcMem, pA, offset);
			break;
		
		}
		offset += SMALL_RECT;
	}
	hBmpMem = SelectBitmap(hdcMem, hBmpOld);
	if(*phbmp)
		DeleteBitmap(*phbmp);
	*phbmp = hBmpMem;
	DeleteDC(hdcMem);
	ReleaseDC(hwnd, hdcSrc);
}

static void SysDrawDelete(HDC hdc, P_NOTE_APPEARANCE pA, int offset){
	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);
	
	// MoveToEx(hdc, offset + 1, 2, NULL);
	// LineTo(hdc, offset + 11, 12);
	// MoveToEx(hdc, offset + 1, 3, NULL);
	// LineTo(hdc, offset + 10, 12);
	// MoveToEx(hdc, offset + 2, 2, NULL);
	// LineTo(hdc, offset + 11, 11);
	// MoveToEx(hdc, offset + 1, 11, NULL);
	// LineTo(hdc, offset + 11, 1);
	// MoveToEx(hdc, offset + 1, 10, NULL);
	// LineTo(hdc, offset + 10, 1);
	// MoveToEx(hdc, offset + 2, 11, NULL);
	// LineTo(hdc, offset + 11, 2);

	MoveToEx(hdc, offset + 2, 3, NULL);
	LineTo(hdc, offset + 10, 11);
	MoveToEx(hdc, offset + 2, 4, NULL);
	LineTo(hdc, offset + 9, 11);
	MoveToEx(hdc, offset + 3, 3, NULL);
	LineTo(hdc, offset + 10, 10);
	MoveToEx(hdc, offset + 2, 10, NULL);
	LineTo(hdc, offset + 10, 2);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 9, 2);
	MoveToEx(hdc, offset + 3, 10, NULL);
	LineTo(hdc, offset + 10, 3);

	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void SysDrawHide(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	if(IsBitOn(g_NoteSettings.reserved1, SB1_HIDE_DELETE) && IsBitOn(g_NoteSettings.reserved1, SB1_CROSS_INST_TRNGL)){
		SysDrawDelete(hdc, pA, offset);
		return;
	}

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);

	MoveToEx(hdc, offset + 0, 6, NULL);
	LineTo(hdc, offset + 12, 6);
	MoveToEx(hdc, offset + 1, 7, NULL);
	LineTo(hdc, offset + 11, 7);
	MoveToEx(hdc, offset + 2, 8, NULL);
	LineTo(hdc, offset + 10, 8);
	MoveToEx(hdc, offset + 3, 9, NULL);
	LineTo(hdc, offset + 9, 9);
	MoveToEx(hdc, offset + 4, 10, NULL);
	LineTo(hdc, offset + 8, 10);
	MoveToEx(hdc, offset + 5, 11, NULL);
	LineTo(hdc, offset + 7, 11);

	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void CDrawFont(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 8, 3);
	LineTo(hdc, offset + 8, 9);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 5, 9);
	MoveToEx(hdc, offset + 7, 9, NULL);
	LineTo(hdc, offset + 10, 9);
	MoveToEx(hdc, offset + 4, 7, NULL);
	LineTo(hdc, offset + 8, 7);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void CDrawFontSize(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 1, 9, NULL);
	LineTo(hdc, offset + 6, 4);
	LineTo(hdc, offset + 6, 9);
	MoveToEx(hdc, offset + 1, 9, NULL);
	LineTo(hdc, offset + 4, 9);
	MoveToEx(hdc, offset + 5, 9, NULL);
	LineTo(hdc, offset + 8, 9);
	MoveToEx(hdc, offset + 3, 7, NULL);
	LineTo(hdc, offset + 6, 7);
	MoveToEx(hdc, offset + 9, 1, NULL);
	LineTo(hdc, offset + 9, 9);
	MoveToEx(hdc, offset + 8, 2, NULL);
	LineTo(hdc, offset + 11, 2);
	MoveToEx(hdc, offset + 8, 7, NULL);
	LineTo(hdc, offset + 11, 7);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void CDrawFontColor(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 2, 3, NULL);
	LineTo(hdc, offset + 10, 3);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 128, 64));
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 2, 4, NULL);
	LineTo(hdc, offset + 10, 4);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 2, 5, NULL);
	LineTo(hdc, offset + 10, 5);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 2, 6, NULL);
	LineTo(hdc, offset + 10, 6);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 2, 7, NULL);
	LineTo(hdc, offset + 10, 7);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 2, 8, NULL);
	LineTo(hdc, offset + 10, 8);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(128, 0, 255));
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 10, 9);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void CDrawBold(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 3, 3, NULL);
	LineTo(hdc, offset + 8, 3);
	MoveToEx(hdc, offset + 3, 4, NULL);
	LineTo(hdc, offset + 9, 4);
	MoveToEx(hdc, offset + 3, 5, NULL);
	LineTo(hdc, offset + 5, 5);
	MoveToEx(hdc, offset + 7, 5, NULL);
	LineTo(hdc, offset + 9, 5);
	MoveToEx(hdc, offset + 3, 6, NULL);
	LineTo(hdc, offset + 8, 6);
	MoveToEx(hdc, offset + 3, 7, NULL);
	LineTo(hdc, offset + 5, 7);
	MoveToEx(hdc, offset + 7, 7, NULL);
	LineTo(hdc, offset + 9, 7);
	MoveToEx(hdc, offset + 3, 8, NULL);
	LineTo(hdc, offset + 9, 8);
	MoveToEx(hdc, offset + 3, 9, NULL);
	LineTo(hdc, offset + 8, 9);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void CDrawItalic(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 6, 2, NULL);
	LineTo(hdc, offset + 8, 2);
	MoveToEx(hdc, offset + 4, 4, NULL);
	LineTo(hdc, offset + 7, 4);
	MoveToEx(hdc, offset + 5, 5, NULL);
	LineTo(hdc, offset + 7, 5);
	MoveToEx(hdc, offset + 5, 6, NULL);
	LineTo(hdc, offset + 7, 6);
	MoveToEx(hdc, offset + 5, 7, NULL);
	LineTo(hdc, offset + 7, 7);
	MoveToEx(hdc, offset + 4, 8, NULL);
	LineTo(hdc, offset + 6, 8);
	MoveToEx(hdc, offset + 4, 9, NULL);
	LineTo(hdc, offset + 7, 9);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void CDrawUnderline(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 3, 2, NULL);
	LineTo(hdc, offset + 3, 7);
	MoveToEx(hdc, offset + 4, 2, NULL);
	LineTo(hdc, offset + 4, 7);
	MoveToEx(hdc, offset + 7, 2, NULL);
	LineTo(hdc, offset + 7, 7);
	MoveToEx(hdc, offset + 8, 2, NULL);
	LineTo(hdc, offset + 8, 7);
	MoveToEx(hdc, offset + 4, 7, NULL);
	LineTo(hdc, offset + 8, 7);
	MoveToEx(hdc, offset + 3, 9, NULL);
	LineTo(hdc, offset + 9, 9);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void CDrawStrikethrough(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 4, 3, NULL);
	LineTo(hdc, offset + 7, 3);
	MoveToEx(hdc, offset + 3, 4, NULL);
	LineTo(hdc, offset + 5, 4);
	MoveToEx(hdc, offset + 7, 4, NULL);
	LineTo(hdc, offset + 8, 4);
	MoveToEx(hdc, offset + 3, 5, NULL);
	LineTo(hdc, offset + 6, 5);
	MoveToEx(hdc, offset + 1, 6, NULL);
	LineTo(hdc, offset + 11, 6);
	MoveToEx(hdc, offset + 5, 7, NULL);
	LineTo(hdc, offset + 8, 7);
	MoveToEx(hdc, offset + 3, 8, NULL);
	LineTo(hdc, offset + 4, 8);
	MoveToEx(hdc, offset + 6, 8, NULL);
	LineTo(hdc, offset + 8, 8);
	MoveToEx(hdc, offset + 4, 9, NULL);
	LineTo(hdc, offset + 7, 9);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void CDrawLeftAlign(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 10, 9);
	MoveToEx(hdc, offset + 2, 7, NULL);
	LineTo(hdc, offset + 6, 7);
	MoveToEx(hdc, offset + 2, 5, NULL);
	LineTo(hdc, offset + 10, 5);
	MoveToEx(hdc, offset + 2, 3, NULL);
	LineTo(hdc, offset + 6, 3);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void CDrawRightAlign(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 10, 9);
	MoveToEx(hdc, offset + 6, 7, NULL);
	LineTo(hdc, offset + 10, 7);
	MoveToEx(hdc, offset + 2, 5, NULL);
	LineTo(hdc, offset + 10, 5);
	MoveToEx(hdc, offset + 6, 3, NULL);
	LineTo(hdc, offset + 10, 3);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void CDrawCenterAlign(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 10, 9);
	MoveToEx(hdc, offset + 4, 7, NULL);
	LineTo(hdc, offset + 8, 7);
	MoveToEx(hdc, offset + 2, 5, NULL);
	LineTo(hdc, offset + 10, 5);
	MoveToEx(hdc, offset + 4, 3, NULL);
	LineTo(hdc, offset + 8, 3);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void CDrawHighlight(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 1, 8, NULL);
	LineTo(hdc, offset + 9, 0);
	MoveToEx(hdc, offset + 1, 9, NULL);
	LineTo(hdc, offset + 10, 0);
	MoveToEx(hdc, offset + 1, 10, NULL);
	LineTo(hdc, offset + 11, 0);
	MoveToEx(hdc, offset + 2, 10, NULL);
	LineTo(hdc, offset + 11, 1);
	MoveToEx(hdc, offset + 3, 10, NULL);
	LineTo(hdc, offset + 11, 2);
	MoveToEx(hdc, offset + 4, 10, NULL);
	LineTo(hdc, offset + 11, 3);
	MoveToEx(hdc, offset + 5, 10, NULL);
	LineTo(hdc, offset + 11, 4);
	MoveToEx(hdc, offset + 6, 10, NULL);
	LineTo(hdc, offset + 11, 5);

	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 1, 9, NULL);
	LineTo(hdc, offset + 6, 4);
	LineTo(hdc, offset + 6, 9);
	MoveToEx(hdc, offset + 1, 9, NULL);
	LineTo(hdc, offset + 4, 9);
	MoveToEx(hdc, offset + 5, 9, NULL);
	LineTo(hdc, offset + 8, 9);
	MoveToEx(hdc, offset + 3, 7, NULL);
	LineTo(hdc, offset + 6, 7);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
}

static void CDrawBullets(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;
	RECT		rc;
	HBRUSH		br;

	br = CreateSolidBrush(pA->crCaption);
	SetRect(&rc, offset + 1, 2, offset + 3, 4);
	FillRect(hdc, &rc, br);
	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectPen(hdc, hPen);
	MoveToEx(hdc, offset + 5, 3, NULL);
	LineTo(hdc, offset + 11, 3);
	SetRect(&rc, offset + 1, 7, offset + 3, 9);
	FillRect(hdc, &rc, br);
	MoveToEx(hdc, offset + 5, 8, NULL);
	LineTo(hdc, offset + 11, 8);
	SelectPen(hdc, hOldPen);
	DeletePen(hPen);
	DeleteBrush(br);
}


