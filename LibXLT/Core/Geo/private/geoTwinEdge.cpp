/****************************************************************************\
**	geoTwinEdge.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/geo/geoTwinEdge.hpp"


//============================================================================
//============================================================================
envPool geoTwinVertex::m_Pool(sizeof(geoTwinVertex), 32);
envPool geoTwinEdge::m_Pool(sizeof(geoTwinEdge), 32);


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
geoTwinEdge::geoTwinEdge(geoTwinVertex* io_Vertex)
:	m_Vertex(io_Vertex),
	m_Next(NULL),
	m_Prev(NULL),
	m_Twin(NULL),
	m_bFaceMarked(false),
	m_bFaceEmpty(true)
{
	io_Vertex->SetEdge(this);
}

//--------------------------------------------------------------------
//	MarkFace and UnmarkFace cause all edges adjacent to the face
//	adjacent to this to be marked or unmarked.
//	This will cause a problem if the edge does not actually bound
//	a closed face!
//--------------------------------------------------------------------
void geoTwinEdge::MarkFace()
{
	m_bFaceMarked = true;
	geoTwinEdge* next = m_Next;
	while( next != this )
	{
		next->m_bFaceMarked = true;
		next = next->m_Next;
	}
}

void geoTwinEdge::UnmarkFace()
{
	m_bFaceMarked = false;
	geoTwinEdge* next = m_Next;
	while( next != this )
	{
		next->m_bFaceMarked = false;
		next = next->m_Next;
	}
}


