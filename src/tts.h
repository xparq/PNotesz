#ifndef __TTS_H__
#define __TTS_H__

#pragma pack(push)
#pragma pack(2)
typedef struct _PNVOICE {
	wchar_t				name[128];
	void				*token;
	long				rate;
	unsigned short		volume;
	short				pitch;
}PNVOICE, *PPNVOICE;
#pragma pack(pop)

PNVOICE					g_PVoices[32], g_TempPVoices[32];
int						g_VoicesCount;
wchar_t					g_DefVoice[128];

void StopSpeak(void * pV);
void Speak(void * pV, const wchar_t * pwcs, void * pToken, PPNVOICE pVoices);
HRESULT LoadVoices(void);
void * CreateVoice(void);
void ReleaseVoice(void * pVoice);
void SetVoiceNotification(void * pV, HWND hwnd);
void RemoveVoiceNotification(void * pV);
PPNVOICE GetVoice(PPNVOICE voices, wchar_t * name);
PPNVOICE GetVoiceByToken(PPNVOICE voices, DWORD token);

#endif	// TTS_H__
