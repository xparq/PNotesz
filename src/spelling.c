// Copyright (C) 2008 Andrey Gruber (aka lamer)

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
#include <commctrl.h>
#include <stdio.h>
#include <wchar.h>
#include <richedit.h>
#include <richole.h>
#include <hunspell.h>
#include <tom.h>
//#include <guiddef.h>

#include "shared.h"
#include "global.h"

#define DELIMETERS 			" -'.,:;`/\"+(){}[]<>*&^%$#@!?~/|\\= \t\n\r"
#define DELIMITERSW			L" -'.,:;`/\"+(){}[]<>*&^%$#@!?~/|\\= \t\n\r"

#define	EDIT_PROP			L"EDIT_PROP"

//#define COBJMACROS
#ifdef DEFINE_GUID
#undef DEFINE_GUID
#endif
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID CDECL name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

DEFINE_GUID(IID_ITextDocument,0x8CC497C0,0xA1DF,0x11CE,0x80,0x98,
                0x00,0xAA,0x00,0x47,0xBE,0x5D);

static Hunhandle 			*m_hh;
static int					m_CodePage;
static HPEN					m_Pen;
static HWND					m_hMain;
static char					m_CustPath[MAX_PATH];

static void Spelling_OnClose(HWND hwnd);
static BOOL CALLBACK Spelling_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Spelling_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Spelling_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static HBRUSH Spelling_OnCtlColorListbox(HWND hwnd, HDC hdc, HWND hwndChild, int type);
static void _clearSuggestions(HWND hwnd3);
static PSUGGESTION _addSuggestion(PSUGGESTION * this, wchar_t * word, int value);
static void _freeSuggestions(PSUGGESTION * this);
static void _replaceOnce(HWND hwnd);
static void _replaceAll(HWND hwnd);
static void _addToDictionary(HWND hwnd);
static void _removeEntry(HWND hwnd, int index, int count);
static void _ignoreOnce(HWND hwnd);
static void _ignoreAll(HWND hwnd);
static void _replaceInEdit(HWND hwnd, HWND hEdit, PSUGGESTION ps, int index1, int index2, int count);
static void _shiftRange(HWND hwnd, int len1, int len2, int index, int count);
static char * _normalizeCodePage(char * s);
static char * _stripNoiseChars(char * s, char * strip);
static void _underscoreError(int start, int width, int top, HDC hdc);
static void _loadCustomDictionary(void);
static void _addToCustomDictionary(wchar_t * wWord);

void CreateSpeller(char * affPath, char * dictPath, HWND hMain, COLORREF color){
	char				*enc;

	m_hMain = hMain;
	if(m_hh){
		Hunspell_destroy(m_hh);
		m_hh = NULL;
	}
	m_hh = Hunspell_create(affPath, dictPath);

	enc = _strdup(Hunspell_get_dic_encoding(m_hh));
	enc = _normalizeCodePage(enc);	
	m_CodePage = SendMessage(hMain, SPM_GETCODEPAGE, 0, (LPARAM)enc);
	free(enc);

	if(m_Pen){
		DeleteObject(m_Pen);
		m_Pen = NULL;
	}
	m_Pen = CreatePen(PS_SOLID, 1, color);
	strcpy(m_CustPath, affPath);
	PathRemoveFileSpec(m_CustPath);
	strcat(m_CustPath, "\\");
	_loadCustomDictionary();
}

void SetNewSpellColor(COLORREF clr){
	if(m_Pen){
		DeleteObject(m_Pen);
		m_Pen = NULL;
	}
	m_Pen = CreatePen(PS_SOLID, 1, clr);
}

void FreeSuggestions(PSUGGESTION * ps){
	_freeSuggestions(ps);
}

