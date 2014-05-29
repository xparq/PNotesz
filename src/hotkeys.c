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
#include <wchar.h>

#include "hotkeys.h"

/*-@@+@@------------------------------------------------------------------
 Procedure: GetModifiers
 Created  : Thu Aug  9 18:09:45 2007
 Modified : Thu Aug  9 18:09:45 2007

 Synopsys : Checks whether any of modifier keys (CTRL, ALT, SHIFT, WIN) is  
            presssed
 Input    : lpID - TCHAR to return modifiers
 Output   : Combination of modifiers, if any is pressed, otherwise - 0
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
int GetModifiers(wchar_t * lpID){
	int			result = 0;
	BOOL		useString = TRUE;

	if(NULL != lpID)
		lpID[0] = '\0';
	else
		useString = FALSE;

	if((GetKeyState(VK_CONTROL) & 0x8000) == 0x8000){
		result |= MOD_CONTROL;
		if(useString)
			wcscat(lpID, L"Ctrl+");
	}
	if((GetKeyState(VK_MENU) & 0x8000) == 0x8000){
		result |= MOD_ALT;
		if(useString)
			wcscat(lpID, L"Alt+");
	}
	if((GetKeyState(VK_SHIFT) & 0x8000) == 0x8000){
		result |= MOD_SHIFT;
		if(useString)
			wcscat(lpID, L"Shift+");
	}
	if((GetKeyState(VK_LWIN) & 0x8000) == 0x8000 || (GetKeyState(VK_RWIN) & 0x8000) == 0x8000){
		result |= MOD_WIN;
		if(useString)
			wcscat(lpID, L"Win+");
	}
	return result;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetOtherKey
 Created  : Thu Aug  9 18:11:20 2007
 Modified : Thu Aug  9 18:11:20 2007

 Synopsys : Returns the first of currently pressed keys (not modifiers)
 Input    : lpKey - TCHAR to return the key string representation
 Output   : Virtual key code or 0
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
int GetOtherKey(wchar_t * lpKey){
	BOOL		useString = TRUE;

	if(NULL != lpKey)
		lpKey[0] = '\0';
	else
		useString = FALSE;
	
	if((GetKeyState(VK_A) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"A");
		return VK_A;
	}
	if((GetKeyState(VK_B) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"B");
		return VK_B;
	}
	if((GetKeyState(VK_C) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"C");
		return VK_C;
	}
	if((GetKeyState(VK_D) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"D");
		return VK_D;
	}
	if((GetKeyState(VK_E) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"E");
		return VK_E;
	}
	if((GetKeyState(VK_F) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F");
		return VK_F;
	}
	if((GetKeyState(VK_G) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"G");
		return VK_G;
	}
	if((GetKeyState(VK_H) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"H");
		return VK_H;
	}
	if((GetKeyState(VK_I) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"I");
		return VK_I;
	}
	if((GetKeyState(VK_J) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"J");
		return VK_J;
	}
	if((GetKeyState(VK_K) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"K");
		return VK_K;
	}
	if((GetKeyState(VK_L) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"L");
		return VK_L;
	}
	if((GetKeyState(VK_M) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"M");
		return VK_M;
	}
	if((GetKeyState(VK_N) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"N");
		return VK_N;
	}
	if((GetKeyState(VK_O) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"O");
		return VK_O;
	}
	if((GetKeyState(VK_P) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"P");
		return VK_P;
	}
	if((GetKeyState(VK_Q) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Q");
		return VK_Q;
	}
	if((GetKeyState(VK_R) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"R");
		return VK_R;
	}
	if((GetKeyState(VK_S) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"S");
		return VK_S;
	}
	if((GetKeyState(VK_T) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"T");
		return VK_T;
	}
	if((GetKeyState(VK_U) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"U");
		return VK_U;
	}
	if((GetKeyState(VK_V) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"V");
		return VK_V;
	}
	if((GetKeyState(VK_W) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"W");
		return VK_W;
	}
	if((GetKeyState(VK_X) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"X");
		return VK_X;
	}
	if((GetKeyState(VK_Y) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Y");
		return VK_Y;
	}
	if((GetKeyState(VK_Z) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Z");
		return VK_Z;
	}
	if((GetKeyState(VK_ESCAPE) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Esc");
		return VK_ESCAPE;
	}
	if((GetKeyState(VK_F1) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F1");
		return VK_F1;
	}
	if((GetKeyState(VK_F2) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F2");
		return VK_F2;
	}
	if((GetKeyState(VK_F3) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F3");
		return VK_F3;
	}
	if((GetKeyState(VK_F4) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F4");
		return VK_F4;
	}
	if((GetKeyState(VK_F5) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F5");
		return VK_F5;
	}
	if((GetKeyState(VK_F6) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F6");
		return VK_F6;
	}
	if((GetKeyState(VK_F7) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F7");
		return VK_F7;
	}
	if((GetKeyState(VK_F8) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F8");
		return VK_F8;
	}
	if((GetKeyState(VK_F9) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F9");
		return VK_F9;
	}
	if((GetKeyState(VK_F10) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F10");
		return VK_F10;
	}
	if((GetKeyState(VK_F11) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F11");
		return VK_F11;
	}
	if((GetKeyState(VK_F12) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"F12");
		return VK_F12;
	}
	if((GetKeyState(VK_BACK) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Backspace");
		return VK_BACK;
	}
	if((GetKeyState(VK_INSERT) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Ins");
		return VK_INSERT;
	}
	if((GetKeyState(VK_HOME) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Home");
		return VK_HOME;
	}
	if((GetKeyState(VK_PRIOR) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"PgUp");
		return VK_PRIOR;
	}
	if((GetKeyState(VK_NEXT) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"PgDn");
		return VK_NEXT;
	}
	if((GetKeyState(VK_END) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"End");
		return VK_END;
	}
	if((GetKeyState(VK_DELETE) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Del");
		return VK_DELETE;
	}
	if((GetKeyState(VK_SPACE) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Space");
		return VK_SPACE;
	}
	if((GetKeyState(VK_UP) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Up");
		return VK_UP;
	}
	if((GetKeyState(VK_DOWN) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Down");
		return VK_DOWN;
	}
	if((GetKeyState(VK_LEFT) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Left");
		return VK_LEFT;
	}
	if((GetKeyState(VK_RIGHT) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Right");
		return VK_RIGHT;
	}
	if((GetKeyState(VK_0) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"0");
		return VK_0;
	}
	if((GetKeyState(VK_1) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"1");
		return VK_1;
	}
	if((GetKeyState(VK_2) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"2");
		return VK_2;
	}
	if((GetKeyState(VK_3) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"3");
		return VK_3;
	}
	if((GetKeyState(VK_4) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"4");
		return VK_4;
	}
	if((GetKeyState(VK_5) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"5");
		return VK_5;
	}
	if((GetKeyState(VK_6) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"6");
		return VK_6;
	}
	if((GetKeyState(VK_7) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"7");
		return VK_7;
	}
	if((GetKeyState(VK_8) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"8");
		return VK_8;
	}
	if((GetKeyState(VK_9) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"9");
		return VK_9;
	}
	if((GetKeyState(VK_NUMPAD0) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Num 0");
		return VK_NUMPAD0;
	}
	if((GetKeyState(VK_NUMPAD1) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Num 1");
		return VK_NUMPAD1;
	}
	if((GetKeyState(VK_NUMPAD2) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Num 2");
		return VK_NUMPAD2;
	}
	if((GetKeyState(VK_NUMPAD3) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Num 3");
		return VK_NUMPAD3;
	}
	if((GetKeyState(VK_NUMPAD4) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Num 4");
		return VK_NUMPAD4;
	}
	if((GetKeyState(VK_NUMPAD5) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Num 5");
		return VK_NUMPAD5;
	}
	if((GetKeyState(VK_NUMPAD6) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Num 6");
		return VK_NUMPAD6;
	}
	if((GetKeyState(VK_NUMPAD7) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Num 7");
		return VK_NUMPAD7;
	}
	if((GetKeyState(VK_NUMPAD8) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Num 8");
		return VK_NUMPAD8;
	}
	if((GetKeyState(VK_NUMPAD9) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Num 9");
		return VK_NUMPAD9;
	}
	if((GetKeyState(VK_PAUSE) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Pause");
		return VK_PAUSE;
	}
	if((GetKeyState(VK_ADD) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"+");
		return VK_ADD;
	}
	if((GetKeyState(VK_SUBTRACT) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"-");
		return VK_SUBTRACT;
	}
	if((GetKeyState(VK_MULTIPLY) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"*");
		return VK_MULTIPLY;
	}
	if((GetKeyState(VK_DIVIDE) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"/");
		return VK_DIVIDE;
	}
	if((GetKeyState(VK_RETURN) & 0x8000) == 0x8000){
		if(useString)
			wcscpy(lpKey, L"Enter");
		return VK_RETURN;
	}
	return FALSE;
}

