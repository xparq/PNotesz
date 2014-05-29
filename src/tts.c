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

#define WIN32_LEAN_AND_MEAN

#ifndef	COBJMACROS
#define COBJMACROS
#endif

#include <sperror.h>
#include <wchar.h>
#include <sapi.h>
#include <objbase.h>

#include "shared.h"
// #include "tts.h"
#include "numericconstants.h"
#include "stringconstants.h"

#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))

static HRESULT _GetCategoryFromId(const wchar_t * pszCategoryId, ISpObjectTokenCategory ** ppCategory, BOOL fCreateIfNotExist);
static HRESULT _EnumTokens(const wchar_t * pszCategoryId, const wchar_t * pszReqAttribs, const wchar_t * pszOptAttribs, IEnumSpObjectTokens ** ppEnum);
static void _HexFromUlong(wchar_t * psz, ULONG ul);
static HRESULT _GetDescription(ISpObjectToken * pObjToken, wchar_t ** ppszDescription);
static LANGID _GetUserDefaultUILanguage(void);

static LANGID _GetUserDefaultUILanguage(void){
    LANGID 						wUILang = 0;
    OSVERSIONINFO 				Osv ;

    Osv.dwOSVersionInfoSize = sizeof(Osv) ;
    if(GetVersionEx(&Osv) && Osv.dwMajorVersion >= 5.0){
		//Windows 200 and later
		HMODULE 				hMKernel32 = LoadLibraryW(L"kernel32.dll") ;
        if (hMKernel32){
			LANGID (WINAPI *pfnGetUserDefaultUILanguage) () = (LANGID (WINAPI *)(void))GetProcAddress(hMKernel32, "GetUserDefaultUILanguage");
			if(NULL != pfnGetUserDefaultUILanguage){
                wUILang = pfnGetUserDefaultUILanguage() ;
            }
            FreeLibrary(hMKernel32);
		}
	}
	return (wUILang ? wUILang : GetUserDefaultLangID());
}

static HRESULT _GetCategoryFromId(const wchar_t * pszCategoryId, ISpObjectTokenCategory ** ppCategory, BOOL fCreateIfNotExist){
	HRESULT 					hr;
	ISpObjectTokenCategory		*cpTokenCategory = NULL;
	GUID						otgGuid = {0x2d3d3845,0x39af,0x4850,0xbb,0xf9,0x40,0xb4,0x97,0x80,0x01,0x1d};

	hr = CoCreateInstance(&CLSID_SpObjectTokenCategory, NULL, CLSCTX_ALL, &otgGuid, (void **)&cpTokenCategory);
	if(SUCCEEDED(hr)){
		hr = ISpObjectTokenCategory_SetId(cpTokenCategory, pszCategoryId, fCreateIfNotExist);
		if(SUCCEEDED(hr)){
			*ppCategory = cpTokenCategory;
			cpTokenCategory = NULL;
		}
		else{
			ISpObjectTokenCategory_Release(cpTokenCategory);
		}
	}
	return hr;
}

static HRESULT _EnumTokens(const wchar_t * pszCategoryId, const wchar_t * pszReqAttribs, const wchar_t * pszOptAttribs, IEnumSpObjectTokens ** ppEnum){
	HRESULT 					hr;
	ISpObjectTokenCategory		*cpCategory = NULL;

	hr = _GetCategoryFromId(pszCategoryId, &cpCategory, FALSE);
	if (SUCCEEDED(hr)){
		hr = ISpObjectTokenCategory_EnumTokens(cpCategory, pszReqAttribs, pszOptAttribs, ppEnum);
		ISpObjectTokenCategory_Release(cpCategory);
	}
	return hr;
}

static void _HexFromUlong(wchar_t * psz, ULONG ul)
{
    const static wchar_t szHexChars[] = L"0123456789ABCDEF";
    if (ul == 0)
    {
        psz[0] = L'0';
        psz[1] = 0;
    }
    else
    {
        ULONG ulChars = 1;
        psz[0] = 0;
        while (ul)
        {
            memmove(psz + 1, psz, ulChars * sizeof(wchar_t));
            psz[0] = szHexChars[ul % 16];
            ul /= 16;
            ulChars++;
        }
    }
}

