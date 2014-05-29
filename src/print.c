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

#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <richedit.h>

#include "notes.h"
#include "stringconstants.h"
#include "print.h"
#include "shared.h"

#define	BASE_DPI		1440

static BOOL SetDefaultPSD(void);
static void GetPageInfo(HWND hEdit, BOOL fSelection);

static POINT		m_Paper;
static RECT			m_Margin, m_MinMargin;
static int			m_CountPage;
static long			* m_pChar;

void PrintEdit(HWND hEdit, wchar_t * lpDocName){
	FORMATRANGE		fr;
	CHARRANGE		chrg;
	HDC				hPrintDC;
	int				tempFlags;
	int				pageMin = 0, pageMax = 0;
	HFONT			hFontPageNumber, hFontOld;
	LOGFONTW		lf;
	int				xNP, yNP, yDoc;
	DOCINFOW		docinfo;
	wchar_t			szPage[32];
	int				oldAlign;
	COLORREF		oldColor;

	ZeroMemory(&g_pPdlg, sizeof(g_pPdlg));
	g_pPdlg.lStructSize = sizeof(g_pPdlg);
	g_pPdlg.hwndOwner = g_hMain;
	g_pPdlg.hInstance = g_hInstance;
	SendMessageW(hEdit, EM_EXGETSEL ,0, (LPARAM)&chrg);
	if(chrg.cpMax == 0 || chrg.cpMax == chrg.cpMin){
		g_pPdlg.Flags = PD_ALLPAGES | PD_NOSELECTION;
	}
	else{
		if(chrg.cpMax > chrg.cpMin){
			g_pPdlg.Flags = PD_SELECTION;
		}
		else{
			g_pPdlg.Flags = PD_ALLPAGES;
		}
	}
	g_pPdlg.Flags |= PD_RETURNDC | PD_HIDEPRINTTOFILE | 0x20000;	//PD_NONETWORKBUTTON
	GetPageInfo(hEdit, FALSE);

	g_pPdlg.nFromPage = 1;
	g_pPdlg.nToPage = m_CountPage;
	g_pPdlg.nMinPage = 1;
	g_pPdlg.nMaxPage = m_CountPage;
	g_pPdlg.nCopies = 1;

	if(PrintDlgW(&g_pPdlg)){
		hPrintDC = g_pPdlg.hDC;
		fr.hdc = hPrintDC;
		fr.hdcTarget = NULL;

		tempFlags = g_pPdlg.Flags;
		tempFlags &= PD_ALLPAGES | PD_SELECTION | PD_PAGENUMS;
		if(tempFlags == PD_ALLPAGES){
			pageMin = 0;
			pageMax = m_CountPage;
		}
		else if(tempFlags == PD_SELECTION){
			GetPageInfo(hEdit, TRUE);
			pageMin = 0;
			pageMax = m_CountPage;
		}
		else if(tempFlags == PD_PAGENUMS){
			pageMin = g_pPdlg.nFromPage;
			pageMin--;
			pageMax = g_pPdlg.nToPage;
		}

		fr.rc.left = m_Margin.left;
		fr.rc.top = m_Margin.top;
		fr.rc.right = m_Paper.x - m_Margin.right;
		fr.rc.bottom = m_Paper.y - m_Margin.bottom;

		ZeroMemory(&lf, sizeof(lf));
		lf.lfHeight = -MulDiv(10, GetDeviceCaps(hPrintDC, LOGPIXELSY), 72);
		wcscpy(lf.lfFaceName, FONT_NP);
		hFontPageNumber = CreateFontIndirectW(&lf);

		yNP = GetDeviceCaps(hPrintDC, VERTRES);
		xNP = GetDeviceCaps(hPrintDC, HORZRES) / 2;
		yDoc = 16;

		fr.rcPage.left = 0;
        fr.rcPage.top = 0;
        fr.rcPage.right = m_Paper.x;
        fr.rcPage.bottom = m_Paper.y  ;

		ZeroMemory(&docinfo, sizeof(docinfo));
		docinfo.cbSize = sizeof(docinfo);
		docinfo.lpszDocName = lpDocName;

		StartDocW(hPrintDC, &docinfo);
		if(pageMax){
			do{
				pageMin++;
				StartPage(hPrintDC);
				hFontOld = SelectFont(hPrintDC, hFontPageNumber);
				wsprintfW(szPage, L"Page %i", pageMin);
				oldAlign = SetTextAlign(hPrintDC, TA_CENTER | TA_BOTTOM);
	            oldColor = SetTextColor(hPrintDC, 0);
	            TextOutW(hPrintDC, xNP, yNP, szPage, wcslen(szPage));
				SetTextAlign(hPrintDC, TA_CENTER | TA_TOP);
				TextOutW(hPrintDC, xNP, yDoc, lpDocName, wcslen(lpDocName));
	            SetTextColor(hPrintDC, oldColor);
	            SelectFont(hPrintDC, hFontOld);
	            SetTextAlign(hPrintDC, oldAlign);
				if(m_pChar){
					int			min	= pageMin;
					min--;
					fr.chrg.cpMin = *(m_pChar + min);
					min++;
					fr.chrg.cpMax = *(m_pChar + min);
				}
				else{
					fr.chrg.cpMin = 0;
					fr.chrg.cpMax = 0;
				}
				SendMessageW(hEdit, EM_FORMATRANGE, TRUE, (LPARAM)&fr);
				EndPage(hPrintDC);
			}while(pageMin < pageMax);
		}
		DeleteFont(hFontPageNumber);
		SendMessageW(hEdit, EM_FORMATRANGE, FALSE, (LPARAM)NULL);
        EndDoc(hPrintDC);
        DeleteDC(hPrintDC);
	}
}

