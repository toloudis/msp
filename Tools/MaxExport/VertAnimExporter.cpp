
/*****************************************************************************
**  VertAnimExporter.cpp
**
**	Exports nodes containing geometry (poly-mesh, tri-mesh, shapes)
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "VertAnimExporter.hpp"


#include "MaxCommon.hpp"
#include "MaxExportUtils.hpp"
#ifndef MAXEXP_POLYORTRIMESH_HPP
#include "PolyOrTriMesh.hpp"
#endif
#include "MaxMeshUtils.hpp"
#include "MtlExporter.hpp"
#include "ExportDoc.hpp"
#include "ExportIntent.hpp"
#include "HashSetPoint3.hpp"
#include "SkinExporter.hpp"
#include "VertAnimExportIntent.hpp"
#include "MaxObjectFlags.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/vtx/vtxVertexAnimKeysStatic.hpp"
#include "Graphics/vtx/vtxCompressionUtil.hpp"
#include "Graphics/vtx/vtxStreamCompression.hpp"
#include <iostream>
#include <string>
#include <hash_set>
#include <cmath>
#include <map>
#include <crtdbg.h>


using namespace std;
using namespace stdext;
using ::Mesh;


namespace MaxExp
{	

	//========================================================================
	//Derived class of VertexAnimData to store exported vertex animation data
	//of a mesh across frames
	//========================================================================
	VertAnimExporter::VertexAnimDataUncompressed::VertexAnimDataUncompressed ( const std::string &i_Name, VertAnimExportIntent &i_Intent ):
VertAnimExporter::VertexAnimData( i_Name, i_Intent )
{
	m_VertexFrames.reset( new vtxVertexFramesStatic() );
	m_VertexAnim.reset( new vtxVertexAnimKeysStatic() );
}

void VertAnimExporter::VertexAnimDataUncompressed::UpdateFrame( vtxVertexFrame *i_pCurFrame, float i_Offset )
{
	m_VertexFrames->AddVertexFrame( i_pCurFrame );
	m_VertexAnim->Frames().AddKey( i_Offset, i_pCurFrame);
}
void VertAnimExporter::VertexAnimDataUncompressed::DepleteData()
{
	chWriter &writer = *( m_Intent.GetExportDoc()->GetWriter() );	
	VertAnimExporter::ReportProgress rprogress( m_Intent );
	vtxVertexAnimWriter:: WriteVertexAnimForMesh ( writer, rprogress, m_Name, m_VertexAnim ); 	
}



//========================================================================
//Derived class of VertexAnimData to store compressed 
//exported vertex animation data
//of a mesh across frames
//========================================================================
VertAnimExporter::VertexAnimDataCompressed::VertexAnimDataCompressed ( const std::string &i_Name, VertAnimExportIntent &i_Intent ):
VertexAnimData( i_Name, i_Intent )
{
	DBG_ASSERT( (NULL != m_Intent.m_GeomCache.get() ), ("should have a geom cache for vertex animation compression" ));
	if ( m_Intent.OPTS.m_fToleranceVertexAnim <= 0.0f )
		m_CompressStream = 
		vtxCompressionUtil::CreateLosslessCompressStream(
		m_Name,
		*m_Intent.m_GeomCache
		);
	else
		m_CompressStream = 
		vtxCompressionUtil::CreateToleranceCompressStream(
		m_Intent.OPTS.m_fToleranceVertexAnim,
		MbcsToUtf8( i_Name.c_str() ), 
		*m_Intent.m_GeomCache
		);
}


void VertAnimExporter::VertexAnimDataCompressed::UpdateFrame( vtxVertexFrame *i_pVertexFrame, float i_Offset )
{
	VertAnimExporter::ReportProgress rprogress( m_Intent );
	rprogress( i_pVertexFrame );
	m_CompressStream->SubmitFrame( i_Offset, *i_pVertexFrame );
	delete i_pVertexFrame;
}


void VertAnimExporter::VertexAnimDataCompressed::FinishUpdateFrame(  )
{
	m_CompressStream->Finish();
}

void VertAnimExporter::VertexAnimDataCompressed::DepleteData()
{
	VertexAnimDataCompressed *pCopy = new VertexAnimDataCompressed( m_Name, m_Intent );
	pCopy->m_CompressStream = m_CompressStream;
	shared_ptr< VertexAnimData > queuedData(pCopy  );
	m_Intent.m_VertAnimDataQueue.push_back( queuedData );
}

void VertAnimExporter::ReportProgress::operator()(vtxVertexFrame *i_pVertexFrame )const
{
	//progress report calculation
	//size of this frame
	float thisProgress = ( i_pVertexFrame->m_Normals.size() + i_pVertexFrame->m_Positions.size() ) *  12.0f;
	m_Intent.ReportProgress( thisProgress );
}

void VertAnimExporter::VertexAnimDataCompressed::WriteCompressedData( 
	chWriter & i_Writer, 
	ExportIntent &i_Intent,
	std::vector< shared_ptr< VertexAnimData > >::const_iterator &i_VitBegin, 
	std::vector< shared_ptr< VertexAnimData > >::const_iterator &i_VitEnd
	)
{
	if( i_Intent.OPTS.m_bCompressVertexAnim )
	{
		std::vector< shared_ptr< VertexAnimData > >::const_iterator vit;
		for( vit = i_VitBegin; vit != i_VitEnd; ++ vit )
		{
			VertexAnimData *pVertAnimData = vit->get();
			DBG_ASSERT( pVertAnimData, "should have a non-null vertexAnimData");
			VertexAnimDataCompressed *pCompressedData = dynamic_cast< VertexAnimDataCompressed * > ( pVertAnimData );
			DBG_ASSERT( pCompressedData, "VertexAnimData should be compressed" );
			pCompressedData->m_CompressStream->WriteCompressedAnimationData( i_Writer );
		}
	}
}

void VertAnimExporter::CheckOnObject( INode  *i_pCurNode )
{

	MCHAR *szNodeName = i_pCurNode->GetName(); 
	TimeValue sampleTime;
	for( sampleTime = OPTS.m_StartTime; sampleTime <= OPTS.m_EndTime; sampleTime += OPTS.m_StepTime )
	{	
		//Check the export as subdiv flag

		bool bExportAsSubdiv;
		bool bObjFlagRet = AllowedObjectFlags::GetValue( i_pCurNode, L"export as subdiv", bExportAsSubdiv);					
		DBG_ASSERT( bObjFlagRet, "cannot get export as subdiv flag!");

		bool bTriangulate = !bExportAsSubdiv;
		ObjectRefResolver< ResolvePolicyVertAnim > objResolve( *m_pExportDoc, i_pCurNode, sampleTime , bTriangulate );

		::Mesh& mesh = objResolve.GetPolyOrTriMesh()->m_pTri->GetMesh();
		int nVerts = mesh.getNumVerts();
		for( int iv =0; iv < nVerts; ++iv )
		{
			Point3 pos =  mesh.verts[ iv ];
			_RPT5( _CRT_WARN, "%s: %dth Vert: %g %g %g\n", szNodeName, iv, pos.x, pos.y, pos.z );
		}
	}
}

float VertAnimExporter::EstimateAnimBudget( INode *i_pCurNode )
{

	bool bExportAsSubdiv;
	bool bObjFlagRet = AllowedObjectFlags::GetValue( i_pCurNode, L"export as subdiv", bExportAsSubdiv);					
	DBG_ASSERT( bObjFlagRet, "cannot get export as subdiv flag!");


	bool bExportNormals = !bExportAsSubdiv;
	bool bTriangulate = !bExportAsSubdiv;

	TimeValue startTime = OPTS.m_StartTime;
	TimeValue endTime = OPTS.m_EndTime;
	TimeValue stepTime = OPTS.m_StepTime;
	assert( stepTime > 0.0f);
	float startFrame = OPTS.m_fStartFrame;
	float endFrame = OPTS.m_fEndFrame;
	float stepFrame =  OPTS.m_fStepFrame;


	ObjectRefResolver< ResolvePolicyVertAnim > objRefResolve ( *m_pExportDoc,  i_pCurNode,  startTime, bTriangulate );
	int nVerts = objRefResolve.GetPolyOrTriMesh()->GetNVerts();
	int nFaces = objRefResolve.GetPolyOrTriMesh()->GetNFaces();
	int nNormals = nFaces * 3;
	int numKeys = static_cast<int> ( ( endFrame - startFrame ) / stepFrame ) + 1;
	float animBudgetInBytes = nVerts  * numKeys * 12.0f  + numKeys * 500;
	if( bExportNormals )
	{
		animBudgetInBytes = (nVerts + nNormals ) * numKeys * 12.0f + numKeys * 500 ;
	}
	return animBudgetInBytes;
}



void VertAnimExporter::ApplyRTTransformation( const maMatrix4x4 &i_Tm, vtxVertexFrame  &io_VertexFrame )
{

	vtxVertexFrame::VertContainerT::iterator vit;
	transform( io_VertexFrame.m_Positions.begin(), io_VertexFrame.m_Positions.end(),io_VertexFrame.m_Positions.begin(),  TransformRTVec3d( i_Tm ) );
	transform( io_VertexFrame.m_Normals.begin(), io_VertexFrame.m_Normals.end(), io_VertexFrame.m_Normals.begin(),  TransformRTVec3d( i_Tm ) );
}




// i_pExportedRootNode, please see BaseExporter.hpp for details
void VertAnimExporter::Export(
							  INode *i_pCurNode, 
							  ExportIntent &i_Intent,					  
							  INode *i_pExportedRootNode,
							  shared_ptr<mdlNodeInfo> &o_MdlNodeInfo )
{			

	MCHAR *szNodeName = i_pCurNode->GetName(); 
	ExportLogger::WriteStartElementWrap explogWrap( "exporting: %s", MBCSTOLPCSTR( szNodeName ) ); 

	bool bExportAsSubdiv;
	bool bObjFlagRet = AllowedObjectFlags::GetValue( i_pCurNode, L"export as subdiv", bExportAsSubdiv);					
	DBG_ASSERT( bObjFlagRet, "cannot get export as subdiv flag!");

	bool bExportNormals = !bExportAsSubdiv;

	TimeValue startTime = OPTS.m_StartTime;
	TimeValue endTime = OPTS.m_EndTime;
	TimeValue stepTime = OPTS.m_StepTime;
	assert( stepTime > 0.0f);
	float startFrame = OPTS.m_fStartFrame;
	float endFrame = OPTS.m_fEndFrame;
	float stepFrame =  OPTS.m_fStepFrame;		
	float sampleFrame;
	int nKeys = static_cast<int> ( ( (endFrame - startFrame) / stepFrame ) + 1 );
#if defined(_DEBUG)
	int nKeys_debug=0;
	for( sampleFrame = startFrame, nKeys_debug=0; sampleFrame <= endFrame; sampleFrame += stepFrame, ++nKeys_debug );
	assert( nKeys == nKeys_debug );
#endif



	bool isSkinned = false;
	{

		TimeValue startTime = OPTS.m_StartTime;
		ResolveSkinModifier skinDet( *m_pExportDoc,  i_pCurNode , startTime );				
		isSkinned = skinDet.Found();
	}


	maMatrix4x4 requiredWorldTransformForGeometry;

	if(  nKeys > 0 )
	{
		VertAnimExportIntent &vIntent = dynamic_cast< VertAnimExportIntent & > ( i_Intent );
		if( OPTS.m_bCompressVertexAnim )
		{

			m_VertexAnimData.reset ( 
				new VertexAnimDataCompressed ( 
				i_pCurNode->GetName(),
				vIntent
				)
				);
		} else
		{
			m_VertexAnimData.reset ( 
				new VertexAnimDataUncompressed ( 
				i_pCurNode->GetName(),
				vIntent
				)
				);
		}



		VertAnimExporter::ReportProgress rprogress( i_Intent );
		//degin adding exported vertex frames of this mesh
		m_VertexAnimData->BeginUpdateFrame();

		for( sampleFrame = startFrame; sampleFrame <= endFrame; sampleFrame += stepFrame )
		{
			TimeValue sampleTime = static_cast< TimeValue > ( sampleFrame * GetTicksPerFrame() );
			requiredWorldTransformForGeometry = CalculateRequiredWorldTransformForGeometry(
				m_pExportDoc,
				i_pCurNode,
				i_pExportedRootNode,
				sampleTime
				);
			MeshAnimExportCore animExport( 
				*m_pExportDoc, 
				*i_pCurNode, 
				sampleTime, 
				requiredWorldTransformForGeometry,
				true // bTriangulate
				);
			vtxVertexFrame  *pVertexFrame = NULL;

			animExport.DoExportFrame( pVertexFrame);		
			assert( pVertexFrame != NULL );
#if defined(_DEBUG)
#define SGPU_DEBUG 1
#if SGPU_DEBUG
#define DEBUG_VERTEX_IDX 95
			if( pVertexFrame->m_Positions.size() > DEBUG_VERTEX_IDX  )
			{
				const maVector3d & pos = pVertexFrame->m_Positions[ DEBUG_VERTEX_IDX ];
				_RPT5( _CRT_WARN, "%dth vertex at frame %g is %g %g %g\n", DEBUG_VERTEX_IDX, sampleFrame, pos[0], pos[1], pos[2] ); 
			}
#endif
#undef SGPU_DEBUG
#endif
			//progress report calculation
			//size of this frame
			rprogress(  pVertexFrame );	
			//Z-up transformation of the frame
			ApplyRTTransformation( ExportIntent::m_ZaxisUpToYaxisUp, *pVertexFrame );
			//add the frame to the m_VertexAnimData of the current mesh
			m_VertexAnimData->UpdateFrame( pVertexFrame,  sampleFrame - startFrame );

		}
		//done with adding
		m_VertexAnimData->FinishUpdateFrame();
	}
}		

} //namespace MaxExp