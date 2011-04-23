#ifndef COLOR_SCHEME_H
#define COLOR_SCHEME_H


#include "Options.h"
#include "ColorPalette.h"
#include "Theme.h"


class CColorScheme
{
public:
	enum {
		COLOR_STATUSBACK1,
		COLOR_STATUSBACK2,
		COLOR_STATUSTEXT,
		COLOR_STATUSITEMBORDER,
		COLOR_STATUSBOTTOMITEMBACK1,
		COLOR_STATUSBOTTOMITEMBACK2,
		COLOR_STATUSBOTTOMITEMTEXT,
		COLOR_STATUSBOTTOMITEMBORDER,
		COLOR_STATUSHIGHLIGHTBACK1,
		COLOR_STATUSHIGHLIGHTBACK2,
		COLOR_STATUSHIGHLIGHTTEXT,
		COLOR_STATUSHIGHLIGHTBORDER,
		COLOR_STATUSBORDER,
		COLOR_STATUSRECORDINGCIRCLE,
		COLOR_SPLITTER,
		COLOR_SCREENBORDER,
		COLOR_PANELBACK,
		COLOR_PANELTEXT,
		COLOR_PANELTABBACK1,
		COLOR_PANELTABBACK2,
		COLOR_PANELTABTEXT,
		COLOR_PANELTABBORDER,
		COLOR_PANELCURTABBACK1,
		COLOR_PANELCURTABBACK2,
		COLOR_PANELCURTABTEXT,
		COLOR_PANELCURTABBORDER,
		COLOR_PANELTABMARGIN1,
		COLOR_PANELTABMARGIN2,
		COLOR_PANELTABMARGINBORDER,
		COLOR_PANELTABLINE,
		COLOR_PANELTITLEBACK1,
		COLOR_PANELTITLEBACK2,
		COLOR_PANELTITLETEXT,
		COLOR_PANELTITLEBORDER,
		COLOR_PROGRAMINFOBACK,
		COLOR_PROGRAMINFOTEXT,
		COLOR_PROGRAMLISTPANEL_EVENTBACK1,
		COLOR_PROGRAMLISTPANEL_EVENTBACK2,
		COLOR_PROGRAMLISTPANEL_EVENTTEXT,
		COLOR_PROGRAMLISTPANEL_EVENTBORDER,
		COLOR_PROGRAMLISTPANEL_CUREVENTBACK1,
		COLOR_PROGRAMLISTPANEL_CUREVENTBACK2,
		COLOR_PROGRAMLISTPANEL_CUREVENTTEXT,
		COLOR_PROGRAMLISTPANEL_CUREVENTBORDER,
		COLOR_PROGRAMLISTPANEL_TITLEBACK1,
		COLOR_PROGRAMLISTPANEL_TITLEBACK2,
		COLOR_PROGRAMLISTPANEL_TITLETEXT,
		COLOR_PROGRAMLISTPANEL_TITLEBORDER,
		COLOR_PROGRAMLISTPANEL_CURTITLEBACK1,
		COLOR_PROGRAMLISTPANEL_CURTITLEBACK2,
		COLOR_PROGRAMLISTPANEL_CURTITLETEXT,
		COLOR_PROGRAMLISTPANEL_CURTITLEBORDER,
		COLOR_CHANNELPANEL_CHANNELNAMEBACK1,
		COLOR_CHANNELPANEL_CHANNELNAMEBACK2,
		COLOR_CHANNELPANEL_CHANNELNAMETEXT,
		COLOR_CHANNELPANEL_CHANNELNAMEBORDER,
		COLOR_CHANNELPANEL_CURCHANNELNAMEBACK1,
		COLOR_CHANNELPANEL_CURCHANNELNAMEBACK2,
		COLOR_CHANNELPANEL_CURCHANNELNAMETEXT,
		COLOR_CHANNELPANEL_CURCHANNELNAMEBORDER,
		COLOR_CHANNELPANEL_EVENTNAME1BACK1,
		COLOR_CHANNELPANEL_EVENTNAME1BACK2,
		COLOR_CHANNELPANEL_EVENTNAME1TEXT,
		COLOR_CHANNELPANEL_EVENTNAME1BORDER,
		COLOR_CHANNELPANEL_EVENTNAME2BACK1,
		COLOR_CHANNELPANEL_EVENTNAME2BACK2,
		COLOR_CHANNELPANEL_EVENTNAME2TEXT,
		COLOR_CHANNELPANEL_EVENTNAME2BORDER,
		COLOR_CHANNELPANEL_CUREVENTNAME1BACK1,
		COLOR_CHANNELPANEL_CUREVENTNAME1BACK2,
		COLOR_CHANNELPANEL_CUREVENTNAME1TEXT,
		COLOR_CHANNELPANEL_CUREVENTNAME1BORDER,
		COLOR_CHANNELPANEL_CUREVENTNAME2BACK1,
		COLOR_CHANNELPANEL_CUREVENTNAME2BACK2,
		COLOR_CHANNELPANEL_CUREVENTNAME2TEXT,
		COLOR_CHANNELPANEL_CUREVENTNAME2BORDER,
		COLOR_CONTROLPANELBACK1,
		COLOR_CONTROLPANELBACK2,
		COLOR_CONTROLPANELTEXT,
		COLOR_CONTROLPANELITEMBORDER,
		COLOR_CONTROLPANELHIGHLIGHTBACK1,
		COLOR_CONTROLPANELHIGHLIGHTBACK2,
		COLOR_CONTROLPANELHIGHLIGHTTEXT,
		COLOR_CONTROLPANELHIGHLIGHTBORDER,
		COLOR_CONTROLPANELMARGIN,
		COLOR_CAPTIONPANELBACK,
		COLOR_CAPTIONPANELTEXT,
		COLOR_TITLEBARBACK1,
		COLOR_TITLEBARBACK2,
		COLOR_TITLEBARTEXT,
		COLOR_TITLEBARTEXTBORDER,
		COLOR_TITLEBARICONBACK1,
		COLOR_TITLEBARICONBACK2,
		COLOR_TITLEBARICON,
		COLOR_TITLEBARICONBORDER,
		COLOR_TITLEBARHIGHLIGHTBACK1,
		COLOR_TITLEBARHIGHLIGHTBACK2,
		COLOR_TITLEBARHIGHLIGHTICON,
		COLOR_TITLEBARHIGHLIGHTBORDER,
		COLOR_TITLEBARBORDER,
		COLOR_SIDEBARBACK1,
		COLOR_SIDEBARBACK2,
		COLOR_SIDEBARICON,
		COLOR_SIDEBARITEMBORDER,
		COLOR_SIDEBARHIGHLIGHTBACK1,
		COLOR_SIDEBARHIGHLIGHTBACK2,
		COLOR_SIDEBARHIGHLIGHTICON,
		COLOR_SIDEBARHIGHLIGHTBORDER,
		COLOR_SIDEBARCHECKBACK1,
		COLOR_SIDEBARCHECKBACK2,
		COLOR_SIDEBARCHECKICON,
		COLOR_SIDEBARCHECKBORDER,
		COLOR_SIDEBARBORDER,
		COLOR_NOTIFICATIONBARBACK1,
		COLOR_NOTIFICATIONBARBACK2,
		COLOR_NOTIFICATIONBARTEXT,
		COLOR_NOTIFICATIONBARWARNINGTEXT,
		COLOR_NOTIFICATIONBARERRORTEXT,
		COLOR_PROGRAMGUIDEBACK,
		COLOR_PROGRAMGUIDETEXT,
		COLOR_PROGRAMGUIDECHANNELBACK1,
		COLOR_PROGRAMGUIDECHANNELBACK2,
		COLOR_PROGRAMGUIDECHANNELTEXT,
		COLOR_PROGRAMGUIDECURCHANNELBACK1,
		COLOR_PROGRAMGUIDECURCHANNELBACK2,
		COLOR_PROGRAMGUIDECURCHANNELTEXT,
		COLOR_PROGRAMGUIDETIMEBACK1,
		COLOR_PROGRAMGUIDETIMEBACK2,
		COLOR_PROGRAMGUIDETIMEBACK_0TO2_1,
		COLOR_PROGRAMGUIDETIMEBACK_0TO2_2,
		COLOR_PROGRAMGUIDETIMEBACK_3TO5_1,
		COLOR_PROGRAMGUIDETIMEBACK_3TO5_2,
		COLOR_PROGRAMGUIDETIMEBACK_6TO8_1,
		COLOR_PROGRAMGUIDETIMEBACK_6TO8_2,
		COLOR_PROGRAMGUIDETIMEBACK_9TO11_1,
		COLOR_PROGRAMGUIDETIMEBACK_9TO11_2,
		COLOR_PROGRAMGUIDETIMEBACK_12TO14_1,
		COLOR_PROGRAMGUIDETIMEBACK_12TO14_2,
		COLOR_PROGRAMGUIDETIMEBACK_15TO17_1,
		COLOR_PROGRAMGUIDETIMEBACK_15TO17_2,
		COLOR_PROGRAMGUIDETIMEBACK_18TO20_1,
		COLOR_PROGRAMGUIDETIMEBACK_18TO20_2,
		COLOR_PROGRAMGUIDETIMEBACK_21TO23_1,
		COLOR_PROGRAMGUIDETIMEBACK_21TO23_2,
		COLOR_PROGRAMGUIDETIMETEXT,
		COLOR_PROGRAMGUIDETIMELINE,
		COLOR_PROGRAMGUIDECURTIMELINE,
		COLOR_PROGRAMGUIDE_CONTENT_NEWS,
		COLOR_PROGRAMGUIDE_CONTENT_SPORTS,
		COLOR_PROGRAMGUIDE_CONTENT_INFORMATION,
		COLOR_PROGRAMGUIDE_CONTENT_DRAMA,
		COLOR_PROGRAMGUIDE_CONTENT_MUSIC,
		COLOR_PROGRAMGUIDE_CONTENT_VARIETY,
		COLOR_PROGRAMGUIDE_CONTENT_MOVIE,
		COLOR_PROGRAMGUIDE_CONTENT_ANIME,
		COLOR_PROGRAMGUIDE_CONTENT_DOCUMENTARY,
		COLOR_PROGRAMGUIDE_CONTENT_THEATER,
		COLOR_PROGRAMGUIDE_CONTENT_EDUCATION,
		COLOR_PROGRAMGUIDE_CONTENT_WELFARE,
		COLOR_PROGRAMGUIDE_CONTENT_OTHER,
		COLOR_PROGRAMGUIDE_CONTENT_FIRST=COLOR_PROGRAMGUIDE_CONTENT_NEWS,
		COLOR_PROGRAMGUIDE_CONTENT_LAST=COLOR_PROGRAMGUIDE_CONTENT_OTHER,
		COLOR_LAST=COLOR_PROGRAMGUIDE_CONTENT_LAST,
		NUM_COLORS
	};
	enum {
		GRADIENT_STATUSBACK,
		GRADIENT_STATUSBOTTOMITEMBACK,
		GRADIENT_STATUSHIGHLIGHTBACK,
		GRADIENT_PANELTABBACK,
		GRADIENT_PANELCURTABBACK,
		GRADIENT_PANELTABMARGIN,
		GRADIENT_PANELTITLEBACK,
		GRADIENT_PROGRAMLISTPANEL_EVENTBACK,
		GRADIENT_PROGRAMLISTPANEL_CUREVENTBACK,
		GRADIENT_PROGRAMLISTPANEL_TITLEBACK,
		GRADIENT_PROGRAMLISTPANEL_CURTITLEBACK,
		GRADIENT_CHANNELPANEL_CHANNELNAMEBACK,
		GRADIENT_CHANNELPANEL_CURCHANNELNAMEBACK,
		GRADIENT_CHANNELPANEL_EVENTNAMEBACK1,
		GRADIENT_CHANNELPANEL_EVENTNAMEBACK2,
		GRADIENT_CHANNELPANEL_CUREVENTNAMEBACK1,
		GRADIENT_CHANNELPANEL_CUREVENTNAMEBACK2,
		GRADIENT_CONTROLPANELBACK,
		GRADIENT_CONTROLPANELHIGHLIGHTBACK,
		GRADIENT_TITLEBARBACK,
		GRADIENT_TITLEBARICON,
		GRADIENT_TITLEBARHIGHLIGHTBACK,
		GRADIENT_SIDEBARBACK,
		GRADIENT_SIDEBARHIGHLIGHTBACK,
		GRADIENT_SIDEBARCHECKBACK,
		GRADIENT_NOTIFICATIONBARBACK,
		GRADIENT_PROGRAMGUIDECHANNELBACK,
		GRADIENT_PROGRAMGUIDECURCHANNELBACK,
		GRADIENT_PROGRAMGUIDETIMEBACK,
		GRADIENT_PROGRAMGUIDETIME0TO2BACK,
		GRADIENT_PROGRAMGUIDETIME3TO5BACK,
		GRADIENT_PROGRAMGUIDETIME6TO8BACK,
		GRADIENT_PROGRAMGUIDETIME9TO11BACK,
		GRADIENT_PROGRAMGUIDETIME12TO14BACK,
		GRADIENT_PROGRAMGUIDETIME15TO17BACK,
		GRADIENT_PROGRAMGUIDETIME18TO20BACK,
		GRADIENT_PROGRAMGUIDETIME21TO23BACK,
		NUM_GRADIENTS
	};
	enum {
		BORDER_SCREEN,
		BORDER_STATUS,
		BORDER_STATUSITEM,
		BORDER_STATUSBOTTOMITEM,
		BORDER_STATUSHIGHLIGHT,
		BORDER_TITLEBAR,
		BORDER_TITLEBARCAPTION,
		BORDER_TITLEBARICON,
		BORDER_TITLEBARHIGHLIGHT,
		BORDER_SIDEBAR,
		BORDER_SIDEBARITEM,
		BORDER_SIDEBARHIGHLIGHT,
		BORDER_SIDEBARCHECK,
		BORDER_PROGRAMGUIDESTATUS,
		BORDER_PANEL_TAB,
		BORDER_PANEL_CURTAB,
		BORDER_PANEL_TABMARGIN,
		BORDER_PANEL_TITLE,
		BORDER_PROGRAMLISTPANEL_EVENT,
		BORDER_PROGRAMLISTPANEL_CUREVENT,
		BORDER_PROGRAMLISTPANEL_TITLE,
		BORDER_PROGRAMLISTPANEL_CURTITLE,
		BORDER_CHANNELPANEL_CHANNELNAME,
		BORDER_CHANNELPANEL_CURCHANNELNAME,
		BORDER_CHANNELPANEL_EVENTNAME1,
		BORDER_CHANNELPANEL_EVENTNAME2,
		BORDER_CHANNELPANEL_CUREVENTNAME1,
		BORDER_CHANNELPANEL_CUREVENTNAME2,
		BORDER_CONTROLPANELITEM,
		BORDER_CONTROLPANELHIGHLIGHTITEM,
		NUM_BORDERS
	};
	enum {
		STYLE_STATUSITEM,
		STYLE_STATUSBOTTOMITEM,
		STYLE_STATUSHIGHLIGHTITEM,
		STYLE_TITLEBARCAPTION,
		STYLE_TITLEBARICON,
		STYLE_TITLEBARHIGHLIGHTITEM,
		STYLE_SIDEBARITEM,
		STYLE_SIDEBARHIGHLIGHTITEM,
		STYLE_SIDEBARCHECKITEM,
		STYLE_PANEL_TAB,
		STYLE_PANEL_CURTAB,
		STYLE_PANEL_TABMARGIN,
		STYLE_PANEL_TITLE,
		STYLE_PROGRAMLISTPANEL_EVENT,
		STYLE_PROGRAMLISTPANEL_CUREVENT,
		STYLE_PROGRAMLISTPANEL_TITLE,
		STYLE_PROGRAMLISTPANEL_CURTITLE,
		STYLE_CHANNELPANEL_CHANNELNAME,
		STYLE_CHANNELPANEL_CURCHANNELNAME,
		STYLE_CHANNELPANEL_EVENTNAME1,
		STYLE_CHANNELPANEL_EVENTNAME2,
		STYLE_CHANNELPANEL_CURCHANNELEVENTNAME1,
		STYLE_CHANNELPANEL_CURCHANNELEVENTNAME2,
		STYLE_CONTROLPANELITEM,
		STYLE_CONTROLPANELHIGHLIGHTITEM,
		NUM_STYLES
	};
	struct GradientStyle {
		Theme::GradientType Type;
		Theme::GradientDirection Direction;
	};

