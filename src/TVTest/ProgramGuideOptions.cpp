#include "stdafx.h"
#include "TVTest.h"
#include "AppMain.h"
#include "ProgramGuideOptions.h"
#include "DialogUtil.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define IEPG_ASSOCIATE_COMMAND TEXT("iEpgAssociate")




CProgramGuideOptions::CProgramGuideOptions(CProgramGuide *pProgramGuide,CPluginManager *pPluginManager)
	: m_pProgramGuide(pProgramGuide)
	, m_pPluginManager(pPluginManager)
	, m_fOnScreen(false)
	, m_BeginHour(-1)
	, m_ViewHours(26)
	, m_ItemWidth(pProgramGuide->GetItemWidth())
	, m_LinesPerHour(pProgramGuide->GetLinesPerHour())
	, m_LineMargin(1)
	, m_WheelScrollLines(pProgramGuide->GetWheelScrollLines())
	, m_VisibleEventIcons(m_pProgramGuide->GetVisibleEventIcons())
	, m_himlEventIcons(NULL)
{
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT),sizeof(LOGFONT),&m_Font);
}


CProgramGuideOptions::~CProgramGuideOptions()
{
}


bool CProgramGuideOptions::Load(LPCTSTR pszFileName)
{
	CSettings Settings;

	if (Settings.Open(pszFileName,TEXT("ProgramGuide"),CSettings::OPEN_READ)) {
		int Value;

		Settings.Read(TEXT("OnScreen"),&m_fOnScreen);
		if (Settings.Read(TEXT("BeginHour"),&Value)
				&& Value>=-1 && Value<=23)
			m_BeginHour=Value;
		m_pProgramGuide->SetBeginHour(m_BeginHour);
		if (Settings.Read(TEXT("ViewHours"),&Value)
				&& Value>=MIN_VIEW_HOURS && Value<=MAX_VIEW_HOURS)
			m_ViewHours=Value;
		if (Settings.Read(TEXT("ItemWidth"),&Value)
				&& Value>=CProgramGuide::MIN_ITEM_WIDTH
				&& Value<=CProgramGuide::MAX_ITEM_WIDTH)
			m_ItemWidth=Value;
		if (Settings.Read(TEXT("LinesPerHour"),&Value)
				&& Value>=CProgramGuide::MIN_LINES_PER_HOUR
				&& Value<=CProgramGuide::MAX_LINES_PER_HOUR)
			m_LinesPerHour=Value;
		if (Settings.Read(TEXT("LineMargin"),&Value))
			m_LineMargin=max(Value,0);
		m_pProgramGuide->SetUIOptions(m_LinesPerHour,m_ItemWidth,m_LineMargin);

		if (Settings.Read(TEXT("WheelScrollLines"),&Value))
			m_WheelScrollLines=Value;
		m_pProgramGuide->SetWheelScrollLines(m_WheelScrollLines);

		TCHAR szText[512];
		if (Settings.Read(TEXT("ProgramLDoubleClick"),szText,lengthof(szText))) {
			if (szText[0]!=_T('\0'))
				m_ProgramLDoubleClickCommand.Set(szText);
			else
				m_ProgramLDoubleClickCommand.Clear();
		}

		Settings.Read(TEXT("EventIcons"),&m_VisibleEventIcons);
		m_pProgramGuide->SetVisibleEventIcons(m_VisibleEventIcons);

		bool f;
		if (Settings.Read(TEXT("ShowToolTip"),&f))
			m_pProgramGuide->SetShowToolTip(f);

		// Font
		TCHAR szFont[LF_FACESIZE];
		if (Settings.Read(TEXT("FontName"),szFont,LF_FACESIZE) && szFont[0]!=_T('\0')) {
			lstrcpy(m_Font.lfFaceName,szFont);
			m_Font.lfEscapement=0;
			m_Font.lfOrientation=0;
			m_Font.lfUnderline=0;
			m_Font.lfStrikeOut=0;
			m_Font.lfCharSet=DEFAULT_CHARSET;
			m_Font.lfOutPrecision=OUT_DEFAULT_PRECIS;
			m_Font.lfClipPrecision=CLIP_DEFAULT_PRECIS;
			m_Font.lfQuality=DRAFT_QUALITY;
			m_Font.lfPitchAndFamily=DEFAULT_PITCH | FF_DONTCARE;
		}
		if (Settings.Read(TEXT("FontSize"),&Value)) {
			m_Font.lfHeight=Value;
			m_Font.lfWidth=0;
		}
		if (Settings.Read(TEXT("FontWeight"),&Value))
			m_Font.lfWeight=Value;
		if (Settings.Read(TEXT("FontItalic"),&Value))
			m_Font.lfItalic=Value;
		m_pProgramGuide->SetFont(&m_Font);

		bool fDragScroll;
		if (Settings.Read(TEXT("DragScroll"),&fDragScroll))
			m_pProgramGuide->SetDragScroll(fDragScroll);

		unsigned int Filter;
		if (Settings.Read(TEXT("Filter"),&Filter))
			m_pProgramGuide->SetFilter(Filter);

		int Width,Height;
		m_pProgramGuide->GetInfoPopupSize(&Width,&Height);
		if (Settings.Read(TEXT("InfoPopupWidth"),&Width)
				|| Settings.Read(TEXT("InfoPopupHeight"),&Height))
			m_pProgramGuide->SetInfoPopupSize(Width,Height);

		CProgramSearch *pProgramSearch=m_pProgramGuide->GetProgramSearch();
		int NumSearchKeywords;
		if (Settings.Read(TEXT("NumSearchKeywords"),&NumSearchKeywords)
				&& NumSearchKeywords>0) {
			if (NumSearchKeywords>CProgramSearch::MAX_KEYWORD_HISTORY)
				NumSearchKeywords=CProgramSearch::MAX_KEYWORD_HISTORY;
			LPTSTR *ppszKeywords=new LPTSTR[NumSearchKeywords];
			int j=0;
			for (int i=0;i<NumSearchKeywords;i++) {
				TCHAR szName[32],szKeyword[CProgramSearch::MAX_KEYWORD_LENGTH];

				::wsprintf(szName,TEXT("SearchKeyword%d"),i);
				if (Settings.Read(szName,szKeyword,lengthof(szKeyword))
						&& szKeyword[0]!='\0')
					ppszKeywords[j++]=DuplicateString(szKeyword);
			}
			if (j>0) {
				pProgramSearch->SetKeywordHistory(ppszKeywords,j);
				for (j--;j>=0;j--)
					delete [] ppszKeywords[j];
			}
			delete [] ppszKeywords;
		}
		for (int i=0;i<CProgramSearch::NUM_COLUMNS;i++) {
			TCHAR szName[32];

			::wsprintf(szName,TEXT("SearchColumn%d_Width"),i);
			if (Settings.Read(szName,&Value))
				pProgramSearch->SetColumnWidth(i,Value);
		}
		int Left,Top;
		pProgramSearch->GetPosition(&Left,&Top,&Width,&Height);
		Settings.Read(TEXT("SearchLeft"),&Left);
		Settings.Read(TEXT("SearchTop"),&Top);
		Settings.Read(TEXT("SearchWidth"),&Width);
		Settings.Read(TEXT("SearchHeight"),&Height);
		pProgramSearch->SetPosition(Left,Top,Width,Height);

		Settings.Close();
	}

	if (Settings.Open(pszFileName,TEXT("ProgramGuideTools"),CSettings::OPEN_READ)) {
		int NumTools;

		if (Settings.Read(TEXT("ToolCount"),&NumTools) && NumTools>0) {
			CProgramGuideToolList *pToolList=m_pProgramGuide->GetToolList();

			for (int i=0;i<NumTools;i++) {
				TCHAR szName[16];
				TCHAR szToolName[CProgramGuideTool::MAX_NAME];
				TCHAR szCommand[CProgramGuideTool::MAX_COMMAND];

				wsprintf(szName,TEXT("Tool%d_Name"),i);
				if (!Settings.Read(szName,szToolName,lengthof(szToolName))
						|| szToolName[0]=='\0')
					break;
				wsprintf(szName,TEXT("Tool%d_Command"),i);
				if (!Settings.Read(szName,szCommand,lengthof(szCommand))
						|| szCommand[0]=='\0')
					break;
				pToolList->Add(new CProgramGuideTool(szToolName,szCommand));
			}
		}
		Settings.Close();
	}
	return true;
}


