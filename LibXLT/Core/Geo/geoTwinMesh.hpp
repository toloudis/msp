/****************************************************************************\
**	geoTwinMesh.hpp
**
**	A geoTwinMesh contains a group of possibly connected geoTwinEdges and
**	geoTwinVertices.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GEO_TWINMESH_HPP
#error geoTwinMesh.hpp multiply included
#endif
#define GEO_TWINMESH_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class geoTwinVertex;
class geoTwinEdge;


//============================================================================
//============================================================================
class geoTwinMesh
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		geoTwinMesh(int i_NumVertices);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~geoTwinMesh();

		//--------------------------------------------------------------------
		//	Adds a single triangle to the mesh.  This function is not totally
		//	robust; some complicated cases with adding triangles to meshes
		//	that have triangles intersecting only at points will cause
		//	problems.  Meshes that have well defined winding orders around
		//	all vertices will work fine.
		//--------------------------------------------------------------------
		void AddTriangle(envType::UInt32 i_V0, envType::UInt32 i_V1, envType::UInt32 i_V2);

		//--------------------------------------------------------------------
		//	ConstructMesh builds a triangle mesh from the given indices.  This
		//	function should be used if a group of indices is to be added, as
		//	it will behave better than adding the triangles one by one with
		//	AddTriangle.
		//--------------------------------------------------------------------
		void ConstructMesh(const envType::UInt32* i_Indices, int i_NumIndices);

		//--------------------------------------------------------------------
		//	UnmarkAll causes all edges to be unmarked.
		//--------------------------------------------------------------------
		void UnmarkAll();

		//--------------------------------------------------------------------
		//	GetEmptyEdges gets only those edges which are not "inside" a
		//	triangle.
		//--------------------------------------------------------------------
		void GetEmptyEdges(std::vector<geoTwinEdge*>& o_Edges);

		//--------------------------------------------------------------------
		//	GetNonEmptyEdges returns those edges which have triangles on
		//	two sides.  Only one edge out of each edge-twin pair will be
		//	returned.
		//--------------------------------------------------------------------
		void GetNonEmptyEdges(std::vector<geoTwinEdge*>& o_Edges);

		//--------------------------------------------------------------------
		//	GetUnmarkedNonEmptyEdges returns edges which have mark = false
		//	and empty = false.
		//--------------------------------------------------------------------
		void GetUnmarkedNonEmptyEdges(std::vector<geoTwinEdge*>& o_Edges);

	private:

		//--------------------------------------------------------------------
		//	test_edge_triangulation is a debugging function which will test
		//	an invariant of the mesh - that each non-empty face must have
		//	three edges.
		//--------------------------------------------------------------------
		void test_edge_triangulation();

		std::vector<geoTwinVertex*>	m_TwinVertices;
		int	m_NumVertices;
		std::vector<geoTwinEdge*> m_TwinEdges;
};
