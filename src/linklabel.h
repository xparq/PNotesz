#ifndef __LINKLABEL_H__
#define __LINKLABEL_H__

#define LLGetText(hwnd) (char *)GetWindowLongPtr((hwnd), (0))
#define LLGetTextW(hwnd) (wchar_t *)GetWindowLongPtrW((hwnd), (0))

HWND CreateLinkLable(HWND hParent, HINSTANCE hInstance, char * lpText, int x, int y, int w, int h, int id, BOOL rtl);
HWND CreateLinkLableW(HWND hParent, HINSTANCE hInstance, wchar_t * lpText, int x, int y, int w, int h, int id, BOOL rtl);

#endif	// LINKLABEL_H__
