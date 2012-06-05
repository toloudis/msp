/*****************************************************************************
**  smdlSubdivNetwork.cpp
**
**	A smdlSubdivNetwork subdivides a mesh into a certain number of levels
**	and then maintains information so that it can update the mesh when the
**	base mesh's vertices are animated. 
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlSubdivNetwork.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"

#include <algorithm>


//============================================================================
//============================================================================
namespace
{
	const bool c_bDebugSubdivDetails = false;
	bool l_bUpdateNormals = true;
	const int c_SubdivLimit = 0x3FFFFFFF; // High number using almost 32-bits (used to be 65536)

	//--------------------------------------------------------------------
	// compute normal of plane from three points of triangle
	//--------------------------------------------------------------------
	maVector3d compute_normal(const maPoint3d &i_A,
		const maPoint3d &i_B,
		const maPoint3d &i_C)
	{
		maVector3d norm = (i_B - i_A) / (i_C - i_A);
		norm.Normalize();
		return norm;
	}


	//--------------------------------------------------------------------
	// Calculate vertex normals using face normal averaging
	//--------------------------------------------------------------------
	void calculate_normals(smdlSubdivUtil::sVert* i_pVList,
		int i_nVerts,
		smdlSubdivUtil::sFace* i_pTList,
		int i_nFaces)
	{
		// reset all vertex normals
		for (int i=0; i<i_nVerts; i++ )
		{
			i_pVList[i].m_Norm.Set(0,0,0);
		}

		// find all triangle normals
		for (int i=0; i<i_nFaces; i++ )
		{
			// Note: these are quads now, so we should consider all four points
			i_pTList[i].m_Normal = compute_normal(
				i_pTList[i].m_VertList[0]->m_Loc,
				i_pTList[i].m_VertList[1]->m_Loc,
				i_pTList[i].m_VertList[2]->m_Loc);

			// add the normal to each vertex
			for (int v=0; v<i_pTList[i].m_NumVerts; v++)
			{
				// By using the AddNormal() function, the vertex can pass
				// the normal to its shared vertices when it is smoothed.
				//i_pTList[i].m_VertList[v]->m_Norm += i_pTList[i].m_Normal;
				i_pTList[i].m_VertList[v]->AddNormal( i_pTList[i].m_Normal );
			}
		}

		// reset all vertex normals
		for (int i=0; i<i_nVerts; i++ )
		{
			if (!i_pVList[i].m_Norm.Normalize())
				i_pVList[i].m_Norm.Set(0,0,1);
		}
	}

	// Compare this code with that in bumpTriMeshBumpFrag.cpp. Could probably be factored together.
	void compute_basis_vectors(smdlSubdivUtil::sVert* i_pVList,
			int i_nVerts,
			smdlSubdivUtil::sFace* i_pTList,
			int i_nFaces)
	{
		// Clear the basis vectors
		int i;
		for (i = 0; i < i_nVerts; i++)
		{
			i_pVList[i].m_S = maVector3d(0.0f, 0.0f, 0.0f);
			i_pVList[i].m_T = maVector3d(0.0f, 0.0f, 0.0f);
		}

		// Walk through the triangle list and calculate gradiants for each triangle.
		// Sum the results into the S and T components.
		maVector3d S, T;
		for( i = 0; i < i_nFaces; i++ )
		{

			DBG_ASSERT(i_pTList[i].m_NumVerts >= 3, "bad subd face, too few vertices");
			if (i_pTList[i].m_NumVerts < 3)
				continue;

			// triangle fan indexing, arbitrarily chosen to cover all verts in the sFace
			for (int j = 0; j < i_pTList[i].m_NumVerts - 2; j++)
			{
				smdlSubdivUtil::sVert& v0 = *(i_pTList[i].m_VertList[0]);
				smdlSubdivUtil::sVert& v1 = *(i_pTList[i].m_VertList[j+1]);
				smdlSubdivUtil::sVert& v2 = *(i_pTList[i].m_VertList[j+2]);

				S.Set(0,0,0);
				T.Set(0,0,0);

				// x, s, t
				maVector3d edge01( v1.m_Loc.m_X - v0.m_Loc.m_X, v1.m_UV.m_X - v0.m_UV.m_X, v1.m_UV.m_Y - v0.m_UV.m_Y );
				maVector3d edge02( v2.m_Loc.m_X - v0.m_Loc.m_X, v2.m_UV.m_X - v0.m_UV.m_X, v2.m_UV.m_Y - v0.m_UV.m_Y );

				maVector3d cp = edge01 / edge02;
				if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
				{
					S.m_X = -cp.m_Y / cp.m_X;
					T.m_X = -cp.m_Z / cp.m_X;
				}

				// y, s, t
				edge01.Set( v1.m_Loc.m_Y - v0.m_Loc.m_Y, v1.m_UV.m_X - v0.m_UV.m_X, v1.m_UV.m_Y - v0.m_UV.m_Y );
				edge02.Set( v2.m_Loc.m_Y - v0.m_Loc.m_Y, v2.m_UV.m_X - v0.m_UV.m_X, v2.m_UV.m_Y - v0.m_UV.m_Y );

				cp = edge01 / edge02;
				if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
				{
					S.m_Y = -cp.m_Y / cp.m_X;
					T.m_Y = -cp.m_Z / cp.m_X;
				}


				// z, s, t
				edge01.Set( v1.m_Loc.m_Z - v0.m_Loc.m_Z, v1.m_UV.m_X - v0.m_UV.m_X, v1.m_UV.m_Y - v0.m_UV.m_Y );
				edge02.Set( v2.m_Loc.m_Z - v0.m_Loc.m_Z, v2.m_UV.m_X - v0.m_UV.m_X, v2.m_UV.m_Y - v0.m_UV.m_Y );

				cp = edge01 / edge02;
				if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
				{
					S.m_Z = -cp.m_Y / cp.m_X;
					T.m_Z = -cp.m_Z / cp.m_X;
				}

				//if (i_Indices[i] == 175 || i_Indices[i+1] == 175 || i_Indices[i+2] == 175)
				//{
				//	DBG_LOG("i_Indices[?] == 175, #" << i);
				//	DBG_LOG3("  m_S: %f %f %f", S.m_X, S.m_Y, S.m_Z);
				//	DBG_LOG3("  m_T: %f %f %f", T.m_X, T.m_Y, T.m_Z);
				//
				//	S.Normalize();
				//	T.Normalize();
				//
				//	DBG_LOG3("  N-S: %f %f %f", S.m_X, S.m_Y, S.m_Z);
				//	DBG_LOG3("  N-T: %f %f %f", T.m_X, T.m_Y, T.m_Z);
				//
				//	DBG_LOG5("  v0: %f %f %f (%f,%f)", v0.m_Loc.m_X, v0.m_Loc.m_Y, v0.m_Loc.m_Z, v0.m_UV.m_X, v0.m_UV.m_Y);
				//	DBG_LOG5("  v1: %f %f %f (%f,%f)", v1.m_Loc.m_X, v1.m_Loc.m_Y, v1.m_Loc.m_Z, v1.m_UV.m_X, v1.m_UV.m_Y);
				//	DBG_LOG5("  v2: %f %f %f (%f,%f)", v2.m_Loc.m_X, v2.m_Loc.m_Y, v2.m_Loc.m_Z, v2.m_UV.m_X, v2.m_UV.m_Y);
				//	
				//}

				S.Normalize();
				T.Normalize();

				// Now add normalized vector to actual vertex
				v0.m_S += S; v0.m_T += T;
				v1.m_S += S; v1.m_T += T;
				v2.m_S += S; v2.m_T += T;
			}
		}

		// Calculate the SxT vector
		maVector3d vecSxT;
  		for(i = 0; i < i_nVerts; i++)
  		{
  			// Normalize the S, T vectors
			i_pVList[i].m_S.Normalize();
			i_pVList[i].m_T.Normalize();

  			// Get the cross of the S and T vectors
  			//vecSxT = i_pVList[i].m_S / i_pVList[i].m_T;

  			// Get the direction of the SxT vector
  			//if (vecSxT * i_pVList[i].m_Normal < 0.0f)
  			//{
  			//	vecSxT *= -1.0f;
  			//}

			// It seems like the SxT is the same as the normal now
			//i_pVList[i].m_SxT = vecSxT;
  			// Need a normalized normal
			//i_pVList[i].m_SxT.Normalize();

			//DBG_LOG("Vertex #" << i);
			//DBG_LOG3("  Normal: %f %f %f", i_pVList[i].m_Normal.m_X, i_pVList[i].m_Normal.m_Y, i_pVList[i].m_Normal.m_Z);
			//DBG_LOG3("  m_S: %f %f %f", i_pVList[i].m_S.m_X, i_pVList[i].m_S.m_Y, i_pVList[i].m_S.m_Z);
			//DBG_LOG3("  m_T: %f %f %f", i_pVList[i].m_T.m_X, i_pVList[i].m_T.m_Y, i_pVList[i].m_T.m_Z);
			//DBG_LOG3("  m_SxT: %f %f %f", i_pVList[i].m_SxT.m_X, i_pVList[i].m_SxT.m_Y, i_pVList[i].m_SxT.m_Z);
  		} 
	}

	//--------------------------------------------------------------------
	// Create edge structure if necessary.
	//--------------------------------------------------------------------
	int add_edge(smdlSubdivUtil::sEdge* i_pEdges,
		int currEdge, 
		smdlSubdivUtil::sVert* pV0, 
		smdlSubdivUtil::sVert* pV1, 
		smdlSubdivUtil::sFace* pFace,
		bool i_bCreased)
	{
		// Find out if the current edge is already created
		smdlSubdivUtil::sEdge* edge = pV0->GetEdge( pV1 );
		if (edge)
		{
			edge->AssignOtherFace( pFace );
		}
		else
		{
			// Look for shared edge here using shared vertex linked lists,
			// Do this before adding the new edge so it doesn't match
			smdlSubdivUtil::sEdge* shared_edge = pV0->FindSharedEdge(pV1);

			// Add new edge in
			smdlSubdivUtil::sEdge *new_edge = &(i_pEdges[currEdge++]);
			new_edge->Init( pV0, pV1, pFace );
			new_edge->m_bCreased = i_bCreased;

			if (shared_edge)
			{
				//DBG_LOG2("Found shared edge %d %d", shared_edge->m_pVert[0]->m_Index, shared_edge->m_pVert[1]->m_Index);
				//DBG_LOG2(" matches %d %d", pV1->m_Index, pV0->m_Index);

				// Link these edges together as shared
				shared_edge->m_pOther = new_edge;
				new_edge->m_pOther = shared_edge;

//				new_edge->AssignSharedEdge( shared_edge );
//				shared_edge->AssignSharedEdge( new_edge );

				// call AssignOtherFace() in this case also?
				shared_edge->AssignOtherFace( pFace );
				new_edge->AssignOtherFace( shared_edge->m_pFaces[0] );
			}
		}

		return currEdge;
	}

}

bool smdlSubdivNetwork::sm_bComputeBasisVectors = true;

//--------------------------------------------------------------------
// Set flag to turn off generation of basis vectors (for faster loads)
//--------------------------------------------------------------------
//static 
void smdlSubdivNetwork::SetComputeBasisVectors(bool i_bCompute)
{
	smdlSubdivNetwork::sm_bComputeBasisVectors = i_bCompute;
}


//--------------------------------------------------------------------
// Constructor for info for a single subdivision level.
// i_nEdges can be 0 which will result in no array allocation
// for m_pEList.
//--------------------------------------------------------------------
smdlSubdivNetwork::SubdivLevel::SubdivLevel(int i_nVerts, 
											int i_nFaces, 
											int i_nEdges)
: m_nVerts(i_nVerts), m_nFaces(i_nFaces), m_nEdges(i_nEdges)
{
	m_pVList = new smdlSubdivUtil::sVert[ m_nVerts ];
	if (!m_pVList)
	{
		DBG_ERROR("Cannot allocate vertices in subdiv level.");
		throw std::bad_alloc();
	}
	
	m_pTList = new smdlSubdivUtil::sFace[ m_nFaces ];
	if (!m_pTList)
	{
		delete [] m_pVList;
		DBG_ERROR("Cannot allocate faces in subdiv level.");
		throw std::bad_alloc();
	}
#ifdef _DEBUG
	for( int f = 0; f < m_nFaces; f++ )
	{
		m_pTList[f].m_Index = f;
	}
#endif

	if (m_nEdges > 0)
	{
		m_pEList = new smdlSubdivUtil::sEdge[ m_nEdges ];
		if (!m_pEList)
		{
			delete [] m_pVList;
			delete [] m_pTList;
			DBG_ERROR("Cannot allocate edges in subdiv level.");
			throw std::bad_alloc();
		}
#ifdef _DEBUG
		for( int e = 0; e < m_nEdges; e++ )
		{
			m_pEList[e].m_Index = e;
		}
#endif
	}
	else 
		m_pEList = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlSubdivNetwork::SubdivLevel::~SubdivLevel()
{
	delete [] m_pVList;
	delete [] m_pTList;
	if (m_pEList) delete [] m_pEList;
}

//--------------------------------------------------------------------
// Constructor takes info about the mesh and subdivides so that it 
// is initially set to i_InitialSubdivLevel.
// By setting the i_MaxSubdivLevel, the network can make some
// memory optimizations knowing that it does not need to prepare
// for deeper subdivisions.
// If i_bMergeVertices is true, then the positions are compared
// within a tolerance in order to close up seams.
//--------------------------------------------------------------------
smdlSubdivNetwork::smdlSubdivNetwork(const mdlSubdivInfo& i_SubdivInfo, 
										int i_InitialSubdivLevel,
										int i_MaxSubdivLevel,
										bool i_bMergeVertices)
: m_CurSubdivLevel(0), 
	m_MaxSubdivLevel(i_MaxSubdivLevel),
	m_bHasTextureCoords(true)
{
	// Copy over some basic info from the subdiv info
	//m_SubdivFragInfo.m_Material = i_SubdivInfo.m_Material->m_pMaterial;
	m_SubdivFragInfo.m_Materials = i_SubdivInfo.m_Materials;
	m_SubdivFragInfo.m_MaterialChanges = i_SubdivInfo.m_MaterialChanges;

	// Decide if bump map is necessary
	//int num_texcoords = (i_SubdivInfo.m_UVs.size() > 0) ? 1 : 0;
	//m_SubdivFragInfo.m_Flags.m_bBumpMap = mdlFragCreate::UseBumpFrag(num_texcoords,
	//				i_SubdivInfo.m_Material->m_Info.GetShader().GetLength() > 0);
	// Other flags
	m_SubdivFragInfo.m_Flags.m_bTriangleSort = i_SubdivInfo.m_Flags.m_bTriangleSort;
	m_SubdivFragInfo.m_Flags.m_bDoubleSided = i_SubdivInfo.m_Flags.m_bDoubleSided;
	m_SubdivFragInfo.m_Flags.m_bCastsShadow = i_SubdivInfo.m_Flags.m_bCastsShadow;
	m_SubdivFragInfo.m_Flags.m_bReceivesShadow = i_SubdivInfo.m_Flags.m_bReceivesShadow;
	m_SubdivFragInfo.m_Flags.m_bShadowHull = i_SubdivInfo.m_Flags.m_bShadowHull;

	// Prepare the network
	//
	int nIndices = i_SubdivInfo.m_Indices.size();
	m_bHasTextureCoords = (!i_SubdivInfo.m_UVs.empty());

	// we don't know how many edges we will need because it
	// depends on how many are boundary edges and how many are
	// shared edges. I guess I will allocate enough space
	// for the worst case scenario.
	int edge_estimate = nIndices;  // this is over-estimate because it includes num_verts_per_poly
	SubdivLevel* pBase = new SubdivLevel(i_SubdivInfo.m_Vertices.size(), i_SubdivInfo.m_NumFaces, edge_estimate);
	if (c_bDebugSubdivDetails)
	{
		DBG_LOG("Base mesh num faces: " << pBase->m_nFaces << " num verts: " << pBase->m_nVerts);
	}
	this->m_Levels.push_back(pBase);

	// Load the vertices
	smdlSubdivUtil::sVert* pV = pBase->m_pVList;
	for (int i=0; i<pBase->m_nVerts; i++, pV++ )
	{
		pV->m_Loc = i_SubdivInfo.m_Vertices[i];
		//pV->m_Norm = i_SubdivInfo.m_Normals[i];
		pV->m_UV = (m_bHasTextureCoords) ? i_SubdivInfo.m_UVs[i] : maPoint2d(0,0);
		pV->m_Index = i;

	}

	if (i_bMergeVertices)
	{
		// n-squared algorithm, could be improved with hash table
		// or similar data structure
		pV = pBase->m_pVList;
		int ind;
		for (int v=0; v<pBase->m_nVerts; ++v)
		{
			const maPoint3d &value = pBase->m_pVList[v].m_Loc;
			pV = pBase->m_pVList;
			for (ind=0; ind<v; ++ind, pV++)
			{
				// This comparison checks for equality within an
				// epsilon value for maVectors. Different than operator==()
				if ( (value - pV->m_Loc).LengthSqr() < maConstants::c_fEpsilon)
				{
					//DBG_LOG("Sharing vertex " << v << " matches " << ind);
					pV->ConnectSharedVert(pBase->m_pVList + v);
					break;
				}
			}
		}
	}
	else
	{
		// Connect shared vertices using vertex remapping
		std::vector<smdlSubdivUtil::sVert*> shared_verts(i_SubdivInfo.m_NumOrigVertices, (smdlSubdivUtil::sVert*)NULL);
		std::multimap<int, int>::const_iterator map_it, end = i_SubdivInfo.m_VertexRemap.end();
		for (map_it = i_SubdivInfo.m_VertexRemap.begin(); map_it != end; ++map_it)
		{
			// map_it->first is original vertex index, map_it->second is new vertex index
			pV = &(pBase->m_pVList[map_it->second]);
			if (shared_verts[map_it->first])
			{
				// already existing vertex, add new vertex to its linked list
				//DBG_LOG2("Sharing verts %d and %d", shared_verts[map_it->first]->m_Index, pV->m_Index);
				shared_verts[map_it->first]->ConnectSharedVert(pV);
			}
			else
			{
				// new vertex for this point, just set it in the array
				shared_verts[map_it->first] = pV;
			}
		}
	}

	// Add material assignments to base subdiv level
	pBase->m_MaterialChanges = i_SubdivInfo.m_MaterialChanges;

	// Load the triangles (simultaneously load the edges)
	int ind = 0, currEdge = 0, face_ind = 0;
	smdlSubdivUtil::sFace* pT = pBase->m_pTList;
	for (int i=0; i<pBase->m_nFaces; i++, pT++ )
	{
		// init the Face with the full set of vertices for the polygon
		int num_verts_per_poly = i_SubdivInfo.m_Indices[ind++];	
		//DBG_LOG("num_verts_per_poly: " << num_verts_per_poly);
		std::vector<smdlSubdivUtil::sVert*> poly_verts( num_verts_per_poly );
		for (int v=0; v<num_verts_per_poly; v++)
		{
			face_ind = i_SubdivInfo.m_Indices[ind++];
			poly_verts[v] = &(pBase->m_pVList[face_ind]);
		}

		pT->Init( &(poly_verts[0]), num_verts_per_poly);

		// Add edges for triangles, either matching with existing edges
		// or creating new edges.
		for (int v=0; v<num_verts_per_poly; v++)
		{
			const bool creased = false; // creasing info for the base mesh will be assigned later
			currEdge = add_edge(pBase->m_pEList, currEdge, 
				pT->m_VertList[v], pT->m_VertList[(v+1)%num_verts_per_poly], pT, creased);
		}
	}

	// Cache some lengths for easy access later
	//m_nEdges = m_EList.size();
	if (c_bDebugSubdivDetails)
	{
		DBG_LOG("After edging: edge_estimate " << edge_estimate << " currEdge " << currEdge);
	}
	pBase->m_nEdges = currEdge;

#ifdef QUAD_PATCHES
	GenSharedLists( pBase );
#endif//QUAD_PATCHES
/*
	//uniquely add all edges and vertexes faces (no duplicates) including there shared m_pNext lists
	pV = pBase->m_pVList;
	for( int i = 0; i < pBase->m_nVerts; i++, pV++ )
	{
		smdlSubdivUtil::sVert* pCur = pV;
		do 
		{
			//add all edged of this vertex to the shared edge list
			for( int e = 0; e < pCur->m_EdgeList.size(); e++ )
			{
				pV->AddSharedEdge( pCur->m_EdgeList[e] );
			}
			//add all faces of this vertex to the shared face list
			for( int f = 0; f < pCur->m_FaceList.size(); f++ )
			{
				pV->AddSharedFace( pCur->m_FaceList[f] );
			}

			pCur = pCur->m_pNext;	//link to shared vertex
		} while( pCur && pCur != pV ); //iterate through the list (tail = start vertex)
	}
*/

	// FIX: [bga] - no support for vertex creases
	const int level = 0;
	assign_creases(pBase, i_SubdivInfo.m_EdgeCreases, level);

	// Now, subdivide it if needed
	m_EdgeCreases = i_SubdivInfo.m_EdgeCreases;
	m_MaxEdgeCreaseLevel = i_SubdivInfo.m_MaxEdgeCreaseLevel;
	Subdivide(i_InitialSubdivLevel, m_EdgeCreases, m_MaxEdgeCreaseLevel);


	// Set initial level
	this->SetCurrentSubdivLevel(i_InitialSubdivLevel);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlSubdivNetwork::~smdlSubdivNetwork()
{
	envSTLHelpers::DeleteContainer(this->m_Levels);
}

