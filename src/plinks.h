#ifndef __PLINKS_H__
#define __PLINKS_H__

#include <windows.h>

#include "structures.h"

LPPLINK PLinksAdd(LPPLINK this, wchar_t * lpID);
void PLinksFree(LPPLINK this);
int PLinksCount(LPPLINK this);
LPPLINK PLinksRemove(LPPLINK this, wchar_t * lpID);
BOOL PLinksContains(LPPLINK this, wchar_t * lpID);
LPPLINK PLinksCopy(LPPLINK tDest, LPPLINK tSrc);

#endif	// PLINKS_H__
