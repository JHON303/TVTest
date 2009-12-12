#include "stdafx.h"
#include "TVTest.h"
#include "AppMain.h"
#include "InformationPanel.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define IDC_PROGRAMINFO		1000
#define IDC_PROGRAMINFOPREV	1001
#define IDC_PROGRAMINFONEXT	1002

#define PROGRAMINFO_BUTTON_SIZE	16

#define ITEM_FLAG(item) (1<<(item))




const LPCTSTR CInformationPanel::m_pszClassName=APP_NAME TEXT(" Information Panel");
HINSTANCE CInformationPanel::m_hinst=NULL;


const LPCTSTR CInformationPanel::m_pszItemNameList[] = {
	TEXT("VideoInfo"),
	TEXT("VideoDecoder"),
	TEXT("VideoRenderer"),
	TEXT("AudioDevice"),
	TEXT("SignalLevel"),
	TEXT("Error"),
	TEXT("Record"),
	TEXT("ProgramInfo"),
};


bool CInformationPanel::Initialize(HINSTANCE hinst)
{
	if (m_hinst==NULL) {
		WNDCLASS wc;

		wc.style=CS_HREDRAW;
		wc.lpfnWndProc=WndProc;
		wc.cbClsExtra=0;
		wc.cbWndExtra=0;
		wc.hInstance=hinst;
		wc.hIcon=NULL;
		wc.hCursor=LoadCursor(NULL,IDC_ARROW);
		wc.hbrBackground=NULL;
		wc.lpszMenuName=NULL;
		wc.lpszClassName=m_pszClassName;
		if (::RegisterClass(&wc)==0)
			return false;
		m_hinst=hinst;
	}
	return true;
}


CInformationPanel::CInformationPanel()
	: m_hwndProgramInfo(NULL)
	, m_pOldProgramInfoProc(NULL)
	, m_hwndProgramInfoPrev(NULL)
	, m_hwndProgramInfoNext(NULL)
	, m_hbrBack(NULL)
	, m_hbrProgramInfoBack(NULL)
	, m_hFont(NULL)
	, m_pEventHandler(NULL)
{
	m_crBackColor=RGB(0,0,0);
	m_crTextColor=RGB(255,255,255);
	m_crProgramInfoBackColor=RGB(0,0,0);
	m_crProgramInfoTextColor=RGB(255,255,255);
	m_LineMargin=1;
	m_ItemVisibility=(unsigned int)-1;

	m_OriginalVideoWidth=0;
	m_OriginalVideoHeight=0;
	m_DisplayVideoWidth=0;
	m_DisplayVideoHeight=0;
	m_AspectX=0;
	m_AspectY=0;
	m_fSignalLevel=false;
	m_SignalLevel=0.0f;
	m_fBitRate=false;
	m_BitRate=0.0f;
	m_fRecording=false;
	m_fNextProgramInfo=false;
}


CInformationPanel::~CInformationPanel()
{
	if (m_hFont!=NULL)
		::DeleteObject(m_hFont);
}


bool CInformationPanel::Create(HWND hwndParent,DWORD Style,DWORD ExStyle,int ID)
{
	return CreateBasicWindow(hwndParent,Style,ExStyle,ID,
							 m_pszClassName,TEXT("���"),m_hinst);
}


void CInformationPanel::ResetStatistics()
{
	/*
	m_OriginalVideoWidth=0;
	m_OriginalVideoHeight=0;
	m_DisplayVideoWidth=0;
	m_DisplayVideoHeight=0;
	*/
	m_AspectX=0;
	m_AspectY=0;
	//m_VideoDecoderName.Clear();
	//m_VideoRendererName.Clear();
	//m_AudioDeviceName.Clear();
	m_SignalLevel=0.0f;
	m_fBitRate=false;
	m_BitRate=0.0f;
	//m_fRecording=false;
	m_ProgramInfo.Clear();
	m_fNextProgramInfo=false;
	if (m_hwnd!=NULL) {
		InvalidateRect(m_hwnd,NULL,TRUE);
		SetWindowText(m_hwndProgramInfo,TEXT(""));
		EnableWindow(m_hwndProgramInfoPrev,FALSE);
		EnableWindow(m_hwndProgramInfoNext,TRUE);
	}
}


