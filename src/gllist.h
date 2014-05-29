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

#ifndef __GLLIST_H__
#define __GLLIST_H__

#include <windows.h>

typedef struct _GLSTRUCT *PGLSTRUCT;

typedef struct _GLSTRUCT {
	PGLSTRUCT		pPrev;		// pointer to previous member
	HWND			hwnd;		// window handle
	WNDPROC			hproc;		// window procedure address
	int				count;		// count of glist windows belong to current window
	PGLSTRUCT		pNext;		// pointer to next member
}GLSTRUCT; 

PGLSTRUCT AddNewGLStruct(HWND hwnd, WNDPROC hproc);
PGLSTRUCT GetLastGLStruct(void);
WNDPROC GetGLProc(HWND hwnd);
BOOL GLStructExists(HWND hwnd);
void DeleteGLStruct(HWND hwnd);
void IncGListCount(HWND hwnd);
void DecGListCount(HWND hwnd);
int GetGListCount(HWND hwnd);

#endif	// GLLIST_H__
