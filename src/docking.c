// Copyright (C) 2008 Andrey Gruber (aka lamer)

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
#include <stdlib.h>

#include "docking.h"
#include "enums.h"

static int SortListByIndex(PDOCKHEADER pHeader);

PDOCKITEM AddDockItem(PDOCKHEADER pHeader, HWND hwnd, int index){
	PDOCKITEM		pItem, pTemp;

	pItem = (PDOCKITEM)calloc(1, sizeof(DOCKITEM));
	pItem->hwnd = hwnd;
	if(pHeader->count == 0){
		pHeader->pNext = pItem;
	}
	else{
		pTemp = pHeader->pNext;
		while(pTemp->pNext){
			pTemp = pTemp->pNext;
		}
		pTemp->pNext = pItem;
		pItem->pPrev = pTemp;
	}
	pHeader->count++;
	if(index == -1){
		pItem->index = LastIndex(pHeader);
	}
	else{
		pItem->index = index;
	}
	return pItem;
}

short LastIndex(PDOCKHEADER pHeader){
	short			index = -1;
	PDOCKITEM		pTemp;

	pTemp = pHeader->pNext;
	while(pTemp->pNext){
		index = pTemp->index;
		pTemp = pTemp->pNext;
	}

	return ++index;
}

void FreeDockList(PDOCKHEADER pHeader){
	PDOCKITEM		pTemp;

	if(pHeader){
		while(pHeader->pNext){
			pTemp = pHeader->pNext;
			pHeader->pNext = pTemp->pNext;
			free(pTemp);
		}
		free(pHeader);
	}
}

void RemoveDockItem(PDOCKHEADER pHeader, HWND hwnd){
	PDOCKITEM		pItem, pTemp;

	if(pHeader->count > 0){
		pItem = pHeader->pNext;
		while(pItem->hwnd != hwnd && pItem->pNext){
			pItem = pItem->pNext;
		}
		if(pItem->hwnd == hwnd){
			if(pItem->pPrev == NULL){	//the first item
				pTemp = pItem->pNext;
				pHeader->pNext = pTemp;
				if(pTemp)
					pTemp->pPrev = NULL;
				free(pItem);
				//shift indexes
				while(pTemp){
					pTemp->index--;
					pTemp = pTemp->pNext;
				}
			}
			else if(pItem->pNext == NULL){	//the last item
				pItem->pPrev->pNext = NULL;
				free(pItem);
			}
			else{	//the item is in the middle of list
				pTemp = pItem->pNext;
				pItem->pPrev->pNext = pTemp;
				pTemp->pPrev = pItem->pPrev;
				free(pItem);
				//shift indexes
				while(pTemp){
					pTemp->index--;
					pTemp = pTemp->pNext;
				}
			}
			pHeader->count--;
		}
	}
}

PDOCKITEM NextDockItem(PDOCKHEADER pHeader, HWND hwnd){
	PDOCKITEM		pTemp;

	if(pHeader->count >0){
		pTemp = pHeader->pNext;
		while(pTemp->pNext){
			if(pTemp->hwnd == hwnd){
				return pTemp->pNext;
			}
			pTemp = pTemp->pNext;
		}
		return NULL;
	}
	return NULL;
}

void SortDockList(PDOCKHEADER pHeader){
	while(SortListByIndex(pHeader))
		;
}

static int SortListByIndex(PDOCKHEADER pHeader){
	int			result = 0;
	PDOCKITEM	p1, p2;

	if(pHeader->count < 2)
		return 0;

	p1 = pHeader->pNext;
	p2 = p1->pNext;
	if(p1->index > p2->index){
		pHeader->pNext = p2;
		p2->pPrev = p1->pPrev;
		p1->pPrev = p2;
		p1->pNext = p2->pNext;
		p2->pNext = p1;
		if(p1->pNext){
			p1->pNext->pPrev = p1;
		}
		//move forward
		p2 = p1->pNext;
		result = 1;
	}
	else{
		p1 = p2;
		p2 = p2->pNext;
	}
	while(p2){
		if(p1->index > p2->index){
			p1->pPrev->pNext = p2;
			p2->pPrev = p1->pPrev;
			p1->pNext = p2->pNext;
			p2->pNext = p1;
			p1->pPrev = p2;
			if(p1->pNext){
				p1->pNext->pPrev = p1;
			}
			//move forward
			p2 = p1->pNext;
			result = 1;
		}
		else{
			p1 = p2;
			p2 = p2->pNext;
		}
	}
	return result;
}


PDOCKHEADER DHeader(int dockType){
	switch(dockType){
		case DOCK_LEFT:
			return g_DHeaders.pLeft;
		case DOCK_TOP:
			return g_DHeaders.pTop;
		case DOCK_RIGHT:
			return g_DHeaders.pRight;
		case DOCK_BOTTOM:
			return g_DHeaders.pBottom;
		default:
			return NULL;
	}
}
