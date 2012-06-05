/****************************************************************************\
**  mdlFragInfo.cpp
**
**      Contains structures for passing around fragment data.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/mdl/mdlFragInfo.hpp"

//#include "Core/ma/maSTLHelpers.hpp"

#include <algorithm>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlFragInfo::mdlFragInfo()
: m_ResolutionLevel(0), 
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
//void mdlFragInfo::AddGeometry(const mdlFragInfo& i_ToAdd)
//{
//	int vertex_base = m_Vertices.size();
//	int index_base = m_Indices.size();
//
//	std::transform(	i_ToAdd.m_Indices.begin(),
//					i_ToAdd.m_Indices.end(),
//					std::back_inserter(m_Indices),
//					maAdder(vertex_base));
//
//	std::copy(	i_ToAdd.m_Vertices.begin(),
//				i_ToAdd.m_Vertices.end(),
//				std::back_inserter(m_Vertices));
//
//	std::copy(	i_ToAdd.m_Normals.begin(),
//				i_ToAdd.m_Normals.end(),
//				std::back_inserter(m_Normals));
//
//	//	if there are UVs in i_ToAdd but we didn't have any before,
//	//	fill our array up to the right index
//	if( (i_ToAdd.m_UVs.size() > 0) && (m_UVs.size() < vertex_base) )
//		m_UVs.resize(vertex_base);
//
//	std::copy(	i_ToAdd.m_UVs.begin(),
//				i_ToAdd.m_UVs.end(),
//				std::back_inserter(m_UVs));
//
//	//	if there are vertex colors in i_ToAdd but we didn't have any before,
//	//	fill our array up to the right index
//	if( (i_ToAdd.m_Colors.size() > 0) && (m_Colors.size() < vertex_base) )
//		m_Colors.resize(vertex_base, maFloatRGBA(1,1,1,1));
//
//	std::copy(	i_ToAdd.m_Colors.begin(),
//				i_ToAdd.m_Colors.end(),
//				std::back_inserter(m_Colors));
//
//	int material_base = m_Materials.size();
//	if( index_base > 0 )
//	{
//		//	if this isn't the first data added, add an material change here
//		m_MaterialChanges.push_back(index_base / 3);
//	}
//
//	std::transform(	i_ToAdd.m_MaterialChanges.begin(),
//					i_ToAdd.m_MaterialChanges.end(),
//					std::back_inserter(m_MaterialChanges),
//					maAdder(index_base / 3));
//
//	std::copy(	i_ToAdd.m_Materials.begin(),
//				i_ToAdd.m_Materials.end(),
//				std::back_inserter(m_Materials));
//}

//------------------------------------------------------------------------
//	GetIndexRange - Get start index and num indices for given
//	material index
//------------------------------------------------------------------------
void mdlFragInfo::GetIndexRange(int i_MatNum,
								int& o_FirstIndex,
								int& o_NumIndices) const
{
	if( i_MatNum == 0 )
		o_FirstIndex = 0;
	else
		o_FirstIndex = m_MaterialChanges[i_MatNum - 1] * 3;

	int last_index;

	if( i_MatNum == (m_MaterialNames.size() - 1) )
		last_index = m_Indices.size();
	else
		last_index = m_MaterialChanges[i_MatNum] * 3;

	o_NumIndices = last_index - o_FirstIndex;
}
