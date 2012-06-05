/****************************************************************************\
**	g3dMeshUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dMeshUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoTwinEdge.hpp"
#include "Core/geo/geoTwinMesh.hpp"

#include <algorithm>
#include <vector>


//============================================================================
//============================================================================
namespace
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	inline bool coincident_test(const maPoint3d& i_P0, const maPoint3d& i_P1)
	{
		const float c_CoincidentDist = 0.001f;
		return (i_P0 - i_P1).LengthSqr() < (c_CoincidentDist * c_CoincidentDist);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	struct CoincidentEdge
	{
		geoTwinEdge* m_Edge0;
		geoTwinEdge* m_Edge1;

		bool operator == (const CoincidentEdge& i_Edge) const {	return	((m_Edge0 == i_Edge.m_Edge0) &&
																		(m_Edge1 == i_Edge.m_Edge1)) ||
																		((m_Edge1 == i_Edge.m_Edge0) &&
																		(m_Edge0 == i_Edge.m_Edge1)) ; }
	};

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void get_coincident_edges(	const std::vector<geoTwinEdge*>& i_EmptyEdges,
								const maPoint3d* i_Vertices,
								std::vector<CoincidentEdge>& o_CEdges)
	{
		int inner;
		int outer;
		int num_empty_edges = i_EmptyEdges.size();

		int num_outer = num_empty_edges - 1;
		for ( outer = 0 ; outer < num_outer ; ++outer )
		{
			for ( inner = outer+1 ; inner < num_empty_edges ; ++inner )
			{
				geoTwinEdge* edge0 = i_EmptyEdges[outer];
				geoTwinEdge* edge1 = i_EmptyEdges[inner];

				const int i0 = edge0->GetVertex()->GetIndex();
				const int i1 = edge1->GetTwin()->GetVertex()->GetIndex();

				if ( i0 == i1 ) continue;

				const int i2 = edge1->GetVertex()->GetIndex();
				const int i3 = edge0->GetTwin()->GetVertex()->GetIndex();

				if ( i2 == i3 ) continue;

				const maPoint3d& p0 = i_Vertices[i0];
				const maPoint3d& p1 = i_Vertices[i1];

				if ( coincident_test(p0, p1) )
				{
					const maPoint3d& p2 = i_Vertices[i2];
					const maPoint3d& p3 = i_Vertices[i3];

					if ( coincident_test(p2, p3) )
					{
						if ( coincident_test(p2, p1) )
							return;	//	edge has no length

						if ( coincident_test(p0, p3) )
							return;	//	edge has no length

						CoincidentEdge cedge;
						cedge.m_Edge0 = edge0;
						cedge.m_Edge1 = edge1;
						o_CEdges.push_back(cedge);
					}
				}
			}
		}
	}

	//----------------------------------------------------------------------------
	//	"half coincident edges" have one vertex equal (index) and the other vertex coincident
	//----------------------------------------------------------------------------
	struct half_coincident_edge_detector
	{
		half_coincident_edge_detector(	std::vector<CoincidentEdge>& o_Edges,
									const maPoint3d* i_Vertices) : m_Edges(o_Edges), m_Vertices(i_Vertices) {}

		void operator()(geoTwinEdge* i_Edge0, geoTwinEdge* i_Edge1)
		{
			const int i0 = i_Edge0->GetVertex()->GetIndex();
			const int i1 = i_Edge1->GetTwin()->GetVertex()->GetIndex();
			const int i2 = i_Edge1->GetVertex()->GetIndex();
			const int i3 = i_Edge0->GetTwin()->GetVertex()->GetIndex();

			bool found = false;
			if ( (i0 == i1) ^ (i2 == i3) )
			{
				if ( i0 == i1 )
				{
					const maPoint3d& p2 = m_Vertices[i2];
					const maPoint3d& p3 = m_Vertices[i3];
					if ( coincident_test(p2, p3) )
						found = true;
				}
				else
				{
					const maPoint3d& p0 = m_Vertices[i0];
					const maPoint3d& p1 = m_Vertices[i1];
					if ( coincident_test(p0, p1) )
						found = true;
				}
			}

			if ( found )
			{
				CoincidentEdge cedge;
				cedge.m_Edge0 = i_Edge0;
				cedge.m_Edge1 = i_Edge1;
				m_Edges.push_back(cedge);
			}
		}

		const maPoint3d* m_Vertices;
		std::vector<CoincidentEdge>& m_Edges;
	};

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void fill_edge(const CoincidentEdge& i_Edge, geoTwinMesh& o_Mesh, std::vector<envType::UInt32>& io_Indices)
	{
		geoTwinVertex::IndexType v0, v1, v2, v3;

		v0 = i_Edge.m_Edge0->GetVertex()->GetIndex();
		v1 = i_Edge.m_Edge0->GetTwin()->GetVertex()->GetIndex();

		v2 = i_Edge.m_Edge1->GetTwin()->GetVertex()->GetIndex();
		v3 = i_Edge.m_Edge1->GetVertex()->GetIndex();

		o_Mesh.AddTriangle(v0, v3, v1);
		o_Mesh.AddTriangle(v0, v2, v3);

		io_Indices.push_back(v0);
		io_Indices.push_back(v3);
		io_Indices.push_back(v1);

		io_Indices.push_back(v0);
		io_Indices.push_back(v2);
		io_Indices.push_back(v3);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void maybe_fill_half_edge(	geoTwinEdge* i_Edge,
								const maPoint3d* i_Vertices,
								geoTwinMesh& o_Mesh,
								std::vector<envType::UInt32>& io_Indices)
	{
		if ( !i_Edge->GetFaceEmpty() )
			return;

		if ( !i_Edge->GetPrev()->GetFaceEmpty() )
			return;

		geoTwinVertex::IndexType test1 = i_Edge->GetTwin()->GetVertex()->GetIndex();
		geoTwinVertex::IndexType test2 = i_Edge->GetPrev()->GetVertex()->GetIndex();

		if ( (test1 != test2) && coincident_test(i_Vertices[test1], i_Vertices[test2]) )
		{
			geoTwinVertex::IndexType v0, v1, v2;

			v0 = i_Edge->GetVertex()->GetIndex();
			v1 = i_Edge->GetPrev()->GetVertex()->GetIndex();
			v2 = i_Edge->GetTwin()->GetVertex()->GetIndex();

			o_Mesh.AddTriangle(v0, v1, v2);
			io_Indices.push_back(v0);
			io_Indices.push_back(v1);
			io_Indices.push_back(v2);
		}
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool find_loop(	std::vector<geoTwinEdge*>& o_LoopEdges,
					std::vector<geoTwinEdge*>& io_EmptyEdges,
					const maPoint3d* i_Vertices)
	{
		o_LoopEdges.clear();

		int num_empty_edges = io_EmptyEdges.size();
		int cur_start_edge_num = 0;

		for ( cur_start_edge_num = 0 ; cur_start_edge_num < num_empty_edges ; ++cur_start_edge_num )
		{
			geoTwinEdge* start_edge = io_EmptyEdges[cur_start_edge_num];
			geoTwinEdge* next_edge = start_edge->GetNext();
			geoTwinEdge* cur_edge = start_edge;

			while( true )
			{
				int i0 = cur_edge->GetVertex()->GetIndex();
				int i1 = next_edge->GetVertex()->GetIndex();

				if ( i0 == i1 )
					break;

				const maPoint3d& p0 = i_Vertices[i0];
				const maPoint3d& p1 = i_Vertices[i1];

				if ( !coincident_test(p0, p1) )
				{
					//	points not coincident, quit iteration
					break;
				}

				if ( next_edge == start_edge )
				{
					//	finished loop; copy it over and return
					cur_edge = start_edge;
					o_LoopEdges.push_back(cur_edge);
					cur_edge = cur_edge->GetNext();

					while( cur_edge != start_edge )
					{
						o_LoopEdges.push_back(cur_edge);
						cur_edge = cur_edge->GetNext();
					}

					//	remove the loop edges from our input list, so we don't find them again
					int i;
					int num_output_edges = o_LoopEdges.size();
					for ( i = 0 ; i < num_output_edges ; ++i )
						io_EmptyEdges.erase(std::find(io_EmptyEdges.begin(), io_EmptyEdges.end(), o_LoopEdges[i]));

					return true;
				}

				cur_edge = next_edge;
				next_edge = next_edge->GetNext();
			}
		}

		return false;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void fill_loop(const std::vector<geoTwinEdge*>& i_LoopEdges, std::vector<envType::UInt32>& io_Indices)
	{
		int num_tris = i_LoopEdges.size() - 2;
		int cur_tri_num;

		geoTwinVertex::IndexType v0 = i_LoopEdges[0]->GetVertex()->GetIndex();

		for ( cur_tri_num = 0 ; cur_tri_num < num_tris ; ++cur_tri_num )
		{
			geoTwinVertex::IndexType v1 = i_LoopEdges[cur_tri_num + 1]->GetVertex()->GetIndex();
			geoTwinVertex::IndexType v2 = i_LoopEdges[cur_tri_num + 2]->GetVertex()->GetIndex();
			io_Indices.push_back(v2);
			io_Indices.push_back(v1);
			io_Indices.push_back(v0);
		}
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	inline maPoint3d make_normal(const maPoint3d& i_V0, const maPoint3d& i_V1, const maPoint3d& i_V2)
	{
		maVector3d cross = (i_V1 - i_V0).Cross(i_V2 - i_V0);
		cross.Normalize();
		return cross;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	int find_tri(	const envType::UInt32* i_Indices,
					int i_NumIndices,
					envType::UInt32 i_V0,
					envType::UInt32 i_V1,
					envType::UInt32 i_V2)
	{
		int cur_index;
		for ( cur_index = 0 ; cur_index < i_NumIndices ; cur_index += 3 )
		{
			if ( i_Indices[cur_index] == i_V0 )
			{
				if ( (i_Indices[cur_index+1] == i_V1) && (i_Indices[cur_index+2] == i_V2) )
					return cur_index;
			}

			if ( i_Indices[cur_index] == i_V1 )
			{
				if ( (i_Indices[cur_index+1] == i_V2) && (i_Indices[cur_index+2] == i_V0) )
					return cur_index;
			}

			if ( i_Indices[cur_index] == i_V2 )
			{
				if ( (i_Indices[cur_index+1] == i_V0) && (i_Indices[cur_index+2] == i_V1) )
					return cur_index;
			}
		}

		return -1;
	}
}

//============================================================================
//============================================================================
namespace g3dMeshUtil
{
	//------------------------------------------------------------------------
	//	FacetAngledFaces will causes adjacent triangle faces which have a
	//	plane angle difference of less than i_Threshold (given as a dot
	//	product) to be unwelded.  This operation creates new vertices and
	//	reassigns triangle vertices, but does not create any new triangles.
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
							std::vector<envType::UInt32>& o_Indices)
	{
		o_Vertices.resize(i_NumVertices);
		o_Normals.resize(i_NumVertices);
		o_Indices.resize(i_NumIndices);
		if ( i_UVs )
		{
			o_UVs->resize(i_NumVertices);
			memcpy(&((*o_UVs)[0]), i_UVs, i_NumVertices * sizeof(maPoint2d));
		}

		memcpy(&o_Vertices[0], i_Vertices, i_NumVertices * sizeof(maPoint3d));
		memcpy(&o_Normals[0], i_Normals, i_NumVertices * sizeof(maPoint3d));
		memcpy(&o_Indices[0], i_Indices, i_NumIndices * sizeof(envType::UInt32));

		{
			geoTwinMesh mesh(i_NumVertices);

			mesh.ConstructMesh(i_Indices, i_NumIndices);

			std::vector<geoTwinEdge*> edges;
			mesh.GetNonEmptyEdges(edges);

			//	loop thru each edge and decide if the angle between the two
			//	adjacent tris is too large
			int num_edges = edges.size();
			int i;
			for ( i = 0 ; i < num_edges ; ++i )
			{
				geoTwinEdge* e0 = edges[i];
				geoTwinEdge* e1 = e0->GetTwin();

				if ( e0->GetNext()->GetNext()->GetNext() != e0 )
					continue;

				if ( e1->GetNext()->GetNext()->GetNext() != e1 )
					continue;

				int t0i0 = e0->GetVertex()->GetIndex();
				int t0i1 = e0->GetPrev()->GetVertex()->GetIndex();
				int t0i2 = e0->GetNext()->GetVertex()->GetIndex();

				const maPoint3d& t0v0 = i_Vertices[t0i0];
				const maPoint3d& t0v1 = i_Vertices[t0i1];
				const maPoint3d& t0v2 = i_Vertices[t0i2];

				const maPoint3d& t1v0 = i_Vertices[e1->GetVertex()->GetIndex()];
				const maPoint3d& t1v1 = i_Vertices[e1->GetPrev()->GetVertex()->GetIndex()];
				const maPoint3d& t1v2 = i_Vertices[e1->GetNext()->GetVertex()->GetIndex()];

				maVector3d normal0 = make_normal(t0v0, t0v1, t0v2);
				maVector3d normal1 = make_normal(t1v0, t1v1, t1v2);

				if ( normal0 * normal1 < i_Threshold )
				{
					//	sharp angle; unweld this triangle
					//	make 2 new vertices
					int new_index_base = o_Vertices.size();
					o_Vertices.push_back(i_Vertices[t0i0]);
					o_Vertices.push_back(i_Vertices[t0i2]);
					o_Normals.push_back(i_Normals[t0i0]);
					o_Normals.push_back(i_Normals[t0i2]);
					//o_Normals.push_back(normal0);
					//o_Normals.push_back(normal0);
					if ( i_UVs )
					{
						o_UVs->push_back(i_UVs[t0i0]);
						o_UVs->push_back(i_UVs[t0i2]);
					}

					//	reassign triangle
					//	this could cause a rotation but that shouldn't be a problem
					int index = find_tri(&o_Indices[0], o_Indices.size(), t0i0, t0i1, t0i2);
					if ( index >= 0 )
					{
						o_Indices[index+0] = new_index_base;
						o_Indices[index+1] = t0i1;
						o_Indices[index+2] = new_index_base + 1;
					}
				}
			}
		}
	}

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
							std::vector<envType::UInt32>& io_Indices)
	{
		geoTwinMesh mesh(i_NumVertices);
		mesh.ConstructMesh(&io_Indices[0], io_Indices.size());

		std::vector<geoTwinEdge*> empty_edges;
		mesh.GetEmptyEdges(empty_edges);

		int num_empty_edges = empty_edges.size();
		if ( num_empty_edges == 0 )
			return;

		//	fill "half" edges first
		int i;
		for ( i = 0 ; i < num_empty_edges ; ++i )
			maybe_fill_half_edge(empty_edges[i], i_Vertices, mesh, io_Indices);

		//	get new list of empty edges, after edge fill
		empty_edges.clear();
		mesh.GetEmptyEdges(empty_edges);
		num_empty_edges = empty_edges.size();
		if ( num_empty_edges == 0 )
			return;

		std::vector<CoincidentEdge> cedges;
		get_coincident_edges(empty_edges, i_Vertices, cedges);

		//	fill each edge
		int num_coincident_edges = cedges.size();
		if ( num_coincident_edges )
		{
			for ( i = 0 ; i < num_coincident_edges ; ++i )
				fill_edge(cedges[i], mesh, io_Indices);

			//	get new list of empty edges, after edge fill
			empty_edges.clear();
			mesh.GetEmptyEdges(empty_edges);
		}

		//	fill each hole with coincident vertices
		std::vector<geoTwinEdge*> loop_edges;
		while( find_loop(loop_edges, empty_edges, i_Vertices) )
			fill_loop(loop_edges, io_Indices);
	}
}

