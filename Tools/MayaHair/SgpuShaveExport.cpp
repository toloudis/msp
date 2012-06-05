//
// Shave and a Haircut
// Copyright Joe Alter, Inc., 2003, all rights reserved.
// US Patent #6,720,962
//
#include <iostream>
#include <sstream>
#include <string>
#include <maya/MFStream.h>
#include <maya/MGlobal.h>
#include <maya/MIOStream.h>
#include <maya/MPxCommand.h>
#include <maya/MString.h>
#include <maya/MSyntax.h>
#include <maya/MTypes.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnTransform.h>
#include <maya/MDagPath.h>
#include <maya/MArgDatabase.h>
#include <maya/MTime.h>



#include "SgpuShaveExport.h"
#include "CommonUtils.h"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Graphics/mdl/mdlMatInfo.hpp"
#include "Graphics/mdl/mdlMatInfoTable.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlHairInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/mdl/mdlWriter.hpp"
#include "Graphics/vtx/vtxVertexFrame.hpp"
#include "Graphics/vtx/vtxStreamCompression.hpp"
#include "Graphics/vtx/vtxCompressionUtil.hpp"
#include "Graphics/vtx/vtxGeometryCacheWriter.hpp"

using namespace std;

//macros used in parseArgs
#define SGPU_MODEL_INTENT	"model"
#define SGPU_ANIM_INTENT	"anim"

namespace
{
	const chDefs::Name c_ACHR = chDefs::MakeName('A', 'C', 'H', 'R');
}
//------------------------------------------------------------------------
//	Internal book keeping struct used to schedule a hair export for a 
//	shave & haircut maya node
//------------------------------------------------------------------------
struct HairExportSubmission
{
	MDagPath							m_DagPath;
	std::string							m_Name;
	int									m_nHairVertices;
	bool								m_bWriteNormals;
	shared_ptr<vtxStreamCompression>	m_CompressStream;

	HairExportSubmission():
		m_nHairVertices(0),
		m_bWriteNormals(false)
		{}

	//is the submission a valid hair export
	bool Validate () const
	{
		return m_Name.length() > 0 && m_nHairVertices > 0;
	}
	//make a compression stream
	void InitializeCompressStream(  float i_fTolerance, vtxGeometryCacheWriter& io_GeometryCache )
	{
		assert( Validate() );
		if (i_fTolerance < 0)
		{
			m_CompressStream =  vtxCompressionUtil::CreateLosslessCompressStream(m_Name, io_GeometryCache);
		}
		else
		{
			m_CompressStream = vtxCompressionUtil::CreateToleranceCompressStream(
				i_fTolerance, 
				m_Name, 
				io_GeometryCache);
		}
	}
};


//------------------------------------------------------------------------
// structure used to capture the logic of an animation range
//------------------------------------------------------------------------
struct AnimRangeInfo
{
	float m_StartFrame;
	float m_EndFrame;
	float m_StepFrame;
	AnimRangeInfo():
		m_StartFrame(0.0f),
		m_EndFrame(0.0f),
		m_StepFrame(1.0f)
	{}
	//compute the number of frames from the maxframe and minimum frame
	static int ComputeNumFrames( float i_MaxFrame, float i_MinFrame, float i_StepFrame )
	{
			float fNSteps = ceil( ( i_MaxFrame - i_MinFrame ) / i_StepFrame );
			if( fNSteps == floor( ( i_MaxFrame - i_MinFrame ) / i_StepFrame ) )
			{
				fNSteps += 1;
			}
			return static_cast< int > ( fNSteps );
	}
	//Get the number of frames in the range
	int GetNumFrames()const
	{
		int retVal =0;
		if( m_StartFrame > m_EndFrame )
		{
			assert( m_StepFrame < 0.0f );
			assert( !sgpuMaya::EpsilonEqual( m_StepFrame, 0.0f ) );
			retVal = ComputeNumFrames( m_StartFrame, m_EndFrame, m_StepFrame );
		} else
		{	
			assert( m_EndFrame >= m_StartFrame );
			retVal = ComputeNumFrames( m_EndFrame, m_StartFrame, m_StepFrame );
		}
		return retVal;
	}
	//get the i'th frame
	float GetIthFrame( int i_FrameNum ) const
	{
		assert( i_FrameNum <  GetNumFrames() );
		//If m_StartFrame > m_EndFrame, m_stepFrame < 0
		assert( ( m_StartFrame <= m_EndFrame ) || m_StepFrame < 0 );
		return m_StartFrame + m_StepFrame * i_FrameNum;
		
	}
};


//------------------------------------------------------------------------
// structure used to capture the arguments passed in though the MPxCommand
//------------------------------------------------------------------------
struct ShaveExportArgs
{
	ShaveExportArgs():
		m_bCompress( true ),
		m_fTolerance( -1.0f ),
		m_Intent( SGPU_MODEL_INTENT )
		{}
		AnimRangeInfo	m_ARange;
		bool			m_bCompress;
		float			m_fTolerance;
		MString		m_OutFilepath;
		MString		m_Intent;
};


