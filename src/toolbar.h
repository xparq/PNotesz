#ifndef __TOOLBAR_H__
#define __TOOLBAR_H__

void CreateImageLists(HIMAGELIST *lpImlNormal, HIMAGELIST *lpImlGray, HINSTANCE hInst, int idBmpNormal, int idBmpGray, COLORREF crMask, int x, int y, int imgCount);
void CreateImageListsW(HIMAGELIST *lpImlNormal, HIMAGELIST *lpImlGray, HINSTANCE hInst, int idBmpNormal, int idBmpGray, COLORREF crMask, int x, int y, int imgCount);
void DoToolbar(HWND toolbar, HIMAGELIST imlNormal, HIMAGELIST imlGray, UINT btnsCount, LPTBBUTTON lpButtons);
void DoToolbarW(HWND toolbar, HIMAGELIST imlNormal, HIMAGELIST imlGray, UINT btnsCount, LPTBBUTTON lpButtons);
int GetButtonIndex(HWND toolbar, int id);
int GetButtonIndexW(HWND toolbar, int id);
#endif	// TOOLBAR_H__

