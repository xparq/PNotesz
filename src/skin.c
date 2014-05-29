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

#include <windowsx.h>

#include "skin.h"

HRGN RegionFromSkin(HBITMAP hBmp, COLORREF crMask){
	
	HRGN		hRgn = CreateRectRgn(0, 0, 0, 0);
	int			x = 0;
	BITMAP		bmp;
	HDC			hdcMem;
	HBITMAP		hbOld;

	hdcMem = CreateCompatibleDC(GetDC(0));
	hbOld = SelectBitmap(hdcMem, hBmp);
	GetObject(hBmp, sizeof(bmp), &bmp);
	for(int y = 0; y < bmp.bmHeight; y++){
		do{
			while(x < bmp.bmWidth && GetPixel(hdcMem, x, y) == crMask)
				x++;
			int leftX = x;
			while(x < bmp.bmWidth && GetPixel(hdcMem, x, y) != crMask)
				++x;
			HRGN rgnTemp = CreateRectRgn(leftX, y, x, y + 1);
			CombineRgn(hRgn, hRgn, rgnTemp, RGN_OR);
			DeleteRgn(rgnTemp);
		}while(x < bmp.bmWidth);
		x = 0;
	}
	SelectBitmap(hdcMem, hbOld);
	DeleteDC(hdcMem);
	return hRgn;
}

HRGN RegionFromSkinWithOffset(HBITMAP hBmp, COLORREF crMask, int offset, int bWidth){
	
	HRGN		hRgn = CreateRectRgn(0, 0, 0, 0);
	int			x = offset;
	BITMAP		bmp;
	HDC			hdcMem;
	HBITMAP		hbOld;

	bWidth += offset;
	hdcMem = CreateCompatibleDC(GetDC(0));
	hbOld = SelectBitmap(hdcMem, hBmp);
	GetObject(hBmp, sizeof(bmp), &bmp);
	for(int y = 0; y < bmp.bmHeight; y++){
		do{
			while(x < bWidth && GetPixel(hdcMem, x, y) == crMask)
				x++;
			int leftX = x;
			while(x < bWidth && GetPixel(hdcMem, x, y) != crMask)
				++x;
			HRGN rgnTemp = CreateRectRgn(leftX, y, x, y + 1);
			CombineRgn(hRgn, hRgn, rgnTemp, RGN_OR);
			DeleteRgn(rgnTemp);
		}while(x < bWidth);
		x = offset;
	}
	SelectBitmap(hdcMem, hbOld);
	DeleteDC(hdcMem);
	return hRgn;
}