class ObjWriter
{
public:
	ObjWriter( shaveAPI::HairInfo &hInfo, const std::vector< int > &i_Idxs ):
		m_HairInfo( hInfo ),
		m_IdxVec( i_Idxs )
		{}

friend
	std::ostream &operator << ( std::ostream &os, const ObjWriter & i_Obj); 

	 shaveAPI::HairInfo &m_HairInfo;
	 const std::vector< int > & m_IdxVec;
};

std::ostream &operator << ( std::ostream &os, const ObjWriter & i_Obj)
{

	int istrand;
	os << std::endl;
	for ( istrand = 0;  (istrand < i_Obj.m_HairInfo.numHairs); istrand++)
	{
		int iStartVertex = i_Obj.m_HairInfo.hairStartIndices[istrand];
		int iEndVertex = i_Obj.m_HairInfo.hairEndIndices[istrand];
		assert(iStartVertex < iEndVertex );
		int iVertIdx = i_Obj.m_HairInfo.hairVertices[iStartVertex];
		assert( iVertIdx < i_Obj.m_HairInfo.numVertices );
		const shaveAPI::Vertex &shaveVertex = i_Obj.m_HairInfo.vertices[ iVertIdx ];	
		os << "v " << shaveVertex.x << " " << shaveVertex.y << " " <<  shaveVertex.z << std::endl;
	}
	for ( istrand = 0;  (istrand < i_Obj.m_HairInfo.numHairs); istrand++)
	{
		int iStartVertex = i_Obj.m_HairInfo.hairStartIndices[istrand];
		int iEndVertex = i_Obj.m_HairInfo.hairEndIndices[istrand];
		assert(iStartVertex < iEndVertex );
		int iVertIdx = i_Obj.m_HairInfo.hairVertices[iStartVertex];
		assert( iVertIdx < i_Obj.m_HairInfo.numVertices );
		const shaveAPI::Vertex &shaveNormal = i_Obj.m_HairInfo.surfaceNormals[ iVertIdx ];
		//os << std::endl;
		os << "vn " << shaveNormal.x << " " << shaveNormal.y << " " <<  shaveNormal.z << std::endl;
	}
	const std::vector< int > &indices = i_Obj.m_IdxVec;
	int nFaces = indices.size()/3;
	for( int i=0 ; i < indices.size() ; i += 3)
	{
		int vIdx0= indices[i] + 1;
		int vIdx1 = indices[i+1] + 1;
		int vIdx2 = indices[i+2] + 1;
		assert( vIdx0 <=  i_Obj.m_HairInfo.numVertices );
		assert( vIdx1 <=  i_Obj.m_HairInfo.numVertices );
		assert( vIdx2 <=  i_Obj.m_HairInfo.numVertices );
		os << "f " << vIdx0 << "//" << vIdx0  << " " << vIdx1 << "//" << vIdx1 << " " << vIdx2 << "//" << vIdx2 <<  std::endl; 
	}
	return os;
}

//------------------------------------------------------------------------
// structure with a smart destructor which restores
// the current time in Maya when it goes out of scope
//------------------------------------------------------------------------
struct TimeRestorer
{

	TimeRestorer():
		m_OrigTime( sgpuMaya::GetCurrentTime() )
		{
		}
	~TimeRestorer()
	{
		sgpuMaya::SetCurrentTime( m_OrigTime );
	}
	MTime m_OrigTime;
};


//------------------------------------------------------------------------
//If the argument is a transform node, return the shape node
//If the argument is a shape node, then just return it.
//------------------------------------------------------------------------

MObject getPossibleShapeNode(  MDagPath & dagPath )
{
	MStatus status;
	MObject dependNode;
	dependNode = dagPath.node(&status);
	sgpuMaya::MAssert( status, "sgpuShaveExport::doIt, cannot get dagNode" );
	MObject returnObj = dependNode;
	MFnDependencyNode fnDepends(dependNode, &status);
	sgpuMaya::MAssert( status, "sgpuShaveExport::doIt, object should be a dependency node!" );
	MTypeId tid = fnDepends.typeId( &status );
	sgpuMaya::MAssert( status, "sgpuShaveExport::doIt, errror in getting typeId, got %u", tid.id() );
	MString tName = fnDepends.typeName( &status );
	sgpuMaya::MAssert( status, "sgpuShaveExport::doIt, cannot get typeName, got %s", tName.asChar() );
	const char *pzTypeName = tName.asChar();
	MString sNameOfDependNode = fnDepends.name( &status );
	const char *pzName =sNameOfDependNode.asChar();
	sgpuMaya::MAssert( status, "cannot get name of depend node" );			
	MFnTransform fnTransform;
	bool bTransform = fnTransform.hasObj( dependNode );
	if( bTransform )
	{
		status = dagPath.extendToShape( );
		sgpuMaya::MAssert( status, "cannot get dag path of shape node for %s", pzName );
		MObject dagNodeObj = dagPath.node(&status);
		sgpuMaya::MAssert( status, "cannot get object of the shape %s", pzName );
		status = fnDepends.setObject( dagNodeObj );
		sgpuMaya::MAssert( status, "object %s is not a dependency Node", pzName );
		tid = fnDepends.typeId( &status );
		sgpuMaya::MAssert( status, "cannot get the type id for shape node %s", pzName );
		tName = fnDepends.typeName( &status );
		sgpuMaya::MAssert( status, " cannot get typename for %s" );
		pzTypeName = tName.asChar();
		returnObj = dagNodeObj;
	}
	return returnObj;
}

