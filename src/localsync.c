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


#include "localsync.h"

LPPLOCALSYNC PLocalSyncAdd(LPPLOCALSYNC * this, wchar_t * lpName, wchar_t * lpPathNotes, wchar_t * lpPathId){
	LPPLOCALSYNC		pNew = calloc(1, sizeof(PLOCALSYNC));
	LPPLOCALSYNC		pNext;

	if(pNew){
		pNew->next = NULL;
		pNew->prev = NULL;
		wcscpy(pNew->name, lpName);
		wcscpy(pNew->pathNotes, lpPathNotes);
		wcscpy(pNew->pathId, lpPathId);
		if(!*this){
			*this = pNew;
		}
		else{
			pNext = *this;
			while(pNext->next){
				pNext = pNext->next;
			}
			pNext->next = pNew;
			pNew->prev = pNext;
		}
	}
	return *this;
}

void PLocalSyncFree(LPPLOCALSYNC * this){
	LPPLOCALSYNC	pNext, pTemp;

	pNext = *this;
	while(pNext){
		pTemp = pNext;
		pNext = pNext->next;
		free(pTemp);
	}
	*this = NULL;
}

LPPLOCALSYNC PLocalSyncCopy(LPPLOCALSYNC * tDest, LPPLOCALSYNC tSrc){
	LPPLOCALSYNC	pNext;

	if(*tDest)
		PLocalSyncFree(tDest);
	*tDest = NULL;
	pNext = tSrc;
	while(pNext){
		*tDest = PLocalSyncAdd(tDest, pNext->name, pNext->pathNotes, pNext->pathId);
		pNext = pNext->next;
	}
	return *tDest;
}

BOOL PLocalSyncContains(LPPLOCALSYNC this, wchar_t * lpName){
	for(LPPLOCALSYNC pTemp = this; pTemp; pTemp = pTemp->next){
		if(_wcsicmp(pTemp->name, lpName) == 0){
			return TRUE;
		}
	}
	return FALSE;
}

int PLocalSyncCount(LPPLOCALSYNC this){
	int			count = 0;

	for(LPPLOCALSYNC pNext = this; pNext; pNext = pNext->next)
		count++;
	return count;
}

BOOL PLocalSyncDifference(LPPLOCALSYNC pc1, LPPLOCALSYNC pc2){
	int				c1, c2;
	LPPLOCALSYNC	pN1 = pc1, pN2 = pc2;

	c1 = PLocalSyncCount(pc1);
	c2 = PLocalSyncCount(pc2);

	if(c1 != c2){
		return TRUE;
	}
	else{
		while(pN1){
			if(_wcsicmp(pN1->name, pN2->name) != 0 || _wcsicmp(pN1->pathNotes, pN2->pathNotes) != 0  || _wcsicmp(pN1->pathId, pN2->pathId) != 0){
				return TRUE;
			}
			pN1 = pN1->next;
			pN2 = pN2->next;
		}
	}
	return FALSE;
}

LPPLOCALSYNC PLocalSyncRemove(LPPLOCALSYNC * this, wchar_t * lpName){
	LPPLOCALSYNC	pNext = *this, pTemp;

	while(pNext){
		if(_wcsicmp(pNext->name, lpName) == 0){
			pTemp = pNext;
			if(pNext->prev == NULL){
				//very first contacts group, i.e. "this"
				if(pNext->next){
					pNext->next->prev = NULL;
					*this = pNext->next;
				}
				else{
					*this = NULL;
				}
			}
			else if(pNext->next == NULL){
				//very last contacts group
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
	return *this;
}

LPPLOCALSYNC PLocalSyncItem(LPPLOCALSYNC this, wchar_t * lpName){
	for(LPPLOCALSYNC pNext = this; pNext; pNext = pNext->next){
		if(_wcsicmp(pNext->name, lpName) == 0){
			return pNext;
		}
	}
	return NULL;
}
