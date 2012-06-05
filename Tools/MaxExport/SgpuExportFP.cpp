/*****************************************************************************
**  SgpuExportFP.cpp
**
**	Function publishing mechanism for sgpu Export
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/


#include "SgpuExportFP.hpp"
#include "SgpuExport.hpp"
#include "ExportDoc.hpp"
#include "MaxExportUtils.hpp"
#include "SgpuMaxExportVersion.hpp"
#include "MaxObjectFlags.hpp"
#include "SgpuExportLib/include/SgpuExportLibVersion.hpp"

#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif 

#include "iFnPub.h"
#include "guplib.h"


#include <string>

using namespace std;
using namespace MaxExp;




int SgpuExportFP_GUP::m_NumInstances = 0;

SgpuExportFP_GUP::SgpuExportFP_GUP()
{
	++m_NumInstances;
	_RPT1( _CRT_WARN, "SgpuExportFP_GUP Constructor, numInstances = %d\n", m_NumInstances );
	int i=0;
}
SgpuExportFP_GUP::~SgpuExportFP_GUP()
{
	--m_NumInstances;	
	_RPT1( _CRT_WARN, "SgpuExportFP_GUP Destructor, numInstances = %d\n", m_NumInstances );
	int k=0;
}

// GUP Methods
DWORD	SgpuExportFP_GUP::Start( )
{
return GUPRESULT_KEEP; // Activate and Stay Resident
}
void	SgpuExportFP_GUP::Stop	( )
{
	//nothing to do here
}

// Loading/Saving
IOResult SgpuExportFP_GUP::Save(ISave *isave)
{
	return IO_OK;
}
IOResult SgpuExportFP_GUP::Load(ILoad *iload)
{
	return IO_OK;
}

SgpuExportFP_GUP_ClassDesc SgpuExportFP_GUP::theDesc;

//This is a contructor for class SgpuExportFP that takes a variable number of arugments.
SgpuExportFP	sgpu_export_fp_desc(
	SGPU_EXPORT_FP_INTERFACE_ID, //Interface_ID
	_T("SgpuExport"),		//Internal Fixed Name
	IDS_SGPU_EXPORT_FP,		//localized string resource ID
	&SgpuExportFP_GUP::theDesc,		//owning class descriptor
	FP_CORE,			//Flags

		//Functions -------------------------
		//Function that takes two numbers and multiplies them together
		//Note the scripter visible name of the function is passed in as a string
		ISgpuExportFP::em_products, _T("products"), 0, TYPE_FLOAT, 0, 2,
			_T("float_X"), 0, TYPE_FLOAT,
			_T("float_Y"), 0, TYPE_FLOAT,

		//Function that displays a message box
		ISgpuExportFP::em_message, _T("Message"), 0, TYPE_VOID, 0, 0,

		//Function that displays a message box
		ISgpuExportFP::em_getOptions, _T("Get_Options"), 0, TYPE_INTERFACE, 0, 0,

		//Function that displays a message box
		ISgpuExportFP::em_Do, _T("Do"), 0, TYPE_INT, 0, 2,
						_T("options"), 0, TYPE_INTERFACE,
						_T("exportFilePath"), 0, TYPE_TSTR_BR, f_inOut, FPP_IN_PARAM,

		//Function that displays a message box
		ISgpuExportFP::em_Do_2, _T("Do_2"), 0, TYPE_INT, 0, 3,
						_T("options"), 0, TYPE_INTERFACE,
						_T("exportFilePath"), 0, TYPE_TSTR_BR, f_inOut, FPP_IN_PARAM,
						_T("objects"), 0, TYPE_INODE_TAB_BR, f_inOut, FPP_IN_PARAM,

		//Get exporter version as in SGPU_EXPORTER_VERSION_STR in SgpuExportVersion.hpp
		ISgpuExportFP::em_GetExporterVersion, _T("Get_ExporterVersion"), 0, TYPE_TSTR_BV, 0, 0,
		
		//Get exporter version as in SGPU_EXPORT_LIB_VERSION_STR in 
		//..\ExporterPlugin\public\SgpuExportLib\include\SgpuExportLibVersion.hpp
		ISgpuExportFP::em_GetExportLibVersion, _T("Get_ExportLibVersion"), 0, TYPE_TSTR_BV, 0, 0,

		ISgpuExportFP::em_GetObjectFlagBool, _T("Get_ObjectFlagBool"), 0, TYPE_BOOL, 0, 2,
							_T("node"), 0, TYPE_INODE,
							_T("flagName"), 0, TYPE_TSTR_BR,
							
		ISgpuExportFP::em_SetObjectFlagBool, _T("Set_ObjectFlagBool"), 0, TYPE_VOID, 0, 3,
							_T("node"), 0, TYPE_INODE,
							_T("flagName"), 0, TYPE_TSTR_BR,
							_T("bVal"), 0, TYPE_BOOL,
		//Properties ------------------------
		//Property description that can has read / write functionality
		properties,
			ISgpuExportFP::em_getNum, ISgpuExportFP::em_setNum, _T("Number"), 0 , TYPE_FLOAT,

		
		end
);
//--------------------------------------------------------
//Maxscript usage:
//--------------------------------------------------------
//fpbasics.products 2.5 4
//--> 10.0
int SgpuExportFP::m_NumInstances = 0;

float SgpuExportFP::products(float x, float y)
{
	int k=m_NumInstances;
	return x * y;
}

//--------------------------------------------------------
//Maxscript usage:
//--------------------------------------------------------
//fpbasics.message()
//--> A standard Windows message box will then appear.
void SgpuExportFP::message()
{
	int k = m_NumInstances;
	MessageBox(NULL, _T("This was called via a void member function."), _T("Function Publishing Demonstration"), MB_OK);
}

//--------------------------------------------------------
//Maxscript usage:
//--------------------------------------------------------
//fpbasics.number
//--> 0.0
float SgpuExportFP::GetNum()
{
	int k=m_NumInstances;
	return m_Num;
}

//--------------------------------------------------------
//Maxscript usage:
//--------------------------------------------------------
//fpbasics.number
//--> 0.0
//fpbasics.number = 4.5
//--> 4.5
//fpbasics.number
//--> 4.5
void SgpuExportFP::SetNum(float x)
{
	int k = m_NumInstances;
	m_Num = x;
}


SgpuExportOptions* SgpuExportFP::Get_Options()
{
	SgpuExportOptions *pOptions = static_cast< SgpuExportOptions * > ( SgpuExportOptionsClassDesc::theDesc.Create( FALSE ) );
	return pOptions;
}


int SgpuExportFP::Do( FPInterface *i_pObject, TSTR & i_ExportFilepath )
{
	int retVal = IMPEXP_SUCCESS;
	std::string sInputFilepath;
	SgpuExportOptions *pOptions = dynamic_cast< SgpuExportOptions * > ( i_pObject );
	DBG_ASSERT( pOptions, "i_pObject passed in should be a sgpuExportOptions object" );
	LibXLTInitCleanupWrapper libXltWrapper;
	ExportLogger::Wrap wrapLogger( NULL );
	try {
		sInputFilepath = envString::WideCharToUTF8( pOptions->m_wsCurMaxFilepath );
		//main export document
		ExportDoc doc( i_ExportFilepath.data(), *pOptions );
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
		doc.Do( selectedNodes );
	} 
	catch ( export_failure & )
	{
		EXPLOG.WriteError( "export of %s failed", sInputFilepath.c_str() );
		retVal = IMPEXP_FAIL;
	}
	catch( export_cancel & )
	{
		retVal = IMPEXP_CANCEL;
	}
	catch( std::runtime_error &rerr )
	{
		EXPLOG.WriteError( "export of %s failed", sInputFilepath.c_str() );
		retVal = IMPEXP_FAIL;
	}
	return retVal;
}



int SgpuExportFP::Do_2( FPInterface *i_pOptions, TSTR &i_ExportFilepath, Tab<INode*>& i_Objects )
{

	int retVal = IMPEXP_SUCCESS;
	std::string sInputFilepath;
	SgpuExportOptions *pOptions = dynamic_cast< SgpuExportOptions * > ( i_pOptions );
	DBG_ASSERT( pOptions, "i_pObject passed in should be a sgpuExportOptions object" );
	LibXLTInitCleanupWrapper libXltWrapper;	
	ExportLogger::Wrap wrapLogger( NULL );
	try {

		if( i_Objects.Count() > 0 )
		{
			pOptions->m_bExportSelected = true;
		} else 
		{
			EXPLOG.WriteError( "export of %s: nothing to export!", sInputFilepath.c_str() );
			throw export_cancel();
		}

		sInputFilepath = envString::WideCharToUTF8( pOptions->m_wsCurMaxFilepath );
		//main export document
		ExportDoc doc( i_ExportFilepath.data(), *pOptions );
		//starts the log
		wrapLogger.Init( doc.m_FileName.c_str() );
		//set to estimate time taken to export
		SimpleProfile prof("Export");
		//do the export
		list< INode * > suggestedNodes;
		for( int i=0; i < i_Objects.Count(); ++i )
		{
			suggestedNodes.push_back( i_Objects[ i ] );
		}
		doc.Do( suggestedNodes );
	} 
	catch ( export_failure & )
	{
		EXPLOG.WriteError( "export of %s failed", sInputFilepath.c_str() );
		retVal = IMPEXP_FAIL;
	}
	catch( export_cancel & )
	{
		retVal = IMPEXP_CANCEL;
	}
	catch( std::runtime_error &rerr )
	{
		EXPLOG.WriteError( "export of %s failed", sInputFilepath.c_str() );
		retVal = IMPEXP_FAIL;
	}
	return retVal;

}



MSTR SgpuExportFP::Get_ExporterVersion()
{
	return SGPU_MAX_EXPORTER_VERSION;
}



MSTR SgpuExportFP::Get_ExportLibVersion()
{
	return SGPU_EXPORT_LIB_VERSION;
}

bool SgpuExportFP::Get_ObjectFlagBool( INode * i_pNode, TSTR &i_PropertyName )
{
	bool bRetVal;
	MSTR propName( i_PropertyName );
	std::wstring wPropertyName;
	MbcsToUnicode( propName.data(), wPropertyName );
	bool bSuccess = AllowedObjectFlags::GetValue< bool >( i_pNode, wPropertyName, bRetVal );
	if ( !bSuccess )
	{
		throw std::runtime_error("parameter not found");
	}
	return bRetVal;
}



void SgpuExportFP::Set_ObjectFlagBool( INode * i_pNode, TSTR &i_PropertyName, bool i_bVal )
{
	
	MSTR propName( i_PropertyName );
	std::wstring wPropertyName;	
	MbcsToUnicode( propName.data(), wPropertyName );
	bool bSuccess = AllowedObjectFlags::SetValue< bool >( i_pNode, wPropertyName, i_bVal );
	if ( !bSuccess )
	{
		throw std::runtime_error("parameter not found");
	}
}