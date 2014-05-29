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
#include <commctrl.h>

void CreateImageLists(HIMAGELIST *lpImlNormal, HIMAGELIST *lpImlGray, HINSTANCE hInst, int idBmpNormal, int idBmpGray, COLORREF crMask, int x, int y, int imgCount){
	HBITMAP			hBmp;

	*lpImlNormal = ImageList_Create(x, y, ILC_MASK | ILC_COLOR24, imgCount, 0);
	hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(idBmpNormal));
	ImageList_AddMasked(*lpImlNormal, hBmp, crMask);
	DeleteBitmap(hBmp);
	DeleteObject((void*)crMask);
	*lpImlGray = ImageList_Create(x, y, ILC_MASK | ILC_COLOR24, imgCount, 0);
	hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(idBmpGray));
	ImageList_AddMasked(*lpImlGray, hBmp, crMask);
	DeleteBitmap(hBmp);
	DeleteObject((void*)crMask);
}

void CreateImageListsW(HIMAGELIST *lpImlNormal, HIMAGELIST *lpImlGray, HINSTANCE hInst, int idBmpNormal, int idBmpGray, COLORREF crMask, int x, int y, int imgCount){
	HBITMAP			hBmp;

	*lpImlNormal = ImageList_Create(x, y, ILC_MASK | ILC_COLOR24, imgCount, 0);
	hBmp = LoadBitmapW(hInst, MAKEINTRESOURCEW(idBmpNormal));
	ImageList_AddMasked(*lpImlNormal, hBmp, crMask);
	DeleteBitmap(hBmp);
	DeleteObject((void*)crMask);
	*lpImlGray = ImageList_Create(x, y, ILC_MASK | ILC_COLOR24, imgCount, 0);
	hBmp = LoadBitmapW(hInst, MAKEINTRESOURCEW(idBmpGray));
	ImageList_AddMasked(*lpImlGray, hBmp, crMask);
	DeleteBitmap(hBmp);
	DeleteObject((void*)crMask);
}

void DoToolbar(HWND toolbar, HIMAGELIST imlNormal, HIMAGELIST imlGray, UINT btnsCount, LPTBBUTTON lpButtons){
	SendMessage(toolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
	SendMessage(toolbar, TB_SETIMAGELIST, 0, (LPARAM)imlNormal);
	SendMessage(toolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)imlGray);
	SendMessage(toolbar, TB_ADDBUTTONS, btnsCount, (LPARAM)lpButtons);
}

void DoToolbarW(HWND toolbar, HIMAGELIST imlNormal, HIMAGELIST imlGray, UINT btnsCount, LPTBBUTTON lpButtons){
	SendMessageW(toolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
	SendMessageW(toolbar, TB_SETIMAGELIST, 0, (LPARAM)imlNormal);
	SendMessageW(toolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)imlGray);
	SendMessageW(toolbar, TB_ADDBUTTONSW, btnsCount, (LPARAM)lpButtons);
}

int GetButtonIndex(HWND toolbar, int id){
	TBBUTTON	tbb;
	int			count = SendMessage(toolbar, TB_BUTTONCOUNT, 0, 0);

	for(int i = 0; i < count; i++){
		SendMessage(toolbar, TB_GETBUTTON, i, (LPARAM)&tbb);
		if(tbb.idCommand == id){
			return i;
		}
	}
	return -1;
}

int GetButtonIndexW(HWND toolbar, int id){
	TBBUTTON	tbb;
	int			count = SendMessageW(toolbar, TB_BUTTONCOUNT, 0, 0);

	for(int i = 0; i < count; i++){
		SendMessageW(toolbar, TB_GETBUTTON, i, (LPARAM)&tbb);
		if(tbb.idCommand == id){
			return i;
		}
	}
	return -1;
}