static void GetPageInfo(HWND hEdit, BOOL fSelection){
	long				lastChar, lastOldChar, tempChar;
	FORMATRANGE		fr;
	long			maxLen;
	HDC				hdc;
	CHARRANGE		chrg;
	long			* pCharOld, * pTemp;

	hdc = GetDC(NULL);

	m_CountPage = 0;
	if(m_pChar){
		HeapFree(g_hHeap, 0,  m_pChar);
		m_pChar = 0;
	}
	SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&chrg);
	if(!chrg.cpMax || chrg.cpMax == chrg.cpMin){
		lastChar = 0;
		maxLen = -1;
	}
	else{
		if(fSelection){
			lastChar = chrg.cpMin;
			maxLen = chrg.cpMax;
		}
		else{
			lastChar = 0;
			maxLen = -1;
		}
	}
	fr.hdc = hdc;
	fr.hdcTarget = hdc;
	fr.rc.left = 0;
	fr.rc.top = 0;
	fr.rc.right = m_Paper.x - m_Margin.right - m_Margin.left;
	fr.rc.bottom = m_Paper.y - m_Margin.bottom - m_Margin.top;
	CopyRect(&fr.rcPage, &fr.rc);
	goto start_c;

new_page:
	m_CountPage++;
	pCharOld = m_pChar;
	m_pChar = (long *)HeapAlloc(g_hHeap, 0, m_CountPage * sizeof(int));
	if(pCharOld){
		m_CountPage--;
		memcpy(m_pChar, pCharOld, m_CountPage * sizeof(int));
		m_CountPage++;
		HeapFree(g_hHeap, 0, pCharOld);
	}
	pTemp = m_pChar;
	*(pTemp + (m_CountPage - 1)) = lastOldChar;

start_c:
	fr.chrg.cpMin = lastChar;
    fr.chrg.cpMax = maxLen;
	
	lastOldChar = lastChar;
	if(!fSelection){
    	lastChar = SendMessageW(hEdit, EM_FORMATRANGE, FALSE, (LPARAM)&fr);
		if(lastChar > lastOldChar)
			goto new_page;
	}
    else{
		tempChar = SendMessageW(hEdit, EM_FORMATRANGE, FALSE, (LPARAM)&fr);
		if(maxLen > tempChar){
			lastChar = tempChar;
		}
		else{
			lastChar = maxLen;
		}
		if(lastChar > lastOldChar)
			goto new_page;
	}
	
	m_CountPage++;
	pCharOld = m_pChar;
	m_pChar = (long *)HeapAlloc(g_hHeap, 0, m_CountPage * sizeof(int));

	if(pCharOld){
		m_CountPage--;
		memcpy(m_pChar, pCharOld, m_CountPage * sizeof(int));
		m_CountPage++;
		HeapFree(g_hHeap, 0, pCharOld);
	}
	pTemp = m_pChar;
	*(pTemp + (m_CountPage - 1)) = lastOldChar;

	m_CountPage--;
	SendMessageW(hEdit, EM_FORMATRANGE, FALSE, (LPARAM)NULL);
	if(m_CountPage == 0)
		m_CountPage = 1;

	ReleaseDC(NULL, hdc);
}

