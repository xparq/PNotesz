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
#include <wchar.h>
#include <commctrl.h>
#include <colordlg.h>
#include <shlwapi.h>
#include "gradients.h"
#include "glistbox.h"
#include "gcolors.h"

typedef enum {O_HANDLES = 0};

#define	GCOLOR_CLASS_W			L"__RUBNVCF__GCOLOR__TWFCVHD__"
#define	GRAINBOW_CLASS			L"__SDFHGVS__RAINBOW__HDVHDGV__"
#define CUSTOM_CLASS			L"__ADJHUDH__CUSTOM__RWDXCDG__"
#define	T_CAPTION_1_W			L"Basic"
#define	T_CAPTION_2_W			L"Web"
#define	T_CAPTION_3_W			L"Custom"
#define	LST_CLR_PROC_PROP_W		L"_GColors_LstProc_Name"

#define	ID_H					301
#define	ID_S					302
#define	ID_L					303
#define	ID_R					304
#define	ID_G					305
#define	ID_B					306
#define	ID_SLIDER				307

#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))
#define	GetHandles(hwnd)			(PCHANDLES)GetWindowLongPtrW(hwnd, O_HANDLES)
#define	GetCustHandles(hwnd)			(PCUSTHANDLES)GetWindowLongPtrW(hwnd, O_HANDLES)

typedef struct _CHANDLES {
	HWND			hParent;
	HWND			hTab;
	HWND			hBasics;
	HWND			hWebs;
	HWND			hCustom;
	HFONT			hFont;
	COLORREF		savedColor;
	POINT		currBasic;
}CHANDLES, *PCHANDLES;

typedef struct _CUSTHANDLES {
	HWND			hRainbow;
	HWND			hHue;
	HWND			hSatur;
	HWND			hLuma;
	HWND			hRed;
	HWND			hGreen;
	HWND			hBlue;
	HWND			hSample;
	HWND			hOK;
	HWND			hSlider;
	HBRUSH			hBrush;
}CUSTHANDLES, *PCUSTHANDLES;

static void CreateBasicRects(LPRECT lpClient);
static void DrawBasicRects(HDC hdc);
static LRESULT CALLBACK BasicProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK Colors_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Colors_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
static void Colors_OnDestroy(HWND hwnd);
static void Colors_OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
static BOOL RegisterGColorsWindowW(void);
static BOOL RegisterGColorsWindow(void);
static int IsWebColor(PCHANDLES pch, COLORREF crColor);
static int IsBasicColor(COLORREF crColor);
static LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK LstProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void ListOnReturn(HWND hwnd);
static void BasicOnReturn(HWND hColors, PCHANDLES pch);
static void DrawBasicFocus(PCHANDLES pch, int x, int y);
static HWND CreateWebList(int x, int y, int w, int h, HWND hParent);
static HWND CreateBasicWindow(int x, int y, int w, int h, HWND hParent);
static LRESULT CALLBACK Custom_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Custom_OnDestroy(HWND hwnd);
static HBRUSH Custom_OnCtlColorStatic(HWND hwnd, HDC hdc, HWND hwndChild, int type);
static void Custom_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Custom_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
static LRESULT CALLBACK Rainbow_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Rainbow_OnPaint(HWND hwnd);
static void Rainbow_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
static void Rainbow_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
static void ColorChanged(COLORREF crColor, HWND hCustom);
static void RGBChanged(HWND hCustom);
static void HSLChanged(HWND hCustom);
static void MouseMoveClick(HWND hwnd, int x, int y);
static void SetHSL(HWND hCustom, COLORREF crColor, BOOL setSlider);
static BYTE _cred(int color);
static BYTE _cgreen(int color);
static BYTE _cblue(int color);

// basic colors
static int arrBasics[8][6] = {0xFFFFFF,0xE0E0E0,0xC0C0C0,0x808080,0x404040,0x0,		
							0xC0C0FF,0x8080FF,0xFF,0xC0,0x80,0x40,					
							0xC0E0FF,0x80C0FF,0x80FF,0x40C0,0x4080,0x404080,			
							0xC0FFFF,0x80FFFF,0xFFFF,0xC0C0,0x8080,0x4040,			
							0xC0FFC0,0x80FF80,0xFF00,0xC000,0x8000,0x4000,			
							0xFFFFC0,0xFFFF80,0xFFFF00,0xC0C000,0x808000,0x404000,	
							0xFFC0C0,0xFF8080,0xFF0000,0xC00000,0x800000,0x400000,	
							0xFFC0FF,0xFF80FF,0xFF00FF,0xC000C0,0x800080,0x400040};
// basic colors rectangles
static RECT arrRectBasics[8][6];

