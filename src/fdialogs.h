#ifndef __FDIALOGS_H__
#define __FDIALOGS_H__

BOOL ShowOpenFileDlg(HWND hwnd, wchar_t *pstrFileName, wchar_t *pstrTitleName, wchar_t *pstrFilter, wchar_t *pstrTitle, wchar_t *pstrInitialDir);
BOOL ShowSaveFileDlg(HWND hwnd, wchar_t *pstrFileName, wchar_t *pstrTitleName, wchar_t *pstrFilter, wchar_t *pstrTitle, wchar_t *pstrInitialDir, wchar_t *pstrDefExt);
int ShowCustomSaveFileDlg(HWND hwnd, wchar_t *pstrFileName, wchar_t *pstrTitleName, wchar_t *pstrFilter, wchar_t *pstrTitle, wchar_t *pstrDefExt);

#endif	// FDIALOGS_H__
