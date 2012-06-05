/*****************************************************************************
**  MaxMeshUtils.cpp
**
**	Collection of  utility functions and classes, 
**	related to meshes that glues Sgpu api and max api
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "MaxMeshUtils.hpp"


#include "MtlExporter.hpp"
#include "ExportDoc.hpp"
#include "HashSetPoint3.hpp"

#ifndef MAXEXP_POLYORTRIMESH_HPP
#include "PolyOrTriMesh.hpp"
#endif
#ifndef MAXEXP_MAXEXPORTERUTILS_HPP
#include "MaxExportUtils.hpp"
#endif
#ifndef MAXEXP_EXPORTINTENT_HPP
#include "ExportIntent.hpp"
#endif
#include "MeshExporter.hpp"
#include "SkinExporter.hpp"
#include "MaxObjectFlags.hpp"

#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif 
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#include "Graphics/vtx/vtxVertexFrame.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
//max include files
#include "MaxCommon.hpp"
#include "stdmat.h"
#include "buildver.h"
#include "shaders.h"
#include "XRef/iXrefObj.h"
#include "XRef/iXrefMaterial.h"
#include "CS/Bipexp.h"
#include "mnMesh.h"
#include "ipointcache.h"

#include <cstdio>
#include <string>
#include <algorithm>
#include <hash_set>
#include <crtdbg.h>
#include <set>

using namespace std;
using namespace stdext;

namespace MaxExp
{ 
	const chDefs::Name c_MCHR = chDefs::MakeName('M', 'C', 'H', 'R');
	const chDefs::Name c_SKIN = chDefs::MakeName('S', 'K', 'I', 'N');
	const chDefs::Name c_ACHR = chDefs::MakeName('A', 'C', 'H', 'R');
	const chDefs::Name c_DELT = chDefs::MakeName('D', 'E', 'L', 'T');


	struct UniqueVertex
	{
		UniqueVertex() :	m_NormalIndex(-1),
			m_UVIndex(-1),
			m_PosIndex(-1), 
			m_Index(-1){}

		UniqueVertex(	int i_PosIndex,
			int i_NormalIndex,
			int i_UVIndex, int i_Index) :	m_PosIndex(i_PosIndex),
			m_NormalIndex(i_NormalIndex),
			m_UVIndex(i_UVIndex), m_Index(i_Index){}

		bool operator == (const UniqueVertex& rhs) const 
		{	
			return	(m_NormalIndex == rhs.m_NormalIndex) &&
				(m_UVIndex ==  rhs.m_UVIndex) &&
				(m_PosIndex == rhs.m_PosIndex); 
		}

		bool operator < (const UniqueVertex& rhs) const
		{
			if( m_PosIndex == rhs.m_PosIndex )
			{
				if( m_NormalIndex == rhs.m_NormalIndex )
					return m_UVIndex < rhs.m_UVIndex;
				else
					return m_NormalIndex < rhs.m_NormalIndex;
			}
			else
				return m_PosIndex < rhs.m_PosIndex;
		}

		int m_NormalIndex;
		int m_UVIndex;
		int m_PosIndex;
		int m_Index;
	};

	//To ease the use of UniqueVertex for hash_set
	//we need to be able to  construct a hash_key from a Point3
	//This derived class accomplishes that.

	//This enables us to define 
	// hash_set<UniqueVertex, hash_compare_UniqueVertex > 
	class Hash_Compare_UniqueVertex : public stdext::hash_compare< UniqueVertex, less< UniqueVertex> >, private StdLibHashAlgorithm
	{
	public:
		size_t operator()(const UniqueVertex& v) const
		{	
			unsigned int f[4];
			f[0] = v.m_PosIndex; f[1] = v.m_NormalIndex; f[2] = v.m_UVIndex;
			unsigned char * fAsUCharPtr = reinterpret_cast< unsigned char * >(f);
			size_t hash = hash_value( fAsUCharPtr );
			return hash;		
		}

		bool operator()(const UniqueVertex &K1, const UniqueVertex &K2) const
		{	
			return _lessP( K1, K2);
		}

		less<UniqueVertex> _lessP; 
	};



	void ResolveModifier::Init()
	{
		assert( NULL != m_pCurNode );
		Object *obj = m_pCurNode->GetObjectRef();
		if(  NULL == obj )
		{
			EXPLOG.WriteError(_T("node '%s' has null obj!"), MBCSTOLPCSTR( m_pCurNode->GetName() ));
			return;
		} 	

		assert( obj != NULL);
		MCHAR *szNodename = m_pCurNode->GetName();
		Class_ID id = obj->ClassID();
		SClass_ID sid = obj->SuperClassID();
		IDerivedObject* dobj = NULL;
		dobj = static_cast< IDerivedObject *>( obj);
		while ( MaxObjectType::IsSuperClassDerivedObj( sid ) )
		{				
			for (int j = 0; j < dobj->NumModifiers(); ++j)
			{
				Modifier* mod = dobj->GetModifier(j);

				MatchCont::iterator mit;
				bool  bVal = false;
				//If any of  the modofier is a point cache,
				// then just return

				if ( m_PointCacheMatch( mod )  )
				{
					return;
				}
				for( mit =  m_Matchers.begin(); !bVal &&  mit != m_Matchers.end(); ++mit )
				{
					shared_ptr< MatchBase > &match = *mit;
					bVal = (*match)( mod );
					if( bVal )
					{
						m_pIDerivedObject = dobj;
						m_iModStackIdx = j;
						m_pModifier = mod;
						return;
					}
				}
			}
			dobj = static_cast<IDerivedObject*> ( dobj->GetObjRef() );
			sid = dobj->SuperClassID();
		}
		return;		
	}

	template< >
	bool ResolveModifier::Match< SKIN_CLASS_ID_A, SKIN_CLASS_ID_B >::operator()( Modifier *i_pModifier )const
	{
		bool bVal = (i_pModifier->ClassID() == m_TheClassID ) ? true : false;
		if( bVal )
		{
			ISkin *pSkin = static_cast< ISkin * > ( i_pModifier->GetInterface( I_SKIN  ) );
			return ( pSkin && pSkin->GetNumBonesFlat() > 0 );
		} else
		{
			return false;
		}
	}	

	ResolveSkinModifier::ResolveSkinModifier(ExportDoc &i_ExportDoc, INode *i_pCurNode, TimeValue i_Time):
	ResolveModifier( i_pCurNode, i_Time ),
		m_pExportDoc( &i_ExportDoc ) 
	{
		shared_ptr< MatchBase > p1, p2;
		p1.reset( new PhysiqueMatch() );
		m_Matchers.push_back( p1 );
		p2.reset( new SkinMatch() );
		m_Matchers.push_back( p2 );
		Init();
	}

	SkinBaseExporter *ResolveSkinModifier::GetSkinExporter()const
	{
		SkinBaseExporter *pSkinExporter = NULL;
		bool bFound = Found();
		if( bFound )
		{
			PhysiqueMatch phy;
			if ( phy( m_pModifier ) )
			{
				pSkinExporter =  m_pExportDoc->m_pPhysiqueExporter;
			}
			SkinMatch skMatch;
			if ( skMatch( m_pModifier ) )
			{
				pSkinExporter = m_pExportDoc->m_pSkinModExporter;				
			}

			if( NULL == pSkinExporter )
			{
				Class_ID cid = m_pModifier->ClassID();
				EXPLOG.WriteError( "Unsupported skin modifier of class (%x, %x), name %s", cid.PartA(), cid.PartB(), MBCSTOLPCSTR( m_pModifier->GetName() ) );
				throw export_failure();
			}
		}

		return pSkinExporter;
	}


	Object * ResolvePolicyBindPose::Do( INode * i_pCurNode, ResolveModifier &detect, TimeValue i_Time )
	{
		Object *pRetMaxObject = NULL;
		if( detect.Found() )
		{
			int nIdxOfThisModInDerObject = detect.GetModStackIdx();
			IDerivedObject *pDerObj = detect.GetIDerivedObject();

			int nModsForThisDerObject =  pDerObj->NumModifiers();
			if( nIdxOfThisModInDerObject   < (nModsForThisDerObject - 1) )
			{
				//unless the mod idx ( ie: nIdxOfThisModInDerObject )
				//is for the bottom most modifier in the stack
				//for this desired object
				//get the object-ref below  the derived object
				ObjectState state = pDerObj->Eval( i_Time,  nIdxOfThisModInDerObject  + 1);	
				pRetMaxObject = state.obj;
			} else
			{
				//if the mod idx ( ie: nIdxOfThisModInDerObject )
				//is for the bottom most modifier in the stack
				//for this derived object
				//get the base object ref
				pRetMaxObject = pDerObj->GetObjRef();
			}
		} else
		{
			ObjectState state = i_pCurNode->EvalWorldState( i_Time );
			pRetMaxObject  = state.obj;
		}
		return pRetMaxObject;
	}

	PolyOrTriMesh * ResolvePolicyBindPose::MakePolyOrTriMesh( INode &i_CurNode, ExportDoc *i_pExportDoc, Object &i_MaxObject, bool i_bTriangulate )
	{
		return new PolyOrTriMesh(i_CurNode, i_pExportDoc, i_MaxObject , i_bTriangulate );
	}



	Object *  ResolvePolicyVertAnim::Do( INode * i_pCurNode, ResolveModifier &detect, TimeValue i_Time )
	{
		Object *pRetMaxObject = NULL;
		if( detect.Found() )
		{

			int nIdxOfThisModInDerObject = detect.GetModStackIdx(); 
			IDerivedObject *pDerObj = detect.GetIDerivedObject();
			int idx = detect.GetModStackIdx();
			ObjectState state = pDerObj->Eval( i_Time, nIdxOfThisModInDerObject );	
			pRetMaxObject = state.obj;
		} else
		{
			ObjectState state = i_pCurNode->EvalWorldState( i_Time );
			pRetMaxObject  = state.obj;
		}
		return pRetMaxObject;
	}

	PolyOrTriMesh * ResolvePolicyVertAnim::MakePolyOrTriMesh( INode &i_CurNode,ExportDoc *i_pExportDoc, Object &i_MaxObject, bool i_bTriangulate )
	{
		return new PolyOrTriMesh( i_CurNode, i_pExportDoc, i_MaxObject , i_bTriangulate );
	}

	void AssociatedBonesCollectVisit::Pre( INode  *i_pCurNode )
	{
		//If this is t be exported as a mesh
		//the collect the associated bone nodes into
		// m_BonesAssociatedWithExportedMeshes
		if( m_ExportIntent.IsExported( i_pCurNode ) )
		{
			BaseExporter *pExporter = m_ExportIntent.GetAppropriateExporter( i_pCurNode );
			MeshExporter *pMeshExp = dynamic_cast< MeshExporter * > ( pExporter );
			if( pMeshExp )
			{
				pMeshExp->GetBonesAssociatedWithThisMesh( i_pCurNode,  m_BonesAssociatedWithAllMeshes );				
			}
		}
	}


	bool FindRootBoneOfThisHierarchy::IsBone::operator()( INode * i_pCurNode, SgpuExportOptions &i_Options, bool i_bNegate )
	{			
		MaxObjectType::TypeVal t = MaxObjectType::Get(i_pCurNode, i_Options );
		return  (i_bNegate ) ? ( t != MaxObjectType::Bone) : ( t == MaxObjectType::Bone);			
	};


	INode * FindRootBoneOfThisHierarchy::operator()( INode * i_pCurBone ) 
	{
		RootBoneCacheType::const_iterator rit = m_RootBoneCache.find( i_pCurBone );
		const MCHAR *szNode = i_pCurBone->GetName();
		if( rit != m_RootBoneCache.end() )
		{
			INode *pRoot = rit->second;
			assert( pRoot );
			return pRoot;
		}

		INode * pBone = i_pCurBone;
		INode * pPrevBone = NULL;
		assert( NULL !=  pBone  );
		IsBone isBone;
		MaxObjectType::TypeVal tpBone = MaxObjectType::Get(pBone, OPTS );
		assert( tpBone == MaxObjectType::Bone );
		bool bNegate = false;
		std::deque< INode *> nodesThatShareThisRootBone;
		nodesThatShareThisRootBone.push_back( pBone );
		//Find the node which is a bone and which is
		//the highest in the hierarchy
		int maxIter = 1000000;
		do
		{
			do
			{
				//if the loop runs too much
				//throws an exception
				if( --maxIter <= 0)
				{
					EXPLOG.WriteError("cannot find root bone of %s", MBCSTOLPCSTR( i_pCurBone->GetName() ) );
					throw export_failure();
				}
				if( !bNegate )
				{
					pPrevBone = pBone;
				}
				pBone = pBone->GetParentNode();	
				//since this pBone alsoshould be in the same
				//bone hierarchy, we can cache this node
				if( NULL != pBone && isBone( pBone, OPTS, false  ) )
				{
					nodesThatShareThisRootBone.push_back( pBone );
				}
				if( pBone  != NULL)
				{
					const MCHAR *szPBone = pBone->GetName();
					int i=0;
				}
			} while ( pBone != NULL && isBone( pBone, OPTS, bNegate ) );
			assert( pPrevBone != NULL );
			if( NULL != pBone ) 
			{
				bNegate = (bNegate ) ? false  : true;
			} else
			{
				break;
			}
		} while ( true );

		MaxObjectType::TypeVal tprevBone = MaxObjectType::Get(pPrevBone, OPTS );
		assert( tprevBone == MaxObjectType::Bone );	

		//insert all bones that we encountered,
		//that share this root bone into the bone cache
		std::deque<INode *>::iterator dit;
		for( dit =  nodesThatShareThisRootBone.begin(); dit != nodesThatShareThisRootBone.end(); ++dit )
		{
			m_RootBoneCache.insert( pair<INode*,INode*>( *dit, pPrevBone ) );
		}

		return pPrevBone;
	}

	void DepthFirstBoneHierarchyTraversal::Pre( INode  *i_pCurNode )
	{
		MaxObjectType::TypeVal t = MaxObjectType::Get(i_pCurNode, OPTS );
		if ( t == MaxObjectType::Bone )
		{
			m_BonesOfThisHierarchy.push_back( i_pCurNode );
		}			
	}


	//=============================================================================
	// In 3dsMax,  you can specify  the normals attached  to a face-vertex by means
	//  of a EditNormal modifier. THe following routine, detects the presence of
	//  such normals 
	//static
	//=============================================================================
	template< class ResolvePolicy >
	bool MeshBaseExportCore< ResolvePolicy >::CheckUserSpecifiedNormals( ::Mesh & mesh )
	{
		MeshNormalSpec *meshNormalSpec = mesh.GetSpecifiedNormals();
		return ( meshNormalSpec != NULL && meshNormalSpec->GetNumNormals() >= 0 );
	}





	MeshGeomExportCore::MeshGeomExportCore(  
		ExportDoc &exportDoc, 
		INode &i_CurNode, 
		TimeValue i_ExportTime,			
		const maMatrix4x4 &i_GeometryTransform,
		bool i_bAddRemapInfo
		):
	MeshBaseExportCore( 
		exportDoc, 
		i_CurNode, 
		i_ExportTime, 
		i_GeometryTransform, 
		true //i_bTriangulate
		),
		m_bAddRemapInfo( i_bAddRemapInfo ),
		m_bPackNormals( true )
	{	


		//If we add remapp info to our exported mesh,
		//then we cannot use pack normals,
		//because the remap indices would be invalid
		m_bPackNormals =  !m_bAddRemapInfo;

	}

	SubdivisionExportCore::SubdivisionExportCore(  
		ExportDoc &exportDoc, 
		INode &i_CurNode, 
		TimeValue i_ExportTime,			
		const maMatrix4x4 &i_GeometryTransform,
		bool i_bAddRemapInfo
		):
	MeshBaseExportCore( 
		exportDoc, 
		i_CurNode, 
		i_ExportTime, 
		i_GeometryTransform,
		false //i_bTriangulate
		),
		m_bAddRemapInfo( i_bAddRemapInfo )
	{	
	}



	MeshAnimExportCore::MeshAnimExportCore(  
		ExportDoc &exportDoc, 
		INode &i_CurNode, 
		TimeValue i_ExportTime,		
		const maMatrix4x4 &i_GeometryTransform,
		bool bTriangulate
		):
	MeshBaseExportCore( 
		exportDoc, 
		i_CurNode, 
		i_ExportTime, 
		i_GeometryTransform,
		bTriangulate)
	{	
	}

	template< class ResolvePolicy >
	void MeshBaseExportCore< ResolvePolicy >::GetMtlIdFaceMap(   MtlIdFaceListMapT &o_MtlIdFaceListMap )
	{
		Mtl *nodeMtl = m_pCurNode->GetMtl();	
		size_t nSubMtls = 1;
		if( nodeMtl && nodeMtl->IsMultiMtl() )
		{
			nSubMtls = nodeMtl->NumSubMtls();
		}
		if( NULL !=  m_ObjResolver->GetPolyOrTriMesh()->m_pTri )
		{
			::Mesh &mesh = m_ObjResolver->GetPolyOrTriMesh()->m_pTri->GetMesh();


			int nFaces = mesh.getNumFaces();
			int nVerts = mesh.getNumVerts();

			for (int i = 0; i < nFaces; ++i) 
			{
				Face& face = mesh.faces[i];
				size_t matid  = face.getMatID();
				string sMtlName;
				Mtl *subMtl = NULL;
				//take care of the material
				MtlExporter *mtlExp = m_pExportDoc->m_pMtlExporter;
				SubMtlAux subMtlAux;
				mtlExp->GetSubMtlFromFaceMatId( matid, nodeMtl, subMtlAux );
				MtlIdFaceListMapT::FaceIdxsT &faceList = o_MtlIdFaceListMap[ subMtlAux.m_nFaceMatId ];
				faceList.push_back( i );
			}
		} else
		{
			MNMesh &mnMesh = m_ObjResolver->GetPolyOrTriMesh()->m_pPoly->GetMesh();
			int nFaces = mnMesh.FNum();
			int nVerts = mnMesh.VNum();

			for (int i = 0; i < nFaces; ++i) {
				MNFace *face = mnMesh.F(i);
				size_t matid = face->material;

				string sMtlName;
				Mtl *subMtl = NULL;
				//take care of the material
				MtlExporter *mtlExp = m_pExportDoc->m_pMtlExporter;
				SubMtlAux subMtlAux;
				mtlExp->GetSubMtlFromFaceMatId( matid, nodeMtl, subMtlAux );
				MtlIdFaceListMapT::FaceIdxsT &faceList = o_MtlIdFaceListMap[ subMtlAux.m_nFaceMatId ];
				faceList.push_back( i );
			}
		}
	}


	template< class ResolvePolicy >
	void MeshBaseExportCore< ResolvePolicy >::ExportMeshMaterials(  )
	{
		Mtl *nodeMtl = m_pCurNode->GetMtl();	
		size_t nSubMtls = 1;
		if( nodeMtl && nodeMtl->IsMultiMtl() )
		{
			nSubMtls = nodeMtl->NumSubMtls();
		}


		if( NULL !=  m_ObjResolver->GetPolyOrTriMesh()->m_pTri )
		{
			::Mesh &mesh = m_ObjResolver->GetPolyOrTriMesh()->m_pTri->GetMesh();

			int nFaces = mesh.getNumFaces();
			int nVerts = mesh.getNumVerts();

			for (int i = 0; i < nFaces; ++i) 
			{
				Face& face = mesh.faces[i];
				size_t matid  = face.getMatID();
				string sMtlName;
				Mtl *subMtl = NULL;
				//take care of the material
				MtlExporter *mtlExp = m_pExportDoc->m_pMtlExporter;
				SubMtlAux subMtlAux;
				mtlExp->GetSubMtlFromFaceMatId( matid, nodeMtl, subMtlAux );
				mtlExp->Export( subMtlAux.m_pSubMtl );
			}
		} else
		{

			MNMesh &mnMesh = m_ObjResolver->GetPolyOrTriMesh()->m_pPoly->GetMesh();


			int nFaces = mnMesh.FNum();
			int nVerts = mnMesh.VNum();

			for (int i = 0; i < nFaces; ++i) {
				MNFace *face = mnMesh.F(i);
				size_t matid = face->material;

				string sMtlName;
				Mtl *subMtl = NULL;
				//take care of the material
				MtlExporter *mtlExp = m_pExportDoc->m_pMtlExporter;
				SubMtlAux subMtlAux;				
				mtlExp->GetSubMtlFromFaceMatId( matid, nodeMtl, subMtlAux );
				mtlExp->Export( subMtlAux.m_pSubMtl );
			}
		}
	}


	// If so go over the faces of the mesh
	// and export each unique submaterial
	// Then find unique vertices which differ in
	// position, approximated normal and uv co-ordinates
	// Export the mesh in terms of unique vertices
	void MeshGeomExportCore::DoExport(
		shared_ptr<mdlFragInfo> &io_MeshInfo )
	{			

		shared_ptr< PolyOrTriMesh > &pOrT = m_ObjResolver->GetPolyOrTriMesh();


		int nVerts = pOrT->GetNVerts();
		int nFaces = pOrT->GetNFaces();


		//Phase_1
		//Iterate through the faces
		//and for each face get the material used.
		//From this construct a map of material versus list of faces 
		//associated with that material
		//Also, for each fresh material found call mtlExporter on the material
		MtlIdFaceListMapT mtlIdFaceListMap;
		GetMtlIdFaceMap(  mtlIdFaceListMap );
		ExportMeshMaterials();



		//Phase_2
		//Do this only if m_bPackNormals is true
		//For each material
		//	for each face associated with that material
		//		for each vertex associated with that face
		//			evaluate the normal based  on the smoothing group of that face
		//			and collect the normal 		

		//Collect the normal in a hash_set
		//The hash_set serves the purpose of easy look up.
		//Finally copy the hash_set into a vector, and sort the vector,
		//This is because, we need the normals to be in a easily indexed data structure,
		//Also, for look-up you can use std::binary_search, for a sorted vector

		//hash_set_Point3: the hash set used to collect the normals
		//hash_setPoint3::insert( const Point3 &val)
		//quantizes val, and then inserts it into the hash_set.
		//Similarly, hash_set_Point3::find( const Point3 &val),
		//quantizes val and the tries to find the quantized stored value

		LessPoint3<Point3> lessP; // a less structure for comparing Point3
		typedef HashSetPoint3<Point3> VertNormalSetT; //see the definition of HashSetPOint3
		VertNormalSetT vertNormalSet;
		MtlIdFaceListMapT::const_iterator mit;
		vector<Point3> vertNormals(0);

		if( m_bPackNormals )
		{
			for( mit = mtlIdFaceListMap.begin(); mit != mtlIdFaceListMap.end(); ++mit )
			{
				const MtlIdFaceListMapT::FaceIdxsT &faceList = mit->second;
				MtlIdFaceListMapT::FaceIdxsT::const_iterator lit;
				for( lit = faceList.begin(); lit != faceList.end(); ++lit )
				{
					int fIdx = *lit;
					assert( fIdx >= 0 && fIdx < nFaces );
					int nVertsInFace = pOrT->GetNVertsInFace( fIdx );
					for( int vertIdxInFace=0; vertIdxInFace < nVertsInFace ; ++vertIdxInFace )
					{
						const int i_nVertId = pOrT->GetVertIdxInMesh( fIdx, vertIdxInFace );
						DBG_ASSERT( (i_nVertId >= 0 && i_nVertId < nVerts), "i_nVertId: " << i_nVertId << " shouldnbe less than " << nVerts );
						Point3 vertNormal(0,0,0 );
						pOrT->GetNormal( fIdx, vertIdxInFace, vertNormal );		
						pair< VertNormalSetT::iterator , bool> res = vertNormalSet.insert( vertNormal );
						bool was_inserted = res.second;
					}
				}	
			}

			//Copy the vertNormals ifrom the hash_set into a vector,
			// for easy indexing
			vertNormals.resize( vertNormalSet.size());
			copy( vertNormalSet.begin(), vertNormalSet.end(), vertNormals.begin()); 
			sort( vertNormals.begin(), vertNormals.end(), lessP);
			//no need of the hash_set
			vertNormalSet.clear();
		}


		//Phase_3
		//For each material 
		//	for each face associated with that material
		//		for each vertex associated with that face
		//			evaluate the normal based on the moothing group of the face
		//			find the normal index in the vector-container of the normals

		//get the first texture channel
		//Right now, we only export the first texture channel
		vector<int> textureChannelIds;

		m_ObjResolver->GetPolyOrTriMesh()->GetTextureChannels(  textureChannelIds );

		int tchIdx = -1;
		if( textureChannelIds.size() > 0)
		{
			//get the first texture channel
			tchIdx = textureChannelIds[0];
		}
		typedef hash_set<UniqueVertex, Hash_Compare_UniqueVertex > UniqueVertexSetT;
		UniqueVertexSetT uniqueVertexSet;
		io_MeshInfo.reset( new mdlFragInfo );


		io_MeshInfo->m_Name = SgpuConvert::NodeName( m_pCurNode->GetName() );
		io_MeshInfo->m_NumOrigVertices = nVerts;
		if( m_bPackNormals )
		{
			io_MeshInfo->m_NumOrigNormals  = vertNormals.size();
		} else
		{
			io_MeshInfo->m_NumOrigNormals = pOrT->GetNNormals();
		}
		maAxisBox bbox;


		//for each material
		for( mit = mtlIdFaceListMap.begin(); mit != mtlIdFaceListMap.end(); ++mit )
		{
			int matid = mit->first;
			Mtl *subMtl = NULL;	
			//record a material change
			if ( mit != mtlIdFaceListMap.begin() )
				io_MeshInfo->m_MaterialChanges.push_back(io_MeshInfo->m_Indices.size() / 3);
			MtlExporter *mtlExporter = m_pExportDoc->m_pMtlExporter;
			SubMtlAux subMtlAux;
			Mtl *nodeMtl = m_pCurNode->GetMtl();			
			mtlExporter->GetSubMtlFromFaceMatId( matid, nodeMtl , subMtlAux );
			string sSubMtlName( mtlExporter->GetSubMtlName( subMtlAux.m_pSubMtl ) );
			//get the matInfo from the material info table
			mdlMatInfoTable &matInfoTable = *( m_pExportDoc->m_pMdlMatInfoTable );
			mdlMatInfoTable::iterator tit = matInfoTable.find( sSubMtlName );			
			assert( tit != matInfoTable.end() );
			//record the material
			io_MeshInfo->m_Materials.push_back( tit->second );

			const MtlIdFaceListMapT::FaceIdxsT &faceList = mit->second;
			MtlIdFaceListMapT::FaceIdxsT::const_iterator lit;
			//for each face associated with the material
			for( lit = faceList.begin(); lit != faceList.end(); ++lit )
			{
				int fIdx = *lit;
				assert( fIdx >= 0 && fIdx < nFaces );
				int nVertsInFace = pOrT->GetNVertsInFace( fIdx );
				//the new indices of the unique vertices
				//comprising this face
				DynamicBuffer< int, 3 > uniqueVertIndicesOfFace( nVertsInFace );
				//int &uniqueVertIndicesOfFace[] = *(uniqueVertIndicesOfFaceBuf.GetBuffer());
				for( int j=0; j < nVertsInFace; ++j)
					( uniqueVertIndicesOfFace.GetBuffer() )[j] = -1;
				//for each vertex assiciated with the face

				DynamicBuffer< Point3, 3 > vertPositionsTemp( nVertsInFace );
				DynamicBuffer< Point3, 3 > vertNormalsTemp( nVertsInFace );
				for( int vertIdxInFace=0; vertIdxInFace < nVertsInFace ; ++vertIdxInFace )
				{

					//----------------------------------------------------
					//find the position index (for calculating unique vertex)
					//----------------------------------------------------
					const int vertId = pOrT->GetVertIdxInMesh( fIdx, vertIdxInFace );
					DBG_ASSERT( (vertId >= 0 && vertId < nVerts), " vertId: " << vertId << " should be les than " << nVerts );		
					vertPositionsTemp.GetBuffer()[ vertIdxInFace ] = pOrT->GetVert( vertId );
#if defined(_DEBUG)
#if SGPU_DEBUG
#define VERTEX_IDX 95
					if( vertId == VERTEX_IDX)
					{
						_RPT4( _CRT_WARN, "vert %d has pos %g %g %g\n", VERTEX_IDX, pos[0], pos[1], pos[2] );
					}
#endif
#endif

					Point3 vertNormal(0,0,0);
					int normalIdx = pOrT->GetNormal( fIdx, vertIdxInFace, vertNormal );
					DBG_ASSERT( normalIdx >=0, \
						"cannot compute normal for the face: " << fIdx << " corner: " << vertIdxInFace << " mesh: " <<   MBCSTOLPCSTR( m_pCurNode->GetName())  );
					if ( !EpsilonEqualZero( vertNormal.LengthSquared() - 1.0f ) )
					{
						EXPLOG.WriteError( "Mesh %s face %d could be degenerate", MBCSTOLPCSTR( m_pCurNode->GetName() ), fIdx+1 );
					}
					const Point3 &quantizedVertNormal = vertNormalSet.Quantize( vertNormal );	

					if( m_bPackNormals)
					{
						//----------------------------------------------------
						//find the normal index (for calculating the unique vertex)
						//----------------------------------------------------
						//hash_setPoint3 hasd automatic quantization interface
						//But we are not using a hash_set any more,
						//We are using a vector containing the quantized values,
						//The vector doesnt have any quantization interface
						//So to find a normal we need to quantize the normal by ourselves and
						//check for it in the vector


						//Use the std::equal_range algorithm
						pair< vector<Point3>::const_iterator, vector<Point3>::const_iterator > eqRange =
							equal_range< vector<Point3>::const_iterator, Point3, LessPoint3<Point3> >
							( vertNormals.begin(), vertNormals.end(), quantizedVertNormal, lessP);
						//check that we actually found the normal
						assert( eqRange.first != eqRange.second);
						vector<Point3>::const_iterator distIt1 = vertNormals.begin();
						vector<Point3>::const_iterator distIt2 = eqRange.first;
						//get the index of the normal
						iterator_traits< vector<Point3>::const_iterator>::difference_type dist = distance( distIt1, distIt2); 
						normalIdx = static_cast<int>( dist);
					}

					//debug vertnormal and pos
					//_RPT5(_CRT_WARN, "face Idx = %d, vIdx = %d, normal = %g %g %g\n", fIdx, vertId, vertNormal[0], vertNormal[1], vertNormal[2] );
					//_RPT5(_CRT_WARN, "face Idx = %d, vIdx = %d, pos = %g %g %g\n", fIdx, vertId, pos[0], pos[1], pos[2] );

					//----------------------------------------------------
					//find the texture index (for the calculating the unique vertex)
					//----------------------------------------------------
					int textureIdx = -1;
					//If we have a  texture channel
					if( tchIdx >= 0)
					{				
						
						textureIdx = pOrT->GetTVertIdx( tchIdx, fIdx, vertIdxInFace );
					}

					UniqueVertex uVertex( vertId, normalIdx, textureIdx, io_MeshInfo->m_Vertices.size());
					pair< UniqueVertexSetT::iterator, bool> insertResult = uniqueVertexSet.insert( uVertex );
					//If this unique vertex is fresh					
					if( insertResult.second )
					{
						if( m_bAddRemapInfo)
						{
							io_MeshInfo->m_VertexRemap.insert( pair<int, int> ( vertId ,  io_MeshInfo->m_Vertices.size() ) );
							io_MeshInfo->m_NormalRemap.insert( pair<int, int> ( normalIdx ,  io_MeshInfo->m_Vertices.size() ) );
						}
						maVector3d sgpuPos = SgpuConvert::Vec3( vertPositionsTemp.GetBuffer()[ vertIdxInFace ] );
						sgpuPos = m_VertTransform * sgpuPos;
						vertNormalsTemp.GetBuffer()[vertIdxInFace] = quantizedVertNormal;
						maVector3d sgpuNormal = SgpuConvert::Vec3( quantizedVertNormal );
						m_NormalTransform.TransformDir( sgpuNormal );
						bbox.Union( sgpuPos );
						io_MeshInfo->m_Vertices.push_back( sgpuPos  );
						io_MeshInfo->m_Normals.push_back( sgpuNormal );

						if( tchIdx >= 0)
						{				
							
							const UVVert uv = pOrT->GetTVert( tchIdx,   textureIdx );
							io_MeshInfo->m_UVs.push_back( SgpuConvert::UV(uv) );
						}
						uniqueVertIndicesOfFace.GetBuffer()[ vertIdxInFace ] = insertResult.first->m_Index;
					} else
					{

						uniqueVertIndicesOfFace.GetBuffer()[ vertIdxInFace ] = insertResult.first->m_Index;
					}
				}
				//----------------------------------------------------
				//collect the indices for this triangular face
				//----------------------------------------------------

				for( int vertIdxInFace =0; vertIdxInFace < nVertsInFace ; ++vertIdxInFace )
				{
					io_MeshInfo->m_Indices.push_back( uniqueVertIndicesOfFace.GetBuffer()[ vertIdxInFace ] );
				}

			}// for( lit = faceList.begin(); lit != faceList.end(); ++lit )

		} //for( mit = mtlIdFaceListMap.begin(); mit != mtlIdFaceListMap.end(); ++mit )



		_RPT3(_CRT_WARN, "bboxmin: %g %g %g\n", bbox.GetMinX(), bbox.GetMinY(), bbox.GetMinZ() );
		_RPT3(_CRT_WARN, "bboxmax: %g %g %g\n", bbox.GetMaxX(), bbox.GetMaxY(), bbox.GetMaxZ() );
#if defined(_DEBUG)

#if SGPU_DEBUG
		{
			const MCHAR *szNodeName = m_pCurNode->GetName();
			_RPT1(_CRT_WARN, "%s\n", szNodeName );
			for( size_t k=0; k < io_MeshInfo->m_UVs.size() ; ++k)
			{
				const maVector2d &uv = io_MeshInfo->m_UVs[k];
				if( uv.GetX() < 0.0f || uv.GetX() > 1.0f ||
					uv.GetY() < 0.0f || uv.GetY() > 1.0f )
				{
					_RPT3(_CRT_WARN, "%d: %g %g\n", k, uv.GetX(), uv.GetY() ); 
				}
			}
		}
#endif
#endif

	}


	// If so go over the faces of the mesh
	// and export each unique submaterial
	// Then find unique vertices which differ in
	// position, approximated normal and uv co-ordinates
	// Export the mesh in terms of unique vertices
	void SubdivisionExportCore::DoExport(
		shared_ptr<mdlSubdivInfo> &io_SubdivInfo )
	{		


		//Phase_1
		//Iterate through the faces
		//and for each face get the material used.
		//From this construct a map of material versus list of faces 
		//associated with that material
		//Also, for each fresh material found call mtlExporter on the material
		MtlIdFaceListMapT mtlIdFaceListMap;
		GetMtlIdFaceMap(  mtlIdFaceListMap );
		ExportMeshMaterials();
		shared_ptr< PolyOrTriMesh > &pOrT = m_ObjResolver->GetPolyOrTriMesh();


		int nVerts = pOrT->GetNVerts();
		int nFaces = pOrT->GetNFaces();


		//Phase_2
		// Initialize io_SubdivInfo and 
		// associate the subdiv with the material


		io_SubdivInfo.reset( new mdlSubdivInfo() );

		io_SubdivInfo->m_Name = SgpuConvert::NodeName( m_pCurNode->GetName() );
		io_SubdivInfo->m_NumOrigVertices = nVerts;


		//assciate the subdiv with the material
		//-------------------------------------
		MtlIdFaceListMapT::const_iterator mit = mtlIdFaceListMap.begin();
		/*	assert( mit != mtlIdFaceListMap.end() );
		MtlIdFaceListMapT::IdxT matId = mit->first;
		const MtlIdFaceListMapT::FaceIdxsT & faceList = mit->second;
		MtlExporter *mtlExporter = m_pExportDoc->m_pMtlExporter;
		SubMtlAux subMtlAux;
		Mtl *nodeMtl = m_pCurNode->GetMtl();			
		mtlExporter->GetSubMtlFromFaceMatId( matId, nodeMtl , subMtlAux );
		string sSubMtlName( mtlExporter->GetSubMtlName( subMtlAux.m_pSubMtl ) );
		//get the matInfo from the material info table
		mdlMatInfoTable &matInfoTable = *( m_pExportDoc->m_pMdlMatInfoTable );
		mdlMatInfoTable::iterator tit = matInfoTable.find( sSubMtlName );			
		assert( tit != matInfoTable.end() );
		//record the material
		io_SubdivInfo->m_Material = tit->second;
		*/




		//Phase_3
		//For each material 
		//	for each face associated with that material
		//		for each vertex associated with that face
		//			form a uniquevertex structure and see
		//			if that uniquevertex has already been defined
		//			If not the unique vertex is entered to the unique
		//			vertex book keeping.
		maAxisBox bbox;
		MtlIdFaceListMapT::FaceIdxsT::const_iterator lit;



		//get the first texture channel
		//Right now, we only export the first texture channel
		vector<int> textureChannelIds;
		pOrT->GetTextureChannels(  textureChannelIds );
		int tchIdx = -1;
		if( textureChannelIds.size() > 0)
		{
			//get the first texture channel
			tchIdx = textureChannelIds[0];
		}
		typedef hash_set<UniqueVertex, Hash_Compare_UniqueVertex > UniqueVertexSetT;
		UniqueVertexSetT uniqueVertexSet;
		//get number of texture verts
		int nUvIdxs=0;
		if( tchIdx >= 0)
		{
			nUvIdxs = pOrT->GetNTVerts( tchIdx );
		}

		//for each material
		for( mit = mtlIdFaceListMap.begin(); mit != mtlIdFaceListMap.end(); ++mit )
		{

			int matid = mit->first;
			Mtl *subMtl = NULL;	
			//record a material change
			if ( mit != mtlIdFaceListMap.begin() )
				io_SubdivInfo->m_MaterialChanges.push_back(io_SubdivInfo->m_NumFaces );
			MtlExporter *mtlExporter = m_pExportDoc->m_pMtlExporter;
			SubMtlAux subMtlAux;
			Mtl *nodeMtl = m_pCurNode->GetMtl();			
			mtlExporter->GetSubMtlFromFaceMatId( matid, nodeMtl , subMtlAux );
			string sSubMtlName( mtlExporter->GetSubMtlName( subMtlAux.m_pSubMtl ) );
			//get the matInfo from the material info table
			mdlMatInfoTable &matInfoTable = *( m_pExportDoc->m_pMdlMatInfoTable );
			mdlMatInfoTable::iterator tit = matInfoTable.find( sSubMtlName );			
			assert( tit != matInfoTable.end() );
			//record the material
			io_SubdivInfo->m_Materials.push_back( tit->second );
			const MtlIdFaceListMapT::FaceIdxsT &faceList = mit->second;			
			io_SubdivInfo->m_NumFaces += faceList.size();
			MtlIdFaceListMapT::FaceIdxsT::const_iterator lit;
			for( lit = faceList.begin(); lit != faceList.end(); ++lit )
			{
				int fIdx = static_cast< int > ( *lit );
				assert( fIdx >= 0 && fIdx < nFaces );	
				//get the number of verts in the face
				int vertexCountOfFace = pOrT->GetNVertsInFace( fIdx );
				io_SubdivInfo->m_Indices.push_back( vertexCountOfFace );

				//the new indices of the unique vertices
				//comprising this face
				vector< int >  uniqueVertIndicesOfFace( vertexCountOfFace, -1);
				//for each vertex assiciated with the face

				for( int j=0; j < vertexCountOfFace ; ++j )
				{

					//----------------------------------------------------
					//find the position index (for calculating unique vertex)
					//----------------------------------------------------
					const int vertId = pOrT->GetVertIdxInMesh( fIdx, j);
					assert( vertId >= 0 && vertId < nVerts);										

					const Point3 pos = pOrT->GetVert( vertId );
#if defined(_DEBUG)
#if SGPU_DEBUG
#define VERTEX_IDX 95
					//if( vertId == VERTEX_IDX)
					{
						_RPT4( _CRT_WARN, "vert %d has pos %g %g %g\n", VERTEX_IDX, pos[0], pos[1], pos[2] );
					}
#endif
#undef SGPU_DEBUG 
#endif
					//----------------------------------------------------
					//find the texture index (for the calculating the unique vertex)
					//----------------------------------------------------
					int textureIdx = -1;
					//If we have a  texture channel
					if( tchIdx >= 0)
					{				
						textureIdx = pOrT->GetTVertIdx( tchIdx, fIdx, j );
					}

					UniqueVertex uVertex( vertId, -1, textureIdx, io_SubdivInfo->m_Vertices.size());
					pair< UniqueVertexSetT::iterator, bool> insertResult = uniqueVertexSet.insert( uVertex );
					//If this unique vertex is fresh					
					if( insertResult.second )
					{
						if( m_bAddRemapInfo)
						{
							io_SubdivInfo->m_VertexRemap.insert( pair<int, int> ( vertId ,  io_SubdivInfo->m_Vertices.size() ) );
						}
						maVector3d sgpuPos = SgpuConvert::Vec3( pos );
						sgpuPos = m_VertTransform * sgpuPos;
						bbox.Union( sgpuPos );
						io_SubdivInfo->m_Vertices.push_back( sgpuPos  );

						if( tchIdx >= 0)
						{												
							const UVVert uv = pOrT->GetTVert( tchIdx,   textureIdx );
							io_SubdivInfo->m_UVs.push_back( SgpuConvert::UV(uv) );
						}
						//int fIdx =  insertResult.first->m_Index;
						uniqueVertIndicesOfFace[j] = insertResult.first->m_Index;
					} else
					{
						//int fIdx =  insertResult.first->m_Index;
						uniqueVertIndicesOfFace[j] = insertResult.first->m_Index;
					}
				}

				//----------------------------------------------------
				//collect the indices for this triangular face
				//----------------------------------------------------

				for( int j =0; j < vertexCountOfFace ; ++j )
				{
					io_SubdivInfo->m_Indices.push_back( uniqueVertIndicesOfFace[j] );
				}

			}// for( lit = faceList.begin(); lit != faceList.end(); ++lit )

		}//for( mit = mtlIdFaceListMap.begin(); mit != mtlIdFaceListMap.end(); ++mit )
		_RPT3(_CRT_WARN, "bboxmin: %g %g %g\n", bbox.GetMinX(), bbox.GetMinY(), bbox.GetMinZ() );
		_RPT3(_CRT_WARN, "bboxmax: %g %g %g\n", bbox.GetMaxX(), bbox.GetMaxY(), bbox.GetMaxZ() );