// web colors
static int arrWebs[] = {0x000000,0x696969,0x808080,0x0A9A9A9,0x0C0C0C0,0x0D3D3D3,0x0DCDCDC,0x0F5F5F5,0x0FFFFFF,
						0x8F8FBC,0x5C5CCD,0x2A2AA5,0x2222B2,0x8080F0,0x000080,0x00008B,0x0000FF,0x0FAFAFF,
						0x0E1E4FF,0x7280FA,0x4763FF,0x7A96E9,0x507FFF,0x0045FF,0x7AA0FF,0x2D52A0,0x0EEF5FF,
						0x1E69D2,0x13458B,0x60A4F4,0x0B9DAFF,0x3F85CD,0x0E6F0FA,0x0C4E4FF,0x008CFF,0x87B8DE,
						0x8CB4D2,0x0D7EBFA,0x0ADDEFF,0x0CDEBFF,0x0D5EFFF,0x0B5E4FF,0x00A5FF,0x0B3DEF5,0x0E6F5FD,
						0x0F0FAFF,0x0B86B8,0x20A5DA,0x0DCF8FF,0x00D7FF,0x8CE6F0,0x0CDFAFF,0x0AAE8EE,0x6BB7BD,
						0x0DCF5F5,0x0D2FAFA,0x008080,0x00FFFF,0x0E0FFFF,0x0F0FFFF,0x238E6B,0x32CD9A,0x2F6B55,
						0x2FFFAD,0x00FF7F,0x00FC7C,0x8FBC8F,0x228B22,0x32CD32,0x90EE90,
						0x98FB98,0x006400,0x008000,0x00FF00,0x0F0FFF0,0x578B2E,0x71B33C,0x7FFF00,
						0x0FAFFF5,0x9AFA00,0x0AACD66,0x0D4FF7F,0x0D0E040,0x0AAB220,0x0CCD148,
						0x4F4F2F,0x0EEEEAF,0x808000,0x8B8B00,0x0FFFF00,0x0FFFF00,0x0FFFFE0,0x0FFFFF0,0x0D1CE00,
						0x0A09E5F,0x0E6E0B0,0x0E6D8AD,0x0FFBF00,0x0EBCE87,0x0FACE87,0x0B48246,0x0FFF8F0,0x0FF901E,
						0x908070,0x998877,0x0DEC4B0,0x0ED9564,0x0E16941,0x701919,0x0FAE6E6,
						0x800000,0x8B0000,0x0CD0000,0x0FF0000,0x0FFF8F8,0x0CD5A6A,0x8B3D48,0x0EE687B,
						0x0DB7093,0x0E22B8A,0x82004B,0x0CC3299,0x0D30094,0x0D355BA,0x0D8BFD8,0x0DDA0DD,0x0EE82EE,
						0x800080,0x8B008B,0x0FF00FF,0x0FF00FF,0x0D670DA,0x8515C7,0x9314FF,0x0B469FF,0x0F5F0FF,0x9370DB,
						0x3C14DC,0x0CBC0FF,0x0C1B6FF
						};
// web colors names
static wchar_t * arrNamesWeb[] = {L"Black",L"DimGray",L"Gray",L"DarkGray",L"Silver",L"LightGray",L"Gainsboro",L"WhiteSmoke",L"White",
								L"RosyBrown",L"IndianRed",L"Brown",L"FireBrick",L"LightCoral",L"Maroon",L"DarkRed",L"Red",L"Snow",
								L"MistyRose",L"Salmon",L"Tomato",L"DarkSalmon",L"Coral",L"OrangeRed",L"LightSalmon",L"Sienna",L"Seashell",
								L"Chocolate",L"SaddleBrown",L"SandyBrown",L"PeachPuff",L"Peru",L"Linen",L"Bisque",L"DarkOrange",L"BurlyWood",
								L"Tan",L"AntiqueWhite",L"NavajoWhite",L"BlanchedAlmond",L"PapayaWhip",L"Moccasin",L"Orange",L"Wheat",L"OldLace",
								L"FloralWhite",L"DarkGoldenrod",L"Goldenrod",L"Cornsilk",L"Gold",L"Khaki",L"LemonChiffon",L"PaleGoldenrod",L"DarkKhaki",
								L"Beige",L"LightGoldenrodYellow",L"Olive",L"Yellow",L"LightYellow",L"Ivory",L"OliveDrab",L"YellowGreen",L"DarkOliveGreen",
								L"GreenYellow",L"Chartreuse",L"LawnGreen",L"DarkSeaGreen",L"ForestGreen",L"LimeGreen",L"LightGreen",
								L"PaleGreen",L"DarkGreen",L"Green",L"Lime",L"Honeydew",L"SeaGreen",L"MediumSeaGreen",L"SpringGreen",
								L"MintCream",L"MediumSpringGreen",L"MediumAquamarine",L"Aquamarine",L"Turquoise",L"LightSeaGreen",L"MediumTurquoise",
								L"DarkSlateGray",L"PaleTurquoise",L"Teal",L"DarkCyan",L"Aqua",L"Cyan",L"LightCyan",L"Azure",L"DarkTurquoise",
								L"CadetBlue",L"PowderBlue",L"LightBlue",L"DeepSkyBlue",L"SkyBlue",L"LightSkyBlue",L"SteelBlue",L"AliceBlue",L"DodgerBlue",
								L"SlateGray",L"LightSlateGray",L"LightSteelBlue",L"CornflowerBlue",L"RoyalBlue",L"MidnightBlue",L"Lavender",
								L"Navy",L"DarkBlue",L"MediumBlue",L"Blue",L"GhostWhite",L"SlateBlue",L"DarkSlateBlue",L"MediumSlateBlue",
								L"MediumPurple",L"BlueViolet",L"Indigo",L"DarkOrchid",L"DarkViolet",L"MediumOrchid",L"Thistle",L"Plum",L"Violet",
								L"Purple",L"DarkMagenta",L"Magenta",L"Fuchsia",L"Orchid",L"MediumVioletRed",L"DeepPink",L"HotPink",L"LavenderBlush",L"PaleVioletRed",
								L"Crimson",L"Pink",L"LightPink"
								};

static BOOL			bRegistered = FALSE, bDrawBasic = TRUE, bRaiseUpdate = TRUE;

