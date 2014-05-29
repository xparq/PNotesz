#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

typedef struct _PNSPELL {
	BOOL				isCheckLib;
	wchar_t				spellCulture[16];
	COLORREF			color;
	BOOL				spellAuto;
}PNSPELL, * LPPNSPELL;

typedef struct PLOCALSYNC * LPPLOCALSYNC;
typedef struct PLOCALSYNC {
	wchar_t				name[128];
	wchar_t				pathNotes[MAX_PATH];
	wchar_t				pathId[MAX_PATH];
	LPPLOCALSYNC		next;
	LPPLOCALSYNC		prev;
}PLOCALSYNC;

typedef struct PCONTGROUP * LPPCONTGROUP;
typedef struct PCONTGROUP {
	wchar_t			name[128];
	int				id;
	LPPCONTGROUP	next;
	LPPCONTGROUP	prev;
}PCONTGROUP;

typedef struct _PPIN {
	wchar_t			text[MAX_PATH];
	wchar_t			class[MAX_PATH];
}PPIN, *LPPPIN;

typedef struct PCONTPROP {
	wchar_t			name[128];
	wchar_t			host[256];
	DWORD			address;
	BOOL			usename;
	BOOL			send;
	int				group;
}PCONTPROP, *LPPCONTPROP;

typedef struct PCONTACT * LPPCONTACT;
typedef struct PCONTACT {
	PCONTPROP		prop;
	LPPCONTACT		next;
	LPPCONTACT		prev;
}PCONTACT;

typedef struct _SRSTATUS {
	wchar_t			sentTo[256];
	wchar_t			recFrom[256];
	SYSTEMTIME		lastSent;
	SYSTEMTIME		lastRec;
}SRSTATUS, *LPSRSTATUS;

typedef struct PLINK * LPPLINK;
typedef struct PLINK {
	wchar_t				id[28];
	LPPLINK				next;
	LPPLINK				prev;
}PLINK;

typedef struct PSENG * LPPSENG;
typedef struct PSENG {
	wchar_t				name[128];
	wchar_t				query[1024];
	wchar_t				commandline[MAX_PATH];
	LPPSENG				next;
	LPPSENG				prev;
}PSENG;

typedef struct PTAG * LPPTAG;
typedef struct PTAG {
	wchar_t				text[128];
	wchar_t				oldText[128];
	LPPTAG				next;
	LPPTAG				prev;
}PTAG;

typedef union _TIMEUNION {
	FILETIME 		fileTime;
	ULARGE_INTEGER 	ul;
} TIMEUNION;

typedef struct _NOTE_FLAGS{
	BOOL		mouseIn;
	BOOL		saved;
	BOOL		transparent;
	BOOL		fromDB;
	BOOL		inDrag;
	BOOL		trackRoll;
	BOOL		locked;
	WORD		maskChanged;
	wchar_t		id[28];
	wchar_t		idBackup[128];
} NOTE_FLAGS, *P_NOTE_FLAGS;

typedef struct _SMALLVALUES{
	BYTE		value1;
	BYTE		value2;
	BYTE		value3;
	BYTE		value4;
	BYTE		value5;
	BYTE		value6;
	BYTE		value7;
	BYTE		value8;
}SMALLVALUES, *PSMALLVALUES;

typedef struct _NOTE_DATA{
	wchar_t		szName[128];
	SYSTEMTIME	stChanged;
	BOOL		onTop;		
	int			idGroup;
	int			dockData;
	BOOL		visible;
	RECT		rcp;
	BOOL		prevOnTop;
	BOOL		rolled;
	int			res1;
	int			idPrevGroup;
} NOTE_DATA, *P_NOTE_DATA;

typedef struct _NOTE_RTHANDLES{
	HBITMAP		hbSkin;
	HBITMAP		hbSys;
	HBITMAP		hbBack;
	HBITMAP		hbDelHide;
	HBITMAP		hbCommand;
	COLORREF	crMask;
	HFONT		hFont;
	HFONT		hFCaption;
	UINT_PTR	idTimer;
	UINT_PTR	pinTimer;
	HWND		hTooltip;
	RECT		rcEdit;
	RECT		rcTooltip;
	RECT		rcSchedule;
	RECT		rcNotSaved;
	RECT		rcProtected;
	RECT		rcPriority;
	RECT		rcCompleted;
	RECT		rcPassword;
	RECT		rcPin;
	RECT		rcDH;
	SIZE		szDef;
	RECT		rcCaption;
	RECT		rcGrip;
	RECT		rcTextCaption;
	RECT		rcCommands;
	UINT		nRepeat;
	RECT		rcSize;
	BOOL		rolled;
	wchar_t		szSkinInfo[256];
	BOOL		vertTbr;
	int			marks;
	int			favorite;
	int			syncstatusFTP;
	BOOL		wipe;
	BOOL		pinActive;
	int			syncstatusLocal;
	short		closeCounter;
	short		stopAlarmCounter;
} NOTE_RTHANDLES, *P_NOTE_RTHANDLES;

