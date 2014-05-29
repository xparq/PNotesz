#ifndef __HTABLE_H__
#define __HTABLE_H__

#define	HASH_SIZE			101

typedef struct _HENTRY * PHENTRY;
typedef struct _HENTRY {
	PHENTRY			next;
	wchar_t			*key;
	wchar_t			*value;
}HENTRY;

PHENTRY addEntry(PHENTRY * pArray, wchar_t * key, wchar_t * value);
void freeTable(PHENTRY * pArray);
PHENTRY getEntry(PHENTRY * pArray, wchar_t * key);

#endif	// HTABLE_H__