bool CInformationPanel::IsVisible() const
{
	return m_hwnd!=NULL;
}


void CInformationPanel::SetColor(COLORREF crBackColor,COLORREF crTextColor)
{
	m_crBackColor=crBackColor;
	m_crTextColor=crTextColor;
	if (m_hwnd!=NULL) {
		::DeleteObject(m_hbrBack);
		m_hbrBack=CreateSolidBrush(crBackColor);
		::InvalidateRect(m_hwnd,NULL,TRUE);
		::InvalidateRect(m_hwndProgramInfoPrev,NULL,TRUE);
		::InvalidateRect(m_hwndProgramInfoNext,NULL,TRUE);
	}
}


void CInformationPanel::SetProgramInfoColor(COLORREF crBackColor,COLORREF crTextColor)
{
	m_crProgramInfoBackColor=crBackColor;
	m_crProgramInfoTextColor=crTextColor;
	if (m_hwnd!=NULL) {
		::DeleteObject(m_hbrProgramInfoBack);
		m_hbrProgramInfoBack=CreateSolidBrush(crBackColor);
		::InvalidateRect(m_hwndProgramInfo,NULL,TRUE);
	}
}


bool CInformationPanel::SetFont(const LOGFONT *pFont)
{
	HFONT hfont=::CreateFontIndirect(pFont);

	if (hfont==NULL)
		return false;
	if (m_hFont!=NULL)
		::DeleteObject(m_hFont);
	m_hFont=hfont;
	if (m_hwnd!=NULL) {
		CalcFontHeight();
		SetWindowFont(m_hwndProgramInfo,m_hFont,TRUE);
		RECT rc;
		GetClientRect(&rc);
		SendMessage(WM_SIZE,0,MAKELPARAM(rc.right,rc.bottom));
		Invalidate();
	}
	return true;
}


void CInformationPanel::SetVideoSize(int OriginalWidth,int OriginalHeight,int DisplayWidth,int DisplayHeight)
{
	if (OriginalWidth!=m_OriginalVideoWidth || OriginalHeight!=m_OriginalVideoHeight
			|| DisplayWidth!=m_DisplayVideoWidth || DisplayHeight!=m_DisplayVideoHeight) {
		m_OriginalVideoWidth=OriginalWidth;
		m_OriginalVideoHeight=OriginalHeight;
		m_DisplayVideoWidth=DisplayWidth;
		m_DisplayVideoHeight=DisplayHeight;
		UpdateItem(ITEM_VIDEO);
	}
}


void CInformationPanel::SetAspectRatio(int AspectX,int AspectY)
{
	if (AspectX!=m_AspectX || AspectY!=m_AspectY) {
		m_AspectX=AspectX;
		m_AspectY=AspectY;
		UpdateItem(ITEM_VIDEO);
	}
}


void CInformationPanel::SetVideoDecoderName(LPCTSTR pszName)
{
	if (m_VideoDecoderName.Compare(pszName)!=0) {
		m_VideoDecoderName.Set(pszName);
		UpdateItem(ITEM_DECODER);
	}
}


void CInformationPanel::SetVideoRendererName(LPCTSTR pszName)
{
	if (m_VideoRendererName.Compare(pszName)!=0) {
		m_VideoRendererName.Set(pszName);
		UpdateItem(ITEM_VIDEORENDERER);
	}
}


void CInformationPanel::SetAudioDeviceName(LPCTSTR pszName)
{
	if (m_AudioDeviceName.Compare(pszName)!=0) {
		m_AudioDeviceName.Set(pszName);
		UpdateItem(ITEM_AUDIODEVICE);
	}
}


void CInformationPanel::SetSignalLevel(float Level)
{
	if (!m_fSignalLevel || Level!=m_SignalLevel) {
		m_fSignalLevel=true;
		m_SignalLevel=Level;
		UpdateItem(ITEM_SIGNALLEVEL);
	}
}


void CInformationPanel::ShowSignalLevel(bool fShow)
{
	if (fShow!=m_fSignalLevel) {
		m_fSignalLevel=fShow;
		UpdateItem(ITEM_SIGNALLEVEL);
	}
}


