#ifndef __GTOOLBAR_H__
#define __GTOOLBAR_H__

#define	GTBSTATE_NORMAL			0
#define	GTBSTATE_HIGHLIGHTED	1
#define	GTBSTATE_SELECTED		2

#define	GTBM_BASE				WM_APP + 0x111
#define	GTBM_ADDBUTTON			GTBM_BASE + 1
#define	GTBM_SETIMAGELIST		GTBM_BASE + 2
#define	GTBM_CLEAR				GTBM_BASE + 3
#define	GTBM_SELECTBUTTON		GTBM_BASE + 4
#define	GTBM_GETSELECTED		GTBM_BASE + 5

#define	GTBN_BUTTONCLICKED		1

typedef struct _GTBNMHEADER {
	NMHDR		nmh;
	int			idButton;
}GTBNMHEADER, *LPGTBNMHEADER;

typedef struct _GTBBUTTON {
	int			index;
	int			id;
	wchar_t		*lpText;
}GTBBUTTON, *LPGTBBUTTON;

HWND CreateGTB(HINSTANCE hInstance, HWND hParent, int id, int height);

#endif	// GTOOLBAR_H__
