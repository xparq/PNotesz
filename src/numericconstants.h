//colors
#define CLR_MASK					RGB(255, 0, 255)
#define CR_IN_MASK					RGB(255, 0, 254)
#define	SPELL_COLOR					RGB(255, 0, 0)
#define	DEF_COLOR					RGB(242, 221, 116)
#define	DEF_TEXT_COLOR				RGB(0, 0, 1)

//edit box identifier
#define IDC_NOTE_EDIT				8500
//note toolbar constants
#define	CMD_TBR_BUTTONS				12
#define	SMALL_RECT					12
#define	CMD_SYS_BUTTONS				2	
#define	MARKS_COUNT					7
//default skinless dimensions
#define	DEF_FLAT_WIDTH				276
#define	DEF_FLAT_HEIGHT				276
#define	DEF_SIZE_MAX				800
#define	DEF_SIZE_MIN				64
//apply changes constants
#define APP_NEW_FONT				9001
#define APP_NEW_SKIN				9002
#define APP_NEW_COLOR				9003
#define	APP_TR_ALWD					9004
#define	APP_TR_NALWD				9005
#define	APP_TR_VALUE				9006
#define	APP_SAVE_EXIT				9007
#define	APP_CLOSE_NOTE				9008
#define	APP_NEW_CAP_FONT			9009
#define	APP_NEW_CAP_COLOR			9010
#define	APP_NEW_BCKGRND				9011
#define	APP_NEW_LANGUAGE			9012
#define	APP_SAVE_NOTE				9013
#define	APP_AUTOSAVE				9014
#define	APP_SAVE_ALL				9015
#define	APP_SAVE_BEFORE_RELOAD		9016
#define	APP_ON_TOP_CHANGED			9017
#define	APP_DOCK_DIST_CHANGED		9018
#define	APP_DOCK_SKIN_CHANGED		9019
#define	APP_DOCK_SIZE_CHANGED		9020
#define	APP_DOCK_ORDER_CHANGED		9021
#define	APP_DOCK_FONT_CHANGED		9022
#define	APP_DOCK_SKNLSS_FC_CHANGED	9023
#define	APP_DOCK_BGC_CHANGED		9024
#define	APP_SESSION_LOGON			9025
#define	APP_JUST_REDRAW				9026

//color menu identifiers
// #define	IDI_COLOR_START				13000
#define	IDI_FORMAT_COLOR_START		13100
#define	IDI_FORMAT_COLOR_END		13116
#define	IDI_FORMAT_HIGHLIGHT_START	13200
#define	IDI_FORMAT_HIGHLIGHT_END	13216
#define	IDI_SIZE_START				13300
#define	IDI_COLOR_CAPTION_START		IDI_FORMAT_COLOR_START + 1
#define	IDI_COLOR_CAPTION_END		IDI_FORMAT_COLOR_END
#define	IDI_COLOR_FONT_START		IDI_FORMAT_HIGHLIGHT_START + 1
#define	IDI_COLOR_FONT_END			IDI_FORMAT_HIGHLIGHT_END
#define	MAX_FONT_SIZE				72
#define	ID_FC_CAPTION				5100
#define	ID_FH_CAPTION				5101
//delete/hide buttons identifiers
// #define	IDI_CMD_DEL				10012
// #define	IDI_CMD_HIDE			10011
//size menu identifiers

//skin resources
#define IDS_SKIN_BODY				25000
#define IDS_SKIN_SYS				25001
#define	IDS_SKIN_DEL_HIDE			25002
#define IDS_EDIT_RECT				25010
#define	IDS_TT_RECT					25011
#define	IDS_SCH_RECT				25012
#define	IDS_NOT_SAVED_RECT			25013
#define	IDS_PROTECTED_RECT			25014
#define	IDS_PRIORITY_RECT			25015
#define	IDS_COMPLETED_RECT			25023
#define	IDS_MASK_COLOR				25016
#define	IDS_SKIN_COMMANDS			25017
#define	IDS_COMMANDS_RECT			25018
#define IDS_DH_RECT					25019
#define	IDS_SKIN_INFO				25020
#define	IDS_VERTICAL_TBR			25021
#define	IDS_MARKS_COUNT				25022
#define	IDS_PASSWORD_RECT			25024
#define	IDS_PIN_RECT				25025

//language menu item addition
#define LANG_ADDITION				25000
//external programs addition
#define	EXTERNALS_ADDITION			19000
#define EXTERNALS_MAX				1000
//search engines menu addition
#define	SEARCH_ENGINE_ADDITION		17000
#define SEARCH_ENGINE_MAX			1000
//diary group addition
#define	DIARY_ADDITION				16000
//group menu addition
#define	GROUP_ADDITION				15000
//show groups addition
#define	SHOW_GROUP_ADDITION			13000
//hide group addition
#define	HIDE_GROUP_ADDITION			14000
//favorites menu addition
#define FAVORITES_ADDITION			12000
#define	FAVORITES_MAX				128
//columns visibility addition
#define	COLUMN_VIS_ADDITION			18000
//links menu item addition
#define	LINKS_ADDITION				20000
#define	LINKS_MAX					500
//contacts menu item addition
#define	CONTACTS_ADDITION			20500
#define	CONTACTS_MAX				500
//tags menu items addition
#define	SHOW_BY_TAG_ADDITION		21000
#define	HIDE_BY_TAG_ADDITION		22000
#define	TAGS_MAX_MENU				1000
//groups of contacts menu item addition
#define	CONT_GROUPS_ADDITION		23000
#define	CONT_GROUPS_MAX				500
//spelling dictionaries item addition
#define SPELL_DICT_ADDITION			23500
#define SPELL_DICT_MAX				500
//suggestions addition
#define	SUGG_ADDITION				24000
#define	SUGG_START					24001
#define	SUGG_MAX					500