//------------------------------------------------------------------------
//If the argument is a transform node, return the shape node
//If the argument is a shape node, then just return it.
//------------------------------------------------------------------------
MObject getPossibleShapeNode( MItSelectionList & iter )
{
	MStatus status;
	MObject dependNode;
	MDagPath dagPath;
	status = iter.getDagPath( dagPath );
	sgpuMaya::MAssert( status, "sgpuShaveExport::doIt, cannot get dagPath" );
	MObject returnObj = getPossibleShapeNode( dagPath );
	return returnObj;
}


//------------------------------------------------------------------------
//	Argument flags
//------------------------------------------------------------------------
const char * sgpuShaveExport::m_HelpFlag = "-h", *sgpuShaveExport::m_HelpFlagLong = "-help";
const char * sgpuShaveExport::m_HelpText = "Export one or more selected hair nodes\n"
"-o[-output]		<full file path>\n"
"\tIf the <full file path> has a '.txt' extension then a text file is written, if the intent is model\n"
"\tIf the <full file path> has a '.gxb' extension then a gxb or a .gab file is written, based on the intent\n"
"-g[-geom]			exports the geometry of the hair nodes\n"
"-a[-anim]			exports the animation of the hair nodes\n"
"\tNote that the flags '-geom' and '-anim' are exclusive\n"
"\tIf both '-geom' and '-anim' are specified then -anim is chosen\n"
"-s[-start]			specify the start frame for the animation\n" 
"-e[-end]			specify the end frame for the animation\n"
"\tNote that '-start' and '-end' work only when '-anim' is specified\n"
"-c[-compress] compress the animation export, works only with -anim\n"
"-t[-tolerance] tolerance for vertex compression\n"
"\tIf -tolerance is given without -compress, the -compress is implicitly assumed\n"
"\tand the animation is compressed\n"
"\tIf -tolerance is omitted and -compress is given, a default tolerance of -1.0 is\n"
"\tassumed which stands for lossless compression"

;
const char * sgpuShaveExport::m_OutFilename = "-o", *sgpuShaveExport::m_OutFilenameLong = "-output";
const char *sgpuShaveExport::m_ModelIntent = "-g", *sgpuShaveExport::m_ModelIntentLong = "-geom";
const char *sgpuShaveExport::m_AnimIntent = "-a", *sgpuShaveExport::m_AnimIntentLong="-anim";
const char *sgpuShaveExport::m_StartFrame = "-s", *sgpuShaveExport::m_StartFrameLong="-startFrame";
const char *sgpuShaveExport::m_EndFrame = "-e", *sgpuShaveExport::m_EndFrameLong="-endFrame";
const char *sgpuShaveExport::m_Compress = "-c", *sgpuShaveExport::m_CompressLong = "-compress";
const char *sgpuShaveExport::m_Tolerance = "-t", *sgpuShaveExport::m_ToleranceLong = "-tolerance";


void* sgpuShaveExport::creator()
{
	return new sgpuShaveExport();
}


sgpuShaveExport::sgpuShaveExport()
{
}


sgpuShaveExport::~sgpuShaveExport()
{
}


bool sgpuShaveExport::isUndoable() const
{
	return false;
}

MSyntax sgpuShaveExport::createSyntax()
{
	MSyntax   syntax;

	syntax.enableQuery(false);
	syntax.enableEdit(false);
	syntax.addFlag( m_OutFilename, m_OutFilenameLong, MSyntax::kString  );
	syntax.addFlag( m_HelpFlag, m_HelpFlagLong );
	syntax.addFlag( m_ModelIntent, m_ModelIntentLong );	
	syntax.addFlag( m_AnimIntent, m_AnimIntentLong );	
	syntax.addFlag( m_StartFrame, m_StartFrameLong , MSyntax::kDouble );	
	syntax.addFlag( m_EndFrame, m_EndFrameLong , MSyntax::kDouble );
	syntax.addFlag( m_Compress, m_CompressLong );	
	syntax.addFlag( m_Tolerance, m_ToleranceLong, MSyntax::kDouble );	
	return syntax;
}

//------------------------------------------------------------------------
//		higher level work routine
//------------------------------------------------------------------------
MStatus sgpuShaveExport::doIt(const MArgList& argList)
{
	MStatus  status;
	ShaveExportArgs exportArgs;
	try
	{
		status = parseArgs( argList , exportArgs );
		if( status != MStatus::kSuccess )
		{
			return status;
		}
		if( exportArgs.m_Intent == SGPU_ANIM_INTENT )
		{
			status = doItCore_Anim( exportArgs );
		} else 
		{
			status = doItCore_Geom( exportArgs  );
		}
	}
	catch( std::runtime_error &s_error )
	{
		status = MStatus::kFailure;
		std::stringstream ss;
		ss << "shave export failed:" << s_error.what();
		MGlobal::displayInfo( ss.str().c_str() );
	}
	catch( MStatus &e_status )
	{
		status = e_status;
		std::stringstream ss;
		ss << "shave export failed: " << status.errorString().asChar();
		MGlobal::displayInfo( ss.str().c_str() );
	}
	return status;
}

