#ifndef __SOCKETS_H__
#define __SOCKETS_H__

#define	SOCKET_DEF_PORT			27951

#define	WSPM_BASE				WM_USER + 11180
#define	WSPM_ERROR				WSPM_BASE + 1
#define	WSPM_PROGRESS			WSPM_BASE + 2
#define	WSPM_SEND_FINISHED		WSPM_BASE + 3
#define	WSPM_RECEIVE_FINISHED	WSPM_BASE + 4
#define	WSPM_DATA_SAVING_ERROR	WSPM_BASE + 5

#define	REPLY_WELCOME			"WELCOME"
#define	REPLY_OK				"OK"
#define	REPLY_DONE				"DONE"
#define	MSG_EOF					"C009CA17-181B-4C80-BF04-3A86E69BE506-PNMEOF"
#define	READ_MUTEX				"PN_READ_MUTEX"
#define	WRITE_MUTEX				"PN_WRITE_MUTEX"

typedef	struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

// typedef struct _ID_STRUCT {
	// char		f1;
	// char		f2;
	// char		f3;
	// char		f4;
	// char		f5;
	// char		f6;
	// char		f7;
	// char		f8;
	// char		f9;
	// char		f10;
	// char		f11;
	// char		f12;
	// char		f13;
	// char		f14;
	// char		f15;
	// char		f16;
	// char		f17;
	// char		f18;
	// char		f19;
	// char		f20;
// }ID_STRUCT, *LPID_STRUCT;

// typedef struct _SOCKET_SENDER_DATA {
	// wchar_t			szHostName[256];
	// DWORD			dwAddress;
	// SYSTEMTIME		stSendTime;
// }SOCKET_SENDER_DATA, *LPSOCKET_SENDER_DATA;

// typedef struct _SOCKET_SEND_DATA {
	// char			szAddress[256];
	// ULONG			nSize;
	// HWND			hCaller;
// }SOCKET_SEND_DATA, *LPSOCKET_SEND_DATA;

// typedef int (*SENDDATAPROC)(LPCSTR, LPVOID, int, HWND);

DWORD WINAPI SocketClientFunc(LPVOID lpParam);
BOOL InitializeWinsock(void);
void DeInitializeWinsock(void);
DWORD WINAPI SocketServerFunc(LPVOID lpParam);
int SendData(LPBYTE pData, int nSize);
void GetHostProperties(char * lpName);
// ID_STRUCT StructFromID(wchar_t * lpID);
// void IDFromStruct(LPID_STRUCT lpids, wchar_t * lpID);
LPBYTE GetDataToSend(wchar_t * lpID, ULONG * pSize);

USHORT					g_wsDefPort;
BOOL					g_wsStopServer, g_wsServerUp, g_wsPause;
// SOCKET_SEND_DATA		g_wsSendData;
LPBYTE					g_wsPData;
wchar_t					g_wsID[128], g_wsTempID[128];
char					g_wsAddress[256], g_wsLocalHost[256];
HANDLE					g_wsReadMutex;
HWND					g_wsCaller;

#endif	// SOCKETS_H__
