/*****************************************************************************
**	smdlSubdivUtil.cpp
**
**	smdlSubdivUtil provides structures and functions for subdivision.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlSubdivUtil.hpp"

#include "Core/ma/maConstants.hpp"


//============================================================================
//============================================================================
namespace smdlSubdivUtil
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool EdgeCompare::operator()(const sEdge* a, const sEdge* b)
	{
		if ( a->m_pFaces[0] == b->m_pFaces[0] ) return true;
		return false;
	}

	//--------------------------------------------------------------------
	// constructor
	//--------------------------------------------------------------------
	sVert::sVert()
		: m_pNext(NULL), m_bSmoothed(true)
	{
	}

#ifdef QUAD_PATCHES
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void sVert::AddSharedEdge( sEdge* pEdge )
	{
		//check if edge or shared edge exists
		EdgeList2::iterator eit = m_SharedEdgeList.begin();
		for ( ; eit != m_SharedEdgeList.end(); eit++ )
		{
			if ( *eit == pEdge || (*eit)->m_pOther == pEdge ) return;
		}
		m_SharedEdgeList.insert( eit, pEdge );
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void sVert::AddSharedFace( sFace* pFace )
	{
		FaceList::iterator fit = m_SharedFaceList.begin();
		for ( ; fit != m_SharedFaceList.end(); fit++ )
		{
			if ( *fit == pFace ) return;
		}
		m_SharedFaceList.insert( fit, pFace );
	}

	//--------------------------------------------------------------------
	//sorts for winding order around this vertex starting at face
	//--------------------------------------------------------------------
	void sVert::SortSharedLists( sFace* pFace )
	{
		EdgeList2 EdgeSort;
		FaceList  FaceSort;

		//get edge for this vertex;
		sVert* pNext = pFace->NextVert( this );
		sVert* pPrev = pFace->PrevVert( this );

		DBG_ASSERT( pNext, "Invalid Vertex in face" );
		DBG_ASSERT( pPrev, "Invalid Vertex in face" );
		if (!pNext || !pPrev)
			return;

		sEdge* pBaseEdge = GetEdge( pNext );
		sEdge* pPrevEdge = GetEdge( pPrev );
		
		DBG_ASSERT( pBaseEdge, "Base edge invalid" );
		DBG_ASSERT( pPrevEdge, "Prev edge invalid" );
		if (!pBaseEdge || !pPrevEdge)
			return;

		EdgeSort.push_back( pBaseEdge );
		EdgeSort.push_back( pPrevEdge );
		FaceSort.push_back( pFace );

		//remove the edge from the shared list including it's shared
		EdgeList2::iterator eit = m_SharedEdgeList.begin();
		for ( ; eit != m_SharedEdgeList.end(); eit++ )
		{
			if ( *eit == pBaseEdge || (*eit)->m_pOther == pBaseEdge )
			{
				m_SharedEdgeList.erase( eit );
				break;
			}
		}
		eit = m_SharedEdgeList.begin();
		for ( ; eit != m_SharedEdgeList.end(); eit++ )
		{
			if ( *eit == pPrevEdge || (*eit)->m_pOther == pPrevEdge )
			{
				m_SharedEdgeList.erase( eit );
				break;
			}
		}

		sEdge* pCurEdge = pPrevEdge;
		sFace* pCurFace = pFace;
		while( m_SharedEdgeList.size() )
		{
			pCurFace = pCurEdge->OtherFace( pCurFace );	//walk to adjacent face of this edge
			DBG_ASSERT( pCurFace, "Need to support boundaries edges." );
			if (!pCurFace)
				continue;
			FaceSort.push_back( pCurFace );

			bool bFound = false;
			//search all remaining edges for a matching shared faces
			EdgeList2::iterator eit2 = m_SharedEdgeList.begin();
			for ( ; eit2 != m_SharedEdgeList.end(); eit2++ )
			{
				if ( (*eit2)->m_pFaces[0] == pCurFace ||
					(*eit2)->m_pFaces[1] == pCurFace )
				{
					pCurEdge = *eit2;
					EdgeSort.push_back( pCurEdge );	//add to sorted list

					EdgeList2::iterator eit3 = m_SharedEdgeList.begin();
					for ( ; eit3 != m_SharedEdgeList.end(); eit3++ )
					{
						if ( *eit3 == *eit2 || (*eit3)->m_pOther == *eit2 )
						{
							m_SharedEdgeList.erase( eit3 );
							break;
						}
					}
					bFound = true;
					break;
				}
			}
			DBG_ASSERT( bFound, "Support boundary edges. " );
		}

		if ( pCurEdge )	//link to final face
		{
			pCurFace = pCurEdge->OtherFace( pCurFace );	//walk to adjacent face of this edge
			DBG_ASSERT( pCurFace, "Need to support boundaries edges." );
			if (pCurFace)
				FaceSort.push_back( pCurFace );
		}


		m_SharedEdgeList = EdgeSort;	//supplant the original lists with the new sorted ones.
		m_SharedFaceList = FaceSort;	
	}
#endif//QUAD_PATCHES

	//--------------------------------------------------------------------
	// Add this vertex to our linked list of shared vertices
	//--------------------------------------------------------------------
	void sVert::ConnectSharedVert( sVert* i_pVert )
	{
		// Make sure we don't already have the vertex in our list
		if (IsSharedWith(i_pVert))
			return;

		DBG_ASSERT(i_pVert->m_pNext == NULL, "Pass new vertex as argument to ConnectSharedVert");
		if (i_pVert->m_pNext != NULL)
			return;

		if (m_pNext)
		{
			// Insert into list
			i_pVert->m_pNext = m_pNext;
			m_pNext = i_pVert;
		}
		else
		{
			i_pVert->m_pNext = this;
			m_pNext = i_pVert;
		}
#ifdef QUAD_PATCHES
		//support for shared edges
		EdgeList2::iterator eit = m_SharedEdgeList.begin();
		for ( ; eit != m_SharedEdgeList.end(); eit++ )
		{
			i_pVert->AddSharedEdge( *eit );
		}
#endif//QUAD_PATCHES
	}

	//--------------------------------------------------------------------
	// Look for existing edge between this vertex and the given vertex
	// Will return NULL if not found.
	//--------------------------------------------------------------------
	sEdge*	sVert::GetEdge( sVert* pOther )
	{
		for ( int i=0; i<m_EdgeList.size(); i++ )
		{
			if ( m_EdgeList[i]->Contains( pOther ) )
				return m_EdgeList[i];
		}
		//DBG_ASSERT(false, "GetEdge - could not find edge");
		return NULL;
	}

	//--------------------------------------------------------------------
	// Returns true if this vertex is one of the shared vertices
	// in the linked list.
	//--------------------------------------------------------------------
	bool sVert::IsSharedWith( sVert* pOther )
	{
		sVert *pCur = this->m_pNext; 
		while (pCur)
		{
			if (pCur == pOther) return true;
			
			pCur = pCur->m_pNext;
			if (pCur == this) break; // looped around
		}
		return false;
	}

	//--------------------------------------------------------------------
	// Returns true if the input vertex is this vertex or any of the shared vertices
	//--------------------------------------------------------------------
	bool sVert::Contains( sVert* pOther )
	{
		if ( pOther == this ) return true;
		return IsSharedWith( pOther );
	}


	//--------------------------------------------------------------------
	// Look through shared vertex linked list for an edge that matches
	// this one such that the vertices are in each other's linked lists.
	//--------------------------------------------------------------------
	sEdge*	sVert::FindSharedEdge( sVert* pOther )
	{
		// start with current edge because we might find
		// an edge match where one vertex is shared and one vertex isn't
		sVert *pCur = this;
//		while (pCur)
		do
		{
			// Handle this vertex
			for ( int i=0; i<pCur->m_EdgeList.size(); i++ )
			{
				sVert* pVert = pCur->m_EdgeList[i]->Other( pCur );
				if ( (pVert == pOther) || (pVert->IsSharedWith(pOther)) )
					return pCur->m_EdgeList[i];
			}
			
			pCur = pCur->m_pNext;
//			if (pCur == this) break; // looped around
		} while( pCur && pCur != this );
		return NULL;
	}

	//--------------------------------------------------------------------
	// Compute new vertex location, storing info into the given 
	//	vertex structure
	//--------------------------------------------------------------------
	void sVert::ComputeNewVertexLocation( sVert& o_Vert )
	{
		maPoint3d Q(0,0,0), R(0,0,0);
		maPoint2d QUV(0,0), RUV(0,0);

		// Average the edge midpoints
		float fNumEdges = 0; 
		int creaseCount = 0, sharedCount = 0;
		maPoint3d edgeAvg(0,0,0);
		maPoint2d edgeAvgUV(0,0);
		sVert *pCur = this; 
		while (pCur)
		{
			const int edge_count = pCur->m_EdgeList.size();
			for (int e=0; e<edge_count; e++)
			{
				sEdge *pEdge = pCur->m_EdgeList[e];
				// Shared edges get counted twice, trying to compensate
				bool shared_edge = (pEdge->m_pOther != NULL);
				if (shared_edge)
				{
					fNumEdges += 0.5f;

					R += pEdge->m_MidPoint * 0.5f;
					RUV += pEdge->m_MidPointUV * 0.5f;

					// shared edge can't be a boundary edge
					//if (pEdge->IsBoundary() || pEdge->m_bCreased)
					if (pEdge->m_bCreased)
					{
						creaseCount++;
						sharedCount++;
						edgeAvg += pEdge->m_MidPoint * 0.5f;
						edgeAvgUV += pEdge->m_MidPointUV * 0.5f;
					}
				}
				else
				{
					fNumEdges += 1;
					R += pEdge->m_MidPoint;
					RUV += pEdge->m_MidPointUV;
					if (pEdge->IsBoundary() || pEdge->m_bCreased)
					{
						creaseCount++;
						edgeAvg += pEdge->m_MidPoint;
						edgeAvgUV += pEdge->m_MidPointUV;
					}
				}
			}
			pCur = pCur->m_pNext;
			if (pCur == this) break; // looped around
		}
		R /= fNumEdges;
		RUV /= fNumEdges;
		creaseCount -= sharedCount;

		if (creaseCount < 2)
		{
			// Average face centers from all vertices that share this position
			int nFaces = 0;
			pCur = this; 
			while (pCur)
			{
				const int face_count = pCur->m_FaceList.size();
				nFaces += face_count;
				for (int f=0; f<face_count; f++)
				{
					Q += pCur->m_FaceList[f]->m_Average.m_Loc;
					QUV += pCur->m_FaceList[f]->m_Average.m_UV;
				}
				pCur = pCur->m_pNext;
				if (pCur == this) break; // looped around
			}
			Q /= (float) nFaces;
			QUV /= (float) nFaces;

			// This is the blend for Catmull-Clark for new vertex locations:
			//  (Q + 2R + S(n-3)) / n, where n is valence of vertex.
			o_Vert.m_Loc = (Q + R * 2 + this->m_Loc * (fNumEdges-3)) / fNumEdges;
			if (this->m_pNext) // split vertex, so don't blend UVs
				o_Vert.m_UV  = this->m_UV;
			else
				o_Vert.m_UV = (QUV + RUV * 2 + this->m_UV * (fNumEdges-3)) / fNumEdges;
			o_Vert.m_bSmoothed = true;
		}
		else if (creaseCount == 2)
		{
			// vertex lies along a sharp crease.
			// Note: this is my interpretation of a formula in a paper,
			// it seems right, but could be wrong slightly.
			o_Vert.m_Loc = (this->m_Loc * 2 + edgeAvg) / 4.0f;
			o_Vert.m_UV  = this->m_UV;
			//o_Vert.m_UV = (this->m_UV * 2 + edgeAvgUV) / 4.0f;
			o_Vert.m_bSmoothed = false;
		}
		else
		{
			// vertex is a sharp corner, stay at current position
			o_Vert.m_Loc = this->m_Loc;
			o_Vert.m_UV = this->m_UV;
			o_Vert.m_bSmoothed = false;
		}

		o_Vert.m_Norm = this->m_Norm;
	}

	//--------------------------------------------------------------------
	// Used in the normal computation step, add contribution for
	// this normal. If this vertex is smoothed, add this normal
	// to all shared vertices.
	//--------------------------------------------------------------------
	void sVert::AddNormal( const maPoint3d& i_Normal )
	{
		this->m_Norm += i_Normal;

		if (this->m_bSmoothed)
		{
			sVert *pCur = this->m_pNext; 
			while (pCur)
			{
				pCur->m_Norm += i_Normal;
				
				pCur = pCur->m_pNext;
				if (pCur == this) break; // looped around
			}
		}
	}

	//--------------------------------------------------------------------
	// Compute the location for the new subdivided vertex for this edge
	//--------------------------------------------------------------------
	void sEdge::GenerateNewVertexLocation(int new_index)
	{
		// Note: If we use m_pFaces[1] to signify the other face even
		// when this edge is split because of a texture coord boundary,
		// then we can just use that pointer directly instead of
		// checking the m_pOther pointer
		//
		//sFace *pNeighbor = (m_pOther) ? m_pOther->m_pFaces[0] : m_pFaces[1];
		sFace *pNeighbor = m_pFaces[1];
		if (pNeighbor == NULL || m_bCreased) // boundary edge or creased
		{
			// Sharp rules
			m_NewVert.m_Loc = m_MidPoint;
			m_NewVert.m_UV = m_MidPointUV;
			m_NewVert.m_bSmoothed = false;
		}
		else
		{
			// Smooth rules
			m_NewVert.m_Loc = (2*m_MidPoint + m_pFaces[0]->m_Average.m_Loc + pNeighbor->m_Average.m_Loc) / 4.0;
			if (m_pOther)
				m_NewVert.m_UV = m_MidPointUV;
			else
				m_NewVert.m_UV = (2*m_MidPointUV + m_pFaces[0]->m_Average.m_UV + pNeighbor->m_Average.m_UV) / 4.0;
			m_NewVert.m_bSmoothed = true;
		}

		// Assign the new vertex an index (this is useful later)
		m_NewVert.m_Index = new_index;
	}

#ifdef QUAD_PATCHES
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void sEdge::AssignSharedEdge( sEdge* pEdge )
	{
		m_pVert[0]->AddSharedEdge( pEdge );
		m_pVert[1]->AddSharedEdge( pEdge );
	}
#endif//QUAD_PATCHES

	//--------------------------------------------------------------------
	// Init from an array of vertices
	//--------------------------------------------------------------------
	void sFace::Init( sVert** pVerts, int i_NumVerts)
	{
		m_NumVerts = i_NumVerts;
		m_VertList.resize(m_NumVerts);
		for (int i=0; i<i_NumVerts; i++)
		{
			m_VertList[i] = pVerts[i];
			m_VertList[i]->AddFace( this );
		}

	}
}
