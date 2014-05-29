#ifndef __NOTE_H__
#define __NOTE_H__

#include "structures.h"
#include "menu.h"

BOOL RegisterNoteClass(HINSTANCE hInstance);
void LoadNewDiaryNoteProperties(PMEMNOTE pNote, wchar_t ** formats);
void LoadNoteProperties(PMEMNOTE pNote, P_NOTE_RTHANDLES pHandles, wchar_t * lpID, BOOL loadNew);
HWND CreateNote(PMEMNOTE pNote, HINSTANCE hInstance, BOOL loadNew, wchar_t * lpFile);
void SetNoteMIText(int id, wchar_t * lpText);
void SetEditMIText(int id, wchar_t * lpText);
void SetScheduleForPrevious(HWND hwnd, wchar_t * lpID, wchar_t * lpFile);
void AddColorsMenu(void);
void SetNoteMIImageIndex(int id, int index);
PMITEM NoteMenu(void);
PMITEM EditMenu(void);
PMITEM DropMenu(void);
int NoteMenuCount(void);
int EditMenuCount(void);
int DropMenuCount(void);
BOOL SaveNote(PMEMNOTE pNote);
void SaveNoteAs(PMEMNOTE pNote, int nAsOrRename);
void AutosaveNote(PMEMNOTE pNote);
void SaveVisibility(HWND hwnd, BOOL visible);
void DeleteRTFile(wchar_t * id);
int SetRepeatCount(LPSYSTEMTIME lst);
void CALLBACK NoteTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
BOOL CALLBACK SaveAs_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Adjust_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Tags_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void CenterRestorePlacementNote(PMEMNOTE pNote, int operation, BOOL leaveAtPosition);
void AddToFavorites(PMEMNOTE pNote);
void DeleteNoteCompletely(PMEMNOTE pNote);
void RollNote(PMEMNOTE pNote);
void UnrollNote(PMEMNOTE pNote);
void ShowNoteMarks(HWND hwnd);
void FitToCaption(PMEMNOTE pNote, BOOL copyCaptions);
void DuplicateNote(PMEMNOTE pSrc);
void SaveTags(PMEMNOTE pNote);
void ExchangeSameRE(HWND hEdit);
BOOL IsHotkeyAvailable(PMEMNOTE pNote, int command);
void SetREFontCommon(HWND hEdit, LPLOGFONTW lf);
void SaveNoteAsTextFile(PMEMNOTE pNote);
int GetTemporarySaveName(HWND hNote, wchar_t * lpName, int nMax);
int GetFirstNoteCharacters(HWND hNote, wchar_t * lpBuffer, int nMax);
void FormatNewNoteID(wchar_t * lpID);
void AddPin(HWND hParent, PMEMNOTE pNote);
void RemovePin(PMEMNOTE pNote);
// void ApplyTabStops(HWND hEdit, BOOL invalidate);
#endif	// NOTE_H__