bool CProgramGuideOptions::Save(LPCTSTR pszFileName) const
{
	CSettings Settings;

	if (Settings.Open(pszFileName,TEXT("ProgramGuide"),CSettings::OPEN_WRITE)) {
		Settings.Write(TEXT("OnScreen"),m_fOnScreen);
		Settings.Write(TEXT("BeginHour"),m_BeginHour);
		Settings.Write(TEXT("ViewHours"),m_ViewHours);
		Settings.Write(TEXT("ItemWidth"),m_ItemWidth);
		Settings.Write(TEXT("LinesPerHour"),m_LinesPerHour);
		Settings.Write(TEXT("LineMargin"),m_LineMargin);
		Settings.Write(TEXT("WheelScrollLines"),m_WheelScrollLines);
		Settings.Write(TEXT("ProgramLDoubleClick"),m_ProgramLDoubleClickCommand.Get());
		Settings.Write(TEXT("EventIcons"),m_VisibleEventIcons);

		// Font
		Settings.Write(TEXT("FontName"),m_Font.lfFaceName);
		Settings.Write(TEXT("FontSize"),(int)m_Font.lfHeight);
		Settings.Write(TEXT("FontWeight"),(int)m_Font.lfWeight);
		Settings.Write(TEXT("FontItalic"),(int)m_Font.lfItalic);

		Settings.Write(TEXT("DragScroll"),m_pProgramGuide->GetDragScroll());
		Settings.Write(TEXT("ShowToolTip"),m_pProgramGuide->GetShowToolTip());
		Settings.Write(TEXT("Filter"),m_pProgramGuide->GetFilter());

		int Width,Height;
		m_pProgramGuide->GetInfoPopupSize(&Width,&Height);
		Settings.Write(TEXT("InfoPopupWidth"),Width);
		Settings.Write(TEXT("InfoPopupHeight"),Height);

		const CProgramSearch *pProgramSearch=m_pProgramGuide->GetProgramSearch();
		int NumSearchKeywords=pProgramSearch->GetKeywordHistoryCount();
		Settings.Write(TEXT("NumSearchKeywords"),NumSearchKeywords);
		for (int i=0;i<NumSearchKeywords;i++) {
			TCHAR szName[32];

			::wsprintf(szName,TEXT("SearchKeyword%d"),i);
			Settings.Write(szName,pProgramSearch->GetKeywordHistory(i));
		}
		for (int i=0;i<CProgramSearch::NUM_COLUMNS;i++) {
			TCHAR szName[32];

			::wsprintf(szName,TEXT("SearchColumn%d_Width"),i);
			Settings.Write(szName,pProgramSearch->GetColumnWidth(i));
		}
		int Left,Top;
		pProgramSearch->GetPosition(&Left,&Top,&Width,&Height);
		Settings.Write(TEXT("SearchLeft"),Left);
		Settings.Write(TEXT("SearchTop"),Top);
		Settings.Write(TEXT("SearchWidth"),Width);
		Settings.Write(TEXT("SearchHeight"),Height);

		Settings.Close();
	}

	if (Settings.Open(pszFileName,TEXT("ProgramGuideTools"),CSettings::OPEN_WRITE)) {
		const CProgramGuideToolList *pToolList=m_pProgramGuide->GetToolList();

		Settings.Clear();
		Settings.Write(TEXT("ToolCount"),pToolList->NumTools());
		for (int i=0;i<pToolList->NumTools();i++) {
			const CProgramGuideTool *pTool=pToolList->GetTool(i);
			TCHAR szName[16];

			::wsprintf(szName,TEXT("Tool%d_Name"),i);
			Settings.Write(szName,pTool->GetName());
			::wsprintf(szName,TEXT("Tool%d_Command"),i);
			Settings.Write(szName,pTool->GetCommand());
		}
		Settings.Close();
	}
	return true;
}


