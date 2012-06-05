/*****************************************************************************
**  ModelExportIntent.cpp
**
**	Exports max nodes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "ModelExportIntent.hpp"


#ifndef MAXEXP_MAXEXPORTERUTILS_HPP
#include "MaxExportUtils.hpp"
#endif
#include "ExportDoc.hpp"
#include "MtlExporter.hpp"
#include "MeshExporter.hpp"
#include "HelperExporter.hpp"
#include "MdlHierarchyVisit.hpp"
#include "MaxMeshUtils.hpp"
#include "CameraAnimExporter.hpp"
#include "InstanceMgr.hpp"

#undef CreateFile
#undef DeleteFile
#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif 
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/mdl/mdlWriter.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"
#include "Graphics/mdl/mdlNodeUtil.hpp"
#include "MaxCommon.hpp"

#include <string>




using namespace std;
using namespace stdext;

namespace MaxExp
{
	

	//=============================================================================
	// Cleanup the  book-keeping data structures
	//=============================================================================
	void ModelExportIntent::CleanUp()
	{	
		ExportIntent::CleanUp();
	}

	

BaseExporter *ModelExportIntent::GetAppropriateExporter( INode *i_pCurNode, string &stypeName, MaxObjectType::TypeVal &objType )
	{	
		BaseExporter *retVal = NULL;
		MaxObjectType::Get(  i_pCurNode, OPTS, stypeName, objType );
		switch( objType )
		{
		case MaxObjectType::Mesh: 
			retVal = m_pExportDoc->m_pMeshExporter;
			break;

		case MaxObjectType::Helper:
			retVal = m_pExportDoc->m_pHelperExporter;
			break;
		case MaxObjectType::Bone:
			retVal = m_pExportDoc->m_pTrivialExporter;
			break;			
		case MaxObjectType::Camera:
			retVal = m_pExportDoc->m_pCameraAnimExporter;
			break;
		case MaxObjectType::Target:
		case MaxObjectType::Light:
			retVal = m_pExportDoc->m_pIgnoreExporter;
			break;
		default:
			retVal = m_pExportDoc->m_pErrorExporter;
			break;
		}
		return retVal;
	}


	bool ModelExportIntent::PreExportValidation()
	{
		//setting the progress report
		//total bytes exported for animation + number of nodes exported
		float fProgressReportBudget = static_cast< float > (  m_NodesToBeExported.size() );
		m_ProgressReport.reset( new ProgressReport( fProgressReportBudget  ) ) ;	
		return true;
	}
	
	void ModelExportIntent::PreExportBookKeeping()
	{

		//Let us collect all bones associated with
		//all exported meshes into a single container

		AssociatedBonesCollectVisit collectBones( *this  );
		VisitINodeHierarchyRec( m_pRootNode, collectBones );		
		m_BonesAssociatedWithAllMeshes.clear();
		m_BonesAssociatedWithAllMeshes.assign( 
			collectBones.m_BonesAssociatedWithAllMeshes.begin(),
			collectBones.m_BonesAssociatedWithAllMeshes.end()
			);
		_RPT0(_CRT_WARN, "bones before sorting\n" );
		for_each( m_BonesAssociatedWithAllMeshes.begin(),  m_BonesAssociatedWithAllMeshes.end(), DbgPrintINodeName() );
		sort( m_BonesAssociatedWithAllMeshes.begin(), m_BonesAssociatedWithAllMeshes.end(), LessINode() );
		_RPT0( _CRT_WARN, "bones after sorting\n" );
		for_each( m_BonesAssociatedWithAllMeshes.begin(),  m_BonesAssociatedWithAllMeshes.end(), DbgPrintINodeName() );
		
	}

	//=============================================================================
	// export the nodes that are already evaluated to be exported
	//=============================================================================
	void ModelExportIntent::ExportNodes()
	{
		assert( NULL != m_pRootNode );
		
		//Do a pre export
		//to do some book keeping
		PreExportBookKeeping();


		//The definition of a FileWriterLifeTimeKeeper
		//will accomplish the following lines of code
		/*
		fsLocator locator;
		fsFileUtil::ANSIFilenameToLocator(m_pExportDoc->m_FileName,  locator);
		if( fsFileUtil::FileExists(locator) )
		{
			fsFileUtil::DeleteFile(locator);
		}
		fsFileUtil::CreateFile(locator);
		gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
		file.WriteHeader();		
		chBinWriter writer(file);

		Further it will regsiter writer with m_pExportDoc
		so that it can be accessed in its life time from elsewhere.

		*/
		
		

		FileWriterLifeTimeKeeper fileWriterLifeTimeKeeper( *m_pExportDoc );
		
		

		m_RootMdlNode.reset( new mdlNodeInfo );
		m_RootMdlNode->m_NodeName = MbcsToUtf8( m_pRootNode->GetName() );
		maMatrix4x4 parentCumulativeTransform = m_RootMdlNode->m_Transform;
		for (int j = 0; j < m_pRootNode->NumberOfChildren(); j++)
		{		
			INode *pExportedRootNode = NULL;
			ExportNodesRec( 
				m_pRootNode->GetChildNode(j), 			
				pExportedRootNode,
				m_RootMdlNode );	
		}
		
		PostProcess( m_RootMdlNode );
	

		//Final writing
		if( m_RootMdlNode->m_Children.size() > 0)
		{
			mdlWriter::WriteHierarchicalModel(*(m_pExportDoc->GetWriter()), m_RootMdlNode, *(m_pExportDoc->m_pMdlMatInfoTable) );
		}
		else
		{
			string sfname = envString::WideCharToUTF8(m_pExportDoc->m_FileName.c_str() );
			EXPLOG.WriteWarning("no valid nodes were exported, output file '%s' may be empty", sfname.c_str()  );
			throw export_failure();
		}
	}
	//=============================================================================
	// recursive exporting function called by ExportNodes()
	// 
	//=============================================================================
	void ModelExportIntent::ExportNodesRec( 
		INode *i_pCurNode,   
		INode *& io_pExportedRootNode,
		shared_ptr<mdlNodeInfo> &io_ParentMdlNode )
	{	
		shared_ptr<mdlNodeInfo> nodeToBePassedToChildren(io_ParentMdlNode) ;
		bool bExportChildren = true;
		if( m_NodesToBeExported.find( i_pCurNode) != m_NodesToBeExported.end() )
		{
			try
			{	
				shared_ptr<mdlNodeInfo> curMdlNode;

				MCHAR *szNodeName = i_pCurNode->GetName();
				Object* object = i_pCurNode->GetObjectRef();
				if (object == NULL)
				{
					EXPLOG.WriteWarning( "object corresponding to this node '%s'  is NULL" , MBCSTOLPCSTR( szNodeName ) );
					throw export_failure();	
				}

				//this node ought to be exported			

				bool bJoint = false;
				curMdlNode.reset(new mdlNodeInfo(bJoint));

				//set the node name
				curMdlNode->m_NodeName = SgpuConvert::NodeName(szNodeName);

				//get the  local transform
				const Matrix3 &tm = m_pExportDoc->GetLocalTranform( i_pCurNode );
				curMdlNode->m_Transform = SgpuConvert::Ma4x4( tm);

				SgpuInstanceMgr::TData  tdata;
				
				size_t nInstances = m_pExportDoc->m_InstanceMgr->FindInstance( i_pCurNode, tdata );
				shared_ptr< mdlNodeInfo > possiblePrevInstance = tdata.m_NodeInfo;
				


				if( nInstances > 1 && NULL != possiblePrevInstance.get() )
				{
					deque<string> namePath;
					vector< shared_ptr< mdlNodeInfo >  > path;
					//get all the mdlNodes in the path from the m-RootMdlNode
					//to the node tdata.m_NodeInfo
					GetPathFromRoot( tdata.m_NodeInfo, path );
					vector< shared_ptr< mdlNodeInfo > >::const_iterator pit;
					//Get a vector of all the names 
					//in the path
					for( pit = path.begin(); pit != path.end(); ++pit )
					{
							namePath.push_back( (*pit)->m_NodeName );
					}
					//if namePath is empty,
					//no referenced node has been found
					if( namePath.size() <= 0)
					{
						EXPLOG.WriteError( "Cant export %s as instance because the referred node %s cant be found", MBCSTOLPCSTR( szNodeName ), tdata.m_NodeInfo->m_NodeName.c_str() );
						throw export_failure();
					}
					shared_ptr< mdlPathReference > nodeInfoProxy( new mdlNodeInfoProxy(  namePath.begin(), namePath.end() ) );
					curMdlNode->m_InstanceInfo = nodeInfoProxy;
					//Giving the instanced node,
					//the same name  as the referenced node is a 
					//work around. 
					//Right now, this has the effect that,
					////in MSP only the referenced mesh's name comes up in
					// the explorer view.  THis is a huge
					// benefit if there are tens of thousands of
					//instanced nodes.
#define SGPU_INSTANCED_NODE_SAME_NAME_AS_REFERENCED_NODE 1
#if SGPU_INSTANCED_NODE_SAME_NAME_AS_REFERENCED_NODE
					MCHAR *pzLeadingInstanceName = tdata.m_LeadingInstance->GetName();					
					curMdlNode->m_NodeName = SgpuConvert::NodeName( pzLeadingInstanceName );
#endif
					bExportChildren = false;

					//book keeping statisctics
					ExportLogger::WriteStartElementWrap explogWrap( "exporting: %s as instance of %s", MBCSTOLPCSTR( szNodeName ), tdata.m_NodeInfo->m_NodeName.c_str()  ); 
					ReportProgress( 1.0 );
				} else
				{
					MaxObjectType::TypeVal objType;
					string  sTypeName;
					BaseExporter *baseExporter = GetAppropriateExporter( i_pCurNode, sTypeName, objType );
					baseExporter->Export( 
						i_pCurNode, 
						*this, 
						io_pExportedRootNode, 
						curMdlNode );

					MeshExporter *pMeshExporter = dynamic_cast< MeshExporter * >( baseExporter );
					if( NULL != pMeshExporter && NULL != curMdlNode->m_SubdivInfo.get() )
					{
						shared_ptr<smdlCharacterSkin> skin_info(new smdlCharacterSkin());	
						curMdlNode->m_SkinInfo = skin_info;
					}
					ExtractUserDefinedProps( i_pCurNode, curMdlNode );
					m_pExportDoc->m_InstanceMgr->AddInstances( i_pCurNode, curMdlNode );
				}
				

				nodeToBePassedToChildren = curMdlNode;				
				io_ParentMdlNode->m_Children.push_back( curMdlNode );
				if( NULL == io_pExportedRootNode )
				{
					io_pExportedRootNode = i_pCurNode;
				}
			}

			catch( export_failure  & )
			{
				//The export of this node failed for some reasons
				//This information is already logged
				//Please lte us continue with the export of the children
				//Please note, the children will be exported as 
				//children of io_ParentMdlNode
			}

			catch( export_ignore & )
			{
				//The export of this node is ignored for some reasons
				//This information is already logged
				//Please lte us continue with the export of the children
				//Please note, the children will be exported as 
				//children of io_ParentMdlNode
			}

	
		} //if( m_NodesToBeExported.find( i_pCurNode) != m_NodesToBeExported.end() )

		if( bExportChildren )
		{
			//export its children
			for (int j = 0; j < i_pCurNode->NumberOfChildren(); j++)
			{				
				shared_ptr<mdlNodeInfo> childMdlNode; 		
				ExportNodesRec( 
					i_pCurNode->GetChildNode(j), 
					io_pExportedRootNode,
					nodeToBePassedToChildren );

			}
		}
	}



	void ModelExportIntent::PostProcess( shared_ptr<mdlNodeInfo> &rootMdlNode)
	{

		ApplyYAxisUpXformCorrection( rootMdlNode );

		CorrectLeftHandednessVisit leftHandednessCorrection;
		VisitMdlHierarchyRec( rootMdlNode, leftHandednessCorrection );

		CreateBasisVectorsVisit cbvVisit;
		VisitMdlHierarchyRec( rootMdlNode, cbvVisit );
		std::vector< shared_ptr< mdlNodeInfo > > tobeMerged( 1 );
		tobeMerged[0] = rootMdlNode;
		wstring wFname = GetFileTitle( m_pExportDoc->m_FileName );
		std::string sFname = envString::WideCharToUTF8( wFname );
		if ( OPTS.m_bMergeBasedOnMtls )
		{ 
			if ( OPTS.m_nMaxNumTrianglesInMergedMesh < 1000 )
			{
				EXPLOG.WriteWarning( "merging meshes based on mtls: max number of tri-s %d in merged mesh too low", OPTS.m_nMaxNumTrianglesInMergedMesh );
			}
			stringstream ss;
			ss << "merging based on materials: max num of primitives = " << OPTS.m_nMaxNumTrianglesInMergedMesh;
			SimpleProfile sp( ss.str() );
			int numMeshesMerged = 0;
			int numMeshesRsulted =0;
			mdlNodeUtil::DoMerge( 
				sFname, 
				rootMdlNode, 
				OPTS.m_nMaxNumTrianglesInMergedMesh, 
				numMeshesMerged, 
				numMeshesRsulted );
			EXPLOG.WriteInfo( "Mtl Merge Stats: num Meshes Merged = %d , num merged meshes resulted %d", numMeshesMerged, numMeshesRsulted);
		}
	}


	bool ModelExportIntent::GetPathFromRootRec( const shared_ptr< mdlNodeInfo > & i_DesiredNode, const shared_ptr< mdlNodeInfo > &i_CurNode,  std::vector< shared_ptr< mdlNodeInfo > > &o_PathOutput )
	{
		bool bRet = false;
		if( i_DesiredNode == i_CurNode )
		{
			o_PathOutput.push_back( i_CurNode);
			bRet = true;
			return bRet;
		}
		std::vector< shared_ptr< mdlNodeInfo > >::const_iterator cit;
		for( cit = i_CurNode->m_Children.begin(); cit != i_CurNode->m_Children.end(); ++cit )
		{
			bRet = GetPathFromRootRec( i_DesiredNode, *cit, o_PathOutput);			
			if( bRet )
			{
				o_PathOutput.push_back( i_CurNode );
				return bRet;
			}
		}
		DBG_ASSERT( (!bRet), "inconsistent logic in getting node: " << i_CurNode->m_NodeName << " from hierarchy" );
		return bRet;
	}



	void ModelExportIntent::GetPathFromRoot( const shared_ptr< mdlNodeInfo > & i_DesiredNode, std::vector< shared_ptr< mdlNodeInfo > > &o_PathOutput )
	{
		bool bRet = GetPathFromRootRec( i_DesiredNode, 	m_RootMdlNode, o_PathOutput );
		std::reverse( o_PathOutput.begin(), o_PathOutput.end() );
	}



} //maxExp