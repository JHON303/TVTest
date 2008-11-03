#include "stdafx.h"
#include <streams.h>
#include <d3d9.h>
#include <vmr9.h>
#include "VideoRenderer.h"
#include "DirectShowUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif




CVideoRenderer::CVideoRenderer()
{
	m_pRenderer=NULL;
	m_pFilterGraph=NULL;
	m_hwndRender=NULL;
}


CVideoRenderer::~CVideoRenderer()
{
	TRACE(TEXT("CVideoRenderer::~CVideoRenderer()\n"));
}


bool CVideoRenderer::ShowProperty(HWND hwndOwner)
{
	if (m_pRenderer)
		return DirectShowUtil::ShowPropertyPage(m_pRenderer,hwndOwner);
	return false;
}




class CVideoRenderer_Default : public CVideoRenderer {
	IVideoWindow *m_pVideoWindow;
	IBasicVideo2 *m_pBasicVideo;
public:
	CVideoRenderer_Default();
	bool Initialize(IGraphBuilder *pFilterGraph,IPin *pInputPin,HWND hwndRender,HWND hwndMessageDrain);
	bool Finalize();
	bool SetVideoPosition(int SourceWidth,int SourceHeight,const RECT *pSourceRect,
								const RECT *pDestRect,const RECT *pWindowRect);
	bool GetDestPosition(RECT *pRect);
	bool GetCurrentImage(void **ppBuffer);
	bool ShowCursor(bool fShow);
	bool SetVisible(bool fVisible);
};


CVideoRenderer_Default::CVideoRenderer_Default()
{
	m_pVideoWindow=NULL;
	m_pBasicVideo=NULL;
}


bool CVideoRenderer_Default::Initialize(IGraphBuilder *pFilterGraph,IPin *pInputPin,HWND hwndRender,HWND hwndMessageDrain)
{
	HRESULT hr;
	RECT rc;

	hr=pFilterGraph->Render(pInputPin);
	if (FAILED(hr)) {
		SetError(TEXT("�f�������_�����\�z�ł��܂���B"));
		return false;
	}
	hr=pFilterGraph->QueryInterface(IID_IVideoWindow,reinterpret_cast<LPVOID *>(&m_pVideoWindow));
	if (FAILED(hr)) {
		SetError(TEXT("IVideoWindow���擾�ł��܂���B"));
		return false;
	}
	hr=pFilterGraph->QueryInterface(IID_IBasicVideo2,reinterpret_cast<LPVOID *>(&m_pBasicVideo));
	if (FAILED(hr)) {
		SAFE_RELEASE(m_pVideoWindow);
		SetError(TEXT("IBasicVideo2���擾�ł��܂���B"));
		return false;
	}
	m_pVideoWindow->put_Owner((OAHWND)hwndRender);
	m_pVideoWindow->put_MessageDrain((OAHWND)hwndMessageDrain);
	m_pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	m_pVideoWindow->put_BackgroundPalette(OATRUE);
	m_pVideoWindow->put_BorderColor(RGB(0,0,0));
	m_pVideoWindow->put_Caption(L"");
	::GetClientRect(hwndRender,&rc);
	m_pVideoWindow->SetWindowPosition(0,0,rc.right,rc.bottom);
	m_pVideoWindow->SetWindowForeground(OATRUE);
	m_pVideoWindow->put_Visible(OATRUE);
	m_pFilterGraph=pFilterGraph;
	return true;
}


bool CVideoRenderer_Default::Finalize()
{
	CHECK_RELEASE(m_pBasicVideo);
	if (m_pVideoWindow) {
		m_pVideoWindow->put_Visible(OAFALSE);
		m_pVideoWindow->put_Owner(NULL);
		CHECK_RELEASE(m_pVideoWindow);
	}
	return true;
}


bool CVideoRenderer_Default::SetVideoPosition(int SourceWidth,int SourceHeight,const RECT *pSourceRect,
								const RECT *pDestRect,const RECT *pWindowRect)
{
	HRESULT hr;

	if (m_pVideoWindow==NULL || m_pBasicVideo==NULL)
		return false;
	m_pBasicVideo->SetSourcePosition(pSourceRect->left,pSourceRect->top,
		pSourceRect->right-pSourceRect->left,pSourceRect->bottom-pSourceRect->top);
	m_pBasicVideo->SetDestinationPosition(pDestRect->left,pDestRect->top,
		pDestRect->right-pDestRect->left,pDestRect->bottom-pDestRect->top);
	m_pVideoWindow->SetWindowPosition(pWindowRect->left,pWindowRect->top,
		pWindowRect->right-pWindowRect->left,pWindowRect->bottom-pWindowRect->top);
	return true;
}


bool CVideoRenderer_Default::GetDestPosition(RECT *pRect)
{
	if (m_pBasicVideo==NULL
			|| FAILED(m_pBasicVideo->GetDestinationPosition(
					&pRect->left,&pRect->top,&pRect->right,&pRect->bottom)))
		return false;
	pRect->right+=pRect->left;
	pRect->bottom+=pRect->top;
	return true;
}


bool CVideoRenderer_Default::GetCurrentImage(void **ppBuffer)
{
	bool fOK=false;

	if (m_pBasicVideo!=NULL) {
		long Size;

		if (SUCCEEDED(m_pBasicVideo->GetCurrentImage(&Size,NULL)) && Size>0) {
			long *pDib;

			pDib=static_cast<long*>(::CoTaskMemAlloc(Size));
			if (pDib!=NULL) {
				if (SUCCEEDED(m_pBasicVideo->GetCurrentImage(&Size,pDib))) {
					*ppBuffer=pDib;
					fOK=true;
				} else {
					::CoTaskMemFree(pDib);
				}
			}
		}
	}
	return fOK;
}