#ifdef QUAD_PATCHES
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void smdlSubdivNetwork::GenSharedLists(smdlSubdivNetwork::SubdivLevel* i_pLevel)
{
	//uniquely add all edges and vertexes faces (no duplicates) including there shared m_pNext lists
	smdlSubdivUtil::sVert* pV = i_pLevel->m_pVList;
	for( int i = 0; i < i_pLevel->m_nVerts; i++, pV++ )
	{
		smdlSubdivUtil::sVert* pCur = pV;
		do 
		{
			//add all edged of this vertex to the shared edge list
			for( int e = 0; e < pCur->m_EdgeList.size(); e++ )
			{
				pV->AddSharedEdge( pCur->m_EdgeList[e] );
			}
			//add all faces of this vertex to the shared face list
			for( int f = 0; f < pCur->m_FaceList.size(); f++ )
			{
				pV->AddSharedFace( pCur->m_FaceList[f] );
			}

			pCur = pCur->m_pNext;	//link to shared vertex
		} while( pCur && pCur != pV ); //iterate through the list (tail = start vertex)
	}
}
#endif//QUAD_PATCHES

//--------------------------------------------------------------------
// Return current subdivision level being used.
//--------------------------------------------------------------------
int smdlSubdivNetwork::GetCurrentSubdivLevel() const
{
	return this->m_CurSubdivLevel;
}