void AutoCheckRESpelling(HWND hEdit){
	HDC					hdc;
	long				firstChar, charPos, lineIndex;
	char				*word;
	RECT				rc = {0}, rcReal = {0}, rcText = {0};
	HRGN				hRgn, hOldRgn;
	TEXTRANGEW			trg = {0};
	CHARRANGE			chrg = {0};
	wchar_t				buffer[1024 * 10], *pString = NULL, *pText = NULL, *pWord = NULL;
	int					bufferSize, counter, len;

	hdc = GetDC(hEdit);
	SetBkMode(hdc, TRANSPARENT);

	HRESULT			hr;
	ITextDocument	*pTextDocument;
	ITextRange		*pTextRange;

	LPRICHEDITOLE 	pREOle = NULL;
	SendMessageW(hEdit, EM_GETOLEINTERFACE, 0, (LPARAM)&pREOle);
	if(pREOle == NULL)
		return;
	hr = pREOle->lpVtbl->QueryInterface(pREOle, &IID_ITextDocument, (void**)&pTextDocument);
	if(!SUCCEEDED(hr)){
		return;
	}
	//get current selection
	SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&chrg);
	//get textbox rectangle
	SendMessageW(hEdit, EM_GETRECT, 0, (LPARAM)&rc);
	CopyRect(&rcText, &rc);
	//get first visible character
	charPos = SendMessageW(hEdit, EM_CHARFROMPOS, 0, (LPARAM)&rc);
	//obtain line number
	lineIndex = SendMessageW(hEdit, EM_EXLINEFROMCHAR, 0, charPos);
	firstChar = SendMessageW(hEdit, EM_LINEINDEX, lineIndex, 0);
	trg.chrg.cpMin = firstChar;
	//get last visible character
	rc.left = rc.right;
	rc.top = rc.bottom;
	trg.chrg.cpMax = SendMessageW(hEdit, EM_CHARFROMPOS, 0, (LPARAM)&rc);
	CopyRect(&rcReal, &rcText);
	hRgn = CreateRectRgnIndirect(&rcReal);
	hOldRgn = SelectObject(hdc, hRgn);
	//get the visible text into buffer
	trg.lpstrText = buffer;
	counter = bufferSize = SendMessageW(hEdit, EM_GETTEXTRANGE, 0, (LPARAM)&trg);
	if(bufferSize > 0){
		pString = _wcsdup(buffer);
		pWord = pString;
		while(counter > 0){
			if(wcschr(DELIMITERSW, *pWord))
				*pWord = '\0';
			pWord++;
			counter--;
		}
		counter = bufferSize;
		pText = pString;
		while(counter > 0){
			if(*pText){
				len = wcslen(pText) + 1;
				charPos = (bufferSize - counter) + firstChar;

				if(chrg.cpMin >= charPos && chrg.cpMax <= charPos + (len - 1)){
					//do nothing
				}
				else{
					word = calloc(len, sizeof(char));
					WideCharToMultiByte(m_CodePage, 0, pText, -1, word, len, NULL, NULL);
					if(Hunspell_spell(m_hh, word) == 0){
						hr = pTextDocument->lpVtbl->Range(pTextDocument, charPos, charPos + (len - 2), &pTextRange );
						if(SUCCEEDED(hr)){
							POINT		pts[2] = {{0}, {0}};
							hr = pTextRange->lpVtbl->GetPoint(pTextRange, 32 | TA_BASELINE | TA_LEFT, &pts[0].x, &pts[0].y);
							if(SUCCEEDED(hr)){
								hr = pTextRange->lpVtbl->GetPoint(pTextRange, 0 | TA_BASELINE | TA_RIGHT, &pts[1].x, &pts[1].y);
								if(SUCCEEDED(hr)){
									MapWindowPoints(HWND_DESKTOP, hEdit, pts, 2);
									_underscoreError(pts[0].x, pts[1].x - pts[0].x, pts[0].y, hdc);
								}
							}
						}
					}
					free(word);
				}
				pText += (len - 1);
				counter -= (len - 1);
			}
			pText++;
			counter--;
		}
		free(pString);
	}
	SelectObject(hdc, hOldRgn);
	DeleteObject(hRgn);
	ReleaseDC(hEdit, hdc);
}