//------------------------------------------------------------------------
//		Export selected hair shape nodes to a .txt file or to a .gxb file
//------------------------------------------------------------------------
MStatus sgpuShaveExport::doItCore_Geom( const ShaveExportArgs &i_ExportArgs  )
{
	MStatus status;
	{
		std::stringstream ss;
		ss << "Exporting selected shaveHairShapes to " << i_ExportArgs.m_OutFilepath.asChar() << endl;
		MGlobal::displayInfo( ss.str().c_str() );
	}
	//std::string form of i_OutFilepath
	string sOutFilepath( i_ExportArgs.m_OutFilepath.asChar() );

	//make null fileWrappers for the txt file and the gxb file
	shared_ptr< sgpuMaya::FileWrapper< ofstream> > txtFileWrapper;
	shared_ptr< sgpuMaya::SgpuFileWriterLifeTimeKeeper > gxbFileWrapper;

	//sExt should govern how we should export
	//to a .txt file or to a .gxb file
	string sExt = sgpuMaya::GetFileExt( sOutFilepath );

	//Create common hair material structures for use
	//if we need to write to a gxb file
	mdlMatInfoTable material_table;
	shared_ptr<mdlMatInfo> material_info(new mdlMatInfo());
	if( !_stricmp( sExt.c_str(), ".gxb" ) )
	{
		//construct a default hair material
		material_info->m_Info.SetMaterialName( "Hair_Material" );
		shared_ptr<effShaderParams> hair_params(new effShaderParams());
		hair_params->SetShaderName(itString("Hair_SH.fx"));
		material_info->m_Info.SetShaderParams(hair_params);
		material_table[material_info->m_Info.GetMaterialName()] = material_info;
	}
	//Create common sceneroot node
	//if we need to write to a gxb file
	shared_ptr< mdlNodeInfo > sgpuRootNode( new mdlNodeInfo );
	sgpuRootNode->m_NodeName = "SceneRoot";


	MSelectionList list;
	status = MGlobal::getActiveSelectionList( list );
	sgpuMaya::MCheck( status, "sgpuShaveExport::doIt, cannot get sctive selection set!" );
	MItSelectionList iter( list, MFn::kDependencyNode, &status );
	sgpuMaya::MCheck( status, "sgpuShaveExport::doIt, cannot get itSelectionList!");	

	//go through selected hair nodes
	for ( ; !iter.isDone(); iter.next() ) 
	{
		MObjectArray shaveShapeNodes;			
		MObject dependNode;
		status = iter.getDependNode(dependNode);
		sgpuMaya::MAssert( status, "sgpuShaveExport::doItCore, cannot get dependNode" );
		MFnDependencyNode fnDepends( dependNode );
		MString dependNodeName = fnDepends.name();
		//get the shape node
		MObject shaveShapeNode = getPossibleShapeNode( iter );
		status = shaveShapeNodes.append( shaveShapeNode );
		sgpuMaya::MAssert( status, "sgpuShaveExport::doItCore, getting shave shapeNode %s", dependNodeName.asChar()  );
		shaveAPI::HairInfo hairInfo;	
		//export hair info using the shave API
		status = shaveAPI::exportHair( shaveShapeNodes, &hairInfo);
		if( status == MStatus::kNotFound )
		{
			std::stringstream ss;
			ss << "selected node " << dependNodeName.asChar() << " not a shaveHair Node";
			MGlobal::displayInfo( ss.str().c_str() );
			continue;
		}else if (!status )
		{
			MGlobal::displayInfo(
				"sgpuShaveExport got an unexpected error. in exportHair");
			return status;
		}
		//If the extension of i_OutFilepath was a .txt file
		//then we are usibg a .txt exporter
		if( !_stricmp( sExt.c_str(), ".txt" ) )
		{		
			//If the txtFileWrapper is null, create one
			if( !txtFileWrapper )
			{
				txtFileWrapper.reset( new sgpuMaya::FileWrapper< ofstream > ( i_ExportArgs.m_OutFilepath.asChar() ) );
				if( !txtFileWrapper->m_File.is_open() )
				{
					status = MStatus::kNotFound;
					std::stringstream ss;
					ss << "cannot open file " << i_ExportArgs.m_OutFilepath.asChar() << " for writing";
					MGlobal::displayInfo( ss.str().c_str() );
					return status;
				}
			}
			//DBG_ASSERT( (txtFileWrapper->m_File.is_open() ), "File should be open for writing: " << i_ExportArgs.m_OutFilepath.asChar() );
			ofstream & os = txtFileWrapper->m_File;
			os << "Name: " << dependNodeName.asChar() << endl;
			//export the hair info to the txt file
			exportHairInfo( &hairInfo, false, os );
			os << "End:" << endl;
			//if extension of the i_OutFilepath is a .gxb file
		} else if ( !_stricmp( sExt.c_str(), ".gxb" ) )
		{
			//create a gxbfilewrapper, if it doesnt exist
			if( gxbFileWrapper == NULL )
			{
				gxbFileWrapper.reset( new sgpuMaya::SgpuFileWriterLifeTimeKeeper ( sOutFilepath ) );
				if( NULL == gxbFileWrapper->m_pFile )
				{
					status = MStatus::kNotFound;
					std::stringstream ss;
					ss << "not yet implemented " << i_ExportArgs.m_OutFilepath.asChar() << "\n";
					MGlobal::displayInfo( ss.str().c_str() );
					return status;
				}
			}

			//export hair to an mdlHairInfo struct
			shared_ptr< mdlHairInfo > sgpuHairInfo( new mdlHairInfo );
			exportHairInfo( &hairInfo , false, sgpuHairInfo );
			//put name and material to the hair info
			sgpuHairInfo->m_HairName = string( dependNodeName.asChar() );
			sgpuHairInfo->m_Material = material_info;


			//make a child node which houses the hair
			shared_ptr< mdlNodeInfo > sgpuHairNode( new mdlNodeInfo );
			sgpuRootNode->m_Children.push_back( sgpuHairNode );
			sgpuHairNode->m_NodeName = dependNodeName.asChar(); 
			sgpuHairNode->m_HairInfo = sgpuHairInfo;
			//write the mdl hierarchy to the gxb file
			chBinWriter &writer = *gxbFileWrapper->m_pWriter;
			mdlWriter::WriteHierarchicalModel( writer ,
				sgpuRootNode,
				material_table );

		}
	}
	return status;
}