	CColorScheme();
	CColorScheme(const CColorScheme &ColorScheme);
	~CColorScheme();
	CColorScheme &operator=(const CColorScheme &ColorScheme);
	COLORREF GetColor(int Type) const;
	COLORREF GetColor(LPCTSTR pszText) const;
	bool SetColor(int Type,COLORREF Color);
	Theme::GradientType GetGradientType(int Gradient) const;
	Theme::GradientType GetGradientType(LPCTSTR pszText) const;
	bool SetGradientStyle(int Gradient,const GradientStyle &Style);
	bool GetGradientStyle(int Gradient,GradientStyle *pStyle) const;
	bool GetGradientInfo(int Gradient,Theme::GradientInfo *pInfo) const;
	Theme::BorderType GetBorderType(int Border) const;
	bool SetBorderType(int Border,Theme::BorderType Type);
	bool GetBorderInfo(int Border,Theme::BorderInfo *pInfo) const;
	bool GetStyle(int Type,Theme::Style *pStyle) const;
	LPCTSTR GetName() const { return m_Name.Get(); }
	bool SetName(LPCTSTR pszName);
	LPCTSTR GetFileName() const { return m_FileName.Get(); }
	bool Load(LPCTSTR pszFileName);
	bool Save(LPCTSTR pszFileName) const;
	bool SetFileName(LPCTSTR pszFileName);
	void SetDefault();
	bool IsLoaded(int Type) const;
	void SetLoaded();

