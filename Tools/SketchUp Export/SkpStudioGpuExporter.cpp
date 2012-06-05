// SkpStudioGpuExporter.cpp : Implementation of CSkpStudioGpuExporter

#include "stdafx.h"
#include "SkpStudioGpuExporter.h"
#include "SGPUExporter.h"
#include "ExportResultsDlg.h"

#include <memory>


// CSkpStudioGpuExporter

//IEnumSketchUpExporter methods----------------------------------

//Get the number of exporters supported by this plugin
STDMETHODIMP CSkpStudioGpuExporter::get_Count(/*[out,retval]*/long* pCount)  
{  
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());  
    if(pCount == NULL)  
    {  
        return Error("Out value was null", IID_IEnumSketchUpExporter, E_POINTER);  
    }  
    *pCount = 1;  
    return S_OK;  
}

//Get an exporter based on the index passed in
STDMETHODIMP CSkpStudioGpuExporter::get_Item(/*[in]*/long nIndex, /*[out,retval]*/ISketchUpExporter** pExporter)  
{  
    AFX_MANAGE_STATE (AfxGetStaticModuleState ());  
    if(nIndex != 0)  
    {  
        return Error("Index must be exactly 0", IID_IEnumSketchUpExporter, E_INVALIDARG);  
    }  
    if(pExporter == NULL)  
    {  
        return Error("Out value was null", IID_IEnumSketchUpExporter, E_POINTER);  
    }  
    *pExporter = NULL;  

    // this is the Skp exporter  
    return QueryInterface(IID_ISketchUpExporter, (void**)pExporter);  
}   

// ISupportExporterAbout methods---------------------------------

//Get an about string that will be displayed in SketchUp's Help>About Plugins interface
STDMETHODIMP CSkpStudioGpuExporter::get_AboutString(/*[out,retval]*/BSTR* pAboutString)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState ());

    CString szAbout;
    szAbout.LoadString(IDS_EXPORT_GXB_ABOUT);
    *pAboutString = szAbout.AllocSysString();

    return S_OK;
}

STDMETHODIMP CSkpStudioGpuExporter::DoAbout()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState ());
    return S_FALSE;
}

//return whether this exporter supports the about plugins functionality
STDMETHODIMP CSkpStudioGpuExporter::get_SupportsAboutBox(/*[out,retval]*/BOOL* pSupports)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState ());
    *pSupports = TRUE;
    return S_OK;
}

// ISupportExporterVersion methods-------------------------------

//Get the version of this exporter
STDMETHODIMP CSkpStudioGpuExporter::get_Version(/*[out,reval]*/BSTR* bVersion)
{
    if(bVersion == NULL)
    {
        return E_POINTER;
    }

    CString szVersion;
	szVersion.LoadString(IDS_VERSION);
    *bVersion = szVersion.AllocSysString();

    return S_OK;
}

// ISketchUpExport methods---------------------------------------

//Get a unique id string for this exporter
STDMETHODIMP CSkpStudioGpuExporter::get_Id (BSTR* pID)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState ());

    CString desc ("com.sketchup.exporters.gxb");
    *pID = desc.AllocSysString ();
    return S_OK;
}

// This method is called to get a description of the type of export
STDMETHODIMP CSkpStudioGpuExporter::get_Description (BSTR *pDescription)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState ());

    CString desc ("StudioGPU (*.gxb)");
    *pDescription = desc.AllocSysString ();
    return S_OK;
}

// Get the default extension for files of this type
STDMETHODIMP CSkpStudioGpuExporter::get_FileExtension (BSTR *pExtension)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState ());

    CString ext ("gxb");
    *pExtension = ext.AllocSysString ();
    return S_OK;
}

//Return a value that indicates whether this exporter is a 3D Model exporter or a 2D Graphic exporter
STDMETHODIMP CSkpStudioGpuExporter::get_ExporterType (SkpExporterType* pType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState ());

    *pType = ExporterTypeModel3d;
    return S_OK;
}

STDMETHODIMP CSkpStudioGpuExporter::get_AllowsExistingFiles (BOOL *pAllows)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState ());

    *pAllows = false;
    return S_OK;
}


// Get the options needed for the export.  This method is called
// if the user clicks on the Options button on the dialog
STDMETHODIMP CSkpStudioGpuExporter::GetOptions (IUnknown* activeDocument)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState ());

    return S_OK;
}

// Do the export.  An interface to the active model is passed in.
STDMETHODIMP CSkpStudioGpuExporter::DoExport (BSTR fileName, IUnknown* activeDocument, IProgressCB* pCB)
{   
    AFX_MANAGE_STATE(AfxGetStaticModuleState ());

    //Initialize the exporter class
	//std::auto_ptr<SGPUExporter> exporter = new SGPUExporter();
	SGPUExporter exporter;

    //Do the export
	COLE2T fn(fileName);
	HRESULT hr = exporter.DoExport(fn, activeDocument, pCB)?S_OK:S_OK;//E_FAIL; //In other case application is hang down

	m_stats = exporter.GetStats();

    return hr;
}

//Display the Export Result dialog
STDMETHODIMP CSkpStudioGpuExporter::ShowSummary()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState ());

#ifndef _DEBUG
    //Show the summary
    CExportResultsDlg statsdlg;
    statsdlg.SetStats( m_stats );
    statsdlg.DoModal();
#endif

    return S_OK;
}