#ifndef __DIARY_H__
#define __DIARY_H__

void GetDiaryDateString(int bitfield, LPSYSTEMTIME lpDate, wchar_t ** formats, int index, wchar_t * lpText);
void ChangeAllDiaryNames(wchar_t ** formats);
BOOL IsDiaryOfToday(void);
void ShowDiaryNote(PMEMNOTE pNote);

#endif	// DIARY_H__
