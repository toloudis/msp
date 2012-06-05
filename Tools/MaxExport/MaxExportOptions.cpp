
/*****************************************************************************
**  MaxExportOptions.cpp
**
**	class which holds the GUI and data-binding members of
**  the max export options
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "MaxExportOptions.hpp"
#include "MaxExportUtils.hpp"
#include "SgpuMaxExportVersion.hpp"
#include "SgpuExportLib/include/SgpuExportLibVersion.hpp"

#include "Core/env/envString.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsXMLWriter.hpp"

#include <string>
#include "tinyxml.h"
using namespace std;
using namespace MaxExp;

static BOOL sInterfaceAdded = FALSE;

void SgpuExportOptions::Init( bool bExportSelected )
{
	m_bExportSelected = bExportSelected;
	m_bExportPolyAsTriangle = true;
	m_bExportParentIfChildExports = false;
	m_bExportChildIfParentExports = true;
	m_bSupportXref = false;
	m_bExportPivotAsSeparateNode = false;
	m_bMergeBasedOnMtls = false;
	m_nMaxNumTrianglesInMergedMesh = 30000;
	m_eExportIntent = eModel;
	m_bExportGeom = true;
	m_bCompressVertexAnim = true;
	m_fToleranceVertexAnim= 0.0f;
	
	if( m_eExportIntent != eModel )
	{
		m_bExportChildIfParentExports = false;		
	}

	InitAnimRangeParams( GetTicksPerFrame(), 0, 0 );

	
	m_fRenderImageAspect = 0;
	m_fRenderApertureWidth = 0;
}


void SgpuExportOptions::InitAnimRangeParams( int i_nTicksPerFrame, TimeValue i_StartTime, TimeValue i_EndTime )
{
	m_StepTime = i_nTicksPerFrame;
	m_fStepFrame = 1.0f;
	DBG_ASSERT( i_nTicksPerFrame > 0 , "i_nTicksPerFrame: " << i_nTicksPerFrame << " should be > 0 " );
	m_StartTime = i_StartTime;
	m_EndTime = i_EndTime;
	m_fStartFrame = i_StartTime / m_StepTime;
	m_fEndFrame = i_EndTime / m_StepTime;
	m_fMaxEndFrame = m_fEndFrame;
	m_fStepFrame = 1;
}

void SgpuExportOptions::InitRenderCameraParams( float i_fRenderImageAspect, float i_fRenderApertureWidth )
{

	m_fRenderImageAspect = i_fRenderImageAspect;
	m_fRenderApertureWidth = i_fRenderApertureWidth;
	DBG_ASSERT( i_fRenderImageAspect > 0.0f, "i_fREnderImageAspect: " << i_fRenderImageAspect << " should be > 0.0f"  );
	DBG_ASSERT( i_fRenderApertureWidth > 0.0f, "i_fRenderApertureWidth: " << i_fRenderApertureWidth << " should be > 0.0f" );
}


	void SgpuExportOptions::Set_AnimRange( int i_nTicksPerFrame, float i_fStartFrame, float i_fEndFrame )
	{
		DBG_ASSERT( (i_nTicksPerFrame > 0 ), "TicksPerFrame " << i_nTicksPerFrame << "should be greater than zero" );
		TimeValue startTime = static_cast< TimeValue > ( i_fStartFrame * i_nTicksPerFrame );
		TimeValue endTime = static_cast< TimeValue > ( i_fEndFrame * i_nTicksPerFrame );
		InitAnimRangeParams( i_nTicksPerFrame, startTime, endTime );
	}

//========================================================================
// Output the current options to the log file
// 	
//========================================================================
void SgpuExportOptions::Log( ExportLogger &logger ) const
{
	logger.WriteStartElement( string( "ExportOptions" ) );
	logger.WriteElement( string( "ExportSelected"), string( SGPU_BOOL_STRING( m_bExportSelected ) ) );
	logger.WriteElement( string( "ExportPolyAsTriangle" ), string( SGPU_BOOL_STRING( m_bExportPolyAsTriangle ) ) ); 
	logger.WriteElement( string( "ExportParentIfChildExports" ), string( SGPU_BOOL_STRING( m_bExportParentIfChildExports ) ) ); 
	logger.WriteElement( string( "ExportChildIfParentExports" ), string( SGPU_BOOL_STRING( m_bExportChildIfParentExports ) ) ); 	
	logger.WriteElement( string( "SupportXrefs" ), string( SGPU_BOOL_STRING( m_bSupportXref ) ) ); 
	logger.WriteElement( string( "ExportPivitAsSeparateNode" ), string( SGPU_BOOL_STRING( m_bExportPivotAsSeparateNode ) ) ); 		
	logger.WriteElement( string( "Merge TriMesh-es Based on Materials " ), string( SGPU_BOOL_STRING( m_bMergeBasedOnMtls ) ) );		
	{
		stringstream ss;
		ss << m_nMaxNumTrianglesInMergedMesh;
		logger.WriteElement( string( "Max Num Triangles In Merged Mesh " ), ss.str() );
	}
	logger.WriteElement( string( "ExportIntent"), IntentAsChar( m_eExportIntent ).c_str() );
	logger.WriteElement( string( "ExportGeom" ), string( SGPU_BOOL_STRING( m_bExportGeom ) ) );
	logger.WriteElement( string( "ExportAnim" ), string( SGPU_BOOL_STRING( !m_bExportGeom ) ) );
	logger.WriteElement( string( "Use Compression For Vertex Animation" ), string( SGPU_BOOL_STRING( !m_bCompressVertexAnim ) ) );
	{
		stringstream ss;
		ss << m_fToleranceVertexAnim;
		logger.WriteElement( string( "Tolerance for Vertex Animation " ), ss.str() ); 		
	}
	{
		stringstream ss;
		ss << m_fStartFrame;
		logger.WriteElement( string( "StartFrame " ), ss.str() ); 		
	}
	{
		stringstream ss;
		ss << m_fEndFrame;
		logger.WriteElement( string( "EndFrame " ), ss.str() ); 		
	}
	{
		stringstream ss;
		ss << m_fStepFrame;
		logger.WriteElement( string( "StepFrame " ), ss.str() ); 		
	}
	
	{
		stringstream ss;
		ss << m_fRenderImageAspect;
		logger.WriteElement( string( "RenderImageAspect " ), ss.str() ); 		
	}
	
	{
		stringstream ss;
		ss << m_fRenderApertureWidth;
		logger.WriteElement( string( "RenderApertureWidth " ), ss.str() ); 		
	}
	
	std::string utf8Filepath = envString::WideCharToUTF8( m_wsCurMaxFilepath );
	logger.WriteElement( string("Current Max Filepath " ), utf8Filepath );
	
	logger.WriteEndElement();
}

void SgpuExportOptions::Log( std::string &outFile ) const
{
		
	TiXmlDocument doc;
	{
		TiXmlDeclaration dec;
		dec.Parse( "<?xml version='1.0' encoding='UTF-8'?>", 0, TIXML_ENCODING_UNKNOWN );
		doc.InsertEndChild( dec );
	}
	TiXmlElement opts("Options");
	
	{
		TiXmlElement option( "ExporterVersion" );
		option.SetAttribute( "type", "string" );
		option.SetAttribute( "value", SGPU_MAX_EXPORTER_VERSION );
		opts.InsertEndChild( option );
	}
	{
		TiXmlElement option( "ExportIntent" );
		option.SetAttribute( "type", "int" );
		option.SetAttribute( "value", static_cast< int > ( m_eExportIntent ) );
		opts.InsertEndChild( option );
	}
	{
		TiXmlElement option( "ExportSelected" );
		option.SetAttribute( "type", "bool" );
		option.SetAttribute( "value", m_bExportSelected );
		opts.InsertEndChild( option );
	}
	{
		TiXmlElement option( "ExportPolyAsTriangle" );
		option.SetAttribute( "type", "bool" );
		option.SetAttribute( "value", m_bExportPolyAsTriangle );
		opts.InsertEndChild( option );
	}
	{
		TiXmlElement option( "ExportParentIfChildExport" );
		option.SetAttribute( "type", "bool" );
		option.SetAttribute( "value", m_bExportParentIfChildExports );
		opts.InsertEndChild( option );
	}
	{
		TiXmlElement option( "ExportChildIfParentExports" );
		option.SetAttribute( "type", "bool" );
		option.SetAttribute( "value", m_bExportChildIfParentExports );
		opts.InsertEndChild( option );
	}
	{
		TiXmlElement option( "SupportXrefs" );
		option.SetAttribute( "type", "bool" );
		option.SetAttribute( "value", m_bSupportXref );
		opts.InsertEndChild( option );
	}
	{
		TiXmlElement option( "ExportPivotAsSeparateNode" );
		option.SetAttribute( "type", "bool" );
		option.SetAttribute( "value", m_bExportPivotAsSeparateNode );
		opts.InsertEndChild( option );
	}
	{
		TiXmlElement option( "MergeTriMeshesBasedOnMaterials" );
		option.SetAttribute( "type", "bool" );
		option.SetAttribute( "value", m_bMergeBasedOnMtls );
		opts.InsertEndChild( option );
	}
	{
		TiXmlElement option( "MaxNumTrianglesInMergedMesh" );
		option.SetAttribute( "type", "int" );
		option.SetAttribute( "value", m_nMaxNumTrianglesInMergedMesh );
		opts.InsertEndChild( option );
	}
	
	{
		TiXmlElement option( "ExportGeom" );
		option.SetAttribute( "type", "bool" );
		option.SetAttribute( "value", m_bExportGeom );
		opts.InsertEndChild( option );
	}
	{
		TiXmlElement option( "UseCompressionForVertexAnimation" );
		option.SetAttribute( "type", "bool" );
		option.SetAttribute( "value",m_bCompressVertexAnim );
		opts.InsertEndChild( option );
	}

	{
		TiXmlElement option( "ToleranceForVertexAnimation" );
		option.SetAttribute( "type", "float" );
		option.SetDoubleAttribute( "value", static_cast< double > ( m_fToleranceVertexAnim ) );
		opts.InsertEndChild( option );
	}
	
	{
		TiXmlElement option( "StartFrame" );
		option.SetAttribute( "type", "float" );
		option.SetDoubleAttribute( "value", static_cast< double >( m_fStartFrame ) );
		opts.InsertEndChild( option );
	}
		
	{
		TiXmlElement option( "EndFrame" );
		option.SetAttribute( "type", "float" );
		option.SetDoubleAttribute( "value",  static_cast< double >( m_fEndFrame ) );
		opts.InsertEndChild( option );
	}
	
	{
		TiXmlElement option( "StepFrame" );
		option.SetAttribute( "type", "float" );
		option.SetDoubleAttribute( "value",  static_cast< double >( m_fStepFrame ) );
		opts.InsertEndChild( option );
	}
	
	{
		TiXmlElement option( "RenderImageAspect" );
		option.SetAttribute( "type", "float" );
		option.SetDoubleAttribute( "value",  static_cast< double >( m_fRenderImageAspect ) );
		opts.InsertEndChild( option );
	}
	{
		TiXmlElement option( "RenderApertureWidth" );
		option.SetAttribute( "type", "float" );
		option.SetDoubleAttribute( "value", static_cast< double > ( m_fRenderApertureWidth ) );
		opts.InsertEndChild( option );
	}
	
	{
		std::string utf8FileName = envString::WideCharToUTF8( m_wsCurMaxFilepath );
		TiXmlElement option( "CurrentMaxFilepath" );
		option.SetAttribute( "type", "string" );
		option.SetAttribute( "value", utf8FileName.c_str() );
		opts.InsertEndChild( option );
	}
	
	doc.InsertEndChild( opts );
	
	outFile << doc;
}

void SgpuExportOptions::Init( const std::string &inFile ) 
{
		
	TiXmlDocument doc;
	doc.Parse( inFile.c_str() );
	Init( false );
	{
		const TiXmlElement *pOpts = doc.FirstChildElement("Options");
		if( pOpts == NULL )
		{
			return;
		}

		const TiXmlElement *pChild = pOpts->FirstChildElement("ExportSelected");
		if( pChild )
		{
			int iVal;
			const char *pAttr = pChild->Attribute("value", &iVal);
			m_bExportSelected = (iVal) ? true : false;
		}
		pChild = pOpts->FirstChildElement("ExportPolyAsTriangle" );
		if( pChild )
		{
			int iVal;
			const char *pAttr = pChild->Attribute("value", &iVal);
			m_bExportPolyAsTriangle = (iVal) ? true : false;
		}
		
		pChild = pOpts->FirstChildElement( "ExportParentIfChildExport" );
		if( pChild )
		{
			int iVal;
			const char *pAttr = pChild->Attribute("value", &iVal);
			m_bExportParentIfChildExports = (iVal) ? true : false;
		}
		
		pChild = pOpts->FirstChildElement( "ExportChildIfParentExports" );
		if( pChild )
		{
			int iVal;
			const char *pAttr = pChild->Attribute("value", &iVal);
			m_bExportChildIfParentExports = (iVal) ? true : false;
		}
		pChild = pOpts->FirstChildElement( "SupportXrefs" );
		if( pChild )
		{
			int iVal;
			const char *pAttr = pChild->Attribute("value", &iVal);
			m_bSupportXref  = (iVal) ? true : false;
		}
		pChild = pOpts->FirstChildElement( "ExportPivotAsSeparateNode" );
		if( pChild )
		{
			int iVal;
			const char *pAttr = pChild->Attribute("value", &iVal);
			m_bExportPivotAsSeparateNode  = (iVal) ? true : false;
		}
		
		pChild = pOpts->FirstChildElement( "MergeTriMeshesBasedOnMaterials" );
		if( pChild )
		{
			int iVal;
			const char *pAttr = pChild->Attribute("value", &iVal);
			m_bMergeBasedOnMtls = (iVal) ? true : false;
		}
		
		pChild = pOpts->FirstChildElement( "MaxNumTrianglesInMergedMesh" );
		if( pChild )
		{
			int iVal;
			const char *pAttr = pChild->Attribute("value", &iVal);
			m_nMaxNumTrianglesInMergedMesh = iVal;
		}
		
		pChild = pOpts->FirstChildElement( "ExportIntent" );
		if( pChild )
		{
			int iVal;
			const char *pAttr = pChild->Attribute("value", &iVal);
			 m_eExportIntent = static_cast< Intent >( iVal );
		}
		pChild = pOpts->FirstChildElement( "ExportGeom" );
		if( pChild )
		{
			int iVal;
			const char *pAttr = pChild->Attribute("value", &iVal);
			 m_bExportGeom = static_cast< bool >( iVal );
		}
		
		pChild = pOpts->FirstChildElement( "UseCompressionForVertexAnimation" );
		if( pChild )
		{
			int iVal;
			const char *pAttr = pChild->Attribute("value", &iVal);
			 m_bCompressVertexAnim = static_cast< bool >( iVal );
		}
	
		pChild = pOpts->FirstChildElement( "ToleranceForVertexAnimation" );
		if( pChild )
		{
			double dVal;
			const char *pAttr = pChild->Attribute("value", &dVal);
			m_fToleranceVertexAnim = static_cast< float >( dVal );
		}
		
		pChild = pOpts->FirstChildElement( "StartFrame" );
		if( pChild )
		{
			double dVal;
			const char *pAttr = pChild->Attribute("value", &dVal);
			m_fStartFrame = static_cast< float >( dVal );
		}
		
		pChild = pOpts->FirstChildElement( "EndFrame" );
		if( pChild )
		{
			double dVal;
			const char *pAttr = pChild->Attribute("value", &dVal);
			m_fEndFrame = static_cast< float >( dVal );
		}
		
		pChild = pOpts->FirstChildElement( "StepFrame" );
		if( pChild )
		{
			double dVal;
			const char *pAttr = pChild->Attribute("value", &dVal);
			m_fStepFrame = static_cast< float >( dVal );
		}
		
		pChild = pOpts->FirstChildElement( "RenderImageAspect" );
		if( pChild )
		{
			double dVal;
			const char *pAttr = pChild->Attribute("value", &dVal);
			m_fRenderImageAspect = static_cast< float >( dVal );
		}
		
		
		pChild = pOpts->FirstChildElement( "RenderApertureWidth" );
		if( pChild )
		{
			double dVal;
			const char *pAttr = pChild->Attribute("value", &dVal);
			m_fRenderApertureWidth = static_cast< float >( dVal );
		}
		
		pChild = pOpts->FirstChildElement( "CurrentMaxFilepath" );
		if( pChild )
		{
			std::string sVal( pChild->Attribute("value") );
			m_wsCurMaxFilepath = envString::UTF8ToWideChar( sVal );
		}
	}
}

MSTR  SgpuExportOptions::Get_ExporterVersion()
{
	return SGPU_MAX_EXPORTER_VERSION;

}


MSTR  SgpuExportOptions::Get_ExportLibVersion()
{
	return SGPU_EXPORT_LIB_VERSION;

}
MSTR SgpuExportOptions::Get_CurMaxFilepath()
{
	MSTR mbcsCurMaxFilepath;
	int nBytes = UnicodeToMbcs( m_wsCurMaxFilepath, mbcsCurMaxFilepath );
	return mbcsCurMaxFilepath;
}

void SgpuExportOptions::Set_CurMaxFilepath( MSTR& i_filepath )
{
	int nBytes = MbcsToUnicode( i_filepath.data(), m_wsCurMaxFilepath );

}

SgpuExportOptionsClassDesc SgpuExportOptionsClassDesc::theDesc;

//This is a contructor for class FP_SgpuExporterGenerator that takes a variable number of arugments.
static FPInterfaceDesc sgpuExportOptionsFP_Desc(
	SGPU_EXPORT_OPTIONS_FP_INTERFACE_ID, _T("ISgpuExportOptionsFP"), //Interface_ID
	0,		//localized string resource ID
	&SgpuExportOptionsClassDesc::theDesc,		//owning class descriptor
	FP_MIXIN,			//Flags

	ISgpuExportOptionsFP::kFoo, _T("foo"), 0, TYPE_VOID, 0, 1,
	_T("iArg"), 0, TYPE_DWORD,

	ISgpuExportOptionsFP::k_LogToString, _T("LogToString"), 0, TYPE_TSTR_BV, 0, 0,

	ISgpuExportOptionsFP::k_InitFromString, _T("InitFromString"), 0 , TYPE_VOID, 0, 1, 
	_T("pzXmlFile"), 0, TYPE_TSTR_BR, f_inOut, FPP_IN_PARAM,

	ISgpuExportOptionsFP::kGet_ExporterVersion, _T("sExporterVersion"), 0, TYPE_TSTR_BV, 0, 0,
	
	ISgpuExportOptionsFP::kGet_ExportLibVersion, _T("sExportLibVersion"), 0, TYPE_TSTR_BV, 0, 0,

	ISgpuExportOptionsFP::kGet_CurMaxFilepath, _T("Get_CurMaxFilepath"), 0, TYPE_TSTR_BV, 0, 0,

	ISgpuExportOptionsFP::kSet_CurMaxFilepath, _T("Set_CurMaxFilepath"), 0 , TYPE_VOID, 0, 1, 
	_T("pzFilepath"), 0, TYPE_TSTR_BR, f_inOut, FPP_IN_PARAM,

	ISgpuExportOptionsFP::kSet_AnimRange, _T("Set_AnimRange"), 0, TYPE_VOID, 0, 3,
		_T("nTicksPerFrame"), 0, TYPE_INT,
		_T("fStartFrame"), 0, TYPE_FLOAT,
		_T("fEndFrame"), 0, TYPE_FLOAT,

	properties,
	ISgpuExportOptionsFP::kGet_bExportPolyAsTriangle, ISgpuExportOptionsFP::kSet_bExportPolyAsTriangle, _T("bExportPolyAsTriangle"), 0 , TYPE_BOOL,

	ISgpuExportOptionsFP::kGet_bExportSelected, ISgpuExportOptionsFP::kSet_bExportSelected, _T("bExportSelected"), 0 , TYPE_BOOL,

	ISgpuExportOptionsFP::kGet_bExportParentIfChildExports, ISgpuExportOptionsFP::kSet_bExportParentIfChildExports, _T("bExportParentIfChildExports"), 0 , TYPE_BOOL,

	ISgpuExportOptionsFP::kGet_bExportChildIfParentExports, ISgpuExportOptionsFP::kSet_bExportChildIfParentExports, _T("bExportChildIfParentExports"), 0 , TYPE_BOOL,

	ISgpuExportOptionsFP::kGet_bSupportXref, ISgpuExportOptionsFP::kSet_bSupportXref, _T("bSupportXref"), 0 , TYPE_BOOL,

	ISgpuExportOptionsFP::kGet_bExportPivotAsSeparateNode, ISgpuExportOptionsFP::kSet_bExportPivotAsSeparateNode, _T("bExportPivotAsSeparateNode"), 0 , TYPE_BOOL,

	ISgpuExportOptionsFP::kGet_bMergeBasedOnMtls, ISgpuExportOptionsFP::kSet_bMergeBasedOnMtls, _T("bMergeBasedOnMtls"), 0 , TYPE_BOOL,

	ISgpuExportOptionsFP::kGet_bExportGeom, ISgpuExportOptionsFP::kSet_bExportGeom, _T("bExportGeom"), 0 , TYPE_BOOL,

	ISgpuExportOptionsFP::kGet_bCompressVertexAnim, ISgpuExportOptionsFP::kSet_bCompressVertexAnim, _T("bCompressVertexAnim"), 0 , TYPE_BOOL,

	ISgpuExportOptionsFP::kGet_nMaxNumTrianglesInMergedMesh, ISgpuExportOptionsFP::kSet_nMaxNumTrianglesInMergedMesh, _T("nMaxNumTrianglesInMergedMesh"), 0 , TYPE_INT,

	ISgpuExportOptionsFP::kGet_eExportIntent, ISgpuExportOptionsFP::kSet_eExportIntent, _T("eExportIntent"), 0 , TYPE_INT,

	ISgpuExportOptionsFP::kGet_fStartFrame, ISgpuExportOptionsFP::kSet_fStartFrame, _T("fStartFrame"), 0 , TYPE_FLOAT,

	ISgpuExportOptionsFP::kGet_fEndFrame, ISgpuExportOptionsFP::kSet_fEndFrame, _T("fEndFrame"), 0 , TYPE_FLOAT,

	ISgpuExportOptionsFP::kGet_fStepFrame, FP_NO_FUNCTION, _T("fStepFrame"), 0 , TYPE_FLOAT, //read only property

	ISgpuExportOptionsFP::kGet_fMaxEndFrame, FP_NO_FUNCTION, _T("fMaxEndFrame"), 0 , TYPE_FLOAT,

	ISgpuExportOptionsFP::kGet_fStartTime, FP_NO_FUNCTION, _T("fStartTime"), 0 , TYPE_FLOAT,

	ISgpuExportOptionsFP::kGet_fEndTime, FP_NO_FUNCTION,  _T("fEndTime"), 0 , TYPE_FLOAT,

	ISgpuExportOptionsFP::kGet_fStepTime, FP_NO_FUNCTION,  _T("fStepTime"), 0 , TYPE_FLOAT,

	ISgpuExportOptionsFP::kGet_fToleranceVertexAnim, ISgpuExportOptionsFP::kSet_fToleranceVertexAnim, _T("fToleranceVertexAnim"), 0 , TYPE_FLOAT,

	ISgpuExportOptionsFP::kGet_fRenderImageAspect, ISgpuExportOptionsFP::kSet_fRenderImageAspect, _T("fRenderImageAspect"), 0 , TYPE_FLOAT,

	ISgpuExportOptionsFP::kGet_fRenderApertureWidth, ISgpuExportOptionsFP::kSet_fRenderApertureWidth, _T("fRenderApertureWidth"), 0 , TYPE_FLOAT,

	end
	);


//functions  inherited from FPMixinInterface
FPInterfaceDesc* ISgpuExportOptionsFP::GetDescByID(Interface_ID id) 
{
	if (id == SGPU_EXPORT_OPTIONS_FP_INTERFACE_ID ) return &sgpuExportOptionsFP_Desc;
	return NULL;
}

FPInterfaceDesc* ISgpuExportOptionsFP::GetDesc()
{
	return GetDescByID( SGPU_EXPORT_OPTIONS_FP_INTERFACE_ID );
}


void * SgpuExportOptionsClassDesc::Create(BOOL loading) 
{ 
	if (!sInterfaceAdded) {
		AddInterface(&sgpuExportOptionsFP_Desc);
		sInterfaceAdded = TRUE;
	}
	return new SgpuExportOptions; 
}



