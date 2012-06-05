/****************************************************************************\
**	g3dMeshUtil.hpp
**
**		g3dMeshUtil contains function for manipulating triangle meshes.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_MESHUTIL_HPP
#error g3dMeshUtil.hpp multiply included
#endif
#define G3D_MESHUTIL_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
namespace g3dMeshUtil
{
	//------------------------------------------------------------------------
	//	FacetAngledFaces will causes adjacent triangle faces which have a
	//	plane angle difference of less than i_Threshold (given as a dot
	//	product) to be unwelded.  This operation creates new vertices and
	//	reassigns triangle vertices, but does not create any new triangles.
	//	io_UVs can be NULL, if you don't want to process any UVs.
	//------------------------------------------------------------------------
	void FacetAngledFaces(	float i_Threshold,
							const maPoint3d* i_Vertices,
							const maPoint3d* i_Normals,
							const maPoint2d* i_UVs,
							int i_NumVertices,
							const envType::UInt32* i_Indices,
							int i_NumIndices,
							std::vector<maPoint3d>& o_Vertices,
							std::vector<maPoint3d>& o_Normals,
							std::vector<maPoint2d>* o_UVs,
							std::vector<envType::UInt32>& o_Indices);

	//------------------------------------------------------------------------
	//	FillNonWeldedEdges adds polygons to a mesh between edges that are not
	//	welded together.  This helps to make the mesh a two-manifold.
	//	The function does not need to add any new vertices; indices may be
	//	added but will not be removed.
	//	Polygons are added between edges and also at areas where more than
	//	two non-welded vertices meet.
	//------------------------------------------------------------------------
	void FillNonWeldedEdges(const maPoint3d* i_Vertices,
							int i_NumVertices,
							std::vector<envType::UInt32>& io_Indices);
}