static HRESULT _GetDescription(ISpObjectToken * pObjToken, wchar_t ** ppszDescription)
{
	LANGID 				languageID = _GetUserDefaultUILanguage();
    wchar_t 			szLangId[10];
	HRESULT 			hr = E_FAIL;

	//if(m_WinVer == 7){
		////Vista and above
	 	//wchar_t		* pRegKeyPath = 0;
	    //wchar_t		* pszTemp = 0;
	    //HKEY		Handle = NULL;
		//HMODULE 	hmodAdvapi32Dll = NULL;

		//if(ppszDescription == NULL){
	        //return E_POINTER;
	    //}
	    //*ppszDescription = NULL;
		
        //typedef HRESULT (WINAPI* LPFN_RegLoadMUIStringW)(HKEY, LPCWSTR, LPWSTR, DWORD, LPDWORD, DWORD, LPCWSTR);
        //LPFN_RegLoadMUIStringW pfnRegLoadMUIStringW = NULL;
		//// Delay bind with RegLoadMUIStringW since this function is not supported on previous versions of advapi32.dll
        //// RegLoadMUIStringW is supported only on advapi32.dll that ships with Windows Vista  and above
        //// Calling RegLoadMUIStringW directly makes the loader try to resolve the function reference at load time which breaks,
        //// hence we manually load advapi32.dll, query for the function pointer and invoke it.
        //hmodAdvapi32Dll = LoadLibrary("advapi32.dll");
		//if(hmodAdvapi32Dll)
        //{
            //pfnRegLoadMUIStringW = (LPFN_RegLoadMUIStringW)GetProcAddress(hmodAdvapi32Dll, "RegLoadMUIStringW");
            //if(!pfnRegLoadMUIStringW){
                //// This should not happen in Vista
                //hr = TYPE_E_DLLFUNCTIONNOTFOUND;
            //}
        //}
		//else{
            //hr = HRESULT_FROM_WIN32(ERROR_DLL_NOT_FOUND);
        //}
		//if(SUCCEEDED(hr)){
            //hr = ISpObjectToken_GetId(pObjToken, &pszTemp);
        //}
		//if(SUCCEEDED(hr)){
			//LONG   lErrorCode = ERROR_SUCCESS;

            //pRegKeyPath = wcschr(pszTemp, L'\\');   // Find the first occurance of '\\' in the absolute registry key path
			//if(pRegKeyPath){
				//*pRegKeyPath = L'\0';
                //pRegKeyPath++;                         // pRegKeyPath now points to the path to the recognizer token under the HKLM or HKCR hive
                //*ppszDescription = 0;

                //// Open the registry key for read and get the handle
                //if(wcsncmp(pszTemp, L"HKEY_LOCAL_MACHINE", MAX_PATH) == 0){
                    //lErrorCode = RegOpenKeyExW(HKEY_LOCAL_MACHINE, pRegKeyPath, 0, KEY_QUERY_VALUE, &Handle);
                //}
                //else if(wcsncmp(pszTemp, L"HKEY_CURRENT_USER", MAX_PATH) == 0){
                    //lErrorCode = RegOpenKeyExW(HKEY_CURRENT_USER, pRegKeyPath, 0, KEY_QUERY_VALUE, &Handle);
                //}
                //else{
                    //lErrorCode = ERROR_BAD_ARGUMENTS;
                //}
				//// Use MUI RegLoadMUIStringW API to load the localized string
                //if(ERROR_SUCCESS == lErrorCode){
                    //*ppszDescription = (wchar_t*)CoTaskMemAlloc(MAX_PATH * sizeof(wchar_t)); // This should be enough memory to allocate the localized Engine Name
                    //lErrorCode = (*pfnRegLoadMUIStringW) (Handle, SR_LOCALIZED_DESCRIPTION, *ppszDescription, MAX_PATH * sizeof(wchar_t), NULL, REG_MUI_STRING_TRUNCATE, NULL);
                //}
			//}
		//}
	//}


    _HexFromUlong(szLangId, languageID);
    hr = ISpObjectToken_GetStringValue(pObjToken, szLangId, ppszDescription);
    if (hr == SPERR_NOT_FOUND)
    {
        hr = ISpObjectToken_GetStringValue(pObjToken, NULL, ppszDescription);
    }
    return hr;
}

PPNVOICE GetVoice(PPNVOICE voices, wchar_t * name){
	for(int i = 0; i < 32; i++){
		if(wcscmp(voices[i].name, name) == 0){
			return &voices[i];
		}
	}
	return NULL;
}

PPNVOICE GetVoiceByToken(PPNVOICE voices, DWORD token){
	for(int i = 0; i < 32; i++){
		if((DWORD)voices[i].token == token){
			return &voices[i];
		}
	}
	return NULL;
}