//--------------------------------------------------------------------
// Return maximum level of subdivision for which the network 
//	has been created
//--------------------------------------------------------------------
int smdlSubdivNetwork::GetMaxSubdivLevel() const
{
	return (this->m_Levels.size() - 1);
}

//--------------------------------------------------------------------
// Set the current subdivision level being used, this should be 
// a level less than or equal to the return value of 
// GetMaxSubdivLevel()
//--------------------------------------------------------------------
void smdlSubdivNetwork::SetCurrentSubdivLevel(int i_SubdivLevel)
{
	if (i_SubdivLevel != m_CurSubdivLevel)
	{
		if (i_SubdivLevel > this->GetMaxSubdivLevel())
		{
			// Subdivide the network further in order to reach the
			// desired subdivision level. This also alters 
			// m_SubdivFragInfo and m_CurSubdivLevel
			this->Subdivide(i_SubdivLevel, m_EdgeCreases, m_MaxEdgeCreaseLevel);
		}
		else
		{
			// Generate new fragment info for this level
			// Note: I guess this could be cached, depends on usage later.
			GatherFragInfo(m_Levels[i_SubdivLevel], this->m_SubdivFragInfo);
			m_CurSubdivLevel = i_SubdivLevel;
		}
	}
}


//--------------------------------------------------------------------
// Return info about the subdivided mesh in SubdivInfo format.
//	This should be called once after subdividing in order to
//	create the fragment, but then you should be able to just
//	alter the fragment's vertices when animating using the
//	AlterBaseMesh() function and GetSubdivVertices().
//--------------------------------------------------------------------
const mdlFragInfo& smdlSubdivNetwork::GetSubdivFragInfo() const
{
	return m_SubdivFragInfo;
}


