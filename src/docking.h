#ifndef __DOCKING_H__
#define __DOCKING_H__

typedef struct _DOCKITEM * PDOCKITEM;
typedef struct _DOCKITEM {
	PDOCKITEM		pPrev;
	HWND			hwnd;
	PDOCKITEM		pNext;
	short			index;
}DOCKITEM;
typedef struct _DOCKHEADER {
	short			count;
	PDOCKITEM		pNext;
}DOCKHEADER, *PDOCKHEADER;

typedef struct {
	PDOCKHEADER		pLeft;
	PDOCKHEADER		pTop;
	PDOCKHEADER		pRight;
	PDOCKHEADER		pBottom;
}PDHEADERS, *P_PDHEADERS;

typedef struct {
	BOOL		fCustSkin;
	BOOL		fCustSize;
	BOOL		fCustColor;
	BOOL		fCustCaption;
	// BOOL		fCustFont;
	BOOL		fInvOrder;
	LOGFONTW	lfCaption;
	COLORREF	crCaption;
	COLORREF	crWindow;
	// LOGFONTW	lfFont;
	// COLORREF	crFont;
	int			dist;
	SIZE		custSize;
	wchar_t		szCustSkin[64];
} PNDOCK, *P_PNDOCK;

PDOCKITEM AddDockItem(PDOCKHEADER pHeader, HWND hwnd, int index);
void FreeDockList(PDOCKHEADER pHeader);
void RemoveDockItem(PDOCKHEADER pHeader, HWND hwnd);
PDOCKITEM NextDockItem(PDOCKHEADER pHeader, HWND hwnd);
void SortDockList(PDOCKHEADER pHeader);
short LastIndex(PDOCKHEADER pHeader);
PDOCKHEADER DHeader(int dockType);

PDHEADERS				g_DHeaders;
PNDOCK					g_DockSettings;

#endif	// DOCKING_H__
