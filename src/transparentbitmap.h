#ifndef __TRANSPARENTBITMAP_H__
#define __TRANSPARENTBITMAP_H__

void DrawTransparentBitmap(HBITMAP hBmp, HDC hdcDest, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, COLORREF crMask);

#endif	// TRANSPARENTBITMAP_H__
