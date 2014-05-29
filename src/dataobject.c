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
 Module   : dataobject.c
 Abstract : This file is implementation of IDataObject interface needed for 
            loading picture files and inserting pictures into RichEdit 
            control
 ------------------------------------------------------------------@@-@@-*/
#include <ocidl.h>
#include <richedit.h>
#include <richole.h>
#include <stddef.h>

#include "macros.h"

// --- Macros ---
#define OleSetData(this, pformatetc, pmedium, fRelease)\
	(this)->lpVtbl->SetData(this, pformatetc, pmedium, fRelease)

#undef INTERFACE
#define INTERFACE IDataObject

typedef struct tagCDataObject{
	INTERFACE		IDataObjectIFace;
	ULONG			cRef;

	STGMEDIUM 		stgmed;
	FORMATETC 		formatetc;
} CDataObject;

// --- IUnknown methods ---
static STDMETHODIMP IDOC_QueryInterface(THIS_ REFIID riid, void **ppv){
	CDataObject * ThisObj = IMPL(CDataObject, IDataObjectIFace);

	if(IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IDataObject)){
		*ppv = &ThisObj->IDataObjectIFace;
	}
	else{
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	OleAddRef((INTERFACE *)*ppv);

	return NOERROR;
}

static STDMETHODIMP_(ULONG)IDOC_AddRef(THIS){
	CDataObject * ThisObj = IMPL(CDataObject, IDataObjectIFace);

	return InterlockedIncrement((long int *)&ThisObj->cRef);
}

static STDMETHODIMP_(ULONG)IDOC_Release(THIS){
	CDataObject * ThisObj = IMPL(CDataObject, IDataObjectIFace);

	if(InterlockedDecrement((long int *)&ThisObj->cRef) == 0){
		free(ThisObj);
		return 0;
	}
	return ThisObj->cRef;
}

//--- IDataObject methods ---
static STDMETHODIMP IDOC_GetData(THIS_ FORMATETC *pformatetcIn, STGMEDIUM *pmedium){ 
    CDataObject * ThisObj = IMPL(CDataObject, IDataObjectIFace);

	HANDLE		hDst;
	hDst = OleDuplicateData(ThisObj->stgmed.hBitmap, CF_BITMAP, 0);
	if (hDst == NULL)
	{
		return E_HANDLE;
	}

	pmedium->tymed = TYMED_GDI;
	pmedium->hBitmap = (HBITMAP)hDst;
	pmedium->pUnkForRelease = NULL;

	return S_OK; 
} 

static STDMETHODIMP IDOC_GetDataHere(THIS_ FORMATETC *pformatetc, STGMEDIUM *pmedium){ 
    // CDataObject * ThisObj = IMPL(CDataObject, IDataObjectIFace);
    return E_NOTIMPL; 
}

static STDMETHODIMP IDOC_QueryGetData(THIS_ FORMATETC *pformatetc){ 
    // CDataObject * ThisObj = IMPL(CDataObject, IDataObjectIFace);
    return E_NOTIMPL; 
}

static STDMETHODIMP IDOC_GetCanonicalFormatEtc(THIS_ FORMATETC *pformatectIn, FORMATETC *pformatetcOut){ 
    // CDataObject * ThisObj = IMPL(CDataObject, IDataObjectIFace);
    return E_NOTIMPL; 
}

static STDMETHODIMP IDOC_SetData(THIS_ FORMATETC *pformatetc, STGMEDIUM *pmedium, BOOL fRelease){ 
    CDataObject * ThisObj = IMPL(CDataObject, IDataObjectIFace);

	ThisObj->formatetc = *pformatetc;
	ThisObj->stgmed = *pmedium;
    return E_NOTIMPL; 
}

static STDMETHODIMP IDOC_EnumFormatEtc(THIS_ DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc){ 
    // CDataObject * ThisObj = IMPL(CDataObject, IDataObjectIFace);
    return E_NOTIMPL; 
}

static STDMETHODIMP IDOC_DAdvise(THIS_ FORMATETC *pformatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection){ 
    // CDataObject * ThisObj = IMPL(CDataObject, IDataObjectIFace);
    return E_NOTIMPL; 
}

static STDMETHODIMP IDOC_DUnadvise(THIS_ DWORD dwConnection){ 
    // CDataObject * ThisObj = IMPL(CDataObject, IDataObjectIFace);
    return E_NOTIMPL; 
}

static const IDataObjectVtbl vtblIDataObject = { 
    IDOC_QueryInterface, IDOC_AddRef, IDOC_Release, 
    IDOC_GetData, IDOC_GetDataHere, 
    IDOC_QueryGetData, IDOC_GetCanonicalFormatEtc, 
    IDOC_SetData, IDOC_EnumFormatEtc, 
    IDOC_DAdvise, IDOC_DUnadvise 
};