PSUGGESTION SuggestionsForRightClick(HWND hEdit, POINT pt){
	TEXTRANGEW			trg = {0}, trWord = {0};
	wchar_t				*wpText = NULL, *wpt = NULL, *pStr;
	GETTEXTLENGTHEX		gtx;
	long				i1, i2;
	char				*w = NULL, **arr;
	int					result, countSuggs = 0, len;
	LRESULT				selChar, charCount;
	PSUGGESTION			ps = NULL;

	__try{
		gtx.flags = GTL_NUMCHARS | GTL_PRECISE;
		gtx.codepage = 1200;
		charCount = SendMessageW(hEdit, EM_GETTEXTLENGTHEX, (WPARAM)&gtx, 0);
		if(charCount == 0)
			return NULL;

		wpText = calloc(charCount + 1, sizeof(wchar_t));
		trg.chrg.cpMin = 0;
		trg.chrg.cpMax = charCount + 1;
		trg.lpstrText = wpText;
		SendMessageW(hEdit, EM_GETTEXTRANGE, 0, (LPARAM)&trg);

		selChar = SendMessageW(hEdit, EM_CHARFROMPOS, 0, (LPARAM)&pt);
		if(wcschr(DELIMITERSW, wpText[selChar]))
			return NULL;

		i1 = i2 = selChar;
		while(TRUE){
			if((i1 == 0 && wcschr(DELIMITERSW, wpText[i2])) || (i1 == 0 && i2 == charCount - 1) || (wcschr(DELIMITERSW, wpText[i1]) && wcschr(DELIMITERSW, wpText[i2])) || (wcschr(DELIMITERSW, wpText[i1]) && i2 == charCount - 1)){
				break;
			}
			if(i1 > 0 && !wcschr(DELIMITERSW, wpText[i1]))
				i1--;
			if(i2 < charCount - 1 && !wcschr(DELIMITERSW, wpText[i2]))
				i2++;
		}
		
		if(i2 == charCount - 1 && !wcschr(DELIMITERSW, wpText[i2])){
			i2++;
		}

		wpt = calloc(i2 - i1 + 1, sizeof(wchar_t));
		trWord.chrg.cpMin = (i1 > 0) ? (i1 + 1) : (wcschr(DELIMITERSW, wpText[i1]) ? 1 : 0);
		trWord.chrg.cpMax = (i2 == charCount) ? ((wcschr(DELIMITERSW, wpText[i2]) && wpText[i2] != '\0') ? i2 - 1 : i2) : i2;// < charCount - 1 ? i2 - 1 : i2;
		trWord.lpstrText = wpt;
		SendMessageW(hEdit, EM_GETTEXTRANGE, 0, (LPARAM)&trWord);
		len = i2 - i1 + 1;
		w = calloc(len, sizeof(char));
		WideCharToMultiByte(m_CodePage, 0, wpt, -1, w, len - 1, NULL, NULL);

		result = Hunspell_spell(m_hh, w);
		if(result == 0){
			_addSuggestion(&ps, wpt, trWord.chrg.cpMin);
			countSuggs = Hunspell_suggest(m_hh, &arr, w);
			if(countSuggs > 0){
				for(int i = 0; i < countSuggs; i++){
					len = strlen(arr[i]) + 1;
					pStr = calloc(len, sizeof(wchar_t));
					MultiByteToWideChar(m_CodePage, 0, arr[i], -1, pStr, len);
					_addSuggestion(&ps, pStr, len);
					free(pStr);
				}
				Hunspell_free_list(m_hh, &arr, countSuggs);
			}
		}
		return ps;
	}
	__finally{
		if(wpText)
			free(wpText);
		if(wpt)
			free(wpt);
		if(w)
			free(w);
	}
	return NULL;
}