void CInformationPanel::SetBitRate(float BitRate)
{
	if (!m_fBitRate || BitRate!=m_BitRate) {
		m_fBitRate=true;
		m_BitRate=BitRate;
		UpdateItem(ITEM_BITRATE);
	}
}


void CInformationPanel::UpdateErrorCount()
{
	UpdateItem(ITEM_ERROR);
}


void CInformationPanel::SetRecordStatus(bool fRecording,LPCTSTR pszFileName,
								ULONGLONG WroteSize,unsigned int RecordTime)
{
	m_fRecording=fRecording;
	if (fRecording) {
		m_RecordWroteSize=WroteSize;
		m_RecordTime=RecordTime;
		TCHAR szPath[MAX_PATH];
		lstrcpy(szPath,pszFileName);
		*PathFindFileName(szPath)='\0';
		if (!GetDiskFreeSpaceEx(szPath,&m_DiskFreeSpace,NULL,NULL))
			m_DiskFreeSpace.QuadPart=0;
	}
	UpdateItem(ITEM_RECORD);
}


void CInformationPanel::SetProgramInfo(LPCTSTR pszInfo)
{
	if (m_ProgramInfo.Compare(pszInfo)!=0) {
		m_ProgramInfo.Set(pszInfo);
		if (m_hwndProgramInfo!=NULL)
			::SetWindowText(m_hwndProgramInfo,m_ProgramInfo.GetSafe());
	}
}


bool CInformationPanel::SetEventHandler(CEventHandler *pHandler)
{
	m_pEventHandler=pHandler;
	return true;
}


CInformationPanel *CInformationPanel::GetThis(HWND hwnd)
{
	return static_cast<CInformationPanel*>(GetBasicWindow(hwnd));
}