//--------------------------------------------------------------------
// Return number of vertices in the subdivided model
// at the current subdivision level, used for sizing
// the array for GetSubdivVertices()
//--------------------------------------------------------------------
int smdlSubdivNetwork::GetNumFacesAtLevel(int i_SubdivLevel) const
{
	// Cap maximum
	int subdiv_level = maFunctions::Lowest(i_SubdivLevel, this->GetMaxSubdivLevel());
	SubdivLevel* pLevel = this->m_Levels[subdiv_level];
	return pLevel->m_nFaces;
}

//--------------------------------------------------------------------
// Return number of vertices in base mesh
//--------------------------------------------------------------------
int smdlSubdivNetwork::GetNumBaseMeshVertices()
{
	SubdivLevel* pBase = this->m_Levels[0];
	return pBase->m_nVerts;
}

//--------------------------------------------------------------------
// Return number of vertices in the subdivided model
//--------------------------------------------------------------------
int smdlSubdivNetwork::GetNumSubdivVertices() const
{
	return m_SubdivFragInfo.m_Vertices.size();
}

//--------------------------------------------------------------------
// Return positions of vertices from the subdivided mesh into the 
//	given array. Make sure the array is big enough to hold 
//	GetNumSubdivVertices() number of points.
//--------------------------------------------------------------------
void smdlSubdivNetwork::GetSubdivVertices(maPoint3d* o_pVertices,
										  maVector3d* o_pNormals,
										  maVector3d* o_pSs,
										  maVector3d* o_pTs,
										  int i_ArraySize) const
{
	SubdivLevel* pFinal = this->m_Levels[m_CurSubdivLevel];
	int len = pFinal->m_nVerts;
	DBG_ASSERT(len <= i_ArraySize, "Receiving array is not big enough for GetSubdivVertices()");
	if (len > i_ArraySize)
		return;
	for (int i=0; i<len; i++)
	{
		o_pVertices[i] = pFinal->m_pVList[i].m_Loc;
		o_pNormals[i] = pFinal->m_pVList[i].m_Norm;
		o_pSs[i] = pFinal->m_pVList[i].m_S;
		o_pTs[i] = pFinal->m_pVList[i].m_T;
	}
}