void CheckRESpelling(HWND hEdit){
	TEXTRANGEW			trg = {0};
	wchar_t				*wpText = NULL, *wpt, *pw, *ptrW, *wpos;
	GETTEXTLENGTHEX		gtx;
	long				charCount;
	char				*pText = NULL, *p, *w, **arr;
	int					result, countSuggs = 0, len, findRes = 0;
	SUGGARRAY			suggs = {0};
	FINDTEXTEXW			ft = {0};

	__try{
		SetCursor(LoadCursor(NULL, IDC_WAIT));
		suggs.hEdit = hEdit;
		
		gtx.flags = GTL_NUMCHARS | GTL_PRECISE;
		gtx.codepage = 1200;
		charCount = SendMessageW(hEdit, EM_GETTEXTLENGTHEX, (WPARAM)&gtx, 0);
		wpText = calloc(charCount + 1, sizeof(wchar_t));

		trg.chrg.cpMin = 0;
		trg.chrg.cpMax = charCount + 1;
		trg.lpstrText = wpText;
		SendMessageW(hEdit, EM_GETTEXTRANGE, 0, (LPARAM)&trg);

		ft.chrg = trg.chrg;

		pText = calloc(charCount + 1, sizeof(char));
		WideCharToMultiByte(m_CodePage, 0, wpText, -1, pText, charCount, NULL, NULL);

		p = pText;
		pw = wpText;
		w = strtok(p, DELIMETERS);
		wpos = wcstok(pw, DELIMITERSW, &ptrW);
		while(w){
			result = Hunspell_spell(m_hh, w);
			if(result == 0){
				len = strlen(w) + 1;
				wpt = calloc(len, sizeof(wchar_t));
				MultiByteToWideChar(m_CodePage, 0, w, -1, wpt, len - 1);
				if(suggs.count == 0){
					suggs.pSuggs = calloc(1, sizeof(PSUGGESTION));
				}
				else{
					suggs.pSuggs = realloc(suggs.pSuggs, sizeof(PSUGGESTION) * (suggs.count + 1));
					suggs.pSuggs[suggs.count] = NULL;
				}
				
				ft.lpstrText = wpos;
				findRes = SendMessageW(hEdit, EM_FINDTEXTEXW, FR_DOWN | FR_MATCHCASE | FR_WHOLEWORD, (LPARAM)&ft);

				_addSuggestion(&suggs.pSuggs[suggs.count], wpt, findRes);
				
				ft.chrg.cpMin = findRes + wcslen(wpos);

				free(wpt);
				countSuggs = Hunspell_suggest(m_hh, &arr, w);
				if(countSuggs > 0){
					for(int i = 0; i < countSuggs; i++){
						len = strlen(arr[i]) + 1;
						wpt = calloc(len, sizeof(wchar_t));
						MultiByteToWideChar(m_CodePage, 0, arr[i], -1, wpt, len - 1);
						_addSuggestion(&suggs.pSuggs[suggs.count], wpt, len);
						free(wpt);
					}
					Hunspell_free_list(m_hh, &arr, countSuggs);
				}
				suggs.count++;
			}
			w = strtok(NULL, DELIMETERS);
			wpos = wcstok(NULL, DELIMITERSW, &ptrW);
		}
	}
	__finally{
		if(pText)
			free(pText);
		if(wpText)
			free(wpText);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	if(suggs.count > 0){
		DialogBoxParamW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(DLG_SPELLING), GetParent(hEdit), Spelling_DlgProc, (LPARAM)&suggs);
		free(suggs.pSuggs);
	}
	else{
		wchar_t			szBuffer[512];
		GetPrivateProfileStringW(L"options", L"1052", L"Spell checking", szBuffer, 256, g_NotePaths.CurrLanguagePath);
		MessageBoxW(GetParent(hEdit), g_Strings.SpellCheckComplete, szBuffer, MB_OK);
	}
}

void DestroySpeller(void){
	if(m_hh){
		Hunspell_destroy(m_hh);
		m_hh = NULL;
	}
}

static BOOL CALLBACK Spelling_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_INITDIALOG, Spelling_OnInitDialog);
		HANDLE_MSG (hwnd, WM_CLOSE, Spelling_OnClose);
		HANDLE_MSG (hwnd, WM_COMMAND, Spelling_OnCommand);
		HANDLE_MSG (hwnd, WM_CTLCOLORLISTBOX, Spelling_OnCtlColorListbox);
		default: return FALSE;
	}
}

