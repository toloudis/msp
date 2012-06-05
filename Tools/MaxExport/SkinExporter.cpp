/*****************************************************************************
**  MtlExporter.cpp
**
**	Exports materials
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "SkinExporter.hpp"

#include "MaxCommon.hpp"
#include "ExportDoc.hpp"
#ifndef MAXEXP_POLYORTRIMESH_HPP
#include "PolyOrTriMesh.hpp"
#endif
#include "MaxObjectFlags.hpp"

#include "cs/BipedApi.h"
#include "cs/Phyexp.h"
#include <algorithm>


using namespace std;
using namespace stdext;

namespace MaxExp
{

	/************************************************************************************
	// The following function has been taken from
	// maxsdk\howto\characterStudio\PhysExportSample.cpp


	// This function can be used to set the non-uniform scale of a biped.
	// The node argument should be a biped node.
	// If the scale argument is non-zero the non-uniform scale will be removed from the biped.
	// Remove the non-uniform scale before exporting biped nodes and animation data
	// If the scale argument is zero the non-uniform scaling will be reapplied to the biped.
	// Add the non-uniform scaling back on the biped before exporting skin data
	//***********************************************************************************/

	void ScaleBiped(INode* node, int scale)
	{
		if (node->IsRootNode()) return;

		// Use the class ID to check to see if we have a biped node
		Control* c = node->GetTMController();
		if ((c->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) ||
			(c->ClassID() == BIPBODY_CONTROL_CLASS_ID) ||
			(c->ClassID() == FOOTPRINT_CLASS_ID))
		{

			// Get the Biped Export Interface from the controller 
			IBipedExport *BipIface = (IBipedExport *) c->GetInterface(I_BIPINTERFACE);

			// Either remove the non-uniform scale from the biped, 
			// or add it back in depending on the boolean scale value
			BipIface->RemoveNonUniformScale(scale);
			ReferenceTarget* iMaster = (ReferenceTarget*) c->GetInterface(I_MASTER);
			iMaster->NotifyDependents(FOREVER, PART_TM, REFMSG_CHANGE);

			// Release the interfaces
			c->ReleaseInterface(I_MASTER,iMaster);
			c->ReleaseInterface(I_BIPINTERFACE,BipIface);
		}
	}


	SgpuPhysiqueInterface::SgpuPhysiqueInterface( ExportDoc &i_ExportDoc,  INode *i_pCurNode, TimeValue i_Time):
		m_pIMod( NULL ),
		m_pIContext( NULL ),
		m_pModifier( NULL),
		m_CurTime(i_Time),
		m_pCurNode( i_pCurNode),
		m_pExportDoc( &i_ExportDoc )
	{
		ResolveSkinModifier resSkin( i_ExportDoc, i_pCurNode, i_Time ); 
		if( !resSkin.Found() )
		{
			return;
		}
		//Acquire the IPhysiqueExport interface
		//and the context interface
		if( resSkin.GetModifier() )
		{
			m_pModifier = resSkin.GetModifier();
			m_pIMod = (IPhysiqueExport *)m_pModifier->GetInterface(I_PHYEXPORT);
		}			
		if( m_pIMod )
		{
			m_pIContext = (IPhyContextExport *)m_pIMod->GetContextInterface(m_pCurNode);
			if( NULL != m_pIContext )
			{
				//convert to rigid for time independent vertex assignment
				m_pIContext->ConvertToRigid(true);
				//allow blending to export multi-link assignments
				m_pIContext->AllowBlending(true);	
			}
		}	
	} 

	SgpuSkinModInterface::SgpuSkinModInterface( ExportDoc &i_ExportDoc,  INode *i_pCurNode, TimeValue i_Time):
		m_pIMod( NULL ),
		m_pIContext( NULL ),
		m_pModifier( NULL),
		m_CurTime(i_Time),
		m_pCurNode( i_pCurNode),
		m_pExportDoc( &i_ExportDoc )
	{
		ResolveSkinModifier resSkin( i_ExportDoc, i_pCurNode, i_Time ); 
		if( !resSkin.Found() )
		{
			return;
		}
		//Acquire the IPhysiqueExport interface
		//and the context interface
		if( resSkin.GetModifier() )
		{
			m_pModifier = resSkin.GetModifier();
			m_pIMod = (ISkin *)m_pModifier->GetInterface(I_SKIN);
		}			
		if( m_pIMod )
		{
			m_pIContext = (ISkinContextData *)m_pIMod->GetContextInterface(m_pCurNode);
			if( NULL != m_pIContext )
			{

			}
		}	
	} 


	SgpuSkinModInterface::~SgpuSkinModInterface()
	{			
		//Release the interfaces 
		if( m_pModifier && m_pIMod )
		{						
			m_pModifier->ReleaseInterface(I_SKIN, m_pIMod );
		}
	}



	bool SgpuSkinModInterface::Found() const
	{
		return m_pModifier != NULL;
	}

	SgpuPhysiqueInterface::~SgpuPhysiqueInterface()
	{			
		//Release the interfaces 
		if( m_pModifier && m_pIMod )
		{
			if( m_pIContext  )
			{
				m_pIMod->ReleaseContextInterface( m_pIContext );
			}				
			m_pModifier->ReleaseInterface(I_PHYINTERFACE, m_pIMod );
		}


	}

	bool SgpuPhysiqueInterface::Found() const
	{
		return m_pModifier != NULL;
	}
	
	template < class Cont >
	void SgpuSkinModInterface::GetBoneNodes( Cont &bones )
	{
		int nBones = m_pIMod->GetNumBones();
		for( int i=0; i < nBones; ++i )
		{
			INode *pBone = m_pIMod->GetBone( i );
			//It is possible that pBone is still some helper node
			//if this content has been through some export/import stages.
			//So insert only if it is a bone node
			MaxObjectType::TypeVal t = MaxObjectType::Get( pBone, OPTS );
			if ( t == MaxObjectType::Bone) 
			{
				bones.insert( pBone );
			}
		}
	}

	template< class Cont >
	struct CollectBoneNode
	{
		CollectBoneNode( Cont &bones):
		m_Bones(bones)
		{
		}
		void PreVertexVisit(int vIdx ) {}
		void PostVertexVisit( int vIdx ) {};
		void operator()(int x, int vIdx,  IPhyBlendedRigidVertex* rb_vtx )
		{			
			INode *bone = rb_vtx->GetNode(x);
			m_Bones.insert( bone );
		}

		void operator()( int vIdx, IPhyRigidVertex* r_vtx )
		{			
			INode *bone = r_vtx->GetNode();
			m_Bones.insert( bone );
		}

		
		void operator()(int x, int vIdx, IPhyFloatingVertex* f_vtx )
		{			
			INode *bone = f_vtx->GetNode(x);
			m_Bones.insert( bone );
		}
		
		void PostProcess()
		{

		}
		
		Cont &m_Bones;
	};


	template< typename E >
	void SgpuPhysiqueInterface::EnumerateBoneNodes( E &eop )
	{
		bool bPhysiquePresent = Found();
		if( !bPhysiquePresent )
		{
			return;
		}


		int i = 0, x = 0;
		//These are the different types of vertex classes 
		IPhyBlendedRigidVertex *rb_vtx;
		IPhyRigidVertex *r_vtx;
		IPhyFloatingVertex *f_vtx;


		//get the vertex count from the export interface
		int numverts = m_pIContext->GetNumberVertices();
#if defined(_DEBUG)
#define SGPU_DEBUG 1
#if SGPU_DEBUG
		{

			bool bExportAsSubdiv;
			bool bObjFlagRet = AllowedObjectFlags::GetValue( m_pCurNode, L"export as subdiv", bExportAsSubdiv);					
			DBG_ASSERT( bObjFlagRet, "cannot get export as subdiv flag!");
			bool bTriangulate = !bExportAsSubdiv;


			assert( m_pCurNode );
			ObjectRefResolver< ResolvePolicyBindPose >  objRefResolve( *m_pExportDoc,  m_pCurNode,static_cast<TimeValue>(OPTS.m_StartTime), bTriangulate );

			::Mesh maxMesh = objRefResolve.GetPolyOrTriMesh()->m_pTri->GetMesh();
			int nVerts = maxMesh.getNumVerts();
			if( numverts != nVerts )
			{
				EXPLOG.WriteError( "Eggreggious Error!, Physique Exp Interface not accounting for all vertices, %d versus %d", numverts, nVerts );
				throw export_failure();
			}
		}
#endif
#undef SGPU_DEBUG 
#endif 

		//iterate through all vertices and gather the bone list
		for (i = 0; i<numverts; i++) 
		{
			eop.PreVertexVisit( i );
			//get the hierarchial vertex interface
			IPhyVertexExport* vi = m_pIContext->GetVertexInterface(i);
			if (vi) {
				
				//check the vertex type and process accordingly
				int type = vi->GetVertexType();
				switch (type) 
				{
					//The vertex is rigid, blended vertex.  It's assigned to multiple links
				case RIGID_BLENDED_TYPE:
					//type-cast the node to the proper class		
					rb_vtx = (IPhyBlendedRigidVertex*)vi;

					//iterate through the bones assigned to this vertex
					for (x = 0; x<rb_vtx->GetNumberNodes(); x++) 
					{
						eop(x, i, rb_vtx );
					}
					break;
					//The vertex is a rigid vertex and only assigned to one link
				case RIGID_TYPE:
					//type-cast the node to the proper calss
					r_vtx = (IPhyRigidVertex*)vi;
					eop( i, r_vtx );
					break;

					// Shouldn't make it here because we converted to rigid earlier.  
					// It should be one of the above two types
				default: break;  
				}
			}
			m_pIContext->ReleaseVertexInterface(vi);
			// After gathering the bones from the rigid vertex interface
			// gather all floating bones if there are any 
			f_vtx = (IPhyFloatingVertex*)m_pIContext->GetFloatingVertexInterface(i);
			if (f_vtx) {	//We have a vertex assigned to a floating bone
				// iterate through the links assigned to this vertex
				for (x = 0; x<f_vtx->GetNumberNodes(); x++)
				{
					eop( x, i, f_vtx );
				}
			}
			m_pIContext->ReleaseVertexInterface( f_vtx );
			eop.PostVertexVisit( i );
		}

 		eop.PostProcess();

	}

	template < class Cont >
	void SgpuPhysiqueInterface::GetBoneNodes( Cont &bones )
	{
		CollectBoneNode< Cont > collect( bones );
		EnumerateBoneNodes( collect );
	}


	struct ExportBoneNode
	{
		typedef PhysiqueExporter::ExportOutput ExportOutput;
		typedef ExportOutput::BoneInflVector  BoneInflVector;
		typedef PhysiqueExporter::BoneInfluence BoneInfluence;

		ExportBoneNode(INode *i_pCurNode, ExportOutput  &o_Output):
			m_Output( o_Output ),
			m_TotalWeightForCurVertex(0),
			m_pCurNode( i_pCurNode ),
			m_NumMaxVertsConsideredSofar( 0 )
		{
		}

		void PreVertexVisit(int vIdx ) 
		{
			m_TotalWeightForCurVertex =0;
			m_FloatingVerticesForCurVertex.clear();
			++m_NumMaxVertsConsideredSofar;
		}
	
		void PostVertexVisit( int i_nVIdx )
		{
			if( EpsilonEqualZero(m_TotalWeightForCurVertex) || EpsilonEqualZero(m_TotalWeightForCurVertex - 1.0f) )
			{
				const MCHAR *szNodeName = m_pCurNode->GetName();
				EXPLOG.WriteWarning( "The %d th vertex of %s mesh has a total of less than 1.0 bone weight", i_nVIdx, MBCSTOLPCSTR( szNodeName ) );
				int i=0;
			}
			if ( m_FloatingVerticesForCurVertex.size() > 0)
			{
				ExportOutput::iterator mit;	
				typedef std::binder2nd<   NormalizeWeight > NormalizeWeightUn;
				NormalizeWeightUn normalizeWeight(  NormalizeWeight(), m_TotalWeightForCurVertex );
				//If the total weight for this vertex sofar is
				// less than or equal to 0.0f
				//just return
				if( EpsilonEqualZero( m_TotalWeightForCurVertex ) )
				{
					const MCHAR *szNodeName = m_pCurNode->GetName();
					EXPLOG.WriteWarning( "The %d th vertex of %s mesh has floating vertex weights and a total  weight of 0.0", i_nVIdx, MBCSTOLPCSTR( szNodeName ) );		
					assert( false );
					return;
				}
				for( mit = m_FloatingVerticesForCurVertex.begin(); mit != m_FloatingVerticesForCurVertex.end(); ++mit )
				{
					int vIdx = mit->first;
					assert( vIdx == i_nVIdx );
					BoneInflVector &bInflVector = mit->second;
					std::transform( bInflVector.begin(), bInflVector.end(), bInflVector.begin(), normalizeWeight ); 
				}

				for( mit = m_FloatingVerticesForCurVertex.begin(); mit != m_FloatingVerticesForCurVertex.end(); ++mit )
				{
					int vIdx = mit->first;				
					assert( vIdx == i_nVIdx );
					BoneInflVector &merged = m_Output[ vIdx ];
					const BoneInflVector &bInflVector = mit->second;
					merged.insert(merged.end(), bInflVector.begin(), bInflVector.end() );
				}
			}
		}

		void operator()( int x, int vIdx, IPhyBlendedRigidVertex* rb_vtx )
		{			
			INode *bone = rb_vtx->GetNode(x);
			ScaleBiped(bone, 0);
			float normalizedWeight;
			normalizedWeight = rb_vtx->GetWeight(x);
			m_TotalWeightForCurVertex += normalizedWeight;
			Point3 vtx = rb_vtx->GetOffsetVector( x );
			BoneInflVector &bInflVector =  m_Output[ vIdx ];
			bInflVector.push_back( BoneInfluence(bone, normalizedWeight )  );
			ScaleBiped( bone, 1);
		}

		void operator()( int vIdx, IPhyRigidVertex* r_vtx )
		{			
			INode *bone = r_vtx->GetNode();
			ScaleBiped(bone, 0);
			float normalizedWeight = 1.0f;
			Point3 vtx = r_vtx->GetOffsetVector( );
			m_TotalWeightForCurVertex += normalizedWeight;			
			BoneInflVector &bInflVector =  m_Output[vIdx];
			bInflVector.push_back( BoneInfluence(bone, normalizedWeight )  );
			ScaleBiped( bone, 1);
		}

		
		void operator()(int x, int vIdx,  IPhyFloatingVertex* f_vtx )
		{			
			INode *bone = f_vtx->GetNode(x);
			ScaleBiped(bone, 0);
			float actualWeight;
			f_vtx->GetWeight( x , actualWeight );
			m_TotalWeightForCurVertex += actualWeight;
			//we  need to dvide the  actual weight by
			//totalweight to find th normaklized weight
			Point3 vtx = f_vtx->GetOffsetVector( x );
			BoneInflVector &bInflVector = m_FloatingVerticesForCurVertex[ vIdx ];
			bInflVector.push_back( BoneInfluence( bone, actualWeight ) );
			ScaleBiped( bone, 1);
		}	

		struct NormalizeWeight: public std::binary_function< BoneInfluence, float, BoneInfluence >
		{
			BoneInfluence  & operator()(  BoneInfluence &b, float total  )const
			{
				b.second /= total;
				return b;
			}
		};


		void PostProcess()
		{
			//transfer this result at the end
			//to output
			m_Output.m_NumMaxVerts = m_NumMaxVertsConsideredSofar;		
		}
		INode *m_pCurNode;
		float m_TotalWeightForCurVertex;
		ExportOutput &m_Output;
		ExportOutput m_FloatingVerticesForCurVertex;
		//keep an account of the number of
		//3ds max vertices of this mesh considered so far
		int m_NumMaxVertsConsideredSofar;
	};

	/*
	void SgpuPhysiqueInterface::GetBoneNodes( hash_set< INode *> bones )
	{
		bool bPhysiquePresent = Found();
		if( !bPhysiquePresent )
		{
			return;
		}


		int i = 0, x = 0;
		INode* bone;
		//These are the different types of vertex classes 
		IPhyBlendedRigidVertex *rb_vtx;
		IPhyRigidVertex *r_vtx;
		IPhyFloatingVertex *f_vtx;


		//get the vertex count from the export interface
		int numverts = m_pIContext->GetNumberVertices();

		//iterate through all vertices and gather the bone list
		for (i = 0; i<numverts; i++) 
		{
			BOOL exists = false;

			//get the hierarchial vertex interface
			IPhyVertexExport* vi = m_pIContext->GetVertexInterface(i);
			if (vi) {

				//check the vertex type and process accordingly
				int type = vi->GetVertexType();
				switch (type) 
				{
					//The vertex is rigid, blended vertex.  It's assigned to multiple links
				case RIGID_BLENDED_TYPE:
					//type-cast the node to the proper class		
					rb_vtx = (IPhyBlendedRigidVertex*)vi;

					//iterate through the bones assigned to this vertex
					for (x = 0; x<rb_vtx->GetNumberNodes(); x++) 
					{
						exists = false;
						//get the node by index
						bone = rb_vtx->GetNode(x);
						bones.insert( bone );
					}
					break;
					//The vertex is a rigid vertex and only assigned to one link
				case RIGID_TYPE:
					//type-cast the node to the proper calss
					r_vtx = (IPhyRigidVertex*)vi;

					//get the node
					bone = r_vtx->GetNode();
					bones.insert( bone );
					break;

					// Shouldn't make it here because we converted to rigid earlier.  
					// It should be one of the above two types
				default: break;  
				}
			}
			m_pIContext->ReleaseVertexInterface(vi);
			// After gathering the bones from the rigid vertex interface
			// gather all floating bones if there are any 
			f_vtx = (IPhyFloatingVertex*)m_pIContext->GetFloatingVertexInterface(i);
			if (f_vtx) {	//We have a vertex assigned to a floating bone
				// iterate through the links assigned to this vertex
				for (x = 0; x<f_vtx->GetNumberNodes(); x++)
				{
					bone = f_vtx->GetNode(x);
					bones.insert( bone );
				}
			}
			m_pIContext->ReleaseVertexInterface( f_vtx );
		}
	}
*/
/*
typedef std::pair<iNode *, float> BoneInfluence;
		class ExportOutput: public std::map< int, std::vector< BoneInfluence > >
		{
			typedef std::vector< BoneInfulence > DataType;
			typedef std::map< int, DataType > BaseType;					
		};
*/

	void PhysiqueExporter::Export ( INode * i_pCurNode, ExportOutput &o_Output )
	{
		TimeValue startTime = OPTS.m_StartTime;
		SgpuPhysiqueInterface phyMod (*m_pExportDoc,  i_pCurNode, startTime );	
		bool bPhysiquePresent = phyMod.Found();
		if( !bPhysiquePresent )
		{
			return;
		}
		ExportBoneNode expBoneEnumStruct(i_pCurNode, o_Output );
		phyMod.EnumerateBoneNodes(  expBoneEnumStruct );
	}


	void  PhysiqueExporter::GetBonesAssociatedWithThisMesh( INode * i_pCurNode, INodeHS &io_Bones )
	{
		TimeValue startTime = OPTS.m_StartTime;
		SgpuPhysiqueInterface phyMod ( *m_pExportDoc, i_pCurNode , startTime );	
		bool bPhysiquePresent = phyMod.Found();
		if( !bPhysiquePresent )
		{
			return;
		}
		//get the physique version number.  
		//If the version number is > 30 you may have floating bones
		int ver = phyMod.m_pIMod->Version();

		//get the node's initial transformation matrix and store it in a matrix3
		Matrix3 initTM;
		int msg = phyMod.m_pIMod->GetInitNodeTM(i_pCurNode, initTM);
		phyMod.GetBoneNodes< INodeHS >( io_Bones  );	

	}

	void SkinModExporter::GetBonesAssociatedWithThisMesh( INode * i_pCurNode, INodeHS &io_Bones )
	{
		TimeValue startTime = OPTS.m_StartTime;
		SgpuSkinModInterface skinMod ( *m_pExportDoc, i_pCurNode , startTime );	
		bool bSkinModPresent = skinMod.Found();
		if( !bSkinModPresent )
		{
			return;
		}

		//get the node's initial transformation matrix and store it in a matrix3
		Matrix3 initTM;
		int msg = skinMod.m_pIMod->GetBoneInitTM(i_pCurNode, initTM);
		skinMod.GetBoneNodes< INodeHS >( io_Bones  );
	}

}// namespace MaxExp