void ShowColorsWindow(int x, int y, HWND hColors, COLORREF crColor){
	int					selected = 0;
	PCHANDLES			pch = GetHandles(hColors);

	SetWindowPos(hColors, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	
	ColorChanged(crColor, pch->hCustom);
	if((selected = IsWebColor(pch, crColor)) != -1){
		// if received color is web color - show web colors list and select appropriate tab
		SendMessageW(pch->hWebs, LB_SETCURSEL, selected, 0);
		ShowWindow(pch->hBasics, SW_HIDE);
		ShowWindow(pch->hWebs, SW_SHOW);
		ShowWindow(pch->hCustom, SW_HIDE);
		SendMessageW(pch->hTab, TCM_SETCURSEL, 1, 0);
		SetFocus(pch->hWebs);
	}
	else if((selected = IsBasicColor(crColor)) != -1){
		pch->currBasic.x = LOWORD(selected);
		pch->currBasic.y = HIWORD(selected);
		ShowWindow(pch->hBasics, SW_SHOW);
		ShowWindow(pch->hWebs, SW_HIDE);
		ShowWindow(pch->hCustom, SW_HIDE);
		SendMessageW(pch->hTab, TCM_SETCURSEL, 0, 0);
		RedrawWindow(pch->hBasics, NULL, NULL, RDW_INVALIDATE);
		SetFocus(pch->hBasics);
	}
	else{
		// otherwise show custom colors
		PCUSTHANDLES	pcth = GetCustHandles(pch->hCustom);
		ShowWindow(pch->hBasics, SW_HIDE);
		ShowWindow(pch->hWebs, SW_HIDE);
		ShowWindow(pch->hCustom, SW_SHOW);
		SendMessageW(pch->hTab, TCM_SETCURSEL, 2, 0);
		SetFocus(pcth->hHue);
	}
}

HWND CreateGColorsWindowW(int x, int y, HWND hParentWnd, COLORREF crColor){
	PCHANDLES			pch;
	static HWND			hColors;
	
	// if the function is called for the first time
	if(!bRegistered){
		// register gcolor window
		bRegistered = RegisterGColorsWindowW();
		if(!bRegistered)
			return NULL;
	}
	hColors = CreateWindowExW(0, GCOLOR_CLASS_W, NULL, WS_POPUP, x, y, 177, 213, hParentWnd, NULL, GetModuleHandleW(0), NULL);
	if(hColors == NULL)
		return NULL;
	pch = GetHandles(hColors);
	// save parent window handle
	pch->hParent = hParentWnd;
	// save or clear color type
	pch->savedColor = crColor;
	// set basic colors rectangle to the first
	pch->currBasic.x = 0;
	pch->currBasic.y = 0;
	ColorChanged(crColor, pch->hCustom);
	return hColors;
}

static int IsWebColor(PCHANDLES pch, COLORREF crColor){
	int count = 0;

	count = SendMessageW(pch->hWebs, LB_GETCOUNT, 0, 0);

	for(int i = 0; i < count; i++){
		PGLIST_IMAGE	pData;
		pData = (PGLIST_IMAGE)SendMessageW(pch->hWebs, LB_GETITEMDATA, i, 0);
		if(crColor == pData->dwValue)
			return i;
	}
	return -1;
}

static int IsBasicColor(COLORREF crColor){
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 6; j++){
			if(arrBasics[i][j] == crColor){
				return MAKELONG(i, j);
			}
		}
	}
	return -1;
}

static BOOL RegisterGColorsWindowW(void){

    WNDCLASSEXW 			wcx;

	ZeroMemory(&wcx, sizeof(wcx));
    wcx.cbSize = sizeof(wcx);
	wcx.cbWndExtra = 4;
    wcx.hInstance = GetModuleHandleW(0);
	wcx.lpfnWndProc = Colors_WndProc;
    wcx.lpszClassName = GCOLOR_CLASS_W;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    if (!RegisterClassExW(&wcx)){
		if(GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
			return FALSE;
	}
	//register custom window
	wcx.cbWndExtra == 4;
	wcx.lpfnWndProc = Custom_WndProc;
	wcx.lpszClassName = CUSTOM_CLASS;
	if (!RegisterClassExW(&wcx)){
		if(GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
			return FALSE;
	}
	//register rainbow window
	wcx.cbWndExtra == 0;
	wcx.lpfnWndProc = Rainbow_WndProc;
	wcx.lpszClassName = GRAINBOW_CLASS;
	wcx.hbrBackground = NULL;
	wcx.hCursor = LoadCursor(NULL, IDC_CROSS);
	if (!RegisterClassExW(&wcx)){
		if(GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
			return FALSE;
	}
	return TRUE;
}

static LRESULT CALLBACK Custom_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		HANDLE_MSG (hwnd, WM_DESTROY, Custom_OnDestroy);
		HANDLE_MSG (hwnd, WM_CTLCOLORSTATIC, Custom_OnCtlColorStatic);
		HANDLE_MSG (hwnd, WM_COMMAND, Custom_OnCommand);
		HANDLE_MSG (hwnd, WM_HSCROLL, Custom_OnHScroll);

		case WM_KEYDOWN:
			if(wParam == VK_ESCAPE)
				ShowWindow(GetParent(hwnd), SW_HIDE);
			else
				return DefWindowProcW(hwnd, msg, wParam, lParam);
			return 0;
		default: 
			return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

static void Custom_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(code != TB_THUMBPOSITION && code != TB_THUMBTRACK){
		pos = TrackBar_GetPos(hwndCtl);
	}
	SetDlgItemInt(hwnd, ID_L, pos, FALSE);
}

static void Custom_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case ID_R:
		case ID_G:
		case ID_B:
			switch(codeNotify){
				case EN_CHANGE:{
					if(!bRaiseUpdate)
						break;
					wchar_t		szBuffer[12];
					GetDlgItemTextW(hwnd, id, szBuffer, 12);
					if(wcslen(szBuffer) == 0)
						SetDlgItemInt(hwnd, id, 0, FALSE);
					else{
						if(GetDlgItemInt(hwnd, id, NULL, FALSE) > 255){
							SetDlgItemInt(hwnd, id, 255, FALSE);
						}
					}
					RGBChanged(hwnd);
					break;
				}
			}
			break;
		case ID_H:
		case ID_S:
		case ID_L:
			switch(codeNotify){
				case EN_CHANGE:{
					if(!bRaiseUpdate)
						break;
					wchar_t		szBuffer[12];
					GetDlgItemTextW(hwnd, id, szBuffer, 12);
					if(wcslen(szBuffer) == 0)
						SetDlgItemInt(hwnd, id, 0, FALSE);
					else{
						if(id == ID_H && GetDlgItemInt(hwnd, id, NULL, FALSE) > 239)
							SetDlgItemInt(hwnd, id, 239, FALSE);
						else{
							if(GetDlgItemInt(hwnd, id, NULL, FALSE) > 240)
								SetDlgItemInt(hwnd, id, 240, FALSE);
						}
					}
					HSLChanged(hwnd);
					break;
				}
			}
			break;
		case IDOK:{
			PCHANDLES pch = GetHandles(GetParent(hwnd));
			PostMessage(pch->hParent, GCN_COLOR_SELECTED, 0, RGB(GetDlgItemInt(hwnd, ID_R, NULL, FALSE), GetDlgItemInt(hwnd, ID_G, NULL, FALSE), GetDlgItemInt(hwnd, ID_B, NULL, FALSE)));
			ShowWindow(GetParent(hwnd), SW_HIDE);
			break;
		}
	}
}

