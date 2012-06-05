/****************************************************************************\
**	mdlSubdivInfo.hpp
**
**		Contains structures for passing around subdivision data.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_SUBDIVINFO_HPP
#error mdlSubdivInfo.hpp multiply included
#endif
#define MDL_SUBDIVINFO_HPP

#ifndef ENT_FRAGINFOSINK_HPP
#include "Graphics/ent/entFragInfoSink.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MDL_MATINFO_HPP
#include "Graphics/mdl/mdlMatInfo.hpp"
#endif

#include <map>
#include <vector>


//============================================================================
// Info about which vertex or edge is creased.
// This uses Maya's format for identifying a component
// in the hierarchical subdivision surface format.
//============================================================================
struct mayCreaseInfo
{
	int m_Base;		// Index of face in base mesh
	int m_First;	// for first subdivision level, which child face to use in the path
	int m_Level;	// Subdivision level, 0 is base mesh
	int m_Path;		// path from level 1, 2 bits per level, marking path down to component (4 child faces per face)
	int m_Corner;	// index of the vertex or edge in face
};


//============================================================================
//============================================================================
class  mdlSubdivInfo : public entFragInfo
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	mdlSubdivInfo();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	const std::vector<maPoint3d>& GetVertices();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	int GetNumOrigVertices();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	std::string GetName();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	const std::multimap<int, int>& GetVertexRemap();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	const std::vector< shared_ptr<mdlMatInfo> >& GetMaterials();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	std::vector< envType::UInt32 >* GetIndices();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	std::vector< maPoint2d >* GetUVs();


public:
	std::string m_Name;
	std::vector<maPoint3d> m_Vertices;
	//std::vector<maVector3d> m_Normals;
	std::vector<maPoint2d> m_UVs;
	//std::vector<maFloatRGBA> m_Colors;
	int m_NumFaces;
	std::vector<envType::UInt32> m_Indices;

	// Subdivs switched to multiple materials
	//shared_ptr<mdlMatInfo> m_Material;
	std::vector< shared_ptr<mdlMatInfo> > m_Materials;
	std::vector<int> m_MaterialChanges;

	// Info about reampping of shared vertices
	int m_NumOrigVertices;
	std::multimap<int, int> m_VertexRemap;

	// Sharp areas of the subdivision
	std::vector<mayCreaseInfo> m_VertexCreases;
	int m_MaxVertexCreaseLevel;
	std::vector<mayCreaseInfo> m_EdgeCreases;
	int m_MaxEdgeCreaseLevel;

	// Flags
	struct
	{
		bool m_bDoubleSided : 1;	// render both sides of mesh
		bool m_bTriangleSort : 1;	// triangles needs to be sorted for transparency
		bool m_bCastsShadow : 1;	// casts shadows
		bool m_bReceivesShadow : 1;	// receives shadows
		bool m_bShadowHull : 1;		// fragment is invisible, but casts shadow
		bool m_bAutoGenLowRes : 1;	// importer should generate low-res for this surface
	} m_Flags;

public:
	//----------------------------------------------------------------------------
	// Expand the indices for the faces into triangle indices
	//----------------------------------------------------------------------------
	void GenerateTriangleIndices(std::vector<envType::UInt32> &o_Indices) const;
};

