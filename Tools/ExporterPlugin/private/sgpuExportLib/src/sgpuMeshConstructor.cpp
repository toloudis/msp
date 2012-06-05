/****************************************************************************\
**  sgpuMeshConstructor.cpp
**
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuMeshConstructor.hpp"
#include "sgpuNode.hpp"
#include "sgpuModelExportScene.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMaterialImpl.hpp"
#include "sgpuMeshImpl.hpp"
#include "sgpuMeshConstructorImpl.hpp"
#include "sgpuException.hpp"
#include "sgpuPropertyValue.hpp"
#include "sgpuStringImpl.hpp"
#include "sgpuPropertyValue.hpp"

#include "Core/It/itStringUtil.hpp"
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <set>





void ExtractUserDefinedProps( sgpuConstructor &constructor, shared_ptr< mdlFragInfo > &io_FragInfo, bool i_bVertexAnimation )
	{

		sgpuPropertyValue pVal;
		bool bVal = constructor.GetProperty( sgpuString("doublesided"), pVal );
		bVal = !bVal && constructor.GetProperty( sgpuString("double sided"), pVal );
		
		bool bDoubleSided = true;
		if( bVal )
		{
			bDoubleSided = pVal.GetBoolValue();
		}
			
		bool bCastsShadow = true;
		
		bVal = constructor.GetProperty( sgpuString("castsshadow"), pVal );
		bVal = !bVal && constructor.GetProperty( sgpuString("casts shadow"), pVal );
		if( bVal )
		{
			bCastsShadow = pVal.GetBoolValue();
		}
		bool bReceivesShadow = true;
		
		bVal = constructor.GetProperty( sgpuString("receivesshadow"), pVal );
		bVal = !bVal && constructor.GetProperty( sgpuString("receives shadow"), pVal );
		if( bVal )
		{
			bReceivesShadow = pVal.GetBoolValue();
		}

		bool bShadowHull = false;
		bVal = constructor.GetProperty( sgpuString("shadow hull"), pVal );
		bVal = !bVal && constructor.GetProperty( sgpuString("shadowhull"), pVal );
		if( bVal )
		{
			bShadowHull = pVal.GetBoolValue();
		}

		bool bTriangleSort = false;
		bVal = constructor.GetProperty( sgpuString("triangle sort"), pVal );
		bVal = !bVal && constructor.GetProperty( sgpuString("trianglesort"), pVal );
		if( bVal )
		{
			bTriangleSort = pVal.GetBoolValue();
		}

		bool bCloth = false;

		bVal = constructor.GetProperty( sgpuString("cloth"), pVal );
		bVal = !bVal && constructor.GetProperty( sgpuString("cloth"), pVal );
		if( bVal )
		{
			bCloth = pVal.GetBoolValue();
		}
		bool bExportAsSubdiv = false;
		bVal = constructor.GetProperty( sgpuString("export as subdiv"), pVal );
		bVal = !bVal && constructor.GetProperty( sgpuString("exportassubdiv"), pVal );
		if( bVal )
		{
			bExportAsSubdiv = pVal.GetBoolValue();
		}
		bool bVisibleAnim = false;		
		bVal = constructor.GetProperty( sgpuString("visible anim"), pVal );
		bVal = !bVal && constructor.GetProperty( sgpuString("visibleanim"), pVal );
		if( bVal )
		{
			bVisibleAnim = pVal.GetBoolValue();
		}
		
		int lresVal=0;
		bVal = constructor.GetProperty( sgpuString("low res"), pVal );
		bVal = !bVal && constructor.GetProperty( sgpuString("lowres"), pVal );
		if( bVal )
		{
			bVal = pVal.GetBoolValue();
			lresVal = (bVal) ? 1:2;
		}

		if( lresVal == 1  && bExportAsSubdiv )
		{
			
			// "mesh flag conflict: %s, low resultion flag set and export as subdiv flag is set", io_FragInfo->m_Name.c_str() );
			lresVal = 0;
		}

		if( io_FragInfo.get() )
		{	//set the flags for mesh
			io_FragInfo->m_Flags.m_bCastsShadow = bCastsShadow;
			io_FragInfo->m_Flags.m_bReceivesShadow = bReceivesShadow;
			io_FragInfo->m_Flags.m_bDoubleSided = bDoubleSided;
			io_FragInfo->m_Flags.m_bShadowHull = bShadowHull;
			io_FragInfo->m_Flags.m_bTriangleSort = bTriangleSort;
			io_FragInfo->m_Flags.m_bVertexAnimation = bCloth || i_bVertexAnimation;
			io_FragInfo->m_ResolutionLevel = lresVal;
		} 
	}


sgpuMeshConstructor::sgpuMeshConstructor( sgpuModelExportScene &i_Scene,  sgpuNode & i_Node , bool i_bVertexAnimation ):
	sgpuConstructor( i_Scene, i_Node ),
	m_pImpl( new sgpuMeshConstructorImpl )
{
	m_pImpl->m_bVertexAnimation = i_bVertexAnimation;
}

sgpuMeshConstructor::~sgpuMeshConstructor()
{ 
	delete m_pImpl;
}

sgpuMeshConstructor::sgpuMeshConstructor( const sgpuMeshConstructor &i_Other ):
sgpuConstructor( i_Other )
{
	DBG_ASSERT( false, "copy constructor for sgpuMeshConstructor not allowed" );
}



sgpuMeshConstructor &sgpuMeshConstructor::operator=( const sgpuMeshConstructor & i_Other)
{
	DBG_ASSERT( false, "copy constructor for sgpuMeshConstructor not allowed" );
	return *this;
}


void  sgpuMeshConstructor::SetPosition( const sgpuVector3 *i_pPositions, int i_NumPos )
{
	m_pImpl->m_PositionTemp.resize( i_NumPos );
	for( int i=0; i < i_NumPos; ++i)
	{
		const sgpuVector3 & curVector = i_pPositions[i];
		m_pImpl->m_PositionTemp[ i ] = sgpuVector3( curVector[0], curVector[1], curVector[2] );
	}
}

	//i_Normals array of T-s which contain the normals
	//i_NumNormals = number of normals
	//This data will be copiued tonthe internal data of sgpuMeshConstructor
void sgpuMeshConstructor::SetNormal( const sgpuVector3 *i_pNormals, int i_NumNormals )
{	
	m_pImpl->m_NormalTemp.resize( i_NumNormals );
	for( int i=0; i < i_NumNormals; ++i)
	{
		const sgpuVector3 & curVector = i_pNormals[i];
		m_pImpl->m_NormalTemp[ i ] = sgpuVector3( curVector[0], curVector[1], curVector[2] );
	}
}
	//i_pUVs array of T-s, which contain the UV-s
	//i_NumUVs = number of UV-s
	//This data will be copied to the internal data of the sgpuMeshConstructor
void sgpuMeshConstructor::SetUV( const sgpuVector3 *i_pUVs, int i_NumUVs )
{
	m_pImpl->m_UVTemp.resize( i_NumUVs );
	for( int i=0; i < i_NumUVs; ++i)
	{
		const sgpuVector3 & curVector = i_pUVs[i];
		m_pImpl->m_UVTemp[ i ] = sgpuVector3( curVector[0], curVector[1], 0.0f );
	}
}


int sgpuMeshConstructor::AddFace( const FaceVertex *i_pFace, int i_NVerticesInFace, const sgpuMaterial & i_FaceMtl  )
{
	DBG_ASSERT( ( NULL != m_pImpl ), "meshConstructor implementation should not be NULL" );
	if( !m_pImpl->IsValidVertexIndex( i_pFace[0].m_VertexId ) ||
		!m_pImpl->IsValidVertexIndex( i_pFace[1].m_VertexId ) ||
		!m_pImpl->IsValidVertexIndex( i_pFace[2].m_VertexId ) ||
		!m_pImpl->IsValidNormalIndex( i_pFace[0].m_NormalId )   ||
		!m_pImpl->IsValidNormalIndex( i_pFace[1].m_NormalId )   ||
		!m_pImpl->IsValidNormalIndex( i_pFace[2].m_NormalId )   ||
		!m_pImpl->IsValidUVIndex( i_pFace[0].m_UVId )           ||
		!m_pImpl->IsValidUVIndex( i_pFace[1].m_UVId )           ||
		!m_pImpl->IsValidUVIndex( i_pFace[2].m_UVId ) ||
		i_NVerticesInFace != 3
		)
	{
		return 0;
	}
	m_pImpl->m_FaceTemp.push_back( PolyFace(i_NVerticesInFace, i_FaceMtl) );
	PolyFace &face = m_pImpl->m_FaceTemp.back();
	DBG_ASSERT( face.m_FaceVertices != NULL, "PolyFace, shared Ptr should not be null!");
	DBG_ASSERT( face.m_FaceVertices->size() == i_NVerticesInFace, "PolyFace:FaceVertices size: " << face.m_FaceVertices->size() << " should be " << i_NVerticesInFace );


	(*face.m_FaceVertices)[0] = i_pFace[0];
	(*face.m_FaceVertices)[1] = i_pFace[1];
	(*face.m_FaceVertices)[2] = i_pFace[2];
	return m_pImpl->m_FaceTemp.size();
}



sgpuMesh sgpuMeshConstructor::Construct( const sgpuString  &i_MeshName )
{
	std::vector< PolyFace >::const_iterator fit;
	typedef std::list< int > TFaceIdList;
	bool bAddRemapInfo = true;
	typedef std::map< std::string, TFaceIdList> TMtlToFaceIdListMap;
	TMtlToFaceIdListMap mtlToFaceIdListMap;
	int nTotalNumIndices = 0;
	int nTotalNumFaces = 0;
	size_t ithFace =0;
	for( fit = m_pImpl->m_FaceTemp.begin(); fit != m_pImpl->m_FaceTemp.end(); ++fit, ++ithFace )
	{
		sgpuMaterial mtl = fit->m_Mtl;
		const PolyFace &face =  *fit;
		DBG_ASSERT( (NULL != face.m_FaceVertices ), "ith Face: " << ithFace << " of mesh: " << i_MeshName.m_pImpl->m_Data << " has zero vertices" );
		DBG_ASSERT( (0 < face.m_FaceVertices->size() ), "ith Face: " << ithFace << " of mesh: " << i_MeshName.m_pImpl->m_Data << " has zero vertices" );

		nTotalNumIndices += face.m_FaceVertices->size();
		++nTotalNumFaces;
		TMtlToFaceIdListMap::iterator mit2 = mtlToFaceIdListMap.find( mtl.m_pImpl->m_Name );
		if( mit2 == mtlToFaceIdListMap.end() )
		{			
			TMtlToFaceIdListMap::value_type val( mtl.m_pImpl->m_Name, TFaceIdList() );
			std::pair<TMtlToFaceIdListMap::iterator, bool> insertResult = mtlToFaceIdListMap.insert( val );
			mit2 = insertResult.first;
		}
		DBG_ASSERT( ( mit2 != mtlToFaceIdListMap.end()), "logically not possible condition ithFace: " << ithFace << " mesh: " <<  i_MeshName.m_pImpl->m_Data );
		TFaceIdList &faceIdList = mit2->second;
		DBG_ASSERT( ( std::find( faceIdList.begin(), faceIdList.end(), ithFace )  == faceIdList.end()), "duplicate face : " << ithFace << " in the face id list, mesh: " << i_MeshName.m_pImpl->m_Data );
		faceIdList.push_back( ithFace );

	}
	typedef std::set<UniqueVertex > UniqueVertexSetT;
	UniqueVertexSetT uniqueVertexSet;
	if( m_pImpl->m_PositionTemp.size() <= 0 ) //&& mtlToFaceIdListMap.size() > 0)
	{	
		std::stringstream ss;
		ss << "number of vertices == 0 for the mesh being constructed: " << i_MeshName.m_pImpl->m_Data;
		throw sgpuException( sgpuString( ss.str().c_str() ) );
	}
	if ( mtlToFaceIdListMap.size() <=0 )
	{
		std::stringstream ss;
		ss << "number of faces == 0 for the mesh being constructed: " << i_MeshName.m_pImpl->m_Data;
		throw sgpuException( sgpuString(ss.str().c_str()) );
	}

	int nOriginalFaces = m_pImpl->m_FaceTemp.size();
	int nOriginalVerts = m_pImpl->m_PositionTemp.size();

	int iNumIndicesSoFar = 0;
	int iNumFacesSoFar = 0;
	int iNumVerticesSoFar =0;
	int iNumUVsSoFar = 0;

	sgpuMesh retMesh = m_Node.CreateNodeContent_TriangleMesh( m_pImpl->m_bVertexAnimation );
	retMesh.SetName( i_MeshName );
	retMesh.SetNumFaces( nTotalNumFaces );

	TMtlToFaceIdListMap::const_iterator mit;
	for( mit = mtlToFaceIdListMap.begin(); mit != mtlToFaceIdListMap.end(); ++mit )
	{
		const TMtlToFaceIdListMap::value_type &vtype( *mit );		
		const TFaceIdList &faceList = mit->second;
		TFaceIdList::const_iterator lit;
		const std::string& sMtlName = mit->first;
		sgpuMaterial sMat = m_Scene.GetMaterial( sgpuString( sMtlName.c_str() ) );
		retMesh.SetMaterialChange( iNumFacesSoFar, sMat );
		//record a material change
	
		for( lit = faceList.begin(); lit != faceList.end(); ++lit )
		{
			int fIdx = *lit;
			assert( fIdx >= 0 && fIdx < nOriginalFaces );
			PolyFace &face = m_pImpl->m_FaceTemp[ fIdx ];
			DBG_ASSERT( face.m_FaceVertices != NULL, "PolyFace, shared Ptr should not be null!");
			int nVerticesInFace = static_cast< int > ( face.m_FaceVertices->size() );
			std::vector<int> uniqueVertIndicesOfFace( nVerticesInFace, -1 );
			for( int j=0; j < nVerticesInFace ; ++j )
			{
				const FaceVertex &fv = (*face.m_FaceVertices)[j];	
				DBG_ASSERT( m_pImpl->IsValidFaceVertex( fv ), "face vertex: " << j << " of face " << fIdx << "  is not valid!, mesh: " <<  i_MeshName.m_pImpl->m_Data ); 
				int vIdx = fv.m_VertexId;
				int nIdx = fv.m_NormalId;
				int tIdx = fv.m_UVId;

				UniqueVertex uVertex( vIdx, nIdx, tIdx,  uniqueVertexSet.size() );
				std::pair< UniqueVertexSetT::iterator, bool> insertResult = uniqueVertexSet.insert( uVertex );
				//If this unique vertex is fresh					
				if( insertResult.second )
				{
					uniqueVertIndicesOfFace[j] = insertResult.first->m_Index;
				} else
				{
					uniqueVertIndicesOfFace[j] = insertResult.first->m_Index;
				}
			}
			m_pImpl->m_FaceRemap.insert( std::pair< int, int > ( fIdx,  iNumFacesSoFar ) );
			for( int j =0; j < nVerticesInFace ; ++j )
			{
				retMesh.SetFaceVertex( iNumFacesSoFar, j,  uniqueVertIndicesOfFace[j] );
			}
			++iNumFacesSoFar;
		} //for( mit = mtlIdFaceListMap.begin(); mit != mtlIdFaceListMap.end(); ++mit )

		retMesh.SetNumOriginalPositions( m_pImpl->m_PositionTemp.size() );
		retMesh.SetNumOriginalNormals( m_pImpl->m_NormalTemp.size() );
		//making the sgpuMesh
		retMesh.SetNumVertices( uniqueVertexSet.size() );
		UniqueVertexSetT::const_iterator  uit;
		for( uit = uniqueVertexSet.begin(); uit != uniqueVertexSet.end(); ++uit )
		{
			const UniqueVertex &uv = *uit;
			DBG_ASSERT( (m_pImpl->IsValidVertexIndex(uv.m_PosIndex)), "PositionIndex: " << uv.m_PosIndex << " is not valid, mesh: " << i_MeshName.m_pImpl->m_Data );
			DBG_ASSERT( (m_pImpl->IsValidNormalIndex(uv.m_NormalIndex)), "NormalIndex: " << uv.m_NormalIndex << " is not valid, mesh: " << i_MeshName.m_pImpl->m_Data  );
			const sgpuVector3 &thisPos = m_pImpl->m_PositionTemp[ uv.m_PosIndex ];
			const sgpuVector3 &thisNormal = m_pImpl->m_NormalTemp[ uv.m_NormalIndex ];
			retMesh.SetPosition(  uv.m_Index , thisPos(0), thisPos(1), thisPos(2) );
			retMesh.SetNormal(  uv.m_Index , thisNormal(0), thisNormal(1), thisNormal(2) );
			if( uv.m_UVIndex >= 0 )
			{
				const sgpuVector3 &thisUV = m_pImpl->m_UVTemp[ uv.m_UVIndex ];
				retMesh.SetTexCoord(  uv.m_Index ,  thisUV(0), thisUV(1) );
			}
			if( bAddRemapInfo )
			{
				retMesh.SetVertexRemap( uv.m_Index, uv.m_PosIndex, uv.m_NormalIndex );
			}
		}
	} 
	ExtractUserDefinedProps( *this, retMesh.m_pImpl->m_Mesh , m_pImpl->m_bVertexAnimation );

	return retMesh;
}

void  sgpuMeshConstructor::SetProperty(const sgpuString &i_PropertyName, const sgpuPropertyValue & i_PropertyValue)
{
	m_pImpl->SetProperty( i_PropertyName.m_pImpl->m_Data, i_PropertyValue );
}

bool sgpuMeshConstructor::GetProperty(const sgpuString &i_PropertyName, sgpuPropertyValue &o_Property )
{
	return m_pImpl->GetProperty( i_PropertyName.m_pImpl->m_Data, o_Property );
}