static HBRUSH Spelling_OnCtlColorListbox(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
	if(hwndChild == GetDlgItem(hwnd, IDC_LST_MISPRINTS))
		SetTextColor(hdc, RGB(255, 0, 0));
	return GetSysColorBrush(COLOR_WINDOW);
}

static void Spelling_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDC_LST_MISPRINTS:
			if(codeNotify == LBN_SELCHANGE){
				HWND	hEdit = (HWND)GetPropW(hwnd, EDIT_PROP);
				SendDlgItemMessageW(hwnd, IDC_LST_SUGGESTIONS, LB_RESETCONTENT, 0, 0);
				int		index = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETCURSEL, 0, 0);
				if(index > LB_ERR){
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_CHANGE_ONCE), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_CHANGE_ALL), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_IGNORE_ALL), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_IGNORE_ONCE), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_ADD_TO_DICT), TRUE);

					PSUGGESTION		ps = (PSUGGESTION)SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETITEMDATA, index, 0);
					if(ps){
						CHARRANGE	chr;
						chr.cpMin = ps->value;
						chr.cpMax = chr.cpMin + wcslen(ps->word);
						SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&chr);
						for(ps = ps->next; ps; ps = ps->next){
							SendDlgItemMessageW(hwnd, IDC_LST_SUGGESTIONS, LB_ADDSTRING, 0, (LPARAM)ps->word);
						}
					}
				}
				else{
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_IGNORE_ALL), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_IGNORE_ONCE), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_ADD_TO_DICT), FALSE);
				}
			}
			break;
		case IDC_LST_SUGGESTIONS:
			if(codeNotify == LBN_SELCHANGE){
				int		index = SendDlgItemMessageW(hwnd, IDC_LST_SUGGESTIONS, LB_GETCURSEL, 0, 0);
				if(index != LB_ERR){
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_CHANGE_ONCE), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_CHANGE_ALL), TRUE);
				}
				else{
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_CHANGE_ONCE), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_CHANGE_ALL), FALSE);
				}
			}
			break;
		case IDC_CMD_CHANGE_ONCE:
			_replaceOnce(hwnd);
			break;
		case IDC_CMD_CHANGE_ALL:
			_replaceAll(hwnd);
			break;
		case IDC_CMD_IGNORE_ONCE:
			_ignoreOnce(hwnd);
			break;
		case IDC_CMD_IGNORE_ALL:
			_ignoreAll(hwnd);
			break;
		case IDC_CMD_ADD_TO_DICT:
			_addToDictionary(hwnd);
			break;
		case IDCANCEL:
			_clearSuggestions(hwnd);
			g_hSpellChecking = NULL;
			EndDialog(hwnd, IDCANCEL);
			break;
	}
}

static void Spelling_OnClose(HWND hwnd)
{
	_clearSuggestions(hwnd);
	g_hSpellChecking = NULL;
	EndDialog(hwnd, IDCANCEL);
}

static BOOL Spelling_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	PSUGGARRAY		psa = (PSUGGARRAY)lParam;
	int				index;
	wchar_t			szBuffer[512];

	g_hSpellChecking = hwnd;
	if(psa->pSuggs){
		SetPropW(hwnd, EDIT_PROP, (HANDLE)psa->hEdit);
		for(int i = 0; i < psa->count; i++){
			index = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_ADDSTRING, 0, (LPARAM)psa->pSuggs[i]->word);
			SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_SETITEMDATA, index, (LPARAM)psa->pSuggs[i]);
		}
	}
	GetPrivateProfileStringW(L"options", L"1052", L"Spell checking", szBuffer, 256, g_NotePaths.CurrLanguagePath);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDCANCEL, g_NotePaths.CurrLanguagePath, L"Cancel");
	SetDlgCtlText(hwnd, IDC_ST_NOT_IN_DICT, g_NotePaths.CurrLanguagePath, L"Not in dictionary");
	SetDlgCtlText(hwnd, IDC_ST_SUGGESTIONS, g_NotePaths.CurrLanguagePath, L"Suggestions");
	SetDlgCtlText(hwnd, IDC_CMD_IGNORE_ONCE, g_NotePaths.CurrLanguagePath, L"Ignore once");
	SetDlgCtlText(hwnd, IDC_CMD_IGNORE_ALL, g_NotePaths.CurrLanguagePath, L"Ignore all");
	SetDlgCtlText(hwnd, IDC_CMD_ADD_TO_DICT, g_NotePaths.CurrLanguagePath, L"Add to dictionary");
	SetDlgCtlText(hwnd, IDC_CMD_CHANGE_ONCE, g_NotePaths.CurrLanguagePath, L"Change");
	SetDlgCtlText(hwnd, IDC_CMD_CHANGE_ALL, g_NotePaths.CurrLanguagePath, L"Change all");

	return TRUE;
}

