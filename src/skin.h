#include <windows.h>

/** Prototypes ********************************************************/
HRGN RegionFromSkin(HBITMAP hBmp, COLORREF crMask);
HRGN RegionFromSkinWithOffset(HBITMAP hBmp, COLORREF crMask, int offset, int bWidth);