typedef struct _NOTE_APPEARANCE{
	wchar_t		szSkin[64];
	LOGFONTW	lfFont;
	COLORREF	crFont;
	LOGFONTW	lfCaption;
	COLORREF	crCaption;
	COLORREF	crWindow;
	BOOL		fFontSet;
	int			nPrivate;
} NOTE_APPEARANCE, *P_NOTE_APPEARANCE;

typedef struct _ADDITIONAL_NAPP{
	BYTE		transValue;
	BYTE		addResByte;
	int			addRes1;
	int			addRes2;
} ADDITIONAL_NAPP, *P_ADDITIONAL_NAPP;

typedef struct _NOTE_SETTINGS{
	BOOL		transAllow;
	BYTE		transValue;
	BOOL		oneMoreBool;
	BOOL		saveOnExit;
	BOOL		confirmSave;
	BOOL		onStartup;
	BOOL		confirmDelete;
	BOOL		showScrollbar;
	BOOL		autoSave;
	BYTE		autoSValue;
	BOOL		checkOnStart;
	BOOL		visualAlert;
	BOOL		rollOnDblClick;
	BOOL		newOnTop;
	int			reserved1;
	int			reserved2;
} NOTE_SETTINGS, *P_NOTE_SETTINGS;

typedef struct _NEXT_SETTINGS {
	int			flags1;
	int			flags2;
	int			flags3;
	int			flags4;
	int			flags5;
	int			flags6;
	int			flags7;
	int			flags8;
	int			flags9;
	int			flags10;
	int			cleanDays;
} NEXT_SETTINGS, P_NEXT_SETTINGS;

typedef struct _NOTE_REL_POSITION {
	double		left;
	double		top;
	int			width;
	int			height;
} NOTE_REL_POSITION, *P_NOTE_REL_POSITION;

#pragma pack(push)
#pragma pack(2)
typedef struct _SCHEDULE_TYPE{
	WORD		scType;
	SYSTEMTIME	scDate;
	SYSTEMTIME	scStart;
	SYSTEMTIME	scLastRun;
	wchar_t		szSound[128];
	int			params;
	WORD		stopLoop;
} SCHEDULE_TYPE, *P_SCHEDULE_TYPE;
#pragma pack(pop)

typedef struct _NOTE_PATHS{
	wchar_t		INIFile[MAX_PATH];
	wchar_t		DataFile[MAX_PATH];
	wchar_t		DataDir[MAX_PATH];
	wchar_t		LangDir[MAX_PATH];
	wchar_t		LangFile[256]; 
	wchar_t		CurrLanguagePath[MAX_PATH];
	wchar_t		SkinDir[MAX_PATH];
	wchar_t		SoundDir[MAX_PATH];
	wchar_t		ProgName[256];
	wchar_t		ProgFullPath[MAX_PATH];
	wchar_t		FontsPath[MAX_PATH];
	wchar_t		BackupDir[MAX_PATH];
	wchar_t		IconsPath[MAX_PATH];
	wchar_t		DictDir[MAX_PATH];
} NOTE_PATHS, *P_NOTE_PATHS;

typedef struct _NT_DT_FORMATS{
	wchar_t		DateFormat[128];
	wchar_t		TimeFormat[128];
} NT_DT_FORMATS, *P_NT_DT_FORMATS;

typedef struct _SOUND_TYPE{
	int			allowSound;
	wchar_t		custSound[128];
} SOUND_TYPE, *P_SOUND_TYPE;

typedef struct _HK_TYPE{
	int			id;
	int			identifier;
	UINT		fsModifiers;
	UINT		vk;
	wchar_t		szKey[24];
} HK_TYPE, *P_HK_TYPE;

typedef struct _PNCOMMAND{
	wchar_t		szCommand[128];
} PNCOMMAND, *P_PNCOMMAND;

typedef struct _NCMD_BAR{
	HWND		hCmd;
	HWND		hTtp;
} NCMD_BAR, *P_NCMD_BAR;

typedef struct _FONT_TYPE{
	wchar_t		szFace[LF_FACESIZE];
	int			nCharSet;
} FONT_TYPE, *P_FONT_TYPE;