static void _replaceAll(HWND hwnd){
	int				index1, index2, len, count;
	PSUGGESTION		ps;
	wchar_t			*pText, *pWord;
	BOOL			fFound;

	index1 = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETCURSEL, 0, 0);
	index2 = SendDlgItemMessageW(hwnd, IDC_LST_SUGGESTIONS, LB_GETCURSEL, 0, 0);
	if(index1 == LB_ERR || index2 == LB_ERR)
		return;
	ps = (PSUGGESTION)SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETITEMDATA, index1, 0);
	pWord = _wcsdup(ps->word);
	while(TRUE){
		fFound = FALSE;
		count = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETCOUNT, 0, 0);
		for(int i = 0; i < count; i++){
			len = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETTEXTLEN, i, 0);
			pText = calloc(len + 1, sizeof(wchar_t));
			SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETTEXT, i, (LPARAM)pText);
			if(wcscmp(pText, pWord) == 0){
				SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_SETCURSEL, i, 0);
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_LST_MISPRINTS, LBN_SELCHANGE), (LPARAM)GetDlgItem(hwnd, IDC_LST_MISPRINTS));
				SendDlgItemMessageW(hwnd, IDC_LST_SUGGESTIONS, LB_SETCURSEL, index2, 0);
				_replaceOnce(hwnd);
				fFound = TRUE;
				break;
			}
			free(pText);
		}
		if(!fFound)
			break;
	}
	free(pWord);
}

void AddWordToDictionary(wchar_t * wWord, BOOL addToCustom){
	int				len;
	char			*word;

	len = wcslen(wWord) + 1;
	word = calloc(len, sizeof(char));
	WideCharToMultiByte(m_CodePage, 0, wWord, -1, word, len, NULL, NULL);
	Hunspell_add(m_hh, word);
	free(word);
	if(addToCustom){
		_addToCustomDictionary(wWord);
	}
}

static void _addToCustomDictionary(wchar_t * wWord){
	char			szFileName[MAX_PATH];
	char			szTemp[64];
	HANDLE			hFile;
	int				size = 0, bytesToWrite = 0;
	wchar_t			*pBuffer = NULL;
	DWORD			dwBytesWritten = 0;

	_itoa(m_CodePage, szTemp, 10);
	strcat(szTemp, ".cust");
	strcpy(szFileName, m_CustPath);
	strcat(szFileName, szTemp);
	hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		size = GetFileSize(hFile, NULL);
		SetFilePointer(hFile, 0, NULL, FILE_END);
		if(size == 0){
			bytesToWrite = wcslen(wWord) + 2;
		}
		else{
			bytesToWrite = wcslen(wWord) + 1;	
		}	
		pBuffer = calloc(bytesToWrite, sizeof(wchar_t));
		bytesToWrite *= sizeof(wchar_t);
		if(pBuffer){
			if(size == 0){
				wcscpy(pBuffer, L"^");
			}
			wcscat(pBuffer, wWord);
			wcscat(pBuffer, L"^");
			WriteFile(hFile, pBuffer, bytesToWrite, &dwBytesWritten, NULL);
			free(pBuffer);
		}
		CloseHandle(hFile);
	}
}

