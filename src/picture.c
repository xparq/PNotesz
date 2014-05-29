// Copyright (C) 2010 Andrey Gruber (aka lamer)

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <windows.h>
#include <windowsx.h>
#include <ocidl.h>
#include <olectl.h>
#include <richedit.h>
#include <richole.h>
#include <pnglib.h>

#define HIMETRIC_INCH	2540

/*-@@+@@------------------------------------------------------------------
 Procedure: GetPictureObjectSize

 Synopsys : Loads picture file into IStream and finds picture size
 Input    : szFile [in] - path of picture file
            lpSize [out] - pointer to SIZE structure filled with width and 
            height of picture
 ------------------------------------------------------------------@@-@@-*/
void GetPictureObjectSize(LPCWSTR szFile, LPSIZE lpSize){
	HANDLE			hFile = INVALID_HANDLE_VALUE;
	LPVOID			pvData = NULL;
	HGLOBAL			hGlobal = NULL;
	DWORD			dwFileSize = INVALID_FILE_SIZE;
	DWORD			dwBytesRead = 0;
	BOOL			bRead = FALSE, bUnlocked = FALSE;
	LPSTREAM		pstm = NULL;
	HRESULT			hr;
	LPPICTURE		pPicture = NULL;
	long			hmWidth, hmHeight;
	HDC				hdc;

	//reset size
	lpSize->cx = lpSize->cy = 0;

	//open file
	hFile = CreateFileW(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		goto _exit;

	//get file size
	dwFileSize = GetFileSize(hFile, NULL);
	if(dwFileSize == INVALID_FILE_SIZE)
		goto _exit;

	//alloc memory based on file size
	hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
	if(hGlobal == NULL)
		goto _exit;

	pvData = GlobalLock(hGlobal);
	if(pvData == NULL)
		goto _exit;

	//read file and store in global memory
	bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
	if(!bRead)
		goto _exit;

	GlobalUnlock(hGlobal);
	bUnlocked = TRUE;

	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;

	//create IStream* from global memory
	hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
	if(!SUCCEEDED(hr) || !pstm)
		goto _exit;

	//Create IPicture from image file
	hr = OleLoadPicture(pstm, dwFileSize, FALSE, &IID_IPicture, (LPVOID *)&pPicture);
	if(!SUCCEEDED(hr) || !pPicture)
		goto _exit;
	pstm->lpVtbl->Release(pstm);
	pstm = NULL;
	
	//get width and height of picture
	pPicture->lpVtbl->get_Width(pPicture, &hmWidth);
	pPicture->lpVtbl->get_Height(pPicture, &hmHeight);

	//convert himetric to pixels
	hdc = GetDC(NULL);
	lpSize->cx = MulDiv(hmWidth, GetDeviceCaps(hdc, LOGPIXELSX), HIMETRIC_INCH);
	lpSize->cy = MulDiv(hmHeight, GetDeviceCaps(hdc, LOGPIXELSY), HIMETRIC_INCH);
	ReleaseDC(NULL, hdc);

_exit:
	//clean up resources
	if(hGlobal){
		if(!bUnlocked)
			GlobalUnlock(hGlobal);
		GlobalFree(hGlobal);
	}
	if(hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	if(pstm)
		pstm->lpVtbl->Release(pstm);
	if(pPicture)
		pPicture->lpVtbl->Release(pPicture);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: LoadPictureFile

 Synopsys : Loads picture file into IStream and returns HBITMAP created 
            from it
 Input    : szFile [in] - path to picture file
            hBrush [in] - brush used to fill bitmap backgroud
 Output   : handle to bitmap created from picture 
 ------------------------------------------------------------------@@-@@-*/
HBITMAP LoadPictureFile(LPCWSTR szFile, HBRUSH hBrush){
	HBITMAP			hBmp = NULL, hBmpOld;
	HANDLE			hFile = INVALID_HANDLE_VALUE;
	LPVOID			pvData = NULL;
	HGLOBAL			hGlobal = NULL;
	DWORD			dwFileSize = INVALID_FILE_SIZE;
	DWORD			dwBytesRead = 0;
	BOOL			bRead = FALSE, bUnlocked = FALSE;
	LPSTREAM		pstm = NULL;
	HRESULT			hr;
	LPPICTURE		pPicture = NULL;
	HDC				hdcScreen = NULL, hdcTemp = NULL;
	long			hmWidth, hmHeight;
	int				nWidth, nHeight;
	RECT			rc;

	// open file
	hFile = CreateFileW(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		goto _exit;

	// get file size
	dwFileSize = GetFileSize(hFile, NULL);
	if(dwFileSize == INVALID_FILE_SIZE)
		goto _exit;

	// alloc memory based on file size
	hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
	if(hGlobal == NULL)
		goto _exit;

	pvData = GlobalLock(hGlobal);
	if(pvData == NULL)
		goto _exit;

	// read file and store in global memory
	bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
	if(!bRead)
		goto _exit;

	GlobalUnlock(hGlobal);
	bUnlocked = TRUE;

	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;

	// create IStream* from global memory
	hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
	if(!SUCCEEDED(hr) || !pstm)
		goto _exit;

	// Create IPicture from image file
	hr = OleLoadPicture(pstm, dwFileSize, FALSE, &IID_IPicture, (LPVOID *)&pPicture);
	if(!SUCCEEDED(hr) || !pPicture)
		goto _exit;
	pstm->lpVtbl->Release(pstm);
	pstm = NULL;

	// Get screen DC
	hdcScreen = GetDC(NULL);
	if(!hdcScreen)
		goto _exit;

	// Create temporary DC
	hdcTemp = CreateCompatibleDC(hdcScreen);
	if(!hdcTemp)
		goto _exit;

	// get width and height of picture
	pPicture->lpVtbl->get_Width(pPicture, &hmWidth);
	pPicture->lpVtbl->get_Height(pPicture, &hmHeight);

	// convert himetric to pixels
	nWidth = MulDiv(hmWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), HIMETRIC_INCH);
	nHeight = MulDiv(hmHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), HIMETRIC_INCH);

	// create HBITMAP
	hBmp = CreateCompatibleBitmap(hdcScreen, nWidth, nHeight);
	if(!hBmp)
		goto _exit;

	ReleaseDC(NULL, hdcScreen);
	hdcScreen = NULL;

	//select bitmap in temporary DC
	hBmpOld = SelectBitmap(hdcTemp, hBmp);

	// fill bitmap background
	SetRect(&rc, 0, 0, nWidth, nHeight);
	FillRect(hdcTemp, &rc, hBrush);

	// blit picture using IPicture::Render
	hr = pPicture->lpVtbl->Render(pPicture, hdcTemp, 0, 0, nWidth, nHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
	if(hr != S_OK)
		goto _exit;

	pPicture->lpVtbl->Release(pPicture);
	pPicture = NULL;

	// select our bitmap back
	SelectBitmap(hdcTemp, hBmpOld);
	
	DeleteDC(hdcTemp);
	hdcTemp = NULL;

_exit:
	//clean up resources
	if(hGlobal){
		if(!bUnlocked)
			GlobalUnlock(hGlobal);
		GlobalFree(hGlobal);
	}
	if(hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	if(pstm)
		pstm->lpVtbl->Release(pstm);
	if(pPicture)
		pPicture->lpVtbl->Release(pPicture);
	if(hdcScreen)
		ReleaseDC(NULL, hdcScreen);
	if(hdcTemp)
		DeleteDC(hdcTemp);

	return hBmp;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetIconObjectSize

 Synopsys : Loads icon file and finds its size
 Input    : szFile [in] - path of icon file
            lpSize [out] - pointer to SIZE structure filled with width and 
            height of icon
 ------------------------------------------------------------------@@-@@-*/
void GetIconObjectSize(LPCWSTR lpszFile, LPSIZE lpSize){
	HICON		hIcon = LoadImageW(NULL, lpszFile, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	ICONINFO	iInfo;
	BITMAP		bm;
	int			w, h;

	if(hIcon == NULL)
		return;
	ZeroMemory(&iInfo, sizeof(iInfo));
	GetIconInfo(hIcon, &iInfo);
	if(iInfo.hbmColor != NULL){
		GetObject(iInfo.hbmColor, sizeof(bm), &bm);
		w = bm.bmWidth;
		h = bm.bmHeight;
	}
	else{
		GetObject(iInfo.hbmMask, sizeof(bm), &bm);
		w = bm.bmWidth;
		h = bm.bmHeight / 2;
	}
	lpSize->cx = w;
	lpSize->cy = h;
	DestroyIcon(hIcon);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: LoadIconFile

 Synopsys : Loads icon file and returns HBITMAP created 
            from it
 Input    : szFile [in] - path to icon file
            hBrush [in] - brush used to fill bitmap backgroud
 Output   : handle to bitmap created from icon 
 ------------------------------------------------------------------@@-@@-*/
HBITMAP LoadIconFile(LPCWSTR lpszFile, HBRUSH hBrush){
	HBITMAP		hBmp = NULL, hOldBmp;
	HDC			hdcTemp, hdcScreen;
	HICON		hIcon = LoadImageW(NULL, lpszFile, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	ICONINFO	iInfo;
	BITMAP		bm;
	int			w, h;

	if(hIcon == NULL)
		return NULL;
	ZeroMemory(&iInfo, sizeof(iInfo));
	GetIconInfo(hIcon, &iInfo);
	if(iInfo.hbmColor != NULL){
		GetObject(iInfo.hbmColor, sizeof(bm), &bm);
		w = bm.bmWidth;
		h = bm.bmHeight;
	}
	else{
		GetObject(iInfo.hbmMask, sizeof(bm), &bm);
		w = bm.bmWidth;
		h = bm.bmHeight / 2;
	}
	RECT	rc = { 0, 0, w, h};
	hdcScreen = GetDC(NULL);
	hdcTemp = CreateCompatibleDC(hdcScreen);
	hBmp = CreateCompatibleBitmap(hdcScreen, w, h);
	ReleaseDC(NULL, hdcScreen);
	hOldBmp = SelectBitmap(hdcTemp, hBmp);
	FillRect(hdcTemp, &rc, hBrush);
	DrawIconEx(hdcTemp, 0, 0, hIcon, w, h, 0, NULL, DI_NORMAL);
	SelectBitmap(hdcTemp, hOldBmp);
	DeleteDC(hdcTemp);
	DestroyIcon(hIcon);
	return hBmp;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetCursorObjectSize

 Synopsys : Loads cursor file and finds its size
 Input    : szFile [in] - path of cursor file
            lpSize [out] - pointer to SIZE structure filled with width and 
            height of cursor
 ------------------------------------------------------------------@@-@@-*/
void GetCursorObjectSize(LPCWSTR lpszFile, LPSIZE lpSize){
	HCURSOR		hCursor = LoadCursorFromFileW(lpszFile);
	ICONINFO	iInfo;
	BITMAP		bm;
	int			w, h;

	if(hCursor == NULL)
		return;
	ZeroMemory(&iInfo, sizeof(iInfo));
	GetIconInfo(hCursor, &iInfo);
	if(iInfo.hbmColor != NULL){
		GetObject(iInfo.hbmColor, sizeof(bm), &bm);
		w = bm.bmWidth;
		h = bm.bmHeight;
	}
	else{
		GetObject(iInfo.hbmMask, sizeof(bm), &bm);
		w = bm.bmWidth;
		h = bm.bmHeight / 2;
	}
	lpSize->cx = w;
	lpSize->cy = h;
	DestroyCursor(hCursor);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: LoadCursorFile

 Synopsys : Loads cursor file and returns HBITMAP created 
            from it
 Input    : szFile [in] - path to cursor file
            hBrush [in] - brush used to fill bitmap backgroud
 Output   : handle to bitmap created from cursor 
 ------------------------------------------------------------------@@-@@-*/
HBITMAP LoadCursorFile(LPCWSTR lpszFile, HBRUSH hBrush){
	HBITMAP		hBmp = NULL, hOldBmp;
	HDC			hdcTemp, hdcScreen;
	HCURSOR		hCursor = LoadCursorFromFileW(lpszFile);
	ICONINFO	iInfo;
	BITMAP		bm;
	int			w, h;

	if(hCursor == NULL)
		return NULL;
	ZeroMemory(&iInfo, sizeof(iInfo));
	GetIconInfo(hCursor, &iInfo);
	if(iInfo.hbmColor != NULL){
		GetObject(iInfo.hbmColor, sizeof(bm), &bm);
		w = bm.bmWidth;
		h = bm.bmHeight;
	}
	else{
		GetObject(iInfo.hbmMask, sizeof(bm), &bm);
		w = bm.bmWidth;
		h = bm.bmHeight / 2;
	}
	RECT	rc = { 0, 0, w, h};
	hdcScreen = GetDC(NULL);
	hdcTemp = CreateCompatibleDC(hdcScreen);
	hBmp = CreateCompatibleBitmap(hdcScreen, w, h);
	ReleaseDC(NULL, hdcScreen);
	hOldBmp = SelectBitmap(hdcTemp, hBmp);
	FillRect(hdcTemp, &rc, hBrush);
	DrawIconEx(hdcTemp, 0, 0, hCursor, w, h, 0, NULL, DI_NORMAL);
	SelectBitmap(hdcTemp, hOldBmp);
	DeleteDC(hdcTemp);
	DestroyCursor(hCursor);
	return hBmp;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: LoadPNGFile

 Synopsys : Loads PNG file and returns HBITMAP created 
            from it
 Input    : lpszFileName [in] - path to PNG file
 Output   : handle to bitmap created from picture 
 ------------------------------------------------------------------@@-@@-*/
HBITMAP LoadPNGFile(char * lpszFileName){
	HBITMAP		hBitmap = NULL;
	PNGINFO		pngInfo;

	// Initialize structure
	if(PNG_Init(&pngInfo)){
		// Load PNG file
		if(PNG_LoadFile(&pngInfo, lpszFileName)){
			// Decode PNG data
			if(PNG_Decode(&pngInfo)){
				// Create bitmap from PNG
				hBitmap = PNG_CreateBitmap(&pngInfo, NULL, PNG_OUTF_AUTO, FALSE);
				if(hBitmap){
					// Cleanup
					PNG_Cleanup(&pngInfo);
				}
			}
		}
	}
	// return HBITMAP
	return hBitmap;
}

void InsertBitmapFromClipboard(LPRICHEDITOLE lpRichEditOle)
{
	HRESULT hr;
	IDataObject * pDataObject;
	LPOLEOBJECT lpOleObject = NULL;
	LPSTORAGE lpStorage = NULL;
	LPOLECLIENTSITE lpOleClientSite = NULL;
	LPLOCKBYTES lpLockBytes = NULL;
	CLIPFORMAT cfFormat = 0;
	LPFORMATETC lpFormatEtc = NULL;
	FORMATETC formatEtc;
	SCODE sc;
	LPUNKNOWN lpUnknown;
	REOBJECT reobject;
	CLSID clsid;
	SIZEL sizel;

	OleGetClipboard(&pDataObject);

	if (pDataObject == NULL)
		return;
	sc = CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
	if(sc != S_OK || lpLockBytes == NULL)
		return;

	sc = StgCreateDocfileOnILockBytes(lpLockBytes, STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &lpStorage);
	if(sc != S_OK || lpStorage == NULL)
		return;

	lpFormatEtc = &formatEtc;
	lpFormatEtc->cfFormat = cfFormat;
	lpFormatEtc->ptd = NULL;
	lpFormatEtc->dwAspect = DVASPECT_CONTENT;
	lpFormatEtc->lindex = -1;
	lpFormatEtc->tymed = TYMED_NULL;

	lpRichEditOle->lpVtbl->GetClientSite(lpRichEditOle, &lpOleClientSite);
	if(lpOleClientSite == NULL)
		return;

	hr = OleCreateStaticFromData(pDataObject, &IID_IUnknown, OLERENDER_DRAW, lpFormatEtc, lpOleClientSite, lpStorage, (void**)&lpOleObject);
	if(hr != S_OK || lpOleObject == NULL)
		return;

	lpUnknown = (LPUNKNOWN)lpOleObject;
	lpUnknown->lpVtbl->QueryInterface(lpUnknown, &IID_IOleObject, (void**)&lpOleObject);
	lpUnknown->lpVtbl->Release(lpUnknown);
	OleSetContainedObject((LPUNKNOWN)lpOleObject, TRUE);

	ZeroMemory(&reobject, sizeof(REOBJECT));
	reobject.cbStruct = sizeof(REOBJECT);

	sc = lpOleObject->lpVtbl->GetUserClassID(lpOleObject, &clsid);

	reobject.clsid = clsid;
	reobject.cp = REO_CP_SELECTION;
	reobject.dvaspect = DVASPECT_CONTENT;
	// reobject.dwFlags = REO_BELOWBASELINE; //REO_RESIZABLE |	REO_BELOWBASELINE;
	// reobject.dwUser = 0;
	reobject.poleobj = lpOleObject;
	reobject.polesite = lpOleClientSite;
	reobject.pstg = lpStorage;

	sizel.cx = sizel.cy = 0;
	reobject.sizel = sizel;

	lpRichEditOle->lpVtbl->InsertObject(lpRichEditOle, &reobject);

	lpLockBytes->lpVtbl->Release(lpLockBytes);
	lpStorage->lpVtbl->Release(lpStorage);
	pDataObject->lpVtbl->Release(pDataObject);
	lpRichEditOle->lpVtbl->Release(lpRichEditOle);
	lpOleClientSite->lpVtbl->Release(lpOleClientSite);
	lpOleObject->lpVtbl->Release(lpOleObject);
}