//--- Constructor ---
CDataObject * new_CDataObject(void) 
{ 
    CDataObject * ThisObj = calloc(1, sizeof(CDataObject)); 
    if (ThisObj) 
    { 
        ThisObj->cRef = 1; 
        ThisObj->IDataObjectIFace.lpVtbl = 
            (IDataObjectVtbl *) &vtblIDataObject; 
    } 
    return ThisObj; 
}
//--- Destructor ---
void delete_CDataObject(CDataObject * pDataObject) 
{ 
	ReleaseStgMedium(&pDataObject->stgmed);
    OleRelease(&pDataObject->IDataObjectIFace); 
}

//--- Other functions ---
static void SetBitmap(CDataObject * pDataObject, HBITMAP hBitmap);
static IOleObject * GetOleObject(CDataObject * pDataObject, IOleClientSite *pOleClientSite, IStorage *pStorage);

static void SetBitmap(CDataObject * pDataObject, HBITMAP hBitmap){
	STGMEDIUM stgm;
	stgm.tymed = TYMED_GDI;					// Storage medium = HBITMAP handle		
	stgm.hBitmap = hBitmap;
	stgm.pUnkForRelease = NULL;				// Use ReleaseStgMedium

	FORMATETC fm;
	fm.cfFormat = CF_BITMAP;				// Clipboard format = CF_BITMAP
	fm.ptd = NULL;							// Target Device = Screen
	fm.dwAspect = DVASPECT_CONTENT;			// Level of detail = Full content
	fm.lindex = -1;							// Index = Not applicaple
	fm.tymed = TYMED_GDI;					// Storage medium = HBITMAP handle

	OleSetData(&pDataObject->IDataObjectIFace, &fm, &stgm, TRUE);
}

static IOleObject * GetOleObject(CDataObject * pDataObject, IOleClientSite *pOleClientSite, IStorage *pStorage)
{
	SCODE sc;
	IOleObject *pOleObject;
	IDataObject	* idd = (IDataObject *)(void *)pDataObject;
	sc = OleCreateStaticFromData(idd, &IID_IOleObject, OLERENDER_FORMAT, 
			&pDataObject->formatetc, pOleClientSite, pStorage, (void **)&pOleObject);
	if (sc != S_OK)
		return NULL;
	return pOleObject;
}

void InsertBitmap(IRichEditOle* pRichEditOle, HBITMAP hBitmap)
{
	SCODE sc;

	//Create new image data object
	CDataObject *pods = new_CDataObject();
	LPDATAOBJECT lpDataObject;
	OleQueryInterface(&pods->IDataObjectIFace, &IID_IDataObject, (void **)&lpDataObject);

	SetBitmap(pods, hBitmap);

	//Get the RichEdit container site
	IOleClientSite *pOleClientSite;	
	pRichEditOle->lpVtbl->GetClientSite(pRichEditOle, &pOleClientSite);

	//Initialize a Storage Object
	IStorage *pStorage;	

	LPLOCKBYTES lpLockBytes = NULL;
	sc = CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
	if (sc != S_OK)
		return;
	
	sc = StgCreateDocfileOnILockBytes(lpLockBytes,
		STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
	if (sc != S_OK)
	{
		lpLockBytes->lpVtbl->Release(lpLockBytes);
		lpLockBytes = NULL;
		return;
	}

	//The final ole object which will be inserted in the richedit control
	IOleObject *pOleObject; 
	pOleObject = GetOleObject(pods, pOleClientSite, pStorage);
	if(pOleObject == NULL)
		return;

	//all items are "contained" -- this makes our reference to this object
	//weak -- which is needed for links to embedding silent update.
	OleSetContainedObject((IUnknown *)pOleObject, TRUE);

	//Now Add the object to the RichEdit 
	REOBJECT reobject;
	ZeroMemory(&reobject, sizeof(REOBJECT));
	reobject.cbStruct = sizeof(REOBJECT);
	
	CLSID clsid;
	sc = pOleObject->lpVtbl->GetUserClassID(pOleObject, &clsid);
	if (sc != S_OK)
		return;

	reobject.clsid = clsid;
	reobject.cp = REO_CP_SELECTION;
	reobject.dvaspect = DVASPECT_CONTENT;
	reobject.poleobj = pOleObject;
	reobject.polesite = pOleClientSite;
	reobject.pstg = pStorage;

	//Insert the bitmap at the current location in the richedit control
	pRichEditOle->lpVtbl->InsertObject(pRichEditOle, &reobject);

	//Release all unnecessary interfaces
	pOleObject->lpVtbl->Release(pOleObject);
	pOleClientSite->lpVtbl->Release(pOleClientSite);
	pStorage->lpVtbl->Release(pStorage);
	lpDataObject->lpVtbl->Release(lpDataObject);

	//delete IDataObject
	delete_CDataObject(pods);
}
