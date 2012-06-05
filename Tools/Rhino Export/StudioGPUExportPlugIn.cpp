/////////////////////////////////////////////////////////////////////////////
// StudioGPUExportPlugIn.cpp : defines the initialization routines for the plug-in.
//

#include "StdAfx.h"
#include "StudioGPUExportPlugIn.h"
#include "ExportStudioGPU.h"

#define PLUGIN_VERSION L"1.2.3.1"

// The plug-in object must be constructed before any plug-in classes
// derived from CRhinoCommand. The #pragma init_seg(lib) ensures that
// this happens.

#pragma warning( push )
#pragma warning( disable : 4073 )
#pragma init_seg( lib )
#pragma warning( pop )

// Rhino plug-in declaration
RHINO_PLUG_IN_DECLARE

// Rhino plug-in developer declarations
// TODO: fill in the following developer declarations with
// your company information. Note, all of these declarations
// must be present or your plug-in will not load.
//
// When completed, delete the following #error directive.
//#error Developer declarations block is incomplete!
RHINO_PLUG_IN_DEVELOPER_ORGANIZATION( L"StudioGPU" );
RHINO_PLUG_IN_DEVELOPER_ADDRESS( L"1680 Vine Street suite 1010\r\nLos Angeles, CA 90028" );
RHINO_PLUG_IN_DEVELOPER_COUNTRY( L"United States" );
RHINO_PLUG_IN_DEVELOPER_PHONE( L"323.544.1003" );
RHINO_PLUG_IN_DEVELOPER_FAX( L"323.544.1003" );
RHINO_PLUG_IN_DEVELOPER_EMAIL( L"support@studiogpu.com" );
RHINO_PLUG_IN_DEVELOPER_WEBSITE( L"http://www.studiogpu.com" );
RHINO_PLUG_IN_UPDATE_URL( L"http://www.studiogpu.com/support" );

// The one and only CStudioGPUExportPlugIn object
static CStudioGPUExportPlugIn thePlugIn;

/////////////////////////////////////////////////////////////////////////////
// CStudioGPUExportPlugIn definition

CStudioGPUExportPlugIn& StudioGPUExportPlugIn()
{ 
  // Return a reference to the one and only CStudioGPUExportPlugIn object
  return thePlugIn; 
}

CStudioGPUExportPlugIn::CStudioGPUExportPlugIn()
{
	// Description:
	//   CStudioGPUExportPlugIn constructor. The constructor is called when the
	//   plug-in is loaded and "thePlugIn" is constructed. Once the plug-in
	//   is loaded, CStudioGPUExportPlugIn::OnLoadPlugIn() is called. The 
	//   constructor should be simple and solid. Do anything that might fail in
	//   CStudioGPUExportPlugIn::OnLoadPlugIn().

	m_plugin_version = PLUGIN_VERSION;

	m_mutex = CreateMutex( 0, 0, TEXT("{482F52CF-E4B2-47AD-8DC1-B29344C1714E}") );
}

CStudioGPUExportPlugIn::~CStudioGPUExportPlugIn()
{
	// Description:
	//   CStudioGPUExportPlugIn destructor. The destructor is called to destroy
	//   "thePlugIn" when the plug-in is unloaded. Immediately before the
	//   DLL is unloaded, CStudioGPUExportPlugIn::OnUnloadPlugin() is called. Do
	//   not do too much here. Be sure to clean up any memory you have allocated
	//   with onmalloc(), onrealloc(), oncalloc(), or onstrdup().

	CloseHandle( m_mutex );
}

/////////////////////////////////////////////////////////////////////////////
// Required overrides

const wchar_t* CStudioGPUExportPlugIn::PlugInName() const
{
  // Description:
  //   Plug-in name display string.  This name is displayed by Rhino when
  //   loading the plug-in, in the plug-in help menu, and in the Rhino 
  //   interface for managing plug-ins.

  // TODO: Return a short, friendly name for the plug-in.
  return L"GXB Export plug-in";
}

const wchar_t* CStudioGPUExportPlugIn::PlugInVersion() const
{
  // Description:
  //   Plug-in version display string. This name is displayed by Rhino 
  //   when loading the plug-in and in the Rhino interface for managing
  //   plug-ins.

  // TODO: Return the version number of the plug-in.
  return m_plugin_version;
}

