#ifndef EPG_DATA_LOADER_H
#define EPG_DATA_LOADER_H


#include "BonTsEngine/EventManager.h"


// EpgDataCap_Bon��EPG�f�[�^��ǂݍ��ރN���X
class CEpgDataLoader
{
public:
	class ABSTRACT_DECL CEventHandler {
	public:
		virtual ~CEventHandler() = 0;
		virtual void OnStart() {}
		virtual void OnEnd(bool fSuccess,CEventManager *pEventManager) {}
	};
	CEpgDataLoader();
	~CEpgDataLoader();
	bool Load(LPCTSTR pszFolder,HANDLE hAbortEvent=NULL);
	bool LoadAsync(LPCTSTR pszFolder,CEventHandler *pEventHandler=NULL);
	bool Abort(DWORD Timeout=10000);

private:
	CEventManager m_EventManager;
	HANDLE m_hThread;
	HANDLE m_hAbortEvent;
	LPTSTR m_pszFolder;
	CEventHandler *m_pEventHandler;
	bool LoadFromFile(LPCTSTR pszFileName);
	static DWORD WINAPI LoadThread(LPVOID lpParameter);
};


#endif	/* EPG_DATA_LOADER_H */