static HBRUSH Custom_OnCtlColorStatic(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
	PCUSTHANDLES 	pcth = GetCustHandles(hwnd);

	if(hwndChild == pcth->hSample)
		return pcth->hBrush;
	else{
		SetBkMode(hdc, TRANSPARENT);
		return GetSysColorBrush(COLOR_BTNFACE);
	}
}

static void Custom_OnDestroy(HWND hwnd)
{
	PCUSTHANDLES pcth = GetCustHandles(hwnd);
	if(pcth){
		if(pcth->hBrush)
			DeleteBrush(pcth->hBrush);
		free(pcth);
	}
}

static LRESULT CALLBACK Rainbow_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		HANDLE_MSG (hwnd, WM_PAINT, Rainbow_OnPaint);
		HANDLE_MSG (hwnd, WM_MOUSEMOVE, Rainbow_OnMouseMove);
		HANDLE_MSG (hwnd, WM_LBUTTONUP, Rainbow_OnLButtonUp);

		default: 
			return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

static void Rainbow_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	MouseMoveClick(hwnd, x, y);
}

static void Rainbow_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	if(keyFlags == MK_LBUTTON){
		MouseMoveClick(hwnd, x, y);
	}
}

static void Rainbow_OnPaint(HWND hwnd)
{
	PAINTSTRUCT		ps;
	RECT			rc;

	BeginPaint(hwnd, &ps);
	GetClientRect(hwnd, &rc);
	DrawRainbow(ps.hdc, &rc);
	EndPaint(hwnd, &ps);
}

static LRESULT CALLBACK Colors_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR			lpnmhdr;
	int				currTab;
	POINT			pt;

	switch(msg){
	HANDLE_MSG (hwnd, WM_CREATE, Colors_OnCreate);
	HANDLE_MSG (hwnd, WM_DESTROY, Colors_OnDestroy);
	HANDLE_MSG (hwnd, WM_KEYDOWN, Colors_OnKeyDown);
	// case GCM_CLOSE:
		// bRegistered = FALSE;
		// hParent = NULL;
		// DestroyWindow(hwnd);
		// return 0;
	case WM_ACTIVATE:
		// hide gcolor window when it becames inactive
		if(wParam == WA_INACTIVE){
			ShowWindow(hwnd, SW_HIDE);
		}
		return 0;
	case WM_NOTIFY:
		lpnmhdr = (LPNMHDR)lParam;
		if(lpnmhdr->code == TCN_SELCHANGE){
			PCHANDLES pch = GetHandles(hwnd);
			// show appropriate colors
			currTab = SendMessageW(lpnmhdr->hwndFrom, TCM_GETCURSEL, 0, 0);
			switch(currTab){
			case 0:
				ShowWindow(pch->hBasics, SW_SHOW);
				ShowWindow(pch->hWebs, SW_HIDE);
				ShowWindow(pch->hCustom, SW_HIDE);
				SetFocus(pch->hBasics);
				break;
			case 1:
				ShowWindow(pch->hBasics, SW_HIDE);
				ShowWindow(pch->hWebs, SW_SHOW);
				ShowWindow(pch->hCustom, SW_HIDE);
				SetFocus(pch->hWebs);
				break;
			case 2:
				ShowWindow(pch->hBasics, SW_HIDE);
				ShowWindow(pch->hWebs, SW_HIDE);
				ShowWindow(pch->hCustom, SW_SHOW);
				break;
			}
			return 0;
		}
		else
			return DefWindowProcW(hwnd, msg, wParam, lParam);
		break;
	case WM_COMMAND:{
		PCHANDLES pch = GetHandles(hwnd);
		if((HWND)lParam == pch->hBasics && HIWORD(wParam) == STN_CLICKED){
			// select appropriate basic colors rectangle
			GetCursorPos(&pt);
			MapWindowPoints(HWND_DESKTOP, pch->hBasics, &pt, 1);
			for(int i = 0; i < 8; i++){
				for(int j = 0; j < 6; j++){
					if(PtInRect(&arrRectBasics[i][j], pt)){
						if(pch->currBasic.x != i || pch->currBasic.y != j){
							DrawBasicFocus(pch, i, j);
							BasicOnReturn(hwnd, pch);
							return 0;
						}
					}
				}	
			}		
		}
	}
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	default: 
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

static void Colors_OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	if(vk == VK_ESCAPE){
		ShowWindow(hwnd, SW_HIDE);
	}
}

