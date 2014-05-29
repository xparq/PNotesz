// Copyright (C) 2010 Andrey Gruber (aka lamer)

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

#include <wchar.h>

#include "plinks.h"

LPPLINK PLinksAdd(LPPLINK this, wchar_t * lpID){
	LPPLINK		pLink = calloc(1, sizeof(PLINK));
	LPPLINK		pNext;

	if(pLink){
		wcscpy(pLink->id, lpID);
		if(!this){
			this = pLink;
		}
		else{
			pNext = this;
			while(pNext->next){
				pNext = pNext->next;
			}
			pNext->next = pLink;
			pLink->prev = pNext;
		}
	}
	return this;
}

BOOL PLinksContains(LPPLINK this, wchar_t * lpID){
	for(LPPLINK	pNext = this; pNext; pNext = pNext->next){
		if(_wcsicmp(pNext->id, lpID) == 0){
			return TRUE;
		}
	}
	return FALSE;
}

void PLinksFree(LPPLINK this){
	LPPLINK	pNext, pTemp;

	pNext = this;
	while(pNext){
		pTemp = pNext;
		pNext = pNext->next;
		free(pTemp);
	}
}

LPPLINK PLinksRemove(LPPLINK this, wchar_t * lpID){
	LPPLINK	pNext = this, pTemp;

	while(pNext){
		if(_wcsicmp(pNext->id, lpID) == 0){
			pTemp = pNext;
			if(pNext->prev == NULL){
				//very first link, i.e. "this"
				if(pNext->next){
					pNext->next->prev = NULL;
					this = pNext->next;
				}
				else{
					this = NULL;
				}
			}
			else if(pNext->next == NULL){
				//very last link
				pNext->prev->next = NULL;
			}
			else{
				pNext->prev->next = pNext->next;
				pNext->next->prev = pNext->prev;
			}
			free(pTemp);
			break;
		}
		pNext = pNext->next;
	}
	return this;
}

LPPLINK PLinksCopy(LPPLINK tDest, LPPLINK tSrc){
	LPPLINK	pNext;

	if(tDest)
		PLinksFree(tDest);
	tDest = NULL;
	pNext = tSrc;
	while(pNext){
		tDest = PLinksAdd(tDest, pNext->id);
		pNext = pNext->next;
	}
	return tDest;
}

int PLinksCount(LPPLINK this){
	int		count = 0;

	for(LPPLINK	pNext = this; pNext; pNext = pNext->next)
		count++;
	return count;
}