void SetPaper(void){
	POINT		paper;
	RECT		margin, minmargin;

	paper.x = g_pSetup.ptPaperSize.x;
	paper.y = g_pSetup.ptPaperSize.y;
	CopyRect(&margin ,&g_pSetup.rtMargin);
	CopyRect(&minmargin, &g_pSetup.rtMinMargin);
	m_Paper.x = (paper.x * BASE_DPI) / 1000;
	m_Paper.y = (paper.y * BASE_DPI) / 1000;
	m_Margin.left = (margin.left * BASE_DPI) / 1000;
	m_Margin.top = (margin.top * BASE_DPI) / 1000;
 	m_Margin.right = (margin.right * BASE_DPI) / 1000;
	m_Margin.bottom = (margin.bottom * BASE_DPI) / 1000;
    m_MinMargin.left = (minmargin.left * BASE_DPI) / 1000;
	m_MinMargin.top = (minmargin.top * BASE_DPI) / 1000;
 	m_MinMargin.right = (minmargin.right * BASE_DPI) / 1000;
	m_MinMargin.bottom = (minmargin.bottom * BASE_DPI) / 1000;
}

static BOOL SetDefaultPSD(void){

	// HDC			hdc;
	int			dpix, dpiy;

	ZeroMemory(&g_pPdlg, sizeof(g_pPdlg));
	g_pPdlg.lStructSize = sizeof(g_pPdlg);
	g_pPdlg.hInstance = g_hInstance;
	g_pPdlg.Flags = PD_RETURNDEFAULT | PD_RETURNDC;

	if(PrintDlgW(&g_pPdlg)){		
		// hdc = CreatePrinterDC(g_pPdlg.hDevMode, g_pPdlg.hDevNames);
		g_pSetup.rtMargin.left = 1000;
		g_pSetup.rtMargin.right = 1000;
		g_pSetup.rtMargin.top = 750;
		g_pSetup.rtMargin.bottom = 750;
		dpix = GetDeviceCaps(g_pPdlg.hDC, LOGPIXELSX);
		dpiy = GetDeviceCaps(g_pPdlg.hDC, LOGPIXELSY);
		g_pSetup.ptPaperSize.x = (GetDeviceCaps(g_pPdlg.hDC, PHYSICALWIDTH) * 1000) / dpix;
		g_pSetup.ptPaperSize.y = (GetDeviceCaps(g_pPdlg.hDC, PHYSICALHEIGHT) * 1000) / dpiy;
		g_pSetup.rtMinMargin.left = (GetDeviceCaps(g_pPdlg.hDC, PHYSICALOFFSETX) * 1000) / dpix;
		g_pSetup.rtMinMargin.top = (GetDeviceCaps(g_pPdlg.hDC, PHYSICALOFFSETY) * 1000) / dpiy;
		g_pSetup.rtMinMargin.right = g_pSetup.ptPaperSize.x - g_pSetup.rtMinMargin.left - (GetDeviceCaps(g_pPdlg.hDC, HORZRES) * 1000) / dpix;
		g_pSetup.rtMinMargin.bottom = g_pSetup.ptPaperSize.y - g_pSetup.rtMinMargin.top - (GetDeviceCaps(g_pPdlg.hDC, VERTRES) * 1000) / dpiy;

		GlobalFree(g_pPdlg.hDevMode);
		GlobalFree(g_pPdlg.hDevNames);
		// DeleteDC(hdc);
		return TRUE;
	}
	else{
		return FALSE;
	}
}

void InitPrintMembers(void){
	// if(!GetPrivateProfileStructW(S_PRINT, K_PRINT_SETUP, &g_pSetup, sizeof(g_pSetup), g_Paths.sINI)){
		SetDefaultPSD();
		// WritePrivateProfileStructW(S_PRINT, K_PRINT_SETUP, &g_pSetup, sizeof(g_pSetup), g_Paths.sINI);
	// }
	SetPaper();
}

