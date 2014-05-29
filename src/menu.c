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

#ifndef WINVER
#define WINVER  0x0501
#endif

#include <windows.h>
#include <wchar.h>
#include <shlwapi.h>
#include <windowsx.h>
#include "transparentbitmap.h"
#include "menu.h"
#include "gradients.h"

/** Prototypes ********************************************************/
static void DrawSeparator(const DRAWITEMSTRUCT * lpDI);
static int IsTab(const wchar_t * lpText);
static void GetLeftPart(wchar_t * src, wchar_t * dst, int pos);
static void GetRightPart(wchar_t * src, wchar_t * dst, int pos);
static void DrawColorItem(const DRAWITEMSTRUCT * lpDI);

#define	COLOR_MENUHILIGHT			29

// static COLORREF		m_ClrText, m_ClrTextGray, m_ClrTextSelected, m_ClrBack, m_ClrSelected, m_ClrBar;
static COLORREF			m_Clr1, m_Clr2;

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateMenuColors
 Created  : Sun Apr  6 15:02:17 2008
 Modified : Sun Apr  6 15:02:17 2008

 Synopsys : Prepares colors for menu drawing
 Input    : winVer - specifies windows version. "6" means XP and above
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

// void CreateMenuColors(int winVer){
	// m_ClrText = GetSysColor(COLOR_MENUTEXT);
	// m_ClrTextGray = GetSysColor(COLOR_GRAYTEXT);
	// m_ClrBack = GetSysColor(COLOR_MENU);
	// m_ClrSelected = GetSysColor(COLOR_HIGHLIGHT);
	// m_ClrTextSelected = GetSysColor(COLOR_HIGHLIGHTTEXT);
	// if(winVer == 6){
		// m_ClrSelected = GetSysColor(COLOR_MENUHILIGHT);
	// }
	// m_ClrBar = ColorAdjustLuma(m_ClrSelected, 128, FALSE);
// }

