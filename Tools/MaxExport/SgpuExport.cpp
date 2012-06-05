/*****************************************************************************
**  SgpuExport.cpp
**
**	Highest level export class, derived from SceneExport of 3dsMax SDK
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/


#include "SgpuExport.hpp"

#include "ExportDoc.hpp"
#include "MaxExportOptionsGUI.hpp"
#include "MaxExportUtils.hpp"

#include "iFnPub.h"
#include "guplib.h"
#include "Core/env/envString.hpp"
#if defined(_UNICODE)
#error "The 3ds Max Model exporter doesnt support unicode configuration"
#endif

using namespace std;
using namespace MaxExp;




//=============================================================================
// SgpuExport class member functions
//====================
int SgpuExporter::m_NumInstances=0;
SgpuExporter::SgpuExporter() {
	++m_NumInstances;
	_RPT1( _CRT_WARN, "SgpuExporter Constructor, numInstances = %d\n", m_NumInstances );
	int i=0;
}

SgpuExporter::~SgpuExporter() {
	--m_NumInstances;
	_RPT1( _CRT_WARN, "SgpuExporter Destructor, numInstances = %d\n", m_NumInstances );
	int k=0;
}

int SgpuExporter::ExtCount() {
	return SgpuExportClassDesc::theSgpuExportClassDesc.ExtCount();
}

const TCHAR *
SgpuExporter::Ext(int n) {		// Extensions supported for import/export modules
	return SgpuExportClassDesc::theSgpuExportClassDesc.Ext(n);
}


const TCHAR *
SgpuExporter::LongDesc() {			// Long ASCII description (i.e. "Targa 2.0 Image File")
	return GetString(IDS_SGPU_FILE);
}


const TCHAR *
SgpuExporter::ShortDesc() {			// Short ASCII description (i.e. "Targa")
	return GetString(IDS_SGPU_FILE);
}

const TCHAR *
SgpuExporter::AuthorName() {			// ASCII Author name
	return GetString(IDS_SGPU_AUTHOR);
}

const TCHAR *
SgpuExporter::CopyrightMessage() {	// ASCII Copyright message
	return GetString(IDS_COPYRIGHT_SGPU);
}

const TCHAR *
SgpuExporter::OtherMessage1() {		// Other message #1
	return _T("");
}

const TCHAR *
SgpuExporter::OtherMessage2() {		// Other message #2
	return _T("");
}

unsigned int
SgpuExporter::Version() {				// Version number * 100 (i.e. v3.01 = 301)
	return 200;
}

void
SgpuExporter::ShowAbout(HWND hWnd) {			// Optional
}

// SCENE_EXPORT_SELECTED option is supported for all extensions
BOOL SgpuExporter::SupportsOptions(int ext, DWORD options) {
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}



//=============================================================================
// Highest level export function
//=============================================================================

int SgpuExporter::DoExport(
						   const MCHAR *i_szFileName, //the full path name of the output file
						   ExpInterface *i_pEi, //ExportInterface dont know what this is
						   Interface *i_pCoreInterface, // GetCoreInterface()
						   BOOL i_bSuppressPrompts, //supress any dialogs
						   DWORD i_Options // options set by the max sdk (eg: SCENE_EXPORT_SELECTED )
						   ) 
{
	LibXLTInitCleanupWrapper libXltWrapper;
	int retVal = IMPEXP_SUCCESS;
	{	//For scoping ExportLog
		//automatically opens and closes the log on file
		ExportLogger::Wrap wrapLogger( NULL );
		string sInputFileName;
		try {
			//a smart pointer for the progress bar
			//InterfaceProgressSP ip( i_pCoreInterface);
			//extended options which include several sgpu specific export parameters

			Interface *pMaxInterface = GetCOREInterface();
			if( NULL == pMaxInterface )
			{
				throw(std::runtime_error("max: not able to get the CoreInterface") );
			}

			SgpuExportOptionsGUI exportOptionsGUI;
			exportOptionsGUI.Init( pMaxInterface, i_Options );
			bool proceed = true;
			
			sInputFileName = envString::WideCharToUTF8( exportOptionsGUI.m_Options.m_wsCurMaxFilepath );
			if( !i_bSuppressPrompts ) {
				proceed = exportOptionsGUI.ShowDialog();
			}
			if ( proceed )
			{


				//main export document
				ExportDoc doc(  i_szFileName, exportOptionsGUI.m_Options );
				//starts the log
				wrapLogger.Init( doc.m_FileName.c_str() );
				//set to estimate time taken to export
				SimpleProfile prof("Export");

				//get all the selected nodes
				list< INode * > selectedNodes;
				if (doc.m_Options.m_bExportSelected)
				{
					int nSelectedNodes = GetCOREInterface()->GetSelNodeCount();
					for (int i = 0; i < nSelectedNodes; ++i)
					{
						selectedNodes.push_back(GetCOREInterface()->GetSelNode(i));
					}
				}

				//do the export
				doc.Do(  selectedNodes );
			} 
			else
			{
				retVal = IMPEXP_CANCEL;
			}
		}
		catch ( export_failure & )
		{
			EXPLOG.WriteError( "export of %s failed", sInputFileName.c_str() );
			retVal = IMPEXP_FAIL;
		}
		catch( export_cancel & )
		{
			retVal = IMPEXP_CANCEL;
		}
		catch( std::runtime_error &rerr )
		{
			EXPLOG.WriteError( "export of %s failed", sInputFileName.c_str() );
			retVal = IMPEXP_FAIL;
		}
	}
	if( !i_bSuppressPrompts )
	{
		if( retVal == IMPEXP_SUCCESS )
		{
			MessageBox( NULL, (LPCSTR)"Export Succeeded", (LPCSTR)"SgpuExport", MB_OK );
		} else if( retVal == IMPEXP_FAIL )
		{
			MessageBox( NULL, (LPCSTR)"Export Failed", (LPCSTR)"SgpuExport", MB_OK );
		}
	}
	
	return retVal;
}
/*
DWORD SgpuExport::DoExport( const TCHAR *name, BOOL suppressPrompts, DWORD options)
{
	Interface *ip = GetCOREInterface();
	int retVal =  DoExport( name, NULL, ip, suppressPrompts, options);
	return static_cast< DWORD > ( retVal );
}
*/