bool CVideoRenderer_Default::ShowCursor(bool fShow)
{
	if (m_pVideoWindow) {
		if (SUCCEEDED(m_pVideoWindow->HideCursor(fShow?OAFALSE:OATRUE)))
			return true;
	}
	return false;
}


bool CVideoRenderer_Default::SetVisible(bool fVisible)
{
	if (m_pVideoWindow)
		return SUCCEEDED(m_pVideoWindow->put_Visible(fVisible));
	return false;
}




class CVideoRenderer_VMR7 : public CVideoRenderer {
public:
	CVideoRenderer_VMR7();
	~CVideoRenderer_VMR7();
	bool Initialize(IGraphBuilder *pFilterGraph,IPin *pInputPin,HWND hwndRender,HWND hwndMessageDrain);
	bool Finalize();
	bool SetVideoPosition(int SourceWidth,int SourceHeight,const RECT *pSourceRect,
								const RECT *pDestRect,const RECT *pWindowRect);
	bool GetDestPosition(RECT *pRect);
	bool GetCurrentImage(void **ppBuffer);
	bool RepaintVideo(HWND hwnd,HDC hdc);
	bool DisplayModeChanged();
	bool SetVisible(bool fVisible);
};


CVideoRenderer_VMR7::CVideoRenderer_VMR7()
{
}


CVideoRenderer_VMR7::~CVideoRenderer_VMR7()
{
	CHECK_RELEASE(m_pRenderer);
}


bool CVideoRenderer_VMR7::Initialize(IGraphBuilder *pFilterGraph,IPin *pInputPin,HWND hwndRender,HWND hwndMessageDrain)
{
	HRESULT hr;

	hr=::CoCreateInstance(CLSID_VideoMixingRenderer,NULL,CLSCTX_INPROC_SERVER,
						IID_IBaseFilter,reinterpret_cast<LPVOID*>(&m_pRenderer));
	if (FAILED(hr)) {
		SetError(TEXT("VMR�̃C���X�^���X���쐬�ł��܂���B"));
		return false;
	}
	hr=pFilterGraph->AddFilter(m_pRenderer,L"VMR7");
	if (FAILED(hr)) {
		CHECK_RELEASE(m_pRenderer);
		SetError(TEXT("VMR���t�B���^�O���t�ɒǉ��ł��܂���B"));
		return false;
	}

	IVMRFilterConfig *pFilterConfig;
	IVMRWindowlessControl *pWindowlessControl;
	RECT rc;

	m_pRenderer->QueryInterface(IID_IVMRFilterConfig,
									reinterpret_cast<LPVOID*>(&pFilterConfig));
	pFilterConfig->SetRenderingMode(VMRMode_Windowless);
	pFilterConfig->Release();
	m_pRenderer->QueryInterface(IID_IVMRWindowlessControl,
							reinterpret_cast<LPVOID*>(&pWindowlessControl));
	pWindowlessControl->SetVideoClippingWindow(hwndRender);
	pWindowlessControl->SetBorderColor(RGB(0,0,0));
	pWindowlessControl->SetAspectRatioMode(VMR_ARMODE_NONE);
	::GetClientRect(hwndRender,&rc);
	pWindowlessControl->SetVideoPosition(NULL,&rc);
	pWindowlessControl->Release();

	IFilterGraph2 *pFilterGraph2;

	hr=pFilterGraph->QueryInterface(IID_IFilterGraph2,
									reinterpret_cast<LPVOID*>(&pFilterGraph2));
	if (FAILED(hr)) {
		SetError(TEXT("IFilterGraph2���擾�ł��܂���B"));
		return false;
	}
	hr=pFilterGraph2->RenderEx(pInputPin,
								AM_RENDEREX_RENDERTOEXISTINGRENDERERS,NULL);
	pFilterGraph2->Release();
	if (FAILED(hr)) {
		SetError(TEXT("�f�������_�����\�z�ł��܂���B"));
		return false;
	}
	m_pFilterGraph=pFilterGraph;
	m_hwndRender=hwndRender;
	return true;
}


bool CVideoRenderer_VMR7::Finalize()
{
	//CHECK_RELEASE(m_pRenderer);
	return true;
}


bool CVideoRenderer_VMR7::SetVideoPosition(int SourceWidth,int SourceHeight,const RECT *pSourceRect,
								const RECT *pDestRect,const RECT *pWindowRect)
{
	IVMRWindowlessControl *pWindowlessControl;
	HRESULT hr;
	RECT rcSrc,rcDest;

	if (m_pRenderer==NULL)
		return false;
	hr=m_pRenderer->QueryInterface(IID_IVMRWindowlessControl,
							reinterpret_cast<LPVOID*>(&pWindowlessControl));
	if (FAILED(hr))
		return false;

	LONG Width,Height;

	if (SUCCEEDED(pWindowlessControl->GetNativeVideoSize(&Width,&Height,NULL,NULL))
			&& SourceWidth>0 && SourceHeight>0) {
		rcSrc.left=pSourceRect->left*Width/SourceWidth;
		rcSrc.top=pSourceRect->top*Height/SourceHeight;
		rcSrc.right=pSourceRect->right*Width/SourceWidth;
		rcSrc.bottom=pSourceRect->bottom*Height/SourceHeight;
	} else {
		rcSrc=*pSourceRect;
	}
	rcDest=*pDestRect;
	::OffsetRect(&rcDest,pWindowRect->left,pWindowRect->top);
	pWindowlessControl->SetVideoPosition(&rcSrc,&rcDest);
	pWindowlessControl->Release();
	::InvalidateRect(m_hwndRender,NULL,TRUE);
	return true;
}