//------------------------------------------------------------------------
//		Export compressed vertex anims to a .gab file
//------------------------------------------------------------------------
MStatus sgpuShaveExport::doItCore_Anim( const ShaveExportArgs &i_ExportArgs )
{
	MStatus status;
	std::deque< HairExportSubmission > hairNodesToExport;

	//std::string form of i_OutFilepath
	string sOutFilepath( i_ExportArgs.m_OutFilepath.asChar() );
	string sNewExt(".gab");
	sOutFilepath = sgpuMaya::ChangeExtension< std::string > ( sOutFilepath, sNewExt );
	{
		std::stringstream ss;
		ss << "Exporting compressed vertex anims of selected shaveHairShapes to " << sOutFilepath.c_str() << endl;
		ss << "startFrame : " << i_ExportArgs.m_ARange.m_StartFrame << " endFrame : " << i_ExportArgs.m_ARange.m_EndFrame << endl;
		MGlobal::displayInfo( ss.str().c_str() );
	}
	//make null fileWrappers for the txt file and the gxb file
	shared_ptr< sgpuMaya::FileWrapper< ofstream> > txtFileWrapper;
	shared_ptr< sgpuMaya::SgpuFileWriterLifeTimeKeeper > gxbFileWrapper;



	MSelectionList list;
	status = MGlobal::getActiveSelectionList( list );
	sgpuMaya::MCheck( status, "sgpuShaveExport::doIt, cannot get sctive selection set!" );
	MItSelectionList iter( list, MFn::kDependencyNode, &status );
	sgpuMaya::MCheck( status, "sgpuShaveExport::doIt, cannot get itSelectionList!");	

	//go through selected hair nodes
	//collect them
	for ( ; !iter.isDone(); iter.next() ) 
	{
		MObjectArray shaveShapeNodes;			
		MObject dependNode;
		status = iter.getDependNode(dependNode);
		sgpuMaya::MAssert( status, "sgpuShaveExport::doItCore, cannot get dependNode" );
		MFnDependencyNode fnDepends( dependNode );
		MString dependNodeName = fnDepends.name();
		const char *pzName = dependNodeName.asChar();
		//get the shape node
		MObject shaveShapeNode = getPossibleShapeNode( iter );
		status = shaveShapeNodes.append( shaveShapeNode );
		sgpuMaya::MAssert( status, "sgpuShaveExport::doItCore, getting shave shapeNode %s", dependNodeName.asChar()  );
		shaveAPI::HairInfo hairInfo;	
		//export hair info using the shave API
		status = shaveAPI::exportHair( shaveShapeNodes, &hairInfo);
		if( status == MStatus::kNotFound )
		{
			std::stringstream ss;
			ss << "selected node " << dependNodeName.asChar() << " not a shaveHair Node";
			MGlobal::displayInfo( ss.str().c_str() );
			continue;
		}else if (!status )
		{
			MGlobal::displayInfo(
				"sgpuShaveExport got an unexpected error. in exportHair");
			return status;
		}

		MDagPath dagPathOfDependNode;
		status = iter.getDagPath( dagPathOfDependNode, dependNode );
		sgpuMaya::MAssert( status, "cannot get dag the path of transform node %s", pzName );
		//schedule the hair export
		HairExportSubmission hairSubmission;
		hairSubmission.m_DagPath = dagPathOfDependNode;
		hairSubmission.m_nHairVertices = hairInfo.numHairVertices;
		hairSubmission.m_Name = string( dependNodeName.asChar() );
		hairNodesToExport.push_back( hairSubmission );
	}
	//if there is no hair to export...
	if( hairNodesToExport.size() <= 0)
	{
		std::stringstream ss;
		ss << "no shave nodes to export";
		MGlobal::displayInfo( ss.str().c_str() );
		return MStatus::kEndOfFile;
	}

	//sExt should be ".gab"
	string sExt = sgpuMaya::GetFileExt( sOutFilepath );
	assert ( !_stricmp( sExt.c_str(), ".gab" ) );

	//Initial rituals...
	//Open the  gxb file fr writing,
	//initialize the chunk writer
	sgpuMaya::SgpuFileWriterLifeTimeKeeper fileWrapper( sOutFilepath );
	fileWrapper.m_pWriter->WriteChunkHeader(c_ACHR, 0, true);	// Character Animation
	// Write in the frame rate in small chunk at top
	sgpuMaya::WriteCurrentFrameRate( *fileWrapper.m_pWriter );	
	// Write the beginFrame
	sgpuMaya::WriteBeginFrame( *fileWrapper.m_pWriter, i_ExportArgs.m_ARange.m_StartFrame );

	//construct the geometry caache for vertex compression streaming
	shared_ptr<vtxGeometryCacheWriter> geom_cache;
	if ( i_ExportArgs.m_bCompress )
		geom_cache.reset(new vtxGeometryCacheWriter( *fileWrapper.m_pWriter, *fileWrapper.m_pFile ) );
	
	std::deque< HairExportSubmission >::iterator hit;
	for( hit = hairNodesToExport.begin(); hit != hairNodesToExport.end(); ++hit )
	{
		HairExportSubmission &hs = *hit;
		if( i_ExportArgs.m_bCompress )
		{
			//open a compression stream for each hair that we export
			hs.InitializeCompressStream( i_ExportArgs.m_fTolerance, *geom_cache );
		}
	}

	// Open the geometry cache now to receive the frames as we
	// compress them in the streams.
	if ( i_ExportArgs.m_bCompress )
		geom_cache->OpenCache();

	//restore the current time in Maya when this goes out of scope
	TimeRestorer trestorer; 
	int nFrames = i_ExportArgs.m_ARange.GetNumFrames();
	for( int i=0; i < nFrames; ++i )
	{
		//set the current frame
		float curFrame = i_ExportArgs.m_ARange.GetIthFrame( i );
		MTime time(curFrame, MTime::uiUnit());
		sgpuMaya::SetCurrentTime(time);
		//for each hair node that is scheduled to export
		for( hit = hairNodesToExport.begin(); hit != hairNodesToExport.end(); ++hit )
		{
			MObjectArray shaveShapeNodes;
			HairExportSubmission &he = *hit;
			shaveAPI::HairInfo hairInfo;
			//get the shape node
			MObject shaveShapeNode = getPossibleShapeNode( he.m_DagPath );
			status = shaveShapeNodes.append( shaveShapeNode );
			MFnDependencyNode fnDepends( shaveShapeNode );
			MString shaveShapeNodeName = fnDepends.name();
			const char *pzShaveShapeNode = shaveShapeNodeName.asChar();
			//export hair info using the shave API
			status = shaveAPI::exportHair( shaveShapeNodes, &hairInfo);
			if( !status )
			{
				std::stringstream ss;
				ss << "sgpuShaveExport got an unexpected error. in exportHair, while exporting " << pzShaveShapeNode << " at frame " << i << endl;
				MGlobal::displayInfo( ss.str().c_str() );
				return status;
			}
			if( i_ExportArgs.m_bCompress )
			{

				vtxVertexFrame vertex_frame;

				// Extract vertex info from ShaveAPI::HairInfo flor this frame
				vertex_frame.m_Positions.resize( he.m_nHairVertices );
				std::vector<maPoint3d>::iterator vit = vertex_frame.m_Positions.begin();
				int iStrand =0;
				int nVerticesPerStrand =  hairInfo.hairEndIndices[ iStrand ] - hairInfo.hairStartIndices[ iStrand ];
				assert( he.m_nHairVertices == hairInfo.numHairs * nVerticesPerStrand );
				for(iStrand = 0; iStrand < hairInfo.numHairs; ++iStrand ) 
				{
					for (
						int j = hairInfo.hairStartIndices[iStrand];
						j < hairInfo.hairEndIndices[iStrand];
						j++
						)
						{
							int vert = hairInfo.hairVertices[j];
							vit->Set(
								hairInfo.vertices[ vert ].x,
								hairInfo.vertices[ vert ].y,
								hairInfo.vertices[ vert ].z
								);
							++vit;
						}
				}
				he.m_CompressStream->SubmitFrame( curFrame-i_ExportArgs.m_ARange.m_StartFrame, vertex_frame);
			}
		}
	}
	//All frames has been exported
	if ( i_ExportArgs.m_bCompress )
	{
		//finish each com;pression stream
		for( hit = hairNodesToExport.begin(); hit != hairNodesToExport.end(); ++hit )
		{
			HairExportSubmission &hs = *hit;
			hs.m_CompressStream->Finish( );
		}
		//close the cache
		geom_cache->CloseCache();
		//write each compression stream
		for( hit = hairNodesToExport.begin(); hit != hairNodesToExport.end(); ++hit )
		{
			HairExportSubmission &hs = *hit;
			hs.m_CompressStream->WriteCompressedAnimationData( *fileWrapper.m_pWriter  );
		}
	}

	//Finish rituals
	// Move the file cursor to the end of the file to continue writing
	fileWrapper.m_pFile->SetFilePos(0, fsFileStream::e_End);
	fileWrapper.m_pWriter->FinishChunk(); //C_ACHR chunk
	sgpuMaya::WriteExporterVersionStamp( *fileWrapper.m_pWriter );

	return status;
}