void PrepareMenuGradientColors(COLORREF color){
	m_Clr1 = m_Clr2 = color;
	m_Clr1 = ColorAdjustLuma(m_Clr1, 195, FALSE);
	m_Clr2 = ColorAdjustLuma(m_Clr2, -195, FALSE);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetMenuOwnerDraw
 Created  : Wed May 16 12:35:54 2007
 Modified : Wed May 16 12:35:54 2007

 Synopsys : Converts all menu items of specified menu to owner-rawn
 Input    : Menu handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void SetMenuOwnerDraw(HMENU hMenu){

	int 			count = 0;
	MENUITEMINFOW	mi;

	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_FTYPE | MIIM_SUBMENU;

	count = GetMenuItemCount(hMenu);
	for(int i = 0; i < count; i++){
		GetMenuItemInfoW(hMenu, i, TRUE, &mi);
		if((mi.fType & MFT_OWNERDRAW) != MFT_OWNERDRAW){
			mi.fType |= MFT_OWNERDRAW;
			SetMenuItemInfoW(hMenu, i, TRUE, &mi);
			if(mi.hSubMenu){
				SetMenuOwnerDraw(mi.hSubMenu);
			}
		}
	}
}

void EnablemenuItems(HMENU hMenu, int flag){
	int 			count = 0;
	MENUITEMINFOW	mi;

	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_SUBMENU;

	count = GetMenuItemCount(hMenu);
	for(int i = 0; i < count; i++){
		GetMenuItemInfoW(hMenu, i, TRUE, &mi);
		EnableMenuItem(hMenu, i, MF_BYPOSITION | flag);
		if(mi.hSubMenu){
			EnablemenuItems(mi.hSubMenu, flag);
		}
	}
}

void SetMenuItemProperties(PMITEM pmi, HMENU hMenu, int id, BOOL position){
	MENUITEMINFOW	mi;

	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_SUBMENU;
	if(!position)
		mi.fMask |= MIIM_ID;
	GetMenuItemInfoW(hMenu, id, position, &mi);
	if((mi.fType & MFT_OWNERDRAW) != MFT_OWNERDRAW){
		mi.fType |= MFT_OWNERDRAW;
	}
	if(mi.dwItemData){
		free((PMITEM)mi.dwItemData);
	}
	mi.dwItemData = (int)calloc(1, sizeof(MITEM));
	memcpy((PMITEM)mi.dwItemData, pmi, sizeof(MITEM));
	SetMenuItemInfoW(hMenu, id, position, &mi);
}

void FreeSingleMenu(HMENU hMenu, int pos){
	MENUITEMINFOW	mi;

	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_DATA | MIIM_FTYPE;
	GetMenuItemInfoW(hMenu, pos, TRUE, &mi);
	if((mi.fType & MFT_OWNERDRAW) == MFT_OWNERDRAW){
		if(mi.dwItemData){
			free((PMITEM)mi.dwItemData);
		}
	}
}

void FreeMenus(HMENU hMenu){
	int count = 0;
	MENUITEMINFOW	mi;

	count = GetMenuItemCount(hMenu);
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_SUBMENU;
	for(int i = 0; i < count; i++){
		GetMenuItemInfoW(hMenu, i, TRUE, &mi);
		if((mi.fType & MFT_OWNERDRAW) == MFT_OWNERDRAW){
			if(mi.dwItemData){
				free((PMITEM)mi.dwItemData);
			}
		}
		if(mi.hSubMenu){
			FreeMenus(mi.hSubMenu);
		}
	}
}

void MeasureMItem(HFONT hFont, MEASUREITEMSTRUCT * lpMI){

	HDC			hdcD, hdcI;
	SIZE		sz;
	HFONT		hOldFont;
	int			cy, tabPos;
	PMITEM		pmi;

	pmi = (PMITEM)lpMI->itemData;
	
	// if(pmi){
		if(lpMI->itemID == 0)
			lpMI->itemHeight = 7;
		else{
			cy = GetSystemMetrics(SM_CYMENU);
			hdcD = GetDC(NULL);
			hdcI = CreateCompatibleDC(hdcD);
			hOldFont = SelectFont(hdcI, hFont);
			if(pmi){
				if(pmi->type == MT_REGULARITEM){
					GetTextExtentPoint32W(hdcI, pmi->szText, wcslen(pmi->szText), &sz);
					if(!pmi->firstLevel){
						tabPos = IsTab(pmi->szText);
						if(tabPos != -1)
							lpMI->itemWidth = sz.cx + 68;
						else
							lpMI->itemWidth = sz.cx + 36;
					}
					else{
						lpMI->itemWidth = sz.cx + 4;
					}
					lpMI->itemHeight = cy;
				}
				else{
					lpMI->itemHeight = cy;
				}
			}
			SelectFont(hdcI, hOldFont);
			DeleteDC(hdcI);
			ReleaseDC(NULL, hdcD);
		}
	// }
}

void DrawMItem(const DRAWITEMSTRUCT * lpDI, HBITMAP hBmp, HBITMAP hBmpGray, COLORREF crMask){

	int			state;
	wchar_t		szLeft[128], szRight[128];
	RECT		rc, rcBar, rcRight, rcLF;
	BOOL		selected = FALSE, grayed = FALSE, checked = FALSE, hotlight = FALSE;
	int			xPos, yOffset, tabPos;
	PMITEM		pmi;

	state = SaveDC(lpDI->hDC);
	pmi = (PMITEM)lpDI->itemData;

	if(lpDI->itemID == 0)
		DrawSeparator(lpDI);
	else if(pmi && pmi->type == MT_COLORITEM)
		DrawColorItem(lpDI);
	else{
		SetBkMode(lpDI->hDC, TRANSPARENT);
		if((lpDI->itemState & ODS_SELECTED) == ODS_SELECTED)
			selected = TRUE;
		if((lpDI->itemState & ODS_GRAYED) == ODS_GRAYED)
			grayed = TRUE;
		if((lpDI->itemState & ODS_CHECKED) == ODS_CHECKED)
			checked = TRUE;
		if((lpDI->itemState & ODS_HOTLIGHT) == ODS_HOTLIGHT)
			hotlight = TRUE;

		if(grayed){
			SetTextColor(lpDI->hDC, GetSysColor(COLOR_GRAYTEXT));
		}
		else{
			if(selected || hotlight){
				SetTextColor(lpDI->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
				}
			else{
				SetTextColor(lpDI->hDC, GetSysColor(COLOR_MENUTEXT));
			}
		}
		if(selected || hotlight){
			// HBRUSH 		br = CreateSolidBrush(m_ClrSelected);
			if(!grayed){
				FillRect(lpDI->hDC, &lpDI->rcItem, GetSysColorBrush(COLOR_HIGHLIGHT));
			}
			FrameRect(lpDI->hDC, &lpDI->rcItem, GetSysColorBrush(COLOR_BTNSHADOW));
			// DeleteBrush(br);
		}
		else{
			if(pmi && !pmi->firstLevel){
				// HBRUSH 		br = CreateSolidBrush(m_ClrBack);
				CopyRect(&rcBar, &lpDI->rcItem);
				rcBar.right = 26;
				Fill2ColorsRectangle(lpDI->hDC, &rcBar, m_Clr1, m_Clr2, GRADIENT_FILL_RECT_H);
				CopyRect(&rc, &lpDI->rcItem);
				OffsetRect(&rc, 26, 0);
				FillRect(lpDI->hDC, &rc, GetSysColorBrush(COLOR_MENU));
				
				// SetRect(&rc, 0, lpDI->rcItem.top, 26, lpDI->rcItem.bottom);
				// Fill2ColorsRectangle(lpDI->hDC, &rc, RGB(255, 255, 255), m_ClrBar, GRADIENT_FILL_RECT_H);
				// DeleteBrush(br);
			}
			else{
				FillRect(lpDI->hDC, &lpDI->rcItem, GetSysColorBrush(COLOR_MENUBAR));
			}
		}

		CopyRect(&rc, &lpDI->rcItem);
		OffsetRect(&rc, 30, 0);
		if(pmi){
			tabPos = IsTab(pmi->szText);
			if(tabPos == -1)
				if(!pmi->firstLevel){
					DrawTextW(lpDI->hDC, pmi->szText, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				}
				else{
					CopyRect(&rcLF, &lpDI->rcItem);
					DrawTextW(lpDI->hDC, pmi->szText, -1, &rcLF, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				}
			else{
				GetLeftPart(pmi->szText, szLeft, tabPos);
				GetRightPart(pmi->szText, szRight, tabPos);
				CopyRect(&rcRight, &lpDI->rcItem);
				OffsetRect(&rcRight, -4, 0);
				DrawTextW(lpDI->hDC, szLeft, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				DrawTextW(lpDI->hDC, szRight, -1, &rcRight, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
			}
			yOffset = (lpDI->rcItem.bottom - lpDI->rcItem.top - 16) / 2;
			if(!checked)
				xPos = pmi->xPos;
			else
				xPos = pmi->xCheck;
			if(xPos >= 0){
				if(grayed)
					DrawTransparentBitmap(hBmpGray, lpDI->hDC, 5, lpDI->rcItem.top + yOffset, 16, 16, xPos * 16, 0, crMask);
				else
					DrawTransparentBitmap(hBmp, lpDI->hDC, 5, lpDI->rcItem.top + yOffset, 16, 16, xPos * 16, 0, crMask);
			}
		}
	}
	RestoreDC(lpDI->hDC, state);

} 

static void DrawColorItem(const DRAWITEMSTRUCT * lpDI){
	HBRUSH		hBrush;
	RECT		rc, rcBar;
	COLORREF	color;
	PMITEM		pmi;

	CopyRect(&rcBar, &lpDI->rcItem);
	rcBar.right = 26;
	Fill2ColorsRectangle(lpDI->hDC, &rcBar, m_Clr1, m_Clr2, GRADIENT_FILL_RECT_H);
	CopyRect(&rc, &lpDI->rcItem);
	rc.top += 1;
	rc.bottom -= 1;
	rc.right -= 1;
	rc.left += 26 + 1;

	pmi = (PMITEM)lpDI->itemData;
	if(pmi){
		color = _wtoi(pmi->szText);
		hBrush = CreateSolidBrush(color);
		FillRect(lpDI->hDC, &rc, hBrush);
		FrameRect(lpDI->hDC, &rc, GetStockBrush(BLACK_BRUSH));
		// SetRect(&rc, 0, lpDI->rcItem.top, 26, lpDI->rcItem.bottom);
		// Fill2ColorsRectangle(lpDI->hDC, &rc, RGB(255, 255, 255), m_ClrBar, GRADIENT_FILL_RECT_H);
		DeleteBrush(hBrush);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DrawSeparator
 Created  : Wed May 16 12:58:25 2007
 Modified : Wed May 16 12:58:25 2007

 Synopsys : Draws menu separator
 Input    : DRAWITEMSTRUCT structure
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void DrawSeparator(const DRAWITEMSTRUCT * lpDI){

	HPEN		hPen, hOldPen;
	POINT		pt;
	int			middle;
	RECT		rc, rcBar;

	CopyRect(&rcBar, &lpDI->rcItem);
	rcBar.right = 26;
	Fill2ColorsRectangle(lpDI->hDC, &rcBar, m_Clr1, m_Clr2, GRADIENT_FILL_RECT_H);
	CopyRect(&rc, &lpDI->rcItem);
	OffsetRect(&rc, 30, 0);

	middle = 3 + rc.top;
	MoveToEx(lpDI->hDC, rc.left, middle, &pt);
	hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	hOldPen = SelectPen(lpDI->hDC, hPen);
	LineTo(lpDI->hDC, rc.right, middle);
	DeletePen(SelectPen(lpDI->hDC, hOldPen));
	middle++;
	MoveToEx(lpDI->hDC, rc.left, middle, &pt);
	hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHILIGHT));
	hOldPen = SelectPen(lpDI->hDC, hPen);
	LineTo(lpDI->hDC, rc.right, middle);
	DeletePen(SelectPen(lpDI->hDC, hOldPen));
	// SetRect(&rc, 0, lpDI->rcItem.top, 26, lpDI->rcItem.bottom);
	// Fill2ColorsRectangle(lpDI->hDC, &rc, RGB(255, 255, 255), m_ClrBar, GRADIENT_FILL_RECT_H);
}

static int IsTab(const wchar_t * lpText){
	int		res = -1;
	while(*lpText){
		res++;
		if(*lpText++ == '\t')
			return res;
	}
	return -1;
}


static void GetLeftPart(wchar_t * src, wchar_t * dst, int pos){
	for(; (*dst++ = *src++) && pos-- > 0;)
		;
	*--dst = '\0';
}


static void GetRightPart(wchar_t * src, wchar_t * dst, int pos){

	src += pos + 1;
	while((*dst++ = *src++))
		;
}