//static definition of the class desc
SgpuExportClassDesc SgpuExportClassDesc::theSgpuExportClassDesc;

#if defined(SGPU_MIXIN_FP)
//This is a contructor for class FP_SgpuExporterGenerator that takes a variable number of arugments.
static FPInterfaceDesc sgpuModelExportFP_Desc(
	SGPU_EXPORT_FP_MIXIN_INTERFACE_ID, _T("ISgpuExportFP"), //Interface_ID
	0,		//localized string resource ID
	&SgpuExportClassDesc::theSgpuExportClassDesc,		//owning class descriptor
	FP_MIXIN,			//Flags

		//Functions -------------------------
		//Function that takes two numbers and multiplies them together
		//Note the scripter visible name of the function is passed in as a string
	/*	ISgpuExportFP::kDoExport, _T("do"), 0, TYPE_DWORD, 0, 3,
			_T("fname"), 0, TYPE_FILENAME,
			_T("suppressPrompts"), 0, TYPE_BOOL,
			_T("options"),0, TYPE_DWORD,
*/
		ISgpuExportFP::kFoo, _T("foo"), 0, TYPE_VOID, 0, 1,
					_T("iArg"), 0, TYPE_DWORD,
	end
);


//functions  inherited from FPMixinInterface
FPInterfaceDesc* ISgpuExportFP::GetDescByID(Interface_ID id) 
{
	if (id == SGPU_EXPORT_FP_MIXIN_INTERFACE_ID ) return &sgpuModelExportFP_Desc;
	return NULL;
}
FPInterfaceDesc* ISgpuExportFP::GetDesc()
{
	return GetDescByID( SGPU_EXPORT_FP_MIXIN_INTERFACE_ID );
}
#endif
//=============================================================================
// SgpuExportClassDesc
//=============================================================================


int SgpuExportClassDesc::ExtCount() {
	return 3;
}

const TCHAR *
SgpuExportClassDesc::Ext(int n) {		// Extensions supported for import/export modules
	switch(n) {
		case 0:
			return _T("gxb");
		case 1:
			return _T("gab");		
		case 2:
			return _T("cam");
	}
	return _T("");
}

void *	SgpuExportClassDesc::Create(BOOL loading ) 
{ 
	//AddInterface( &sgpuModelExportFP_Desc  ); 
	return new SgpuExporter(); 
}
