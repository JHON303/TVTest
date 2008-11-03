#ifndef INFO_PANEL_H
#define INFO_PANEL_H


#include "BasicWindow.h"


class CInfoPanelEventHandler {
public:
	virtual void OnSelChange() {}
	virtual void OnRButtonDown() {}
	virtual bool OnKeyDown(UINT KeyCode,UINT Flags) { return false; }
	virtual void OnVisibleChange(bool fVisible) {}
};

class CInfoPanel : public CBasicWindow {
	enum {MAX_WINDOWS=4};
	class CWindowInfo {
	public:
		CBasicWindow *m_pWindow;
		LPTSTR m_pszTitle;
		CWindowInfo(CBasicWindow *pWindow,LPCTSTR pszTitle);
		~CWindowInfo();
	};
	CWindowInfo *m_pWindowList[MAX_WINDOWS];
	int m_NumWindows;
	COLORREF m_crBackColor;
	COLORREF m_crMarginColor;
	COLORREF m_crTabBackColor;
	COLORREF m_crTabTextColor;
	COLORREF m_crTabBorderColor;
	COLORREF m_crCurTabBackColor;
	COLORREF m_crCurTabTextColor;
	COLORREF m_crCurTabBorderColor;
	HFONT m_hfont;
	int m_TabHeight;
	int m_TabWidth;
	int m_ClientMargin;
	int m_CurTab;
	CInfoPanelEventHandler *m_pEventHandler;
	static HINSTANCE m_hinst;
	static CInfoPanel *GetThis(HWND hwnd);
	static LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	void CalcTabWidth();
public:
	static bool Initialize(HINSTANCE hinst);
	CInfoPanel();
	~CInfoPanel();
	// CBasicWindow
	bool Create(HWND hwndParent,DWORD Style,DWORD ExStyle=0,int ID=0);
	void SetVisible(bool fVisible);
	// CInfoPanel
	bool AddWindow(CBasicWindow *pWindow,LPCTSTR pszTitle);
	bool SetCurTab(int Index);
	int GetCurTab() const { return m_CurTab; }
	void SetEventHandler(CInfoPanelEventHandler *pHandler);
	void SetBackColors(COLORREF crBack,COLORREF crMargin);
	void SetTabColors(COLORREF crBack,COLORREF crText,COLORREF crBorder);
	void SetCurTabColors(COLORREF crBack,COLORREF crText,COLORREF crBorder);
};


#endif