bool CVideoRenderer_VMR7::GetDestPosition(RECT *pRect)
{
	bool fOK=false;

	if (m_pRenderer) {
		IVMRWindowlessControl *pWindowlessControl;

		if (SUCCEEDED(m_pRenderer->QueryInterface(IID_IVMRWindowlessControl,
							reinterpret_cast<LPVOID*>(&pWindowlessControl)))) {
			fOK=SUCCEEDED(pWindowlessControl->GetVideoPosition(NULL,pRect));
			pWindowlessControl->Release();
		}
	}
	return fOK;
}


bool CVideoRenderer_VMR7::GetCurrentImage(void **ppBuffer)
{
	bool fOK=false;

	if (m_pRenderer) {
		IVMRWindowlessControl *pWindowlessControl;

		if (SUCCEEDED(m_pRenderer->QueryInterface(IID_IVMRWindowlessControl,
							reinterpret_cast<LPVOID*>(&pWindowlessControl)))) {
			BYTE *pDib;

			if (SUCCEEDED(pWindowlessControl->GetCurrentImage(&pDib))) {
				*ppBuffer=pDib;
				fOK=true;
			}
			pWindowlessControl->Release();
		}
	}
	return fOK;
}


bool CVideoRenderer_VMR7::RepaintVideo(HWND hwnd,HDC hdc)
{
	bool fOK=false;

	if (m_pRenderer) {
		IVMRWindowlessControl *pWindowlessControl;

		if (SUCCEEDED(m_pRenderer->QueryInterface(IID_IVMRWindowlessControl,
							reinterpret_cast<LPVOID*>(&pWindowlessControl)))) {
			if (SUCCEEDED(pWindowlessControl->RepaintVideo(hwnd,hdc)))
				fOK=true;
			pWindowlessControl->Release();
		}
	}
	return fOK;
}


bool CVideoRenderer_VMR7::DisplayModeChanged()
{
	bool fOK=false;

	if (m_pRenderer) {
		IVMRWindowlessControl *pWindowlessControl;

		if (SUCCEEDED(m_pRenderer->QueryInterface(IID_IVMRWindowlessControl,
							reinterpret_cast<LPVOID*>(&pWindowlessControl)))) {
			if (SUCCEEDED(pWindowlessControl->DisplayModeChanged()))
				fOK=true;
			pWindowlessControl->Release();
		}
	}
	return fOK;
}


bool CVideoRenderer_VMR7::SetVisible(bool fVisible)
{
	// �E�B���h�E���ĕ`�悳����
	if (m_hwndRender)
		return ::InvalidateRect(m_hwndRender,NULL,TRUE)!=FALSE;
	return false;
}




class CVideoRenderer_VMR9 : public CVideoRenderer {
public:
	CVideoRenderer_VMR9();
	~CVideoRenderer_VMR9();
	bool Initialize(IGraphBuilder *pFilterGraph,IPin *pInputPin,HWND hwndRender,HWND hwndMessageDrain);
	bool Finalize();
	virtual bool SetVideoPosition(int SourceWidth,int SourceHeight,const RECT *pSourceRect,
								const RECT *pDestRect,const RECT *pWindowRect);
	bool GetDestPosition(RECT *pRect);
	bool GetCurrentImage(void **ppBuffer);
	bool RepaintVideo(HWND hwnd,HDC hdc);
	bool DisplayModeChanged();
	bool SetVisible(bool fVisible);
};


CVideoRenderer_VMR9::CVideoRenderer_VMR9()
{
}


CVideoRenderer_VMR9::~CVideoRenderer_VMR9()
{
	CHECK_RELEASE(m_pRenderer);
}


bool CVideoRenderer_VMR9::Initialize(IGraphBuilder *pFilterGraph,IPin *pInputPin,HWND hwndRender,HWND hwndMessageDrain)
{
	HRESULT hr;

	hr=::CoCreateInstance(CLSID_VideoMixingRenderer9,NULL,CLSCTX_INPROC_SERVER,
						IID_IBaseFilter,reinterpret_cast<LPVOID*>(&m_pRenderer));
	if (FAILED(hr)) {
		SetError(TEXT("VMR-9�̃C���X�^���X���쐬�ł��܂���B"));
		return false;
	}
	hr=pFilterGraph->AddFilter(m_pRenderer,L"VMR9");
	if (FAILED(hr)) {
		CHECK_RELEASE(m_pRenderer);
		SetError(TEXT("VMR-9���t�B���^�O���t�ɒǉ��ł��܂���B"));
		return false;
	}
	IVMRFilterConfig *pFilterConfig;
	IVMRWindowlessControl9 *pWindowlessControl;
	IVMRMixerControl9 *pMixerControl;
	RECT rc;

	m_pRenderer->QueryInterface(IID_IVMRFilterConfig9,
									reinterpret_cast<LPVOID*>(&pFilterConfig));
	pFilterConfig->SetRenderingMode(VMRMode_Windowless);
	pFilterConfig->Release();
	m_pRenderer->QueryInterface(IID_IVMRWindowlessControl9,
							reinterpret_cast<LPVOID*>(&pWindowlessControl));
	pWindowlessControl->SetVideoClippingWindow(hwndRender);
	pWindowlessControl->SetBorderColor(RGB(0,0,0));
	pWindowlessControl->SetAspectRatioMode(VMR9ARMode_None);
	::GetClientRect(hwndRender,&rc);
	pWindowlessControl->SetVideoPosition(NULL,&rc);
	pWindowlessControl->Release();
	if (SUCCEEDED(m_pRenderer->QueryInterface(IID_IVMRMixerControl9,
								reinterpret_cast<LPVOID*>(&pMixerControl)))) {
		DWORD MixingPref;

		pMixerControl->GetMixingPrefs(&MixingPref);
		MixingPref=(MixingPref&~MixerPref9_DecimateMask)|MixerPref9_NonSquareMixing;
		pMixerControl->SetMixingPrefs(MixingPref);
		pMixerControl->Release();
	}

	IFilterGraph2 *pFilterGraph2;

	hr=pFilterGraph->QueryInterface(IID_IFilterGraph2,
									reinterpret_cast<LPVOID*>(&pFilterGraph2));
	if (FAILED(hr)) {
		SetError(TEXT("IFilterGraph2���擾�ł��܂���B"));
		return false;
	}
	hr=pFilterGraph2->RenderEx(pInputPin,
								AM_RENDEREX_RENDERTOEXISTINGRENDERERS,NULL);
	pFilterGraph2->Release();
	if (FAILED(hr)) {
		SetError(TEXT("�f�������_�����\�z�ł��܂���B"));
		return false;
	}
	m_pFilterGraph=pFilterGraph;
	m_hwndRender=hwndRender;
	return true;
}


