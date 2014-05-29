#ifndef __DOWNLOAD_H__
#define __DOWNLOAD_H__

#define	DNLM_BASE			WM_USER + 9180
#define	DNLM_PROGRESS		DNLM_BASE + 1
#define	DNLM_FINISHED		DNLM_BASE + 2
#define	DNLM_INETERROR		DNLM_BASE + 3

DWORD WINAPI DownloadFileFunc(LPVOID lpParam);

wchar_t		gd_URL[MAX_PATH * 2], gd_LocalFile[MAX_PATH], gd_ErrDesc[260];

#endif	// DOWNLOAD_H__