static void Colors_OnDestroy(HWND hwnd)
{
	PCHANDLES pch = GetHandles(hwnd);
	if(pch){
		if(pch->hFont)
			DeleteFont(pch->hFont);
		free(pch);
	}
}

static BOOL Colors_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	RECT			rc;
	TCITEMW			tciw;
	wchar_t			szBufferU[128];
	HFONT			hFont;
	HWND			hTemp;
	HINSTANCE		hInstance = GetModuleHandleW(0);
	int				w, t;

	// create font, main window and set its font
	hFont = CreateFontW(-MulDiv(8, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"MS Shell Dialog");

	SendMessageW(hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);

	// allocate room for handles
	PCHANDLES pch = calloc(1, sizeof(CHANDLES));
	SetWindowLongPtrW(hwnd, O_HANDLES, (LONG_PTR)pch);

	// save font handle
	pch->hFont = hFont;

	GetClientRect(hwnd, &rc);
	// create tab window and set its font
	pch->hTab = CreateWindowExW(0, L"SysTabControl32", NULL, WS_CHILD | WS_VISIBLE, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hwnd, NULL, hInstance, NULL);

	SendMessageW(pch->hTab, WM_SETFONT, (WPARAM)hFont, TRUE);
	// add tabs
	ZeroMemory(&tciw, sizeof(tciw));
	tciw.mask = TCIF_TEXT;
	tciw.pszText = szBufferU;
	wcscpy(szBufferU, T_CAPTION_1_W);
	SendMessageW(pch->hTab, TCM_INSERTITEMW, 0, (LPARAM)&tciw);
	wcscpy(szBufferU, T_CAPTION_2_W);
	SendMessageW(pch->hTab, TCM_INSERTITEMW, 1, (LPARAM)&tciw);
	wcscpy(szBufferU, T_CAPTION_3_W);
	SendMessageW(pch->hTab, TCM_INSERTITEMW, 2, (LPARAM)&tciw);
	SendMessageW(pch->hTab, TCM_ADJUSTRECT, FALSE, (LPARAM)&rc);

	// create basic colors window
	pch->hBasics = CreateBasicWindow(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hwnd);

	// create web colors window
	pch->hWebs = CreateWebList(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hwnd);
	SendMessageW(pch->hWebs, WM_SETFONT, (WPARAM)hFont, TRUE);

	//create custom window
	pch->hCustom = CreateWindowExW(0, CUSTOM_CLASS, NULL, WS_CHILD | WS_VISIBLE, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hwnd, NULL, hInstance, 0);
	PCUSTHANDLES	pcth = (PCUSTHANDLES)calloc(1, sizeof(CUSTHANDLES));
	SetWindowLongPtrW(pch->hCustom, O_HANDLES, (LONG_PTR)pcth);
	//create rainbow window
	pcth->hRainbow = CreateWindowExW(0, GRAINBOW_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, rc.right - rc.left, (rc.bottom - rc.top) / 2, pch->hCustom, NULL, hInstance, 0);
	//get needed measures
	SIZE	sz, sz1;
	HDC		hdc = GetDC(NULL);
	GetTextExtentPoint32W(hdc, L"9999", 4, &sz);
	GetTextExtentPoint32W(hdc, L"+", 1, &sz1);
	ReleaseDC(NULL, hdc);
	w = rc.right - rc.left;
	t = (rc.bottom - rc.top) / 2 + 2;
	int 	l1 = 2, l2 = 2 + sz.cx / 3 + 2, l3 = w - 2 - sz.cx / 3 - 2 - sz.cx, l4 = w - 2 - sz.cx;
	int w1 = w;
	w = w - (2 + sz.cx / 3 + 2 + sz.cx + 4) * 2;
	int		l5 = (w1 - w) / 2;
	// w = l3 - 2 - l5;
	//create other custom child windows
	pcth->hSlider = CreateWindowExW(0, TRACKBAR_CLASSW, NULL, WS_CHILD | WS_VISIBLE | TBS_HORZ, l1 + sz.cx / 4, t + 2, rc.right - rc.left - 4 - sz.cx / 2, 16, pch->hCustom, (HMENU)ID_SLIDER, hInstance, 0);
	SendMessageW(pcth->hSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 240));
	t += 14;

	hTemp = CreateWindowExW(0, L"STATIC", L"-", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_RIGHT, 0, 0, sz1.cx, 16, pch->hCustom, NULL, hInstance, 0);
	SendMessageW(hTemp, WM_SETFONT, (WPARAM)hFont, TRUE);
	TrackBar_SetBuddy(pcth->hSlider, TRUE, hTemp);
	hTemp = CreateWindowExW(0, L"STATIC", L"+", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_LEFT, 0, 0, sz1.cx, 16, pch->hCustom, NULL, hInstance, 0);
	SendMessageW(hTemp, WM_SETFONT, (WPARAM)hFont, TRUE);
	TrackBar_SetBuddy(pcth->hSlider, FALSE, hTemp);

	hTemp = CreateWindowExW(0, L"STATIC", L"H:", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, l1, t + 8, sz.cx / 3, sz.cy, pch->hCustom, NULL, hInstance, 0);
	SendMessageW(hTemp, WM_SETFONT, (WPARAM)hFont, TRUE);
	hTemp = CreateWindowExW(0, L"STATIC", L"R:", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, l3, t + 8, sz.cx / 3, sz.cy, pch->hCustom, NULL, hInstance, 0);
	SendMessageW(hTemp, WM_SETFONT, (WPARAM)hFont, TRUE);
	pcth->hHue = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_AUTOHSCROLL, l2, t + 8, sz.cx, sz.cy, pch->hCustom, (HMENU)ID_H, hInstance, 0);
	SendMessageW(pcth->hHue, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessageW(pcth->hHue, EM_SETLIMITTEXT, 3, 0);
	SetWindowLongPtrW(pcth->hHue, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(pcth->hHue, GWLP_WNDPROC, (LONG_PTR)EditProc));
	pcth->hRed = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_AUTOHSCROLL, l4, t + 8, sz.cx, sz.cy, pch->hCustom, (HMENU)ID_R, hInstance, 0);
	SendMessageW(pcth->hRed, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessageW(pcth->hRed, EM_SETLIMITTEXT, 3, 0);
	SetWindowLongPtrW(pcth->hRed, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(pcth->hRed, GWLP_WNDPROC, (LONG_PTR)EditProc));
	hTemp = CreateWindowExW(0, L"STATIC", L"S:", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, l1, t + 8 + sz.cy + 2, sz.cx / 3, sz.cy, pch->hCustom, NULL, hInstance, 0);
	SendMessageW(hTemp, WM_SETFONT, (WPARAM)hFont, TRUE);
	hTemp = CreateWindowExW(0, L"STATIC", L"G:", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, l3, t + 8 + sz.cy + 2, sz.cx / 3, sz.cy, pch->hCustom, NULL, hInstance, 0);
	SendMessageW(hTemp, WM_SETFONT, (WPARAM)hFont, TRUE);
	pcth->hSatur = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_AUTOHSCROLL, l2, t + 8 + sz.cy + 2, sz.cx, sz.cy, pch->hCustom, (HMENU)ID_S, hInstance, 0);
	SendMessageW(pcth->hSatur, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessageW(pcth->hSatur, EM_SETLIMITTEXT, 3, 0);
	SetWindowLongPtrW(pcth->hSatur, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(pcth->hSatur, GWLP_WNDPROC, (LONG_PTR)EditProc));
	pcth->hGreen = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_AUTOHSCROLL, l4, t + 8 + sz.cy + 2, sz.cx, sz.cy, pch->hCustom, (HMENU)ID_G, hInstance, 0);
	SendMessageW(pcth->hGreen, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessageW(pcth->hGreen, EM_SETLIMITTEXT, 3, 0);
	SetWindowLongPtrW(pcth->hGreen, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(pcth->hGreen, GWLP_WNDPROC, (LONG_PTR)EditProc));
	hTemp = CreateWindowExW(0, L"STATIC", L"L:", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, l1, t + 8 + 2 * sz.cy + 4, sz.cx / 3, sz.cy, pch->hCustom, NULL, hInstance, 0);
	SendMessageW(hTemp, WM_SETFONT, (WPARAM)hFont, TRUE);
	hTemp = CreateWindowExW(0, L"STATIC", L"B:", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, l3, t + 8 + 2 * sz.cy + 4, sz.cx / 3, sz.cy, pch->hCustom, NULL, hInstance, 0);
	SendMessageW(hTemp, WM_SETFONT, (WPARAM)hFont, TRUE);
	pcth->hLuma = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_AUTOHSCROLL, l2, t + 8 + 2 * sz.cy + 4, sz.cx, sz.cy, pch->hCustom, (HMENU)ID_L, hInstance, 0);
	SendMessageW(pcth->hLuma, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessageW(pcth->hLuma, EM_SETLIMITTEXT, 3, 0);
	SetWindowLongPtrW(pcth->hLuma, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(pcth->hLuma, GWLP_WNDPROC, (LONG_PTR)EditProc));
	pcth->hBlue = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_AUTOHSCROLL, l4, t + 8 + 2 * sz.cy + 4, sz.cx, sz.cy, pch->hCustom, (HMENU)ID_B, hInstance, 0);
	SendMessageW(pcth->hBlue, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessageW(pcth->hBlue, EM_SETLIMITTEXT, 3, 0);
	SetWindowLongPtrW(pcth->hBlue, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(pcth->hBlue, GWLP_WNDPROC, (LONG_PTR)EditProc));
	pcth->hSample = CreateWindowExW(WS_EX_STATICEDGE, L"STATIC", NULL, WS_CHILD | WS_VISIBLE, l5, t + 8, w, 2 * sz.cy, pch->hCustom, NULL, hInstance, 0);
	pcth->hOK = CreateWindowExW(0, L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, l5, t + 8 + 2 * sz.cy + 4, w, sz.cy, pch->hCustom, (HMENU)IDOK, hInstance, 0);
	SendMessageW(pcth->hOK, WM_SETFONT, (WPARAM)hFont, TRUE);
	return TRUE;
}