bool CVideoRenderer_VMR9::Finalize()
{
	//CHECK_RELEASE(m_pRenderer);
	return true;
}


bool CVideoRenderer_VMR9::SetVideoPosition(int SourceWidth,int SourceHeight,const RECT *pSourceRect,
								const RECT *pDestRect,const RECT *pWindowRect)
{
	IVMRWindowlessControl9 *pWindowlessControl;
	HRESULT hr;
	RECT rcSrc,rcDest;

	if (m_pRenderer==NULL)
		return false;
	hr=m_pRenderer->QueryInterface(IID_IVMRWindowlessControl9,
							reinterpret_cast<LPVOID*>(&pWindowlessControl));
	if (FAILED(hr))
		return false;

	LONG Width,Height;

	if (SUCCEEDED(pWindowlessControl->GetNativeVideoSize(&Width,&Height,NULL,NULL))
			&& SourceWidth>0 && SourceHeight>0) {
		rcSrc.left=pSourceRect->left*Width/SourceWidth;
		rcSrc.top=pSourceRect->top*Height/SourceHeight;
		rcSrc.right=pSourceRect->right*Width/SourceWidth;
		rcSrc.bottom=pSourceRect->bottom*Height/SourceHeight;
	} else {
		rcSrc=*pSourceRect;
	}
	rcDest=*pDestRect;
	::OffsetRect(&rcDest,pWindowRect->left,pWindowRect->top);
	pWindowlessControl->SetVideoPosition(&rcSrc,&rcDest);
	pWindowlessControl->Release();
	::InvalidateRect(m_hwndRender,NULL,TRUE);
	return true;
}


bool CVideoRenderer_VMR9::GetDestPosition(RECT *pRect)
{
	bool fOK=false;

	if (m_pRenderer) {
		IVMRWindowlessControl9 *pWindowlessControl;

		if (SUCCEEDED(m_pRenderer->QueryInterface(IID_IVMRWindowlessControl9,
							reinterpret_cast<LPVOID*>(&pWindowlessControl)))) {
			fOK=SUCCEEDED(pWindowlessControl->GetVideoPosition(NULL,pRect));
			pWindowlessControl->Release();
		}
	}
	return fOK;
}


bool CVideoRenderer_VMR9::GetCurrentImage(void **ppBuffer)
{
	bool fOK=false;

	if (m_pRenderer) {
		IVMRWindowlessControl9 *pWindowlessControl;

		if (SUCCEEDED(m_pRenderer->QueryInterface(IID_IVMRWindowlessControl9,
							reinterpret_cast<LPVOID*>(&pWindowlessControl)))) {
			BYTE *pDib;

			if (SUCCEEDED(pWindowlessControl->GetCurrentImage(&pDib))) {
				*ppBuffer=pDib;
				fOK=true;
			}
			pWindowlessControl->Release();
		}
	}
	return fOK;
}


bool CVideoRenderer_VMR9::RepaintVideo(HWND hwnd,HDC hdc)
{
	bool fOK=false;

	if (m_pRenderer!=NULL) {
		IVMRWindowlessControl9 *pWindowlessControl;

		if (SUCCEEDED(m_pRenderer->QueryInterface(IID_IVMRWindowlessControl9,
							reinterpret_cast<LPVOID*>(&pWindowlessControl)))) {
			if (SUCCEEDED(pWindowlessControl->RepaintVideo(hwnd,hdc)))
				fOK=true;
			pWindowlessControl->Release();
		}
	}
	return fOK;
}


bool CVideoRenderer_VMR9::DisplayModeChanged()
{
	bool fOK=false;

	if (m_pRenderer) {
		IVMRWindowlessControl9 *pWindowlessControl;

		if (SUCCEEDED(m_pRenderer->QueryInterface(IID_IVMRWindowlessControl9,
							reinterpret_cast<LPVOID*>(&pWindowlessControl)))) {
			if (SUCCEEDED(pWindowlessControl->DisplayModeChanged()))
				fOK=true;
			pWindowlessControl->Release();
		}
	}
	return fOK;
}


bool CVideoRenderer_VMR9::SetVisible(bool fVisible)
{
	// �E�B���h�E���ĕ`�悳����
	if (m_hwndRender)
		return ::InvalidateRect(m_hwndRender,NULL,TRUE)!=FALSE;
	return false;
}




#if 1

#include <ddraw.h>
#define D3D_OVERLOADS
#include <d3d.h>