//------------------------------------------------------------------------
//		Parse the arguments into a 'ShaveExportArgs' structure
//------------------------------------------------------------------------
MStatus sgpuShaveExport::parseArgs( const MArgList& argList, ShaveExportArgs &o_ExportArgs )
{
	MStatus  status;
	MArgDatabase argData( syntax(), argList, &status );
	sgpuMaya::MAssert( status, "getting args" );
	if ( argData.isFlagSet( m_HelpFlag )  || argData.isFlagSet( m_HelpFlagLong ) )
	{
		setResult( m_HelpText );				
		return status;
	} else if ( argData.isFlagSet( m_AnimIntent ) || argData.isFlagSet( m_AnimIntentLong ))
	{
		o_ExportArgs.m_Intent = SGPU_ANIM_INTENT;
		double startTime, endTime;
		sgpuMaya::GetTimelineRangeFromAnimControl( startTime, endTime );
		o_ExportArgs.m_ARange.m_StartFrame = static_cast< float > ( startTime );
		o_ExportArgs.m_ARange.m_EndFrame = static_cast< float > ( endTime );
		if( argData.isFlagSet( m_StartFrame ) || argData.isFlagSet( m_StartFrameLong ) )
		{
			status = argData.getFlagArgument(m_StartFrame, 0, startTime );
			o_ExportArgs.m_ARange.m_StartFrame = static_cast<float> ( startTime );
		}
		if( argData.isFlagSet( m_EndFrame ) || argData.isFlagSet( m_EndFrameLong ) )
		{
			status = argData.getFlagArgument(m_EndFrame, 0, endTime );
			o_ExportArgs.m_ARange.m_EndFrame = static_cast<float> ( endTime );
		}

		 
		if( argData.isFlagSet( m_Compress ) || argData.isFlagSet( m_CompressLong ) )
		{
			o_ExportArgs.m_bCompress = true;
		}		
		if( argData.isFlagSet( m_Tolerance ) || argData.isFlagSet( m_ToleranceLong ) )
		{
			o_ExportArgs.m_bCompress = true;
			double toleranceDummy;
			status = argData.getFlagArgument(m_Tolerance, 0, toleranceDummy );
			o_ExportArgs.m_fTolerance = static_cast<float> ( toleranceDummy );
		}

		if( argData.isFlagSet( m_OutFilename ) || argData.isFlagSet( m_OutFilenameLong ) )
		{
			argData.getFlagArgument(m_OutFilename, 0, o_ExportArgs.m_OutFilepath );

		} else
		{
			std::stringstream ss;
			ss << "Hair exports needs an output file name specified, see usage " <<  endl;
			MGlobal::displayInfo( ss.str().c_str() );
			setResult( m_HelpText );			
			status = MStatus::kInvalidParameter;
		}
	} else /* if ( argData.isFlagSet( m_ModelIntent ) || argData.isFlagSet( m_ModelIntentLong ))*/
	{
		//by default model intent is assumed
		o_ExportArgs.m_Intent =  SGPU_MODEL_INTENT;
		MString outFilename;
		if( argData.isFlagSet( m_OutFilename ) || argData.isFlagSet( m_OutFilenameLong ) )
		{
			argData.getFlagArgument(m_OutFilename, 0, o_ExportArgs.m_OutFilepath );
			
		} else
		{
			std::stringstream ss;
			ss << "Hair exports needs an output file name specified, see usage " <<  endl;
			MGlobal::displayInfo( ss.str().c_str() );
			setResult( m_HelpText );				
			status = MStatus::kInvalidParameter;
		}
	}
	return status;
}
//------------------------------------------------------------------------
//		Export shaveAPI::HairInfo to a txt file
//------------------------------------------------------------------------

