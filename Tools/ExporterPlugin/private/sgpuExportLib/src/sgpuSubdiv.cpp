/****************************************************************************\
**  sgpuSubdiv.cpp
**
**      sgpuSubdiv.hpp defines the sgpuSubdiv class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuSubdiv.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMaterialImpl.hpp"
#include "sgpuSubdivImpl.hpp"
#include "sgpuUtilsImpl.hpp"
#include "sgpuException.hpp"
#include "sgpuStringImpl.hpp"

#include "Core/It/itStringUtil.hpp"
#include <sstream>
#include <map>

sgpuSubdiv::sgpuSubdiv()
:m_pImpl( new sgpuSubdivImpl )	
{
	m_NodeContentType = eSubdiv;
} 	

sgpuSubdiv::sgpuSubdiv( const sgpuSubdiv &i_Subdiv ) 
:
sgpuNodeContent( i_Subdiv ),
m_pImpl(new sgpuSubdivImpl(*i_Subdiv.m_pImpl))
{ } 				

sgpuSubdiv::~sgpuSubdiv() 										
{ 																
	delete m_pImpl; 											
} 	

sgpuSubdiv& sgpuSubdiv::operator=(const sgpuSubdiv& i_CopyFrom) 	
{ 	
	sgpuNodeContent::operator=( i_CopyFrom );															
	if (&i_CopyFrom != this) 									
	{ 															
		delete m_pImpl; 										
		m_pImpl = new sgpuSubdivImpl(*i_CopyFrom.m_pImpl); 			
	} 															
	return *this; 												
}

//========================================================================
// Set name of the polygon mesh
//========================================================================
void sgpuSubdiv::SetName(const sgpuString &i_Name)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );	
	m_pImpl->m_Subdiv->m_Name = i_Name.m_pImpl->m_Data;
}

const sgpuString sgpuSubdiv::GetName( ) const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );	
	return sgpuString( m_pImpl->m_Subdiv->m_Name.c_str() );
}

void sgpuSubdiv::SetNumVertices(int i_NumVertices)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );	
	m_pImpl->m_Subdiv->m_Vertices.resize(i_NumVertices, maVector3d(0,0,0));
	m_pImpl->m_Subdiv->m_UVs.resize(i_NumVertices, maVector2d(0,0));
}

void sgpuSubdiv::SetPosition(int i_PosIdx, float i_X, float i_Y, float i_Z)
{	
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );
	INVALID_RANGE_EXCEPTION( i_PosIdx, m_pImpl->m_Subdiv->m_Vertices.size(), m_pImpl->m_Subdiv->m_Name )
	m_pImpl->m_Subdiv->m_Vertices[i_PosIdx].Set(i_X, i_Y, i_Z);
}

void sgpuSubdiv::SetTexCoord(int i_UVIdx, float i_U, float i_V)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );
	INVALID_RANGE_EXCEPTION( i_UVIdx, m_pImpl->m_Subdiv->m_UVs.size(), m_pImpl->m_Subdiv->m_Name )
	m_pImpl->m_Subdiv->m_UVs[i_UVIdx].Set(i_U, i_V);
}

int sgpuSubdiv::GetNumVertices() const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );
	int retNumVerts = m_pImpl->m_Subdiv->m_Vertices.size();
	return retNumVerts;
}




sgpuVector3 sgpuSubdiv::GetPosition( int i_PosIdx ) const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );	
	INVALID_RANGE_EXCEPTION( i_PosIdx, m_pImpl->m_Subdiv->m_Vertices.size(), m_pImpl->m_Subdiv->m_Name )
	sgpuVector3 retVec;
	const maVector3d &pos  = m_pImpl->m_Subdiv->m_Vertices[ i_PosIdx ];
	retVec(0) = pos[0];
	retVec(1) = pos[1];
	retVec(2) = pos[2];
	return retVec;
}



sgpuVector3 sgpuSubdiv::GetTexCoord( int i_UVIdx ) const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );	
	INVALID_RANGE_EXCEPTION( i_UVIdx, m_pImpl->m_Subdiv->m_UVs.size(), m_pImpl->m_Subdiv->m_Name )
	sgpuVector3 retVec;
	const maVector2d &uv  = m_pImpl->m_Subdiv->m_UVs[ i_UVIdx ];
	retVec(0) = uv[0];
	retVec(1) = uv[1];
	retVec(2) = 0;
	return retVec;
}


void sgpuSubdiv::SetNumFaces( int i_NumFaces )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );
	m_pImpl->m_Subdiv->m_NumFaces = i_NumFaces ;
	m_pImpl->m_Subdiv->m_Indices.reserve( i_NumFaces * 4 );
}


int sgpuSubdiv::GetNumFaces() const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );
	int retVal = static_cast< int > ( m_pImpl->m_Subdiv->m_NumFaces );
	return retVal;
}


int sgpuSubdiv::GetNumVertsInFace( int i_FaceIdx) const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );	
	INVALID_RANGE_EXCEPTION( i_FaceIdx, m_pImpl->m_Subdiv->m_NumFaces, m_pImpl->m_Subdiv->m_Name )
	int nIndexToFace = GetIndexToFace( i_FaceIdx );
	INVALID_RANGE_EXCEPTION( nIndexToFace, m_pImpl->m_Subdiv->m_Indices.size(), m_pImpl->m_Subdiv->m_Name )
	int retVal =  m_pImpl->m_Subdiv->m_Indices[nIndexToFace];
	return retVal;
}

void sgpuSubdiv::SetNumVertsInFace( int i_FaceIdx, int i_NumVertsInFace )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );	
	INVALID_RANGE_EXCEPTION( i_FaceIdx, m_pImpl->m_Subdiv->m_NumFaces, m_pImpl->m_Subdiv->m_Name )
	int nIndexToFace = GetIndexToFace( i_FaceIdx );
	INVALID_RANGE_EXCEPTION( nIndexToFace, m_pImpl->m_Subdiv->m_Indices.size(), m_pImpl->m_Subdiv->m_Name )
	m_pImpl->m_Subdiv->m_Indices[nIndexToFace] = i_NumVertsInFace;
}
void sgpuSubdiv::SetFaceVertex( int i_FaceIdx, int i_VertexIdxInFace, int i_VertexIdxInSubdiv )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );	
	int indexToFace = GetIndexToFace( i_FaceIdx );
	int nVertsInFace = GetNumVertsInFace( i_FaceIdx );
	INVALID_RANGE_EXCEPTION( i_VertexIdxInFace, nVertsInFace, m_pImpl->m_Subdiv->m_Name )
	int index = indexToFace + i_VertexIdxInFace + 1;
	INVALID_RANGE_EXCEPTION( index, m_pImpl->m_Subdiv->m_Indices.size() , m_pImpl->m_Subdiv->m_Name )
	m_pImpl->m_Subdiv->m_Indices[index] = i_VertexIdxInSubdiv; 
}
int sgpuSubdiv::GetFaceVertex( int i_FaceIndex, int i_VertexIdxInFace )const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );	
	int nCurIndex = GetIndexToFace( i_FaceIndex );
	int nNumVertsInFace = m_pImpl->m_Subdiv->m_Indices[ nCurIndex ];
	INVALID_RANGE_EXCEPTION( i_VertexIdxInFace, nNumVertsInFace, m_pImpl->m_Subdiv->m_Name )
	INVALID_RANGE_EXCEPTION( nCurIndex + i_VertexIdxInFace + 1,  m_pImpl->m_Subdiv->m_Indices.size(), m_pImpl->m_Subdiv->m_Name )
	int retVal =  m_pImpl->m_Subdiv->m_Indices[ nCurIndex + i_VertexIdxInFace + 1 ];
	return retVal;
}


void sgpuSubdiv::SetMaterialChange( int i_FaceIdx, const sgpuMaterial & i_Mtl )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );	
	INVALID_RANGE_EXCEPTION( i_FaceIdx, GetNumFaces(), m_pImpl->m_Subdiv->m_Name )
	if( i_FaceIdx != 0)
	{
		m_pImpl->m_Subdiv->m_MaterialChanges.push_back( i_FaceIdx );
	}
	m_pImpl->m_Subdiv->m_Materials.push_back( i_Mtl.m_pImpl->m_MatInfo );
}



void sgpuSubdiv::SetNumOriginalPositions( int i_NumOriginalPositions )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );	
	m_pImpl->m_Subdiv->m_NumOrigVertices = i_NumOriginalPositions ;
}

int sgpuSubdiv::GetNumOriginalPositions(  ) const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );	
	int retVal = m_pImpl->m_Subdiv->m_NumOrigVertices;
	return retVal;
}

void sgpuSubdiv::SetVertexRemap( int i_VertexIndexInThisMesh, int i_OriginalPositionIndex, int i_OriginalNormalIndex )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );
	m_pImpl->m_Subdiv-> m_VertexRemap.insert( std::pair<int,int>(i_OriginalPositionIndex, i_VertexIndexInThisMesh ) );	
}


int sgpuSubdiv::GetOriginalPosIdx( int i_PosIdxInSubdiv )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );
	INVALID_RANGE_EXCEPTION( i_PosIdxInSubdiv, m_pImpl->m_Subdiv->m_Vertices.size() , m_pImpl->m_Subdiv->m_Name )
	int retVal = -1;
	if( m_pImpl->m_Subdiv-> m_VertexRemap.size() > 0 )
	{
		std::multimap<int, int>::const_iterator mit;
		for( mit = m_pImpl->m_Subdiv-> m_VertexRemap.begin(); mit != m_pImpl->m_Subdiv-> m_VertexRemap.end(); ++ mit )
		{
			if( mit->second == i_PosIdxInSubdiv )
			{
				retVal = mit->first;
				break;
			}
		}
	}
	return retVal;
}

bool sgpuSubdiv::operator==( const sgpuSubdiv &i_Other)const
{
	return *m_pImpl == *i_Other.m_pImpl;
}

int sgpuSubdiv::GetIndexToFace( int i_FaceIndex ) const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );

	//Get the closest (nClosestFaceIdx, nIdxToIndicesForTheFace) pair
	//from the face index cache so that nClosestFaceIdx <= i_FaceIdx
	int nClosestFaceIdx =0;
	int nIdxToIndicesForTheFace=0;
	std::map<int, int>::iterator mit;
	//get the lower bound of the face index in the face index cache
	mit = m_pImpl->m_FaceIndexCache.lower_bound( i_FaceIndex);
	if ( mit != m_pImpl->m_FaceIndexCache.end() )
	{
		//something is present in the cache
		DBG_ASSERT( (mit->first >= i_FaceIndex), "loss of logic in getting index to face" );
		if( mit->first == i_FaceIndex )
		{
			return mit->second;
		}
		//go one iterator before
		//and we will get the nClosest
		advance( mit, -1);
		nClosestFaceIdx = mit->first;
		nIdxToIndicesForTheFace = mit->second;
	} else
	{
		//cache is empty or
		//all the elements in the cache have faceIdx < i_FaceIdx
		if( m_pImpl->m_FaceIndexCache.size() > 0)
		{
			//all the elements in the cache have faceIdx < i_FaceIdx
			//get the iter previous to the end iterator
			advance( mit, -1);
			DBG_ASSERT( (mit->first < i_FaceIndex), "loss of logic in getting index to face" );
			nClosestFaceIdx = mit->first;
			nIdxToIndicesForTheFace = mit->second;
		}
	}
	//Having got the closest (nClosestFaceIdx, nIdxToIndicesForTheFace) pair
	//from the face index cache so that nClosestFaceIdx <= i_FaceIdx,
	//iterate through the indices, till we reach the target i_FaceIndex
	while( nClosestFaceIdx < i_FaceIndex )
	{
		INVALID_RANGE_EXCEPTION( nIdxToIndicesForTheFace, m_pImpl->m_Subdiv->m_Indices.size() , m_pImpl->m_Subdiv->m_Name )
		m_pImpl->m_FaceIndexCache[ nClosestFaceIdx ] = nIdxToIndicesForTheFace;
		int nCurNumVertsInFace = m_pImpl->m_Subdiv->m_Indices[ nIdxToIndicesForTheFace ];			
		nIdxToIndicesForTheFace += nCurNumVertsInFace + 1;
		nClosestFaceIdx++;
	} 		
	INVALID_RANGE_EXCEPTION( nIdxToIndicesForTheFace, m_pImpl->m_Subdiv->m_Indices.size() , m_pImpl->m_Subdiv->m_Name )
	m_pImpl->m_FaceIndexCache[ nClosestFaceIdx ] = nIdxToIndicesForTheFace;
	int retVal = nIdxToIndicesForTheFace;
	return retVal;
}


void sgpuSubdiv::SetNumIndices(int i_NumIndices)
{	
	NO_IMPL_EXCEPTION( m_pImpl->m_Subdiv, "Subdiv" )
	DBG_ASSERT(m_pImpl->m_Subdiv, "Subdiv not implemented" );
	m_pImpl->m_Subdiv->m_Indices.resize(i_NumIndices, 0);
}


bool sgpuSubdiv::IsForVertexAnimation() const
{
	DBG_ASSERT(m_pImpl->m_Subdiv.get(), "mesh not implemented!");
	return m_pImpl->m_bVertexAnim;
}




