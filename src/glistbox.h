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

#ifndef __GLISTBOX_H__
#define __GLISTBOX_H__

#define	GLIST_I_COLOR		0
#define	GLIST_I_BITMAP		1
#define	GLIST_I_ICON		2

#define	GLM_BASE			WM_USER
#define	GLM_ADDENTRY		GLM_BASE + 1

// macros
#define GList_AddEntryW(hList, pi, lpString) \
	SendMessage((hList), GLM_ADDENTRY, (WPARAM)(PGLIST_IMAGE)(pi), (LPARAM)(LPCWSTR)(lpString))

typedef struct {
	int			nType;			// structure type, should be one of predefined constants
	int			dwValue;		// color value, icon handle or bitmap handle
	SIZE		szImage;		// size of image/color rectangle
	int			xSrc;			// x coordinate of source bitmap when nType is GLIST_I_BITMAP, otherwise ignored
	int			ySrc;			// y coordinate of source bitmap when nType is GLIST_I_BITMAP, otherwise ignored
	int			itemHeight;		// custom listbox item height
	BOOL		frame;			// specifies whether black frame should be drawn arround image/color rectangle
	COLORREF	crBack;			// back color of listbox item. If 0, the default color will be used
	COLORREF	crText;			// text color of listbox item. If 0, the default color will be used
	COLORREF	crSelectedBack;	// selected back color of listbox item. If 0, the default color will be used
	COLORREF	crSelectedText;	// selected text color of listbox item. If 0, the default color will be used
	COLORREF	crMask;			// mask color when nType is GLIST_I_BITMAP, otherwise ignored
	DWORD		reserved;		// reserved
}GLIST_IMAGE, *PGLIST_IMAGE;

HWND CreateGListWindowW(DWORD dwExStyle, DWORD dwStyle, int x, int y, int w, int h, HWND hParent);

#endif	// GLISTBOX_H__