GUID CStudioGPUExportPlugIn::PlugInID() const
{
  // Description:
  //   Plug-in unique identifier. The identifier is used by Rhino to
  //   manage the plug-ins.

  // TODO: Return a unique identifier for the plug-in.
  // {482F52CF-E4B2-47AD-8DC1-B29344C1714E}
  static const GUID StudioGPUExportPlugIn_UUID =
  { 0x482F52CF, 0xE4B2, 0x47AD, { 0x8D, 0xC1, 0xB2, 0x93, 0x44, 0xC1, 0x71, 0x4E } };
  return StudioGPUExportPlugIn_UUID;
}

BOOL CStudioGPUExportPlugIn::OnLoadPlugIn()
{
  // Description:
  //   Called after the plug-in is loaded and the constructor has been
  //   run. This is a good place to perform any significant initialization,
  //   license checking, and so on.  This function must return TRUE for
  //   the plug-in to continue to load.

  // TODO: Add plug-in initialization code here.
  return CRhinoFileExportPlugIn::OnLoadPlugIn();
}

void CStudioGPUExportPlugIn::OnUnloadPlugIn()
{
  // Description:
  //   Called when the plug-in is about to be unloaded.  After
  //   this function is called, the destructor will be called.

  // TODO: Add plug-in cleanup code here.
  CRhinoFileExportPlugIn::OnUnloadPlugIn();
}

/////////////////////////////////////////////////////////////////////////////
// Online help overrides

BOOL CStudioGPUExportPlugIn::AddToPlugInHelpMenu() const
{
  // Description:
  //   Return true to have your plug-in name added to the Rhino help menu.
  //   OnDisplayPlugInHelp will be called when to activate your plug-in help.

  return FALSE;
}

BOOL CStudioGPUExportPlugIn::OnDisplayPlugInHelp( HWND hWnd ) const
{
  // Description:
  //   Called when the user requests help about your plug-in.
  //   It should display a standard Windows Help file (.hlp or .chm).

  // TODO: Add support for online help here.
  return CRhinoFileExportPlugIn::OnDisplayPlugInHelp( hWnd );
}

/////////////////////////////////////////////////////////////////////////////
// File export overrides

void CStudioGPUExportPlugIn::AddFileType( ON_ClassArray<CRhinoFileType>& extensions, const CRhinoFileWriteOptions& options )
{
	// Description:
	//   When Rhino gets ready to display either the save or export file dialog,
	//   it calls AddFileType() once for each loaded file export plug-in.
	// Parameters:
	//   extensions [in] Append your supported file type extensions to this list.
	//   options [in] File write options.
	// Example:
	//   If your plug-in exports "Geometry Files" that have a ".geo" extension,
	//   then your AddToFileType(....) would look like the following:
	//
	//   CExportPlugIn::AddToFileType(ON_ClassArray<CRhinoFileType>&  extensions, const CRhinoFileWriteOptions& options)
	//   {
	//      CRhinoFileType ft(PlugInID(), L"Geometry Files (*.geo)", L"geo");
	//      extensions.Append(ft);
	//   }

	// TODO: Add supported file extensions here.
	CRhinoFileType ft(PlugInID(), L"StudioGPU files (*.gxb)", L"gxb");
	extensions.Append(ft);
}

BOOL CStudioGPUExportPlugIn::WriteFile( const wchar_t* filename, int index, CRhinoDoc& doc, const CRhinoFileWriteOptions& options )
{
	// Description:
	//   Rhino calls WriteFile() to write document geometry to an external file.
	// Parameters:
	//   filename [in] The name of file to write.
	//   index [in] The index of file extension added to list in AddToFileType().
	//   doc [in] The current Rhino document.
	//   options [in] File write options.
	// Remarks:
	//   The plug-in is responsible for opening the file and writing to it.
	// Return TRUE if successful, otherwise return FALSE.

	// TODO: Add file export code here.

	

	bool isCancelled;
	return ExportStudioGPU(doc, filename, options.Mode(CRhinoFileWriteOptions::SelectedMode), !options.Mode(CRhinoFileWriteOptions::BatchMode), PlugInName(), isCancelled);
}

