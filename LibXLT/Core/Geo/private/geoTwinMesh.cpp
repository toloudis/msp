/****************************************************************************\
**	geoTwinMesh.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/geo/geoTwinMesh.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoTwinEdge.hpp"
#include "Core/geo/geoTwinVertexEdgeIterator.hpp"
#include "Core/ma/maFunctions.hpp"

#include <algorithm>
#include <functional>
#include <set>


//============================================================================
//============================================================================
namespace
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void dump_edges(geoTwinVertex* i_Vertex)
	{
		geoTwinVertexEdgeIterator it(i_Vertex);

		while( it.GetEdge() )
		{
			DBG_LOG("Edge of " << i_Vertex->GetIndex() << ": " << it.GetEdge()->GetTwin()->GetVertex()->GetIndex());
			++it;
		}		
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void dump_edges_edges(geoTwinVertex* i_Vertex)
	{
		geoTwinVertexEdgeIterator it(i_Vertex);

		while( it.GetEdge() )
		{
			DBG_LOG("");
			dump_edges(it.GetEdge()->GetTwin()->GetVertex());
			++it;
		}		
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	geoTwinEdge* get_shared_edge(	geoTwinVertex* i_V0,
									geoTwinVertex* i_V1)
	{
		geoTwinVertexEdgeIterator it(i_V0);

		while( it.GetEdge() )
		{

			if( it.GetEdge()->GetTwin()->GetVertex() == i_V1 )
				return it.GetEdge();

			++it;
		}		

		return NULL;
	}


	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void make_twin_edge(geoTwinVertex* i_V0, 
						geoTwinVertex* i_V1,
						geoTwinEdge*& io_Edge0,
						geoTwinEdge*& io_Edge1,
						std::vector<geoTwinEdge*>& o_Edges)
	{
		io_Edge0 = new geoTwinEdge(i_V0);
		o_Edges.push_back(io_Edge0);
		io_Edge1 = new geoTwinEdge(i_V1);
		o_Edges.push_back(io_Edge1);
		io_Edge0->SetTwin(io_Edge1);
		io_Edge1->SetTwin(io_Edge0);	
		io_Edge1->SetFaceEmpty(false);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool get_insert_edges(	geoTwinEdge*& io_TargetPrev,
							geoTwinEdge*& io_TargetNext,
							geoTwinVertex* i_Vertex )
	{
		geoTwinVertexEdgeIterator it(i_Vertex);

		while( it.GetEdge() )
		{
			if( it.GetEdge()->GetFaceEmpty() && it.GetEdge()->GetPrev()->GetFaceEmpty() )
			{
				io_TargetNext = it.GetEdge();
				io_TargetPrev = it.GetEdge()->GetPrev();
				return true;
			}

			++it;
		}
		
		return false;
	}
	
/*	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool get_next_insert_edge(	geoTwinEdge*& io_TargetNext,
								geoTwinVertex* i_Vertex )
	{
		geoTwinVertexEdgeIterator it(i_Vertex);

		while( it.GetEdge() )
		{
			if( it.GetEdge()->GetFaceEmpty() )
			{
				io_TargetNext = it.GetEdge();
				return true;
			}

			++it;
		}
		
		return false;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool get_prev_insert_edge(	geoTwinEdge*& io_TargetPrev,
								geoTwinVertex* i_Vertex )
	{
		geoTwinVertexEdgeIterator it(i_Vertex);

		while( it.GetEdge() )
		{
			if( it.GetEdge()->GetTwin()->GetFaceEmpty() )
			{
				io_TargetPrev = it.GetEdge()->GetTwin();
				return true;
			}

			++it;
		}
		
		return false;
	}
*/

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void make_free_triangle(geoTwinVertex*const* io_Vertices, std::vector<geoTwinEdge*>& o_Edges)
	{
		geoTwinEdge* e[6];
		int i;
		for( i = 0 ; i < 3 ; ++i )
			make_twin_edge(io_Vertices[i], io_Vertices[(i+1)%3], e[i*2], e[i*2 + 1], o_Edges);

		e[0]->SetNext(e[2]);
		e[2]->SetPrev(e[0]);

		e[2]->SetNext(e[4]);
		e[4]->SetPrev(e[2]);

		e[4]->SetNext(e[0]);	
		e[0]->SetPrev(e[4]);	

		e[1]->SetNext(e[5]);
		e[5]->SetPrev(e[1]);	

		e[3]->SetNext(e[1]);	
		e[1]->SetPrev(e[3]);

		e[5]->SetNext(e[3]);
		e[3]->SetPrev(e[5]);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void make_triangle_splice_vertex(geoTwinVertex*const* io_Vertices, geoTwinVertex* io_SpliceVertex, std::vector<geoTwinEdge*>& o_Edges)
	{
		//	find empty edges on the splice vertex to add the triangle between
		//	the edges between which to add will be the two adjacent edges with empty face
		//	we must find the edges first because making the twin edges will change the
		//	vertex edge pointers
		geoTwinEdge* target_prev;
		geoTwinEdge* target_next;
		
		if( !get_insert_edges(target_prev, target_next, io_SpliceVertex) )
		{
			DBG_WARNING("Two-manifold failure");
			get_insert_edges(target_prev, target_next, io_SpliceVertex);
			return;
		}

		geoTwinEdge* e[6];

		geoTwinVertex* local_vertices[3];

		//	for convenience, we'll make the zeroth vertex the splice vertex
		std::rotate_copy(io_Vertices, std::find(io_Vertices, io_Vertices + 3, io_SpliceVertex), io_Vertices + 3, local_vertices);

		make_twin_edge(local_vertices[0], local_vertices[1], e[0], e[1], o_Edges);
		make_twin_edge(local_vertices[1], local_vertices[2], e[2], e[3], o_Edges);
		make_twin_edge(local_vertices[2], local_vertices[0], e[4], e[5], o_Edges);

		e[0]->SetNext(e[2]);
		e[2]->SetPrev(e[0]);

		e[2]->SetNext(e[4]);
		e[4]->SetPrev(e[2]);

		target_next->SetPrev(e[4]);
		e[4]->SetNext(target_next);	

		e[1]->SetNext(e[5]);
		e[5]->SetPrev(e[1]);	

		e[3]->SetNext(e[1]);	
		e[1]->SetPrev(e[3]);

		e[0]->SetPrev(target_prev);	
		target_prev->SetNext(e[0]);

		e[3]->SetPrev(e[5]);
		e[5]->SetNext(e[3]);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void make_triangle_splice_edge(	geoTwinVertex*const* io_Vertices, 
									geoTwinEdge* i_SpliceEdge, 
									std::vector<geoTwinEdge*>& o_Edges)
	{
		//	find the non-edge vertex
		int i;
		geoTwinVertex* free_vertex;
		for( i = 0 ; i < 3 ; ++i )
		{
			if( (io_Vertices[i] != i_SpliceEdge->GetVertex()) &&
				(io_Vertices[i] != i_SpliceEdge->GetTwin()->GetVertex()) )
			{	
				free_vertex = io_Vertices[i];
				break;
			}
		}

		geoTwinEdge* splice_edge;
		if( i_SpliceEdge->GetFaceEmpty() )
			splice_edge = i_SpliceEdge;
		else
		{
			splice_edge = i_SpliceEdge->GetTwin();
			if( !splice_edge->GetFaceEmpty() )
			{
				DBG_WARNING("Two-manifold failure");
				return;
			}
		}

		geoTwinEdge* front_next;
		geoTwinEdge* back_prev;

		back_prev = splice_edge->GetPrev();
		front_next = splice_edge->GetNext();

//		if( !get_prev_insert_edge(back_prev, splice_edge->GetVertex()) )
//		{
//			DBG_WARNING("Two-manifold failure");
//			return;
//		}

//		if( !get_next_insert_edge(front_next, splice_edge->GetTwin()->GetVertex()) )
//		{
//			DBG_WARNING("Two-manifold failure");
//			return;
//		}

		//	only need to make two edges
		geoTwinEdge* e[4];
		make_twin_edge(splice_edge->GetVertex(), free_vertex, e[0], e[1], o_Edges);
		make_twin_edge(free_vertex, splice_edge->GetTwin()->GetVertex(), e[2], e[3], o_Edges);
	
		back_prev->SetNext(e[0]);
		e[0]->SetPrev(back_prev);

		e[0]->SetNext(e[2]);
		e[2]->SetPrev(e[0]);

		e[2]->SetNext(front_next);
		front_next->SetPrev(e[2]);

		splice_edge->SetNext(e[3]);
		e[3]->SetPrev(splice_edge);

		e[3]->SetNext(e[1]);
		e[1]->SetPrev(e[3]);

		e[1]->SetNext(splice_edge);
		splice_edge->SetPrev(e[1]);

		splice_edge->SetFaceEmpty(false);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void make_triangle_splice_two_vertex(	geoTwinVertex*const* io_Vertices, 
											geoTwinVertex* io_SpliceVertex0, 
											geoTwinVertex* io_SpliceVertex1, 
											std::vector<geoTwinEdge*>& o_Edges)
	{
		geoTwinVertex* local_vertices[3];

		//	for convenience, we'll make the zeroth vertex the non-splice vertex
		int non_splice_index;
		for( non_splice_index = 0 ; non_splice_index < 3 ; ++non_splice_index )
			if( (io_Vertices[non_splice_index] != io_SpliceVertex0) &&
				(io_Vertices[non_splice_index] != io_SpliceVertex1) )
					break;

		std::rotate_copy(	io_Vertices, 
							io_Vertices + non_splice_index,
							io_Vertices + 3,
							local_vertices);

		geoTwinEdge* target_prev0;
		geoTwinEdge* target_next0;
		if( !get_insert_edges(target_prev0, target_next0, local_vertices[1]) )
		{
			DBG_WARNING("Two-manifold failure");
			return;
		}

		geoTwinEdge* target_prev1;
		geoTwinEdge* target_next1;
		if( !get_insert_edges(target_prev1, target_next1, local_vertices[2]) )
		{
			DBG_WARNING("Two-manifold failure");
			return;
		}

		geoTwinEdge* e[6];

		int i;
		for( i = 0 ; i < 3 ; ++i )
			make_twin_edge(local_vertices[i], local_vertices[(i+1)%3], e[i*2], e[i*2 + 1], o_Edges);

		e[0]->SetNext(target_next0);
		target_next0->SetPrev(e[0]);

		target_prev0->SetNext(e[2]);
		e[2]->SetPrev(target_prev0);

		e[2]->SetNext(target_next1);
		target_next1->SetPrev(e[2]);

		target_prev1->SetNext(e[4]);
		e[4]->SetPrev(target_prev1);

		e[4]->SetNext(e[0]);	
		e[0]->SetPrev(e[4]);	

		e[1]->SetNext(e[5]);
		e[5]->SetPrev(e[1]);	

		e[3]->SetNext(e[1]);	
		e[1]->SetPrev(e[3]);

		e[5]->SetNext(e[3]);
		e[3]->SetPrev(e[5]);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void make_triangle_splice_three_vertex(	geoTwinVertex*const* io_Vertices, 
											std::vector<geoTwinEdge*>& o_Edges)
	{
		geoTwinEdge* target_prev[3];
		geoTwinEdge* target_next[3];
		geoTwinEdge* e[6];

		int i;
		for( i = 0 ; i < 3 ; ++i )
		{
			if( !get_insert_edges(target_prev[i], target_next[i], io_Vertices[i]) )
			{
				DBG_WARNING("Two-manifold failure");
				return;
			}
		}

		//	this has to be done in a separate loop because making the edges
		//	modifies the vertex edge pointers
		for( i = 0 ; i < 3 ; ++i )
			make_twin_edge(io_Vertices[i], io_Vertices[(i+1)%3], e[i*2], e[i*2 + 1], o_Edges);

		for( i = 0 ; i < 3 ; ++i )
		{
			target_prev[i]->SetNext(e[i*2]);
			e[i*2]->SetNext(target_next[(i+1)%3]);

			e[i*2]->SetPrev(target_prev[i]);
			target_next[(i+1)%3]->SetPrev(e[i*2]);
		}

		e[1]->SetNext(e[5]);
		e[5]->SetPrev(e[1]);	

		e[3]->SetNext(e[1]);	
		e[1]->SetPrev(e[3]);

		e[5]->SetNext(e[3]);
		e[3]->SetPrev(e[5]);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void make_triangle_splice_edge_and_vertex(	geoTwinVertex*const* io_Vertices, 
												geoTwinEdge* i_SpliceEdge, 
												std::vector<geoTwinEdge*>& o_Edges)
	{
		//	this case is an adjacent edge and opposite vertex also used
		//	find the non-edge vertex
		int i;
		geoTwinVertex* splice_vertex;
		for( i = 0 ; i < 3 ; ++i )
		{
			if( (io_Vertices[i] != i_SpliceEdge->GetVertex()) &&
				(io_Vertices[i] != i_SpliceEdge->GetTwin()->GetVertex()) )
			{	
				splice_vertex = io_Vertices[i];
				break;
			}
		}

		geoTwinEdge* splice_edge;
		if( i_SpliceEdge->GetFaceEmpty() )
			splice_edge = i_SpliceEdge;
		else
		{
			splice_edge = i_SpliceEdge->GetTwin();
			if( !splice_edge->GetFaceEmpty() )
			{
				DBG_WARNING("Two-manifold failure");
				return;
			}
		}

		geoTwinEdge* target_prev;
		geoTwinEdge* target_next;
		
		if( !get_insert_edges(target_prev, target_next, splice_vertex) )
		{
			DBG_WARNING("Two-manifold failure");
			return;
		}

		geoTwinEdge* front_next;
		geoTwinEdge* back_prev;

		front_next = splice_edge->GetNext();
		back_prev = splice_edge->GetPrev();

//		if( !get_prev_insert_edge(back_prev, splice_edge->GetVertex()) )
//		{
//			DBG_WARNING("Two-manifold failure");
//			return;
//		}
//
//		if( !get_next_insert_edge(front_next, splice_edge->GetTwin()->GetVertex()) )
//		{
//			DBG_WARNING("Two-manifold failure");
//			return;
//		}

		//	only need to make two edges
		geoTwinEdge* e[4];
		make_twin_edge(splice_edge->GetVertex(), splice_vertex, e[0], e[1], o_Edges);
		make_twin_edge(splice_vertex, splice_edge->GetTwin()->GetVertex(), e[2], e[3], o_Edges);
	
		back_prev->SetNext(e[0]);
		e[0]->SetPrev(back_prev);

		e[0]->SetNext(target_next);
		target_next->SetPrev(e[0]);

		target_prev->SetNext(e[2]);
		e[2]->SetPrev(target_prev);

		e[2]->SetNext(front_next);
		front_next->SetPrev(e[2]);

		splice_edge->SetNext(e[3]);
		e[3]->SetPrev(splice_edge);

		e[3]->SetNext(e[1]);
		e[1]->SetPrev(e[3]);

		e[1]->SetNext(splice_edge);
		splice_edge->SetPrev(e[1]);

		splice_edge->SetFaceEmpty(false);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void make_triangle_splice_two_edges(geoTwinVertex*const* io_Vertices, 
										geoTwinEdge* i_SpliceEdge0, 
										geoTwinEdge* i_SpliceEdge1, 
										std::vector<geoTwinEdge*>& o_Edges)
	{
		//	only need to make one edge
		//	figure out which edge is previous
		geoTwinEdge* target_prev = i_SpliceEdge0;
		geoTwinEdge* target_next = i_SpliceEdge1;

		if( !target_prev->GetFaceEmpty() )
			target_prev = target_prev->GetTwin();

		if( !target_prev->GetFaceEmpty() )
		{
			DBG_WARNING("Two-manifold failure");
			return;
		}
		
		if( !target_next->GetFaceEmpty() )
			target_next = target_next->GetTwin();

		if( !target_next->GetFaceEmpty() )
		{
			DBG_WARNING("Two-manifold failure");
			return;
		}

		int base_index = std::find(io_Vertices, io_Vertices + 3, target_prev->GetVertex()) - io_Vertices;
		int next_index = (base_index + 2) % 3;
		if( target_next->GetVertex() != io_Vertices[next_index] )
			maFunctions::Swap(target_prev, target_next);

		geoTwinVertex* org = target_prev->GetVertex();
		geoTwinVertex* dest = target_next->GetTwin()->GetVertex();

		geoTwinEdge* front_next;
		geoTwinEdge* back_prev;

		front_next = target_next->GetNext();
		back_prev = target_prev->GetPrev();

//		if( !get_prev_insert_edge(back_prev, org) )
//		{
//			DBG_WARNING("Two-manifold failure");
//			return;
//		}
//
//		if( !get_next_insert_edge(front_next, dest) )
//		{
//			DBG_WARNING("Two-manifold failure");
//			return;
//		}

		//	If the next and previous aren't already in the right order, we have to shift
		//	stuff around so that everything is wired properly in the surrounding tris

//	don't handlet this case
/*		if( target_prev->GetNext() != target_next )
		{
			geoTwinVertex* center_vertex = target_next->GetVertex();

			geoTwinEdge* loop_prev = target_prev;
			while( loop_prev->GetVertex() != center_vertex )
			{
				loop_prev = loop_prev->GetPrev();
			}

			geoTwinEdge* loop_prev_prev = loop_prev->GetPrev();

			target_next->GetPrev()->SetNext(loop_prev);
			loop_prev->SetPrev(target_next->GetPrev());

			target_prev->GetNext()->SetPrev(loop_prev_prev);
			loop_prev_prev->SetNext(target_prev->GetNext());

			//	
			target_prev->SetNext(target_next);
			target_next->SetPrev(target_prev);			
		}*/

		geoTwinEdge* inner;
		geoTwinEdge* outer;
		make_twin_edge(org, dest, outer, inner, o_Edges);
		
		back_prev->SetNext(outer);
		outer->SetPrev(back_prev);

		front_next->SetPrev(outer);
		outer->SetNext(front_next);

		target_next->SetNext(inner);
		inner->SetPrev(target_next);

		inner->SetNext(target_prev);
		target_prev->SetPrev(inner);

		target_prev->SetFaceEmpty(false);
		target_next->SetFaceEmpty(false);
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void make_triangle_splice_three_edges(geoTwinEdge*const* i_Edges)
	{
		geoTwinEdge* edges[3];

		int i;
		for( i = 0 ; i < 3; ++i )
		{
			if( i_Edges[i]->GetFaceEmpty() )
				edges[i] = i_Edges[i];
			else
				edges[i] = i_Edges[i]->GetTwin();
		}

		for( i = 0 ; i < 3 ; ++i )
		{
			if( !edges[i]->GetFaceEmpty() )
			{
				DBG_WARNING("Two-manifold failure");
				return;
			}

			edges[i]->SetFaceEmpty(false);

			geoTwinEdge* other1 = edges[ (i+1) % 3 ];
			geoTwinEdge* other2 = edges[ (i+2) % 3 ];
			
			if( other1->GetVertex() == edges[i]->GetTwin()->GetVertex() )
			{
				edges[i]->SetNext(other1);
				other1->SetPrev(edges[i]);
			}
			else
			{
				edges[i]->SetNext(other2);
				other2->SetPrev(edges[i]);
			}
		}
	}

	//====================================================================
	//	stuff used by ConstructMesh
	//====================================================================
	struct edge_rec
	{
		edge_rec() : m_I0(0), m_I1(1), m_Edge0(NULL), m_Edge1(NULL) {}
		edge_rec(envType::UInt32 i_I0, envType::UInt32 i_I1) : m_I0(i_I0), m_I1(i_I1), m_Edge0(NULL), m_Edge1(NULL) {}

		envType::UInt32 m_I0;	//	m_I0 < m_I1
		envType::UInt32 m_I1;

		//	these members are mutable so that they can be modified in place in a set
		//	The < operator does not depend on them
		mutable geoTwinEdge* m_Edge0;	//	this is the edge that has base m_I0
		mutable geoTwinEdge* m_Edge1;	//	this is the edge that has base m_I1

		inline bool operator < (const edge_rec& i_CompareTo) const;
	};

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	inline bool edge_rec::operator < (const edge_rec& i_CompareTo) const
	{
		if( m_I0 != i_CompareTo.m_I0 )
			return m_I0 < i_CompareTo.m_I0;
		else
			return m_I1 < i_CompareTo.m_I1;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	struct make_boundary_edge
	{
		make_boundary_edge(std::vector<geoTwinEdge*>& o_Edges) : m_Edges(o_Edges) {}

		void operator()(geoTwinEdge* i_Edge)
		{
			//	the new edge will have the org and dest of i_Edge reversed
			geoTwinVertex* new_edge_org = i_Edge->GetNext()->GetVertex();
			
			geoTwinEdge* new_edge = new geoTwinEdge(new_edge_org);
			
			new_edge->SetTwin(i_Edge);
			i_Edge->SetTwin(new_edge);

			new_edge->SetFaceEmpty(true);
			m_Edges.push_back(new_edge);
		}

		std::vector<geoTwinEdge*>& m_Edges;
	};

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void link_boundary_edge(geoTwinEdge* i_Edge)
	{
		//	the actual boundary edge is the twin of i_Edge
		geoTwinEdge* boundary_edge = i_Edge->GetTwin();

		//	if we rotate around the org vertex of boundary_edge, we should eventually
		//	find an edge whose prev is not set.  This will be the next edge along
		//	the boundary
		geoTwinEdge* next_edge = i_Edge->GetPrev()->GetTwin();
		while( next_edge->GetPrev() )
			next_edge = next_edge->GetPrev()->GetTwin();

		boundary_edge->SetNext(next_edge);
		next_edge->SetPrev(boundary_edge);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
geoTwinMesh::geoTwinMesh(int i_NumVertices)
:	m_NumVertices(i_NumVertices)
{
	int i;
	for( i = 0 ; i < m_NumVertices ; ++i )
		m_TwinVertices.push_back(new geoTwinVertex(i));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
geoTwinMesh::~geoTwinMesh()
{
	envSTLHelpers::DeleteContainer(m_TwinEdges);
	envSTLHelpers::DeleteContainer(m_TwinVertices);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void geoTwinMesh::AddTriangle(envType::UInt32 i_V0, 
							  envType::UInt32 i_V1, 
							  envType::UInt32 i_V2)
{
	DBG_ASSERT( (i_V0 != i_V1) && (i_V1 != i_V2) && (i_V0 != i_V2), "expected non-identical vertices");

	int num_splice_vertices = 0;
	int num_free_vertices = 0;
	geoTwinVertex* splice_vertices[3];
	geoTwinVertex* free_vertices[3];
	geoTwinVertex* twin_vertices[3];
	twin_vertices[0] = m_TwinVertices[i_V0];
	twin_vertices[1] = m_TwinVertices[i_V1];
	twin_vertices[2] = m_TwinVertices[i_V2];

	int i;
	for( i = 0 ; i < 3 ; ++i )
	{
		if( twin_vertices[i]->GetEdge() == NULL )
			free_vertices[num_free_vertices++] = twin_vertices[i];
		else
			splice_vertices[num_splice_vertices++] = twin_vertices[i];
	}

	if( num_splice_vertices == 0 )
	{
		//	easy case
		make_free_triangle(twin_vertices, m_TwinEdges);	
//		this->test_edge_triangulation();
		return;
	}

	if( num_splice_vertices == 1 )
	{
		//	pretty easy case
		make_triangle_splice_vertex(twin_vertices, splice_vertices[0], m_TwinEdges);
//		this->test_edge_triangulation();
		return;
	}

	if( num_splice_vertices == 2 )
	{
		//	we could have either a whole edge adjacent, or two separate unconnected points
		//	test to see if the vertices share an edge
		geoTwinEdge* shared_edge = get_shared_edge(splice_vertices[0], splice_vertices[1]);
		if( shared_edge )
			make_triangle_splice_edge(twin_vertices, shared_edge, m_TwinEdges);
		else
			make_triangle_splice_two_vertex(twin_vertices, splice_vertices[0], splice_vertices[1], m_TwinEdges);
		
//		this->test_edge_triangulation();
		return;
	}

	if( num_splice_vertices == 3 )
	{
		//	every vertex already has a triangle connected
		//	we could have 0, 1, 2, or 3 edges adjacent
		geoTwinEdge* shared_edges[3];
		
		int num_splice_edges = 0;
		for( i = 0 ; i < 3 ; ++i )
		{
			geoTwinEdge* shared_edge = get_shared_edge(splice_vertices[i], splice_vertices[(i+1) % 3]);
			if( shared_edge )
			{
				shared_edges[num_splice_edges++] = shared_edge;								
				if( !(shared_edge->GetFaceEmpty() ^ shared_edge->GetTwin()->GetFaceEmpty()) )
				{
					DBG_WARNING("Two-manifold failure");
					return;
				}
			}
		}

		switch( num_splice_edges )
		{
			case 0:
				make_triangle_splice_three_vertex(twin_vertices, m_TwinEdges);
//				this->test_edge_triangulation();
			break;

			case 1:
				make_triangle_splice_edge_and_vertex(twin_vertices, shared_edges[0], m_TwinEdges);
//				this->test_edge_triangulation();
			break;
		
			case 2:
				make_triangle_splice_two_edges(twin_vertices, shared_edges[0], shared_edges[1], m_TwinEdges);
//				this->test_edge_triangulation();
			break;

			case 3:
				make_triangle_splice_three_edges(shared_edges);
//				this->test_edge_triangulation();
			break;
		}
	}
}

//--------------------------------------------------------------------
//	ConstructMesh builds a triangle mesh from the given indices.  This
//	function should be used if a group of indices is to be added, as
//	it will behave better than adding the triangles one by one with
//	AddTriangle.
//--------------------------------------------------------------------
void geoTwinMesh::ConstructMesh(const envType::UInt32* i_Indices, int i_NumIndices)
{
	std::set<edge_rec> edge_set;
	typedef std::set<edge_rec>::iterator set_it;

	int cur_tri;
	int num_tris = i_NumIndices / 3;
	for( cur_tri = 0 ; cur_tri < num_tris ; ++cur_tri )
	{
		const envType::UInt32* cur_indices = i_Indices + (cur_tri * 3);
		geoTwinEdge* new_edges[3];

		int i;
		for( i = 0 ; i < 3 ; ++i )
		{
			envType::UInt32 i1 = cur_indices[i];
			envType::UInt32 i0 = cur_indices[(i+1) % 3];

			geoTwinEdge* new_edge = new geoTwinEdge(m_TwinVertices[i0]);
			m_TwinEdges.push_back(new_edge);
			new_edge->SetFaceEmpty(false);

			bool reverse;
			if( i0 < i1 )
			{
				reverse = false;
			}
			else
			{
				reverse = true;
				maFunctions::Swap(i0, i1);
			}

			edge_rec new_edge_rec(i0, i1);
			std::pair<set_it, bool> pairib = edge_set.insert(new_edge_rec);

			if( pairib.second )	//	new insertion
			{
				if( reverse )
					pairib.first->m_Edge1 = new_edge;
				else
					pairib.first->m_Edge0 = new_edge;
			}
			else
			{
				//	this should be the twin of an edge already in the mesh
				geoTwinEdge* existing_edge;
				const edge_rec& existing_edge_rec = *pairib.first;
				if( reverse )
				{
					existing_edge = pairib.first->m_Edge0;
					pairib.first->m_Edge1 = new_edge;
				}
				else
				{
					existing_edge = pairib.first->m_Edge1;
					pairib.first->m_Edge0 = new_edge;
				}

				DBG_ASSERT(existing_edge, "2-manifold violation");
				existing_edge->SetTwin(new_edge);
				new_edge->SetTwin(existing_edge);
			}

			new_edges[i] = new_edge;
		}

		//	link new edges
		for( i = 0 ; i < 3 ; ++i )
		{
			geoTwinEdge* prev = new_edges[(i+1) % 3];
			geoTwinEdge* next = new_edges[i];
			prev->SetNext(next);
			next->SetPrev(prev);
		}
	}

	//	at this point we have a bunch of triangles linked internally
	//	we could still have outer edges which are "NULL" and must
	//	be created and linked.
	set_it cur_it = edge_set.begin();
	set_it end_it = edge_set.end();
	std::vector<geoTwinEdge*> boundary_edges;

	for( ; cur_it != end_it ; ++cur_it )
	{
		const edge_rec& edge_data = *cur_it;

		if( (edge_data.m_Edge0 == NULL) || (edge_data.m_Edge1 == NULL) )
		{
			//	this is a boundary edge
			if( edge_data.m_Edge0 )
				boundary_edges.push_back(edge_data.m_Edge0);
			else
				boundary_edges.push_back(edge_data.m_Edge1);
		}
	}

	//	for each boundary edge, create the twin edge and link it properly to the next edge
	envSTLHelpers::ForAll(boundary_edges, make_boundary_edge(m_TwinEdges));
	envSTLHelpers::ForAll(boundary_edges, link_boundary_edge);
}


//--------------------------------------------------------------------
//	UnmarkAll causes all edges to be unmarked.
//--------------------------------------------------------------------
void geoTwinMesh::UnmarkAll()
{
	envSTLHelpers::ForAll(m_TwinEdges, std::mem_fn(&geoTwinEdge::UnmarkFace));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void geoTwinMesh::GetEmptyEdges(std::vector<geoTwinEdge*>& o_Edges)
{
	int i;
	int num = m_TwinEdges.size();
	for( i = 0 ; i < num ; ++i ) 
	{
		if( m_TwinEdges[i]->GetFaceEmpty() )
			o_Edges.push_back(m_TwinEdges[i]);
	}
}

//--------------------------------------------------------------------
//	GetNonEmptyEdges returns those edges which have triangles on
//	two sides.  Only one edge out of each edge-twin pair will be
//	returned.
//--------------------------------------------------------------------
void geoTwinMesh::GetNonEmptyEdges(std::vector<geoTwinEdge*>& o_Edges)
{
	//	our clever trick to return only one edge, but still do only one pass,
	//	is to only return the edge with the smaller memory pointer value.
	int i;
	int num = m_TwinEdges.size();
	for( i = 0 ; i < num ; ++i ) 
	{
		geoTwinEdge* e0 = m_TwinEdges[i];
		
		if( !e0->GetFaceEmpty() )
		{
			geoTwinEdge* e1 = e0->GetTwin();

			if( !e1->GetFaceEmpty() )
			{
				if( e0 < e1 )
					o_Edges.push_back(e0);
			}
		}
	}
}

//--------------------------------------------------------------------
//	GetUnmarkedNonEmptyEdges returns edges which have mark = false
//	and empty = false.
//--------------------------------------------------------------------
void geoTwinMesh::GetUnmarkedNonEmptyEdges(std::vector<geoTwinEdge*>& o_Edges)
{
	int i;
	int num = m_TwinEdges.size();
	for( i = 0 ; i < num ; ++i ) 
	{
		geoTwinEdge* edge = m_TwinEdges[i];
		if( !(edge->GetFaceEmpty() || edge->GetFaceMarked()) )
			o_Edges.push_back(edge);
	}
}

//--------------------------------------------------------------------
//	test_edge_triangulation is a debugging function which will test
//	an invariant of the mesh - that each non-empty face must have
//	three edges.
//--------------------------------------------------------------------
void geoTwinMesh::test_edge_triangulation()
{
	int num = this->m_TwinEdges.size();
	int i;
	for( i = 0 ; i < num ; ++i )
	{
		geoTwinEdge* e0 = m_TwinEdges[i];
		geoTwinEdge* e1 = e0->GetTwin();

		if( !e0->GetFaceEmpty() )
		{
			DBG_ASSERT(e0->GetNext()->GetNext()->GetNext() == e0, "found an interior edge which is not a triangle");
			DBG_ASSERT(e0->GetPrev()->GetPrev()->GetPrev() == e0, "found an interior edge which is not a triangle");
		}
		if( !e1->GetFaceEmpty() )
		{
			DBG_ASSERT(e1->GetNext()->GetNext()->GetNext() == e1, "found an interior edge which is not a triangle");
			DBG_ASSERT(e1->GetPrev()->GetPrev()->GetPrev() == e1, "found an interior edge which is not a triangle");
		}
	}

	num = m_TwinVertices.size();
	for( i = 0 ; i < num ; ++i )
	{
		geoTwinVertex* vertex = m_TwinVertices[i];
		geoTwinVertexEdgeIterator it(vertex);

		while( it.GetEdge() )
		{
			DBG_ASSERT(it.GetEdge()->GetVertex() == vertex, "miswired vertex");
			++it;
		}		
		
	}
}