bool CProgramGuideOptions::GetTimeRange(SYSTEMTIME *pstFirst,SYSTEMTIME *pstLast)
{
	SYSTEMTIME st;

	::GetLocalTime(&st);
	st.wMinute=0;
	st.wSecond=0;
	st.wMilliseconds=0;
	*pstFirst=st;
	OffsetSystemTime(&st,(LONGLONG)m_ViewHours*(60*60*1000));
	*pstLast=st;
	return true;
}


int CProgramGuideOptions::ParseCommand(LPCTSTR pszCommand) const
{
	if (IsStringEmpty(pszCommand))
		return 0;
	if (::lstrcmpi(pszCommand,IEPG_ASSOCIATE_COMMAND)==0)
		return CM_PROGRAMGUIDE_IEPGASSOCIATE;
	return 0;
}


CProgramGuideOptions *CProgramGuideOptions::GetThis(HWND hDlg)
{
	return static_cast<CProgramGuideOptions*>(GetOptions(hDlg));
}


static void SetFontInfo(HWND hDlg,const LOGFONT *plf)
{
	HDC hdc;
	TCHAR szText[LF_FACESIZE+16];

	hdc=GetDC(hDlg);
	if (hdc==NULL)
		return;
	wsprintf(szText,TEXT("%s, %d pt"),plf->lfFaceName,CalcFontPointHeight(hdc,plf));
	SetDlgItemText(hDlg,IDC_PROGRAMGUIDEOPTIONS_FONTINFO,szText);
	ReleaseDC(hDlg,hdc);
}