HRESULT LoadVoices(void){
	HRESULT 					hr;
    ISpObjectToken 				*pToken;
	IEnumSpObjectTokens			*cpEnum;
	wchar_t						*pDescription, *p, *ptr, szBuffer[64];

	hr = _EnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);
	if(hr == S_OK){
		g_VoicesCount = 0;
		pDescription = (wchar_t *)CoTaskMemAlloc(128 * sizeof(wchar_t));
		while(IEnumSpObjectTokens_Next(cpEnum, 1, &pToken, NULL) == S_OK){
			hr = _GetDescription(pToken, &pDescription);
			if(SUCCEEDED(hr)){
				g_VoicesCount++;
				wcscpy(g_PVoices[g_VoicesCount - 1].name, pDescription);
				g_PVoices[g_VoicesCount - 1].token = pToken;
				GetPrivateProfileStringW(S_VOICES, pDescription, NULL, szBuffer, 64, g_NotePaths.INIFile);
				if(*szBuffer){
					p = wcstok(szBuffer, L"|", &ptr);
					g_PVoices[g_VoicesCount - 1].rate = _wtol(p);
					p = wcstok(NULL, L"|", &ptr);
					g_PVoices[g_VoicesCount - 1].volume = (unsigned short)_wtoi(p);
					g_PVoices[g_VoicesCount - 1].pitch = (short)_wtoi(ptr);
				}
				else{
					ISpVoice		*pVoice = CreateVoice();
					ISpVoice_SetVoice(pVoice, (ISpObjectToken *)g_PVoices[g_VoicesCount - 1].token);
					ISpVoice_GetVolume(pVoice, &g_PVoices[g_VoicesCount - 1].volume);
					ISpVoice_GetRate(pVoice, &g_PVoices[g_VoicesCount - 1].rate);
					g_PVoices[g_VoicesCount - 1].pitch = 0;
					ReleaseVoice(pVoice);
				}
				if(g_VoicesCount == NELEMS(g_PVoices))
					break;
			}
			if (FAILED(hr))
            {
                ISpObjectToken_Release(pToken);
				break;
            }
		}
		CoTaskMemFree(pDescription);
	}
	else
    {
        hr = SPERR_NO_MORE_ITEMS;
    }
    return hr;
}

void StopSpeak(void * pV){
	ISpVoice		*pVoice = pV;
	if(pVoice){
		ISpVoice_Speak(pVoice, NULL, SPF_PURGEBEFORESPEAK, NULL);
	}
}

void Speak(void * pV, const wchar_t * pwcs, void * pToken, PPNVOICE pVoices){
	ISpVoice		*pVoice = pV;
	PPNVOICE		voice = NULL;
	wchar_t			*pBuffer = NULL;

	pBuffer = calloc(wcslen(pwcs) + 256, sizeof(wchar_t));
	if(pVoice && pBuffer){
		if(pToken != NULL){
			ISpVoice_SetVoice(pVoice, (ISpObjectToken *)pToken);
		}
		voice = GetVoiceByToken(pVoices, (DWORD)pToken);
		if(voice){
			swprintf(pBuffer, wcslen(pwcs) + 256, L"<volume level = '%d'/><rate absspeed = '%d'/><pitch middle = '%d'/> %ls", voice->volume, voice->rate, voice->pitch, pwcs);
		}
		else{
			wcscat(pBuffer, pwcs);
		}
		ISpVoice_Speak(pVoice, pBuffer, SPF_ASYNC | SPF_PURGEBEFORESPEAK | SPF_IS_XML, NULL);
		free(pBuffer);
	}
}

void * CreateVoice(void){
	ISpVoice		*pVoice = NULL;
	if(CoCreateInstance(&CLSID_SpVoice, NULL, CLSCTX_ALL, &IID_ISpVoice, (void **)&pVoice) == S_OK){
		return pVoice;
	}
	return NULL;
}

void ReleaseVoice(void * pVoice){
	if(pVoice){
		ISpVoice_Release((ISpVoice *)pVoice);
		pVoice = NULL;
	}
}

void SetVoiceNotification(void * pV, HWND hwnd){
	HRESULT			hr;
	ISpVoice		*pVoice = pV;
	if(pVoice){
		hr = ISpVoice_SetInterest(pVoice, SPFEI(SPEI_END_INPUT_STREAM), SPFEI(SPEI_END_INPUT_STREAM));
		if(hr == S_OK){
			hr = ISpVoice_SetNotifyWindowMessage(pVoice, hwnd, PNM_SOUND_ENDS, 0, 0);
		}
	}
}

void RemoveVoiceNotification(void * pV){
	ISpVoice		*pVoice = pV;
	if(pVoice){
		ISpVoice_SetNotifySink(pVoice, NULL);
	}
}
