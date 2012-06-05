/****************************************************************************\
**	smdlSubdivUtil.hpp
**
**	smdlSubdivUtil provides structures and functions for subdivision.
**
**	The initial implementation here was based on Perez's "Advanced
**	3-D Game Programming with DirectX 7.0" but that implementation
**	was butterfly subdivision and Maya uses Catmull-Clark.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SUBDIVUTIL_HPP
#error smdlSubdivUtil.hpp multiply included
#endif
#define SMDL_SUBDIVUTIL_HPP

#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <algorithm>
#include <list>
#include <set>
#include <vector>


//============================================================================
//this adds shared vertex information for quad patches
//============================================================================
//#define QUAD_PATCHES


//============================================================================
//============================================================================
namespace smdlSubdivUtil
{
	struct sVert;
	struct sFace;
	struct sEdge;

	typedef std::vector<sVert*>	VertList;
	typedef std::vector<sEdge*>	EdgeList;
	typedef std::vector<sFace*>	FaceList;
	typedef std::list<sEdge*>	EdgeList2;

	//============================================================================
	//============================================================================
	class EdgeCompare
	{
	public:
		bool operator()(const sEdge* a, const sEdge* b);
	};


	//============================================================================
	// Subdivision Surface vertex (name 'sVertex' is used in D3D code)
	//============================================================================
	struct sVert
	{
		//--------------------------------------------------------------------
		// These two arrays describe the adjacency information 
		// for a vertex. Each vertex knows who all of it's neighboring 
		// edges and triangles are. an important note is that these 
		// lists aren't sorted.  We need to search through the list 
		// when we need to get a specific adjacent triangle.
		// This is, of course, inefficient.  Consider sorted insertion
		// an excercise to the reader.
		//--------------------------------------------------------------------
		FaceList	m_FaceList;
		EdgeList	m_EdgeList;

		//--------------------------------------------------------------------
		// D3D Vertices are separated if they have different UVs or normals,
		//	even if they have the same position. This pointer connects
		//	all vertices at the same position in a linked list
		//--------------------------------------------------------------------
		sVert* m_pNext;
		bool m_bSmoothed;  // if true, use shared verts also to generate normals

		//--------------------------------------------------------------------
		// position/normal information for the vertex
		//--------------------------------------------------------------------
		maPoint3d m_Loc;
		maVector3d m_Norm;
		maVector2d m_UV;
		maVector3d m_S,m_T;

		//--------------------------------------------------------------------
		// Each Vertex knows it's position in the array it lies in.  
		// This helps when we're constructing the arrays of subdivided data.
		//--------------------------------------------------------------------
		int		m_Index;

		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		sVert();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AddEdge( sEdge* pEdge )
		{
			DBG_ASSERT( 0 == std::count( m_EdgeList.begin(), m_EdgeList.end(), pEdge ), "Edge already added" );
			if (0 != std::count( m_EdgeList.begin(), m_EdgeList.end(), pEdge ))
				return;
			m_EdgeList.push_back( pEdge );
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AddFace( sFace* pFace )
		{
			//DBG_ASSERT( 0 == std::count( m_FaceList.begin(), m_FaceList.end(), pFace ), "Triangle already added" );
			m_FaceList.push_back( pFace );
		}

		//--------------------------------------------------------------------
		// Valence == How many other vertices are connected to this one
		// which said another way is how many edges the vert has.
		//--------------------------------------------------------------------
		int Valence()
		{
			return m_EdgeList.size();
		}

		//--------------------------------------------------------------------
		// Add this vertex to our linked list of shared vertices
		//--------------------------------------------------------------------
		void ConnectSharedVert( sVert* i_pVert );

		//--------------------------------------------------------------------
		// Look for existing edge between this vertex and the given vertex.
		// Will return NULL if not found.
		//--------------------------------------------------------------------
		sEdge*	GetEdge( sVert* pOther );

		//--------------------------------------------------------------------
		// Returns true if this vertex is one of the shared vertices
		// in the linked list.
		//--------------------------------------------------------------------
		bool IsSharedWith( sVert* pOther );

		//--------------------------------------------------------------------
		// Returns true if the input vertex is this vertex or any of the shared vertices
		//--------------------------------------------------------------------
		bool Contains( sVert* pOther );

		//--------------------------------------------------------------------
		// Look through shared vertex linked list for an edge that matches
		// this one such that the vertices are in each other's linked lists.
		//--------------------------------------------------------------------
		sEdge*	FindSharedEdge( sVert* pOther );

		//--------------------------------------------------------------------
		// Compute new vertex location, storing info into the given 
		//	vertex structure
		//--------------------------------------------------------------------
		void ComputeNewVertexLocation( sVert& o_Vert );

		//--------------------------------------------------------------------
		// Used in the normal computation step, add contribution for
		// this normal. If this vertex is smoothed, add this normal
		// to all shared vertices.
		//--------------------------------------------------------------------
		void AddNormal( const maPoint3d& i_Normal );

//---------------------For Bezier Conversion---------------------------
#ifdef QUAD_PATCHES
		EdgeList2	m_SharedEdgeList;
		FaceList	m_SharedFaceList;

		//--------------------------------------------------------------------
		// Valence == How many other vertices are connected to this one
		// which said another way is how many edges the vert has.
		// This version extends to shared vertices
		//--------------------------------------------------------------------
		int SharedValence()
		{
			return m_SharedEdgeList.size();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AddSharedEdge( sEdge* pEdge );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AddSharedFace( sFace* pFace );

		void SortSharedLists( sFace* pFace );	//sorts for winding order around this vertex starting at face
#endif//QUAD_PATCHES
	};
	
	//--------------------------------------------------------------------
	// Edge structure that connects two vertices in a SubSurf
	//--------------------------------------------------------------------
	struct sEdge
	{
		sVert*	m_pVert[2];
		sFace*	m_pFaces[2]; // The second face pointer can be NULL in boundary conditions
		bool	m_bCreased;

		//--------------------------------------------------------------------
		// Because of UVs or creasing, this edge may be split from 
		// its connection to its partner face. In these situations,
		// this pointer is used to maintain the mesh connectivity.
		//--------------------------------------------------------------------
		sEdge* m_pOther;

		//--------------------------------------------------------------------
		// The mid-point of the edge is used in computing the new edge 
		// vertex location and in computing the new vertex locations
		//--------------------------------------------------------------------
		maPoint3d	m_MidPoint;
		maVector2d	m_MidPointUV;

		//--------------------------------------------------------------------
		// When we perform the subdivision calculations on all the edges
		// the result is held in this m_NewVLoc.  Never has any
		// connectivity information, just location and color.
		//--------------------------------------------------------------------
		sVert	m_NewVert;

#ifdef _DEBUG
		int m_Index;	//index the edges for debugging
#endif
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		sEdge()
			: m_pOther(NULL), m_bCreased(false)
		{
			m_pVert[0] = m_pVert[1] = NULL;
			m_pFaces[0] = m_pFaces[1] = NULL;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Init( sVert* v0, sVert* v1, sFace* pFace )
		{
			m_pVert[0] = v0;
			m_pVert[1] = v1;
			m_pFaces[0] = pFace;

			// Note that the edge notifies both of it's vertices that it's 
			// connected to them.
			m_pVert[0]->AddEdge( this );
			m_pVert[1]->AddEdge( this );

//			AssignSharedEdge( this );
		}

		//--------------------------------------------------------------------
		// When the other face that shares this edge is found, 
		//	this function is called to assign that pointer.
		//--------------------------------------------------------------------
		void AssignOtherFace(sFace* pFace)
		{
			m_pFaces[1] = pFace;
		}

		//--------------------------------------------------------------------
		// Returns true if this edge is not shared by two faces.
		//--------------------------------------------------------------------
		bool IsBoundary()
		{
			//return (m_pFaces[1] == NULL && m_pOther == NULL);
			//return (m_pFaces[1] == NULL);
			bool boundary = (m_pFaces[1] == NULL);
			return boundary;
		}

		//--------------------------------------------------------------------
		// Returns true if this edge is not shared by two faces.
		// Extends to shared vertices across UV splits to maintain a smooth mesh.
		//--------------------------------------------------------------------
		bool IsSharedBoundary()
		{
			if ( m_pOther )
			{
				return false;
			}
/*			if ( m_pVert[0]->m_pNext && m_pVert[1]->m_pNext )
			{
				return false;
			}*/
			return (m_pFaces[1] == NULL);
		}

		//--------------------------------------------------------------------
		// ComputeMidpoint - computes vertex that is edge's midpoint
		//--------------------------------------------------------------------
		void ComputeMidpoint()
		{
			m_MidPoint = (m_pVert[0]->m_Loc + m_pVert[1]->m_Loc) * 0.5f;
			m_MidPointUV = (m_pVert[0]->m_UV + m_pVert[1]->m_UV) * 0.5f;
		}

		//--------------------------------------------------------------------
		// true == one of the edges' vertices is the inputted vertex
		//--------------------------------------------------------------------
		bool Contains( sVert* pVert )
		{
			return (m_pVert[0] == pVert) || m_pVert[1] == pVert;
		}

		//--------------------------------------------------------------------
		// retval = the other vertex than the inputted one
		//--------------------------------------------------------------------
		sVert* Other( sVert* pVert )
		{
			return (m_pVert[0] == pVert) ? m_pVert[1] : m_pVert[0];
		}

		//--------------------------------------------------------------------
		// Returns the other edge than the one specified
		// Will look across shared edges
		// NULL if edges don't contain face
		//--------------------------------------------------------------------
		sFace* OtherFace( sFace* i_NotFace )
		{
			if ( m_pFaces[0] && m_pFaces[0] != i_NotFace ) return m_pFaces[0];
			if ( m_pFaces[1] && m_pFaces[1] != i_NotFace ) return m_pFaces[1];
			if ( m_pOther )
			{
				if ( m_pOther->m_pFaces[0] && m_pOther->m_pFaces[0] != i_NotFace ) return m_pFaces[0];
				if ( m_pOther->m_pFaces[1] && m_pOther->m_pFaces[1] != i_NotFace ) return m_pFaces[1];
			}
			return NULL;
		}

		//-----------------------------------------------------------------------------------------
		//find the face that shares the edge defined by the two vertices and is not the input face (an edge can only share two faces)
		// This method also searches the shared vertices to find a matching face (pNext of a vertex)
		//-----------------------------------------------------------------------------------------
		sFace* SharedFace( sFace* i_NotFace )
		{
/*
			sVert* pV2_Cur = m_pVert[1];
			do 
			{
				sVert* pV1_Cur = m_pVert[0];
				do 
				{
					int faces = pV1_Cur->m_FaceList.size();
					for ( int f = 0; f < faces; f++ )
					{
						sFace* pFace = pV1_Cur->m_FaceList[ f ];
						if ( pFace == i_NotFace ) continue;
						if ( pFace->Contains( pV2_Cur ))	return pFace;
					}
					pV1_Cur = pV1_Cur->m_pNext;
				} while( pV1_Cur && pV1_Cur != m_pVert[0] );
				pV2_Cur = pV2_Cur->m_pNext;
			} while( pV2_Cur && pV2_Cur != m_pVert[1] );
*/
			return NULL;
		}



		//--------------------------------------------------------------------
		// Compute the location for the new subdivided vertex for this edge.
		//	Pass in the index for the new vertex.
		//--------------------------------------------------------------------
		void GenerateNewVertexLocation(int new_index);

		//---------------------For Bezier Conversion---------------------------
