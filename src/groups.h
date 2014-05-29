#ifndef __GROUPS_H__
#define __GROUPS_H__

typedef struct _PNGROUP *LPPNGROUP;
typedef struct _PNGROUP {
	int			id;
	int			parent;
	int			image;
	wchar_t		szName[128];
	LPPNGROUP	next;
	HK_TYPE		hotKeyShow;
	HK_TYPE		hotKeyHide;
	wchar_t		szLock[256];
	COLORREF	crWindow;
	COLORREF	crCaption;
	COLORREF	crFont;
	wchar_t		szSkin[64];
	BOOL		customCRWindow;
	BOOL		customCRCaption;
	BOOL		customCRFont;
	BOOL		customSkin;
} PNGROUP;

typedef struct _GROUP_CHECKED *PGRCHECKED;
typedef struct _GROUP_CHECKED {
	int			id;
	BOOL		checked;
}GRCHECKED;

LPPNGROUP PNGroupsAdd(LPPNGROUP * this, LPPNGROUP pg);
void PNGroupsFree(LPPNGROUP * this);
LPPNGROUP PNGroupsCopy(LPPNGROUP * tDest, LPPNGROUP tSrc);
int PNGroupsCount(LPPNGROUP this);
void PNGroupsTotalFree(LPPNGROUP * this);
void PNGroupsDelete(LPPNGROUP this, int id);
int PNGroupsNewId(LPPNGROUP this);
LPPNGROUP PNGroupsGroupById(LPPNGROUP this, int id);

#endif	// GROUPS_H__
