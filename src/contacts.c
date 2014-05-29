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

#include "contacts.h"

LPPCONTACT PContactsAdd(LPPCONTACT this, LPPCONTPROP pc){
	LPPCONTACT		pNew = calloc(1, sizeof(PCONTACT));
	LPPCONTACT		pNext;

	if(pNew){
		memcpy(&pNew->prop, pc, sizeof(PCONTPROP));
		if(!this){
			this = pNew;
		}
		else{
			pNext = this;
			while(pNext->next){
				pNext = pNext->next;
			}
			pNext->next = pNew;
			pNew->prev = pNext;
		}
	}
	return this;
}

BOOL PContactsContains(LPPCONTACT this, wchar_t * lpName){
	for(LPPCONTACT	pNext = this; pNext; pNext = pNext->next){
		if(_wcsicmp(pNext->prop.name, lpName) == 0){
			return TRUE;
		}
	}
	return FALSE;
}

LPPCONTACT PContactsItem(LPPCONTACT this, wchar_t * lpName){
	for(LPPCONTACT	pNext = this; pNext; pNext = pNext->next){
		if(_wcsicmp(pNext->prop.name, lpName) == 0){
			return pNext;
		}
	}
	return NULL;
}

wchar_t * PContactsNameByHost(LPPCONTACT this, wchar_t * lpHost){
	for(LPPCONTACT	pNext = this; pNext; pNext = pNext->next){
		if(_wcsicmp(pNext->prop.host, lpHost) == 0){
			return pNext->prop.name;
		}
	}
	return NULL;
}

void PContactsFree(LPPCONTACT this){
	LPPCONTACT	pNext, pTemp;

	pNext = this;
	while(pNext){
		pTemp = pNext;
		pNext = pNext->next;
		free(pTemp);
	}
}

LPPCONTACT PContactsRemove(LPPCONTACT this, wchar_t * lpName){
	LPPCONTACT	pNext = this, pTemp;

	while(pNext){
		if(_wcsicmp(pNext->prop.name, lpName) == 0){
			pTemp = pNext;
			if(pNext->prev == NULL){
				//very first contact, i.e. "this"
				if(pNext->next){
					pNext->next->prev = NULL;
					this = pNext->next;
				}
				else{
					this = NULL;
				}
			}
			else if(pNext->next == NULL){
				//very last contact
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

LPPCONTACT PContactsCopy(LPPCONTACT tDest, LPPCONTACT tSrc){
	LPPCONTACT	pNext;

	if(tDest)
		PContactsFree(tDest);
	tDest = NULL;
	pNext = tSrc;
	while(pNext){
		tDest = PContactsAdd(tDest, &pNext->prop);
		pNext = pNext->next;
	}
	return tDest;
}

int PContactsCount(LPPCONTACT this){
	int			count = 0;

	for(LPPCONTACT	pNext = this; pNext; pNext = pNext->next)
		count++;
	return count;
}

BOOL PContactsDifference(LPPCONTACT pc1, LPPCONTACT pc2){
	int			c1, c2;
	LPPCONTACT	pN1 = pc1, pN2 = pc2;

	c1 = PContactsCount(pc1);
	c2 = PContactsCount(pc2);

	if(c1 != c2){
		return TRUE;
	}
	else{
		while(pN1){
			if(wcscmp(pN1->prop.name, pN2->prop.name) != 0 || wcscmp(pN1->prop.host, pN2->prop.host) != 0 || pN1->prop.address != pN2->prop.address || pN1->prop.usename != pN2->prop.usename || pN1->prop.group != pN2->prop.group){
				return TRUE;
			}
			pN1 = pN1->next;
			pN2 = pN2->next;
		}
	}
	return FALSE;
}

LPPCONTACT PContactsSort(LPPCONTACT this){
	PCONTPROP	pcp = {0};
	BOOL		found = TRUE;
	int			result = 0;

	if(this && this->next){
		while(found){
			found = FALSE;
			for(LPPCONTACT	pNext = this; pNext->next; pNext = pNext->next){
				result = _wcsicmp(pNext->prop.name, pNext->next->prop.name);
				if(result > 0){
					found = TRUE;
					memcpy(&pcp, &pNext->next->prop, sizeof(PCONTPROP));
					memcpy(&pNext->next->prop, &pNext->prop, sizeof(PCONTPROP));
					memcpy(&pNext->prop, &pcp, sizeof(PCONTPROP));
				}
			}
		}
	}
	return this;
}

int PContactsInGroup(LPPCONTACT this, int group){
	int			result = 0;
	for(LPPCONTACT lpc = this; lpc; lpc = lpc->next){
		if(lpc->prop.group == group){
			result++;
		}
	}
	return result;
}
