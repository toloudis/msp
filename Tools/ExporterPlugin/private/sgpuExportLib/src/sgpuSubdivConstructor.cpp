/****************************************************************************\
**  sgpuSubdivConstructor.cpp
**
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuSubdivConstructor.hpp"
#include "sgpuNode.hpp"
#include "sgpuSubdivImpl.hpp"
#include "sgpuModelExportScene.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMaterialImpl.hpp"
#include "sgpuSubdivConstructorImpl.hpp"
#include "sgpuException.hpp"
#include "sgpuPropertyValue.hpp"
#include "sgpuStringImpl.hpp"

#include "Core/It/itStringUtil.hpp"
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <set>









void ExtractUserDefinedProps( sgpuConstructor &constructor, shared_ptr< mdlSubdivInfo > io_SubdivInfo, bool i_bVertexAnimation )
{

	sgpuPropertyValue pVal;
	bool bVal = constructor.GetProperty( sgpuString("doublesided"), pVal );
	bVal = !bVal && constructor.GetProperty( sgpuString("double sided"), pVal );

	bool bDoubleSided = false;
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
		//EXPLOG.WriteError( "mesh flag conflict: %s, low resultion flag set and export as subdiv flag is set", io_SubdivInfo->m_Name.c_str() );
		lresVal = 0;
	}

	if( io_SubdivInfo.get() )
	{
		//set the flags for subdivinfo

		io_SubdivInfo->m_Flags.m_bCastsShadow = bCastsShadow;
		io_SubdivInfo->m_Flags.m_bReceivesShadow = bReceivesShadow;
		io_SubdivInfo->m_Flags.m_bDoubleSided = bDoubleSided;
		io_SubdivInfo->m_Flags.m_bShadowHull = bShadowHull;
		io_SubdivInfo->m_Flags.m_bTriangleSort = bTriangleSort;
		io_SubdivInfo->m_Flags.m_bAutoGenLowRes = (lresVal == 0);
	}
}



sgpuSubdivConstructor::sgpuSubdivConstructor( sgpuModelExportScene &i_Scene,  sgpuNode & i_Node , bool i_bVertexAnimation ):
sgpuConstructor( i_Scene, i_Node ),
m_pImpl( new sgpuSubdivConstructorImpl() )
{
	m_pImpl->m_bVertexAnimation = i_bVertexAnimation;
}

sgpuSubdivConstructor::~sgpuSubdivConstructor()
{ 
	delete m_pImpl;
}

sgpuSubdivConstructor::sgpuSubdivConstructor( const sgpuSubdivConstructor &i_Other )
:sgpuConstructor( i_Other )
{	
	DBG_ASSERT( false, "copy constructor for sgpuSubdivConstructor not allowed" );
}


sgpuSubdivConstructor &sgpuSubdivConstructor::operator=( const sgpuSubdivConstructor & i_Other)
{
	DBG_ASSERT( false, "copy constructor for sgpuSubdivConstructor not allowed" );
	return *this;
}

int sgpuSubdivConstructor::AddFace(const FaceVertex *i_pFaceVertex, int i_NVerticesInFace, const sgpuMaterial &i_Material )
{

	DBG_ASSERT( ( NULL != m_pImpl ), "subdivConstructor implementation should not be NULL" );
	bool bRet = i_NVerticesInFace > 2;
	for( int i=0; bRet &&  i < i_NVerticesInFace ; ++i )
	{
		const FaceVertex &fv = i_pFaceVertex[ i ];
		bRet = bRet && m_pImpl->IsValidVertexIndex( fv.m_VertexId );
		bRet = bRet && m_pImpl->IsValidUVIndex( fv.m_UVId );
	}
	if( !bRet )
	{
		return 0;
	}
	m_pImpl->m_FaceTemp.push_back( PolyFace(i_NVerticesInFace, i_Material ) );
	PolyFace &face = m_pImpl->m_FaceTemp.back();
	DBG_ASSERT( face.m_FaceVertices != NULL, "PolyFace, shared Ptr should not be null!");
	DBG_ASSERT( face.m_FaceVertices->size() == i_NVerticesInFace, "PolyFace:FaceVertices size should be: " <<  i_NVerticesInFace );
	for( int i=0; i < i_NVerticesInFace; ++i )
	{
		(*face.m_FaceVertices)[i] = i_pFaceVertex[ i ];
	}
	return m_pImpl->m_FaceTemp.size();
}


sgpuSubdiv sgpuSubdivConstructor::Construct( const sgpuString &i_SubdivName )
{
	std::vector< PolyFace >::const_iterator fit;
	typedef std::list< int > TFaceIdList;
	bool bAddRemapInfo = true;
	typedef std::map< std::string, TFaceIdList> TMtlToFaceIdListMap;
	TMtlToFaceIdListMap mtlToFaceIdListMap;
	int nTotalNumIndices = 0;
	size_t ithFace=0;
	for( fit = m_pImpl->m_FaceTemp.begin(), ithFace=0; fit != m_pImpl->m_FaceTemp.end(); ++fit, ++ithFace )
	{
		const PolyFace &face = *fit;
		DBG_ASSERT( (NULL != face.m_FaceVertices ), "ith Face: " << ithFace << " of : " << i_SubdivName.m_pImpl->m_Data << "has zero vertices" );
		DBG_ASSERT( (0 < face.m_FaceVertices->size() ), "ith Face " << ithFace << " of : " << i_SubdivName.m_pImpl->m_Data << " has zero vertices" );
		nTotalNumIndices += face.m_FaceVertices->size() + 1;
		sgpuMaterial mtl = fit->m_Mtl;
		TMtlToFaceIdListMap::iterator mit2 = mtlToFaceIdListMap.find( mtl.m_pImpl->m_Name );
		
		if( mit2 == mtlToFaceIdListMap.end() )
		{	

			TMtlToFaceIdListMap::value_type val( mtl.m_pImpl->m_Name, TFaceIdList() );
			std::pair<TMtlToFaceIdListMap::iterator, bool> insertResult = mtlToFaceIdListMap.insert( val );
			mit2 = insertResult.first;
		}

		DBG_ASSERT( ( mit2 != mtlToFaceIdListMap.end()), "ith Face: " << ithFace << " logically not possible condition: " << i_SubdivName.m_pImpl->m_Data );
		TFaceIdList &faceIdList = mit2->second;
		DBG_ASSERT( ( std::find( faceIdList.begin(), faceIdList.end(), ithFace )  == faceIdList.end()), "duplicate face : " <<  ithFace << " in the face id list of subdiv: " <<  i_SubdivName.m_pImpl->m_Data );
		faceIdList.push_back( ithFace );
	}
	typedef std::set<UniqueVertex > UniqueVertexSetT;
	UniqueVertexSetT uniqueVertexSet;
	if( m_pImpl->m_PositionTemp.size() <= 0 ) //&& mtlToFaceIdListMap.size() > 0)
	{	
		std::stringstream ss;
		ss << "number of vertices == 0 for the subdiv being constructed: " << i_SubdivName.m_pImpl->m_Data;
		throw sgpuException( sgpuString( ss.str().c_str() ) );
	}
	if ( mtlToFaceIdListMap.size() <=0 )
	{
		std::stringstream ss;
		ss << "number of faces == 0 for the subdiv being constructed: " << i_SubdivName.m_pImpl->m_Data;
		throw sgpuException( sgpuString( ss.str().c_str() ) );
	}
	TMtlToFaceIdListMap::const_iterator mit;
	for( mit = mtlToFaceIdListMap.begin(); mit != mtlToFaceIdListMap.end(); ++mit )
	{
		const TFaceIdList &faeList = mit->second;
		const std::string &sMtlName = mit->first;
	}
	int nOriginalFaces = m_pImpl->m_FaceTemp.size();
	int nOriginalVerts = m_pImpl->m_PositionTemp.size();

	int nNumIndicesSoFar = 0;
	int nNumFacesSoFar=0;

	sgpuSubdiv retSubdiv = m_Node.CreateNodeContent_SubdivisionSurface( m_pImpl->m_bVertexAnimation );
	retSubdiv.SetName( i_SubdivName );
	retSubdiv.SetNumFaces( nOriginalFaces );
	retSubdiv.SetNumIndices( nTotalNumIndices );


	for( mit = mtlToFaceIdListMap.begin(); mit != mtlToFaceIdListMap.end(); ++mit )
	{
		const TFaceIdList &faceList = mit->second;

		const std::string& sMtlName= mit->first;
		sgpuMaterial sMat = m_Scene.GetMaterial( sgpuString( sMtlName.c_str() ) );
		//record a material change
		retSubdiv.SetMaterialChange( nNumFacesSoFar, sMat );
		TFaceIdList::const_iterator lit;
		for( lit = faceList.begin(); lit != faceList.end(); ++lit )
		{
			int fIdx = *lit;
			assert( fIdx >= 0 && fIdx < nOriginalFaces );
			const PolyFace &face = m_pImpl->m_FaceTemp[ fIdx ];
			DBG_ASSERT( (NULL != face.m_FaceVertices ), "ith Face: " << ithFace << " of subdivMesh: " << i_SubdivName.m_pImpl->m_Data << " has zero vertices" );
			DBG_ASSERT( (0 < face.m_FaceVertices->size() ), "ith Face: " << ithFace << " of subdivMesh: " << i_SubdivName.m_pImpl->m_Data << " has zero vertices" );
			int nVerticesInFace = face.m_FaceVertices->size();
			std::vector<int> uniqueVertIndicesOfFace( nVerticesInFace, -1 );
			retSubdiv.SetNumVertsInFace( nNumFacesSoFar, nVerticesInFace );
			for( int j=0; j < nVerticesInFace ; ++j )
			{
				const FaceVertex &fv = (*face.m_FaceVertices)[j];	
				DBG_ASSERT( m_pImpl->IsValidFaceVertex( fv ), "face vertex: " <<  j << " of face: " << fIdx << " is not valid!, subdiv: " << i_SubdivName.m_pImpl->m_Data.c_str() ); 
				int vIdx = fv.m_VertexId;
				int tIdx = fv.m_UVId;
				int nIdx = -1;

				UniqueVertex uVertex( vIdx, nIdx, tIdx,  uniqueVertexSet.size() );
				std::pair< UniqueVertexSetT::iterator, bool> insertResult = uniqueVertexSet.insert( uVertex );
				//If this unique vertex is fresh					
				if( insertResult.second )
				{
					retSubdiv.SetFaceVertex( nNumFacesSoFar, j, insertResult.first->m_Index );
				} else
				{	
					retSubdiv.SetFaceVertex( nNumFacesSoFar, j, insertResult.first->m_Index );
				}
			}
			m_pImpl->m_FaceRemap.insert( std::pair< int, int > ( fIdx,  nNumFacesSoFar++ ) );
		} //for( mit = mtlIdFaceListMap.begin(); mit != mtlIdFaceListMap.end(); ++mit )

		retSubdiv.SetNumOriginalPositions( m_pImpl->m_PositionTemp.size() );
		//making the sgpusubdiv
		retSubdiv.SetNumVertices( uniqueVertexSet.size() );
		UniqueVertexSetT::const_iterator  uit;
		for( uit = uniqueVertexSet.begin(); uit != uniqueVertexSet.end(); ++uit )
		{
			const UniqueVertex &uv = *uit;
			DBG_ASSERT( (m_pImpl->IsValidVertexIndex(uv.m_PosIndex)), "PositionIndex: " << uv.m_PosIndex << " is not valid, subdiv: " << i_SubdivName.m_pImpl->m_Data );
			const sgpuVector3 &thisPos = m_pImpl->m_PositionTemp[ uv.m_PosIndex];
			retSubdiv.SetPosition(  uv.m_Index , thisPos(0), thisPos(1), thisPos(2) );
			if(  uv.m_UVIndex  >= 0 )
			{
				const sgpuVector3 &thisUV = m_pImpl->m_UVTemp[ uv.m_UVIndex ];
				retSubdiv.SetTexCoord(  uv.m_Index ,  thisUV(0), thisUV(1) );
			}
			if( bAddRemapInfo )
			{
				retSubdiv.SetVertexRemap( uv.m_Index, uv.m_PosIndex, uv.m_NormalIndex );
			}
		}
	} 

	ExtractUserDefinedProps( *this, retSubdiv.m_pImpl->m_Subdiv , m_pImpl->m_bVertexAnimation );
	return retSubdiv;
}
void  sgpuSubdivConstructor::SetPosition( const sgpuVector3 *i_pPositions, int i_NumPos )
{
	m_pImpl->m_PositionTemp.resize( i_NumPos );
	for( int i=0; i < i_NumPos; ++i)
	{
		const sgpuVector3 & curVector = i_pPositions[i];
		m_pImpl->m_PositionTemp[ i ] = sgpuVector3( curVector[0], curVector[1], curVector[2] );
	}
}
	//i_pUVs array of T-s, which contain the UV-s
	//i_NumUVs = number of UV-s
	//This data will be copied to the internal data of the sgpuMeshConstructor
void sgpuSubdivConstructor::SetUV( const sgpuVector3 *i_pUVs, int i_NumUVs )
{
	m_pImpl->m_UVTemp.resize( i_NumUVs );
	for( int i=0; i < i_NumUVs; ++i)
	{
		const sgpuVector3 & curVector = i_pUVs[i];
		m_pImpl->m_UVTemp[ i ] = sgpuVector3( curVector[0], curVector[1], 0.0f );
	}
}
void  sgpuSubdivConstructor::SetProperty(const sgpuString &i_PropertyName, const sgpuPropertyValue & i_PropertyValue)
{
	m_pImpl->SetProperty( i_PropertyName.m_pImpl->m_Data, i_PropertyValue );
}

bool sgpuSubdivConstructor::GetProperty(const sgpuString &i_PropertyName, sgpuPropertyValue &o_Property )
{
	return m_pImpl->GetProperty( i_PropertyName.m_pImpl->m_Data, o_Property );
}

#if defined( SGPU_SUPPORT_1200)
sgpuSubdiv sgpuSubdivConstructor::Construct( const  char* i_SubdivName )
{
	return Construct( sgpuString( i_SubdivName ) );
}

sgpuSubdiv sgpuSubdivConstructor::Construct( const wchar_t *i_subdivName )
{
	itString it_name( (itString::CharType*) i_subdivName );
	std::string subdivName = itStringUtil::GetStdString(it_name);
	return Construct( sgpuString( subdivName.c_str() ) );
}
#endif
