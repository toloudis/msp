/****************************************************************************\
**	geoTwinEdge.hpp
**
**	The geoTwinEdge component contains geoTwinVertex and geoTwinEdge which
**	implement a "twin-edge" mesh data structure.  The twin-edge data
**	is designed to allow easy navigation between connected faces, edges,
**	and vertices in a mesh.  Each polygon edge is composed of two directed
**	geoTwinEdges; each geoTwinEdge has a pointer to the next and previous
**	edges, the origin geoTwinVertex, and its "twin" edge, running in the
**	opposite direction but coincident.
**
**	This twin-edge implementation may be somewhat specialized for particular
**	operations on triangle meshes but could be generalized, of course.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GEO_TWINEDGE_HPP
#error geoTwinEdge.hpp multiply included
#endif
#define GEO_TWINEDGE_HPP

#ifndef ENV_POOL_HPP
#include "Core/env/envPool.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif


//============================================================================
//============================================================================
class geoTwinEdge;


//============================================================================
//============================================================================
class geoTwinVertex
{
	public:

		typedef envType::UInt32 IndexType;

		//--------------------------------------------------------------------
		//	Each geoTwinVertex must be created from a vertex index, which
		//	doesn't change over the lifetime of the geoTwinVertex.
		//--------------------------------------------------------------------
		inline geoTwinVertex(IndexType i_Index);

		//--------------------------------------------------------------------
		//	Each vertex points to one edge whose origin is the vertex.  There
		//	is no rule for determining which edge of the possibly several
		//	choices may be pointed to.
		//--------------------------------------------------------------------
		inline void SetEdge(geoTwinEdge* i_Edge);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline geoTwinEdge* GetEdge() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline IndexType GetIndex() const;

		//----------------------------------------------------------------------------
		//	the geoTwinVertex uses a pool allocator.
		//----------------------------------------------------------------------------
		inline void* operator new(size_t size);
		inline void operator delete(void* i_Ptr);

	private:

		static envPool	m_Pool;
		geoTwinEdge*	m_Edge;
		IndexType		m_Index;
};

//============================================================================
//============================================================================
class geoTwinEdge
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		geoTwinEdge(geoTwinVertex* io_Vertex);

		//--------------------------------------------------------------------
		//	The next edge is the edge which has the destination vertex 
		//	of this edge as its origin and adjoins the same face.
		//--------------------------------------------------------------------
		inline void SetNext(geoTwinEdge* i_Edge);

		//--------------------------------------------------------------------
		//	The previous edge is the edge whose next edge is this edge.
		//--------------------------------------------------------------------
		inline void SetPrev(geoTwinEdge* i_Edge);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline geoTwinEdge* GetNext();
		inline const geoTwinEdge* GetNext() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline geoTwinEdge* GetPrev();
		inline const geoTwinEdge* GetPrev() const;

		//--------------------------------------------------------------------
		//	This is the vertex at the origin of the edge.
		//--------------------------------------------------------------------
		inline geoTwinVertex* GetVertex();
		inline const geoTwinVertex* GetVertex() const;

		//--------------------------------------------------------------------
		//	The twin edge is at the same position but faces the opposite
		//	direction.
		//--------------------------------------------------------------------
		inline void SetTwin(geoTwinEdge* i_Edge);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline geoTwinEdge* GetTwin();
		inline const geoTwinEdge* GetTwin() const;

		//--------------------------------------------------------------------
		//	The edge's face is empty if it is not considered "filled" by a
		//	polygon.  This is the same as saying that it is adjacent to
		//	empty space.
		//--------------------------------------------------------------------
		inline void SetFaceEmpty(bool i_bEmpty);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline bool GetFaceEmpty() const;

		//----------------------------------------------------------------------------
		//	the geoTwinEdge uses a pool allocator.
		//----------------------------------------------------------------------------
		inline void* operator new(size_t size);
		inline void operator delete(void* i_Ptr);

		//--------------------------------------------------------------------
		//	This returns a user defined "mark" bool for the face adjacent to
		//	the edge; this can be used for things like ensuring that all
		//	faces are visited once in a traversal.
		//	This value is initially false.
		//--------------------------------------------------------------------
		inline bool GetFaceMarked() const;

		//--------------------------------------------------------------------
		//	MarkFace and UnmarkFace cause all edges adjacent to the face
		//	adjacent to this to be marked or unmarked.
		//	This will cause a problem if the edge does not actually bound
		//	a closed face!
		//--------------------------------------------------------------------
		void MarkFace();
		void UnmarkFace();

	private:

		static envPool	m_Pool;
		geoTwinEdge*	m_Next;
		geoTwinEdge*	m_Prev;
		geoTwinVertex*	m_Vertex;
		geoTwinEdge*	m_Twin;
		bool	m_bFaceEmpty;
		bool	m_bFaceMarked;
};

//============================================================================
//	implementation
//============================================================================
//--------------------------------------------------------------------
//	Each geoTwinVertex must be created from a vertex index, which
//	doesn't change over the lifetime of the geoTwinVertex.
//--------------------------------------------------------------------
inline geoTwinVertex::geoTwinVertex(IndexType i_Index)
:	m_Index(i_Index),
	m_Edge(NULL)
{
}

//--------------------------------------------------------------------
//	Each vertex points to one edge whose origin is the vertex.  There
//	is no rule for determining which edge of the possibly several
//	choices may be pointed to.
//--------------------------------------------------------------------
inline void geoTwinVertex::SetEdge(geoTwinEdge* i_Edge)
{
	m_Edge = i_Edge;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline geoTwinEdge* geoTwinVertex::GetEdge() const
{
	return m_Edge;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline geoTwinVertex::IndexType geoTwinVertex::GetIndex() const
{
	return m_Index;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline void* geoTwinVertex::operator new(size_t size)
{
	return m_Pool.Allocate();
}

inline void geoTwinVertex::operator delete(void* i_Ptr)
{
	m_Pool.Deallocate(i_Ptr);
}


//============================================================================
//============================================================================

//--------------------------------------------------------------------
//	The next edge is the edge which has the destination vertex 
//	of this edge as its origin and adjoins the same face.
//--------------------------------------------------------------------
inline void geoTwinEdge::SetNext(geoTwinEdge* i_Edge)
{
	m_Next = i_Edge;
}

//--------------------------------------------------------------------
//	The previous edge is the edge whose next edge is this edge.
//--------------------------------------------------------------------
inline void geoTwinEdge::SetPrev(geoTwinEdge* i_Edge)
{
	m_Prev = i_Edge;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline geoTwinEdge* geoTwinEdge::GetNext()
{
	return m_Next;
}

inline const geoTwinEdge* geoTwinEdge::GetNext() const
{
	return m_Next;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline geoTwinEdge* geoTwinEdge::GetPrev()
{
	return m_Prev;
}

inline const geoTwinEdge* geoTwinEdge::GetPrev() const
{
	return m_Prev;
}

//--------------------------------------------------------------------
//	This is the vertex at the origin of the edge.
//--------------------------------------------------------------------
inline geoTwinVertex* geoTwinEdge::GetVertex()
{
	return m_Vertex;
}

inline const geoTwinVertex* geoTwinEdge::GetVertex() const
{
	return m_Vertex;
}

//--------------------------------------------------------------------
//	The twin edge is at the same position but faces the opposite
//	direction.
//--------------------------------------------------------------------
inline void geoTwinEdge::SetTwin(geoTwinEdge* i_Edge)
{
	m_Twin = i_Edge;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline geoTwinEdge* geoTwinEdge::GetTwin()
{
	return m_Twin;
}

inline const geoTwinEdge* geoTwinEdge::GetTwin() const
{
	return m_Twin;
}

//--------------------------------------------------------------------
//	The edge's face is empty if it is not considered "filled" by a
//	polygon.  This is the same as saying that it is adjacent to
//	empty space.
//--------------------------------------------------------------------
inline void geoTwinEdge::SetFaceEmpty(bool i_bEmpty)
{
	m_bFaceEmpty = i_bEmpty;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool geoTwinEdge::GetFaceEmpty() const
{
	return m_bFaceEmpty;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool geoTwinEdge::GetFaceMarked() const
{
	return m_bFaceMarked;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline void* geoTwinEdge::operator new(size_t size)
{
	return m_Pool.Allocate();
}

inline void geoTwinEdge::operator delete(void* i_Ptr)
{
	m_Pool.Deallocate(i_Ptr);
}
