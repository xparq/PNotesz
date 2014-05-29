#ifndef __LOCALSYNC_H__
#define __LOCALSYNC_H__

#include <windows.h>

#include "structures.h"

LPPLOCALSYNC PLocalSyncAdd(LPPLOCALSYNC * this, wchar_t * lpName, wchar_t * lpPathNotes, wchar_t * lpPathId);
void PLocalSyncFree(LPPLOCALSYNC * this);
LPPLOCALSYNC PLocalSyncCopy(LPPLOCALSYNC * tDest, LPPLOCALSYNC tSrc);
BOOL PLocalSyncContains(LPPLOCALSYNC this, wchar_t * lpName);
int PLocalSyncCount(LPPLOCALSYNC this);
BOOL PLocalSyncDifference(LPPLOCALSYNC pc1, LPPLOCALSYNC pc2);
LPPLOCALSYNC PLocalSyncRemove(LPPLOCALSYNC * this, wchar_t * lpName);
LPPLOCALSYNC PLocalSyncItem(LPPLOCALSYNC this, wchar_t * lpName);

LPPLOCALSYNC			g_TempLocalSyncs;
PLOCALSYNC				g_LocalSync;

#endif	// LOCALSYNC_H__
