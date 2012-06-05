// mtlThumbnailViewerCOM.h : Declaration of the CmtlThumbnailViewerCOM
#ifndef _MTL_THUMBNAIL_VIEWER_COM_H
#define _MTL_THUMBNAIL_VIEWER_COM_H

//#pragma once
//#include "resource.h"       // main symbols

#include "mtlThumbnailViewer.h"
#include <shlobj.h>
#include <shlguid.h>
#include <AtlCom.h>


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CmtlThumbnailViewerCOM

class ATL_NO_VTABLE CmtlThumbnailViewerCOM :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CmtlThumbnailViewerCOM, &CLSID_mtlThumbnailViewerCOM>,
	public IPersistFile,
	public IExtractImage2
{
public:
	CmtlThumbnailViewerCOM()
	{
	}

	BEGIN_COM_MAP(CmtlThumbnailViewerCOM)
		COM_INTERFACE_ENTRY(IPersistFile)
		COM_INTERFACE_ENTRY(IExtractImage)
		COM_INTERFACE_ENTRY(IExtractImage2)
	END_COM_MAP()

	DECLARE_REGISTRY_RESOURCEID(IDR_MTLTHUMBNAILVIEWERCOM)

public:
	    // IPersistFile
    STDMETHODIMP GetClassID( CLSID* ) { return E_NOTIMPL; }
    STDMETHODIMP IsDirty() { return E_NOTIMPL; }
    STDMETHODIMP Save( LPCOLESTR, BOOL ) { return E_NOTIMPL; }
    STDMETHODIMP SaveCompleted( LPCOLESTR ) { return E_NOTIMPL; }
    STDMETHODIMP GetCurFile( LPOLESTR* ) { return E_NOTIMPL; }

	STDMETHOD(Load)( LPCOLESTR wszFile, DWORD i_size )
	{ 
		USES_CONVERSION;
		lstrcpyn ( m_szFilename, OLE2CT(wszFile), MAX_PATH );
		return S_OK;
	}

  // IExtractImage
  STDMETHODIMP GetLocation(LPWSTR pszPathBuffer,
								DWORD cchMax,
								DWORD *pdwPriority,
								const SIZE *prgSize,
								DWORD dwRecClrDepth,
								DWORD *pdwFlags);

  STDMETHODIMP Extract(HBITMAP* o_Thumbnail);

  // IExtractImage2
  STDMETHODIMP GetDateStamp(FILETIME *pDateStamp);
  
protected:
  TCHAR     m_szFilename [MAX_PATH];  // Full path to the file in question
};

OBJECT_ENTRY_AUTO(__uuidof(mtlThumbnailViewerCOM), CmtlThumbnailViewerCOM)
#endif