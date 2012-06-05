/*****************************************************************************
**	smdlPNTessellator.cpp
**
**	A smdlPNTessellator subdivides a mesh into a certain number of levels
**	and then maintains information so that it can update the mesh when the
**	base mesh's vertices are animated. 
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlPNTessellator.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/smdl/private/smdlSubdivNetwork.hpp"


//============================================================================
//============================================================================
namespace
{
	bool l_bUpdateNormals = true;
//	bool l_bFlatTessellate = false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool smdlPNTessellator::sm_bComputeBasisVectors = true;


//--------------------------------------------------------------------
// Set flag to turn off generation of basis vectors (for faster loads)
//--------------------------------------------------------------------
//static 
void smdlPNTessellator::SetComputeBasisVectors(bool i_bCompute)
{
	smdlPNTessellator::sm_bComputeBasisVectors = i_bCompute;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
smdlPNTessellator::smdlPNTessellator( shared_ptr<smdlSubdivNetwork> i_pSubdivNetwork )
{
	m_bDisplayControlMesh = false;

	i_pSubdivNetwork->SetCurrentSubdivLevel(1);		//subdivide for smoothed vertices

	m_SubdivFragInfo = i_pSubdivNetwork->m_SubdivFragInfo;
	m_CurSubdivLevel = 0;
	m_bHasTextureCoords = i_pSubdivNetwork->m_bHasTextureCoords;
	m_MaxSubdivLevel = i_pSubdivNetwork->m_MaxSubdivLevel;

	//initialize vertex lists
	int count = i_pSubdivNetwork->m_Levels[0]->m_nVerts;
	smdlSubdivUtil::sVert* pVerts = i_pSubdivNetwork->m_Levels[1]->m_pVList;
	m_VList.reserve( count );
	for ( int i = 0; i < count; i++, pVerts++ )
	{
		tVert vert;
		vert.Position = pVerts->m_Loc;
		vert.Normal = pVerts->m_Norm;
		vert.UV = pVerts->m_UV;
		vert.Tangent = pVerts->m_S;
		vert.Binormal = pVerts->m_T;
		m_VList.push_back( vert );
	}

	//create N Patches from base mesh
	int patch_count = 0;
	smdlSubdivUtil::sFace* pT = i_pSubdivNetwork->m_Levels[0]->m_pTList;
	int nNewFaces = i_pSubdivNetwork->m_Levels[0]->m_nFaces;
	for (int f=0; f<nNewFaces; f++, pT++)
	{
		patch_count += (pT->m_NumVerts-2);
	}

	m_TList.reserve( patch_count );
	m_PatchList.reserve( patch_count );

	int ind = 0;
	pT = i_pSubdivNetwork->m_Levels[0]->m_pTList;
	for (int f=0; f<nNewFaces; f++, pT++)
	{
		int nv = pT->m_NumVerts;
		for (int v=2; v<nv; v++)
		{
			tTri tri;
			tri.m_pVert[0] = &m_VList[ pT->m_VertList[0]->m_Index ];
			tri.m_pVert[1] = &m_VList[ pT->m_VertList[v-1]->m_Index ];
			tri.m_pVert[2] = &m_VList[ pT->m_VertList[v]->m_Index ];
			m_TList.push_back( tri );

			nPatch patch;
			patch.a = *tri.m_pVert[0];
			patch.b = *tri.m_pVert[1];
			patch.c = *tri.m_pVert[2];
			if ( m_bDisplayControlMesh )
			{
				patch.CalculateControlPoints();
			}
			m_PatchList.push_back( patch );
		}
	}

	i_pSubdivNetwork->SetCurrentSubdivLevel(0);

	Tessellate();

	GatherFragInfo();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
smdlPNTessellator::~smdlPNTessellator()
{
}

//--------------------------------------------------------------------
// Return current subdivision level being used.
//--------------------------------------------------------------------
int smdlPNTessellator::GetCurrentSubdivLevel() const
{
	return m_CurSubdivLevel;
}

//--------------------------------------------------------------------
// Return maximum level of subdivision for which the network 
//	has been created
//--------------------------------------------------------------------
int smdlPNTessellator::GetMaxSubdivLevel() const
{
	return m_MaxSubdivLevel;
}

//--------------------------------------------------------------------
// Set the current subdivision level being used, this should be 
// a level less than or equal to the return value of 
// GetMaxSubdivLevel()
//--------------------------------------------------------------------
void smdlPNTessellator::SetCurrentSubdivLevel(int i_SubdivLevel)
{
	if (i_SubdivLevel != m_CurSubdivLevel)
	{
		m_CurSubdivLevel = i_SubdivLevel;
		Tessellate();
		GatherFragInfo();
	}
}


//--------------------------------------------------------------------
// Return info about the subdivided mesh in SubdivInfo format.
//	This should be called once after subdividing in order to
//	create the fragment, but then you should be able to just
//	alter the fragment's vertices when animating using the
//	AlterBaseMesh() function and GetSubdivVertices().
//--------------------------------------------------------------------
const mdlSplitFragInfo& smdlPNTessellator::GetSubdivFragInfo() const
{
	return m_SubdivFragInfo;
}


//--------------------------------------------------------------------
// Return number of vertices in the subdivided model
// at the current subdivision level, used for sizing
// the array for GetSubdivVertices()
//--------------------------------------------------------------------
int smdlPNTessellator::GetNumFacesAtLevel(int i_SubdivLevel) const
{
	return GetTessTriangles( i_SubdivLevel, m_PatchList.size() );
}

int smdlPNTessellator::GetTessTriangles(int level, int tris ) const
{
	if ( level <= 0 ) return tris;
	if ( level == 1 ) return tris * 6;
	else return tris*level*24;
}

int smdlPNTessellator::GetTessVertices(int level, int tris ) const
{
	int newtris = GetTessTriangles( level, tris );

	if ( level <= 0 ) return tris*3;
	if ( level == 1 ) return tris*7;
	else return tris*7*level;
}

//--------------------------------------------------------------------
// Return number of vertices in base mesh
//--------------------------------------------------------------------
int smdlPNTessellator::GetNumBaseMeshVertices()
{
	return m_VList.size();
}

//--------------------------------------------------------------------
// Return number of vertices in the subdivided model
//--------------------------------------------------------------------
int smdlPNTessellator::GetNumTessellatedVertices() const
{
	return m_VList_Tess.size();
}

//--------------------------------------------------------------------
// Return direct access to the vertex list in the current
//	subdivision level.
//--------------------------------------------------------------------
const std::vector<tVert>& smdlPNTessellator::GetTessellatedVertices() const
{
	return m_VList_Tess;
}

//--------------------------------------------------------------------
// Given the new array of mesh vertices, compute the new positions
//	of the subdivided mesh. The number of vertices passed in
//	should match the number of vertices in the original mesh.
//--------------------------------------------------------------------
void smdlPNTessellator::AlterBaseMesh(const std::vector<maPoint3d>& i_Vertices,
									  const std::vector<maVector3d>& i_Normals )
{
	// Put these new vertex locations into the base mesh
	bool hasNormals = !i_Normals.empty();

	DBG_ASSERT( i_Vertices.size() == m_VList.size(), "AlterBaseMesh, number of vertices don't match." );
	for ( int i = 0; i < i_Vertices.size(); i++ )
	{
		m_VList[ i ].Position = i_Vertices[i];
		if ( hasNormals ) m_VList[ i ].Normal = i_Normals[i];
	}

	if (!hasNormals && l_bUpdateNormals)
	{
		AverageNormals();

		if (sm_bComputeBasisVectors)
		{
			ComputeBasisVectors();
		}
	}

	//update patches
	DBG_ASSERT( m_PatchList.size()== m_TList.size(), "AlterBaseMesh, number of triangles don't match." );
	for ( int t = 0; t < m_TList.size(); t++ )
	{
		m_PatchList[ t ].FillFromTri( m_TList[ t ], 3 );
	}

	Tessellate();
}

/*
//--------------------------------------------------------------------
// Subdiv networks can be shared, but only one can be actively
// updating positions at a time. So, you need to wrap
// calls to "AlterBaseMesh" and "GetSubdivVertices" with
// a envScopedLock using this envMutex.
//--------------------------------------------------------------------
envMutex& smdlPNTessellator::GetMutex()
{
	return m_Mutex;
}
*/