typedef struct _PNSTRINGS{
	wchar_t		SNote[64];
	wchar_t		SQuestion[128];
	wchar_t		SCaption[64];
	wchar_t		DMessage[256];
	wchar_t		DMessageBin[256];
	wchar_t		DMessageComplete[256];
	wchar_t		DMessageMultiple[256];
	wchar_t		DMessageBackup[256];
	wchar_t		DMessageMultiFromBin[256];
	wchar_t		EmptyBinMessage[256];
	wchar_t		DCaption[64];
	wchar_t		TooManyFiles[256];
	wchar_t		DPrevQuestion[128];
	wchar_t		DeleteGroup0[96];
	wchar_t		DeleteGroup1[256];
	wchar_t		DeleteGroup2[256];
	wchar_t		DeleteGroup3[256];
	wchar_t		DFCaption[128];
	wchar_t		DFormats[1536];
	wchar_t		TFCaption[128];
	wchar_t		TFormats[1408];
	wchar_t		SearchComplete[128];
	wchar_t		ReplaceComplete[128];
	wchar_t		MatchesFound[128];
	wchar_t		NoMatchesFound[128];
	wchar_t		CheckUpdate[60];
	wchar_t		NewVersion1[112];
	wchar_t		NewVersion2[112];
	wchar_t		SameVersion[128];
	wchar_t		FontSample[256];
	wchar_t		SaveBeforeSend[256];
	wchar_t		DefaultSettingsWarning[256];
	wchar_t		HK1[128];
	wchar_t		HK2[128];
	wchar_t		HK3[128];
	wchar_t		InsPictureCaption[128];
	wchar_t		FirstBaloonCaption[64];
	wchar_t		FirstBaloonMessage[128];
	wchar_t		DeleteSEngine[128];
	wchar_t		Downloading[64];
	wchar_t		ReplaceWarning1[256];
	wchar_t		ReplaceWarning2[256];
	wchar_t		ContinueQuestion[64];
	wchar_t		InternetUnavailable[128];
	wchar_t		FullBackupWarnung[256];
	wchar_t		DeleteExternal[128];
	wchar_t		CleanBin1[128];
	wchar_t		CleanBin2[128];
	wchar_t		ReceivedCaption[64];
	wchar_t		Received1[64];
	wchar_t		Received2[32];
	wchar_t		Received3[64];
	wchar_t		SentCaption[64];
	wchar_t		Sent1[32];
	wchar_t		Sent2[64];
	wchar_t		Sent3[32];
	wchar_t		Sent4[64];
	wchar_t		NoSuggestion[64];
	wchar_t		AddToDictionary[64];
	wchar_t		SpellCheckComplete[128];
} PNSTRINGS, *P_PNSTRINGS;

typedef struct _DARROWS{
	HWND				hlu;
	HWND				hld;
	HWND				htl;
	HWND				htr;
	HWND				hru;
	HWND				hrd;
	HWND				hbl;
	HWND				hbr;
}DARROWS, *P_DARROWS;

typedef struct _NFIND{
	HWND				hNote;
	int					iFindReplace;
}NFIND, *P_NFIND;

typedef struct _MEMNOTE * PMEMNOTE;
typedef struct _MEMNOTE {
	HWND				hwnd;
	PMEMNOTE			next;
	PMEMNOTE			prev;
	P_NOTE_RTHANDLES	pRTHandles;
	P_NOTE_RTHANDLES	pSavedHandles;
	P_NOTE_FLAGS		pFlags;
	P_NOTE_APPEARANCE	pAppearance;
	P_SCHEDULE_TYPE		pSchedule;
	P_NOTE_DATA			pData;
	P_ADDITIONAL_NAPP	pAddNApp;
	BOOL				isAlarm;
	LPSYSTEMTIME		pCreated;
	LPPTAG				pTags;
	LPSYSTEMTIME		pDeleted;
	LPSYSTEMTIME		pRealDeleted;
	LPVOID				pVoice;
	LPPLINK				pLinks;
	LPSRSTATUS			pSRStatus;
	LPPPIN				pPin;
}_MEMNOTE;

typedef struct _EMPTY_NOTES{
	LPPOINT	pPoint;
	LPINT	pDockData;
	int		count;
}EMPTY_NOTES;

typedef struct _ADJUST_STRUCT {
	PMEMNOTE	pNote;
	int			type;
}ADJUST_STRUCT, *P_ADJUST_STRUCT;

typedef struct _CONT_GROUP_STRUCT {
	int			id;
	wchar_t		name[128];
}CONT_GROUP_STRUCT, *P_CONT_GROUP_STRUCT;

typedef struct _GROUP_MENU_STRUCT {
	int			id;
	wchar_t		*pName;
	P_HK_TYPE	pKeyShow;
	P_HK_TYPE	pKeyHide;
}GROUP_MENU_STRUCT, *P_GROUP_MENU_STRUCT;

typedef struct _FAV_MENU_STRUCT {
	int			id;
	wchar_t		pName[128];
	wchar_t		pReserved[128];
}FAV_MENU_STRUCT, *P_FAV_MENU_STRUCT;

typedef struct _DIARY_MENU_STRUCT {
	SYSTEMTIME	tCreated;
	wchar_t		pName[128];
	wchar_t		pReserved[128];
}DIARY_MENU_STRUCT, *P_DIARY_MENU_STRUCT;

typedef struct _N2DLG {
	PMEMNOTE	pNote;
	int			reserved;
}N2DLG, *P_N2DLG;
#endif	// __STRUCTURES_H__
