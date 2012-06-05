/*****************************************************************************
**  ExportDoc.cpp
**
**	The main document containing/coordinating all 
**	the book keeping of the 3ds max exporter
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "ExportDoc.hpp"
#include "MaxExportUtils.hpp"
#include "SgpuExport.hpp"
#include "InstanceMgr.hpp"
#include "MtlExporter.hpp"
#include "MeshExporter.hpp"
#include "HelperExporter.hpp"
#include "VertAnimExporter.hpp"
#include "SkinExporter.hpp"
#include "ModelExportIntent.hpp"
#include "VertAnimExportIntent.hpp"
#include "ParticleExportIntent.hpp"
#include "CameraAnimExportIntent.hpp"
#include "CameraAnimExporter.hpp"



#undef CreateFile
#undef DeleteFile

#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif 
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/mdlWriter.hpp"
#include <tchar.h>

#include "MaxCommon.hpp"
#include <iInstanceMgr.h> 
using namespace std;

namespace MaxExp
{




	const char* c_ExporterVersion = "1.1.0";


	ExportDoc::ExportDoc(  const MCHAR *fname , SgpuExportOptions &options ):
	m_Options(options),
		m_pExportIntent( NULL ),
		m_pMtlExporter( NULL ),
		m_pMeshExporter( NULL ),
		m_pMdlMatInfoTable( NULL ),
		m_pHelperExporter( NULL ),
		m_pVertAnimExporter( NULL ),
		m_pIgnoreExporter( NULL ),
		m_pErrorExporter( NULL ),
		m_pPhysiqueExporter( NULL ),
		m_pSkinModExporter( NULL ),
		m_pTrivialExporter( NULL ),
		m_pCameraAnimExporter( NULL )
	{
		m_InstanceMgr.reset(   new SgpuInstanceMgr( *this ) );
		m_pMtlExporter = new MtlExporter( *this );
		m_pMeshExporter = new  MeshExporter( *this );
		m_pHelperExporter = new HelperExporter( *this );
		m_pVertAnimExporter = new VertAnimExporter( *this );
		m_pIgnoreExporter = new IgnoreExporter( *this );
		m_pErrorExporter = new ErrorExporter( *this );
		m_pPhysiqueExporter = new PhysiqueExporter( *this );
		m_pSkinModExporter = new SkinModExporter( *this );
		m_pMdlMatInfoTable = new mdlMatInfoTable;
		m_pTrivialExporter = new TrivialExporter( *this );
		m_pCameraAnimExporter = new CameraAnimExporter( *this );


		//fix the appropriate export intent
		switch( m_Options.m_eExportIntent )
		{
		case eVertAnim:		
			m_pExportIntent = new VertAnimExportIntent( *this );
			break;
		case eParticle:		
			m_pExportIntent = new ParticleExportIntent( *this );
			break;
		case eCameraAnim:			
			m_pExportIntent = new CameraAnimExportIntent( *this );
			break;
		default:		
		case eModel:
			m_pExportIntent = new ModelExportIntent(  *this );
			break;
		}

		//fix the extension of the file
		//that we are exporting to
		if( NULL != fname)
		{
			string sext( SgpuExportClassDesc::theSgpuExportClassDesc.Ext(0) );

			switch( m_Options.m_eExportIntent )
			{
			case eVertAnim:
				//if bExportGeom == true
				//then export to a .gxb file else 
				//to a .gab file
				if( !( m_Options.m_bExportGeom ) )
				{
					sext = SgpuExportClassDesc::theSgpuExportClassDesc.Ext(1);
				}
				break;
			case eCameraAnim:
				sext = SgpuExportClassDesc::theSgpuExportClassDesc.Ext(2);
				break;
			default:
				break;

			} 

			sext = string(".") + sext;
			std::wstring wext = envString::UTF8ToWideChar( sext );
			std::wstring wfname;
			MbcsToUnicode( fname, wfname );
			m_FileName = ChangeExtension( wfname, wext);

		}


	}




ExportDoc::~ExportDoc()
{
	delete m_pExportIntent;
	delete m_pCameraAnimExporter;
	delete m_pTrivialExporter;
	delete m_pMdlMatInfoTable;
	delete m_pSkinModExporter;
	delete m_pPhysiqueExporter;
	delete m_pErrorExporter;
	delete m_pIgnoreExporter;
	delete m_pVertAnimExporter;
	delete m_pHelperExporter;
	delete m_pMeshExporter;
	delete m_pMtlExporter;
	m_InstanceMgr->Cleanup();
	m_WorldTransformsCache.clear();
	m_LocalTransformsCache.clear();
}

//=============================================================================
// Log  Pre-Export Statistics
// (ie: max version, user name, file names etc)
//=============================================================================

void ExportDoc::LogPreExportStatistics() const 
{

	//get user name
	std::wstring wUserName( _wgetenv( L"USERNAME"  ) );
	if( wUserName.empty() )
	{
		wUserName = _wgetenv( L"USER" );
	}


	//get unit
	float unitScale = 1.0f;
	int unitType = UNITS_CENTIMETERS;
	GetMasterUnitInfo( &unitType, &unitScale);
	const string & sUnitName( MaxUnit::GetUnitName( unitType ) );
	string sUnitScale;
	{
		stringstream ss;
		ss << unitScale;
		sUnitScale = ss.str();
	}

	//get max version
	string sMaxVersion;
	{
		stringstream ss;
		ss << MAX_VERSION_MAJOR;
		sMaxVersion = ss.str(); 
	}


	//log the stuff 
	EXPLOG.WriteStartElement( string( "PreExport Statistics" ) );
	if( !wUserName.empty() )
	{
		std::string utf8FileName = envString::WideCharToUTF8( m_FileName );
		std::string utf8UserName = envString::WideCharToUTF8( wUserName );
		EXPLOG.WriteElement( string("user"), utf8UserName );
		EXPLOG.WriteElement( string("outpufile"), utf8FileName );
		EXPLOG.WriteElement( string("unittype"), sUnitName );
		EXPLOG.WriteElement( string("unitscale"), sUnitScale );
		EXPLOG.WriteElement( string("maxversion"), sMaxVersion );
		//also log the options
		m_Options.Log( EXPLOG );
		EXPLOG.WriteEndElement();
	}

}


//=============================================================================
// Log  Post-Export Statistics
// ie: material details,  geometry details (faces, vertics) etc:
//=============================================================================

void ExportDoc::LogPostExportStatistics() const 
{

	//log the stuff 
	ExportLogger::WriteStartElementWrap postElementWrap("PostExport Statistics" );
	m_pMtlExporter->LogMaterials();

}

//=============================================================================
// Main public function which executes the export
//=============================================================================
void ExportDoc::Do(const list<INode *> &suggestedNodes )
{		
	if( m_FileName.empty())
	{ 
		EXPLOG.WriteError("empty filename passed in for export!");
		throw export_failure();
	}

	//Log 
	LogPreExportStatistics();

	//call the node exporter to do the export
	//node exporter will call other exporters as and when
	//apropriate data is encountered
	
	m_pExportIntent->Do(suggestedNodes);
	
	//Log 
	LogPostExportStatistics();

}

const char * ExportDoc::GetExporterVersion()
{
	return c_ExporterVersion;
}


	//=============================================================================
	// get the world transform of the current node,
	// cache it if it is not yet  cached
	//=============================================================================
	const Matrix3 & ExportDoc::GetWordTransform( INode * i_pCurNode )
	{
		map<INode*, Matrix3>::iterator mit = m_WorldTransformsCache.find( i_pCurNode );
		if( mit == m_WorldTransformsCache.end() )
		{	
			Matrix3 tm = i_pCurNode->GetNodeTM( static_cast<TimeValue>( m_Options.m_StartTime ) );
			map<INode*, Matrix3>::value_type v(i_pCurNode, tm);
			mit = m_WorldTransformsCache.insert(v).first;
		}
		return mit->second;
	}

	//=============================================================================
	// compute the local transform of the current node
	// local transform = (world transform of cur node)* (inverse of world transform of parent )
	// also  cache the local transform it is not yet cached
	//=============================================================================
	const Matrix3 & ExportDoc::GetLocalTranform( INode * i_pCurNode )
	{
		map<INode*, Matrix3>::iterator mit = m_LocalTransformsCache.find( i_pCurNode );
		if( mit == m_LocalTransformsCache.end() )
		{	
			Matrix3 tm = GetWordTransform( i_pCurNode );
			INode *parent = i_pCurNode->GetParentNode();
			if( NULL != parent )
			{
				const Matrix3 &tm_parent = GetWordTransform( parent );
				tm *= Inverse( tm_parent );
			}
			map<INode*, Matrix3>::value_type v( i_pCurNode, tm );
			mit = m_LocalTransformsCache.insert( v ).first;
		}
		return mit->second;
	}

	

} //namespace MaxExp