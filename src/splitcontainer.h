#ifndef __SPLITCONTAINER_H__
#define __SPLITCONTAINER_H__

#define	CHILD_TOP			0
#define	CHILD_BOTTOM		1
#define	CHILD_LEFT			2
#define	CHILD_RIGHT			3
#define	SPLIT_HORZ			0
#define	SPLIT_VERT			1
#define	SPM_BASE			WM_APP + 0x643
#define	SPM_ADD_CHILD		SPM_BASE + 1
#define	SPM_HIDE_CHILD		SPM_BASE + 2
#define	SPM_SHOW_CHILD		SPM_BASE + 3
#define	SPM_MAIN_RESIZED	SPM_BASE + 4
#define	SPM_TOP_HEIGHT		SPM_BASE + 5
#define	SPM_LEFT_WIDTH		SPM_BASE + 6
#define	SPM_SET_NEW_TOP		SPM_BASE + 7

HWND CreateMainPanel(int splitStyle, HINSTANCE hInstance, HWND hParent, HCURSOR hCursor, int nStopper, int nLeft, int nTop, int nRight, int nBottom, int nInit);

#endif	// SPLITCONTAINER_H__