INT_PTR CALLBACK CProgramGuideOptions::DlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	static LOGFONT lfCurFont;

	switch (uMsg) {
	case WM_INITDIALOG:
		{
			CProgramGuideOptions *pThis=static_cast<CProgramGuideOptions*>(OnInitDialog(hDlg,lParam));

			DlgCheckBox_Check(hDlg,IDC_PROGRAMGUIDEOPTIONS_ONSCREEN,pThis->m_fOnScreen);
			DlgComboBox_AddString(hDlg,IDC_PROGRAMGUIDEOPTIONS_BEGINHOUR,TEXT("現在時"));
			for (int i=0;i<=23;i++) {
				TCHAR szText[8];
				::wsprintf(szText,TEXT("%d時"),i);
				DlgComboBox_AddString(hDlg,IDC_PROGRAMGUIDEOPTIONS_BEGINHOUR,szText);
			}
			DlgComboBox_SetCurSel(hDlg,IDC_PROGRAMGUIDEOPTIONS_BEGINHOUR,pThis->m_BeginHour+1);
			::SetDlgItemInt(hDlg,IDC_PROGRAMGUIDEOPTIONS_VIEWHOURS,pThis->m_ViewHours,TRUE);
			::SendDlgItemMessage(hDlg,IDC_PROGRAMGUIDEOPTIONS_VIEWHOURS_UD,
								 UDM_SETRANGE32,MIN_VIEW_HOURS,MAX_VIEW_HOURS);
			::SetDlgItemInt(hDlg,IDC_PROGRAMGUIDEOPTIONS_CHANNELWIDTH,pThis->m_ItemWidth,TRUE);
			::SendDlgItemMessage(hDlg,IDC_PROGRAMGUIDEOPTIONS_CHANNELWIDTH_UD,
				UDM_SETRANGE32,CProgramGuide::MIN_ITEM_WIDTH,CProgramGuide::MAX_ITEM_WIDTH);
			::SetDlgItemInt(hDlg,IDC_PROGRAMGUIDEOPTIONS_LINESPERHOUR,pThis->m_LinesPerHour,TRUE);
			::SendDlgItemMessage(hDlg,IDC_PROGRAMGUIDEOPTIONS_LINESPERHOUR_UD,
				UDM_SETRANGE32,CProgramGuide::MIN_LINES_PER_HOUR,CProgramGuide::MAX_LINES_PER_HOUR);
			::SetDlgItemInt(hDlg,IDC_PROGRAMGUIDEOPTIONS_LINEMARGIN,pThis->m_LineMargin,TRUE);
			::SendDlgItemMessage(hDlg,IDC_PROGRAMGUIDEOPTIONS_LINEMARGIN_UD,UDM_SETRANGE32,0,100);
			::SetDlgItemInt(hDlg,IDC_PROGRAMGUIDEOPTIONS_WHEELSCROLLLINES,pThis->m_WheelScrollLines,TRUE);
			::SendDlgItemMessage(hDlg,IDC_PROGRAMGUIDEOPTIONS_WHEELSCROLLLINES_UD,
				UDM_SETRANGE,0,MAKELONG(UD_MAXVAL,UD_MINVAL));

			int Sel=pThis->m_ProgramLDoubleClickCommand.IsEmpty()?0:-1;
			DlgComboBox_AddString(hDlg,IDC_PROGRAMGUIDEOPTIONS_PROGRAMLDOUBLECLICK,TEXT("何もしない"));
			DlgComboBox_AddString(hDlg,IDC_PROGRAMGUIDEOPTIONS_PROGRAMLDOUBLECLICK,TEXT("iEPG関連付け実行"));
			if (Sel<0 && pThis->m_ProgramLDoubleClickCommand.Compare(IEPG_ASSOCIATE_COMMAND)==0)
				Sel=1;
			for (int i=0;i<pThis->m_pPluginManager->NumPlugins();i++) {
				const CPlugin *pPlugin=pThis->m_pPluginManager->GetPlugin(i);

				for (int j=0;j<pPlugin->NumProgramGuideCommands();j++) {
					TVTest::ProgramGuideCommandInfo CommandInfo;

					pPlugin->GetProgramGuideCommandInfo(j,&CommandInfo);
					if (CommandInfo.Type==TVTest::PROGRAMGUIDE_COMMAND_TYPE_PROGRAM) {
						TCHAR szCommand[512];

						::wsprintf(szCommand,TEXT("%s:%s"),
								   ::PathFindFileName(pPlugin->GetFileName()),
								   CommandInfo.pszText);
						LRESULT Index=DlgComboBox_AddString(hDlg,IDC_PROGRAMGUIDEOPTIONS_PROGRAMLDOUBLECLICK,
															CommandInfo.pszName);
						DlgComboBox_SetItemData(hDlg,IDC_PROGRAMGUIDEOPTIONS_PROGRAMLDOUBLECLICK,
												Index,reinterpret_cast<LPARAM>(DuplicateString(szCommand)));
						if (Sel<0 && !pThis->m_ProgramLDoubleClickCommand.IsEmpty()
								&& ::lstrcmpi(szCommand,pThis->m_ProgramLDoubleClickCommand.Get())==0)
							Sel=(int)Index;
					}
				}
			}
			if (Sel>=0)
				DlgComboBox_SetCurSel(hDlg,IDC_PROGRAMGUIDEOPTIONS_PROGRAMLDOUBLECLICK,Sel);

			lfCurFont=pThis->m_Font;
			SetFontInfo(hDlg,&lfCurFont);

			pThis->m_Tooltip.Create(hDlg);
			pThis->m_himlEventIcons=::ImageList_LoadImage(
				GetAppClass().GetResourceInstance(),MAKEINTRESOURCE(IDB_PROGRAMGUIDEICONS),
				CEpgIcons::ICON_WIDTH,1,CLR_NONE,IMAGE_BITMAP,LR_CREATEDIBSECTION);
			for (int i=0;i<=CEpgIcons::ICON_LAST;i++) {
				DlgCheckBox_Check(hDlg,IDC_PROGRAMGUIDEOPTIONS_ICON_FIRST+i,
								  (pThis->m_VisibleEventIcons&CEpgIcons::IconFlag(i))!=0);
				::SendDlgItemMessage(hDlg,IDC_PROGRAMGUIDEOPTIONS_ICON_FIRST+i,
									 BM_SETIMAGE,IMAGE_ICON,
									 reinterpret_cast<LPARAM>(::ImageList_ExtractIcon(NULL,pThis->m_himlEventIcons,i)));
				TCHAR szText[64];
				::GetDlgItemText(hDlg,IDC_PROGRAMGUIDEOPTIONS_ICON_FIRST+i,szText,lengthof(szText));
				pThis->m_Tooltip.AddTool(::GetDlgItem(hDlg,IDC_PROGRAMGUIDEOPTIONS_ICON_FIRST+i),szText);
			}

			CProgramGuideToolList *pToolList=pThis->m_pProgramGuide->GetToolList();
			HWND hwndList=GetDlgItem(hDlg,IDC_PROGRAMGUIDETOOL_LIST);
			HIMAGELIST himl=::ImageList_Create(
				::GetSystemMetrics(SM_CXSMICON),::GetSystemMetrics(SM_CYSMICON),
				ILC_COLOR32 | ILC_MASK,1,4);
			RECT rc;
			LV_COLUMN lvc;

			ListView_SetImageList(hwndList,himl,LVSIL_SMALL);
			ListView_SetExtendedListViewStyle(hwndList,LVS_EX_FULLROWSELECT);
			lvc.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
			lvc.fmt=LVCFMT_LEFT;
			lvc.cx=80;
			lvc.pszText=TEXT("名前");
			ListView_InsertColumn(hwndList,0,&lvc);
			lvc.fmt=LVCFMT_LEFT;
			GetClientRect(hwndList,&rc);
			rc.right-=GetSystemMetrics(SM_CXVSCROLL);
			lvc.cx=rc.right-lvc.cx;
			lvc.pszText=TEXT("コマンド");
			ListView_InsertColumn(hwndList,1,&lvc);
			if (pToolList->NumTools()>0) {
				ListView_SetItemCount(hwndList,pToolList->NumTools());
				for (int i=0;i<pToolList->NumTools();i++) {
					CProgramGuideTool *pTool=new CProgramGuideTool(*pToolList->GetTool(i));
					LV_ITEM lvi;

					lvi.mask=LVIF_TEXT | LVIF_PARAM;
					lvi.iItem=i;
					lvi.iSubItem=0;
					lvi.pszText=const_cast<LPTSTR>(pTool->GetName());
					lvi.lParam=reinterpret_cast<LPARAM>(pTool);
					if (pTool->GetIcon()!=NULL) {
						lvi.iImage=::ImageList_AddIcon(himl,pTool->GetIcon());
						if (lvi.iImage>=0)
							lvi.mask|=LVIF_IMAGE;
					}
					ListView_InsertItem(hwndList,&lvi);
					lvi.mask=LVIF_TEXT;
					lvi.iSubItem=1;
					lvi.pszText=const_cast<LPTSTR>(pTool->GetCommand());
					ListView_SetItem(hwndList,&lvi);
				}
				ListView_SetColumnWidth(hwndList,0,LVSCW_AUTOSIZE_USEHEADER);
				int Width=ListView_GetColumnWidth(hwndList,0);
				ListView_SetColumnWidth(hwndList,1,LVSCW_AUTOSIZE_USEHEADER);
				if (ListView_GetColumnWidth(hwndList,1)<rc.right-Width)
					ListView_SetColumnWidth(hwndList,1,rc.right-Width);
				pThis->SetDlgItemState();
			}
		}
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_PROGRAMGUIDEOPTIONS_CHOOSEFONT:
			if (ChooseFontDialog(hDlg,&lfCurFont))
				SetFontInfo(hDlg,&lfCurFont);
			return TRUE;

		case IDC_PROGRAMGUIDETOOL_ADD:
			{
				CProgramGuideTool *pTool;

				pTool=new CProgramGuideTool;
				if (pTool->ShowDialog(hDlg)) {
					HWND hwndList=GetDlgItem(hDlg,IDC_PROGRAMGUIDETOOL_LIST);
					LV_ITEM lvi;

					lvi.mask=LVIF_STATE | LVIF_TEXT | LVIF_PARAM;
					lvi.iItem=ListView_GetItemCount(hwndList);
					lvi.iSubItem=0;
					lvi.state=LVIS_FOCUSED | LVIS_SELECTED;
					lvi.stateMask=lvi.state;
					lvi.pszText=const_cast<LPTSTR>(pTool->GetName());
					lvi.lParam=reinterpret_cast<LPARAM>(pTool);
					if (pTool->GetIcon()!=NULL) {
						lvi.iImage=::ImageList_AddIcon(ListView_GetImageList(hwndList,LVSIL_SMALL),pTool->GetIcon());
						if (lvi.iImage>=0)
							lvi.mask|=LVIF_IMAGE;
					}
					ListView_InsertItem(hwndList,&lvi);
					lvi.mask=LVIF_TEXT;
					lvi.iSubItem=1;
					lvi.pszText=const_cast<LPTSTR>(pTool->GetCommand());
					ListView_SetItem(hwndList,&lvi);
					ListView_EnsureVisible(hwndList,lvi.iItem,FALSE);
				} else {
					delete pTool;
				}
			}
			return TRUE;

		case IDC_PROGRAMGUIDETOOL_EDIT:
			{
				HWND hwndList=GetDlgItem(hDlg,IDC_PROGRAMGUIDETOOL_LIST);
				LV_ITEM lvi;
				CProgramGuideTool *pTool;

				lvi.mask=LVIF_PARAM;
				lvi.iItem=ListView_GetNextItem(hwndList,-1,LVNI_SELECTED);
				lvi.iSubItem=0;
				if (!ListView_GetItem(hwndList,&lvi))
					return TRUE;
				pTool=reinterpret_cast<CProgramGuideTool*>(lvi.lParam);
				if (pTool->ShowDialog(hDlg)) {
					lvi.mask=LVIF_TEXT;
					lvi.pszText=const_cast<LPTSTR>(pTool->GetName());
					if (pTool->GetIcon()!=NULL) {
						lvi.iImage=::ImageList_AddIcon(ListView_GetImageList(hwndList,LVSIL_SMALL),pTool->GetIcon());
						if (lvi.iImage>=0)
							lvi.mask|=LVIF_IMAGE;
					}
					ListView_SetItem(hwndList,&lvi);
					lvi.iSubItem=1;
					lvi.pszText=const_cast<LPTSTR>(pTool->GetCommand());
					ListView_SetItem(hwndList,&lvi);
				}
			}
			return TRUE;

		case IDC_PROGRAMGUIDETOOL_UP:
			{
				HWND hwndList=GetDlgItem(hDlg,IDC_PROGRAMGUIDETOOL_LIST);
				LV_ITEM lvi;
				CProgramGuideTool *pTool;

				lvi.mask=LVIF_IMAGE | LVIF_PARAM;
				lvi.iItem=ListView_GetNextItem(hwndList,-1,LVNI_SELECTED);
				if (lvi.iItem<=0)
					return TRUE;
				lvi.iSubItem=0;
				ListView_GetItem(hwndList,&lvi);
				pTool=reinterpret_cast<CProgramGuideTool*>(lvi.lParam);
				ListView_DeleteItem(hwndList,lvi.iItem);
				lvi.mask=LVIF_STATE | LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lvi.iItem--;
				lvi.state=LVIS_FOCUSED | LVIS_SELECTED;
				lvi.stateMask=lvi.state;
				lvi.pszText=const_cast<LPTSTR>(pTool->GetName());
				ListView_InsertItem(hwndList,&lvi);
				lvi.mask=LVIF_TEXT;
				lvi.iSubItem=1;
				lvi.pszText=const_cast<LPTSTR>(pTool->GetCommand());
				ListView_SetItem(hwndList,&lvi);
				ListView_EnsureVisible(hwndList,lvi.iItem,FALSE);
			}
			return TRUE;

		case IDC_PROGRAMGUIDETOOL_DOWN:
			{
				HWND hwndList=GetDlgItem(hDlg,IDC_PROGRAMGUIDETOOL_LIST);
				LV_ITEM lvi;
				CProgramGuideTool *pTool;

				lvi.mask=LVIF_IMAGE | LVIF_PARAM;
				lvi.iItem=ListView_GetNextItem(hwndList,-1,LVNI_SELECTED);
				if (lvi.iItem<0
						|| lvi.iItem+1==ListView_GetItemCount(hwndList))
					return TRUE;
				lvi.iSubItem=0;
				ListView_GetItem(hwndList,&lvi);
				pTool=reinterpret_cast<CProgramGuideTool*>(lvi.lParam);
				ListView_DeleteItem(hwndList,lvi.iItem);
				lvi.mask=LVIF_STATE | LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lvi.iItem++;
				lvi.state=LVIS_FOCUSED | LVIS_SELECTED;
				lvi.stateMask=lvi.state;
				lvi.pszText=const_cast<LPTSTR>(pTool->GetName());
				ListView_InsertItem(hwndList,&lvi);
				lvi.mask=LVIF_TEXT;
				lvi.iSubItem=1;
				lvi.pszText=const_cast<LPTSTR>(pTool->GetCommand());
				ListView_SetItem(hwndList,&lvi);
				ListView_EnsureVisible(hwndList,lvi.iItem,FALSE);
			}
			return TRUE;

		case IDC_PROGRAMGUIDETOOL_REMOVE:
			{
				HWND hwndList=GetDlgItem(hDlg,IDC_PROGRAMGUIDETOOL_LIST);
				LV_ITEM lvi;

				lvi.mask=LVIF_PARAM;
				lvi.iItem=ListView_GetNextItem(hwndList,-1,LVNI_SELECTED);
				lvi.iSubItem=0;
				if (ListView_GetItem(hwndList,&lvi)) {
					CProgramGuideTool *pTool;

					ListView_DeleteItem(hwndList,lvi.iItem);
					pTool=reinterpret_cast<CProgramGuideTool*>(lvi.lParam);
					delete pTool;
				}
			}
			return TRUE;

		case IDC_PROGRAMGUIDETOOL_REMOVEALL:
			{
				CProgramGuideOptions *pThis=GetThis(hDlg);
				HWND hwndList=GetDlgItem(hDlg,IDC_PROGRAMGUIDETOOL_LIST);

				pThis->DeleteAllTools();
				ListView_DeleteAllItems(hwndList);
				pThis->SetDlgItemState();
			}
			return TRUE;
		}
		return TRUE;

	case WM_NOTIFY:
		switch (reinterpret_cast<LPNMHDR>(lParam)->code) {
		case LVN_ITEMCHANGED:
			GetThis(hDlg)->SetDlgItemState();
			return TRUE;

		case NM_RCLICK:
			{
				LPNMITEMACTIVATE pnmia=(LPNMITEMACTIVATE)lParam;
				HWND hwndList=GetDlgItem(hDlg,IDC_PROGRAMGUIDETOOL_LIST);

				if (pnmia->hdr.hwndFrom==hwndList
						&& ListView_GetNextItem(hwndList,-1,LVNI_SELECTED)>=0) {
					static const int MenuIDs[] = {
						IDC_PROGRAMGUIDETOOL_EDIT,
						0,
						IDC_PROGRAMGUIDETOOL_UP,
						IDC_PROGRAMGUIDETOOL_DOWN,
						0,
						IDC_PROGRAMGUIDETOOL_REMOVE
					};

					PopupMenuFromControls(hDlg,MenuIDs,lengthof(MenuIDs),TPM_RIGHTBUTTON);
				}
			}
			return TRUE;

		case PSN_APPLY:
			{
				CProgramGuideOptions *pThis=GetThis(hDlg);
				int Value;
				bool fUpdate=false;

				pThis->m_fOnScreen=DlgCheckBox_IsChecked(hDlg,IDC_PROGRAMGUIDEOPTIONS_ONSCREEN);
				Value=(int)DlgComboBox_GetCurSel(hDlg,IDC_PROGRAMGUIDEOPTIONS_BEGINHOUR)-1;
				if (pThis->m_BeginHour!=Value) {
					pThis->m_BeginHour=Value;
					pThis->m_pProgramGuide->SetBeginHour(Value);
					fUpdate=true;
				}
				Value=::GetDlgItemInt(hDlg,IDC_PROGRAMGUIDEOPTIONS_VIEWHOURS,NULL,TRUE);
				Value=CLAMP(Value,(int)MIN_VIEW_HOURS,(int)MAX_VIEW_HOURS);
				if (pThis->m_ViewHours!=Value) {
					SYSTEMTIME stFirst,stLast;

					pThis->m_ViewHours=Value;
					pThis->m_pProgramGuide->GetTimeRange(&stFirst,NULL);
					stLast=stFirst;
					OffsetSystemTime(&stLast,(LONGLONG)pThis->m_ViewHours*(60*60*1000));
					pThis->m_pProgramGuide->SetTimeRange(&stFirst,&stLast);
					fUpdate=true;
				}
				if (fUpdate)
					pThis->m_pProgramGuide->UpdateProgramGuide();
				Value=::GetDlgItemInt(hDlg,IDC_PROGRAMGUIDEOPTIONS_CHANNELWIDTH,NULL,TRUE);
				pThis->m_ItemWidth=CLAMP(Value,
					(int)CProgramGuide::MIN_ITEM_WIDTH,(int)CProgramGuide::MAX_ITEM_WIDTH);
				Value=::GetDlgItemInt(hDlg,IDC_PROGRAMGUIDEOPTIONS_LINESPERHOUR,NULL,TRUE);
				pThis->m_LinesPerHour=CLAMP(Value,
					(int)CProgramGuide::MIN_LINES_PER_HOUR,(int)CProgramGuide::MAX_LINES_PER_HOUR);
				pThis->m_LineMargin=::GetDlgItemInt(hDlg,IDC_PROGRAMGUIDEOPTIONS_LINEMARGIN,NULL,TRUE);
				pThis->m_pProgramGuide->SetUIOptions(pThis->m_LinesPerHour,pThis->m_ItemWidth,pThis->m_LineMargin);

				pThis->m_WheelScrollLines=::GetDlgItemInt(hDlg,IDC_PROGRAMGUIDEOPTIONS_WHEELSCROLLLINES,NULL,TRUE);
				pThis->m_pProgramGuide->SetWheelScrollLines(pThis->m_WheelScrollLines);

				LRESULT Sel=DlgComboBox_GetCurSel(hDlg,IDC_PROGRAMGUIDEOPTIONS_PROGRAMLDOUBLECLICK);
				if (Sel>=0) {
					if (Sel==0) {
						pThis->m_ProgramLDoubleClickCommand.Clear();
					} else if (Sel==1) {
						pThis->m_ProgramLDoubleClickCommand.Set(IEPG_ASSOCIATE_COMMAND);
					} else {
						pThis->m_ProgramLDoubleClickCommand.Set(
							reinterpret_cast<LPCTSTR>(
								DlgComboBox_GetItemData(hDlg,IDC_PROGRAMGUIDEOPTIONS_PROGRAMLDOUBLECLICK,Sel)));
					}
				}

				if (!CompareLogFont(&pThis->m_Font,&lfCurFont)) {
					pThis->m_Font=lfCurFont;
					pThis->m_pProgramGuide->SetFont(&lfCurFont);
				}

				UINT VisibleEventIcons=0;
				for (int i=0;i<=CEpgIcons::ICON_LAST;i++) {
					if (DlgCheckBox_IsChecked(hDlg,IDC_PROGRAMGUIDEOPTIONS_ICON_FIRST+i))
						VisibleEventIcons|=CEpgIcons::IconFlag(i);
				}
				if (pThis->m_VisibleEventIcons!=VisibleEventIcons) {
					pThis->m_VisibleEventIcons=VisibleEventIcons;
					pThis->m_pProgramGuide->SetVisibleEventIcons(VisibleEventIcons);
					pThis->m_UpdateFlags|=UPDATE_EVENTICONS;
				}

				CProgramGuideToolList *pToolList=pThis->m_pProgramGuide->GetToolList();
				HWND hwndList=GetDlgItem(hDlg,IDC_PROGRAMGUIDETOOL_LIST);
				int Items,i;

				pToolList->Clear();
				Items=ListView_GetItemCount(hwndList);
				if (Items>0) {
					LV_ITEM lvi;

					lvi.mask=LVIF_PARAM;
					lvi.iSubItem=0;
					for (i=0;i<Items;i++) {
						lvi.iItem=i;
						ListView_GetItem(hwndList,&lvi);
						pToolList->Add(reinterpret_cast<CProgramGuideTool*>(lvi.lParam));
					}
				}
			}
			break;

		case PSN_RESET:
			GetThis(hDlg)->DeleteAllTools();
			break;
		}
		break;

	case WM_DESTROY:
		{
			CProgramGuideOptions *pThis=GetThis(hDlg);

			for (LRESULT i=DlgComboBox_GetCount(hDlg,IDC_PROGRAMGUIDEOPTIONS_PROGRAMLDOUBLECLICK)-1;i>1;i--) {
				delete [] reinterpret_cast<LPTSTR>(
					DlgComboBox_GetItemData(hDlg,IDC_PROGRAMGUIDEOPTIONS_PROGRAMLDOUBLECLICK,i));
			}
			for (int i=0;i<=CEpgIcons::ICON_LAST;i++) {
				HICON hico=reinterpret_cast<HICON>(
					::SendDlgItemMessage(hDlg,IDC_PROGRAMGUIDEOPTIONS_ICON_FIRST+i,
										 BM_SETIMAGE,IMAGE_ICON,reinterpret_cast<LPARAM>((HICON)NULL)));
				if (hico!=NULL)
					::DestroyIcon(hico);
			}
			if (pThis->m_himlEventIcons!=NULL) {
				::ImageList_Destroy(pThis->m_himlEventIcons);
				pThis->m_himlEventIcons=NULL;
			}
			pThis->m_Tooltip.Destroy();
			pThis->OnDestroy();
		}
		return TRUE;
	}
	return FALSE;
}


