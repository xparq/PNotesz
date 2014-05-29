#ifndef __GSEARCHBOX_H__
#define __GSEARCHBOX_H__

#define	GSBM_BASE				WM_USER + 0x456
#define	GSBM_SET_DEF_PROMT		GSBM_BASE + 1
#define	GSBM_CLEAR				GSBM_BASE + 2
#define	GSBM_GET_STATUS			GSBM_BASE + 3

#define	GSBNC_STRING			0
#define	GSBNC_CLEAR				1

typedef struct _GSBNTF {
	NMHDR		nmh;
	wchar_t		*lpString;
}GSBNTF, * P_GSBNTF;

HWND CreateGSearchBoxWindow(HINSTANCE hInstance, HWND hParent, HICON hIcon, HICON hIconDisabled, wchar_t * lpPrompt, int left, int top, int width, int height);

#endif	// GSEARCHBOX_H__
