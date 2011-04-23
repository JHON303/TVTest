#ifndef LOGGER_H
#define LOGGER_H


#include <vector>
#include "Options.h"
#include "TsUtilClass.h"


class CLogItem
{
	FILETIME m_Time;
	LPTSTR m_pszText;

public:
	CLogItem(LPCTSTR pszText);
	~CLogItem();
	LPCTSTR GetText() const { return m_pszText; }
	void GetTime(SYSTEMTIME *pTime) const;
	int Format(char *pszText,int MaxLength) const;
};

class CLogger : public COptions, public CTracer
{
	std::vector<CLogItem*> m_LogList;
	bool m_fOutputToFile;
	CCriticalLock m_Lock;

// CBasicDialog
	INT_PTR DlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);


public:
	CLogger();
	~CLogger();
// COptions
	bool Read(CSettings *pSettings) override;
	bool Write(CSettings *pSettings) const override;
// CBasicDialog
	bool Create(HWND hwndOwner);
// CLogger
	bool AddLog(LPCTSTR pszText, ...);
	bool AddLogV(LPCTSTR pszText,va_list Args);
	void Clear();
	bool SetOutputToFile(bool fOutput);
	bool GetOutputToFile() const { return m_fOutputToFile; }
	bool SaveToFile(LPCTSTR pszFileName,bool fAppend);
	void GetDefaultLogFileName(LPTSTR pszFileName) const;

protected:
// CTracer
	void OnTrace(LPCTSTR pszOutput) override;
};


#endif