class CVideoRenderer_VMR7Renderless :
	public CUnknown,
	public IVMRSurfaceAllocator,
	public IVMRImagePresenter,
	public IVMRSurfaceAllocatorNotify,
	public CVideoRenderer
{
	LPDIRECTDRAWSURFACE7 m_pddsPrimary;
	LPDIRECTDRAWSURFACE7 m_pddsPriText;
	LPDIRECTDRAWSURFACE7 m_pddsRenderT;

	IVMRSurfaceAllocator       *m_pSurfaceAllocator;
	IVMRImagePresenter         *m_pImagePresenter;
	IVMRSurfaceAllocatorNotify *m_pSurfaceAllocatorNotify;

	int m_Duration;

	HRESULT CreateDefaultAllocatorPresenter(HWND hwndRender);
	HRESULT AddVideoMixingRendererToFG();
	HRESULT OnSetDDrawDevice(LPDIRECTDRAW7 pDD,HMONITOR hMon);

public:
	CVideoRenderer_VMR7Renderless();
	~CVideoRenderer_VMR7Renderless();

	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);

	// IVMRSurfaceAllocator
	STDMETHODIMP AllocateSurface(DWORD_PTR dwUserID,
								 VMRALLOCATIONINFO* lpAllocInfo,
								 DWORD* lpdwActualBackBuffers,
								 LPDIRECTDRAWSURFACE7* lplpSurface);
	STDMETHODIMP FreeSurface(DWORD_PTR dwUserID);
	STDMETHODIMP PrepareSurface(DWORD_PTR dwUserID,
						LPDIRECTDRAWSURFACE7 lpSurface,DWORD dwSurfaceFlags);
	STDMETHODIMP AdviseNotify(IVMRSurfaceAllocatorNotify* lpIVMRSurfAllocNotify);

	// IVMRSurfaceAllocatorNotify
	STDMETHODIMP AdviseSurfaceAllocator(DWORD_PTR dwUserID,
								IVMRSurfaceAllocator* lpIVRMSurfaceAllocator);
	STDMETHODIMP SetDDrawDevice(LPDIRECTDRAW7 lpDDrawDevice,HMONITOR hMonitor);
	STDMETHODIMP ChangeDDrawDevice(LPDIRECTDRAW7 lpDDrawDevice,HMONITOR hMonitor);
	STDMETHODIMP RestoreDDrawSurfaces();
	STDMETHODIMP NotifyEvent(LONG EventCode,LONG_PTR lp1,LONG_PTR lp2);
	STDMETHODIMP SetBorderColor(COLORREF clr);

	// IVMRImagePresenter
	STDMETHODIMP StartPresenting(DWORD_PTR dwUserID);
	STDMETHODIMP StopPresenting(DWORD_PTR dwUserID);
	STDMETHODIMP PresentImage(DWORD_PTR dwUserID,VMRPRESENTATIONINFO* lpPresInfo);

	// CVideoRenderer
	bool Initialize(IGraphBuilder *pFilterGraph,IPin *pInputPin,HWND hwndRender,HWND hwndMessageDrain);
	bool Finalize();
	bool SetVideoPosition(int SourceWidth,int SourceHeight,const RECT *pSourceRect,
							const RECT *pDestRect,const RECT *pWindowRect);
	bool GetDestPosition(RECT *pRect);
	bool GetCurrentImage(void **ppBuffer);
	bool RepaintVideo(HWND hwnd,HDC hdc);
	bool DisplayModeChanged();
	bool SetVisible(bool fVisible);
};


CVideoRenderer_VMR7Renderless::CVideoRenderer_VMR7Renderless()
	: CUnknown(TEXT("VMR7 Renderless"),NULL)
	, m_pddsPrimary(NULL)
	, m_pddsPriText(NULL)
	, m_pddsRenderT(NULL)
	, m_pSurfaceAllocator(NULL)
	, m_pImagePresenter(NULL)
	, m_pSurfaceAllocatorNotify(NULL)
	, m_Duration(-1)
{
	AddRef();
}


CVideoRenderer_VMR7Renderless::~CVideoRenderer_VMR7Renderless()
{
	TRACE(TEXT("CVideoRenderer_VMR7Renderless::~CVideoRenderer_VMR7Renderless()\n"));
	CHECK_RELEASE(m_pImagePresenter);
	CHECK_RELEASE(m_pSurfaceAllocator);
	CHECK_RELEASE(m_pSurfaceAllocatorNotify);
	CHECK_RELEASE(m_pRenderer);
}


