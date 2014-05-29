#ifndef	__MACROS_H__
#define	__MACROS_H__

#define OleQueryInterface(this, riid, ppv)\
	(this)->lpVtbl->QueryInterface(this, riid, ppv)

#define OleAddRef(this)\
	(this)->lpVtbl->AddRef(this)

#define OleRelease(this)\
	(this)->lpVtbl->Release(this)

#define IMPL(class, member)\
	(&((class *)0)->member == This,\
	((class*)(((INT_PTR)This) - offsetof(class, member))))

#endif