static void _loadCustomDictionary(void){
	char			szFileName[MAX_PATH];
	char			szTemp[64];
	HANDLE			hFile;
	int				size = 0;

	_itoa(m_CodePage, szTemp, 10);
	strcat(szTemp, ".cust");
	strcpy(szFileName, m_CustPath);
	strcat(szFileName, szTemp);
	hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		size = GetFileSize(hFile, NULL);
		if(size > 0){
			wchar_t	*pWords = calloc(size, sizeof(wchar_t));
			if(pWords){
				wchar_t	*pTemp = pWords;
				DWORD		dwBytesRead = 0;
				ReadFile(hFile, pTemp, size, &dwBytesRead, NULL);
				if(dwBytesRead == size){
					wchar_t		*ptr;
					wchar_t *word = wcstok(pTemp, L"^", &ptr);
					while(word){
						if(wcslen(word) > 0){
							AddWordToDictionary(word, FALSE);
						}
						word = wcstok(NULL, L"^", &ptr);
					}
				}
				free(pWords);
			}
		}
		CloseHandle(hFile);
	}
}

static void _addToDictionary(HWND hwnd){
	int				index, len;
	HWND			hEdit = (HWND)GetPropW(hwnd, EDIT_PROP);
	PSUGGESTION		ps;
	char			*word;

	index = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETCURSEL, 0, 0);
	if(index == LB_ERR)
		return;
	ps = (PSUGGESTION)SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETITEMDATA, index, 0);
	len = wcslen(ps->word) + 1;
	word = calloc(len, sizeof(char));
	WideCharToMultiByte(m_CodePage, 0, ps->word, -1, word, len, NULL, NULL);
	Hunspell_add(m_hh, word);
	_addToCustomDictionary(ps->word);
	RedrawWindow(hEdit, NULL, NULL, RDW_INVALIDATE);
	free(word);
	_ignoreAll(hwnd);
}

static void _replaceOnce(HWND hwnd){
	int				index1, index2, count;
	HWND			hEdit = (HWND)GetPropW(hwnd, EDIT_PROP);
	PSUGGESTION		ps;

	count = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETCOUNT, 0, 0);
	index1 = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETCURSEL, 0, 0);
	index2 = SendDlgItemMessageW(hwnd, IDC_LST_SUGGESTIONS, LB_GETCURSEL, 0, 0);
	if(index1 == LB_ERR || index2 == LB_ERR)
		return;
	ps = (PSUGGESTION)SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETITEMDATA, index1, 0);
	_replaceInEdit(hwnd, hEdit, ps, index1, index2, count);
	_removeEntry(hwnd, index1, count);
}

static void _shiftRange(HWND hwnd, int len1, int len2, int index, int count){
	PSUGGESTION		ps;

	if(len1 != len2){
		for(int i = index + 1; i < count; i++){
			ps = (PSUGGESTION)SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETITEMDATA, i, 0);
			ps->value += (len2 - len1);
		}
	}
}

static void _replaceInEdit(HWND hwnd, HWND hEdit, PSUGGESTION ps, int index1, int index2, int count){
	int				len1, len2;
	wchar_t			*pText;
	CHARRANGE		chr;

	len1 = wcslen(ps->word);
	chr.cpMin = ps->value;
	chr.cpMax = chr.cpMin + len1;
	SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&chr);
	len2 = SendDlgItemMessageW(hwnd, IDC_LST_SUGGESTIONS, LB_GETTEXTLEN, index2, 0);
	pText = calloc(len2 + 1, sizeof(wchar_t));
	SendDlgItemMessageW(hwnd, IDC_LST_SUGGESTIONS, LB_GETTEXT, index2, (LPARAM)pText);
	SendMessageW(hEdit, EM_REPLACESEL, FALSE, (LPARAM)pText);
	free(pText);
	_shiftRange(hwnd, len1, len2, index1, count);
}