//--------------------------------------------------------------------
// Return direct access to the vertex list in the current
//	subdivision level.
//--------------------------------------------------------------------
const smdlSubdivUtil::sVert* smdlSubdivNetwork::GetSubdivVertices() const
{
	SubdivLevel* pFinal = this->m_Levels[m_CurSubdivLevel];
	return pFinal->m_pVList;
}

//--------------------------------------------------------------------
// Given the new array of mesh vertices, compute the new positions
//	of the subdivided mesh. The number of vertices passed in
//	should match the number of vertices in the original mesh.
//--------------------------------------------------------------------
void smdlSubdivNetwork::AlterBaseMesh(const maPoint3d* i_pVertices,
									  int i_NumVertices)
{

	// Put these new vertex locations into the base mesh
	SubdivLevel* pBase = this->m_Levels[0];
	DBG_ASSERT(pBase->m_nVerts == i_NumVertices, "AlterBaseMesh, number of vertices does not match.");
	if (pBase->m_nVerts != i_NumVertices)
		return;
	for (int i=0; i<i_NumVertices; i++ )
	{
		pBase->m_pVList[i].m_Loc = i_pVertices[i];
	}

	// Then push the changes through to the subdivided levels
	for (int l=0; l<m_CurSubdivLevel; l++)
	{
		Update(this->m_Levels[l], this->m_Levels[l+1]);
	}

	if (l_bUpdateNormals)
	{
		SubdivLevel* pFinal = this->m_Levels[m_CurSubdivLevel];
		calculate_normals(pFinal->m_pVList, pFinal->m_nVerts, 
			pFinal->m_pTList, pFinal->m_nFaces);

		if (sm_bComputeBasisVectors)
		{
			compute_basis_vectors(pFinal->m_pVList, pFinal->m_nVerts, 
				pFinal->m_pTList, pFinal->m_nFaces);
		}
	}
	
}

//--------------------------------------------------------------------
// Subdiv networks can be shared, but only one can be actively
// updating positions at a time. So, you need to wrap
// calls to "AlterBaseMesh" and "GetSubdivVertices" with
// a envScopedLock using this envMutex.
//--------------------------------------------------------------------
envMutex& smdlSubdivNetwork::GetMutex()
{
	return m_Mutex;
}

//--------------------------------------------------------------------
// Generate new vertex locations for faces and edges 
//--------------------------------------------------------------------
void smdlSubdivNetwork::GenNewVertLocs(SubdivLevel* i_pLevel)
{
//	DBG_LOG("GenNewVertLocs: Num faces " << i_pLevel->m_nFaces);
	smdlSubdivUtil::sFace* pT = i_pLevel->m_pTList;
	for (int i=0; i<i_pLevel->m_nFaces; i++, pT++ )
	{
		pT->ComputeAverage( i + i_pLevel->m_nVerts );
	}

//	DBG_LOG("GenNewVertLocs: Num edges " << i_pLevel->m_nEdges);
	int vert_offset = i_pLevel->m_nVerts + i_pLevel->m_nFaces;
	smdlSubdivUtil::sEdge* pE = i_pLevel->m_pEList;
	for (int i=0; i<i_pLevel->m_nEdges; i++, pE++ )
	{
		pE->ComputeMidpoint();

		// Assign the new vertex an index (this is useful later,
		// when we start throwing vertex pointers around.)
		pE->GenerateNewVertexLocation( i + vert_offset );
	}
}

//--------------------------------------------------------------------
// Update vertices in an existing subdivision network based
// on new vertex locations in the base mesh.
//--------------------------------------------------------------------
void smdlSubdivNetwork::Update(SubdivLevel* i_pBase, SubdivLevel* i_pLevel)
{
	// Find the location of the new vertices for faces and edges
	// in the base mesh. 
	GenNewVertLocs(i_pBase);

	smdlSubdivUtil::sVert* pNV = i_pLevel->m_pVList;
	// First batch - the original vertices
	for(int i=0; i<i_pBase->m_nVerts; i++, pNV++ )
	{
		// Note: the original vertices are moved in the subdivision process
		i_pBase->m_pVList[i].ComputeNewVertexLocation(*pNV);
	}
	// Second batch - center vertices from each face
	for(int i=0; i<i_pBase->m_nFaces; i++, pNV++ )
	{
		pNV->m_Loc = i_pBase->m_pTList[i].m_Average.m_Loc;
	}
	// Third batch - midpoint vertices from each edge
	for(int i=0; i<i_pBase->m_nEdges; i++, pNV++ )
	{
		pNV->m_Loc = i_pBase->m_pEList[i].m_NewVert.m_Loc;
	}
	
}

//--------------------------------------------------------------------
// Subdivide given number of times and generate mesh from the 
// highest level. Keep around the network for updating.
//--------------------------------------------------------------------
void smdlSubdivNetwork::Subdivide(int i_SubdivLevel,
								const std::vector<mayCreaseInfo> &i_EdgeCreases, 
								int i_MaxEdgeCreaseLevel)
{
	// i_MaxEdgeCreaseLevel signifies that crease info is stored
	// in the i_EdgeCrease array up to this level. After this
	// level, the crease info is inherited from parent to child.
	//
	// FIX: [bga] - can't handle a crease level higher than 1 for now
	int max_crease_level = maFunctions::Lowest(i_MaxEdgeCreaseLevel, 1);

	// Add in extra subdivision levels if needed
	int current_num_levels = m_Levels.size();
	for (int l=current_num_levels; l<=i_SubdivLevel; l++)
	{
		// Create edge info except for last level.
		// This is a memory optimization based on the 
		// MaxSubdivLevel set in the constructor.
		const bool create_edges = (l < m_MaxSubdivLevel);

		// after the max crease level, we keep the same creases
		// by promoting the crease info from one level to the next
		const bool promote_creases = (l > max_crease_level);
		SubdivLevel* pLevel = Subdivide(m_Levels[l-1], create_edges, promote_creases);
		if (pLevel) // can return NULL if too many vertices
		{
			m_Levels.push_back(pLevel);

			// If we are in one of the early levels, the creasing info
			// is set explicitly through the mayCreaseInfo array
			if (create_edges && (l <= max_crease_level))
			{
				assign_creases(pLevel, i_EdgeCreases, l);			
			}
		}
	}

	m_CurSubdivLevel = maFunctions::Lowest(i_SubdivLevel, (int)(m_Levels.size())-1);
	GatherFragInfo(m_Levels[m_CurSubdivLevel], this->m_SubdivFragInfo);
}

