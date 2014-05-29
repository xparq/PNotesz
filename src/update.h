#ifndef __UPDATE_H__
#define __UPDATE_H__

#define	UPDM_BASE			WM_USER + 8180
#define	UPDM_GETSTRINGS		UPDM_BASE + 1
#define	UPDM_GETVERSION		UPDM_BASE + 2
#define	UPDM_UPDATEFOUND	UPDM_BASE + 3
#define	UPDM_SAMEVERSION	UPDM_BASE + 4
#define	UPDM_INETERROR		UPDM_BASE + 5

DWORD WINAPI CheckForUpdateFunc(LPVOID lpParam);

char			gu_ErrDesc[260];

#endif	// UPDATE_H__