#if defined(_DEBUG)
#if SGPU_DEBUG
		_RPT1(_CRT_WARN, "%s\n", szNodeName );
		for( size_t k=0; k < io_MeshInfo->m_UVs.size() ; ++k)
		{
			const maVector2d &uv = io_MeshInfo->m_UVs[k];
			if( uv.GetX() < 0.0f || uv.GetX() > 1.0f ||
				uv.GetY() < 0.0f || uv.GetY() > 1.0f )
			{
				_RPT3(_CRT_WARN, "%d: %g %g\n", k, uv.GetX(), uv.GetY() ); 
			}
		}
#endif
#endif
	}


	void MeshAnimExportCore::DoExportFrame(  vtxVertexFrame *&o_pVertexFrame )
	{

		bool bExportAsSubdiv;
		bool bObjFlagRet = AllowedObjectFlags::GetValue( m_pCurNode, L"export as subdiv" , bExportAsSubdiv );
		DBG_ASSERT( bObjFlagRet, "export as subdiv fla not found in object" );
		bool bExportNormals = !bExportAsSubdiv;

		shared_ptr<PolyOrTriMesh> pPOrT( m_ObjResolver->GetPolyOrTriMesh() );
		int nVerts = pPOrT->GetNVerts();
		int nFaces = pPOrT->GetNFaces();
		o_pVertexFrame = new vtxVertexFrame ;
		o_pVertexFrame->m_Positions.resize( nVerts );
		int nNormals = pPOrT->GetNNormals();

		for( int iv =0; iv < nVerts; ++iv )
		{
			Point3 pos = pPOrT->GetVert( iv );
			o_pVertexFrame->m_Positions[iv] = m_VertTransform * SgpuConvert::Vec3( pos );
		}
		if( bExportNormals )
		{
			typedef HashSetPoint3<Point3> VertNormalSetT; //see the definition of HashSetPOint3
			VertNormalSetT vertNormalSet;
			int nNNormals = pPOrT->GetNNormals();
			o_pVertexFrame->m_Normals.resize( pPOrT->GetNNormals() );

			for( int nIdx =0; nIdx < nNormals; ++nIdx )
			{
				Point3 normal = pPOrT->GetIthNormal( nIdx );
				const Point3 &quantizedVertNormal = vertNormalSet.Quantize( normal );	
				maVector3d sgpuNorm = SgpuConvert::Vec3( quantizedVertNormal );
				m_NormalTransform.TransformDir( sgpuNorm );
				if( !sgpuNorm.Normalize() )
				{
					EXPLOG.WriteWarning( "Mesh %s  has zero lengthed normals ", MBCSTOLPCSTR( m_pCurNode->GetName() ) );
				}					
				o_pVertexFrame->m_Normals[nIdx] = sgpuNorm;
			}
		}
	}


	//            SceneRoot
	//             /     \
	//            A       B
	//          /  \     /  \
	//         C   D    E   F
	//        / \
	//       G   H
	// In the above figure if we select Node C, and export the subhierarchy
	// rooted at A. What is the world transform that need be applied to
	// the geometry of a node like H.?
	// nodeH.GetNodeTM( time )  will give the world transform for H,
	// ( sans the offset transform ). But we need only the part
	// of the tansformartion transformation relative to node C's parent.
	// The following code will calculate the required transform


	maMatrix4x4 CalculateRequiredWorldTransformForGeometry
		(
		ExportDoc *i_pExportDoc,
		INode *i_pCurNode,
		INode *i_pExportedRootNode,
		TimeValue i_CurTime
		)
	{

		INode *pTempNode = ( i_pExportedRootNode ) ? i_pExportedRootNode: i_pCurNode;
		INode *pParentNode = pTempNode->GetParentNode();
		assert( NULL !=  pParentNode );
		Matrix3 parentTM = pParentNode->GetNodeTM( i_CurTime );
		Matrix3 thisNodesTM = i_pCurNode->GetNodeTM( i_CurTime ); 
		//Matrix3 requiredWorldTransformForGeometry = thisNodesTM * Inverse( parentTM );
		
		Matrix3 requiredWorldTransformForGeometry = thisNodesTM;

		bool isSkinned = false;
		{

			TimeValue startTime = i_pExportDoc->m_Options.m_StartTime;
			ResolveSkinModifier skinDet( *i_pExportDoc,  i_pCurNode , startTime );				
			isSkinned = skinDet.Found();
		}
		//If this is not a skinned mesh
		//then include the offset (pivot TM also )
		Matrix3 piv(1);
		piv.SetTrans( i_pCurNode->GetObjOffsetPos() ); 
		PreRotateMatrix( piv, i_pCurNode->GetObjOffsetRot() ); 
		ApplyScaling( piv, i_pCurNode->GetObjOffsetScale() ); 
		requiredWorldTransformForGeometry  = piv * requiredWorldTransformForGeometry;
		maMatrix4x4 sgpuRequiredWorldTransformForGeometry = SgpuConvert::Ma4x4( requiredWorldTransformForGeometry );
		float fdet = sgpuRequiredWorldTransformForGeometry.GetDeterminant();
		return sgpuRequiredWorldTransformForGeometry;
	}


	struct SetIntLess
	{
	
		bool operator()( const std::set<int> &first, const std::set<int> &second ) const
		{
			std::set<int>::const_iterator sit1, sit2;
			for( sit1 = first.begin(), sit2 = second.begin();  ; ++sit1, ++sit2 )
			{
				if( sit1 == first.end() )
				{
					return sit2 != second.end();
				}
				if( sit2 == second.end() )
				{
					return false;
				}
				if( *sit1 < *sit2 )
				{
					return true;
				}
				assert( sit1 != first.end() );
				assert( sit2 != second.end() );
				if( *sit1 > *sit2 )
				{
					return false;
				}
				assert( *sit1 == *sit2 );
				
			}
			//sould not reach here
			assert( false);
			return false;
		}
	};

	bool CheckEquivalencyOfMtlIdFaceListMap( ExportDoc &i_ExportDoc, INode *i_pNode1, INode *i_pNode2 )
	{
		typedef MeshBaseExportCore< ResolvePolicyBindPose > MeshBaseExportCoreImpl;
		typedef MeshBaseExportCoreImpl::MtlIdFaceListMapT  MtlIdFaceListMapT;
		//Check the export as subdiv flag

		bool bExportAsSubdiv1;
		bool bObjFlagRet = AllowedObjectFlags::GetValue( i_pNode1, L"export as subdiv", bExportAsSubdiv1);					
		DBG_ASSERT( bObjFlagRet, "cannot get export as subdiv flag!");
		bool bExportAsSubdiv2;
		bObjFlagRet = AllowedObjectFlags::GetValue( i_pNode2, L"export as subdiv", bExportAsSubdiv2 );	
		DBG_ASSERT( bObjFlagRet, "cannot get export as subdiv flag!");
		
		
		
		assert(( bExportAsSubdiv1 && bExportAsSubdiv2 ) || ( !bExportAsSubdiv1 && !bExportAsSubdiv2 ) );
		bool bAddRemapInfo = ( i_ExportDoc.m_Options.m_eExportIntent == eVertAnim ||  i_ExportDoc.m_Options.m_eExportIntent ==  eParticle );
		maMatrix4x4 ident;
		ident.Identity();
		MeshBaseExportCore< ResolvePolicyBindPose >::MtlIdFaceListMapT mtlIdFaceListMap1;
		MtlIdFaceListMapT mtlIdFaceListMap2;
		TimeValue startTime = i_ExportDoc.m_Options.m_StartTime;
		shared_ptr< MeshBaseExportCore< ResolvePolicyBindPose > > exportCore1, exportCore2;
		if ( bExportAsSubdiv1 )
		{					
			{
				exportCore1.reset( new SubdivisionExportCore( i_ExportDoc, *i_pNode1, startTime, ident, bAddRemapInfo ) );
				exportCore1->GetMtlIdFaceMap(  mtlIdFaceListMap1 );
			}
			{
				shared_ptr< SubdivisionExportCore  > exportCore2;
				exportCore2.reset( new SubdivisionExportCore( i_ExportDoc, *i_pNode2, startTime, ident, bAddRemapInfo ) );
				exportCore2->GetMtlIdFaceMap(  mtlIdFaceListMap1 );
			}
		} else
		{

			{
				exportCore1.reset( new MeshGeomExportCore( i_ExportDoc, *i_pNode1, startTime, ident, bAddRemapInfo ) );
				exportCore1->GetMtlIdFaceMap(  mtlIdFaceListMap1 );
			}
			{
				exportCore2.reset( new MeshGeomExportCore( i_ExportDoc, *i_pNode2, startTime, ident, bAddRemapInfo ) );
				exportCore2->GetMtlIdFaceMap(  mtlIdFaceListMap2 );
			}

		}
		typedef MeshBaseExportCore< ResolvePolicyBindPose >::MtlIdFaceListMapT MtlIdFaceListMap;
		
		if( mtlIdFaceListMap1 == mtlIdFaceListMap2 )
		{
			return true;		
		} 
#if MULTI_MATERIAL_INSTANCING
		else
		{
			std::set< std::set< int >, SetIntLess  > faceListSet1, faceListSet2;
			MtlIdFaceListMap::const_iterator mit;
			for( mit = mtlIdFaceListMap1.begin(); mit != mtlIdFaceListMap1.end(); ++mit )
			{
				int mid = mit->first;
				const MtlIdFaceListMap::FaceIdxsT & fidList = mit->second;
				std::set< int > fidSet ( fidList.begin(), fidList.end() );
				faceListSet1.insert( fidSet );
			}
			for( mit = mtlIdFaceListMap1.begin(); mit != mtlIdFaceListMap1.end(); ++mit )
			{
				int mid = mit->first;
				const MtlIdFaceListMap::FaceIdxsT & fidList = mit->second;
				std::set< int > fidSet ( fidList.begin(), fidList.end() );
				faceListSet2.insert( fidSet );
			}
			return faceListSet1 == faceListSet2 ;
		}
#else
		else return false;
#endif

	}

} //namespace SgpuExp