//--------------------------------------------------------------------
// Subdivides the specific passed-in level one level more detailed.
// Edge information isn't always needed, so i_bCreateEdges determines
//	when to create it.
// i_bPromoteCreases defines when to assign creasing information
//	from parent level to child level
//--------------------------------------------------------------------
smdlSubdivNetwork::SubdivLevel* smdlSubdivNetwork::Subdivide(SubdivLevel* i_pBase, 
						bool i_bCreateEdges, 
						bool i_bPromoteCreases)
{
	// We know how many components our subdivided model will have, calc them
	int nNewVerts = i_pBase->m_nVerts + i_pBase->m_nEdges + i_pBase->m_nFaces;
	int nNewFaces = count_new_faces(i_pBase); // 4*m_nFaces;
	int nNewEdges = 2*i_pBase->m_nEdges + nNewFaces;
	if (c_bDebugSubdivDetails)
	{
		DBG_LOG("Subdivision num tris: " << nNewFaces << " num verts: " << nNewVerts << " num edges: " << nNewEdges);
	}

	// Because of the optimization below, we might have created a level
	// without edges knowing that we couldn't subdivide again. If we notice
	// that the number of edges in the previous level is 0, then we
	// can't subdivide (the edge info is needed to subdivide).
	if (i_pBase->m_nEdges == 0)
	{
		if (c_bDebugSubdivDetails)
		{
			DBG_LOG("Can't subdivide to this level, no edges in previous level");
		}
		return NULL;
	}

	// If the model will have too many triangles (d3d can only handle 2^16), return
	//if( nNewVerts >= 65536 || nNewFaces >= 65536)
	// Now allowing 32-bit indices for subdivision meshes
	if( nNewVerts >= c_SubdivLimit || nNewFaces >= c_SubdivLimit)
	{
		DBG_LOG("Can't subdivide to this level, too many vertices produced, " << nNewVerts << " " << nNewFaces);
		return NULL;
	}

	// Find the location of the new vertices for faces and edges
	GenNewVertLocs(i_pBase);

	// Memory Optimization:
	// If the next subdivision level would be impossible because of too many
	// vertices, then we don't need to generate edge data
	//if (nNewVerts + nNewFaces + nNewEdges > 65536)
	if (nNewVerts + nNewFaces + nNewEdges > c_SubdivLimit)
	{
		i_bCreateEdges = false;
	}

	// Allocate space for the subdivided data
	if (!i_bCreateEdges) nNewEdges = 0;	// no edges generated in leaf
	SubdivLevel* pLevel = new SubdivLevel(nNewVerts, nNewFaces, nNewEdges); 

	int i;
	//==========--------------------------  Step 1: Fill the vertex list
	//DBG_LOG("Step 1: Fill the vertex list");
	smdlSubdivUtil::sVert* pNV = pLevel->m_pVList;
	// First batch - the original vertices
	for( i=0; i<i_pBase->m_nVerts; i++, pNV++ )
	{
		// Note: the original vertices are moved in the subdivision process
		smdlSubdivUtil::sVert *pOrig = &(i_pBase->m_pVList[i]);
		pOrig->ComputeNewVertexLocation(*pNV);
		pNV->m_Index = i;

		// Connect the shared vertices in the new subdivision level
		// The indices of existing vertices stay the same. So, just 
		// have to connect the new vertices in the say way.
		smdlSubdivUtil::sVert *pCur = pOrig->m_pNext; 
		while (pCur)
		{
			int ind2 = pCur->m_Index;
			if (pNV->m_Index > ind2) // only connect them once
			{
				pLevel->m_pVList[ind2].ConnectSharedVert( pNV );
			}
			
			pCur = pCur->m_pNext;
			if (pCur == pOrig) break; // looped around
		}
	}
	// Second batch - center vertices from each face
	for( i=0; i<i_pBase->m_nFaces; i++, pNV++ )
	{
		pNV->m_Index = i_pBase->m_nVerts + i;
		pNV->m_Loc = i_pBase->m_pTList[i].m_Average.m_Loc;
		pNV->m_UV = i_pBase->m_pTList[i].m_Average.m_UV;
		pNV->m_bSmoothed = i_pBase->m_pTList[i].m_Average.m_bSmoothed; // always true?
	}
	// Third batch - midpoint vertices from each edge
	int vert_offset = i_pBase->m_nVerts + i_pBase->m_nFaces;
	//DBG_LOG("vert_offset: " << vert_offset);
	for( i=0; i<i_pBase->m_nEdges; i++, pNV++ )
	{
		pNV->m_Index = vert_offset + i;
		pNV->m_Loc = i_pBase->m_pEList[i].m_NewVert.m_Loc;
		pNV->m_UV = i_pBase->m_pEList[i].m_NewVert.m_UV;
		pNV->m_bSmoothed = i_pBase->m_pEList[i].m_NewVert.m_bSmoothed;

		// Connect the shared vertices in the new subdivision level.
		// When a split edge is subdivided, each edge generates a midpoint
		// in the same place. These new vertices need to be marked as shared.
		if (i_pBase->m_pEList[i].m_pOther)
		{
			int ind2 = i_pBase->m_pEList[i].m_pOther->m_NewVert.m_Index;
			if (pNV->m_Index > ind2)	// only connect them once
			{
				pLevel->m_pVList[ind2].ConnectSharedVert( pNV );
			}
		}

	}

	// Prepare material index for next level
	int material_index = 0, mat_change = -1;
	pLevel->m_MaterialChanges = i_pBase->m_MaterialChanges; // set size, values wil be changed below
	if (!i_pBase->m_MaterialChanges.empty())
		mat_change = i_pBase->m_MaterialChanges[0];

	//==========--------------------------  Step 2: Fill in the face list
	//DBG_LOG("Step 2: Fill in the new face list");
	int currEdge = 0;
	int currTri = 0, nVerts, next_vert, prev_vert;
	smdlSubdivUtil::sEdge *edge1 = NULL, *edge2 = NULL;
	smdlSubdivUtil::sFace *pFace = NULL, *pNewFace = NULL;
	for( i=0; i<i_pBase->m_nFaces; i++ )
	{
		pFace = &(i_pBase->m_pTList[i]);
		
		// See if this face index starts a new material group
		if ((mat_change >= 0) && (i >= mat_change))
		{
			pLevel->m_MaterialChanges[material_index] = currTri;
			material_index++;
			if (i_pBase->m_MaterialChanges.size() > material_index)
				mat_change = i_pBase->m_MaterialChanges[material_index];
			else 
				mat_change = -1; // no more material groups
		}

		nVerts = pFace->m_NumVerts;
		for (int v=0; v<nVerts; v++)
		{
			next_vert = (v+1) % nVerts;
			edge1 = pFace->m_VertList[v]->GetEdge( pFace->m_VertList[next_vert] );
			prev_vert = (v-1+nVerts) % nVerts;
			edge2 = pFace->m_VertList[v]->GetEdge( pFace->m_VertList[prev_vert] );

			//DBG_LOG4("New face %d %d %d %d", pFace->m_VertList[v]->m_Index, edge1->m_NewVert.m_Index,
			//	pFace->m_Average.m_Index, edge2->m_NewVert.m_Index);
			pNewFace = &(pLevel->m_pTList[currTri++]);
			pNewFace->Init( 
				&pLevel->m_pVList[pFace->m_VertList[v]->m_Index],
				&pLevel->m_pVList[edge1->m_NewVert.m_Index], 
				&pLevel->m_pVList[pFace->m_Average.m_Index],
				&pLevel->m_pVList[edge2->m_NewVert.m_Index] ); 

			if (i_bCreateEdges)
			{
				// Submit edges for this new face
				bool creased = false;

				if (i_bPromoteCreases) creased = edge1->m_bCreased;
				currEdge = add_edge(pLevel->m_pEList, currEdge, 
					pNewFace->m_VertList[0], pNewFace->m_VertList[1], pNewFace, creased);

				if (i_bPromoteCreases) creased = false;	// internal edge
				currEdge = add_edge(pLevel->m_pEList, currEdge, 
					pNewFace->m_VertList[1], pNewFace->m_VertList[2], pNewFace, creased);

				if (i_bPromoteCreases) creased = false;	// internal edge
				currEdge = add_edge(pLevel->m_pEList, currEdge, 
					pNewFace->m_VertList[2], pNewFace->m_VertList[3], pNewFace, creased);

				if (i_bPromoteCreases) creased = edge2->m_bCreased;
				currEdge = add_edge(pLevel->m_pEList, currEdge, 
					pNewFace->m_VertList[3], pNewFace->m_VertList[0], pNewFace, creased);
			}
		}
	}
#ifdef QUAD_PATCHES
	GenSharedLists( pLevel );
#endif//QUAD_PATCHES
	return pLevel;
}


