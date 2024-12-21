/****************************************************************************\
**	mdlFragInfo.cpp
**
**		Contains structures for passing around fragment data.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlFragInfo.hpp"

#include "Core/ma/maSTLHelpers.hpp"

#include <algorithm>
#include <iterator>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlFragInfo::mdlFragInfo()
:	m_ResolutionLevel(0), 
	m_NumOrigVertices(0), 
	m_NumOrigNormals(0)
{
	m_Flags.m_bCastsShadow = true;
	m_Flags.m_bReceivesShadow = true;
	m_Flags.m_bBumpMap = false;
	m_Flags.m_bShadowHull = false;
	m_Flags.m_bDoubleSided = false;
	m_Flags.m_bTriangleSort = false;
	m_Flags.m_bVertexAnimation = false;
}

//------------------------------------------------------------------------
//	AddGeometry causes the geometry information from i_ToAdd to be added
//	to this.  All indices will be modified properly, etc.
//------------------------------------------------------------------------
void mdlFragInfo::AddGeometry(const mdlFragInfo& i_ToAdd)
{
	int vertex_base = m_Vertices.size();
	int index_base = m_Indices.size();

	std::transform(	i_ToAdd.m_Indices.begin(),
		i_ToAdd.m_Indices.end(),
		std::back_inserter(m_Indices),
		maAdder(vertex_base));

	std::copy(	i_ToAdd.m_Vertices.begin(),
		i_ToAdd.m_Vertices.end(),
		std::back_inserter(m_Vertices));

	std::copy(	i_ToAdd.m_Normals.begin(),
		i_ToAdd.m_Normals.end(),
		std::back_inserter(m_Normals));

	//	if there are UVs in i_ToAdd but we didn't have any before,
	//	fill our array up to the right index
	if ( (i_ToAdd.m_UVs.size() > 0) && (m_UVs.size() < vertex_base) )
		m_UVs.resize(vertex_base);

	std::copy(	i_ToAdd.m_UVs.begin(),
		i_ToAdd.m_UVs.end(),
		std::back_inserter(m_UVs));

	//	if there are vertex colors in i_ToAdd but we didn't have any before,
	//	fill our array up to the right index
	//if ( (i_ToAdd.m_Colors.size() > 0) && (m_Colors.size() < vertex_base) )
	//	m_Colors.resize(vertex_base, maFloatRGBA(1,1,1,1));

	//std::copy(	i_ToAdd.m_Colors.begin(),
	//			i_ToAdd.m_Colors.end(),
	//			std::back_inserter(m_Colors));

	int material_base = m_Materials.size();
	if ( index_base > 0 )
	{
		//	if this isn't the first data added, add an material change here
		m_MaterialChanges.push_back(index_base / 3);
	}

	std::transform(	i_ToAdd.m_MaterialChanges.begin(),
		i_ToAdd.m_MaterialChanges.end(),
		std::back_inserter(m_MaterialChanges),
		maAdder(index_base / 3));

	std::copy(	i_ToAdd.m_Materials.begin(),
		i_ToAdd.m_Materials.end(),
		std::back_inserter(m_Materials));
}

//------------------------------------------------------------------------
//	AddGeometry adds the geometry from a single material mesh to the
// given fragment which is assumed to have the same material,
//------------------------------------------------------------------------
void mdlFragInfo::AddGeometry(const mdlFragInfo& i_ToAdd, 
							  const maMatrix4x4 * i_pTransformToBeAppliedBeforeMerging )
{
	DBG_ASSERT( \
		(	i_ToAdd.m_Materials.size() == 1 && \
			m_Materials.size() == 1 && \
			i_ToAdd.m_Materials[0] == m_Materials[0]\
			), "Use this routine only if the input geometry and this geometry has  a single identical material" );
	if ((i_ToAdd.m_Materials.size() != 1) || (m_Materials.size() != 1) || (i_ToAdd.m_Materials[0] != m_Materials[0]))
		return;

	//'if A, then B' is logically equivalent to 'not(A) or B'
	//If the input frag has basis vectors, then this frag should either have basis vector or be empty.
	DBG_ASSERT( (!i_ToAdd.HasValidBasisVectors() || ( HasValidBasisVectors() || m_Vertices.size() == 0 ) ),\
			"Input mesh has basis vectors and is possibly merged with a frag that doesnt have basis vectors! - " << i_ToAdd.m_Name.c_str() );
	//If the input frag does not have basis vectors, (that is possible if the input frag does not have any UV-s),
	//then this frag should not have basis vectors or be empty.
	DBG_ASSERT( (i_ToAdd.HasValidBasisVectors() || ( !HasValidBasisVectors() || m_Vertices.size() == 0 ) ),\
			"Input mesh does not have basis vectors and is possibly merged with a frag that has basis vectors! - " << i_ToAdd.m_Name.c_str() );

	bool bEnableBasisVectorComputation = i_ToAdd.HasValidBasisVectors();
	//calculate the transpose of the inverse matrix
	maMatrix4x4 transposeInverse;	
	maMatrix4x4 justRotationAndScale;
	bool bIsTransformIdentity = false;
	if ( i_pTransformToBeAppliedBeforeMerging )
	{
		bIsTransformIdentity = i_pTransformToBeAppliedBeforeMerging->IsIdentity();
		maVector3d translation = i_pTransformToBeAppliedBeforeMerging->GetTranslation();
		translation = -translation;
		justRotationAndScale = *i_pTransformToBeAppliedBeforeMerging;
		justRotationAndScale.TranslateBy( translation );
		transposeInverse= justRotationAndScale ;
		transposeInverse.Invert();
		transposeInverse.Transpose();
	}
	//current bases
	int vertex_base = m_Vertices.size();
	int index_base = m_Indices.size();

	int num_new_indices = i_ToAdd.m_Indices.size();
	m_Indices.resize( m_Indices.size() + num_new_indices );
	if (index_base == 0)
	{
		::memcpy(&(m_Indices[0]), &(i_ToAdd.m_Indices[0]), num_new_indices*sizeof(envType::UInt32));
	}
	else
	{
		envType::UInt32 *pInd = &(m_Indices[index_base]);
		for (int i=0; i<num_new_indices; i++, pInd++)
		{
			*pInd = vertex_base+i_ToAdd.m_Indices[ i ];
		}
	}

	// reserve enough memory for the new set of vertices and normals
	// corresponding to the partial geometry
	int num_new_verts = i_ToAdd.m_Vertices.size();
	m_Vertices.resize( vertex_base + num_new_verts );
	m_Normals.resize( vertex_base + num_new_verts );
	if ( bEnableBasisVectorComputation )
	{
		m_Ts.resize( vertex_base + num_new_verts );
		m_Ss.resize( vertex_base + num_new_verts ); 
	}
	//	if there are UVs in i_ToAdd but we didn't have any before,
	//	fill our array up to the right index
	int num_new_uvs = i_ToAdd.m_UVs.size();
	if (num_new_uvs > 0)
		m_UVs.resize( vertex_base + num_new_uvs );

	// core partial geometry merging
	maPoint3d *pVert = &(m_Vertices[vertex_base]);
	maPoint3d *pNormal = &(m_Normals[vertex_base]);
	
	maPoint3d *pTs = NULL;
	maPoint3d *pSs = NULL;
	if ( bEnableBasisVectorComputation )
	{
		pTs = &(m_Ts[vertex_base]);
		pSs = &(m_Ss[vertex_base]);
	}
	if ( i_pTransformToBeAppliedBeforeMerging && !bIsTransformIdentity  )
	{
		if ( bEnableBasisVectorComputation )
		{
			for (int i=0; i<num_new_verts; i++, pVert++, pNormal++, pTs++, pSs++)
			{
				*pVert = *i_pTransformToBeAppliedBeforeMerging* i_ToAdd.m_Vertices[ i ];
				*pNormal = transposeInverse * i_ToAdd.m_Normals[ i ];
				*pTs = justRotationAndScale* i_ToAdd.m_Ts[ i ];
				*pSs = justRotationAndScale* i_ToAdd.m_Ss[ i ];
			}
		} else
		{
			for (int i=0; i<num_new_verts; i++, pVert++, pNormal++)
			{
				*pVert = *i_pTransformToBeAppliedBeforeMerging* i_ToAdd.m_Vertices[ i ];
				*pNormal = transposeInverse * i_ToAdd.m_Normals[ i ];
			}
		}
	} 
	else
	{
		::memcpy(pVert, &(i_ToAdd.m_Vertices[0]), num_new_verts*sizeof(maPoint3d));
		::memcpy(pNormal, &(i_ToAdd.m_Normals[0]), num_new_verts*sizeof(maPoint3d));
		if ( bEnableBasisVectorComputation )
		{
			::memcpy(pTs,  &(i_ToAdd.m_Ts[0]), num_new_verts*sizeof(maPoint3d));
			::memcpy(pSs,  &(i_ToAdd.m_Ss[0]), num_new_verts*sizeof(maPoint3d));
		}
	}
	if ( num_new_uvs  > 0)
	{	
		::memcpy(&m_UVs[vertex_base], &(i_ToAdd.m_UVs[0]), num_new_verts*sizeof(maPoint2d));
	}

	//numOriginal vertices is updated to the current number of vertices
	m_NumOrigVertices += num_new_verts;
	m_NumOrigNormals += num_new_verts;

	//update m_Parts
	PartComponent part;
	part.m_Name = i_ToAdd.m_Name;
	part.m_PartBeginIndex = index_base;
	m_Parts.push_back( part );
}

//------------------------------------------------------------------------
//	AddGeometry causes the partial and contiguous geometry information 
//	from i_ToAdd to be added to this. 
//------------------------------------------------------------------------
void mdlFragInfo::AddGeometry(const mdlFragInfo& i_ToAdd, 
							  int i_BeginIdx, int i_EndIdx,
							  const maMatrix4x4 * i_pTransformToBeAppliedBeforeMerging )
{
	//'if A, then B' is logically equivalent to 'not(A) or B'
	//If the input frag has basis vectors, then this frag should either have basis vector or be empty.
	DBG_ASSERT( (!i_ToAdd.HasValidBasisVectors() || ( HasValidBasisVectors() || m_Vertices.size() == 0 ) ),\
			"Input mesh has basis vectors and is possibly merged with a frag that doesnt have basis vectors! - " << i_ToAdd.m_Name.c_str() );
	//If the input frag does not have basis vectors, (that is possible if the input frag does not have any UV-s),
	//then this frag should not have basis vectors or be empty.
	DBG_ASSERT( (i_ToAdd.HasValidBasisVectors() || ( !HasValidBasisVectors() || m_Vertices.size() == 0 ) ),\
			"Input mesh does not have basis vectors and is possibly merged with a frag that has basis vectors! - " << i_ToAdd.m_Name.c_str() );

	//calculate the transpose of the inverse matrix
	maMatrix4x4 transposeInverse;
	bool bIsTransformIdentity = false;		
	maMatrix4x4 justRotationAndScale;
	if ( i_pTransformToBeAppliedBeforeMerging )
	{
		bIsTransformIdentity = i_pTransformToBeAppliedBeforeMerging->IsIdentity();		
		maVector3d translation = i_pTransformToBeAppliedBeforeMerging->GetTranslation();
		translation = -translation;
		justRotationAndScale = *i_pTransformToBeAppliedBeforeMerging;
		justRotationAndScale.TranslateBy( translation );
		transposeInverse=  justRotationAndScale ;
		transposeInverse.Invert();
		transposeInverse.Transpose();
	}
	//current bases
	int vertex_base = m_Vertices.size();
	int index_base = m_Indices.size();
	const std::string &sMeshName_ToAdd = i_ToAdd.m_Name;
	//iterator to vertex index in i_ToAdd coresponding to the triangle i_BeginIdx
	std::vector<envType::UInt32>::const_iterator indexBeginIt_ToAdd(i_ToAdd.m_Indices.begin());	
	std::advance( indexBeginIt_ToAdd, i_BeginIdx * 3 );	
	//iterator to vertex index in i_ToAdd coresponding to the triangle i_EndIdx
	std::vector<envType::UInt32>::const_iterator indexEndIt_ToAdd( i_ToAdd.m_Indices.begin());	
	std::advance( indexEndIt_ToAdd, i_EndIdx * 3 );

	std::vector<envType::UInt32>::const_iterator iit;
	//collect all the vertex indices corresponding to the partial geometry
	//in i_ToAdd
	std::vector< envType::UInt32 > indicesUsedBy_ToAdd;
	indicesUsedBy_ToAdd.reserve( 3* (i_EndIdx - i_BeginIdx) );

	for ( iit = indexBeginIt_ToAdd; iit < indexEndIt_ToAdd; ++iit )
	{
		// check if the current vertex index pointed to by 'iit' is already there in indicesUsedBy_ToAdd
		//else add it
		std::vector< envType::UInt32 >::const_iterator fit = std::find( indicesUsedBy_ToAdd.begin(), indicesUsedBy_ToAdd.end(), *iit );
		if ( fit == indicesUsedBy_ToAdd.end() )
		{
			indicesUsedBy_ToAdd.push_back( *iit );
			fit = indicesUsedBy_ToAdd.begin();
			std::advance( fit, indicesUsedBy_ToAdd.size() -1 );
		}
		//now 'fit' points to the current vertex index pointed to by 'it'
		int newIdx = static_cast< int > ( std::distance< std::vector< envType::UInt32 >::const_iterator >( indicesUsedBy_ToAdd.begin(), fit ) );
		//newIdx + vertex_base will be new index in 'this'
		DBG_ASSERT( ( newIdx >= 0 && newIdx < indicesUsedBy_ToAdd.size() ), "index out of range, while merging " << sMeshName_ToAdd.c_str() );
		m_Indices.push_back( newIdx + vertex_base );
	}
	std::vector< envType::UInt32 >::const_iterator vit;
	//reserve enough memory for the new set of vertices and normals
	//corresponding to the partial geometry
	int numNewVerticesToBeAdded = static_cast< int > ( indicesUsedBy_ToAdd.size() );
	m_Vertices.reserve( m_Vertices.size() + numNewVerticesToBeAdded );
	m_Normals.reserve( m_Normals.size() + numNewVerticesToBeAdded) ;
	bool bEnableBasisVectorComputation = i_ToAdd.HasValidBasisVectors();
	if ( bEnableBasisVectorComputation )
	{
		m_Ts.reserve( m_Ts.size() + indicesUsedBy_ToAdd.size()) ;
		m_Ss.reserve( m_Ss.size() + indicesUsedBy_ToAdd.size()) ;
	}
	//	if there are UVs in i_ToAdd but we didn't have any before,
	//	fill our array up to the right index
	if ( (i_ToAdd.m_UVs.size() > 0) && (m_UVs.size() < vertex_base) )
	{
		m_UVs.resize(vertex_base);
	}
	if ( (i_ToAdd.m_UVs.size() > 0) )
	{
		m_UVs.reserve( vertex_base + indicesUsedBy_ToAdd.size() );
	}

	//core partial geometry merging
	if ( i_pTransformToBeAppliedBeforeMerging  && !bIsTransformIdentity )
	{
		if ( bEnableBasisVectorComputation )
		{
			for ( vit = indicesUsedBy_ToAdd.begin(); vit != indicesUsedBy_ToAdd.end(); ++vit )
			{
				m_Vertices.push_back( *i_pTransformToBeAppliedBeforeMerging* i_ToAdd.m_Vertices[ *vit ] );
				m_Normals.push_back( transposeInverse * i_ToAdd.m_Normals[ *vit ] );
				m_Ss.push_back( justRotationAndScale* i_ToAdd.m_Ss[ *vit ] );
				m_Ts.push_back( justRotationAndScale* i_ToAdd.m_Ts[ *vit ] );
			}
		} else
		{
			for ( vit = indicesUsedBy_ToAdd.begin(); vit != indicesUsedBy_ToAdd.end(); ++vit )
			{
				m_Vertices.push_back( *i_pTransformToBeAppliedBeforeMerging* i_ToAdd.m_Vertices[ *vit ] );
				m_Normals.push_back( transposeInverse * i_ToAdd.m_Normals[ *vit ] );
			}
		}
	} else
	{
		if ( bEnableBasisVectorComputation )
		{
			for ( vit = indicesUsedBy_ToAdd.begin(); vit != indicesUsedBy_ToAdd.end(); ++vit )
			{
				m_Vertices.push_back( i_ToAdd.m_Vertices[ *vit ] );
				m_Normals.push_back( i_ToAdd.m_Normals[ *vit ] );
				m_Ss.push_back( i_ToAdd.m_Ss[ *vit ] );
				m_Ts.push_back( i_ToAdd.m_Ts[ *vit ] );
			}
		} else
		{
			for ( vit = indicesUsedBy_ToAdd.begin(); vit != indicesUsedBy_ToAdd.end(); ++vit )
			{
				m_Vertices.push_back( i_ToAdd.m_Vertices[ *vit ] );
				m_Normals.push_back( i_ToAdd.m_Normals[ *vit ] );
			}
		}
	}

	if ( i_ToAdd.m_UVs.size() > 0)
	{	
		//UV merging
		for ( vit = indicesUsedBy_ToAdd.begin(); vit != indicesUsedBy_ToAdd.end(); ++vit )
		{
			m_UVs.push_back( i_ToAdd.m_UVs[ *vit ] );
		}
	}
	//numOriginal vertices is updated to the current number of vertices
	m_NumOrigVertices += static_cast< int > ( indicesUsedBy_ToAdd.size() );
	m_NumOrigNormals += static_cast< int > ( indicesUsedBy_ToAdd.size() );
	//update m_Parts
	PartComponent part;
	part.m_Name = sMeshName_ToAdd;
	part.m_PartBeginIndex = index_base;
	m_Parts.push_back( part );
}

//------------------------------------------------------------------------
//	ApplyTransformation
//	Transform the positions of this geometry by  i_PosTransform,
//	Transform the nornals of this geometry by i_NormalTransform
//------------------------------------------------------------------------

//Embedded functor  class used for std::transform call below in
//ApplyTransformation.
struct mdlFragInfo::TransformVec
{
	TransformVec( const maMatrix4x4 &trans ):
	m_Transform(trans)
	{}
	maPoint3d operator() ( maPoint3d &i_Point )
	{
		return  m_Transform * i_Point;
	}
	maMatrix4x4 m_Transform;
};

//----------------------------------------------------------------------------
//apply the transformations to this geometry
//----------------------------------------------------------------------------
void mdlFragInfo::ApplyTransformation( const maMatrix4x4 &i_PosTransform,  const maMatrix4x4 &i_NormalTransform )
{	
	TransformVec posTrans ( i_PosTransform );
	TransformVec normalTrans( i_NormalTransform );
	transform( m_Vertices.begin(),m_Vertices.end(),m_Vertices.begin(), posTrans);	
	transform( m_Normals.begin(),m_Normals.end(),m_Normals.begin(), normalTrans );
}

//------------------------------------------------------------------------
//	GetIndexRange - Get start index and num indices for given
//	material index
//------------------------------------------------------------------------
void mdlFragInfo::GetIndexRange(int i_MatNum,
								int& o_FirstIndex,
								int& o_NumIndices) const
{
	if ( i_MatNum == 0 )
		o_FirstIndex = 0;
	else
		o_FirstIndex = m_MaterialChanges[i_MatNum - 1] * 3;

	int last_index;

	if ( i_MatNum == (m_Materials.size() - 1) )
		last_index = m_Indices.size();
	else
		last_index = m_MaterialChanges[i_MatNum] * 3;

	o_NumIndices = last_index - o_FirstIndex;
}

//----------------------------------------------------------------------------
//HAs this frag info got valid basis vectors.
//Note that som times it is not able to calculate the basis vectors
//because no uv-s
//----------------------------------------------------------------------------
bool mdlFragInfo::HasValidBasisVectors() const
{
	size_t ssize = m_Ss.size();
	size_t tsize = m_Ts.size();
	size_t vsize = m_Vertices.size();
	return ( vsize > 0 && ssize == vsize && tsize==vsize ) ? true: false;
}
