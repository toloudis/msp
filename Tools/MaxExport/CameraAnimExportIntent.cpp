/*****************************************************************************
**  CameraAnimExportIntent.cpp
**
**	Exports max nodes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "CameraAnimExportIntent.hpp"
#include "ExportDoc.hpp"
#include "MtlExporter.hpp"
#include "VertAnimExporter.hpp"
#include "MeshExporter.hpp"
#include "HelperExporter.hpp"
#include "MdlHierarchyVisit.hpp"
#include "CameraAnimExporter.hpp"

#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif 
#include "Core/ch/chBinWriter.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlWriter.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"

using namespace std;
using namespace stdext;

namespace MaxExp
{
	namespace
	{
		const chDefs::Name c_ACHR = chDefs::MakeName('A', 'C', 'H', 'R');
		const chDefs::Name c_AFPS = chDefs::MakeName('A', 'F', 'P', 'S');
		const chDefs::Name c_BGFR = chDefs::MakeName('B', 'G', 'F', 'R');

	}


	//=============================================================================
	// Cleanup the  book-keeping data structures
	//=============================================================================
	void CameraAnimExportIntent::CleanUp()
	{	
		ExportIntent::CleanUp();
	}

	void CameraAnimExportIntent::ExportPrefaceChunks( shared_ptr<mdlNodeInfo> &i_RootMdlNode )
	{
		

		chBinWriter &writer = *(m_pExportDoc->GetWriter());

		TimeValue startTime = OPTS.m_StartTime;
		TimeValue endTime = OPTS.m_EndTime;
		TimeValue stepTime = OPTS.m_StepTime;
		assert( stepTime > 0.0f);
		float startFrame = OPTS.m_fStartFrame;
		float endFrame = OPTS.m_fEndFrame;
		float stepFrame =  OPTS.m_fStepFrame;


		WriteFrameRateChunk( writer );
		WriteBeginFrameChunk( writer, startFrame );


	}

	void CameraAnimExportIntent::ExportEndChunks(shared_ptr<mdlNodeInfo> &i_RootMdlNode)
	{	

			chBinWriter &writer = *(m_pExportDoc->GetWriter());
			// Stamp version into end of file
			WriteExporterVersionStamp(writer);	
	}

	bool CameraAnimExportIntent::PreExportValidation()
	{
		
		float fTotalAnimBudget = 0;
		NodesToBeExported::const_iterator nit;
		for( nit = m_NodesToBeExported.begin(); nit != m_NodesToBeExported.end() ; ++nit )
		{
			INode *pNode = *nit;
			string stypeName;
			MaxObjectType::TypeVal tval;
			BaseExporter *baseExporter = GetAppropriateExporter( pNode, stypeName, tval );
			CameraAnimExporter *cameraAnimExporter = dynamic_cast< CameraAnimExporter *> (baseExporter );
			if( cameraAnimExporter )
			{
				float thisNodesBudget = cameraAnimExporter->EstimateAnimBudget( pNode );
				fTotalAnimBudget += thisNodesBudget;
			}
		}
		float fCameraAnimCache = static_cast<float>( CameraAnimExporter::m_nMaxCameraAnimCacheSize);
		if ( fTotalAnimBudget >=  fCameraAnimCache  )
		{
			EXPLOG.WriteError("Animantion export estimated size %g is byond %g limit",  fTotalAnimBudget,  fCameraAnimCache );
			return false;
		}

		//setting the progress report
		//total bytes exported for animation times two + number of nodes exported
		//total bytes exported for animation is multiplied by two because
		//progress for vertiex animation is reported twice, once on export and once on writing
		float fProgressReportBudget = fTotalAnimBudget * 2 + static_cast< float > ( m_NodesToBeExported.size() );
		m_ProgressReport.reset( new ProgressReport( fProgressReportBudget  ) ) ;
		return true;
	}	

	BaseExporter *CameraAnimExportIntent::GetAppropriateExporter( INode *i_pCurNode, string &stypeName, MaxObjectType::TypeVal &objType )
	{
		BaseExporter *retVal = NULL;
		MaxObjectType::Get( i_pCurNode, OPTS, stypeName, objType );
		switch( objType )
		{
		case MaxObjectType::Mesh: 
			retVal = m_pExportDoc->m_pTrivialExporter;
			break;
		case MaxObjectType::Helper:
			retVal = m_pExportDoc->m_pTrivialExporter;
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


	//=============================================================================
	// export the nodes that are already evaluated to be exported
	//=============================================================================
	void CameraAnimExportIntent::ExportNodes()
	{
		assert( NULL != m_pRootNode );
		
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
		
	

		shared_ptr<mdlNodeInfo> rootMdlNode( new mdlNodeInfo );
		
		ExportPrefaceChunks( rootMdlNode );

		rootMdlNode->m_NodeName = MbcsToUtf8( m_pRootNode->GetName() );		
		maMatrix4x4 parentCumulativeTransform = rootMdlNode->m_Transform;
		for (int j = 0; j < m_pRootNode->NumberOfChildren(); j++)
		{		
			INode *pExportedRootNode = NULL;
			ExportNodesRec( 
				m_pRootNode->GetChildNode(j),
				pExportedRootNode,
				rootMdlNode );	
		}
		
		

		PostProcess( rootMdlNode );		
		ExportEndChunks( rootMdlNode );
	}


	//=============================================================================
	// recursive exporting function called by ExportNodes()
	// 
	//=============================================================================
	void CameraAnimExportIntent::ExportNodesRec( 
		INode *i_pCurNode,  
		INode *& io_pExportedRootNode,
		shared_ptr<mdlNodeInfo> &io_ParentMdlNode )
	{	
		
		MCHAR *szNodeName = i_pCurNode->GetName();
		BaseExporter *baseExporter = NULL;
		shared_ptr<mdlNodeInfo> nodeToBePassedToChildren(io_ParentMdlNode) ;
		if( m_NodesToBeExported.find( i_pCurNode) != m_NodesToBeExported.end() )
		{	
			try
			{	
				shared_ptr<mdlNodeInfo> curMdlNode;


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
				
		


				//core export process

				MaxObjectType::TypeVal objType;
				string  sTypeName;
				baseExporter =  GetAppropriateExporter( i_pCurNode, sTypeName, objType );
				baseExporter->Export( 
					i_pCurNode, 
					*this, 
					io_pExportedRootNode,
					curMdlNode 
					);



				ExtractUserDefinedProps( i_pCurNode, curMdlNode );

				MeshExporter *pMeshExporter = dynamic_cast< MeshExporter * >( baseExporter );
				if( NULL != pMeshExporter )
				{

					shared_ptr<smdlCharacterSkin> skin_info(new smdlCharacterSkin());	
					curMdlNode->m_SkinInfo = skin_info;

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



			CameraAnimExporter *pCameraAnim = dynamic_cast< CameraAnimExporter * >(baseExporter);
			if( pCameraAnim != NULL )
			{
				chBinWriter &writer = *( m_pExportDoc->GetWriter() );	
				mdlCameraAnimWriter::CameraKeys * cameraExportData = pCameraAnim->m_CameraExportData.get();
				if( NULL != cameraExportData )
				{					
					float startFrame = OPTS.m_fStartFrame;
					std::string sCameraName = SgpuConvert::NodeName( szNodeName );
					CameraAnimExporter::ReportProgress rprogress( *this );
					mdlCameraAnimWriter::WriteCameraAnim ( writer,  rprogress, sCameraName, *cameraExportData, startFrame   ); 
				}		
			}
		}//if( m_NodesToBeExported.find( i_pCurNode) != m_NodesToBeExported.end() )

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
#if DELME
	void CameraAnimExportIntent::PreExportBookKeeping()
	{

		//What we are trying to do is to get an ordered
		//list of all bones ever mentioned in skinning of any
		//meshes considered for export.
		//The bones should be ordered according to the depth
		//first traversal of the bone hierarchy(ies) involved.


		//First let us collect all bones associated with
		//all exported meshes into a single container
		AssociatedBonesCollectVisit collectBones( *this );
		VisitINodeHierarchyRec( m_pRootNode, collectBones );


		//Now, having done that, let us find all root bone nodes
		//Note that there can be more than one root bone node
		AssociatedBonesCollectVisit::Cont::const_iterator bit;
		FindRootBoneOfThisHierarchy findRoot;
		INodeHS rootBones;
		for( bit = collectBones.m_BonesAssociatedWithAllMeshes.begin();
			bit != collectBones.m_BonesAssociatedWithAllMeshes.end();
			++bit
			)
		{
			INode *pBone = *bit;
			INode *pRoot = findRoot( pBone );
			const MCHAR *szRRoot = MBCSTOLPCSTR( pRoot->GetName() );
			rootBones.insert( pRoot );
		}
		

		//For each root bone,
		//do a dpth first traversal and 
		//collect all bones in that hierarchy into
		//a list. 
		//Not that we cannot use this list as our
		//objectives since many of the bones in this list
		//wont be used.
		INodeHS::const_iterator rit;
		std::list< INode *> allBones;
		for(rit = rootBones.begin(); rit != rootBones.end(); ++rit )
		{
			DepthFirstBoneHierarchyTraversal dfirst( *rit );
			VisitINodeHierarchyRec( *rit, dfirst);
			allBones.insert( allBones.end(), dfirst.m_BonesOfThisHierarchy.begin(),  dfirst.m_BonesOfThisHierarchy.end() );
		}
		

		//make enough space for the ordered list
		//of relavant bones
		m_BonesAssociatedWithAllMeshes.clear();
		m_BonesAssociatedWithAllMeshes.reserve( collectBones.m_BonesAssociatedWithAllMeshes.size() );

		//go through the list,
		//and select only the bones which are associated with
		//skinned exported meshes
		std::list< INode *>::iterator lit;
		for( lit = allBones.begin(); lit != allBones.end(); ++lit )
		{
			INode *pNode = *lit;
			if( collectBones.m_BonesAssociatedWithAllMeshes.find( pNode ) !=
				collectBones.m_BonesAssociatedWithAllMeshes.end()
				)
			{
				m_BonesAssociatedWithAllMeshes.push_back( pNode );
			}
		}

#if defined(_DEBUG)
#define SGPU_DEBUG 1
#if SGPU_DEBUG
		for_each( m_BonesAssociatedWithAllMeshes.begin(),  m_BonesAssociatedWithAllMeshes.end(), DbgPrintINodeName() );

		INodeVector::const_iterator vit;
		LessINode lessINode;
		for( vit = m_BonesAssociatedWithAllMeshes.begin(); vit != m_BonesAssociatedWithAllMeshes.end(); ++vit )
		{
			const INode *p1 = *vit;
			INodeVector::const_iterator vit2 = vit;
			++vit2;
			for( ; vit2 < m_BonesAssociatedWithAllMeshes.end() ; ++vit2 )
			{
				const INode *p2 = *vit2;			
				assert( !lessINode( p2, p1 ) );
			}
		}
#endif
#undef SGPU_DEBUG
#endif

	}


	void CameraAnimExportIntent::PostProcess( shared_ptr<mdlNodeInfo> &rootMdlNode)
	{
		if ( !OPTS.m_bExportGeom  )
		{
			
			SetTransformIdentity setTransformIdentity;
			VisitMdlHierarchyRec( rootMdlNode, setTransformIdentity );
			return;
		}
		ApplyYAxisUpXformCorrection( rootMdlNode );
	
		CorrectLeftHandednessVisit leftHandednessCorrection;
		VisitMdlHierarchyRec( rootMdlNode, leftHandednessCorrection );

		FlattenVisit flatten;		
		VisitMdlHierarchyRec( rootMdlNode, flatten );

		CreateBasisVectorsVisit cbvVisit;
		VisitMdlHierarchyRec( rootMdlNode, cbvVisit );
	}


	void CameraAnimExportIntent::ExtractUserDefinedProps( INode *i_pCurNode, shared_ptr< mdlNodeInfo >  &io_mdlNodeInfo )
	{
		ExportIntent::ExtractUserDefinedProps( i_pCurNode, io_mdlNodeInfo );
		assert( OPTS.m_eExportIntent == ExportDoc::eVertAnim );
		if( io_mdlNodeInfo.get() && io_mdlNodeInfo->m_MeshInfo.get() )
		{
			io_mdlNodeInfo->m_MeshInfo->m_Flags.m_bVertexAnimation = true;
		}
	}
#endif


} //maxE