	static LPCTSTR GetColorName(int Type);
	static COLORREF GetDefaultColor(int Type);
	static Theme::GradientType GetDefaultGradientType(int Gradient);
	static bool GetDefaultGradientStyle(int Gradient,GradientStyle *pStyle);
	static bool IsGradientDirectionEnabled(int Gradient);
	static Theme::BorderType GetDefaultBorderType(int Border);
	static int GetColorGradient(int Type);
	static int GetColorBorder(int Type);
	static bool IsBorderAlways(int Border);

private:
	COLORREF m_ColorList[NUM_COLORS];
	GradientStyle m_GradientList[NUM_GRADIENTS];
	Theme::BorderType m_BorderList[NUM_BORDERS];
	CDynamicString m_Name;
	CDynamicString m_FileName;
	struct ColorInfo {
		COLORREF DefaultColor;
		LPCTSTR pszText;
		LPCTSTR pszName;
	};
	struct GradientInfo {
		LPCTSTR pszText;
		Theme::GradientDirection Direction;
		bool fEnableDirection;
		int Color1;
		int Color2;
	};
	struct BorderInfo {
		LPCTSTR pszText;
		Theme::BorderType DefaultType;
		int Color;
		bool fAlways;
	};
	struct StyleInfo {
		int Gradient;
		int Border;
		int TextColor;
	};

