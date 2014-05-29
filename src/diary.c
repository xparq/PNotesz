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

#include "notes.h"
#include "structures.h"
#include "memorynotes.h"
#include "enums.h"
#include "shared.h"
#include "numericconstants.h"
#include "stringconstants.h"
#include "diary.h"

void GetDiaryDateString(int bitfield, LPSYSTEMTIME lpDate, wchar_t ** formats, int index, wchar_t * lpText){
	wchar_t			szFormat[128], szTemp[128];

	wcscpy(szFormat, formats[index]);
	if(IsBitOn(bitfield, SB1_ADD_WEEKDAY_DIARY)){
		if(IsBitOn(bitfield, SB1_FULL_WEEKDAY_NAME)){
			if(IsBitOn(bitfield, SB1_WEEKDAY_DIARY_END))
				wcscat(szFormat, L", dddd");
			else{
				wcscpy(szTemp, L"dddd, ");
				wcscat(szTemp, szFormat);
				wcscpy(szFormat, szTemp);
			}
		}
		else{
			if(IsBitOn(bitfield, SB1_WEEKDAY_DIARY_END))
				wcscat(szFormat, L", ddd");
			else{
				wcscpy(szTemp, L"ddd, ");
				wcscat(szTemp, szFormat);
				wcscpy(szFormat, szTemp);
			}
		}
	}
	GetDateFormatW(MAKELCID(MAKELANGID(g_LangID, g_SubLangID), SORT_DEFAULT), 0, lpDate, szFormat, lpText, 128);
}

void ChangeAllDiaryNames(wchar_t ** formats){
	PMEMNOTE		pNote = MemoryNotes();
	NOTE_DATA		data;

	while(pNote){
		if(pNote->pData->idGroup == GROUP_DIARY){
			//diary notes are always from DB, so just save new name
			GetDiaryDateString(g_NoteSettings.reserved1, pNote->pCreated, formats, g_DiaryFormatIndex, pNote->pData->szName);
			GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
			wcscpy(data.szName, pNote->pData->szName);
			WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
			if(pNote->pData->visible)
				RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
		}
		pNote = pNote->next;
	}
}

BOOL IsDiaryOfToday(void){
	SYSTEMTIME		st;
	PMEMNOTE		pNote = MemoryNotes();

	GetLocalTime(&st);
	while(pNote){
		if(pNote->pData->idGroup == GROUP_DIARY || pNote->pData->idPrevGroup == GROUP_DIARY){
			if(AreDatesEqual(pNote->pCreated, &st)){
				ShowDiaryNote(pNote);
				return TRUE;
			}
		}
		pNote = pNote->next;
	}
	return FALSE;
}

void ShowDiaryNote(PMEMNOTE pNote){
	if(pNote->pData->visible){
		BringWindowToTop(pNote->hwnd);
	}
	else{
		if(pNote->pData->idGroup == GROUP_RECYCLE){
			RestoreNoteFromBin(pNote, -1);
			if(g_hCPDialog){
				SendMessageW(g_hCPDialog, PNM_CTRL_DIARY_FROM_RB, 0, 0);
			}
		}
		if(UnlockGroup(pNote->pData->idGroup) && UnlockNote(pNote))
			ShowNote(pNote);
	}
}
