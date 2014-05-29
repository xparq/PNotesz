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

/*-@@+@@------------------------------------------------------------------
 Module   : oleinterface.c 
 Abstract : This file is implementation of IRichEditOleCallback interface 
            needed for loading pictures from RTF file into RichEdit control
 ------------------------------------------------------------------@@-@@-*/
#include <objbase.h>
#include <richedit.h>
#include <richole.h>
#include <stdlib.h>
#include <stddef.h>

#include "macros.h"

#undef INTERFACE
#define INTERFACE IRichEditOleCallback

typedef struct tagCReCallback{
	INTERFACE		IRichEditOleCallbackIFace;
	ULONG			cRef;
	//drop handle
	HDROP			hDrop;
} CReCallback;

// --- IUnknown methods ---
static STDMETHODIMP IREOC_QueryInterface(THIS_ REFIID riid, void **ppv){
	CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace);

	if(IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IRichEditOleCallback)){
		*ppv = &ThisObj->IRichEditOleCallbackIFace;
	}
	else{
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	OleAddRef((INTERFACE *)*ppv);

	return NOERROR;
}

static STDMETHODIMP_(ULONG)IREOC_AddRef(THIS){
	CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace);

	return InterlockedIncrement((long int *)&ThisObj->cRef);
}

static STDMETHODIMP_(ULONG)IREOC_Release(THIS){
	CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace);

	if(InterlockedDecrement((long int *)&ThisObj->cRef) == 0){
		free(ThisObj);
		return 0;
	}
	return ThisObj->cRef;
}

//--- IRichEditOleCallback methods ---
static STDMETHODIMP IREOC_GetNewStorage(THIS_ LPSTORAGE * lplpstg){
	CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace);

	if(ThisObj->hDrop)
		return E_NOTIMPL;
	HRESULT		hr;
    ILockBytes *pILockBytes;
	hr = CreateILockBytesOnHGlobal(NULL, TRUE, &pILockBytes);
	if(hr == S_OK)
    	hr = StgCreateDocfileOnILockBytes(pILockBytes, STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE | STGM_READWRITE | STGM_CREATE, 0, lplpstg);
	
    return hr;
}

static STDMETHODIMP IREOC_GetInPlaceContext(THIS_ LPOLEINPLACEFRAME * lplpFrame, LPOLEINPLACEUIWINDOW * lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo){ 
    // CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace);
    return E_NOTIMPL; 
} 

static STDMETHODIMP IREOC_ShowContainerUI(THIS_ BOOL fShow){ 
    // CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace);
    return E_NOTIMPL; 
} 

static STDMETHODIMP IREOC_QueryInsertObject(THIS_ LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp){ 
    CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace);
	if(ThisObj->hDrop)
		ThisObj->hDrop = NULL;
    return S_OK; 
} 

static STDMETHODIMP IREOC_DeleteObject(THIS_ LPOLEOBJECT lpoleobj){ 
    // CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace);
    return E_NOTIMPL; 
} 

static STDMETHODIMP IREOC_QueryAcceptData(THIS_ LPDATAOBJECT lpdataobj, CLIPFORMAT * lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict){ 
    CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace);
	if(ThisObj->hDrop)
		return E_NOTIMPL;
	//prepare format and medium storage
	STGMEDIUM 	td = {TYMED_HGLOBAL};
	FORMATETC 	fr = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	//get data from IDataObject
	HRESULT 	hr = lpdataobj->lpVtbl->GetData(lpdataobj, &fr, &td);
	if(!FAILED(hr)){
		//get drop handle
		ThisObj->hDrop = (HDROP)GlobalLock(td.hGlobal);
		GlobalUnlock(td.hGlobal);	
	}
	ReleaseStgMedium(&td);
    return E_NOTIMPL; 
} 

static STDMETHODIMP IREOC_ContextSensitiveHelp(THIS_ BOOL fEnterMode){ 
    // CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace);
    return E_NOTIMPL; 
}

static STDMETHODIMP IREOC_GetClipboardData(THIS_ CHARRANGE * lpchrg, DWORD reco, LPDATAOBJECT * lplpdataobj){ 
    // CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace); 
    return E_NOTIMPL; 
} 

static STDMETHODIMP IREOC_GetDragDropEffect(THIS_ BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect){ 
    // CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace); 
    return E_NOTIMPL; 
}

static STDMETHODIMP IREOC_GetContextMenu(THIS_ WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE * lpchrg, HMENU * lphmenu){ 
    // CReCallback * ThisObj = IMPL(CReCallback, IRichEditOleCallbackIFace); 
    return E_NOTIMPL; 
} 

static const IRichEditOleCallbackVtbl vtblIRichEditOleCallback = { 
    IREOC_QueryInterface, IREOC_AddRef, IREOC_Release, 
    IREOC_GetNewStorage, IREOC_GetInPlaceContext, 
    IREOC_ShowContainerUI, IREOC_QueryInsertObject, 
    IREOC_DeleteObject, IREOC_QueryAcceptData, 
    IREOC_ContextSensitiveHelp, IREOC_GetClipboardData, 
    IREOC_GetDragDropEffect, IREOC_GetContextMenu 
}; 

CReCallback * new_CReCallback(void) 
{ 
    CReCallback * ThisObj = calloc(1, sizeof(CReCallback)); 
    if (ThisObj) 
    { 
        ThisObj->cRef = 1; 
        ThisObj->IRichEditOleCallbackIFace.lpVtbl = 
            (IRichEditOleCallbackVtbl *) &vtblIRichEditOleCallback;
		ThisObj->hDrop = NULL;
    } 
    return ThisObj; 
}

void delete_CReCallback(CReCallback * pCallback) 
{ 
    OleRelease(&pCallback->IRichEditOleCallbackIFace); 
} 
