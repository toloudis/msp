/****************************************************************************\
**	mdlSubdivInfo.cpp
**
**		Contains structures for passing around subdivision data.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlSubdivInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mdlSubdivInfo::mdlSubdivInfo()
:	m_NumFaces(0),
	m_MaxEdgeCreaseLevel(0),
	m_MaxVertexCreaseLevel(0),
	m_NumOrigVertices(0)
{
	m_Flags.m_bDoubleSided = false;
	m_Flags.m_bTriangleSort = false;
	m_Flags.m_bCastsShadow = true;
	m_Flags.m_bReceivesShadow = true;
	m_Flags.m_bShadowHull = false;
	m_Flags.m_bAutoGenLowRes = true;
}

//--------------------------------------------------------------------
// Expand the indices for the faces into triangle indices
//--------------------------------------------------------------------
void mdlSubdivInfo::GenerateTriangleIndices(std::vector<envType::UInt32> &o_Indices) const
{
	o_Indices.clear();
	o_Indices.reserve(m_Indices.size()); // rough estimate

	int ind = 0;
	for (int f=0; f<m_NumFaces; f++)
	{
		int num_poly_verts = m_Indices[ind++];
		int vert0_ind = m_Indices[ind++];
		int last_ind = m_Indices[ind++];
		for (int v=2; v<num_poly_verts; v++)
		{
			// triangle fan
			o_Indices.push_back( vert0_ind );
			o_Indices.push_back( last_ind );
			last_ind = m_Indices[ind++];
			o_Indices.push_back( last_ind );
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const std::vector<maPoint3d>& mdlSubdivInfo::GetVertices()
{
	return m_Vertices;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int mdlSubdivInfo::GetNumOrigVertices()
{
	return m_NumOrigVertices;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::string mdlSubdivInfo::GetName()
{
	return m_Name;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const std::multimap<int, int>& mdlSubdivInfo::GetVertexRemap()
{
	return m_VertexRemap;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const std::vector< shared_ptr<mdlMatInfo> >& mdlSubdivInfo::GetMaterials()
{
	return m_Materials;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::vector< envType::UInt32 >* mdlSubdivInfo::GetIndices()
{
	return &m_Indices;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::vector< maPoint2d >* mdlSubdivInfo::GetUVs()
{
	return &m_UVs;
}