void sgpuShaveExport::exportHairInfo(
									 shaveAPI::HairInfo* hairInfo, 
									 bool instances,
									 ostream &io_s
									 ) const
{
	MString  strandName = (instances ? "face" : "strand");
	io_s << "numHairs: " << hairInfo->numHairs << endl;
	io_s << "numVertices: " << hairInfo->numVertices << endl;
	io_s << "numHairVertices: " << hairInfo->numHairVertices << endl;
	int strand = 0;
	io_s << "numVerticesPerStrand: " << hairInfo->hairEndIndices[strand] - hairInfo->hairStartIndices[strand] << endl;
	int i;

	for (strand = 0;  (strand < hairInfo->numHairs); strand++)
	{
		io_s << strand << " " << "rootRadii: " << 
			hairInfo->rootRadii[strand] << endl;

		io_s << strand << " " << "tipRadii: " << 
			hairInfo->tipRadii[strand] << endl;

		io_s << strand << " " << "rootColor: " << 
			hairInfo->rootColors[strand].r << " " <<
			hairInfo->rootColors[strand].g << " " <<
			hairInfo->rootColors[strand].b << " " << endl;
		io_s << strand << " " << "tipColor: " <<
			hairInfo->tipColors[strand].r << " " << 
			hairInfo->tipColors[strand].g << " " <<
			hairInfo->tipColors[strand].b << endl;
		io_s << strand << " " << "surfaceNormal: " <<			
			hairInfo->surfaceNormals[strand].x << " " <<
			hairInfo->surfaceNormals[strand].y << " " <<
			hairInfo->surfaceNormals[strand].z << " " << endl;

		io_s << strand << " " << "opacity: " << 
			hairInfo->opacities[strand] << endl;


		io_s << strand << " " << "specular: " << 
			hairInfo->speculars[strand] << endl;


		io_s << strand << " " << "gloss: " << 
			hairInfo->glosses[strand] << endl;


		io_s << strand << " " << "ambDiff: " << 
			hairInfo->ambDiffs[strand] << endl;

		for (i = hairInfo->hairStartIndices[strand];
			i < hairInfo->hairEndIndices[strand];
			i++)
		{
			int vert = hairInfo->hairVertices[i];
			io_s << strand << " ";
			io_s << (i-hairInfo->hairStartIndices[strand]) << " ";
			io_s << "vertex: " << hairInfo->vertices[vert].x << " " <<
				hairInfo->vertices[vert].y << " " <<
				hairInfo->vertices[vert].z << " " << endl;
			/*io_s << strand << " ";
			io_s << (i-hairInfo->hairStartIndices[strand]) << " ";
			io_s << "velocity: " << hairInfo->velocities[vert].x << " " <<
			hairInfo->velocities[vert].y << " " <<
			hairInfo->velocities[vert].z << " " << endl;
			*/
			io_s << strand << " ";
			io_s << (i-hairInfo->hairStartIndices[strand]) << " ";
			io_s << "uvw: " << hairInfo->uvws[vert].x << " " <<
			hairInfo->uvws[vert].y << " " <<
			hairInfo->uvws[vert].z << " " << endl;
			
		}
	}
}

