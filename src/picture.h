#ifndef __PICTURE_H__
#define __PICTURE_H__

void GetPictureObjectSize(LPCWSTR szFile, LPSIZE lpSize);
HBITMAP LoadPictureFile(LPCWSTR szFile, HBRUSH hBrush);
void GetIconObjectSize(LPCWSTR lpszFile, LPSIZE lpSize);
HBITMAP LoadIconFile(LPCWSTR lpszFile, HBRUSH hBrush);
void GetCursorObjectSize(LPCWSTR lpszFile, LPSIZE lpSize);
HBITMAP LoadCursorFile(LPCWSTR lpszFile, HBRUSH hBrush);
HBITMAP LoadPNGFile(char * lpszFileName);
void InsertBitmapFromClipboard(LPRICHEDITOLE lpRichEditOle);
// BOOL RichEditInsertBitmap(HWND hWndRichEdit, HBITMAP hBitmap);

#endif	// PICTURE_H__
