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

#ifndef __GCOLORS_H__
#define __GCOLORS_H__

#define	GCM_BASE				WM_USER + 112233
#define	GCN_COLOR_SELECTED		GCM_BASE + 1		// notification message sent to parent window when any color is selected
#define	GCM_CLOSE				GCM_BASE + 2
#define	GCM_NEW_COLOR			GCM_BASE + 3

HWND CreateGColorsWindowW(int x, int y, HWND hParentWnd, COLORREF crColor);
void ShowColorsWindow(int x, int y, HWND hColors, COLORREF crColor);

#endif	// GCOLORS_H__
