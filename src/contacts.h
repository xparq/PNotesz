#ifndef __CONTACTS_H__
#define __CONTACTS_H__

#include <windows.h>

#include "structures.h"

LPPCONTACT PContactsAdd(LPPCONTACT this, LPPCONTPROP pc);
BOOL PContactsContains(LPPCONTACT this, wchar_t * lpName);
LPPCONTACT PContactsItem(LPPCONTACT this, wchar_t * lpName);
void PContactsFree(LPPCONTACT this);
LPPCONTACT PContactsRemove(LPPCONTACT this, wchar_t * lpName);
LPPCONTACT PContactsCopy(LPPCONTACT tDest, LPPCONTACT tSrc);
int PContactsCount(LPPCONTACT this);
BOOL PContactsDifference(LPPCONTACT pc1, LPPCONTACT pc2);
LPPCONTACT PContactsSort(LPPCONTACT this);
wchar_t * PContactsNameByHost(LPPCONTACT this, wchar_t * lpHost);
int PContactsInGroup(LPPCONTACT this, int group);

PCONTPROP			g_Contact;
LPPCONTACT			g_TempContacts;

#endif	// CONTACTS_H__
