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

#include "gllist.h"

static PGLSTRUCT GetByHWND(HWND hwnd);

static PGLSTRUCT	pList = NULL;		// always point to beginning of collection

PGLSTRUCT AddNewGLStruct(HWND hwnd, WNDPROC hproc){
	PGLSTRUCT		pTemp, pLast;

	if(pList == NULL){							// very first member is added
		pTemp = calloc(1, sizeof(GLSTRUCT));
		if(pTemp == NULL)
			return NULL;
		pTemp->hwnd = hwnd;
		pTemp->hproc = hproc;
		pList = pTemp;
	}
	else{
		pLast = GetLastGLStruct();				// get the last member
		pTemp = calloc(1, sizeof(GLSTRUCT));
		if(pTemp == NULL)
			return NULL;
		pTemp->hwnd = hwnd;
		pTemp->hproc = hproc;
		pTemp->pPrev = pLast;
		pLast->pNext = pTemp;
	}
	return pTemp;
}

PGLSTRUCT GetLastGLStruct(void){
	PGLSTRUCT		pTemp;

	if(pList == NULL){
		return NULL;
	}
	else{
		pTemp = pList;
		while(pTemp->pNext != NULL)
			pTemp = pTemp->pNext;
		return pTemp;
	}
}

WNDPROC GetGLProc(HWND hwnd){
	PGLSTRUCT		pTemp;

	if(pList == NULL){
		return NULL;
	}
	else{
		pTemp = pList;
		do{
			if(pTemp->hwnd == hwnd)
				return (WNDPROC)pTemp->hproc;
			pTemp = pTemp->pNext;
		}while(pTemp != NULL);
		return NULL;
	}
}

BOOL GLStructExists(HWND hwnd){
	PGLSTRUCT		pTemp;
	if(pList == NULL){
		return FALSE;
	}
	else{
		pTemp = pList;
		do{
			if(pTemp->hwnd == hwnd)
				return TRUE;
			pTemp = pTemp->pNext;
		}while(pTemp != NULL);
		return FALSE;
	}
}

void DeleteGLStruct(HWND hwnd){
	PGLSTRUCT		pTemp;
	if(pList != NULL){
		pTemp = GetByHWND(hwnd);
		if(pTemp->pPrev == NULL){		//the first member
			if(pTemp->pNext == NULL){	//the single member, just free memory and reset pointer
				free(pTemp);
				pList = NULL;
			}
			else{						//shift next member left
				pList = pTemp->pNext;
				pList->pPrev = NULL;
				free(pTemp);
			}
		}
		else if(pTemp->pNext == NULL){	//the last member
			PGLSTRUCT pT = pTemp->pPrev;
			pT->pNext = NULL;
			free(pTemp);
		}
		else{							//member in the middle of list
			PGLSTRUCT pTP = pTemp->pPrev;
			PGLSTRUCT pTN = pTemp->pNext;
			pTP->pNext = pTN;
			pTN->pPrev = pTP;
			free(pTemp);
		}
	}
}

void IncGListCount(HWND hwnd){
	PGLSTRUCT		pTemp = GetByHWND(hwnd);

	if(pTemp != NULL)
		pTemp->count++;
}

void DecGListCount(HWND hwnd){
	PGLSTRUCT		pTemp = GetByHWND(hwnd);

	if(pTemp != NULL)
		pTemp->count--;
}

int GetGListCount(HWND hwnd){
	PGLSTRUCT		pTemp = GetByHWND(hwnd);

	if(pTemp != NULL)
		return pTemp->count;
	else
		return 0;
}

static PGLSTRUCT GetByHWND(HWND hwnd){
	PGLSTRUCT		pTemp;
	
	if(pList == NULL){
		return NULL;
	}
	else{
		pTemp = pList;
		do{
			if(pTemp->hwnd == hwnd)
				return pTemp;
			pTemp = pTemp->pNext;
		}while(pTemp != NULL);
		return NULL;
	}
}



