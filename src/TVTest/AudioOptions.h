#ifndef AUDIO_OPTIONS_H
#define AUDIO_OPTIONS_H


#include "Options.h"


class CAudioOptions : public COptions {
	enum { MAX_AUDIO_DEVICE_NAME=128 };
	TCHAR m_szAudioDeviceName[MAX_AUDIO_DEVICE_NAME];
	bool m_fDownMixSurround;
	bool m_fRestoreMute;
	static CAudioOptions *GetThis(HWND hDlg);
public:
	CAudioOptions();
	~CAudioOptions();
	LPCTSTR GetAudioDeviceName() const { return m_szAudioDeviceName; }
	bool GetDownMixSurround() const { return m_fDownMixSurround; }
	bool GetRestoreMute() const { return m_fRestoreMute; }
	static BOOL CALLBACK DlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	// COptions
	bool Read(CSettings *pSettings);
	bool Write(CSettings *pSettings) const;
};


#endif