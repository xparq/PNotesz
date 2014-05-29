#ifndef __BACKUPRESTORE_H__
#define __BACKUPRESTORE_H__

#include "shared.h"

BOOL DoFullBackup(wchar_t * lpBackFile);
BOOL RestoreFullBackUp(wchar_t * lpBackFile);

#endif	// BACKUPRESTORE_H__
