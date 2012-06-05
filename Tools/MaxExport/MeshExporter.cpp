
/*****************************************************************************
**  MeshExporter.cpp
**
**	Exports nodes containing geometry (poly-mesh, tri-mesh, shapes)
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "MeshExporter.hpp"


#include "MaxCommon.hpp"
#ifndef MAXEXP_MAXEXPORTERUTILS_HPP
#include "MaxExportUtils.hpp"
#endif
#include "ExportDoc.hpp"
#include "ExportIntent.hpp"
#include "MtlExporter.hpp"
#include "MaxMeshUtils.hpp"
#include "SkinExporter.hpp"

#ifndef MAXEXP_POLYORTRIMESH_HPP
#include "PolyOrTriMesh.hpp"
#endif
#include "MaxObjectFlags.hpp"


#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
//#include "Graphics/mdl/mdlSkinUtil.hpp"
#include "Graphics/smdl/smdlCharacterSkin.hpp"

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

	

	//=============================================================================
	//Find the base object if there are modifiers
	//io_Object = is the current input object, and also th output base-object
	//o_Sid = supre class id of the base object
	//=============================================================================
	void GetBaseObjectAndID(Object*& io_Object, SClass_ID& o_Sid)
	{
		if (io_Object == NULL) return;

		o_Sid = io_Object->SuperClassID();
		if ( MaxObjectType::IsSuperClassDerivedObj( o_Sid ) )
		{
			IDerivedObject* derivedObject = (IDerivedObject*) io_Object;
			if (derivedObject->NumModifiers() > 0)
			{
				// Remember that 3dsMax has the mod stack reversed in its internal structures.
				// So that evaluating the zero'th modifier implies evaluating the whole modifier stack.
				ObjectState state = derivedObject->Eval(0, 0);
				io_Object = state.obj;
			}
			else
			{
				io_Object = derivedObject->GetObjRef();
			}
			o_Sid = io_Object->SuperClassID();
		}
	}

	


	void MeshExporter::CheckOnObject( INode  *i_pCurNode )
	{

		MCHAR *szNodeName = i_pCurNode->GetName(); 
		int dTicks = GetTicksPerFrame();
		TimeValue sampleTime;
		for( sampleTime = OPTS.m_StartTime; sampleTime <= OPTS.m_EndTime; sampleTime += OPTS.m_StepTime )
		{
			const ObjectState& objState = i_pCurNode->EvalWorldState( sampleTime );
			Object *obj = objState.obj;

			//get the poly or triangular representation
			PolyOrTriMesh polyOrTri( *i_pCurNode, m_pExportDoc, *obj, true );
			//well, right now we  support only the triangular representation
			if( !polyOrTri.IsTri() )
			{
				EXPLOG.WriteError( _T("cannot export node '%s'  because we dont have a triangulaation"), MBCSTOLPCSTR( szNodeName ) );
				throw export_failure();
			}			
			::Mesh& mesh = polyOrTri.m_pTri->GetMesh();
			int nVerts = mesh.getNumVerts();
			for( int iv =0; iv < nVerts; ++iv )
			{
				Point3 pos =  mesh.verts[ iv ];
				if( iv == 95 )
				{
				_RPT5( _CRT_WARN, "%s: %dth Vert: %g %g %g\n", szNodeName, iv, pos.x, pos.y, pos.z );
				}
			}
		}
	}



	//=============================================================================
	//Export the current node as a mesh
	// Try to get a triangulated mesh out of the 
	// object evaluated at startTime
	// i_pExportedRootNode, see BaseExporter.hpp for details
	//=============================================================================
	void MeshExporter::Export( 
		INode  *i_pCurNode, 
		ExportIntent &i_Intent, 				  
	    INode *i_pExportedRootNode,
		shared_ptr< mdlNodeInfo > & o_mdlNode )
	{
		MCHAR *szNodeName = i_pCurNode->GetName(); 
		bool bWorldSpace = OPTS.m_eExportIntent == eVertAnim ||  OPTS.m_eExportIntent ==  eParticle;
		bool bAddRemapInfo = OPTS.m_eExportIntent == eVertAnim ||  OPTS.m_eExportIntent ==  eParticle;
		ExportLogger::WriteStartElementWrap explogWrap( "exporting: %s", MBCSTOLPCSTR( szNodeName ) ); 
		bool bExportAsSubdiv = false;


		Matrix3 piv(1 );
		GetPivotTransform( i_pCurNode, piv );
		maMatrix4x4 sgpuPivot = SgpuConvert::Ma4x4( piv ); 

		TimeValue startTime = OPTS.m_StartTime;
		

		//Check the export as subdiv flag

		bool bObjFlagRet = AllowedObjectFlags::GetValue( i_pCurNode, L"export as subdiv", bExportAsSubdiv);					
		DBG_ASSERT( bObjFlagRet, "cannot get export as subdiv flag!");

			
		
		if ( bExportAsSubdiv )
		{
			bAddRemapInfo = true; //always add remap info for subdiv surfaces
			shared_ptr< SubdivisionExportCore  > exportCore;	
			exportCore.reset( new SubdivisionExportCore( *m_pExportDoc, *i_pCurNode, startTime, sgpuPivot, bAddRemapInfo ) );
			shared_ptr< mdlSubdivInfo  > subdivInfo;
			exportCore->DoExport( subdivInfo );

				if( NULL == subdivInfo.get() )
			{
				return;
			}

			assert( NULL !=  subdivInfo.get() );
			EXPLOG.WriteInfo("numVerts %d",  subdivInfo->m_Vertices.size() );
			EXPLOG.WriteInfo("numUVs %d", subdivInfo->m_UVs.size() );

			if( subdivInfo->m_Vertices.size() > 0 &&  subdivInfo->m_Indices.size() > 0 )
			{  
				o_mdlNode->m_SubdivInfo  = subdivInfo;
			} else
			{
				if( subdivInfo->m_Vertices.size() <= 0  )
				{
					EXPLOG.WriteError("subdiv %s not exported  because no vertices could be extracted from it", szNodeName ); 
				}

				if( subdivInfo->m_Indices.size() <= 0  )
				{
					EXPLOG.WriteError("subdiv %s not exported  because no face-indices could be extracted from it", szNodeName ); 
				}
			}	
		} else
		{

			shared_ptr< MeshGeomExportCore  > exportCore;	
			exportCore.reset( new MeshGeomExportCore(  *m_pExportDoc, *i_pCurNode, startTime, sgpuPivot, bAddRemapInfo  ) );
			shared_ptr< mdlFragInfo  > meshInfo;
			exportCore->DoExport(  meshInfo );   


			if( NULL == meshInfo.get() )
			{
				return;
			}

			assert( NULL !=  meshInfo.get() );
			EXPLOG.WriteInfo("numVerts %d", meshInfo->m_Vertices.size() );
			EXPLOG.WriteInfo("numNormals %d", meshInfo->m_Normals.size() );
			EXPLOG.WriteInfo("numUVs %d", meshInfo->m_UVs.size() );

			if( meshInfo->m_Vertices.size() > 0 &&  meshInfo->m_Indices.size() > 0 )
			{  
				o_mdlNode->m_MeshInfo  = meshInfo;
			} else
			{
				if( meshInfo->m_Vertices.size() <= 0  )
				{
					EXPLOG.WriteError("mesh %s not exported  because no vertices could be extracted from it", szNodeName ); 
				}

				if( meshInfo->m_Indices.size() <= 0  )
				{
					EXPLOG.WriteError("mesh %s not exported  because no face-indices could be extracted from it", szNodeName ); 
				}
			}	
		}

		i_Intent.ReportProgress( 1.0f );

	}

	 void MeshExporter::ExportSkinInfo( INode *i_pCurNode , const  INodeVector &i_AllBones, shared_ptr< mdlNodeInfo > & o_MdlNode )
	  {
		  shared_ptr<smdlCharacterSkin> skinInfo(new smdlCharacterSkin());	
		
		  shared_ptr< mdlFragInfo > mdlMeshInfo = o_MdlNode->m_MeshInfo;

		  if( NULL ==  mdlMeshInfo.get() )
		  {
			//nothing to export
		  }
		  bool bPhysiquePresent = false;

		  {
			  TimeValue startTime = OPTS.m_StartTime;
			  ResolveSkinModifier resSkin( *m_pExportDoc, i_pCurNode, startTime );
			  bPhysiquePresent = resSkin.Found();
		  }
		  if( bPhysiquePresent )
		  {
			  PhysiqueExporter *pPhysExp = m_pExportDoc->m_pPhysiqueExporter;
			  PhysiqueExporter::ExportOutput output ;
			  int numMdlVertices = mdlMeshInfo->m_Vertices.size();
			  std::multimap<int, int> &remappingInfo=mdlMeshInfo->m_VertexRemap;

			  pPhysExp->Export( i_pCurNode, output );


			  if( output.size() <= 0)
			  {
				  EXPLOG.WriteWarning( " mesh node %s has no skinning info associated with it, though it has a physique modifier", MBCSTOLPCSTR( i_pCurNode->GetName() ) );
				  return;		  

			  }

			  std::vector<smdlBoneVertex> &boneVertices = skinInfo->m_BoneVertices;
			  boneVertices.resize( numMdlVertices );
			  typedef std::multimap<int, int>::const_iterator MapIt;
			  PhysiqueExporter::ExportOutput::const_iterator cit;
			  for( cit = output.begin(); cit != output.end(); ++cit )
			  {
				  int vMaxIdx = cit->first;
				  assert( vMaxIdx < output.m_NumMaxVerts );
				  const PhysiqueExporter::ExportOutput::BoneInflVector & bVector = cit->second;			
				  PhysiqueExporter::ExportOutput::BoneInflVector::const_iterator bit;
				  for( bit = bVector.begin(); bit != bVector.end(); ++bit )
				  {
					  const PhysiqueExporter::BoneInfluence &bInfl = *bit;
					  const INode *boneNode = bInfl.first;
					  float fWeight = bInfl.second;
					  INodeVector::const_iterator bidit =  lower_bound( i_AllBones.begin(), i_AllBones.end(), boneNode );
					  if( bidit == i_AllBones.end() )
					  {
						INode *non_const_BoneNode = const_cast< INode * > ( boneNode );
						EXPLOG.WriteError( "bone %s mentioned in skiing info of node %s is not in allbones list", MBCSTOLPCSTR( non_const_BoneNode->GetName() ),  MBCSTOLPCSTR(  i_pCurNode->GetName() ) );
						throw export_failure();
					  }
					  size_t uboneIdx = std::distance( i_AllBones.begin(),  bidit );
					  int iBoneIdx = static_cast< int > ( uboneIdx );
					  pair<MapIt, MapIt> range = remappingInfo.equal_range( vMaxIdx );
					  while ( range.first != range.second )
					  {
						  assert( range.first->first == vMaxIdx );
						  int vMdlIdx = range.first->second;
						  smdlBoneVertex& boneVertex = boneVertices[ vMdlIdx];
						  scBoneInfluence boneInfl;
						  boneInfl.m_BoneIndex = iBoneIdx;
						  boneInfl.m_fWeight = fWeight;
						  boneVertex.m_Influences.push_back( boneInfl );
						  ++range.first;
					  }
				  }//for( bit = bVector.begin(); bit != bVector.end(); ++bit )
			  }	//for( cit = output.begin(); cit != output.end(); ++cit ) 

			  TimeValue startTime = OPTS.m_StartTime;
			  skinInfo->m_BindPose = SgpuConvert::Ma4x4( i_pCurNode->GetNodeTM( startTime ) );
			  o_MdlNode->m_SkinInfo = skinInfo;
		  }//if( bPhysiquePresent )
	  }

	 


	//=============================================================================
	//  Apply a  transformation (a rotation + translation ) to the mdlMesh
	//
	//  static
	//=============================================================================
	void MeshExporter::ApplyRTTransformationToMesh( const maMatrix4x4 &i_Tm, mdlFragInfo &io_MdlMeshInfo )
	{
		std::vector<maPoint3d>::iterator vit;
		float  det = i_Tm.GetDeterminant();
		if( det < 0.0f )
		{
			int i=0;
		}

		int i=0;
		for( vit =  io_MdlMeshInfo.m_Vertices.begin(); vit != io_MdlMeshInfo.m_Vertices.end(); ++vit , ++i)
		{
			maPoint3d &p = *vit;
			p = i_Tm * p;
			//_RPT3(_CRT_WARN, "%ef, %ef, %ef, \n", p.GetX(), p.GetY(), p.GetZ() );

		}
#if defined(_DEBUG)
#define SGPU_DEBUG 1
#if SGPU_DEBUG
#define VERTEX_IDX 95
			multimap<int,int>::iterator  pit = io_MdlMeshInfo.m_VertexRemap.find(VERTEX_IDX);
			if ( pit != io_MdlMeshInfo.m_VertexRemap.end() )
			{
				multimap<int, int>::iterator rit = pit;
				assert( VERTEX_IDX == rit->first );
				const maVector3d &p = io_MdlMeshInfo.m_Vertices[ rit->second ];				
				_RPT4( _CRT_WARN, "vert %d has pos %g %g %g\n", VERTEX_IDX, p[0], p[1], p[2] );
			}
#endif
#endif
		std::vector<maPoint3d>::iterator nit;
		for( nit = io_MdlMeshInfo.m_Normals.begin(); nit != io_MdlMeshInfo.m_Normals.end(); ++nit )
		{
			maVector3d &n = *nit;
			i_Tm.TransformDir(n);			
			//_RPT3(_CRT_WARN, "%ef, %ef, %ef, \n", n.GetX(), n.GetY(), n.GetZ() );
			//The correct way of transforming a normal n with an affine 
			//matrix T is 'transpose(inverse(T))*n', but T being just a  Rot followed by Trans
			//this translate to 'T*n'.  

		}

		for( size_t ui =0; ui < io_MdlMeshInfo.m_Indices.size(); ++ui )
		{
			int mi = io_MdlMeshInfo.m_Indices[ui];
			//_RPT1(_CRT_WARN, "%d, ", mi);
		}
		//_RPT0(_CRT_WARN, "\n" );
	}


	//=============================================================================
	//  If the input transform matrix transforms can transform the mesh 
	//	to a lefthanded co-ordinate system,
	//	then invert the vertex oder of triangles in the mesh
	//
	//  static
	//=============================================================================
	void MeshExporter::ApplyCorrectionToVertexOrderForPotentiallyLeftHandedGeometry( const maMatrix4x4 &i_Tm, mdlFragInfo &io_MdlMeshInfo )
	{
		std::vector<maPoint3d>::iterator vit;
		float  det = i_Tm.GetDeterminant();
		//if the determinant of the transformation matrix is 
		//negative, then if this transform i applied
		//to the mesh, the mesh will be transformed to a
		//left-handed co-ordinate system
		if( det < 0.0f )
		{
			int i=0;
		}
		//So the winding of the vertex order has to be compensated
		assert( io_MdlMeshInfo.m_Indices.size() % 3 == 0);
		if( det < 0.0f )
		{
			for( size_t ui =0; ui < io_MdlMeshInfo.m_Indices.size(); ui += 3 )
			{
				swap( io_MdlMeshInfo.m_Indices[ui+1], io_MdlMeshInfo.m_Indices[ui+2] );
			}
		}
	}
	//=============================================================================
	//  If the input transform matrix transforms can transform the subdiv 
	//	to a lefthanded co-ordinate system,
	//	then invert the vertex oder of triangles in the mesh
	//
	//  static
	//=============================================================================
	void MeshExporter::ApplyCorrectionToVertexOrderForPotentiallyLeftHandedGeometry( const maMatrix4x4 &i_Tm, mdlSubdivInfo &io_MdlSubdivInfo )
	{
		std::vector<maPoint3d>::iterator vit;
		float  det = i_Tm.GetDeterminant();
		//if the determinant of the transformation matrix is 
		//negative, then if this transform i applied
		//to the mesh, the mesh will be transformed to a
		//left-handed co-ordinate system
		if( det < 0.0f )
		{
			int i=0;
		}
		//So the winding of the vertex order has to be compensated
		assert( io_MdlSubdivInfo.m_Indices.size() > 0);
		int indexToNextFace = 0;
		if( det < 0.0f )
		{
			while( indexToNextFace < io_MdlSubdivInfo.m_Indices.size() )
			{
				int numSides = io_MdlSubdivInfo.m_Indices[ indexToNextFace ];
				assert( (indexToNextFace + numSides + 1 ) <= io_MdlSubdivInfo.m_Indices.size() );
				int nFirstIndex = indexToNextFace + 1;
				unsigned int *pFirstIndex = &( io_MdlSubdivInfo.m_Indices[ nFirstIndex ] );
				std::reverse( pFirstIndex, pFirstIndex + numSides );
				indexToNextFace += numSides + 1;
			}
		}
	}

	//=============================================================================
	//  Calls the  mdlFragInfo::CreateBasisVectors on the mesh
	//
	//  static
	//=============================================================================
	void MeshExporter::CreateBasisVectors( mdlFragInfo &io_MdlMeshInfo )
	{
		if( io_MdlMeshInfo.m_Vertices.size() > 0 &&  io_MdlMeshInfo.m_Indices.size() > 0 )
		{  
			mdlFragUtil::CreateBasisVectors( io_MdlMeshInfo );
		}
	}

	void  MeshExporter::ComputeConnectedNodesToBeExported( INode * i_pCurNode, stdext::hash_set< INode * > &io_ForcedNodes )
	{			
		INodeHS bonesAssociatedWithThisMesh;
		GetBonesAssociatedWithThisMesh( i_pCurNode, bonesAssociatedWithThisMesh );
		io_ForcedNodes.insert( bonesAssociatedWithThisMesh.begin(), bonesAssociatedWithThisMesh.end() );
	}



	void  MeshExporter::GetBonesAssociatedWithThisMesh( INode * i_pCurNode, INodeHS &o_BonesAssociatedWithThisMesh )
	{			
		bool bPhysiquePresent = false;
		INodeHS bonesAssociatedWithThisMeshTemp;
		TimeValue startTime = OPTS.m_StartTime;			  
		ResolveSkinModifier skinResolve(*m_pExportDoc, i_pCurNode, startTime );
		bPhysiquePresent = skinResolve.Found();
		
		if( bPhysiquePresent )
		{
			SkinBaseExporter *pSkinBase = skinResolve.GetSkinExporter();
			pSkinBase->GetBonesAssociatedWithThisMesh( i_pCurNode,  bonesAssociatedWithThisMeshTemp );
		}
		
		//filter out hidden bones
		vector< INode *> boneVectorContainer( bonesAssociatedWithThisMeshTemp.begin(), bonesAssociatedWithThisMeshTemp.end() );
		vector< INode * >::iterator new_end = remove_if( boneVectorContainer.begin(), boneVectorContainer.end(), HiddenNodeFilter() );
		o_BonesAssociatedWithThisMesh.insert( boneVectorContainer.begin(), new_end );
	}

} //namespace MaxExp