	DWORD m_LoadedFlags[(NUM_COLORS+31)/32];
	void SetLoadedFlag(int Color);
	static const ColorInfo m_ColorInfoList[NUM_COLORS];
	static const GradientInfo m_GradientInfoList[NUM_GRADIENTS];
	static const BorderInfo m_BorderInfoList[NUM_BORDERS];
	static const StyleInfo m_StyleList[NUM_STYLES];
};

class CColorSchemeList
{
	int m_NumColorSchemes;
	CColorScheme **m_ppList;

public:
	CColorSchemeList();
	~CColorSchemeList();
	int NumColorSchemes() const { return m_NumColorSchemes; }
	bool Add(CColorScheme *pColorScheme);
	bool Load(LPCTSTR pszDirectory);
	void Clear();
	CColorScheme *GetColorScheme(int Index);
	bool SetColorScheme(int Index,const CColorScheme *pColorScheme);
};

class CColorSchemeOptions : public COptions
{
public:
	typedef bool (*ApplyFunc)(const CColorScheme *pColorScheme);

	CColorSchemeOptions();
	~CColorSchemeOptions();
// COptions
	bool Load(LPCTSTR pszFileName);
	bool Save(LPCTSTR pszFileName) const;
// CBasicDialog
	bool Create(HWND hwndOwner);
// CColorSchemeOptions
	bool SetApplyCallback(ApplyFunc pCallback);
	bool ApplyColorScheme() const;
	const CColorScheme *GetColorScheme() const { return m_pColorScheme; }
	COLORREF GetColor(int Type) const;
	COLORREF GetColor(LPCTSTR pszText) const;
	static bool GetThemesDirectory(LPTSTR pszDirectory,int MaxLength,bool fCreate=false);

private:
	CColorScheme *m_pColorScheme;
	CColorSchemeList m_PresetList;
	CColorScheme::GradientStyle m_GradientList[CColorScheme::NUM_GRADIENTS];
	Theme::BorderType m_BorderList[CColorScheme::NUM_BORDERS];
	CColorScheme *m_pPreviewColorScheme;
	bool m_fPreview;
	ApplyFunc m_pApplyFunc;
	CColorPalette m_ColorPalette;

// CBasicDialog
	INT_PTR DlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);

	bool Apply(const CColorScheme *pColorScheme) const;
	void GetCurrentSettings(CColorScheme *pColorScheme);
	static const LPCTSTR m_pszExtension;
};


#endif
