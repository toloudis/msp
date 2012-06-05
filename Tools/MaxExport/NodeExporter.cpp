/*****************************************************************************
**  NodeExporter.cpp
**
**	Exports max nodes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "NodeExporter.hpp"


#include "ExportDoc.hpp"
#include "MaxExportOptions.hpp"
#include "MaxExportUtils.hpp"
#include "MtlExporter.hpp"
#include "MeshExporter.hpp"
#undef CreateFile
#undef DeleteFile
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/mdl/mdlWriter.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "MaxCommon.hpp"

#include <string>




using namespace std;
using namespace stdext;

namespace MaxExp
{
	//=============================================================================
	// This is a visit structure  for the procedure 'VisitMdlHierarchyRec'
	// Please see the details of 'VisitMdlHierarchyRec' in MaxExportUtils.hpp
	//
	// In summary, FlattenVisit applies the cumulative transform acting upon a node
	// to its vertices and normals
	//
	// FlattenVisit's stack_type is maMatrix4x4,
	// and its stores the cumulative transform in m_Stack
	//
	// FlattenVisit, on 'Pre' will compute the crrent cumulativeTransform 
	// taking into account its own transform and then
	// apply it to the  vertices and normals of the mesh
	//=============================================================================
	

	struct FlattenVisit
	{
		FlattenVisit()
		{
			m_Stack.Identity();
		}
		typedef maMatrix4x4 stack_type;
		void Pre( shared_ptr< mdlNodeInfo > &node )
		{
			maMatrix4x4 curCumulativeTransform =  (node->m_Transform * m_Stack);
			if( node->m_MeshInfo.get() )
			{
				MeshExporter::ApplyRTTransformationToMesh( curCumulativeTransform, *(node->m_MeshInfo)  );
			}	
			m_Stack = curCumulativeTransform;
		}
		void Post( shared_ptr<mdlNodeInfo > &node ){}
		stack_type m_Stack;	
	};


	//=============================================================================
	// This is a visit structure  for the procedure 'VisitMdlHierarchyRec'
	// Please see the details of 'VisitMdlHierarchyRec' in MaxExportUtils.hpp
	//
	// In summary, CorrectLeftHandednessVisit, computes the cumulative transform 
	//	acting upon a node and if the cumulative tranform is left-handed
	//  it flips the vertex ordeer in the index list
	//
	// CorrectLeftHandednessVisit's stack_type is maMatrix4x4,
	// and its stores the cumulative transform in m_Stack
	//
	// CorrectLeftHandednessVisit, on 'Pre' will compute the crrent 
	// cumulativeTransform taking into account its own transform and then
	// flips the vertex order in the indices if necessary
	//=============================================================================
	
	struct CorrectLeftHandednessVisit
	{
		typedef maMatrix4x4 stack_type;
		void Pre( shared_ptr< mdlNodeInfo > &node )
		{
			maMatrix4x4 curCumulativeTransform =  (node->m_Transform * m_Stack);
			if( node->m_MeshInfo.get() )
			{
				MeshExporter::ApplyCorrectionToVertexOrderForPotentiallyLeftHandedGeometry( curCumulativeTransform, *(node->m_MeshInfo)  );
			}	
			m_Stack = curCumulativeTransform;
		}
		void Post( shared_ptr<mdlNodeInfo > &node ){}
		stack_type m_Stack;	
	};


	//=============================================================================
	// This is a visit structure  for the procedure 'VisitMdlHierarchyRec'
	// Please see the details of 'VisitMdlHierarchyRec' in MaxExportUtils.hpp
	//
	// In summary, CreateBasisVectorVisit, computes the cumulative transform 
	//	acting upon a node and if the cumulative tranform is left-handed
	//  it flips the vertex ordeer in the index list
	//
	// CreateBasisVectorVisit's stack_type is a dummy stack_type
	//
	// CreateBasisVectorVisit, on 'Pre' will compute  the basis vectors
	// of the  mesh corresponding to the node that is currently visited 
	//=============================================================================
	
	struct CreateBasisVectorsVisit
	{
		typedef int stack_type;
		void Pre( shared_ptr< mdlNodeInfo > &node )
		{
			if( node->m_MeshInfo.get() )
			{
				mdlFragUtil::CreateBasisVectors( *(node->m_MeshInfo)  );
			}	
		}
		void Post( shared_ptr<mdlNodeInfo > &node ){}
		stack_type m_Stack;	
	};




	//=============================================================================
	// Do some validation checks on the list of nodes to be exported
	// ToDo[kg]: this function is incomplete as of now (03/24/09)
	//=============================================================================
	bool NodesToBeExported::PreExportValidate()
	{
		bool bVal = true;
		NodesToBeExported::iterator hit;
		for( hit = begin(); bVal && (hit != end())  ; ++hit)
		{
			INode *curNode = *hit;
			if( NULL == curNode )
			{
				bVal = false;
				break;
			}
			if( curNode->IsRootNode() )
			{
				bVal = false;
				break;
			}
			INode *parent = curNode->GetParentNode();
			if( NULL == parent )
			{
				bVal = false;
				break;
			}
			NodesToBeExported::const_iterator fit = end();
			if( OPTS->m_bExportParentIfChildExports )
			{
				//check to make sure that parent of a node is present
				if( !parent->IsRootNode() &&  ( fit = find( parent ) ) == end() )
				{
					bVal = false;
					break;
				}
			}
			//sanity check for making sure that
			//always parents come before children in the hash set
#if defined(SGPU_DEBUG)
			if( fit != end() )
			{
				bVal = ln( *fit, *hit);
				if ( !bVal )
				{
					break;
				}
			}
#endif
		}
		return bVal;
	}


	//=============================================================================
	// Cleanup the  book-keeping data structures
	//=============================================================================
	void NodeExporter::CleanUp()
	{	
		m_NodesToBeExported.clear();
		m_pRootNode = NULL;
		m_WorldTransformsCache.clear();
		m_LocalTransformsCache.clear();
		m_nNumNodesSoFarExported =0;
	}


	//=============================================================================
	// Main public function that conducts the export
	//=============================================================================
	void NodeExporter::Do()
	{
		//do a clean up of the current book keeping
		CleanUp();

		Interface *maxInterface = GetCOREInterface();
		//cache the root node
		m_pRootNode = maxInterface->GetRootNode();
		assert( NULL != m_pRootNode );

		//_nodesToBeExported is filled in
		ComputeNodesToBeExported();

		//Todo [kg:03/24/09] call m_NodesToBeExported.PreExportValidate();

		//export the nodes
		ExportNodes();

	}

	//=============================================================================
	// export the nodes that are already evaluated to be exported
	//=============================================================================
	void NodeExporter::ExportNodes()
	{
		assert( NULL != m_pRootNode );


		//start the write file
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



		shared_ptr<mdlNodeInfo> rootMdlNode( new mdlNodeInfo );
		rootMdlNode->m_NodeName = m_pRootNode->GetName();
		for (int j = 0; j < m_pRootNode->NumberOfChildren(); j++)
		{
			shared_ptr<mdlNodeInfo> curMdlChild; 		
			try {
				ExportNodesRec( m_pRootNode->GetChildNode(j),  curMdlChild );
				if( NULL != curMdlChild.get() )
				{
					rootMdlNode->m_Children.push_back( curMdlChild );
				}
			}
			catch( export_failure &)
			{
				EXPLOG.WriteError( "Export of '%s' failed !!", m_pExportDoc->m_FileName.c_str() );
			}			
		}

		//Apply Y-Axis Up transformation correction
		if( rootMdlNode->m_Children.size() > 0)
		{
			maMatrix4x4 zaxisUpToYaxisUp( 1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, -1.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			maMatrix4x4 invZaxisUpToYaxisUp( 1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
			std::vector< shared_ptr<mdlNodeInfo> >::iterator chit;
			for( chit = rootMdlNode->m_Children.begin(); chit != rootMdlNode->m_Children.end(); ++chit )
			{
				shared_ptr<mdlNodeInfo> ch = *chit;
				ch->m_Transform = ch->m_Transform *  zaxisUpToYaxisUp;
			}
		}

		//In max for a mesh, you can potentially
		//have a left handed tranform cumulative matrix applied to
		//a mesh, in which case the msh is turned inside out.
		//the following code correct that
		if( 
			rootMdlNode->m_Children.size() > 0
			)
		{
			CorrectLeftHandednessVisit leftHandednessCorrection;
			VisitMdlHierarchyRec( rootMdlNode, leftHandednessCorrection );
		}

		//Calculate basis vectors
		if( 
			rootMdlNode->m_Children.size() > 0
			)
		{
			CreateBasisVectorsVisit cbvVisit;
			VisitMdlHierarchyRec( rootMdlNode, cbvVisit );
		
		}


		//Final writing
		if( rootMdlNode->m_Children.size() > 0)
		{
			mdlWriter::WriteHierarchicalModel(writer, rootMdlNode, *(m_pExportDoc->m_pMdlMatInfoTable) );
		}
		else
		{
			EXPLOG.WriteWarning("no valid nodes were exported, output file '%s' may be empty", m_pExportDoc->m_FileName.c_str() );
		}
	}


	//=============================================================================
	// get the world transform of the current node,
	// cache it if it is not yet  cached
	//=============================================================================
	const Matrix3 & NodeExporter::GetWordTransform( INode * i_pCurNode )
	{
		map<INode*, Matrix3>::iterator mit = m_WorldTransformsCache.find( i_pCurNode );
		if( mit == m_WorldTransformsCache.end() )
		{	
			Matrix3 tm = i_pCurNode->GetNodeTM( TIME_EXPORT_START );
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
	const Matrix3 & NodeExporter::GetLocalTranform( INode * i_pCurNode )
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

	//=============================================================================
	// recursive exporting function called by ExportNodes()
	// 
	//=============================================================================
	void NodeExporter::ExportNodesRec( INode *i_pCurNode, shared_ptr<mdlNodeInfo> &io_curMdlNode )
	{	
		try
		{	
			if( m_NodesToBeExported.find( i_pCurNode) != m_NodesToBeExported.end() )
			{	

				TCHAR *szNodeName = i_pCurNode->GetName();
				Object* object = i_pCurNode->GetObjectRef();
				if (object == NULL)
				{
					EXPLOG.WriteWarning( "object corresponding to this node '%s'  is NULL" , szNodeName);
					throw export_failure();	
				}

				//this node ought to be exported			

				bool bJoint = false;
				io_curMdlNode.reset(new mdlNodeInfo(bJoint));
				//set the node name
				io_curMdlNode->m_NodeName = szNodeName;

				//get the  local transform
				const Matrix3 &tm = GetLocalTranform( i_pCurNode );
				io_curMdlNode->m_Transform = SgpuConvert::Ma4x4( tm);


				//if necessary put in an intermediate mdl node which contains
				//the pivot information
				shared_ptr<mdlNodeInfo> intermediateMdlNode( io_curMdlNode );
				Matrix3 pivotTm(1);
				GetPivotTransform( i_pCurNode, pivotTm);			
				if( !pivotTm.IsIdentity() || i_pCurNode->IsGroupHead() == TRUE )
				{
					shared_ptr<mdlNodeInfo> pivotNodeGraph;
					pivotNodeGraph.reset( new mdlNodeInfo(bJoint) );
					pivotNodeGraph->m_NodeName = string( szNodeName) + string("_pivot");
					pivotNodeGraph->m_Transform = SgpuConvert::Ma4x4(pivotTm); 
					io_curMdlNode->m_Children.push_back( pivotNodeGraph );
					intermediateMdlNode = pivotNodeGraph ;
				}

				bool bSupportXref = OPTS->m_bSupportXref;
				MaxObjectType::TypeVal objType = MaxObjectType::Get(i_pCurNode,  bSupportXref );
				const string & sTypeName = MaxObjectType::GetTypeName( objType );
				switch( objType )
				{
				case MaxObjectType::Mesh: 
					m_pExportDoc->m_pMeshExporter->Export( i_pCurNode , intermediateMdlNode );
					EXPLOG.WriteInfo( "object '%s' exported as a '%s'", szNodeName, sTypeName.c_str() );
					break;

				default:
					EXPLOG.WriteInfo( "object '%s' not exported as type '%s' not recognized", szNodeName, sTypeName.c_str()  );
					throw export_failure();
				}



				//Report the  progress 
				Interface *pMaxInterface = GetCOREInterface();
				++m_nNumNodesSoFarExported;
				assert( m_NodesToBeExported.size() > 0);
				assert( m_nNumNodesSoFarExported <=  m_NodesToBeExported.size() );
				int pctExported = (m_nNumNodesSoFarExported * 100)/ m_NodesToBeExported.size();
				pMaxInterface->ProgressUpdate( pctExported, 1, "Nodes Exported" );



			}
			//export its children
			for (int j = 0; j < i_pCurNode->NumberOfChildren(); j++)
			{				
				shared_ptr<mdlNodeInfo> childMdlNode; 		
				ExportNodesRec( i_pCurNode->GetChildNode(j), childMdlNode);
				if( NULL !=  childMdlNode.get() )
				{
					io_curMdlNode->m_Children.push_back( childMdlNode );				
				}
			}
		}
		catch( export_failure  & )
		{
			io_curMdlNode.reset( );
		}
	}



	//=============================================================================
	//  higher level function which evaluates what all nods need be exported
	//  see .hpp
	//=============================================================================
	size_t NodeExporter::ComputeNodesToBeExported()
	{
		list<INode *> selectedNodes;	
		assert( NULL != m_pRootNode );

		const TCHAR *szRootNode = m_pRootNode->GetName();

		//get all the selected nodes
		if (OPTS->m_bExportSelected)
		{
			int nSelectedNodes = GetCOREInterface()->GetSelNodeCount();
			for (int i = 0; i < nSelectedNodes; ++i)
			{
				selectedNodes.push_back(GetCOREInterface()->GetSelNode(i));
			}
		}


		hash_set<INode *> forcedNodes;
		//check all the children of the root node,
		//(this is recursive and fills up the m_NodesToBeExported, and forcedNodes)

		for (int j = 0; j < m_pRootNode->NumberOfChildren(); j++)
		{
			ComputeNodesToBeExportedRec( m_pRootNode->GetChildNode(j), selectedNodes, forcedNodes);
		}


		//add the forced nodes alos into the list of nodes to be exported
		hash_set<INode*>::const_iterator fit;
		for( fit = forcedNodes.begin(); fit != forcedNodes.end(); ++fit )
		{
			INode *thisNodeAlsoNeedBeExported = *fit;
			const TCHAR * szThisNodeName = thisNodeAlsoNeedBeExported->GetName();
			m_NodesToBeExported.insert( thisNodeAlsoNeedBeExported );
		}

		return m_NodesToBeExported.size();
	}


	//=============================================================================
	//Core Recursive function used by the above function
	//i_pCurNode = current input node
	//io_SelectedNodes = if we are in exportOnlySelected mode this, serves the selected 
	//				nodes 
	//io_ForcedNodes = nodes not necessarily selected, but need be anyway exported 
	//				because of some connection
	//				with nodes already present in the relavant set.
	// returns true if the input curNode was elected to be exported
	//				what all nods need be exported
	//=============================================================================
	bool  NodeExporter::ComputeNodesToBeExportedRec(
		INode * i_pCurNode, 
		list<INode *> &io_SelectedNodes, 
		stdext::hash_set<INode *> &io_ForcedNodes
		)
	{
		const TCHAR *szCurNodeName = i_pCurNode->GetName();

		bool bOptionExportSelected = OPTS->m_bExportSelected;

		bool bCurNodeToBeExported = false;
		//if the export mode is by selection,
		if( bOptionExportSelected )
		{
			//check if the current node is cited as a selected node
			list<INode *>::iterator lit = find( io_SelectedNodes.begin(), io_SelectedNodes.end(), i_pCurNode);
			if( lit != io_SelectedNodes.end() )
			{
				bCurNodeToBeExported = true;
				//erase the node from the selected list
				io_SelectedNodes.erase( lit );
			}		

		} else //if the export mode is 'export all'
		{
			//then as long as it is not hidden it is selected
			bCurNodeToBeExported |= !i_pCurNode->IsHidden();
		}

		//if '_bCurNodeToBeExported' then put it in the nodesToBeExported container
		if( bCurNodeToBeExported)
		{			
			m_NodesToBeExported.insert( i_pCurNode );
		}


		//There  are two additional export option policies
		//_bAddChildIfParentNodeExports  = if the parent node exports, force-export the child also as long as it i not hidden
		//_bAddParentIfChildNodeExports =  if the child node exports force-export parent node


		//If _bAddChildIfParentNodeExports  is true,

		//treat curNode as the 'child' node and its parent as the 'parent' node
		INode *parent = i_pCurNode->GetParentNode();
		bool forceThisDueToParent =  false;
		if( !bCurNodeToBeExported && //if the current node is not yet sleighted to be exported
			!i_pCurNode->IsHidden() && //if the current node is not hidden
			OPTS->m_bExportChildIfParentExports && // if the policy is such that children are exported when parents are
			m_NodesToBeExported.find( parent ) != m_NodesToBeExported.end()  && //if the parent is sleighted to be exported
			io_ForcedNodes.find( parent ) != io_ForcedNodes.end()		 //or if the parent is present in the nodes to be exported 'forced'
			)
		{
			//this node has to be enrolled in the nodes to be exported in a forcefull manner
			forceThisDueToParent = true;
			io_ForcedNodes.insert( i_pCurNode );
		}

		//If _bAddParentIfChildNodeExports  is true,

		bool forceThisDueToChild = false;
		//Now recursively evaluate the exportability of  children
		for (int j = 0; j < i_pCurNode->NumberOfChildren(); j++)
		{
			//If any of my children are exported, set forcThisDueToChild = true
			forceThisDueToChild |= ComputeNodesToBeExportedRec(i_pCurNode->GetChildNode(j), io_SelectedNodes, io_ForcedNodes);
		}


		if( !bCurNodeToBeExported  && // if curNode is selected to be exported in a straight manner
			!forceThisDueToParent &&  //if this node has not already been forced due to another policy
			forceThisDueToChild  && //if one of the children were exported
			OPTS->m_bExportParentIfChildExports //if the policy is such that parents get exported whenever a child is
			)
		{
			io_ForcedNodes.insert( i_pCurNode );
		}	
		return bCurNodeToBeExported | forceThisDueToChild | forceThisDueToParent;
	}









} //maxExp