LRESULT CALLBACK CInformationPanel::WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,
																LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		{
			CInformationPanel *pThis=static_cast<CInformationPanel*>(OnCreate(hwnd,lParam));

			if (pThis->m_hFont==NULL)
				pThis->m_hFont=CreateDefaultFont();
			pThis->m_hwndProgramInfo=CreateWindowEx(0,TEXT("EDIT"),
				pThis->m_ProgramInfo.GetSafe(),
				WS_CHILD | (pThis->IsItemVisible(ITEM_PROGRAMINFO)?WS_VISIBLE:0) | WS_CLIPSIBLINGS | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
				0,0,0,0,hwnd,(HMENU)IDC_PROGRAMINFO,m_hinst,NULL);
			SetWindowFont(pThis->m_hwndProgramInfo,pThis->m_hFont,FALSE);
			::SetProp(pThis->m_hwndProgramInfo,APP_NAME TEXT("This"),pThis);
			pThis->m_pOldProgramInfoProc=SubclassWindow(pThis->m_hwndProgramInfo,ProgramInfoHookProc);
			pThis->m_hwndProgramInfoPrev=CreateWindowEx(0,TEXT("BUTTON"),TEXT(""),
				WS_CHILD | (pThis->IsItemVisible(ITEM_PROGRAMINFO)?WS_VISIBLE:0) | WS_CLIPSIBLINGS | (pThis->m_fNextProgramInfo?0:WS_DISABLED)
												| BS_PUSHBUTTON | BS_OWNERDRAW,
				0,0,0,0,hwnd,(HMENU)IDC_PROGRAMINFOPREV,m_hinst,NULL);
			pThis->m_hwndProgramInfoNext=CreateWindowEx(0,TEXT("BUTTON"),TEXT(""),
				WS_CHILD | (pThis->IsItemVisible(ITEM_PROGRAMINFO)?WS_VISIBLE:0) | WS_CLIPSIBLINGS | (pThis->m_fNextProgramInfo?WS_DISABLED:0)
												| BS_PUSHBUTTON | BS_OWNERDRAW,
				0,0,0,0,hwnd,(HMENU)IDC_PROGRAMINFONEXT,m_hinst,NULL);
			pThis->m_hbrBack=CreateSolidBrush(pThis->m_crBackColor);
			pThis->m_hbrProgramInfoBack=CreateSolidBrush(pThis->m_crProgramInfoBackColor);
			pThis->CalcFontHeight();
		}
		return 0;

	case WM_SIZE:
		{
			CInformationPanel *pThis=GetThis(hwnd);

			if (pThis->IsItemVisible(ITEM_PROGRAMINFO)) {
				RECT rc;

				pThis->GetItemRect(ITEM_PROGRAMINFO,&rc);
				MoveWindow(pThis->m_hwndProgramInfo,rc.left,rc.top,
							rc.right-rc.left,rc.bottom-rc.top,TRUE);
				MoveWindow(pThis->m_hwndProgramInfoPrev,
							rc.right-PROGRAMINFO_BUTTON_SIZE*2,rc.bottom,
							PROGRAMINFO_BUTTON_SIZE,PROGRAMINFO_BUTTON_SIZE,TRUE);
				MoveWindow(pThis->m_hwndProgramInfoNext,
							rc.right-PROGRAMINFO_BUTTON_SIZE,rc.bottom,
							PROGRAMINFO_BUTTON_SIZE,PROGRAMINFO_BUTTON_SIZE,TRUE);
			}
		}
		return 0;

	case WM_PAINT:
		{
			CInformationPanel *pThis=GetThis(hwnd);
			PAINTSTRUCT ps;
			HFONT hfontOld;
			COLORREF crOldTextColor;
			int OldBkMode;
			RECT rc;
			TCHAR szText[256];

			BeginPaint(hwnd,&ps);
			hfontOld=static_cast<HFONT>(SelectObject(ps.hdc,pThis->m_hFont));
			FillRect(ps.hdc,&ps.rcPaint,pThis->m_hbrBack);
			crOldTextColor=SetTextColor(ps.hdc,pThis->m_crTextColor);
			OldBkMode=SetBkMode(ps.hdc,TRANSPARENT);
			if (pThis->IsItemVisible(ITEM_VIDEO)) {
				pThis->GetItemRect(ITEM_VIDEO,&rc);
				if (IsRectIntersect(&ps.rcPaint,&rc)) {
					wsprintf(szText,TEXT("%d x %d [%d x %d (%d:%d)]"),
						pThis->m_OriginalVideoWidth,pThis->m_OriginalVideoHeight,
						pThis->m_DisplayVideoWidth,pThis->m_DisplayVideoHeight,
						pThis->m_AspectX,pThis->m_AspectY);
					DrawText(ps.hdc,szText,-1,&rc,DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
				}
			}
			if (pThis->IsItemVisible(ITEM_DECODER)
					&& !pThis->m_VideoDecoderName.IsEmpty()) {
				pThis->GetItemRect(ITEM_DECODER,&rc);
				if (IsRectIntersect(&ps.rcPaint,&rc))
					DrawText(ps.hdc,pThis->m_VideoDecoderName.Get(),-1,&rc,
						DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
			}
			if (pThis->IsItemVisible(ITEM_VIDEORENDERER)
					&& !pThis->m_VideoRendererName.IsEmpty()) {
				pThis->GetItemRect(ITEM_VIDEORENDERER,&rc);
				if (IsRectIntersect(&ps.rcPaint,&rc))
					DrawText(ps.hdc,pThis->m_VideoRendererName.Get(),-1,&rc,
						DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
			}
			if (pThis->IsItemVisible(ITEM_AUDIODEVICE)
					&& !pThis->m_AudioDeviceName.IsEmpty()) {
				pThis->GetItemRect(ITEM_AUDIODEVICE,&rc);
				if (IsRectIntersect(&ps.rcPaint,&rc))
					DrawText(ps.hdc,pThis->m_AudioDeviceName.Get(),-1,&rc,
						DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
			}
			if (pThis->IsItemVisible(ITEM_BITRATE)) {
				pThis->GetItemRect(ITEM_BITRATE,&rc);
				if (IsRectIntersect(&ps.rcPaint,&rc)) {
					if (pThis->m_fSignalLevel) {
						int SignalLevel=(int)(pThis->m_SignalLevel*100.0f);
						wsprintf(szText,TEXT("%d.%02d dB"),
										SignalLevel/100,abs(SignalLevel)%100);
					} else
						szText[0]='\0';
					if (pThis->m_fBitRate) {
						if (pThis->m_fSignalLevel)
							lstrcat(szText,TEXT(" / "));
						int BitRate=(int)(pThis->m_BitRate*100.0f);
						wsprintf(szText+lstrlen(szText),TEXT("%d.%02d Mbps"),
														BitRate/100,BitRate%100);
					}
					DrawText(ps.hdc,szText,-1,&rc,DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
				}
			}
			if (pThis->IsItemVisible(ITEM_ERROR)) {
				pThis->GetItemRect(ITEM_ERROR,&rc);
				if (IsRectIntersect(&ps.rcPaint,&rc)) {
					const CCoreEngine *pCoreEngine=GetAppClass().GetCoreEngine();
					int Length;

					Length=wsprintf(szText,TEXT("D %u / E %u"),
									pCoreEngine->GetContinuityErrorPacketCount(),
									pCoreEngine->GetErrorPacketCount());
					if (pCoreEngine->GetDescramble()
							&& pCoreEngine->GetCardReaderType()!=CCoreEngine::CARDREADER_NONE)
					wsprintf(szText+Length,TEXT(" / S %u"),pCoreEngine->GetScramblePacketCount());
					DrawText(ps.hdc,szText,-1,&rc,DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
				}
			}
			if (pThis->IsItemVisible(ITEM_RECORD)) {
				pThis->GetItemRect(ITEM_RECORD,&rc);
				if (IsRectIntersect(&ps.rcPaint,&rc)) {
					if (pThis->m_fRecording) {
						unsigned int RecordSec=pThis->m_RecordTime/1000;
						unsigned int Size=(unsigned int)(
							pThis->m_RecordWroteSize/(ULONGLONG)(1024*1024/100));
						unsigned int FreeSpace=(unsigned int)(
							pThis->m_DiskFreeSpace.QuadPart/(ULONGLONG)(1024*1024*1024/100));

						wsprintf(szText,
							TEXT("�� %d:%02d:%02d / %d.%02d MB / %d.%02d GB��"),
							RecordSec/(60*60),(RecordSec/60)%60,RecordSec%60,
							Size/100,Size%100,FreeSpace/100,FreeSpace%100);
					} else
						lstrcpy(szText,TEXT("�� <�^�悵�Ă��܂���>"));
					DrawText(ps.hdc,szText,-1,&rc,DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
				}
			}
			SetBkMode(ps.hdc,OldBkMode);
			SetTextColor(ps.hdc,crOldTextColor);
			SelectObject(ps.hdc,hfontOld);
			EndPaint(hwnd,&ps);
		}
		return 0;

	case WM_CTLCOLORSTATIC:
		{
			CInformationPanel *pThis=GetThis(hwnd);
			HDC hdc=reinterpret_cast<HDC>(wParam);

			SetTextColor(hdc,pThis->m_crProgramInfoTextColor);
			SetBkColor(hdc,pThis->m_crProgramInfoBackColor);
			return reinterpret_cast<LRESULT>(pThis->m_hbrProgramInfoBack);
		}

	case WM_DRAWITEM:
		{
			CInformationPanel *pThis=GetThis(hwnd);
			LPDRAWITEMSTRUCT pdis=reinterpret_cast<LPDRAWITEMSTRUCT>(lParam);
			HBRUSH hbrOld,hbr;
			HPEN hpen,hpenOld;
			POINT Points[3];

			hpen=CreatePen(PS_SOLID,1,pThis->m_crTextColor);
			hbrOld=SelectBrush(pdis->hDC,pThis->m_hbrBack);
			hpenOld=SelectPen(pdis->hDC,hpen);
			Rectangle(pdis->hDC,pdis->rcItem.left,pdis->rcItem.top,
								pdis->rcItem.right,pdis->rcItem.bottom);
			if (pdis->CtlID==IDC_PROGRAMINFOPREV) {
				Points[0].x=pdis->rcItem.right-5;
				Points[0].y=pdis->rcItem.top+3;
				Points[1].x=Points[0].x;
				Points[1].y=pdis->rcItem.bottom-4;
				Points[2].x=pdis->rcItem.left+4;
				Points[2].y=pdis->rcItem.top+(pdis->rcItem.bottom-pdis->rcItem.top)/2;
			} else {
				Points[0].x=pdis->rcItem.left+4;
				Points[0].y=pdis->rcItem.top+3;
				Points[1].x=Points[0].x;
				Points[1].y=pdis->rcItem.bottom-4;
				Points[2].x=pdis->rcItem.right-5;
				Points[2].y=pdis->rcItem.top+(pdis->rcItem.bottom-pdis->rcItem.top)/2;
			}
			if ((pdis->itemState&ODS_DISABLED)!=0) {
				hbr=CreateSolidBrush(MixColor(pThis->m_crBackColor,pThis->m_crTextColor));
			} else {
				hbr=CreateSolidBrush(pThis->m_crTextColor);
			}
			SelectBrush(pdis->hDC,hbr);
			SelectObject(pdis->hDC,GetStockObject(NULL_PEN));
			Polygon(pdis->hDC,Points,3);
			SelectPen(pdis->hDC,hpenOld);
			SelectBrush(pdis->hDC,hbrOld);
			DeleteObject(hpen);
			DeleteObject(hbr);
		}
		return TRUE;

	case WM_RBUTTONDOWN:
		{
			CInformationPanel *pThis=GetThis(hwnd);
			HMENU hmenu;
			POINT pt;

			hmenu=::LoadMenu(m_hinst,MAKEINTRESOURCE(IDM_INFORMATIONPANEL));
			for (int i=0;i<NUM_ITEMS;i++)
				CheckMenuItem(hmenu,CM_INFORMATIONPANEL_ITEM_FIRST+i,
							  MF_BYCOMMAND | (pThis->IsItemVisible(i)?MFS_CHECKED:MFS_UNCHECKED));
			pt.x=GET_X_LPARAM(lParam);
			pt.y=GET_Y_LPARAM(lParam);
			::ClientToScreen(hwnd,&pt);
			::TrackPopupMenu(::GetSubMenu(hmenu,0),TPM_RIGHTBUTTON,pt.x,pt.y,0,hwnd,NULL);
			::DestroyMenu(hmenu);
		}
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case CM_INFORMATIONPANEL_ITEM_VIDEO:
		case CM_INFORMATIONPANEL_ITEM_VIDEODECODER:
		case CM_INFORMATIONPANEL_ITEM_VIDEORENDERER:
		case CM_INFORMATIONPANEL_ITEM_AUDIODEVICE:
		case CM_INFORMATIONPANEL_ITEM_SIGNALLEVEL:
		case CM_INFORMATIONPANEL_ITEM_ERROR:
		case CM_INFORMATIONPANEL_ITEM_RECORD:
		case CM_INFORMATIONPANEL_ITEM_PROGRAMINFO:
			{
				CInformationPanel *pThis=GetThis(hwnd);
				int Item=LOWORD(wParam)-CM_INFORMATIONPANEL_ITEM_FIRST;

				pThis->SetItemVisible(Item,!pThis->IsItemVisible(Item));
			}
			return 0;

		case IDC_PROGRAMINFOPREV:
		case IDC_PROGRAMINFONEXT:
			{
				CInformationPanel *pThis=GetThis(hwnd);
				bool fNext=LOWORD(wParam)==IDC_PROGRAMINFONEXT;

				if (fNext!=pThis->m_fNextProgramInfo) {
					pThis->m_fNextProgramInfo=fNext;
					pThis->m_ProgramInfo.Clear();
					EnableWindow(pThis->m_hwndProgramInfoPrev,fNext);
					EnableWindow(pThis->m_hwndProgramInfoNext,!fNext);
					if (pThis->m_pEventHandler!=NULL)
						pThis->m_pEventHandler->OnProgramInfoUpdate(fNext);
					if (pThis->m_ProgramInfo.IsEmpty())
						SetWindowText(pThis->m_hwndProgramInfo,TEXT(""));
				}
			}
			return 0;
		}
		return 0;

	case WM_SETCURSOR:
		{
			CInformationPanel *pThis=GetThis(hwnd);

			if ((HWND)wParam==pThis->m_hwndProgramInfoPrev
					|| (HWND)wParam==pThis->m_hwndProgramInfoNext) {
				::SetCursor(::LoadCursor(NULL,IDC_HAND));
				return TRUE;
			}
		}
		break;

	case WM_DESTROY:
		{
			CInformationPanel *pThis=GetThis(hwnd);

			SubclassWindow(pThis->m_hwndProgramInfo,pThis->m_pOldProgramInfoProc);
			DeleteObject(pThis->m_hbrBack);
			DeleteObject(pThis->m_hbrProgramInfoBack);
			pThis->m_hwndProgramInfo=NULL;
			pThis->m_hwndProgramInfoPrev=NULL;
			pThis->m_hwndProgramInfoNext=NULL;
			pThis->OnDestroy();
		}
		return 0;
	}
	return DefWindowProc(hwnd,uMsg,wParam,lParam);
}


LRESULT CALLBACK CInformationPanel::ProgramInfoHookProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CInformationPanel *pThis=static_cast<CInformationPanel*>(::GetProp(hwnd,APP_NAME TEXT("This")));

	if (pThis==NULL)
		return ::DefWindowProc(hwnd,uMsg,wParam,lParam);

	switch (uMsg) {
	case WM_RBUTTONDOWN:
		if (!pThis->m_ProgramInfo.IsEmpty()) {
			HMENU hmenu=::CreatePopupMenu();
			POINT pt;
			int Command;

			::AppendMenu(hmenu,MFT_STRING | MFS_ENABLED,1,TEXT("�R�s�[(&C)"));
			::AppendMenu(hmenu,MFT_STRING | MFS_ENABLED,2,TEXT("���ׂđI��(&A)"));

			int URLCount=0;
			LPCWSTR p=pThis->m_ProgramInfo.Get();

			while (*p!='\0') {
				if ((*p=='h' && ::StrCmpNW(p,L"http://",7)==0)
						|| (*p==L'��' && ::StrCmpNW(p,L"���������F�^�^",7)==0)) {
					WCHAR szURL[256],szText[256];
					int i=0;

					if (*p=='h') {
						while (*p>0x20 && *p<=0x7F && i<lengthof(szURL)-1)
							szURL[i++]=*p++;
					} else {
						while (*p>0xFF00 && *p<=0xFF5E && i<lengthof(szURL)-1) {
							/*
							i+=::LCMapString(LOCALE_USER_DEFAULT,LCMAP_HALFWIDTH,
											 p,1,&szURL[i],lengthof(szURL)-1-i);
							*/
							szURL[i++]=*p-0xFEE0;
							p++;
						}
					}
					szURL[i]='\0';
					if (URLCount==0)
						::AppendMenu(hmenu,MFT_SEPARATOR,0,NULL);
					CopyToMenuText(szURL,szText,lengthof(szText));
					if (URLCount>0) {
						int j;
						for (j=0;j<URLCount;j++) {
							::GetMenuString(hmenu,3+j,szURL,lengthof(szURL),MF_BYCOMMAND);
							if (::lstrcmp(szURL,szText)==0)
								break;
						}
						if (j<URLCount)
							continue;
					}
					::AppendMenu(hmenu,MFT_STRING | MFS_ENABLED,3+URLCount,szText);
					URLCount++;
				} else {
					p++;
				}
			}
			::GetCursorPos(&pt);
			Command=::TrackPopupMenu(hmenu,TPM_RETURNCMD,pt.x,pt.y,0,hwnd,NULL);
			if (Command==1) {
				DWORD Start,End;

				::SendMessage(hwnd,WM_SETREDRAW,FALSE,0);
				::SendMessage(hwnd,EM_GETSEL,(WPARAM)&Start,(LPARAM)&End);
				if (Start==End)
					::SendMessage(hwnd,EM_SETSEL,0,-1);
				::SendMessage(hwnd,WM_COPY,0,0);
				if (Start==End)
					::SendMessage(hwnd,EM_SETSEL,Start,End);
				::SendMessage(hwnd,WM_SETREDRAW,TRUE,0);
			} else if (Command==2) {
				::SendMessage(hwnd,EM_SETSEL,0,-1);
			} else if (Command>=3) {
				TCHAR szText[256],szURL[256];
				int j;

				::GetMenuString(hmenu,Command,szText,lengthof(szText),MF_BYCOMMAND);
				j=0;
				for (int i=0;szText[i]!='\0';i++) {
					if (szText[i]=='&')
						i++;
					szURL[j++]=szText[i];
				}
				szURL[j]='\0';
#ifdef _DEBUG
				if (::MessageBox(NULL,szURL,TEXT("Detected URL"),MB_OKCANCEL)==IDOK)
#endif
				::ShellExecute(NULL,TEXT("open"),szURL,NULL,NULL,SW_SHOWNORMAL);
			}
			::DestroyMenu(hmenu);
		}
		return 0;

	case WM_RBUTTONUP:
		return 0;

	case WM_NCDESTROY:
		::RemoveProp(hwnd,APP_NAME TEXT("This"));
		break;
	}
	return CallWindowProc(pThis->m_pOldProgramInfoProc,hwnd,uMsg,wParam,lParam);
}


void CInformationPanel::GetItemRect(int Item,RECT *pRect) const
{
	GetClientRect(pRect);
	pRect->top=0;
	for (int i=0;i<Item;i++) {
		if ((m_ItemVisibility&ITEM_FLAG(i))!=0)
			pRect->top+=m_FontHeight+m_LineMargin;
	}
	if ((m_ItemVisibility&ITEM_FLAG(Item))==0) {
		pRect->bottom=pRect->top;
	} else {
		if (Item==ITEM_PROGRAMINFO)
			pRect->bottom-=PROGRAMINFO_BUTTON_SIZE;
		if (Item!=ITEM_PROGRAMINFO || pRect->top>=pRect->bottom)
			pRect->bottom=pRect->top+m_FontHeight;
	}
}


void CInformationPanel::UpdateItem(int Item)
{
	if (m_hwnd!=NULL && IsItemVisible(Item)) {
		RECT rc;

		GetItemRect(Item,&rc);
		::InvalidateRect(m_hwnd,&rc,TRUE);
	}
}


bool CInformationPanel::SetItemVisible(int Item,bool fVisible)
{
	if (Item<0 || Item>=NUM_ITEMS)
		return false;
	if (IsItemVisible(Item)!=fVisible) {
		m_ItemVisibility^=ITEM_FLAG(Item);
		if (m_hwnd!=NULL) {
			if (IsItemVisible(ITEM_PROGRAMINFO)) {
				RECT rc;
				::GetClientRect(m_hwnd,&rc);
				::SendMessage(m_hwnd,WM_SIZE,0,MAKELONG(rc.right,rc.bottom));
			}
			if (Item==ITEM_PROGRAMINFO) {
				const int Show=fVisible?SW_SHOW:SW_HIDE;
				::ShowWindow(m_hwndProgramInfo,Show);
				::ShowWindow(m_hwndProgramInfoPrev,Show);
				::ShowWindow(m_hwndProgramInfoNext,Show);
			} else {
				::InvalidateRect(m_hwnd,NULL,TRUE);
			}
		}
	}
	return true;
}


bool CInformationPanel::IsItemVisible(int Item) const
{
	if (Item<0 || Item>=NUM_ITEMS)
		return false;
	return (m_ItemVisibility&ITEM_FLAG(Item))!=0;
}


void CInformationPanel::CalcFontHeight()
{
	HDC hdc;
	HFONT hfontOld;
	TEXTMETRIC tm;

	hdc=::GetDC(m_hwnd);
	hfontOld=static_cast<HFONT>(::SelectObject(hdc,m_hFont));
	::GetTextMetrics(hdc,&tm);
	// tmInternalLeading�𑫂��ƃ��C���I�Ŗ��ɍs�Ԃ���
	//m_FontHeight=tm.tmHeight+tm.tmInternalLeading;
	m_FontHeight=tm.tmHeight;
	::SelectObject(hdc,hfontOld);
	::ReleaseDC(m_hwnd,hdc);
}


bool CInformationPanel::Load(LPCTSTR pszFileName)
{
	CSettings Settings;

	if (Settings.Open(pszFileName,TEXT("InformationPanel"),CSettings::OPEN_READ)) {
		for (int i=0;i<lengthof(m_pszItemNameList);i++) {
			bool f;

			if (Settings.Read(m_pszItemNameList[i],&f))
				SetItemVisible(i,f);
		}
	}
	return true;
}


bool CInformationPanel::Save(LPCTSTR pszFileName) const
{
	CSettings Settings;

	if (!Settings.Open(pszFileName,TEXT("InformationPanel"),CSettings::OPEN_WRITE))
		return false;
	for (int i=0;i<lengthof(m_pszItemNameList);i++)
		Settings.Write(m_pszItemNameList[i],IsItemVisible(i));
	return true;
}