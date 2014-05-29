// Copyright (C) 2007 Andrey Gruber (aka lamer)

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
#include <shlobj.h>
#include <objbase.h>

HRESULT CreateShortcut(LPCTSTR pszLink, 
						LPCTSTR pszFile, 
						LPCTSTR pszDir, 
						LPCTSTR pszArgs, 
						int iShowCmd, 
						LPCTSTR pszIconPath, 
						int iIcon, 
						LPCTSTR pszDescription, 
						WORD wHotkey){
	
	HRESULT       	hRes;					/* Returned COM result code */
  	IShellLink		* pShellLink;			/* IShellLink object pointer */
  	IPersistFile	* pPersistFile;			/* IPersistFile object pointer */
	WORD          	wszLinkfile[MAX_PATH];	/* pszLinkfile as Unicode string */
  	int           	iWideCharsWritten;		/* Number of wide characters written */
	
	hRes = CoInitialize(NULL);
	if(SUCCEEDED(hRes)){
		hRes = CoCreateInstance(&CLSID_ShellLink,     /* pre-defined CLSID of the IShellLink object */
	                            NULL,                 /* pointer to parent interface if part of aggregate */
	                            CLSCTX_INPROC_SERVER, /* caller and called code are in same process */
	                            &IID_IShellLink,      /* pre-defined interface of the IShellLink object */
	                            (LPVOID)&pShellLink);        
		if(SUCCEEDED(hRes)){
			/* Set the fields in the IShellLink object */
			if(pszFile != NULL && strlen(pszFile) > 0)
	      		hRes = pShellLink->lpVtbl->SetPath(pShellLink, pszFile);
			if(pszArgs != NULL && strlen(pszArgs) > 0)
				hRes = pShellLink->lpVtbl->SetArguments(pShellLink, pszArgs);
			if(iShowCmd >= 0)
				hRes = pShellLink->lpVtbl->SetShowCmd(pShellLink, iShowCmd);
			if(pszDir != NULL && strlen(pszDir) > 0)
				hRes = pShellLink->lpVtbl->SetWorkingDirectory(pShellLink, pszDir);
			if(pszIconPath != NULL && strlen(pszIconPath) >0 && iIcon >=0)
				hRes = pShellLink->lpVtbl->SetIconLocation(pShellLink, pszIconPath, iIcon);
			if(pszDescription != NULL && strlen(pszDescription) > 0)
				hRes = pShellLink->lpVtbl->SetDescription(pShellLink, pszDescription);
			if(wHotkey > 0)
				hRes = pShellLink->lpVtbl->SetHotkey(pShellLink, wHotkey);
			/* Use the IPersistFile object to save the shell link */
	      	hRes = pShellLink->lpVtbl->QueryInterface(pShellLink,        /* existing IShellLink object */
	                                                &IID_IPersistFile, /* pre-defined interface of the IPersistFile object */
	                                                (LPVOID)&pPersistFile);
			if(SUCCEEDED(hRes)){
				iWideCharsWritten = MultiByteToWideChar(CP_ACP, 0, pszLink, -1, (wchar_t *)wszLinkfile, MAX_PATH);
				if(iWideCharsWritten){
					hRes = pPersistFile->lpVtbl->Save(pPersistFile, (wchar_t *)wszLinkfile, TRUE);
				}
				else
					hRes = -1;
				pPersistFile->lpVtbl->Release(pPersistFile);
			}
			pShellLink->lpVtbl->Release(pShellLink);
		}
	}
	CoUninitialize();
	return hRes;
}
