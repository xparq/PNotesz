#ifndef __MEMORYNOTES_H__
#define __MEMORYNOTES_H__

#include "structures.h"
#include "groups.h"

#define	MEMNOTE(hwnd)	((PMEMNOTE)GetWindowLongPtrW(hwnd, OFFSET_MNOTE))

BOOL IsAnyNoteVisible(void);
PMEMNOTE MemoryNotes(void);
PMEMNOTE AddMemNote(void);
void DuplicateMemNote(PMEMNOTE pSrc, PMEMNOTE pDest);
void FreeMemNotes(void);
PMEMNOTE MemNoteByHwnd(HWND hwnd);
PMEMNOTE MemNoteById(wchar_t * lpId);
P_NOTE_APPEARANCE NoteAppearance(HWND hwnd);
P_NOTE_RTHANDLES NoteRTHandles(HWND hwnd);
P_NOTE_RTHANDLES NoteSavedHandles(HWND hwnd);
P_NOTE_FLAGS NoteFlags(HWND hwnd);
P_NOTE_DATA NoteData(HWND hwnd);
LPSRSTATUS NoteSRStatus(HWND hwnd);
P_SCHEDULE_TYPE NoteSchedule(HWND hwnd);
LPSYSTEMTIME NoteCreationTime(HWND hwnd);
LPSYSTEMTIME NoteDeletionTime(HWND hwnd);
LPSYSTEMTIME NoteRealDeletionTime(HWND hwnd);
PMEMNOTE MemNoteByTimer(UINT_PTR idEvent);
PMEMNOTE MemNoteByPinTimer(UINT_PTR idEvent);
void DeleteMemNote(PMEMNOTE pNote);
void ApplyNewDiaryBackgroundColor(void);
void ApplyNewDiaryCaptionColor(void);
void ApplyNewDiaryCaptionFont(void);
void ApplyNewDiarySkin(void);
void ApplyNewMargins(void);
void ApplyNewDockSkin(void);
void ApplyNewCommonSkin(void);
void ApplyNewBackgroundColor(void);
void ApplyNewCaptionFont(void);
void ApplyNewCaptionColor(void);
void ApplyNewCaptionColorForGroup(LPPNGROUP ppg);
void ApplyNewFont(void);
void ApplyNewTextColor(void);
void ApplyNewTextColorForGroup(LPPNGROUP ppg);
void ApplyJustRedraw(void);
void ApplyNewLanguage(void);
void ApplyTransparencyValue(void);
void ApplyRestrictTransparency(void);
void ApplySaveOnUnload(int args, BOOL beforeReload);
void ApplyAutosaveNotes(void);
void ApplyNewBackgroundColorForGroup(LPPNGROUP ppg);
// void ApplyTabStopsAllNotes(void);
void ApplySaveAllNotes(void);
void ApplyCloseAllNotes(void);
void ApplyOnTopChanged(void);
void ApplyDockSizeOrderChanged(void);
void ApplyDockingColors(void);
void ApplySessionLogOn(void);
void ApplyHideAllNotes(void);
void ApplyRedrawEdit(void);
void DeleteToBin(PMEMNOTE pNote, int withGroup);
void EmptyRecycleBin(void);
int NotesCount(void);
int CountNotesInGroup(int id);
int CountNotesAlive(void);
int CountFavorites(void);
void RestoreNoteFromBin(PMEMNOTE pNote, int toGroup);
void HideNote(PMEMNOTE pNote, BOOL fromPin);
void ShowNote(PMEMNOTE pNote);
void SendNoteAsAttachment(PMEMNOTE pNote);
void ShowGroupOfNotes(int group);
void HideGroupOfNotes(int group);
void SavePNote(PMEMNOTE pNote);
void ToggleNoteCompleted(PMEMNOTE pNote);
void ToggleNotePriority(PMEMNOTE pNote);
void ToggleNoteOnTop(PMEMNOTE pNote);
void ToggleNoteProtection(PMEMNOTE pNote);
void ToggleAeroPeek(BOOL fStatus);
void SendMultipleNotesAsAttachments(LONG_PTR *pNotes, int count);
void BringNotesToFront(void);
void ExecuteNoteHotkey(int command);
void ClearSyncStatus(void);
void SaveNotesBeforeSync(void);
void ClearLocalSyncStatus(void);
#endif	// MEMORYNOTES_H__
