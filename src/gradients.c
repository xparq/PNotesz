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
#include <shlwapi.h>
#include "gradients.h"

static int cblue(int arg);
static int cgreen(int arg);
static int cred(int arg);
static int calpha(int arg);
static int ccolorhb(int arg);

void Fill2ColorsRectangle(HDC hdc, LPRECT lprc, COLORREF clr1, COLORREF clr2, ULONG mode){
	GRADIENT_RECT		grc;
	TRIVERTEX			tvx[2];

	//set first trivertex
	tvx[0].x = lprc->left;
	tvx[0].y = lprc->top;
	//set first trivertex colors
	tvx[0].Red = cred(clr1);
	tvx[0].Green = cgreen(clr1);
	tvx[0].Blue = cblue(clr1);
	tvx[0].Alpha = calpha(clr1);
	//set second trivertex
	tvx[1].x = lprc->right;
	tvx[1].y = lprc->bottom;
	//set second trivertex colors
	tvx[1].Red = cred(clr2);
	tvx[1].Green = cgreen(clr2);
	tvx[1].Blue = cblue(clr2);
	tvx[1].Alpha = calpha(clr2);
	//set graphic rectangle
	grc.UpperLeft = 0;
	grc.LowerRight = 1;
	//fill with gradient
	GradientFill(hdc, tvx, 2, &grc, 1, mode);
}


void DrawRainbow(HDC hdc, LPRECT lprc){
	RECT				rc;
	int					w = (lprc->right - lprc->left);
	int 				h = (lprc->bottom - lprc->top);

	SetRect(&rc, 0, 0, w / 7, 1);
	Fill2ColorsRectangle(hdc, &rc, RGB(255, 0, 0), RGB(255, 128, 0), GRADIENT_FILL_RECT_H);
	SetRect(&rc, w / 7, 0, w / 7 * 2, 1);
	Fill2ColorsRectangle(hdc, &rc, RGB(255, 128, 0), RGB(255, 255, 0), GRADIENT_FILL_RECT_H);
	SetRect(&rc, w / 7 * 2, 0, w / 7 * 3, 1);
	Fill2ColorsRectangle(hdc, &rc, RGB(255, 255, 0), RGB(0, 255, 0), GRADIENT_FILL_RECT_H);
	SetRect(&rc, w / 7 * 3, 0, w / 7 * 4, 1);
	Fill2ColorsRectangle(hdc, &rc, RGB(0, 255, 0), RGB(0, 255, 255), GRADIENT_FILL_RECT_H);
	SetRect(&rc, w / 7 * 4, 0, w / 7 * 5, 1);
	Fill2ColorsRectangle(hdc, &rc, RGB(0, 255, 255), RGB(0, 0, 255), GRADIENT_FILL_RECT_H);
	SetRect(&rc, w / 7 * 5, 0, w / 7 * 6, 1);
	Fill2ColorsRectangle(hdc, &rc, RGB(0, 0, 255), RGB(255, 0, 255), GRADIENT_FILL_RECT_H);
	SetRect(&rc, w / 7 * 6, 0, w, 1);
	Fill2ColorsRectangle(hdc, &rc, RGB(255, 0, 255), RGB(255, 0, 26), GRADIENT_FILL_RECT_H);

	SetRect(&rc, 0, h - 1, w / 7, h);
	Fill2ColorsRectangle(hdc, &rc, RGB(132, 123, 123), RGB(132, 128, 123), GRADIENT_FILL_RECT_H);
	SetRect(&rc, w / 7, h - 1, w / 7 * 2, h);
	Fill2ColorsRectangle(hdc, &rc, RGB(132, 128, 123), RGB(132, 132, 123), GRADIENT_FILL_RECT_H);
	SetRect(&rc, w / 7 * 2, h - 1, w / 7 * 3, h);
	Fill2ColorsRectangle(hdc, &rc, RGB(132, 132, 123), RGB(123, 132, 123), GRADIENT_FILL_RECT_H);
	SetRect(&rc, w / 7 * 3, h - 1, w / 7 * 4, h);
	Fill2ColorsRectangle(hdc, &rc, RGB(123, 132, 123), RGB(123, 132, 132), GRADIENT_FILL_RECT_H);
	SetRect(&rc, w / 7 * 4, h - 1, w / 7 * 5, h);
	Fill2ColorsRectangle(hdc, &rc, RGB(123, 132, 132), RGB(123, 123, 132), GRADIENT_FILL_RECT_H);
	SetRect(&rc, w / 7 * 5, h - 1, w / 7 * 6, h);
	Fill2ColorsRectangle(hdc, &rc, RGB(123, 123, 132), RGB(132, 123, 132), GRADIENT_FILL_RECT_H);
	SetRect(&rc, w / 7 * 6, h - 1, w, h);
	Fill2ColorsRectangle(hdc, &rc, RGB(132, 123, 132), RGB(132, 123, 124), GRADIENT_FILL_RECT_H);

	for(int i = 0; i < w; i++){
		COLORREF cr1 = GetPixel(hdc, i, 0);
		COLORREF cr2 = GetPixel(hdc, i, h - 1);
		SetRect(&rc, i, 0, i + 1, h);
		Fill2ColorsRectangle(hdc, &rc, cr1, cr2, GRADIENT_FILL_RECT_V);
	}
}

static int calpha(int arg){
	int		alpha;
	__asm{
		xor edx,edx
		mov ecx,1000000h
		mov eax,arg
		div ecx
		push eax
		call ccolorhb
		mov alpha, eax
	}
	return alpha;
}

static int cblue(int arg){
	int		blue;
	__asm{
		mov eax,arg
		shr eax,16
		push eax
		call ccolorhb
		mov blue, eax
	}
	return blue;
}

static int cgreen(int arg){
	int		green;
	__asm{
		mov eax,arg
		shr eax,8
		push eax
		call ccolorhb
		mov green, eax
	}
	return green;
}

static int cred(int arg){
	int		red;
	__asm{
		push arg
		call ccolorhb
		mov red, eax
	}
	return red;
}

static int ccolorhb(int arg){
	int		hb;
	__asm{
		mov eax,arg
		and eax,0xff
		mov ecx,eax
		and eax,7Fh
		shl eax,8
		and ecx,80h
		jz _exit
		xor eax,0x7fff
		not eax
		_exit:
		mov hb, eax
	}
	return hb;
}