static void _ignoreAll(HWND hwnd){
	int				index, count, len;
	wchar_t			*pText, *pWord;
	BOOL			fFound = FALSE;
	PSUGGESTION		ps;
	
	index = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETCURSEL, 0, 0);
	ps = (PSUGGESTION)SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETITEMDATA, index, 0);
	pWord = _wcsdup(ps->word);
	while(TRUE){
		count = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETCOUNT, 0, 0);
		fFound = FALSE;
		for(int i = 0; i < count; i++){
			len = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETTEXTLEN, i, 0);
			pText = calloc(len + 1, sizeof(wchar_t));
			SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETTEXT, i, (LPARAM)pText);
			if(wcscmp(pText, pWord) == 0){
				SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_SETCURSEL, i, 0);
				_ignoreOnce(hwnd);
				fFound = TRUE;
			}
			free(pText);
			if(fFound)
				break;
		}
		if(!fFound)
			break;
	}
	free(pWord);
}

static void _ignoreOnce(HWND hwnd){
	int				index, count;

	index = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETCURSEL, 0, 0);
	count = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETCOUNT, 0, 0);
	if(index == LB_ERR)
		return;
	_removeEntry(hwnd, index, count);
}

static void _removeEntry(HWND hwnd, int index, int count){
	PSUGGESTION		ps;
	
	ps = (PSUGGESTION)SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETITEMDATA, index, 0);
	_freeSuggestions(&ps);
	SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_DELETESTRING, index, 0);
	if(count > 1){
		if(index == count - 1)
			index = 0;
		SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_SETCURSEL, index, 0);
		SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_LST_MISPRINTS, LBN_SELCHANGE), (LPARAM)GetDlgItem(hwnd, IDC_LST_MISPRINTS));
	}
	else{
		SendDlgItemMessageW(hwnd, IDC_LST_SUGGESTIONS, LB_RESETCONTENT, 0, 0);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_CHANGE_ONCE), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_CHANGE_ALL), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_IGNORE_ONCE), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_IGNORE_ALL), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_ADD_TO_DICT), FALSE);
	}
}

static void _clearSuggestions(HWND hwnd){
	int				count = SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETCOUNT, 0, 0);

	for(int i = 0; i < count; i++){
		PSUGGESTION		ps = (PSUGGESTION)SendDlgItemMessageW(hwnd, IDC_LST_MISPRINTS, LB_GETITEMDATA, i, 0);
		_freeSuggestions(&ps);
	}
	RemovePropW(hwnd, EDIT_PROP);
}

static PSUGGESTION _addSuggestion(PSUGGESTION * this, wchar_t * word, int value){
	PSUGGESTION		ps = malloc(sizeof(SUGGESTION)), p;
	
	ps->word = _wcsdup(word);
	ps->value = value;
	ps->next = NULL;
	if(*this == NULL){
		*this = ps;
	}
	else{
		p = *this;
		while(p->next)
			p = p->next;
		p->next = ps;	
	}
	return *this;
}

static void _freeSuggestions(PSUGGESTION * this){
	PSUGGESTION		p = *this, t;
	while(p){
		t = p->next;
		free(p->word);
		free(p);
		p = t;
	}
	*this = NULL;
}

static char * _normalizeCodePage(char * s){
	char *t;
	s = _stripNoiseChars(s, " -");
	t = s;
	while(*t){
		*t = tolower(*t);
		t++;
	}
	return s;
}

static char * _stripNoiseChars(char * s, char * strip){
	char *t = malloc(strlen(s) + 1);;
	char *p1 = s, *p2 = t;

	while(*s){
		if(!strchr(strip, *s))
			*t++ = *s++;
		else
			s++;
	}
	*t = '\0';
	strcpy(p1,  p2);
	free(t);
	return p1;
}

static void _underscoreError(int start, int width, int top, HDC hdc){
	HPEN		hOldPen;
	int			y = top, count = (width / 4) * 2 + 1;
	LPPOINT		ppt = malloc(count * sizeof(POINT));

	hOldPen = SelectObject(hdc, m_Pen);
	for(int i = 0; i < count; i++){
		ppt[i].x = start + 2 * i;
		if(i % 2 == 0)
			ppt[i].y = y + 2;
		else
			ppt[i].y = y;
	}
	Polyline(hdc, ppt, count);
	SelectObject(hdc, hOldPen);
	free(ppt);
}