#define	NOTE_TIMER_INTERVAL			1000
#define	PIN_TIMER_INTERVAL			500

//custom messages
#define	PNM_BASE					WM_APP + 1234
#define PNM_RESIZE					PNM_BASE + 1
#define PNM_SAVE					PNM_BASE + 2
#define	PNM_TT_UPDATE				PNM_BASE + 3
#define	PNM_CTRL_UPD				PNM_BASE + 4
#define	PNM_CTRL_UPD_LANG			PNM_BASE + 5
#define	PNM_CTRL_INS				PNM_BASE + 6
#define	PNM_CTRL_DEL				PNM_BASE + 7
#define	PNM_MAIN_CLOSE				PNM_BASE + 8
#define	PNM_CHANGE_FONT				PNM_BASE + 9
#define	PNM_CHANGE_FONT_COLOR		PNM_BASE + 10
#define	PNM_NEW_LANGUAGE			PNM_BASE + 11
#define	PNM_NEW_FONT_FACE			PNM_BASE + 12
#define	PNM_SAVE_ON_SHUTDOWN		PNM_BASE + 13
#define PNM_RELOAD					PNM_BASE + 14
#define	PNM_NEW_BGCOLOR				PNM_BASE + 15
#define	PNM_ON_TOP_CHANGED			PNM_BASE + 16
#define	PNM_SHOW_AFTER_SEARCH		PNM_BASE + 17
#define PNM_BECOMES_HIDDEN			PNM_BASE + 18
#define	PNM_MOVE_DOCK_NOTE			PNM_BASE + 19
#define	PNM_UPDATE_CTRL_STATUS		PNM_BASE + 20
#define	PNM_APPLY_NEW_SKIN			PNM_BASE + 21
#define	PNM_NEW_FIND_PARENT			PNM_BASE + 22
#define PNM_CTRL_ITEM_UPDATE		PNM_BASE + 23
#define	PNM_CTRL_GROUP_UPDATE		PNM_BASE + 24
#define	PNM_CTRL_BACK_UPDATE		PNM_BASE + 25
#define	PNM_CTRL_BACK_DELETE		PNM_BASE + 26
#define	PNM_FAVORITES_CHANGED		PNM_BASE + 27
#define	PNM_CTRL_DIARY_CHANGE_LANG	PNM_BASE + 28
#define	PNM_CTRL_DIARY_FROM_RB		PNM_BASE + 29
#define	PNM_CREATE_NEW_NOTE			PNM_BASE + 30
#define	PNM_NEW_FROM_CLIPBOARD		PNM_BASE + 31
#define	PNM_DIARY_TODAY				PNM_BASE + 32
#define	PNM_CTRL_FAV_STATUS_CHANGED	PNM_BASE + 33
#define	PNM_GET_MAIN_MENU			PNM_BASE + 34
#define	PNM_CLEAN_BIN				PNM_BASE + 35
#define	PNM_SOUND_ENDS				PNM_BASE + 36
#define	PNM_NOTE_SENT				PNM_BASE + 37
#define	PNM_SELECT_INCOMING			PNM_BASE + 38
#define	PNM_OPEN_PAGE				PNM_BASE + 39
#define	PNM_NOTES_RELOAD			PNM_BASE + 40
#define	PNM_IS_FTP_ENABLED			PNM_BASE + 41
#define	PNM_REDRAW_EDIT				PNM_BASE + 42
#define	PNM_NEW_CAPTION_COLOR		PNM_BASE + 43

#define	CHN_DEL_WITH_GROUP			1

#define	GROUP_ROOT					-1
#define	GROUP_RECYCLE				-2
#define	GROUP_SEARCH_RESULTS		-3
#define	GROUP_BACKUP				-4
#define	GROUP_FAVORITES				-5
#define	GROUP_INCOMING				-6
#define	GROUP_DIARY					0x10000
#define	NEW_GROUP_ID				-777
#define	MAX_GROUP_COUNT				1000

#define	MAX_SEARCH_COUNT			16
#define	DEF_BACK_COUNT				3
#define MIN_DIARY_PAGES				1
#define MAX_DIARY_PAGES				30
#define	DEF_DIARY_PAGES				7

#define	DEF_SAVE_CHARACTERS			128
#define	DEF_CONTENT_CHARACTERS		24
#define MAX_CONTENT_CHARACTERS		128

#define	COUNT_EDIT_HOTKEYS			5

#define	HKG_SHOW					1
#define	HKG_HIDE					2

//tray + shellnotify
#define IDI_TRAY					0
#define WM_SHELLNOTIFY				WM_USER + 5

//session constants
#define WM_WTSSESSION_CHANGE  		0x02B1
#define	NOTIFY_FOR_THIS_SESSION		0
#define	NOTIFY_FOR_ALL_SESSIONS		1
#define WTS_CONSOLE_CONNECT  		0x1
#define WTS_CONSOLE_DISCONNECT  	0x2
#define WTS_REMOTE_CONNECT  		0x3
#define WTS_REMOTE_DISCONNECT  		0x4
#define WTS_SESSION_LOGON  			0x5
#define WTS_SESSION_LOGOFF  		0x6
#define WTS_SESSION_LOCK  			0x7
#define WTS_SESSION_UNLOCK  		0x8
#define WTS_SESSION_REMOTE_CONTROL  0x9

#define	START_COMP					100
#define	START_PROG					200