static HWND CreateWebList(int x, int y, int w, int h, HWND hParent){
	HWND			hWeb;
	GLIST_IMAGE		gi;
	WNDPROC			hProc;

	ZeroMemory(&gi, sizeof(gi));
	gi.nType = GLIST_I_COLOR;
	gi.frame = TRUE;
	gi.szImage.cx = 22;
	gi.szImage.cy = 12;
	gi.itemHeight = 16;

	hWeb = CreateGListWindowW(WS_EX_CLIENTEDGE ,WS_VISIBLE | WS_CHILD, x, y, w, h, hParent);
	if(hWeb == NULL)
		return NULL;
	// subclass web colors window
	SetLastError(0);
	hProc = (WNDPROC)SetWindowLongPtrW(hWeb, GWLP_WNDPROC, (LONG_PTR)LstProc);
	if(hProc == 0 && GetLastError() != 0)
		return NULL;
	SetPropW(hWeb, LST_CLR_PROC_PROP_W, hProc);

	// add colors entries
	for(int i = 0; i < NELEMS(arrWebs); i++){
		gi.dwValue = arrWebs[i];

		if(GList_AddEntryW(hWeb, &gi, arrNamesWeb[i]) == -1)
			return NULL;
	}
	return hWeb;
}

static HWND CreateBasicWindow(int x, int y, int w, int h, HWND hParent){

	HWND		hBasic;
	WNDPROC		hProc;

	hBasic = CreateWindowExW(0, L"static", NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY, x, y, w, h, hParent, NULL, GetModuleHandleW(0), NULL);
	if(hBasic == NULL)
		return NULL;
	// subclass basic colors window
	SetLastError(0);
	hProc = (WNDPROC)SetWindowLongPtrW(hBasic, GWLP_WNDPROC, (LONG_PTR)BasicProc);
	if(hProc == 0 && GetLastError() != 0)
		return NULL;
	SetPropW(hBasic, LST_CLR_PROC_PROP_W, hProc);

	return hBasic;
}

