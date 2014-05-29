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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <windows.h>
#include <windowsx.h>
#include "bell.h"
#include "global.h"
#include "shared.h"

#define	_AG_BELL_WND_CLASS		L"_ag_bell_wnd_class"
#define	_P_BELL_TIMER			L"_p_bell_timer"
#define	_P_BELL_BRUSH			L"_p_bell_brush"
#define _P_BELL_48				L"_p_bell_48"
#define _P_BELL_32				L"_p_bell_32"
#define _P_BELL_24				L"_p_bell_24"
#define _P_BELL_16				L"_p_bell_16"

static BOOL RegisterBell(HINSTANCE hInstance);
static void CALLBACK BellTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static LRESULT CALLBACK Bell_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Bell_OnDestroy(HWND hwnd);
static BOOL Bell_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
static void Bell_OnPaint(HWND hwnd);
static void Bell_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);

static int				m_IconSize = 48;
static HINSTANCE		m_hInstance;

HWND CreateBell(HWND hParent, HINSTANCE hInstance, int left, int top, BOOL onTop){
	HWND		hBell = NULL;
	int			exStyle = WS_EX_LAYERED;

	m_hInstance = hInstance;
	if(onTop)
		exStyle |= WS_EX_TOPMOST;
	hBell = CreateWindowExW(exStyle, _AG_BELL_WND_CLASS, NULL, WS_CHILD | WS_POPUP | WS_VISIBLE, left, top, 48, 48, hParent, NULL, hInstance, NULL);
	if(hBell == NULL){
		if(GetLastError() == ERROR_CANNOT_FIND_WND_CLASS){
			if(!RegisterBell(hInstance)){
				return NULL;
			}
			hBell = CreateWindowExW(exStyle, _AG_BELL_WND_CLASS, NULL, WS_CHILD | WS_POPUP | WS_VISIBLE, left, top, 48, 48, hParent, NULL, hInstance, NULL);
		}
		else{
			return NULL;
		}
	}
	
	return hBell;
}

static LRESULT CALLBACK Bell_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_DESTROY, Bell_OnDestroy);
	HANDLE_MSG (hwnd, WM_CREATE, Bell_OnCreate);
	HANDLE_MSG (hwnd, WM_PAINT, Bell_OnPaint);
	HANDLE_MSG (hwnd, WM_LBUTTONDOWN, Bell_OnLButtonDown);

	default: return DefWindowProc (hwnd, msg, wParam, lParam);
	}
}

static void Bell_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	DestroyWindow(hwnd);
}

static BOOL Bell_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	HMODULE		hIcons;
	SetPropW(hwnd, _P_BELL_BRUSH, (HANDLE)CreateSolidBrush(RGB(255, 0, 255)));
	hIcons = LoadLibraryExW(g_NotePaths.IconsPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if(hIcons){
		SetPropW(hwnd, _P_BELL_48, (HANDLE)LoadImageW(hIcons, MAKEINTRESOURCEW(IDI_BELL_48), IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR));
		SetPropW(hwnd, _P_BELL_32, (HANDLE)LoadImageW(hIcons, MAKEINTRESOURCEW(IDI_BELL_32), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR));
		SetPropW(hwnd, _P_BELL_24, (HANDLE)LoadImageW(hIcons, MAKEINTRESOURCEW(IDI_BELL_24), IMAGE_ICON, 24, 24, LR_DEFAULTCOLOR));
		SetPropW(hwnd, _P_BELL_16, (HANDLE)LoadImageW(hIcons, MAKEINTRESOURCEW(IDI_BELL_16), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
		FreeLibrary(hIcons);
	}
	SetPropW(hwnd, _P_BELL_TIMER, (HANDLE)SetTimer(hwnd, 117, 100, BellTimerProc));
	SetLayeredWindowAttributes(hwnd, RGB(255, 0, 255), 255, LWA_COLORKEY);
	return TRUE;
}

static void Bell_OnPaint(HWND hwnd)
{
	PAINTSTRUCT		ps;
	RECT			rc;
	HICON			hIcon = NULL;

	BeginPaint(hwnd, &ps);
	GetClientRect(hwnd, &rc);
	FillRect(ps.hdc, &rc, (HBRUSH)GetPropW(hwnd, _P_BELL_BRUSH));
	switch(m_IconSize){
	case 48:
		hIcon = (HICON)GetPropW(hwnd, _P_BELL_48);
		break;
	case 32:
		hIcon = (HICON)GetPropW(hwnd, _P_BELL_32);
		break;
	case 24:
		hIcon = (HICON)GetPropW(hwnd, _P_BELL_24);
		break;
	case 16:
		hIcon = (HICON)GetPropW(hwnd, _P_BELL_16);
		break;
	}
	DrawIconEx(ps.hdc, ((rc.right - rc.left) - m_IconSize) / 2, ((rc.bottom - rc.top) - m_IconSize) / 2, hIcon, m_IconSize, m_IconSize, 0, NULL, DI_NORMAL);
	EndPaint(hwnd, &ps);
}

static void Bell_OnDestroy(HWND hwnd)
{
	if(GetPropW(hwnd, _P_BELL_TIMER))
		KillTimer(hwnd, (int)GetPropW(hwnd, _P_BELL_TIMER));
	if(GetPropW(hwnd, _P_BELL_48))
		DestroyIcon((HICON)GetPropW(hwnd, _P_BELL_48));
	if(GetPropW(hwnd, _P_BELL_32))
		DestroyIcon((HICON)GetPropW(hwnd, _P_BELL_32));
	if(GetPropW(hwnd, _P_BELL_24))
		DestroyIcon((HICON)GetPropW(hwnd, _P_BELL_24));
	if(GetPropW(hwnd, _P_BELL_16))
		DestroyIcon((HICON)GetPropW(hwnd, _P_BELL_16));
	if(GetPropW(hwnd, _P_BELL_BRUSH))
		DeleteBrush((HBRUSH)GetPropW(hwnd, _P_BELL_BRUSH));
}

static void CALLBACK BellTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){
	switch(m_IconSize){
	case 48:
		m_IconSize = 32;
		break;
	case 32:
		m_IconSize = 24;
		break;
	case 24:
		m_IconSize = 16;
		break;
	case 16:
		m_IconSize = 48;
		break;
	}
	RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
}

static BOOL RegisterBell(HINSTANCE hInstance){
	WNDCLASSEXW		wcx;

	ZeroMemory(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.hInstance = hInstance;
	wcx.lpszClassName = _AG_BELL_WND_CLASS;
	wcx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wcx.lpfnWndProc = Bell_WndProc;
	wcx.hbrBackground = CreateSolidBrush(RGB(255, 0, 255));
	if(RegisterClassExW(&wcx)){
		return TRUE;
	}
	else{
		if(GetLastError() == ERROR_CLASS_ALREADY_EXISTS){
			return TRUE;
		}
		return FALSE;
	}
}
