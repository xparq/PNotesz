#ifndef __DATAOBJECT_H__
#define __DATAOBJECT_H__

//#include <windows.h>
#include <richole.h>
typedef struct tagCReCallback CDataObject;

CDataObject * new_CDataObject(void) ;
void delete_CDataObject(CDataObject * pDataObject) ;
void InsertBitmap(IRichEditOle * pRichEditOle, HBITMAP hBitmap);

#endif	// DATAOBJECT_H__