static void CreateBasicRects(LPRECT lpClient){
	int sideX, sideY;

	// create basic colors rectangle
	sideX = (lpClient->right - lpClient->left - 9) / 8;
	sideY = (lpClient->bottom - lpClient->top - 7) / 6;
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 6; j++){
			SetRect(&arrRectBasics[i][j],i * (sideX + 1) + 1, j * (sideY + 1) + 1, (i + 1) * (sideX + 1), (j + 1) * (sideY + 1));
		}
	}
}

static void DrawBasicRects(HDC hdc){
	HBRUSH		hbr;

	// draw basic colors
	for(int i =0; i < 8; i++){
		for(int j = 0; j < 6; j++){
			hbr = CreateSolidBrush(arrBasics[i][j]);
			FillRect(hdc, &arrRectBasics[i][j], hbr);
			DeleteBrush(hbr);
		}
	}
}

static LRESULT CALLBACK BasicProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT		ps;
	RECT			rc;
	HDC				hdc;
	WNDPROC			hProc;
	PCHANDLES 		pch = GetHandles(GetParent(hwnd));

	hProc = (WNDPROC)GetPropW(hwnd, LST_CLR_PROC_PROP_W);

	switch(msg){
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			if(bDrawBasic){
				GetClientRect(hwnd, &rc);
				CreateBasicRects(&rc);
				FillRect(hdc, &rc, GetSysColorBrush(COLOR_BTNFACE));
				DrawBasicRects(hdc);
			}
			// draw focus rectangle arround the color or remove previous focus rectangle
			DrawFocusRect(hdc, &arrRectBasics[pch->currBasic.x][pch->currBasic.y]);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_KEYDOWN:
			if(wParam == VK_ESCAPE){
				ShowWindow(GetParent(hwnd), SW_HIDE);
			}
			else if(wParam == VK_RETURN)
				BasicOnReturn(GetParent(hwnd), pch);
			else if(wParam == VK_RIGHT){
				if(pch->currBasic.x < 7)
					DrawBasicFocus(pch, pch->currBasic.x + 1, pch->currBasic.y);
			}
			else if(wParam == VK_LEFT){
				if(pch->currBasic.x > 0)
					DrawBasicFocus(pch, pch->currBasic.x - 1, pch->currBasic.y);
			}
			else if(wParam == VK_DOWN){
				if(pch->currBasic.y < 5)
					DrawBasicFocus(pch, pch->currBasic.x, pch->currBasic.y + 1);
			}
			else if(wParam == VK_UP){
				if(pch->currBasic.y > 0)
					DrawBasicFocus(pch, pch->currBasic.x, pch->currBasic.y - 1);
			}
			else
				return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
			return 0;
		case WM_DESTROY:
			RemovePropW(hwnd, LST_CLR_PROC_PROP_W);
			return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
		default:
			return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
	}
}

static LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	WNDPROC		hProc;

	hProc = (WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

	switch(msg){
		case WM_SETFOCUS:
			CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
			SendMessageW(hwnd, EM_SETSEL, 0, -1);
			return 0;
		case WM_KEYDOWN:
			if(wParam != VK_TAB)
				if(wParam == VK_ESCAPE)
					ShowWindow(GetParent(GetParent(hwnd)), SW_HIDE);
				else
					return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
			else{
				PCUSTHANDLES	pcth = GetCustHandles(GetParent(hwnd));
				if(hwnd == pcth->hHue)
					SetFocus(pcth->hSatur);
				else if(hwnd == pcth->hSatur)
					SetFocus(pcth->hLuma);
				else if(hwnd == pcth->hLuma)
					SetFocus(pcth->hRed);
				else if(hwnd == pcth->hRed)
					SetFocus(pcth->hGreen);
				else if(hwnd == pcth->hGreen)
					SetFocus(pcth->hBlue);
				else if(hwnd == pcth->hBlue)
					SetFocus(pcth->hHue);

				return 0;
			}
		default:
			return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
	}
}

static LRESULT CALLBACK LstProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	WNDPROC		hProc;

	hProc = (WNDPROC)GetPropW(hwnd, LST_CLR_PROC_PROP_W);

	switch(msg){
	case WM_LBUTTONUP:
		ListOnReturn(hwnd);
		return 0;
	case WM_KEYDOWN:
		if(wParam == VK_RETURN)
			ListOnReturn(hwnd);
		else if(wParam == VK_ESCAPE){
			ShowWindow(GetParent(hwnd), SW_HIDE);
		}
		else
			return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
		return 0;
	case WM_DESTROY:
		RemovePropW(hwnd, LST_CLR_PROC_PROP_W);
		return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
	default:
		return CallWindowProcW(hProc, hwnd, msg, wParam, lParam);
	}
}