//------------------------------------------------------------------------
//		Export shaveAPI::HairInfo to a mdlHairInfo structure
//------------------------------------------------------------------------
void sgpuShaveExport::exportHairInfo(
									 const shaveAPI::HairInfo* i_HairInfo,
									 bool i_bInstances,
									 shared_ptr< mdlHairInfo > &o_HairInfo
									 ) const
{
	MString  strandName = (i_bInstances ? "face" : "strand");
	if( !o_HairInfo )
	{
		o_HairInfo.reset( new mdlHairInfo );
	}
	o_HairInfo->m_nHairVertices = i_HairInfo->numHairVertices;
	if( i_HairInfo->numHairs <= 0 )
	{
		return;
	}
	int iStrand = 0;
	o_HairInfo->m_nVerticesPerStrand =  i_HairInfo->hairEndIndices[ iStrand ] - i_HairInfo->hairStartIndices[ iStrand ];
	o_HairInfo->m_Strands.resize( i_HairInfo->numHairs );
	for ( iStrand = 0;  (iStrand < i_HairInfo->numHairs); iStrand++)
	{
		mdlHairStrand &strand = o_HairInfo->m_Strands[ iStrand ];
		strand.Material.m_RootRadius = i_HairInfo->rootRadii[iStrand];
		strand.Material.m_TipRadius = i_HairInfo->tipRadii[iStrand];
		strand.Material.m_RootColor = maVector3d( i_HairInfo->rootColors[iStrand].r, i_HairInfo->rootColors[iStrand].g,i_HairInfo->rootColors[iStrand].b );
		strand.Material.m_TipColor = maVector3d( i_HairInfo->tipColors[iStrand].r, i_HairInfo->tipColors[iStrand].g,i_HairInfo->tipColors[iStrand].b );
		strand.Material.m_SurfaceNormal = maVector3d( i_HairInfo->surfaceNormals[iStrand].x,
			i_HairInfo->surfaceNormals[iStrand].y,
			i_HairInfo->surfaceNormals[iStrand].z
			);
		strand.Material.m_Opacity = i_HairInfo->opacities[ iStrand ];
		strand.Material.m_Specular = i_HairInfo->speculars[ iStrand ];
		strand.Material.m_Gloss = i_HairInfo->glosses[ iStrand ];
		strand.Material.m_AmbientDiffuse = i_HairInfo->ambDiffs[ iStrand ];
		strand.m_ControlPoints.resize( o_HairInfo->m_nVerticesPerStrand  );
		for (int i = i_HairInfo->hairStartIndices[iStrand];
			i < i_HairInfo->hairEndIndices[iStrand];
			i++)
		{
			int j = i - i_HairInfo->hairStartIndices[iStrand];
			//DBG_ASSERT( ( i - i_HairInfo->hairStartIndices[iStrand] < o_HairInfo->m_nVerticesPerStrand ), "error in reading hair strand " << i );
			int vert = i_HairInfo->hairVertices[i];
			mdlHairVertex &hairVertex = strand.m_ControlPoints[ j ];
			hairVertex.Position = maVector3d(
				i_HairInfo->vertices[vert].x ,
				i_HairInfo->vertices[vert].y ,
				i_HairInfo->vertices[vert].z 
				);
		}
	}
}