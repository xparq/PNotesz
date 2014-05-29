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

#include <windows.h>
#include <wchar.h>
#include "sengs.h"

void SEngsFree(LPPSENG this){
	LPPSENG	pNext, pTemp;

	pNext = this;
	while(pNext){
		pTemp = pNext;
		pNext = pNext->next;
		free(pTemp);
	}
}

LPPSENG SEngsAddWithCommandLine(LPPSENG this, wchar_t * lpName, wchar_t * lpQuery, wchar_t * lpCommand){
	LPPSENG	pSeng = calloc(1, sizeof(PSENG));
	LPPSENG	pNext;
	if(pSeng){
		wcscpy(pSeng->name, lpName);
		wcscpy(pSeng->query, lpQuery);
		wcscpy(pSeng->commandline, lpCommand);
		if(!this){
			this = pSeng;
		}
		else{
			pNext = this;
			while(pNext->next){
				pNext = pNext->next;
			}
			pNext->next = pSeng;
			pSeng->prev = pNext;
		}
	}
	return this;
}

LPPSENG SEngsAdd(LPPSENG this, wchar_t * lpName, wchar_t * lpQuery){
	LPPSENG	pSeng = calloc(1, sizeof(PSENG));
	LPPSENG	pNext;
	if(pSeng){
		wcscpy(pSeng->name, lpName);
		wcscpy(pSeng->query, lpQuery);
		pSeng->commandline[0] = '\0';
		if(!this){
			this = pSeng;
		}
		else{
			pNext = this;
			while(pNext->next){
				pNext = pNext->next;
			}
			pNext->next = pSeng;
			pSeng->prev = pNext;
		}
	}
	return this;
}

LPPSENG SEngsCopy(LPPSENG tDest, LPPSENG tSrc){
	LPPSENG	pNext;

	if(tDest)
		SEngsFree(tDest);
	tDest = NULL;
	pNext = tSrc;
	while(pNext){
		if(wcslen(pNext->commandline) == 0)
			tDest = SEngsAdd(tDest, pNext->name, pNext->query);
		else
			tDest = SEngsAddWithCommandLine(tDest, pNext->name, pNext->query, pNext->commandline);
		pNext = pNext->next;
	}
	return tDest;
}

void SEngsReplace(LPPSENG this, wchar_t * lpOldName, wchar_t * lpNewName, wchar_t * lpNewQuery){
	LPPSENG	pNext = this;

	while(pNext){
		if(_wcsicmp(pNext->name, lpOldName) == 0){
			wcscpy(pNext->name, lpNewName);
			wcscpy(pNext->query, lpNewQuery);
			return;
		}
		pNext = pNext->next;
	}
}

void SEngsReplaceWithCommandLine(LPPSENG this, wchar_t * lpOldName, wchar_t * lpNewName, wchar_t * lpNewQuery, wchar_t * lpNewCommand){
	LPPSENG	pNext = this;

	while(pNext){
		if(_wcsicmp(pNext->name, lpOldName) == 0){
			wcscpy(pNext->name, lpNewName);
			wcscpy(pNext->query, lpNewQuery);
			wcscpy(pNext->commandline, lpNewCommand);
			return;
		}
		pNext = pNext->next;
	}
}

LPPSENG SEngsRemove(LPPSENG this, wchar_t * lpName){
	LPPSENG	pNext = this, pTemp;

	while(pNext){
		if(_wcsicmp(pNext->name, lpName) == 0){
			pTemp = pNext;
			if(pNext->prev == NULL){
				//very first engine, i.e. "this"
				if(pNext->next){
					pNext->next->prev = NULL;
					this = pNext->next;
				}
				else{
					this = NULL;
				}
			}
			else if(pNext->next == NULL){
				//very last engine
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

BOOL SEngsContains(LPPSENG this, wchar_t * lpName){
	LPPSENG	pNext = this;

	while(pNext){
		if(_wcsicmp(pNext->name, lpName) == 0)
			return TRUE;
		pNext = pNext->next;
	}
	return FALSE;
}

int SEngsCount(LPPSENG this){
	LPPSENG	pNext = this;
	int		count = 0;
	while(pNext){
		count++;
		pNext = pNext->next;
	}
	return count;
}

LPPSENG SEngByName(LPPSENG this, wchar_t * lpName){
	LPPSENG	pNext = this;

	while(pNext){
		if(_wcsicmp(pNext->name, lpName) == 0)
			return pNext;
		pNext = pNext->next;
	}
	return NULL;
}

BOOL SEngsDifference(LPPSENG lp1, LPPSENG lp2){
	int		c1, c2;
	LPPSENG	p1, p2;

	c1 = SEngsCount(lp1);
	c2 = SEngsCount(lp2);
	if(c1 != c2){
		return TRUE;
	}
	else{
		p1 = lp1;
		p2 = lp2;
		while(p1){
			if(wcscmp(p1->name, p2->name) != 0 || wcscmp(p1->commandline, p2->commandline) != 0 || wcscmp(p1->query, p2->query) != 0){
				return TRUE;
			}
			p1 = p1->next;
			p2 = p2->next;
		}
	}
	return FALSE;
}