#ifdef QUAD_PATCHES
		void AssignSharedEdge( sEdge* pEdge );

		sVert* SharedVert( sVert* pVert )
		{
			//search through first vertex list
			sVert* pCur = m_pVert[0];
			do 
			{
				if ( pCur == pVert ) return m_pVert[0];
				pCur = pCur->m_pNext;
			} while ( pCur && pCur != m_pVert[0] );

			//search through second vertex list
			pCur = m_pVert[1];
			do 
			{
				if ( pCur == pVert ) return m_pVert[1];
				pCur = pCur->m_pNext;
			} while ( pCur && pCur != m_pVert[1] );

			return NULL;
		}
#endif//QUAD_PATCHES
	};

	//--------------------------------------------------------------------
	// Subdivision surface triangle
	//--------------------------------------------------------------------
	struct sFace
	{
		VertList	m_VertList;
		int m_NumVerts;

		//--------------------------------------------------------------------
		// In subdivision process, each face generates a new vertex as
		// the average of its vertices. This vertex never has any
		// connectivity information, just location and color.
		//--------------------------------------------------------------------
		sVert	m_Average;

		// Face normal of this triangle
		maPoint3d	m_Normal;

#ifdef _DEBUG
		int m_Index;		//index the faces for debugging
#endif

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		sFace()
		{
			m_NumVerts = 0;
		}

		//--------------------------------------------------------------------
		// Init from a triangle
		//--------------------------------------------------------------------
		void Init( sVert* v0, sVert* v1, sVert* v2 )
		{
			m_NumVerts = 3;
			m_VertList.resize(m_NumVerts);
			m_VertList[0] = v0;
			m_VertList[1] = v1;
			m_VertList[2] = v2;

			// Note that the triangle notifies all 3 of it's vertices 
			// that it's connected to them.
			m_VertList[0]->AddFace( this );
			m_VertList[1]->AddFace( this );
			m_VertList[2]->AddFace( this );
		}

		//--------------------------------------------------------------------
		// Init from a quad
		//--------------------------------------------------------------------
		void Init( sVert* v0, sVert* v1, sVert* v2, sVert* v3 )
		{
			m_NumVerts = 4;
			m_VertList.resize(m_NumVerts);
			m_VertList[0] = v0;
			m_VertList[1] = v1;
			m_VertList[2] = v2;
			m_VertList[3] = v3;

			// Note that the face notifies all 4 of it's vertices 
			// that it's connected to them.
			m_VertList[0]->AddFace( this );
			m_VertList[1]->AddFace( this );
			m_VertList[2]->AddFace( this );
			m_VertList[3]->AddFace( this );
		}

		//--------------------------------------------------------------------
		// Init from an array of vertices
		//--------------------------------------------------------------------
		void Init( sVert** pVerts, int i_NumVerts);

		//--------------------------------------------------------------------
		// ComputeAverage - computes vertex that is face's average
		//--------------------------------------------------------------------
		void ComputeAverage( int new_index )
		{
			m_Average.m_Loc.Set(0,0,0);
			m_Average.m_Norm.Set(0,0,0);
			m_Average.m_UV.Set(0,0);
			for ( int i=0; i<m_NumVerts; i++ )
			{
				m_Average.m_Loc += m_VertList[i]->m_Loc;
				m_Average.m_Norm += m_VertList[i]->m_Norm;
				m_Average.m_UV += m_VertList[i]->m_UV;
			}
			m_Average.m_Loc /= (float)m_NumVerts;
			m_Average.m_Norm /= (float)m_NumVerts;
			m_Average.m_Norm.Normalize();
			m_Average.m_UV /= (float)m_NumVerts;

			m_Average.m_Index = new_index;
		}

		//--------------------------------------------------------------------
		// true == the triangle contains the inputted vertex
		//--------------------------------------------------------------------
		bool Contains( sVert* pVert )
		{
			return (std::count(m_VertList.begin(), m_VertList.end(), pVert) > 0);
			//return (pVert == m_pVert[0] || pVert == m_pVert[1] || pVert == m_pVert[2]);
		}

		//--------------------------------------------------------------------
		// retval = the third vertex (first and second are inputted).
		// asserts out if inputted values aren't part of the triangle
		//--------------------------------------------------------------------
		sVert* Other( sVert* v1, sVert* v2 )
		{
			// Note: this function is triangle dependent
			DBG_ASSERT( Contains( v1 ) && Contains( v2 ), "Inputted values aren't part of the triangle");
			if (!Contains( v1 ) || !Contains( v2 ))
				return NULL;
			for ( int i=0; i<m_NumVerts; i++ )
			{
				if ( m_VertList[i] != v1 && m_VertList[i] != v2 )
					return m_VertList[i];
			}
			DBG_ASSERT(false, "Can't find other part of triangle");
			return NULL;
		}

		//--------------------------------------------------------------------
		//
		//--------------------------------------------------------------------
		sVert* SharedVert( sVert* pVert )
		{
			for ( int i = 0; i < m_NumVerts; i++ )
			{
				if ( m_VertList[i]->Contains( pVert ) ) return m_VertList[i];
			}
			return NULL;
		}

		//--------------------------------------------------------------------
		// returns the previous vertex in the sequence given the input vertex, handles wrapping
		// Asserts and returns NULL if the input vertex isn't part of the face.
		//--------------------------------------------------------------------
		sVert* PrevVert( sVert* i_V )
		{
			DBG_ASSERT( Contains( i_V ), "Input value isn't part of the face!");
			if (!Contains(i_V))
				return NULL;
			DBG_ASSERT( m_NumVerts == 4, "PrevVerts only work with 4 vertices.");
			if (m_NumVerts != 4)
				return NULL;

			for ( int i = 0; i < m_NumVerts; i++ )
			{
				if ( m_VertList[i] == i_V )
				{
					int prev = (i-1) & 3;
					return m_VertList[prev];
				}
			}
			return NULL;
		}

		//--------------------------------------------------------------------
		// returns the next vertex in the sequence given the input vertex, handles wrapping
		// Asserts and returns NULL if the input vertex isn't part of the face.
		//--------------------------------------------------------------------
		sVert* NextVert( sVert* i_V )
		{
			DBG_ASSERT( Contains( i_V ), "Input value isn't part of the face!");
			if (!Contains(i_V))
				return NULL;
			DBG_ASSERT( m_NumVerts == 4, "NextVerts only work with 4 vertices.");
			if (m_NumVerts != 4)
				return NULL;

			for ( int i = 0; i < m_NumVerts; i++ )
			{
				if ( m_VertList[i] == i_V )
				{
					int next = (i+1) & 3;
					return m_VertList[next];
				}
			}
			return NULL;
		}
	};
}

