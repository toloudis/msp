/****************************************************************************\
**	geoTwinVertexEdgeIterator.hpp
**
**	The geoTwinVertexEdgeIterator iterates through the edges which have their
**	origin at the given geoTwinVertex.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GEO_TWINVERTEXEDGEITERATOR_HPP
#error geoTwinEdge.hpp multiply included
#endif
#define GEO_TWINVERTEXEDGEITERATOR_HPP

#ifndef GEO_TWINEDGE_HPP
#include "Core/geo/geoTwinEdge.hpp"
#endif


//============================================================================
//============================================================================
class geoTwinVertexEdgeIterator
{
	public:

		//--------------------------------------------------------------------
		//	The iterator must be created with the geoTwinVertex to get the
		//	edges for.
		//--------------------------------------------------------------------
		inline geoTwinVertexEdgeIterator(geoTwinVertex* i_Vertex);

		//--------------------------------------------------------------------
		//	GetEdge will return the current edge at the iterator, or NULL
		//	if the iteration is complete.
		//--------------------------------------------------------------------
		inline geoTwinEdge* GetEdge();

		//--------------------------------------------------------------------
		//	++ moves to the next edge.
		//--------------------------------------------------------------------
		inline void operator ++();

	private:

		geoTwinEdge*	m_InitEdge;
		geoTwinEdge*	m_CurEdge;
		int				m_InfiniteLoopBreaker;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline geoTwinVertexEdgeIterator::geoTwinVertexEdgeIterator(geoTwinVertex* i_Vertex)
: m_InfiniteLoopBreaker(0)
{
	m_InitEdge = m_CurEdge = i_Vertex->GetEdge();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline geoTwinEdge* geoTwinVertexEdgeIterator::GetEdge()
{
	return m_CurEdge;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline void geoTwinVertexEdgeIterator::operator ++()
{
	if( m_CurEdge )
	{
		m_CurEdge = m_CurEdge->GetTwin()->GetNext();

		if( m_CurEdge == m_InitEdge )
			m_CurEdge = NULL;

		if( m_InfiniteLoopBreaker++ > 10000 )
			m_CurEdge = NULL;
	}
}
