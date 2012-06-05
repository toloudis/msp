/*****************************************************************************
**  ExportIntent.cpp
**
**	Exports max nodes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "ExportIntent.hpp"


#include "ExportDoc.hpp"
#include "MaxExportUtils.hpp"
#include "MtlExporter.hpp"
#include "MeshExporter.hpp"
#include "MaxObjectFlags.hpp"
#undef CreateFile
#undef DeleteFile
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/mdl/mdlWriter.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"

#include "MaxCommon.hpp"
#include <string>




using namespace std;
using namespace stdext;

namespace MaxExp
{

	namespace
	{
		const chDefs::Name c_AFPS = chDefs::MakeName('A', 'F', 'P', 'S');
		const chDefs::Name c_BGFR = chDefs::MakeName('B', 'G', 'F', 'R');
		const chDefs::Name c_EXPV = chDefs::MakeName('E', 'X', 'P', 'V');

	}

	//=============================================================================
	// static member initialization
	//=============================================================================
	const maMatrix4x4 ExportIntent::m_ZaxisUpToYaxisUp( 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	const maMatrix4x4 ExportIntent::m_InvZaxisUpToYaxisUp( 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	//========================================================================
	// Wrapper class for progress report logic
	//========================================================================
	//wrapper class for encapsulating
	// the logic of starting, updating and ending
	//progress report communications to
	//GetCOREInterface()
	ProgressReport::ProgressReport( float i_fTotalBudget ):
	m_fTotalBudget( i_fTotalBudget ),
		m_fCurrentProgress( 0.0f )
	{
		assert( !EpsilonEqualZero( m_fTotalBudget ) );
		Interface *pMaxInterface = GetCOREInterface();
		assert( NULL != pMaxInterface);
		pMaxInterface->ProgressStart( _T("Sgpu Export..."), TRUE, fn, NULL);
	}

	ProgressReport::~ProgressReport()
	{
		Interface *pMaxInterface = GetCOREInterface();
		pMaxInterface->ProgressEnd();
	}

	void ProgressReport::Update( float i_fDelta )
	{
		Interface *pMaxInterface = GetCOREInterface();
		m_fCurrentProgress += i_fDelta;
		float fractionExported = m_fCurrentProgress / m_fTotalBudget;
		fractionExported = SgpuConvert::Clamp( fractionExported );
		int pctExported = static_cast< int > ( fractionExported * 100 );
		//update the progress0
		pMaxInterface->ProgressUpdate( pctExported, 1, "Nodes Exported" );	
		//poll the cancel button
		if( pMaxInterface->GetCancel())
		{
			 int retval = MessageBox(pMaxInterface->GetMAXHWnd(), "Really Cancel", "Question", MB_ICONQUESTION | MB_YESNO);
			 if (retval == IDYES)
			 {
				 EXPLOG.WriteError( "user cancelled export" );
				 throw export_cancel();
			 }
			 else if (retval == IDNO)
				 pMaxInterface->SetCancel(FALSE);

		}
	}

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
			} else if ( node->m_SubdivInfo.get() )
			{
				MeshExporter::ApplyCorrectionToVertexOrderForPotentiallyLeftHandedGeometry( curCumulativeTransform, *(node->m_SubdivInfo)  );				
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
	//A wrapper for governing the
	//life time semantics of opening
	//and closing of the exported file
	//=============================================================================

	ExportIntent::FileWriterLifeTimeKeeper::FileWriterLifeTimeKeeper(ExportDoc &exportDoc):
	m_pExportDoc(&exportDoc),
		m_pFile(NULL),
		m_pWriter(NULL)
	{
		fsLocator locator;
		itString itFileName( reinterpret_cast< const itString::CharType * > (  m_pExportDoc->m_FileName.c_str() ) );
		fsFileUtil::UnicodeStringToLocator( itFileName,  locator);
		if( fsFileUtil::FileExists(locator) )
		{
			try 
			{
				fsFileUtil::DeleteFile(locator);
			}
			catch( ... )
			{
				EXPLOG.WriteError( "cant write to file %s", m_pExportDoc->m_FileName.c_str() );
				throw export_failure();
			}
		}
		fsFileUtil::CreateFile(locator);
		m_pFile = new gfFileBin (locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
		m_pFile->WriteHeader();		
		m_pWriter = new chBinWriter (*m_pFile);
		m_pExportDoc->RegisterWriter( m_pWriter  );
	}

	ExportIntent::FileWriterLifeTimeKeeper::~FileWriterLifeTimeKeeper()
	{
		Cleanup();
	}

	void ExportIntent::FileWriterLifeTimeKeeper::Cleanup()
	{
		delete m_pWriter;
		delete m_pFile;
		m_pFile = NULL;
		m_pWriter = NULL;
		m_pExportDoc->UnregsiterWriter();
	}





	bool NodesToBeExported::IsExported( INode *  i_pCurNode )const
	{
		return ( find( i_pCurNode ) != end() );

	}
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
			if( OPTS.m_bExportParentIfChildExports )
			{
				//check to make sure that parent of a node is present
				if( !parent->IsRootNode() &&  ( fit = find( parent ) ) == end() )
				{
					bVal = false;
					break;
				}
			}
		}
		return bVal;
	}




	BaseExporter *ExportIntent::GetAppropriateExporter( INode *i_pCurNode )
	{
		std::string stypeName;
		MaxObjectType::TypeVal tval;
		return GetAppropriateExporter(i_pCurNode, stypeName, tval );
	}


	//=============================================================================
	// Cleanup the  book-keeping data structures
	//=============================================================================
	void ExportIntent::CleanUp()
	{	
		m_NodesToBeExported.clear();
		m_pRootNode = NULL;
		m_nNumNodesSoFarConsideredForExport =0;
	}


	//=============================================================================
	// Main public function that conducts the export
	//=============================================================================
	void ExportIntent::Do( const std::list<INode *> &suggestedNodes )
	{
		//do a clean up of the current book keeping
		CleanUp();

		Interface *maxInterface = GetCOREInterface();
		//cache the root node
		m_pRootNode = maxInterface->GetRootNode();
		assert( NULL != m_pRootNode );

		//_nodesToBeExported is filled in
		ComputeNodesToBeExported( suggestedNodes );

		//can we export safely
		bool bVal = PreExportValidation();
		if( !bVal)
		{
			EXPLOG.WriteError( "Export aborted!");
			throw export_failure();
		}

		//export the nodes
		ExportNodes();

	}










	//=============================================================================
	//  higher level function which evaluates what all nods need be exported
	//  see .hpp
	//=============================================================================
	size_t ExportIntent::ComputeNodesToBeExported( const list<INode *> &suggestedNodes )
	{
		assert( NULL != m_pRootNode );

		const MCHAR *szRootNode = m_pRootNode->GetName();


		hash_set<INode *> forcedNodes;
		//check all the children of the root node,
		//(this is recursive and fills up the m_NodesToBeExported, and forcedNodes)

		for (int j = 0; j < m_pRootNode->NumberOfChildren(); j++)
		{
			ComputeNodesToBeExportedRec( m_pRootNode->GetChildNode(j), suggestedNodes, forcedNodes);
		}


		//add the forced nodes alos into the list of nodes to be exported
		hash_set<INode*>::const_iterator fit;
		for( fit = forcedNodes.begin(); fit != forcedNodes.end(); ++fit )
		{
			INode *thisNodeAlsoNeedBeExported = *fit;
			const MCHAR * szThisNodeName = thisNodeAlsoNeedBeExported->GetName();
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
	bool  ExportIntent::ComputeNodesToBeExportedRec(
		INode * i_pCurNode, 
		const std::list<INode *> &i_SelectedNodes, 
		stdext::hash_set<INode *> &io_ForcedNodes
		)
	{
		const MCHAR *szCurNodeName = i_pCurNode->GetName();

		bool bOptionExportSelected = OPTS.m_bExportSelected;

		bool bCurNodeToBeExported = false;
		//if the export mode is by selection,
		if( bOptionExportSelected )
		{
			//check if the current node is cited as a selected node
			list<INode *>::const_iterator lit = find( i_SelectedNodes.begin(), i_SelectedNodes.end(), i_pCurNode);
			if( lit != i_SelectedNodes.end() )
			{
				bCurNodeToBeExported = true;
			}		

		} else //if the export mode is 'export all'
		{
			//then as long as it is not hidden it is selected
			bCurNodeToBeExported |= !i_pCurNode->IsHidden();
			//bCurNodeToBeExported |= true;
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
		bool exportChild = OPTS.m_bExportChildIfParentExports;
		//treat curNode as the 'child' node and its parent as the 'parent' node
		INode *parent = i_pCurNode->GetParentNode();
		bool forceThisDueToParent =  false;
		if( !bCurNodeToBeExported && //if the current node is not yet sleighted to be exported
			!i_pCurNode->IsHidden() && //if the current node is not hidden
			OPTS.m_bExportChildIfParentExports && // if the policy is such that children are exported when parents are
		 (
		 IsAncestorPresentInContainer( i_pCurNode,  m_NodesToBeExported.begin(), m_NodesToBeExported.end() ) || //if the parent is sleighted to be exported
		 IsAncestorPresentInContainer( i_pCurNode,  io_ForcedNodes.begin(), io_ForcedNodes.end() ) 	//or if the parent is present in the nodes to be exported 'forced'
		 )
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
			forceThisDueToChild |= ComputeNodesToBeExportedRec(i_pCurNode->GetChildNode(j), i_SelectedNodes, io_ForcedNodes);
		}


		if( !bCurNodeToBeExported  && // if curNode is selected to be exported in a straight manner
			!forceThisDueToParent &&  //if this node has not already been forced due to another policy
			forceThisDueToChild  && //if one of the children were exported
			OPTS.m_bExportParentIfChildExports //if the policy is such that parents get exported whenever a child is
			)
		{
			io_ForcedNodes.insert( i_pCurNode );
		}	
		bool bShouldThisNodeBeExported = bCurNodeToBeExported | forceThisDueToChild | forceThisDueToParent;		

		//Exporting this node should force some other dependent nodes
		//unrelated to this node by a ancestor-descendant relationship.
		//Eg: this node has an object ref which is a mesh
		//and that mesh is being modified by a skin modifier or a physique modifier.
		//If that is the case the bones should be exported
		//
		//Note that ComputeConnectedNodesToBeExportedRec
		//should not depend upon the logic used in this routine
		//to compute dependent nodes. 
		//Eg: If the bone hierarchy need be exported,
		//ComputeConnectedNodesToBeExportedRec should not just force the root bone,
		//assuming that the logic used in this routine would find its children
		if( bShouldThisNodeBeExported )
		{
			BaseExporter *pExporter = GetAppropriateExporter( i_pCurNode );
			pExporter->ComputeConnectedNodesToBeExported( i_pCurNode, io_ForcedNodes );
		}		

		return bShouldThisNodeBeExported;
	}


	void ExportIntent::ReportProgress( float i_fDelta )
	{
		if ( NULL !=  m_ProgressReport.get() )
		{
			m_ProgressReport->Update( i_fDelta );
		}
	}

	void ExportIntent::WriteExporterVersionStamp(chBinWriter &o_Writer)
	{
		char date_string[64];
		char time_string[64];
		_strdate(date_string);
		_strtime(time_string);

		// Write as strings so readable from bin viewer
		//
		o_Writer.WriteChunkHeader(c_EXPV, 0, false);
		o_Writer.Write(m_pExportDoc->GetExporterVersion());
		o_Writer.Write(date_string);
		o_Writer.Write(time_string);
		o_Writer.FinishChunk();

	}

	void ExportIntent::WriteFrameRateChunk( chBinWriter &io_Writer )
	{
		io_Writer.WriteChunkHeader(c_AFPS, 0, false);
		float fps = static_cast< float > ( GetFrameRate() );
		io_Writer.Write( fps );
		io_Writer.FinishChunk();
	}

	void ExportIntent::WriteBeginFrameChunk( chBinWriter &io_Writer, float i_fStartFrame )
	{
		io_Writer.WriteChunkHeader(c_BGFR, 0, false);
		io_Writer.Write( i_fStartFrame );
		io_Writer.FinishChunk();
	}
	void ExportIntent::ApplyYAxisUpXformCorrection(  shared_ptr<mdlNodeInfo> &rootMdlNode )
	{
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
	}

	void ExportIntent::ExtractUserDefinedProps( INode *i_pCurNode, shared_ptr< mdlNodeInfo >  &io_mdlNodeInfo )
	{

		if( NULL == io_mdlNodeInfo.get() )
		{
			return;
		}

		const MCHAR *szNodeName = i_pCurNode->GetName( );
		
		bool bDoubleSided;
		bool bRetVal =AllowedObjectFlags::GetValue( i_pCurNode, L"double sided" , bDoubleSided );
		DBG_ASSERT( bRetVal, "cant get the object flag : double sided" );
		bool bCastsShadow;		
		bRetVal =AllowedObjectFlags::GetValue( i_pCurNode, L"casts shadow" , bCastsShadow );		
		DBG_ASSERT( bRetVal, "cant get the object flag : casts shadow" );
		bool bReceivesShadow;		
		bRetVal =AllowedObjectFlags::GetValue( i_pCurNode, L"receives shadow" , bReceivesShadow );	
		DBG_ASSERT( bRetVal, "cant get the object flag : receives shadow" );
		
		bool bShadowHull;
		bRetVal =AllowedObjectFlags::GetValue( i_pCurNode, L"shadow hull" , bShadowHull );	
		DBG_ASSERT( bRetVal, "cant get the object flag : shadow hull" );

		bool bTriangleSort;
		bRetVal =AllowedObjectFlags::GetValue( i_pCurNode, L"triangle sort" , bTriangleSort );	
		DBG_ASSERT( bRetVal, "cant get the object flag : trianngle sort" );

		bool bCloth;		
		bRetVal =AllowedObjectFlags::GetValue( i_pCurNode, L"cloth" , bCloth );	
		DBG_ASSERT( bRetVal, "cant get the object flag : cloth" );

		bool bExportAsSubdiv;		
		bRetVal =AllowedObjectFlags::GetValue( i_pCurNode, L"export as subdiv" , bExportAsSubdiv );	
		DBG_ASSERT( bRetVal, "cant get the object flag : export as subdiv" );


		bool bVisibleAnim;		
		bRetVal =AllowedObjectFlags::GetValue( i_pCurNode, L"visible anim" , bVisibleAnim );	
		DBG_ASSERT( bRetVal, "cant get the object flag : visible anim" );

		//If no flag exists then lresVal = 0
		//If the flag exists and is set the lresVal = 1
		//If the flag exists and is not set then lresVal = 2
		int lresVal;
		bRetVal =AllowedObjectFlags::GetValue( i_pCurNode, L"low res" , lresVal );	
		DBG_ASSERT( bRetVal, "cant get the object flag : low res" );

		if( lresVal == 1  && bExportAsSubdiv )
		{
			EXPLOG.WriteError( "mesh flag conflict: %s, low resultion flag set and export as subdiv flag is set", szNodeName );
			lresVal = 0;
		}

		if( io_mdlNodeInfo->m_MeshInfo.get() )
		{	//set the flags for mesh
			shared_ptr< mdlFragInfo > &mdlMesh = io_mdlNodeInfo->m_MeshInfo;

			mdlMesh->m_Flags.m_bCastsShadow = bCastsShadow;
			mdlMesh->m_Flags.m_bReceivesShadow = bReceivesShadow;
			mdlMesh->m_Flags.m_bDoubleSided = bDoubleSided;
			mdlMesh->m_Flags.m_bShadowHull = bShadowHull;
			mdlMesh->m_Flags.m_bTriangleSort = bTriangleSort;
			mdlMesh->m_Flags.m_bVertexAnimation = bCloth;
			mdlMesh->m_ResolutionLevel = lresVal;
		} else if ( io_mdlNodeInfo->m_SubdivInfo.get() )
		{
			//set the flags for subdivinfo
			shared_ptr< mdlSubdivInfo > &mdlSubdiv = io_mdlNodeInfo->m_SubdivInfo;

			mdlSubdiv->m_Flags.m_bCastsShadow = bCastsShadow;
			mdlSubdiv->m_Flags.m_bReceivesShadow = bReceivesShadow;
			mdlSubdiv->m_Flags.m_bDoubleSided = bDoubleSided;
			mdlSubdiv->m_Flags.m_bShadowHull = bShadowHull;
			mdlSubdiv->m_Flags.m_bTriangleSort = bTriangleSort;
			mdlSubdiv->m_Flags.m_bAutoGenLowRes = (lresVal == 0);
		}
	}

} //maxExp