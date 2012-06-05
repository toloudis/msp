/****************************************************************************\
**  sgpuMesh.cpp
**
**      sgpuMesh.hpp defines the sgpuMesh class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuMesh.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMaterialImpl.hpp"
#include "sgpuMeshImpl.hpp"
#include "sgpuUtilsImpl.hpp"
#include "sgpuException.hpp"
#include "sgpuStringImpl.hpp"

#include "Core/It/itStringUtil.hpp"
#include <map>


//========================================================================
// constructors, destructors, assignment operator
//========================================================================
sgpuMesh::sgpuMesh()
	:m_pImpl(new sgpuMeshImpl )	
{
	m_NodeContentType = eMesh;
} 	

sgpuMesh::sgpuMesh( const sgpuMesh &i_Mesh ) 					
: sgpuNodeContent(i_Mesh),m_pImpl(new sgpuMeshImpl(*i_Mesh.m_pImpl)) { } 				

sgpuMesh::~sgpuMesh() 										
{ 																
	delete m_pImpl; 											
} 	

sgpuMesh& sgpuMesh::operator=(const sgpuMesh& i_CopyFrom) 	
{ 		
	sgpuNodeContent::operator=( i_CopyFrom );
	if (&i_CopyFrom != this) 									
	{ 															
		delete m_pImpl; 										
		m_pImpl = new sgpuMeshImpl(*i_CopyFrom.m_pImpl); 			
	} 															
	return *this; 												
}

//========================================================================
// set name,
// If the name in the source data ( eg: Max, Maya ) is internationalized,
// then the name sould be converted to a utf-8 string and passed in as an 
// std::string
//========================================================================
void sgpuMesh::SetName( const sgpuString &i_Name )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	m_pImpl->m_Mesh->m_Name = i_Name.m_pImpl->m_Data;
}
const sgpuString sgpuMesh::GetName() const 
{	
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	return sgpuString( m_pImpl->m_Mesh->m_Name.c_str() );
}
//========================================================================
// Set/Get POsition, normal and uv-s of vertices
//========================================================================


void sgpuMesh::SetNumVertices(int i_NumVertices)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	m_pImpl->m_Mesh->m_Vertices.resize(i_NumVertices, maVector3d(0,0,0));
	m_pImpl->m_Mesh->m_Normals.resize(i_NumVertices, maVector3d(0,1,0));
	m_pImpl->m_Mesh->m_UVs.resize(i_NumVertices, maVector2d(0,0));
}

void sgpuMesh::SetPosition(int i_PosIdx, float i_X, float i_Y, float i_Z)
{	
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	INVALID_RANGE_EXCEPTION( i_PosIdx, m_pImpl->m_Mesh->m_Vertices.size(), m_pImpl->m_Mesh->m_Name )
	m_pImpl->m_Mesh->m_Vertices[i_PosIdx].Set(i_X, i_Y, i_Z);
}

void sgpuMesh::SetNormal(int i_NormalIdx, float i_X, float i_Y, float i_Z)
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	INVALID_RANGE_EXCEPTION( i_NormalIdx, m_pImpl->m_Mesh->m_Normals.size(), m_pImpl->m_Mesh->m_Name )
	maVector3d normal(i_X, i_Y, i_Z);
	if (!normal.Normalize())
		normal.Set(0,1,0);
	m_pImpl->m_Mesh->m_Normals[i_NormalIdx] = normal;
}

// Throws exception if the m_pImpl->m_Mesh is NULL
void sgpuMesh::SetTexCoord(int i_UVIndex, float i_U, float i_V)
{	
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	INVALID_RANGE_EXCEPTION( i_UVIndex, m_pImpl->m_Mesh->m_UVs.size(), m_pImpl->m_Mesh->m_Name )
	m_pImpl->m_Mesh->m_UVs[i_UVIndex].Set(i_U, i_V);
}

// Throws exception if the m_pImpl->m_Mesh is NULL
int sgpuMesh::GetNumVertices() const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	int retNumVerts =  m_pImpl->m_Mesh->m_Vertices.size();
	return retNumVerts;
}
		

bool sgpuMesh::IsForVertexAnimation() const
{
	DBG_ASSERT(m_pImpl->m_Mesh.get(), "mesh not implemented!");
	return m_pImpl->m_Mesh->m_Flags.m_bVertexAnimation;
}


sgpuVector3 sgpuMesh::GetPosition( int i_PosIdx ) const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	INVALID_RANGE_EXCEPTION( i_PosIdx, m_pImpl->m_Mesh->m_Vertices.size(), m_pImpl->m_Mesh->m_Name )
	sgpuVector3 retVec;
	const maVector3d &pos  = m_pImpl->m_Mesh->m_Vertices[ i_PosIdx ];
	retVec(0) = pos[0];
	retVec(1) = pos[1];
	retVec(2) = pos[2];
	return retVec;
}


sgpuVector3 sgpuMesh::GetNormal( int i_NormIdx ) const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );	
	INVALID_RANGE_EXCEPTION( i_NormIdx, m_pImpl->m_Mesh->m_Normals.size(), m_pImpl->m_Mesh->m_Name )
	sgpuVector3 retVec;
	const maVector3d &pos  = m_pImpl->m_Mesh->m_Normals[ i_NormIdx ];
	retVec(0) = pos[0];
	retVec(1) = pos[1];
	retVec(2) = pos[2];
	return retVec;
}

sgpuVector3 sgpuMesh::GetTexCoord( int i_UVIdx ) const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );	
	INVALID_RANGE_EXCEPTION( i_UVIdx, m_pImpl->m_Mesh->m_UVs.size(), m_pImpl->m_Mesh->m_Name )
	sgpuVector3 retVec;
	const maVector2d &uv  = m_pImpl->m_Mesh->m_UVs[ i_UVIdx ];
	retVec(0) = uv[0];
	retVec(1) = uv[1];
	retVec(2) = 0;
	return retVec;
}


//========================================================================
// Set/Get Face assignments
//========================================================================


void sgpuMesh::SetNumFaces( int i_NumFaces )
{	
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	m_pImpl->m_Mesh->m_Indices.resize( i_NumFaces*3, 0);
}

int sgpuMesh::GetNumFaces() const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT( m_pImpl->m_Mesh, "Mesh not implemented" );	
	DBG_ASSERT( ( ( (m_pImpl->m_Mesh->m_Indices.size() ) % 3 ) == 0 ), "Number of indices : " << m_pImpl->m_Mesh->m_Indices.size() << " not divisible by 3, context mesh: " << m_pImpl->m_Mesh->m_Name );
	int retVal = static_cast< int > ( m_pImpl->m_Mesh->m_Indices.size() ) / 3;
	return retVal;
}

int sgpuMesh::GetNumMaterials()
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );	
	return static_cast< int > ( m_pImpl->m_Mesh->m_Materials.size() );
}

int sgpuMesh::GetMaterialChange( int i_MtlIdx, sgpuString &o_MtlName )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );		
	INVALID_RANGE_EXCEPTION( i_MtlIdx, m_pImpl->m_Mesh->m_Materials.size() , m_pImpl->m_Mesh->m_Name )
	shared_ptr<mdlMatInfo>  mtlInfo = m_pImpl->m_Mesh->m_Materials[i_MtlIdx];
	std::string smtlName = mtlInfo->m_Info.GetMaterialName();
	o_MtlName = sgpuString( smtlName.c_str() );
	if( i_MtlIdx == 0)
		return 0;
	else
		return m_pImpl->m_Mesh->m_MaterialChanges[i_MtlIdx-1];
}

	//
void sgpuMesh::SetMaterialChange( int i_FaceIdx, const sgpuMaterial & i_Mtl )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	if( i_FaceIdx != 0)
	{
		m_pImpl->m_Mesh->m_MaterialChanges.push_back( i_FaceIdx );
	}
	m_pImpl->m_Mesh->m_Materials.push_back( i_Mtl.m_pImpl->m_MatInfo );
}

void sgpuMesh::SetFaceVertex( int i_FaceIdx, int i_VertexIdxInFace, int i_VertexIdxInMesh )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	INVALID_RANGE_EXCEPTION( i_VertexIdxInFace, 3, m_pImpl->m_Mesh->m_Name )	
	int index =   i_FaceIdx *3 + i_VertexIdxInFace;
	INVALID_RANGE_EXCEPTION( index, m_pImpl->m_Mesh->m_Indices.size(), m_pImpl->m_Mesh->m_Name )
	m_pImpl->m_Mesh->m_Indices[index] = i_VertexIdxInMesh;
}

int sgpuMesh::GetFaceVertex( int i_FaceIdx, int i_VIdx )const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );	
	INVALID_RANGE_EXCEPTION( i_VIdx, 3, m_pImpl->m_Mesh->m_Name )
	INVALID_RANGE_EXCEPTION( i_FaceIdx * 3 + i_VIdx,  m_pImpl->m_Mesh->m_Indices.size(), m_pImpl->m_Mesh->m_Name )
	int  retVal = retVal = m_pImpl->m_Mesh->m_Indices[ 3 * i_FaceIdx + i_VIdx ];
	return retVal;

}

//========================================================================
// Remapping of vertices
//========================================================================
void sgpuMesh::SetNumOriginalPositions( int i_NumOriginalVertices )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	m_pImpl->m_Mesh->m_NumOrigVertices = i_NumOriginalVertices ;
}

void sgpuMesh::SetNumOriginalNormals( int i_NumOriginalNormals )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	m_pImpl->m_Mesh->m_NumOrigNormals = i_NumOriginalNormals ;
}

void sgpuMesh::SetVertexRemap( int i_VertexIdxInThisMesh, int i_OriginalPositionIdx, int i_OriginalNormalIdx )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	m_pImpl->m_Mesh-> m_VertexRemap.insert( std::pair<int,int>(i_OriginalPositionIdx, i_VertexIdxInThisMesh ) );
	m_pImpl->m_Mesh-> m_NormalRemap.insert( std::pair<int,int>(i_OriginalNormalIdx, i_VertexIdxInThisMesh ) );
}


int sgpuMesh::GetOriginalPosIdx( int i_VertIdxInsgpuMesh )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );	
	int retVal = -1;
	INVALID_RANGE_EXCEPTION( i_VertIdxInsgpuMesh, m_pImpl->m_Mesh->m_Vertices.size(), m_pImpl->m_Mesh->m_Name )
	if( m_pImpl->m_Mesh-> m_VertexRemap.size() > 0 )
	{
		std::multimap<int, int>::const_iterator mit;
		for( mit = m_pImpl->m_Mesh-> m_VertexRemap.begin(); mit != m_pImpl->m_Mesh-> m_VertexRemap.end(); ++ mit )
		{
			if( mit->second == i_VertIdxInsgpuMesh )
			{
				retVal = mit->first;
				break;
			}
		}
	}
	return retVal;
}

int sgpuMesh::GetOriginalNormalIdx( int i_VertIdxInsgpuMesh )
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );	
	INVALID_RANGE_EXCEPTION( i_VertIdxInsgpuMesh, m_pImpl->m_Mesh->m_Vertices.size(), m_pImpl->m_Mesh->m_Name )
	int retVal = -1;
	if( m_pImpl->m_Mesh-> m_NormalRemap.size() > 0 )
	{
		std::multimap<int, int>::const_iterator mit;
		for( mit = m_pImpl->m_Mesh-> m_NormalRemap.begin(); mit != m_pImpl->m_Mesh-> m_NormalRemap.end(); ++ mit )
		{
			if( mit->second == i_VertIdxInsgpuMesh )
			{
				retVal = mit->first;
				break;
			}
		}			
	}
	return retVal;
}

int sgpuMesh::GetNumOriginalPositions(  ) const
{	
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	int retVal = m_pImpl->m_Mesh->m_NumOrigVertices;
	return retVal;
}

int sgpuMesh::GetNumOriginalNormals(  ) const
{
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );	
	int retVal = m_pImpl->m_Mesh->m_NumOrigNormals;
	return retVal;
}

bool sgpuMesh::operator== ( const sgpuMesh &other ) const
{
	DBG_ASSERT( m_pImpl, "pImpl should not be NULL"  );	
	DBG_ASSERT( other.m_pImpl , "pImpl should not be NULL" );
	return *m_pImpl == *other.m_pImpl;
}


#if defined( SGPU_SUPPORT_1200)

//========================================================================
// Obsolete functions
//========================================================================
void sgpuMesh::AssignMaterial(const sgpuMaterial& i_Material)
{
	REPORT_OBSOLETE( "sgpuMesh::AssignMaterial" )
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );	
	NO_IMPL_EXCEPTION( i_Material.m_pImpl->m_MatInfo, "Material" )
	m_pImpl->m_Mesh->m_Materials.resize(1);
	m_pImpl->m_Mesh->m_Materials[0] = i_Material.m_pImpl->m_MatInfo;
}

void sgpuMesh::SetMeshName(const char* i_Name)
{
	SetName( sgpuString( i_Name ) );
}

void sgpuMesh::SetMeshName(const wchar_t* i_Name)
{
	SetName( sgpuString( i_Name ) );
}

void sgpuMesh::SetNumIndices(int i_NumIndices)
{	REPORT_OBSOLETE( "sgpuMesh::SetNumIndices" )
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	m_pImpl->m_Mesh->m_Indices.resize(i_NumIndices, 0);
}

void sgpuMesh::SetIndex(int i_Idx, int i_VertexIdx)
{
	REPORT_OBSOLETE( "sgpuMesh::SetIndex" )
	NO_IMPL_EXCEPTION( m_pImpl->m_Mesh, "Mesh" )
	DBG_ASSERT(m_pImpl->m_Mesh, "Mesh not implemented" );
	INVALID_RANGE_EXCEPTION( i_Idx, m_pImpl->m_Mesh->m_Indices.size(), m_pImpl->m_Mesh->m_Name )
	m_pImpl->m_Mesh->m_Indices[i_Idx] = i_VertexIdx;
}
#endif //SGPU_SUPPORT_1200