//--------------------------------------------------------------------
// Generate fragment info from a subdivision level.
//--------------------------------------------------------------------
void smdlSubdivNetwork::GatherFragInfo(SubdivLevel* i_pLevel,
									   mdlFragInfo& o_SubdivFragInfo)
{
	// Calculate the vertex normals of the new mesh using face normal averaging
	if (c_bDebugSubdivDetails)
	{
		DBG_LOG("Calculating the vertex normals");
	}
	calculate_normals(i_pLevel->m_pVList, i_pLevel->m_nVerts, 
		i_pLevel->m_pTList, i_pLevel->m_nFaces);
	if (sm_bComputeBasisVectors)
	{
		compute_basis_vectors(i_pLevel->m_pVList, i_pLevel->m_nVerts, 
			i_pLevel->m_pTList, i_pLevel->m_nFaces);
	}

	if (c_bDebugSubdivDetails) 
	{
		DBG_LOG("Generate new frag info");
	}
	int nNewVerts = i_pLevel->m_nVerts;
	o_SubdivFragInfo.m_Vertices.resize(nNewVerts);
	o_SubdivFragInfo.m_Normals.resize(nNewVerts);
	if (m_bHasTextureCoords)
		o_SubdivFragInfo.m_UVs.resize(nNewVerts);
	else
		o_SubdivFragInfo.m_UVs.clear();

	smdlSubdivUtil::sVert* pV = i_pLevel->m_pVList;
	for (int i=0; i<nNewVerts; i++, pV++)
	{
		o_SubdivFragInfo.m_Vertices[i] = pV->m_Loc;
		o_SubdivFragInfo.m_Normals[i] = pV->m_Norm;
		if (m_bHasTextureCoords)
			o_SubdivFragInfo.m_UVs[i] = pV->m_UV;
	}


	if (i_pLevel != m_Levels[0])
	{
		// With Catmull-Clark subdivision, we know that all new faces are quads,
		// So each face will generate 2 triangles and require 3 indices.
		int nNewFaces = i_pLevel->m_nFaces;
		o_SubdivFragInfo.m_Indices.resize(nNewFaces*6);
		int ind = 0;
		smdlSubdivUtil::sFace* pT = i_pLevel->m_pTList;
		for (int f=0; f<nNewFaces; f++, pT++)
		{
			// triangle fan
			o_SubdivFragInfo.m_Indices[ind++] = pT->m_VertList[0]->m_Index;
			o_SubdivFragInfo.m_Indices[ind++] = pT->m_VertList[1]->m_Index;
			o_SubdivFragInfo.m_Indices[ind++] = pT->m_VertList[2]->m_Index;

			o_SubdivFragInfo.m_Indices[ind++] = pT->m_VertList[0]->m_Index;
			o_SubdivFragInfo.m_Indices[ind++] = pT->m_VertList[2]->m_Index;
			o_SubdivFragInfo.m_Indices[ind++] = pT->m_VertList[3]->m_Index;
		}
		//o_SubdivFragInfo.m_WeldIndex = ind;

		// quads converted to triangles just means material changes are multiplied by 2
		o_SubdivFragInfo.m_MaterialChanges.clear();
		for (int mi=0;mi<i_pLevel->m_MaterialChanges.size(); mi++)
		{
			o_SubdivFragInfo.m_MaterialChanges.push_back( 2 * i_pLevel->m_MaterialChanges[mi] );
		}
	}
	else
	{
		// Bring over material assignments
		int material_index = 0, mat_change = -1;
		o_SubdivFragInfo.m_MaterialChanges = i_pLevel->m_MaterialChanges; // set size, values wil be changed below
		if (!i_pLevel->m_MaterialChanges.empty())
			mat_change = i_pLevel->m_MaterialChanges[0];

		// With the base mesh, we don't know anything about the faces.
		// Have to count up the number of triangles we will need.
		int tri_count = 0;
		smdlSubdivUtil::sFace* pT = i_pLevel->m_pTList;
		int nNewFaces = i_pLevel->m_nFaces;
		for (int f=0; f<nNewFaces; f++, pT++)
		{
			// See if this face index starts a new material group
			if ((mat_change >= 0) && (f >= mat_change))
			{
				o_SubdivFragInfo.m_MaterialChanges[material_index] = tri_count;
				material_index++;
				if (i_pLevel->m_MaterialChanges.size() > material_index)
					mat_change = i_pLevel->m_MaterialChanges[material_index];
				else 
					mat_change = -1; // no more material groups
			}

			// Sum the triangle counts
			tri_count += (pT->m_NumVerts-2);
		}

		o_SubdivFragInfo.m_Indices.resize(tri_count*3);
		int ind = 0;
		pT = i_pLevel->m_pTList;
		for (int f=0; f<nNewFaces; f++, pT++)
		{
			int nv = pT->m_NumVerts;
			for (int v=2; v<nv; v++)
			{
				// triangle fan
				o_SubdivFragInfo.m_Indices[ind++] = pT->m_VertList[0]->m_Index;
				o_SubdivFragInfo.m_Indices[ind++] = pT->m_VertList[v-1]->m_Index;
				o_SubdivFragInfo.m_Indices[ind++] = pT->m_VertList[v]->m_Index;

			}
		}
		//o_SubdivFragInfo.m_WeldIndex = ind;
	}
}


