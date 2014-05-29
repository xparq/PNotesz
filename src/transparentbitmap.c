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

/*-@@+@@------------------------------------------------------------------
 Procedure: DrawTransparentBitmap
 Created  : Sun May 27 15:02:16 2007
 Modified : Sun May 27 15:02:16 2007

 Synopsys : Draws transparent bitmap on specified device context
 Input    : hBmp - bitmap to get the image from
            hdcDest - destination device context
            xDest - x position of image on destination
            yDest - y position of image on destination
            wDest - width of image on destination
            hDest - height of image on destination
            xSrc - x position of image on source
            ySrc - y position of image on source
            crMask - transparent color
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void DrawTransparentBitmap(HBITMAP hBmp, HDC hdcDest, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, COLORREF crMask){

	COLORREF	hColorOld;
	HBITMAP		hBmANDBack, hBmANDObject, hBmANDMem;
	HBITMAP		hBmSave, hBmBackOld, hBmObjectOld, hBmMemOld, hBmSaveOld;
	HDC			hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT		pt;
	BITMAP		bm;

	//create temporary compatible DC
	hdcTemp = CreateCompatibleDC(hdcDest);
	//select the bitmap
	SelectBitmap(hdcTemp, hBmp);
	GetObject(hBmp, sizeof(bm), &bm);
	pt.x = bm.bmWidth;
	pt.y = bm.bmHeight;
	DPtoLP(hdcTemp, &pt, 1);
	//Create some DCs to hold temporary data
	hdcBack = CreateCompatibleDC(hdcDest);
	hdcObject = CreateCompatibleDC(hdcDest);
	hdcMem = CreateCompatibleDC(hdcDest);
	hdcSave = CreateCompatibleDC(hdcDest);
	//create a bitmap for each DC. DCs are required for a number of GDI functions
	//Monochrome DC
	hBmANDBack = CreateBitmap(pt.x, pt.y, 1, 1, NULL);
	//Monochrome DC
	hBmANDObject = CreateBitmap(pt.x, pt.y, 1, 1, NULL);
	//Color DC
	hBmANDMem = CreateCompatibleBitmap(hdcDest, pt.x, pt.y);
	//Color DC
	hBmSave = CreateCompatibleBitmap(hdcDest, pt.x, pt.y);
	//Each DC must select a bitmap object to store pixel data
	hBmBackOld = SelectBitmap(hdcBack, hBmANDBack);
	hBmObjectOld = SelectBitmap(hdcObject, hBmANDObject);
	hBmMemOld = SelectBitmap(hdcMem, hBmANDMem);
	hBmSaveOld = SelectBitmap(hdcSave, hBmSave);
	//Set proper mapping mode
	SetMapMode(hdcTemp, GetMapMode(hdcDest));
	//Save the bitmap sent here, because it will be overwritten
	BitBlt(hdcSave, 0, 0, pt.x, pt.y, hdcTemp, 0, 0, SRCCOPY);
	//Set the background color of the source DC to the color contained in the parts of the bitmap that should be transparent
	hColorOld = SetBkColor(hdcTemp, crMask);
	//Create the object mask for the bitmap by performing a BitBlt from the source bitmap to a monochrome bitmap
	BitBlt(hdcObject, xSrc, ySrc, wDest, hDest, hdcTemp, xSrc, ySrc, SRCCOPY);
	//Set the background color of the source DC back to the original color
	SetBkColor(hdcTemp, hColorOld);
	//Create the inverse of the object mask
	BitBlt(hdcBack, xSrc, ySrc, wDest, hDest, hdcObject, xSrc, ySrc, NOTSRCCOPY);
	//Copy the background of the main DC to the destination
	BitBlt(hdcMem, xSrc, ySrc, wDest, hDest, hdcDest, xDest, yDest, SRCCOPY);
	//Mask out the places where the bitmap will be placed
	BitBlt(hdcMem, xSrc, ySrc, wDest, hDest, hdcObject, xSrc, ySrc, SRCAND);
	//Mask out the transparent colored pixels on the bitmap
	BitBlt(hdcTemp, xSrc, ySrc, wDest, hDest, hdcBack, xSrc, ySrc, SRCAND);
	//XOR the bitmap with the background on the destination DC
	BitBlt(hdcMem, xSrc, ySrc, wDest, hDest, hdcTemp, xSrc, ySrc, SRCPAINT);
	//Copy the destination to the screen
	BitBlt(hdcDest, xDest, yDest, wDest, hDest, hdcMem, xSrc, ySrc, SRCCOPY);
	//Place the original bitmap back into the bitmap sent here
	BitBlt(hdcTemp, 0, 0, pt.x, pt.y, hdcSave, 0, 0, SRCCOPY);
	//Delete the memory bitmaps
	DeleteBitmap(SelectBitmap(hdcBack, hBmBackOld));
	DeleteBitmap(SelectBitmap(hdcObject, hBmObjectOld));
	DeleteBitmap(SelectBitmap(hdcMem, hBmMemOld));
	DeleteBitmap(SelectBitmap(hdcSave, hBmSaveOld));
	//Delete the memory DCs
	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);
}