static void ListOnReturn(HWND hwnd){
	int					index;
	PCHANDLES			pch = GetHandles(GetParent(hwnd));

	index = SendMessageW(hwnd, LB_GETCURSEL, 0, 0);
	
	// return selected color
	if(index != -1){
		PGLIST_IMAGE	pData;
		pData = (PGLIST_IMAGE)SendMessageW(hwnd, LB_GETITEMDATA, index, 0);
		PostMessage(pch->hParent, GCN_COLOR_SELECTED, 0, (LPARAM)pData->dwValue);
		ShowWindow(GetParent(hwnd), SW_HIDE);
	}
}

static void BasicOnReturn(HWND hColors, PCHANDLES pch){
	PostMessage(pch->hParent, GCN_COLOR_SELECTED, 0, (LPARAM)arrBasics[pch->currBasic.x][pch->currBasic.y]);
	ShowWindow(hColors, SW_HIDE);
}

static void DrawBasicFocus(PCHANDLES pch, int x, int y){
	// remove previous focus rectangle arround the basic color and draw new one
	bDrawBasic = FALSE;
	RedrawWindow(pch->hBasics, NULL, NULL, RDW_INVALIDATE);
	pch->currBasic.x = x;
	pch->currBasic.y = y;
	RedrawWindow(pch->hBasics, NULL, NULL, RDW_INVALIDATE);
	bDrawBasic = TRUE;
}

static void HSLChanged(HWND hCustom){
	COLORREF			crColor;
	PCUSTHANDLES 		pcth = GetCustHandles(hCustom);

	bRaiseUpdate = FALSE;
	crColor = ColorHLSToRGB(GetDlgItemInt(hCustom, ID_H, NULL, FALSE), GetDlgItemInt(hCustom, ID_L, NULL, FALSE), GetDlgItemInt(hCustom, ID_S, NULL, FALSE));
	if(pcth->hBrush)
		DeleteBrush(pcth->hBrush);
	pcth->hBrush = CreateSolidBrush(crColor);
	RedrawWindow(pcth->hSample, NULL, NULL, RDW_INVALIDATE);
	SetDlgItemInt(hCustom, ID_R, _cred(crColor), FALSE);
	SetDlgItemInt(hCustom, ID_G, _cgreen(crColor), FALSE);
	SetDlgItemInt(hCustom, ID_B, _cblue(crColor), FALSE);
	TrackBar_SetPos(pcth->hSlider, TRUE, GetDlgItemInt(hCustom, ID_L, NULL, FALSE));
	bRaiseUpdate = TRUE;
}

static void RGBChanged(HWND hCustom){
	COLORREF			crColor;
	PCUSTHANDLES 		pcth = GetCustHandles(hCustom);

	bRaiseUpdate = FALSE;
	crColor = RGB(GetDlgItemInt(hCustom, ID_R, NULL, FALSE), GetDlgItemInt(hCustom, ID_G, NULL, FALSE), GetDlgItemInt(hCustom, ID_B, NULL, FALSE));
	if(pcth->hBrush)
		DeleteBrush(pcth->hBrush);
	pcth->hBrush = CreateSolidBrush(crColor);
	RedrawWindow(pcth->hSample, NULL, NULL, RDW_INVALIDATE);
	SetHSL(hCustom, crColor, TRUE);
	
	bRaiseUpdate = TRUE;
}

static void ColorChanged(COLORREF crColor, HWND hCustom){
	PCUSTHANDLES 		pcth = GetCustHandles(hCustom);

	bRaiseUpdate = FALSE;
	if(pcth->hBrush)
		DeleteBrush(pcth->hBrush);
	pcth->hBrush = CreateSolidBrush(crColor);
	RedrawWindow(pcth->hSample, NULL, NULL, RDW_INVALIDATE);
	SetDlgItemInt(hCustom, ID_R, _cred(crColor), FALSE);
	SetDlgItemInt(hCustom, ID_G, _cgreen(crColor), FALSE);
	SetDlgItemInt(hCustom, ID_B, _cblue(crColor), FALSE);
	SetHSL(hCustom, crColor, TRUE);
	bRaiseUpdate = TRUE;
}

static void MouseMoveClick(HWND hwnd, int x, int y){
	HDC					hdc;
	COLORREF			crColor;
	HWND				hCustom = GetParent(hwnd);
	PCUSTHANDLES 		pcth = GetCustHandles(hCustom);

	bRaiseUpdate = FALSE;
	hdc = GetDC(hwnd);
	crColor = GetPixel(hdc, x, y);
	ReleaseDC(hwnd, hdc);
	if(pcth->hBrush)
		DeleteBrush(pcth->hBrush);
	pcth->hBrush = CreateSolidBrush(crColor);
	RedrawWindow(pcth->hSample, NULL, NULL, RDW_INVALIDATE);
	SetHSL(hCustom, crColor, TRUE);
	SetDlgItemInt(hCustom, ID_R, _cred(crColor), FALSE);
	SetDlgItemInt(hCustom, ID_G, _cgreen(crColor), FALSE);
	SetDlgItemInt(hCustom, ID_B, _cblue(crColor), FALSE);
	bRaiseUpdate = TRUE;
}

static void SetHSL(HWND hCustom, COLORREF crColor, BOOL setSlider){
	unsigned short		h, s, l;

	ColorRGBToHLS(crColor, &h, &l, &s);
	SetDlgItemInt(hCustom, ID_H, h, FALSE);
	SetDlgItemInt(hCustom, ID_S, s, FALSE);
	SetDlgItemInt(hCustom, ID_L, l, FALSE);
	if(setSlider){
		PCUSTHANDLES	pcth = GetCustHandles(hCustom);
		TrackBar_SetPos(pcth->hSlider, TRUE, l);
	}
}

static BYTE _cred(int color){
	return (color & 0xff);
}

static BYTE _cgreen(int color){
	return (color & 0xff00) >> 8;
}

static BYTE _cblue(int color){
	return (color & 0xff0000) >> 16;
}
