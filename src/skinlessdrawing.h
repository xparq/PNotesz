#ifndef __SKINLESSDRAWING_H__
#define __SKINLESSDRAWING_H__

#include "structures.h"

void CreateSkinlessToolbarBitmap(HWND hwnd, P_NOTE_APPEARANCE pA, P_NOTE_RTHANDLES pH, HBITMAP * phbmp);
void CreateSkinlessSysBitmap(HWND hwnd, P_NOTE_APPEARANCE pA, P_NOTE_RTHANDLES pH, HBITMAP * phbmp);
void CreateSimpleMarksBitmap(HWND hwnd, HBITMAP * phbmp, int nMarksCount);

#endif	// SKINLESSDRAWING_H__
