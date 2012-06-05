/////////////////////////////////////////////////////////////////////////////
// StudioGPUExportPlugIn.h : main header file for the StudioGPUExport plug-in
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CStudioGPUExportPlugIn
// See StudioGPUExportPlugIn.cpp for the implementation of this class
//

class CStudioGPUExportPlugIn : public CRhinoFileExportPlugIn
{
public:
  CStudioGPUExportPlugIn();
  ~CStudioGPUExportPlugIn();

  // Required overrides
  const wchar_t* PlugInName() const;
  const wchar_t* PlugInVersion() const;
  GUID PlugInID() const;
  BOOL OnLoadPlugIn();
  void OnUnloadPlugIn();

  // Online help overrides
  BOOL AddToPlugInHelpMenu() const;
  BOOL OnDisplayPlugInHelp( HWND hWnd ) const;

  // File export overrides
  void AddFileType( ON_ClassArray<CRhinoFileType>& extensions, const CRhinoFileWriteOptions& options );
  BOOL WriteFile( const wchar_t* filename, int index, CRhinoDoc& doc, const CRhinoFileWriteOptions& options );

private:
  ON_wString m_plugin_version;
  HANDLE m_mutex;

  // TODO: Add additional class information here
};

CStudioGPUExportPlugIn& StudioGPUExportPlugIn();