//--------------------------------------------------------------------
// Each face generates as many new faces as it has vertices.
// This functions counts up the number of faces that will be
// generated by the mesh.
//--------------------------------------------------------------------
int smdlSubdivNetwork::count_new_faces(SubdivLevel* i_pBase)
{
	int total = 0;
	smdlSubdivUtil::sFace* pT = i_pBase->m_pTList;
	for (int i=0; i<i_pBase->m_nFaces; i++, pT++ )
		total += pT->m_NumVerts;
	return total;
}


//--------------------------------------------------------------------
// mark edges as creased 
//--------------------------------------------------------------------
void smdlSubdivNetwork::assign_creases(smdlSubdivNetwork::SubdivLevel *i_pLevel, 
										const std::vector<mayCreaseInfo> &i_EdgeCreases, 
										int i_LevelIndex)
{
	const int num_creases = i_EdgeCreases.size();
	for (int i=0; i<num_creases; i++)
	{
		const mayCreaseInfo &crease = i_EdgeCreases[i];
		if (crease.m_Level == i_LevelIndex)
		{
			DBG_ASSERT(i_LevelIndex<=1, "Only implementing base mesh and first level creasing now");
			if (i_LevelIndex > 1)
				continue;

			// FIX: [bga] - this assumes level 0 or 1
			smdlSubdivUtil::sFace *pFace = NULL;
			if (i_LevelIndex == 0)
			{
				pFace = &(i_pLevel->m_pTList[crease.m_Base]);
			}
			else if (i_LevelIndex == 1)
			{
				// Need to compute the index of the face. This depends on the number 
				// of vertices in the faces in the base mesh
				int face_ind = 0;
				smdlSubdivUtil::sFace* pT = m_Levels[0]->m_pTList; // Note: Base Mesh, not i_pLevel
				for (int i=0; i<crease.m_Base; i++, pT++ )
				{
					face_ind += pT->m_NumVerts;
				}
				// Now skip ahead to the chosen face
				face_ind += crease.m_First;
				pFace = &(i_pLevel->m_pTList[face_ind]);
			}

			if (pFace)
			{
				smdlSubdivUtil::sVert *pV1 = pFace->m_VertList[crease.m_Corner];
				smdlSubdivUtil::sVert *pV2 = pFace->m_VertList[(crease.m_Corner+1) % pFace->m_NumVerts];
				smdlSubdivUtil::sEdge *pEdge = pV1->GetEdge(pV2);
				if (pEdge)
				{
					pEdge->m_bCreased = true;
				}
				else
				{
					DBG_LOG("Could not find edge for face " << crease.m_Base << " corner " << crease.m_Corner);
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void smdlSubdivNetwork::ProjectPoints( maPoint3d* Cp, maPoint3d* C, int N )
{
	double* iV = m_Eigenvals[N].iV;
	int nCount = 2*N+8;

	for( int i = 0; i < nCount; i++ )
	{
		Cp[i] = maPoint3d();	//zero out
		for( int j = 0; j < nCount; j++ )
		{
			Cp[i] += *iV++ * C[j];
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
double Log2( double a )
{
	return log(a) / log(2.0);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void smdlSubdivNetwork::EvalSurf( maPoint3d P, double u, double v, maPoint3d* Cp, int N )
{
	// determine in which domain (omega) the parameter lies

	double n = floor( maFunctions::Lowest( -Log2(u),-Log2(v)) );

	double pow2 = pow(2,n-1);
	u *= pow2; v *= pow2;
	int k = 0;
	if( v < 0.5 )
	{
		k = 0;
		u=2*u-1;
		v=2*v;
	}
	else if( u < 0.5 )
	{
		k = 2;
		u=2*u;
		v=2*v-1;
	}
	else
	{
		k = 1;
		u=2*u-1;
		v=2*v-1;
	}
	// Now evaluate the surface
	P = maPoint3d();
	double* L = m_Eigenvals[N].L;
	double* X = m_Eigenvals[N].x[k];
	for( int i = 0; i < 2*N+8; i++, L++, X += 16 )
	{
		P += pow( *L, n-1 ) * EvalSpline( X, u, v) * Cp[i];
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BernsteinPoly( double* Vec, double x )
{
	double ix = 1-x;
	Vec[0] = ix*ix*ix;
	Vec[1] = 3*x*ix*ix;
	Vec[2] = 3*x*x*ix;
	Vec[3] = x*x*x;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BernsteinPolyD( double* Vec, double x )
{
	double ix = 1-x;
	Vec[0] = -3*x*x + 6*x -3;
	Vec[1] = 9*x*x - 12*x + 3;
	Vec[2] = -9*x*x + 6*x;
	Vec[3] = 3*x*x;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
double smdlSubdivNetwork::EvalSpline( double* Val, double u, double v )
{
	double poly = 0;
	double UB[4];
	double VB[4];

	BernsteinPoly( UB, u );
	BernsteinPoly( VB, v );

	poly += VB[0] * (Val[0]  * UB[0] + Val[1]  * UB[1] + Val[2]  * UB[2] + Val[3]  * UB[3]);
	poly += VB[1] * (Val[4]  * UB[0] + Val[5]  * UB[1] + Val[6]  * UB[2] + Val[7]  * UB[3]);
	poly += VB[2] * (Val[8]  * UB[0] + Val[9]  * UB[1] + Val[10] * UB[2] + Val[11] * UB[3]);
	poly += VB[3] * (Val[12] * UB[0] + Val[13] * UB[1] + Val[14] * UB[2] + Val[15] * UB[3]);
	return poly;
}