void CProgramGuideOptions::SetDlgItemState()
{
	HWND hwndList=::GetDlgItem(m_hDlg,IDC_PROGRAMGUIDETOOL_LIST);
	int Items,Sel;

	Items=ListView_GetItemCount(hwndList);
	Sel=ListView_GetNextItem(hwndList,-1,LVNI_SELECTED);
	EnableDlgItem(m_hDlg,IDC_PROGRAMGUIDETOOL_EDIT,Sel>=0);
	EnableDlgItem(m_hDlg,IDC_PROGRAMGUIDETOOL_UP,Sel>0);
	EnableDlgItem(m_hDlg,IDC_PROGRAMGUIDETOOL_DOWN,Sel>=0 && Sel+1<Items);
	EnableDlgItem(m_hDlg,IDC_PROGRAMGUIDETOOL_REMOVE,Sel>=0);
	EnableDlgItem(m_hDlg,IDC_PROGRAMGUIDETOOL_REMOVEALL,Items>0);
}


void CProgramGuideOptions::DeleteAllTools()
{
	HWND hwndList=::GetDlgItem(m_hDlg,IDC_PROGRAMGUIDETOOL_LIST);
	int Items;

	Items=ListView_GetItemCount(hwndList);
	if (Items>0) {
		LV_ITEM lvi;
		int i;
		CProgramGuideTool *pTool;

		lvi.mask=LVIF_PARAM;
		lvi.iSubItem=0;
		for (i=Items-1;i>=0;i--) {
			lvi.iItem=i;
			ListView_GetItem(hwndList,&lvi);
			pTool=reinterpret_cast<CProgramGuideTool*>(lvi.lParam);
			delete pTool;
		}
	}
}