HRESULT CVideoRenderer_VMR7Renderless::CreateDefaultAllocatorPresenter(HWND hwndRender)
{
	HRESULT hr;

	hr=::CoCreateInstance(CLSID_AllocPresenter,NULL,CLSCTX_INPROC_SERVER,
		IID_IVMRSurfaceAllocator,reinterpret_cast<LPVOID*>(&m_pSurfaceAllocator));
	if (SUCCEEDED(hr)) {
		m_pSurfaceAllocator->AdviseNotify(this);
		hr=m_pSurfaceAllocator->QueryInterface(IID_IVMRImagePresenter,
								reinterpret_cast<LPVOID*>(&m_pImagePresenter));
		if (SUCCEEDED(hr)) {
			IVMRWindowlessControl *pWindowlessControl;

			hr=m_pSurfaceAllocator->QueryInterface(IID_IVMRWindowlessControl,
							reinterpret_cast<LPVOID*>(&pWindowlessControl));
			if (SUCCEEDED(hr)) {
				RECT rc;

				pWindowlessControl->SetVideoClippingWindow(hwndRender);
				pWindowlessControl->SetBorderColor(RGB(0,0,0));
				pWindowlessControl->SetAspectRatioMode(VMR_ARMODE_NONE);
				::GetClientRect(hwndRender,&rc);
				pWindowlessControl->SetVideoPosition(NULL,&rc);
				pWindowlessControl->Release();
			}
		}
	}
	if (FAILED(hr)) {
		CHECK_RELEASE(m_pImagePresenter);
		CHECK_RELEASE(m_pSurfaceAllocator);
	}
	return hr;
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::NonDelegatingQueryInterface(
	REFIID riid,void **ppv)
{
	if (riid==IID_IVMRSurfaceAllocator) {
		return GetInterface(static_cast<IVMRSurfaceAllocator*>(this),ppv);
	} else if (riid==IID_IVMRImagePresenter) {
		return GetInterface(static_cast<IVMRImagePresenter*>(this),ppv);
	}
    return CUnknown::NonDelegatingQueryInterface(riid,ppv);
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::AllocateSurface(
	DWORD_PTR dwUserID,VMRALLOCATIONINFO *lpAllocInfo,DWORD* lpdwBuffer,LPDIRECTDRAWSURFACE7* lplpSurface)
{
	return m_pSurfaceAllocator->AllocateSurface(dwUserID,lpAllocInfo,lpdwBuffer,lplpSurface);
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::FreeSurface(DWORD_PTR dwUserID)
{
	return m_pSurfaceAllocator->FreeSurface(dwUserID);
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::PrepareSurface(DWORD_PTR dwUserID,
						LPDIRECTDRAWSURFACE7 lplpSurface,DWORD dwSurfaceFlags)
{
	return m_pSurfaceAllocator->PrepareSurface(dwUserID,lplpSurface,dwSurfaceFlags);
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::AdviseNotify(
							IVMRSurfaceAllocatorNotify* lpIVMRSurfAllocNotify)
{
	return m_pSurfaceAllocator->AdviseNotify(lpIVMRSurfAllocNotify);
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::AdviseSurfaceAllocator(
			DWORD_PTR dwUserID,IVMRSurfaceAllocator* lpIVRMSurfaceAllocator)
{
	return m_pSurfaceAllocatorNotify->AdviseSurfaceAllocator(dwUserID,lpIVRMSurfaceAllocator);
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::SetDDrawDevice(LPDIRECTDRAW7 lpDDrawDevice,HMONITOR hMonitor)
{
    return m_pSurfaceAllocatorNotify->SetDDrawDevice(lpDDrawDevice,hMonitor);
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::ChangeDDrawDevice(LPDIRECTDRAW7 lpDDrawDevice,HMONITOR hMonitor)
{
	return m_pSurfaceAllocatorNotify->ChangeDDrawDevice(lpDDrawDevice,hMonitor);
}


HRESULT WINAPI DDSurfEnumFunc(LPDIRECTDRAWSURFACE7 pdds,DDSURFACEDESC2 *pddsd,
															void *lpContext)
{
	LPDIRECTDRAWSURFACE7 *ppdds=static_cast<LPDIRECTDRAWSURFACE7*>(lpContext);
	DDSURFACEDESC2 ddsd;

	::ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	HRESULT hr=pdds->GetSurfaceDesc(&ddsd);
	if (SUCCEEDED(hr)) {
		if (ddsd.ddsCaps.dwCaps&DDSCAPS_PRIMARYSURFACE) {
			*ppdds=pdds;
			return DDENUMRET_CANCEL;
		}
	}
	pdds->Release();
	return DDENUMRET_OK;
}


HRESULT CVideoRenderer_VMR7Renderless::OnSetDDrawDevice(LPDIRECTDRAW7 pDD,
														HMONITOR hMon)
{
	HRESULT hr;

	CHECK_RELEASE(m_pddsRenderT);
	CHECK_RELEASE(m_pddsPriText);
	CHECK_RELEASE(m_pddsPrimary);

	DDSURFACEDESC2 ddsd;

	::ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS;
	ddsd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE;
	hr=pDD->EnumSurfaces(DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_ALL,
						 &ddsd,&m_pddsPrimary,DDSurfEnumFunc);
	if (FAILED(hr))
		return hr;
	if (m_pddsPrimary==NULL)
		return E_FAIL;

	MONITORINFOEX mix;
	mix.cbSize=sizeof(mix);
	::GetMonitorInfo(hMon,&mix);

	::ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth =mix.rcMonitor.right-mix.rcMonitor.left;
	ddsd.dwHeight=mix.rcMonitor.bottom-mix.rcMonitor.top;

	hr=pDD->CreateSurface(&ddsd,&m_pddsPriText,NULL);
	if (SUCCEEDED(hr))
		hr=pDD->CreateSurface(&ddsd,&m_pddsRenderT,NULL);
	if (FAILED(hr)) {
		CHECK_RELEASE(m_pddsRenderT);
		CHECK_RELEASE(m_pddsPriText);
		CHECK_RELEASE(m_pddsPrimary);
	}
	return hr;
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::RestoreDDrawSurfaces()
{
    return m_pSurfaceAllocatorNotify->RestoreDDrawSurfaces();
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::NotifyEvent(LONG EventCode,LONG_PTR lp1,LONG_PTR lp2)
{
	return m_pSurfaceAllocatorNotify->NotifyEvent(EventCode,lp1,lp2);
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::SetBorderColor(COLORREF clr)
{
	return m_pSurfaceAllocatorNotify->SetBorderColor(clr);
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::StartPresenting(DWORD_PTR dwUserID)
{
	return m_pImagePresenter->StartPresenting(dwUserID);
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::StopPresenting(DWORD_PTR dwUserID)
{
	return m_pImagePresenter->StopPresenting(dwUserID);
}


STDMETHODIMP CVideoRenderer_VMR7Renderless::PresentImage(DWORD_PTR dwUserID,
											VMRPRESENTATIONINFO* lpPresInfo)
{
#if 0
	LPDIRECTDRAWSURFACE7 lpSurface=lpPresInfo->lpSurf;
	const REFERENCE_TIME rtNow=lpPresInfo->rtStart;
	const DWORD dwSurfaceFlags=lpPresInfo->dwFlags;

	if (m_iDuration>0) {
		HRESULT hr;
		RECT rs,rd;
		DDSURFACEDESC2 ddsdV;

		::ZeroMemory(&ddsdV,sizeof(ddsdV));
		ddsdV.dwSize=sizeof(ddsd);
		hr=lpSurface->GetSurfaceDesc(&ddsdV);

		DDSURFACEDESC2 ddsdP;
		::ZeroMemory(&ddsdP,sizeof(ddsdP));
		ddsdP.dwSize=sizeof(ddsd);
		hr=m_pddsPriText->GetSurfaceDesc(&ddsdP);

		FLOAT fPos = (FLOAT)m_Duration / 30.0F;
		FLOAT fPosInv = 1.0F - fPos;

		::SetRect(&rs, 0, 0,
			::MulDiv((int)ddsdV.dwWidth,30-m_Duration,30),
			ddsdV.dwHeight);
		::SetRect(&rd, 0, 0,
			::MulDiv((int)ddsdP.dwWidth,30-m_Duration,30),
			ddsdP.dwHeight);
		hr=m_pddsRenderT->Blt(&rd,lpSurface,&rs,DDBLT_WAIT,NULL);

		::SetRect(&rs, 0, 0,
			::MulDiv((int)ddsdP.dwWidth,m_Duration,30),
			ddsdP.dwHeight);
		::SetRect(&rd,
			(int)ddsdP.dwWidth-::MulDiv((int)ddsdP.dwWidth,m_Duration,30),
			0,
			ddsdP.dwWidth,
			ddsdP.dwHeight);
		hr=m_pddsRenderT->Blt(&rd,m_pddsPriText,&rs,DDBLT_WAIT,NULL);

		LPDIRECTDRAW lpdd;
		hr = m_pddsPrimary->GetDDInterface((LPVOID*)&lpdd);
		if (SUCCEEDED(hr)) {
			DWORD dwScanLine;
			for (;;) {
				hr=lpdd->GetScanLine(&dwScanLine);
				if (hr==DDERR_VERTICALBLANKINPROGRESS)
					break;
				if (FAILED(hr))
					break;
				if ((LONG)dwScanLine>=rd.top) {
					if ((LONG)dwScanLine<=rd.bottom)
						continue;
				}
				break;
			}
			lpdd->Release();
		}
		hr=m_pddsPrimary->Blt(NULL,m_pddsRenderT,NULL,DDBLT_WAIT,NULL);
		m_Duration--;
		if (m_Duration==0 && (ddsdV.ddsCaps.dwCaps&DDSCAPS_OVERLAY)!=0) {
			::InvalidateRect(m_hwndRender,NULL,FALSE);
		}
		return hr;
	}
#endif
	return m_pImagePresenter->PresentImage(dwUserID,lpPresInfo);
}


bool CVideoRenderer_VMR7Renderless::Initialize(IGraphBuilder *pFilterGraph,
						IPin *pInputPin,HWND hwndRender,HWND hwndMessageDrain)
{
	HRESULT hr;

	hr=::CoCreateInstance(CLSID_VideoMixingRenderer,NULL,CLSCTX_INPROC,
					IID_IBaseFilter,reinterpret_cast<LPVOID*>(&m_pRenderer));
	if (FAILED(hr)) {
		SetError(TEXT("VMR�̃C���X�^���X���쐬�ł��܂���B"));
		return false;
	}
	hr=pFilterGraph->AddFilter(m_pRenderer, L"VMR");
	if (FAILED(hr)) {
		SetError(TEXT("VMR���t�B���^�O���t�ɒǉ��ł��܂���B"));
		return false;
	}
	IVMRFilterConfig *pFilterConfig;
	hr=m_pRenderer->QueryInterface(IID_IVMRFilterConfig,reinterpret_cast<LPVOID*>(&pFilterConfig));
	pFilterConfig->SetRenderingMode(VMRMode_Renderless);
	pFilterConfig->Release();
	m_pRenderer->QueryInterface(IID_IVMRSurfaceAllocatorNotify,reinterpret_cast<LPVOID*>(&m_pSurfaceAllocatorNotify));
	CreateDefaultAllocatorPresenter(hwndRender);
	m_pSurfaceAllocatorNotify->AdviseSurfaceAllocator(1234,this);

	IFilterGraph2 *pFilterGraph2;
	hr=pFilterGraph->QueryInterface(IID_IFilterGraph2,
									reinterpret_cast<LPVOID*>(&pFilterGraph2));
	if (FAILED(hr)) {
		SetError(TEXT("IFilterGraph2���擾�ł��܂���B"));
		return false;
	}
	hr=pFilterGraph2->RenderEx(pInputPin,
								AM_RENDEREX_RENDERTOEXISTINGRENDERERS,NULL);
	pFilterGraph2->Release();
	if (FAILED(hr)) {
		SetError(TEXT("�f�������_�����\�z�ł��܂���B"));
		return false;
	}
	m_pFilterGraph=pFilterGraph;
	m_hwndRender=hwndRender;
	return true;
}


bool CVideoRenderer_VMR7Renderless::Finalize()
{
	/*
	CHECK_RELEASE(m_pSurfaceAllocatorNotify);
	CHECK_RELEASE(m_pSurfaceAllocator);
	CHECK_RELEASE(m_pImagePresenter);
	CHECK_RELEASE(m_pRenderer);
	*/
	return true;
}


bool CVideoRenderer_VMR7Renderless::SetVideoPosition(int SourceWidth,int SourceHeight,const RECT *pSourceRect,
								const RECT *pDestRect,const RECT *pWindowRect)
{
	IVMRWindowlessControl *pWindowlessControl;
	HRESULT hr;
	RECT rcSrc,rcDest;

	if (m_pSurfaceAllocator==NULL)
		return false;
	hr=m_pSurfaceAllocator->QueryInterface(IID_IVMRWindowlessControl,
							reinterpret_cast<LPVOID*>(&pWindowlessControl));
	if (FAILED(hr))
		return false;

	LONG Width,Height;

	if (SUCCEEDED(pWindowlessControl->GetNativeVideoSize(&Width,&Height,NULL,NULL))
			&& SourceWidth>0 && SourceHeight>0) {
		rcSrc.left=pSourceRect->left*Width/SourceWidth;
		rcSrc.top=pSourceRect->top*Height/SourceHeight;
		rcSrc.right=pSourceRect->right*Width/SourceWidth;
		rcSrc.bottom=pSourceRect->bottom*Height/SourceHeight;
	} else {
		rcSrc=*pSourceRect;
	}
	rcDest=*pDestRect;
	::OffsetRect(&rcDest,pWindowRect->left,pWindowRect->top);
	pWindowlessControl->SetVideoPosition(&rcSrc,&rcDest);
	pWindowlessControl->Release();
	::InvalidateRect(m_hwndRender,NULL,TRUE);
	return true;
}


bool CVideoRenderer_VMR7Renderless::GetDestPosition(RECT *pRect)
{
	bool fOK=false;

	if (m_pRenderer) {
		IVMRWindowlessControl *pWindowlessControl;

		if (SUCCEEDED(m_pSurfaceAllocator->QueryInterface(IID_IVMRWindowlessControl,
							reinterpret_cast<LPVOID*>(&pWindowlessControl)))) {
			fOK=SUCCEEDED(pWindowlessControl->GetVideoPosition(NULL,pRect));
			pWindowlessControl->Release();
		}
	}
	return fOK;
}


bool CVideoRenderer_VMR7Renderless::GetCurrentImage(void **ppBuffer)
{
	bool fOK=false;

	if (m_pRenderer) {
		IVMRWindowlessControl *pWindowlessControl;

		if (SUCCEEDED(m_pSurfaceAllocator->QueryInterface(IID_IVMRWindowlessControl,
							reinterpret_cast<LPVOID*>(&pWindowlessControl)))) {
			BYTE *pDib;

			if (SUCCEEDED(pWindowlessControl->GetCurrentImage(&pDib))) {
				*ppBuffer=pDib;
				fOK=true;
			}
			pWindowlessControl->Release();
		}
	}
	return fOK;
}


bool CVideoRenderer_VMR7Renderless::RepaintVideo(HWND hwnd,HDC hdc)
{
	bool fOK=false;

	if (m_pRenderer) {
		IVMRWindowlessControl *pWindowlessControl;

		if (SUCCEEDED(m_pSurfaceAllocator->QueryInterface(IID_IVMRWindowlessControl,
							reinterpret_cast<LPVOID*>(&pWindowlessControl)))) {
			if (SUCCEEDED(pWindowlessControl->RepaintVideo(hwnd,hdc)))
				fOK=true;
			pWindowlessControl->Release();
		}
	}
	return fOK;
}


bool CVideoRenderer_VMR7Renderless::DisplayModeChanged()
{
	bool fOK=false;

	if (m_pRenderer) {
		IVMRWindowlessControl *pWindowlessControl;

		if (SUCCEEDED(m_pSurfaceAllocator->QueryInterface(IID_IVMRWindowlessControl,
							reinterpret_cast<LPVOID*>(&pWindowlessControl)))) {
			if (SUCCEEDED(pWindowlessControl->DisplayModeChanged()))
				fOK=true;
			pWindowlessControl->Release();
		}
	}
	return fOK;
}


bool CVideoRenderer_VMR7Renderless::SetVisible(bool fVisible)
{
	// �E�B���h�E���ĕ`�悳����
	if (m_hwndRender)
		return ::InvalidateRect(m_hwndRender,NULL,TRUE)!=FALSE;
	return false;
}

#endif




bool CVideoRenderer::CreateRenderer(RendererType Type,CVideoRenderer **ppRenderer)
{
	switch (Type) {
	case RENDERER_DEFAULT:
		*ppRenderer=new CVideoRenderer_Default;
		break;
	case RENDERER_VMR7:
		*ppRenderer=new CVideoRenderer_VMR7;
		break;
	case RENDERER_VMR9:
		*ppRenderer=new CVideoRenderer_VMR9;
		break;
	case RENDERER_VMR7RENDERLESS:
		*ppRenderer=new CVideoRenderer_VMR7Renderless;
		break;
	default:
		return false;
	}
	return true;
}


LPCTSTR CVideoRenderer::EnumRendererName(int Index)
{
	static const LPCTSTR pszRendererName[] = {
		TEXT("Default"),
		TEXT("VMR7"),
		TEXT("VMR9"),
		TEXT("VMR7 Renderless"),
	};

	if (Index<0 || Index>=sizeof(pszRendererName)/sizeof(LPCTSTR))
		return NULL;
	return pszRendererName[Index];
}


CVideoRenderer::RendererType CVideoRenderer::ParseName(LPCTSTR pszName)
{
	LPCTSTR pszRenderer;
	int i;

	for (i=0;(pszRenderer=EnumRendererName(i))!=NULL;i++) {
		if (lstrcmpi(pszName,pszRenderer)==0)
			return (RendererType)i;
	}
	return RENDERER_UNDEFINED;
}