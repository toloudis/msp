/****************************************************************************\
**  MeshUtil.hpp
**
**      MeshUtil contains function for manipulating triangle meshes.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_MESHUTIL_HPP
#error MeshUtil.hpp multiply included
#endif
#define G3D_MESHUTIL_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif

#include <vector>

class MFnMesh;

//============================================================================
//============================================================================
namespace MeshUtil
{
	typedef int IndexType;

	//========================================================================
	//	FillNonWeldedEdges adds polygons to a mesh between edges that are not
	//	welded together.  This helps to make the mesh a two-manifold.
	//	The function does not need to add any new vertices; 
	//  indices are added for zero-area triangles for welding.
	//	Polygons are added between edges and also at areas where more than
	//	two non-welded vertices meet.
	//========================================================================
	void FillNonWeldedEdges(MFnMesh &mesh, 
							std::vector<IndexType>& o_GeomIndices, 
							std::vector<IndexType>& o_NormIndices, 
							std::vector<IndexType>& o_UVIndices);
}

