// SkpStudioGpuExporter.h : Declaration of the CSkpStudioGpuExporter

#pragma once
#include "resource.h"       // main symbols


#include "SkpStudioGpuExport.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CSkpStudioGpuExporter

class ATL_NO_VTABLE CSkpStudioGpuExporter :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSkpStudioGpuExporter, &CLSID_SkpStudioGpuExporter>,
	public ISkpStudioGpuExporter,

	public IEnumSketchUpExporter,
    public ISketchUpExporter,
    //public ISupportExporterOptions,
    public ISupportExporterAbout,
    public ISupportExporterSummary,
    public ISupportExporterProgress,
    public ISupportExporterVersion
{
public:
	CSkpStudioGpuExporter()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SKPSTUDIOGPUEXPORTER)


BEGIN_COM_MAP(CSkpStudioGpuExporter)
	COM_INTERFACE_ENTRY(ISkpStudioGpuExporter)

	COM_INTERFACE_ENTRY(IEnumSketchUpExporter)
    COM_INTERFACE_ENTRY(ISketchUpExporter)
    //COM_INTERFACE_ENTRY(ISupportExporterOptions)
    COM_INTERFACE_ENTRY(ISupportExporterAbout)
    COM_INTERFACE_ENTRY(ISupportExporterSummary)
    COM_INTERFACE_ENTRY(ISupportExporterProgress)
    COM_INTERFACE_ENTRY(ISupportExporterVersion)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()


	//IEnumSketchUpExporter methods----------------------------------
    STDMETHOD (get_Count)(/*[out,retval]*/long* pCount);  
    STDMETHOD (get_Item)(/*[in]*/long nIndex, /*[out,retval]*/ISketchUpExporter** pExporter);  

    // ISupportExporterAbout methods---------------------------------
    STDMETHOD (get_AboutString)(/*[out,retval]*/BSTR* pAboutString);
    STDMETHOD (DoAbout)();
    STDMETHOD (get_SupportsAboutBox)(/*[out,retval]*/BOOL* pSupports);

    // ISupportExporterVersion methods-------------------------------
    STDMETHOD(get_Version)(/*[out,reval]*/BSTR* bVersion);

    // ISketchUpExporter methods---------------------------------------
    STDMETHOD (get_Id) (BSTR* pID);

    // This method is called to get a description of the type of export
    STDMETHOD (get_Description) (BSTR* pDescription);

    // Get the default extension for files of this type
    STDMETHOD (get_FileExtension) (BSTR* pExtension);

    //Get the type of exporter (3d, 2d, graphic, etc.)
    STDMETHOD (get_ExporterType) (SkpExporterType* pType);

    //Determine if we should overwrite existing files without prompting the user
    STDMETHOD (get_AllowsExistingFiles) (BOOL *pAllows);

    // Do the export.  An interface to the active model is passed in.
    STDMETHOD (DoExport) (BSTR fileName, IUnknown* activeDocument, IProgressCB* pCB);

    // ISupportExporterOptions methods-------------------------------
    STDMETHOD(GetOptions)(IUnknown* activeDocument);

    // ISupportExporterSummary methods-------------------------------
    STDMETHOD(ShowSummary)();




	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

private:
	std::wstring m_stats;

};

OBJECT_ENTRY_AUTO(__uuidof(SkpStudioGpuExporter), CSkpStudioGpuExporter)