int smdlPNTessellator::AddTessellatedVertex( const nPatch& patch, const float3& barycenter )
{
	m_VList_Tess.push_back( patch.ComputeTessVertex( barycenter ));
	return m_VList_Tess.size()-1;;
}

int smdlPNTessellator::OriginalVerts( const nPatch& patch )
{
	int ID = m_VList_Tess.size();

	AddTessellatedVertex( patch, maVector3d( 1, 0, 0 ));
	AddTessellatedVertex( patch, maVector3d( 0, 1, 0 ));
	AddTessellatedVertex( patch, maVector3d( 0, 0, 1 ));

	return ID;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int smdlPNTessellator::PreTessellate( const nPatch& patch )
{
	int ID = m_VList_Tess.size();

	//Corner verts
	AddTessellatedVertex( patch, maVector3d( 1, 0, 0 ));
	AddTessellatedVertex( patch, maVector3d( 0, 1, 0 ));
	AddTessellatedVertex( patch, maVector3d( 0, 0, 1 ));

	//Calculate Center
	AddTessellatedVertex( patch, maVector3d( 1/3.0f, 1/3.0f, 1/3.0f ));

	//Calculate Edge Center
	AddTessellatedVertex( patch, maVector3d( 0.5f, 0.5f, 0 ));
	AddTessellatedVertex( patch, maVector3d( 0, 0.5f, 0.5f ));
	AddTessellatedVertex( patch, maVector3d( 0.5f, 0, 0.5f ));

	return ID;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int smdlPNTessellator::ComputeSubVertices( const nPatch& patch, float tessellate  )
{
	int ID = m_VList_Tess.size();

	if ( m_bDisplayControlMesh )
	{
		const std::vector<float3>& CP = patch.GetControlPoints();
		tVert V;

		for ( int i = 0; i < CP.size(); i++ )
		{
			V = patch.a;
			V.Position = CP[ i ];
			m_VList_Tess.push_back( V );
		}
	}
	else
	{
		float d = 1.0f / tessellate;

		float w = 0;
		int ycount = ceil(tessellate);
		for ( int y = 0; y <= ycount; y++ )
		{
			float u = 0;
			for ( int x = 0; x <= y; x++ )
			{
				AddTessellatedVertex( patch, maVector3d( u, w-u, 1-w ));
				u += d;
				if ( u >= w ) u = w;
			}
			w += d;
			if ( w >= 1 ) w = 1;
		}
	}

	return ID;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void smdlPNTessellator::CreatePatchIndices( int StartIndex, float tessellate )
{
	int EvenIndex = StartIndex;
	int OddIndex = StartIndex+1;

	int ycount = ceil(tessellate);
	for ( int y = 0; y < ycount; y++ )
	{
		int rowcount = (y<<1)+1;
		bool parity = true;				//true=odd, false = even
		int CurEven = EvenIndex;
		int CurOdd = OddIndex;
		for ( int x = 0; x < rowcount; x++ )
		{
			if ( parity )	//odd
			{
				m_IList_Tess.push_back( CurEven );
				m_IList_Tess.push_back( ++CurOdd );	//reverse winding order
				m_IList_Tess.push_back( CurOdd-1 );
			}
			else			//even
			{
				m_IList_Tess.push_back( CurEven++ );
				m_IList_Tess.push_back( CurEven );
				m_IList_Tess.push_back( CurOdd );
			}
			parity = !parity;	//flip parity
		}
		EvenIndex = OddIndex;
		OddIndex = ++CurOdd;
	}
}

//--------------------------------------------------------------------
// Subdivide given number of times and generate mesh from the 
// highest level. Keep around the network for updating.
//--------------------------------------------------------------------
void smdlPNTessellator::Tessellate()
{
	//calculate the tessellated vertices
//	int newprimcount = GetTessTriangles( m_CurSubdivLevel, m_PatchList.size() );
//	int newvertcount = GetTessVertices( m_CurSubdivLevel, m_PatchList.size() );

	m_VList_Tess.clear();
//	m_VList_Tess.reserve( newvertcount );

	m_IList_Tess.clear();
//	m_IList_Tess.reserve( newprimcount * 3 );

//	float tess = g_tessellate;
	float tess = m_CurSubdivLevel+1.0f;
	if ( tess < 1 ) tess = 1;
	if ( m_CurSubdivLevel >= 3 ) tess = 4;
	if ( m_bDisplayControlMesh ) tess = 2;

	std::vector<nPatch>::iterator it = m_PatchList.begin();
	for ( int f = 0; f < m_PatchList.size(); f++, it++ )
	{
		int ID = ComputeSubVertices( *it, tess );
		CreatePatchIndices( ID, tess );
	}
}

//--------------------------------------------------------------------
// Generate fragment info from a subdivision level.
//--------------------------------------------------------------------
void smdlPNTessellator::GatherFragInfo()
{
	// Calculate the vertex normals of the new mesh using face normal averaging
/*	if (c_bDebugSubdivDetails)
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
*/
	int nNewVerts = m_VList_Tess.size();
	tVert* pV = &m_VList_Tess[0];

	m_SubdivFragInfo.m_Indices = m_IList_Tess;
	m_SubdivFragInfo.m_WeldIndex = m_IList_Tess.size();

	m_SubdivFragInfo.m_Vertices.resize(nNewVerts);
	m_SubdivFragInfo.m_Normals.resize(nNewVerts);
	if (m_bHasTextureCoords)
	{
		m_SubdivFragInfo.m_UVs.resize(nNewVerts);
		m_SubdivFragInfo.m_Ss.resize(nNewVerts);
		m_SubdivFragInfo.m_Ts.resize(nNewVerts);
	}
	else
	{
		m_SubdivFragInfo.m_UVs.clear();
		m_SubdivFragInfo.m_Ss.clear();
		m_SubdivFragInfo.m_Ts.clear();
	}

	for (int i=0; i<nNewVerts; i++, pV++)
	{
		m_SubdivFragInfo.m_Vertices[i] = pV->Position;
		m_SubdivFragInfo.m_Normals[i] = pV->Normal;
		if (m_bHasTextureCoords)
		{
			m_SubdivFragInfo.m_UVs[i] = pV->UV;
			m_SubdivFragInfo.m_Ss[i] = pV->Tangent;
			m_SubdivFragInfo.m_Ts[i] = pV->Binormal;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void smdlPNTessellator::AverageNormals()
{
	// reset all vertex normals
	for ( int i = 0; i < m_VList.size(); i++ )
	{
		m_VList[i].Normal.Set(0,0,0);
	}

	// find all triangle normals
	for ( int i = 0; i < m_TList.size(); i++ )
	{
		maVector3d faceNormal = m_TList[i].ComputeNormal();

		// add the normal to each vertex of the face
		m_TList[i].m_pVert[0]->Normal += faceNormal;
		m_TList[i].m_pVert[1]->Normal += faceNormal;
		m_TList[i].m_pVert[2]->Normal += faceNormal;
	}

	// normalize all vertex normals
	for ( int i = 0; i < m_VList.size(); i++ )
	{
		if ( !m_VList[i].Normal.Normalize() )
		{
			m_VList[i].Normal.Set(0,0,1);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Compare this code with that in bumpTriMeshBumpFrag.cpp. Could probably be factored together.
void smdlPNTessellator::ComputeBasisVectors()
{
	// Clear the basis vectors
	for ( int i = 0; i < m_VList.size(); i++ )
	{
		m_VList[i].Tangent.Set(0,0,0);
		m_VList[i].Binormal.Set(0,0,0);
	}

	// Walk through the triangle list and calculate gradients for each triangle.
	for ( int i = 0; i < m_TList.size(); i++ )
	{
		tVert& v0 = *(m_TList[i].m_pVert[0]);
		tVert& v1 = *(m_TList[i].m_pVert[1]);
		tVert& v2 = *(m_TList[i].m_pVert[2]);

		maVector3d S, T;

		// x, s, t
		maVector3d edge01( v1.Position.m_X - v0.Position.m_X, v1.UV.m_X - v0.UV.m_X, v1.UV.m_Y - v0.UV.m_Y );
		maVector3d edge02( v2.Position.m_X - v0.Position.m_X, v2.UV.m_X - v0.UV.m_X, v2.UV.m_Y - v0.UV.m_Y );

		maVector3d cp = edge01 / edge02;
		if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
		{
			S.m_X = -cp.m_Y / cp.m_X;
			T.m_X = -cp.m_Z / cp.m_X;
		}

		// y, s, t
		edge01.Set( v1.Position.m_Y - v0.Position.m_Y, v1.UV.m_X - v0.UV.m_X, v1.UV.m_Y - v0.UV.m_Y );
		edge02.Set( v2.Position.m_Y - v0.Position.m_Y, v2.UV.m_X - v0.UV.m_X, v2.UV.m_Y - v0.UV.m_Y );

		cp = edge01 / edge02;
		if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
		{
			S.m_Y = -cp.m_Y / cp.m_X;
			T.m_Y = -cp.m_Z / cp.m_X;
		}


		// z, s, t
		edge01.Set( v1.Position.m_Z - v0.Position.m_Z, v1.UV.m_X - v0.UV.m_X, v1.UV.m_Y - v0.UV.m_Y );
		edge02.Set( v2.Position.m_Z - v0.Position.m_Z, v2.UV.m_X - v0.UV.m_X, v2.UV.m_Y - v0.UV.m_Y );

		cp = edge01 / edge02;
		if ( fabs(cp.m_X) > maConstants::c_fEpsilon*maConstants::c_fEpsilon )
		{
			S.m_Z = -cp.m_Y / cp.m_X;
			T.m_Z = -cp.m_Z / cp.m_X;
		}

		S.Normalize();
		T.Normalize();

		// Now add normalized vector to actual vertex
		v0.Tangent += S; v0.Binormal += T;
		v1.Tangent += S; v1.Binormal += T;
		v2.Tangent += S; v2.Binormal += T;
	}

	// normalize the summed vectors
	for ( int i = 0; i < m_VList.size(); i++ )
	{
		// Normalize the S, T vectors
		m_VList[i].Tangent.Normalize();
		m_VList[i].Binormal.Normalize();
	} 
}

