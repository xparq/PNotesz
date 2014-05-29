#ifndef __SPELLING_H__
#define __SPELLING_H__

#define	SPELLM_BASE					WM_APP + 1734
#define	SPM_GETCODEPAGE				SPELLM_BASE + 1

typedef struct _SUGGESTION *PSUGGESTION;
typedef struct _SUGGESTION {
	PSUGGESTION				next;
	wchar_t					*word;
	long					value;
}SUGGESTION;

typedef struct _SUGGARRAY {
	PSUGGESTION				*pSuggs;
	int						count;
	HWND					hEdit;
}SUGGARRAY, *PSUGGARRAY;

void CreateSpeller(char * affPath, char * dictPath, HWND hMain, COLORREF color);
void SetNewSpellColor(COLORREF clr);
void FreeSuggestions(PSUGGESTION * ps);
void AutoCheckRESpelling(HWND hEdit);
void AutomaticCheckRESpelling(HWND hEdit);
PSUGGESTION SuggestionsForRightClick(HWND hEdit, POINT pt);
void CheckRESpelling(HWND hEdit);
void DestroySpeller(void);
void AddWordToDictionary(wchar_t * wWord, BOOL addToCustom);

#endif	// SPELLING_H__
