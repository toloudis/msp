/*****************************************************************************
**	smdlPatchTessellator.cpp
**
**		see .hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlPatchTessellator.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/Mat/matTexture.hpp"
#include "Graphics/Mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/smdl/private/smdlSubdivNetwork.hpp"

#include <set>


//============================================================================
//============================================================================
namespace
{
	bool l_bUpdateNormals = true;
}

bool smdlPatchTessellator::sm_bComputeBasisVectors = true;


//--------------------------------------------------------------------
// Set flag to turn off generation of basis vectors (for faster loads)
//--------------------------------------------------------------------
//static 
void smdlPatchTessellator::SetComputeBasisVectors(bool i_bCompute)
{
	smdlPatchTessellator::sm_bComputeBasisVectors = i_bCompute;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
smdlPatchTessellator::smdlPatchTessellator( shared_ptr<smdlSubdivNetwork> i_pSubdivNetwork )
{
	m_pSubdivNetwork = i_pSubdivNetwork;
	m_bDisplayControlMesh = false;
	int sublevel = 0;
	m_bSubRequired = false;

	//check for non quads in base mesh
	smdlSubdivUtil::sFace* pT = i_pSubdivNetwork->m_Levels[0]->m_pTList;
	int nFaces = i_pSubdivNetwork->m_Levels[0]->m_nFaces;
	for (int f=0; f<nFaces; f++, pT++)
	{
		if ( pT->m_NumVerts != 4 )
		{
			DBG_WARNING( "Patch Tessellator: Input mesh has non quads, subdivision required.  Consider quadrangulating mesh for better performance.");
			sublevel = 1;	//we need to subdivide to guarantee quads
			m_bSubRequired = true;
			break;
		}
	}

	i_pSubdivNetwork->SetCurrentSubdivLevel(sublevel);		//subdivide for smoothed vertices

	m_SubdivFragInfo = i_pSubdivNetwork->m_SubdivFragInfo;
	m_CurSubdivLevel = 0;
	m_bHasTextureCoords = i_pSubdivNetwork->m_bHasTextureCoords;
	m_MaxSubdivLevel = i_pSubdivNetwork->m_MaxSubdivLevel;

	std::vector<float3> oVerts;
	//initialize vertex lists
	int count = i_pSubdivNetwork->m_Levels[sublevel]->m_nVerts;
	smdlSubdivUtil::sVert* pVerts = i_pSubdivNetwork->m_Levels[sublevel]->m_pVList;
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

	pT = i_pSubdivNetwork->m_Levels[sublevel]->m_pTList;
	int nNewFaces = i_pSubdivNetwork->m_Levels[sublevel]->m_nFaces;

	//create N Patches from base mesh
#ifdef NEWPATCHES
	m_BPatchList.resize( nNewFaces );
#else
	ConvertSubdivs( pT, nNewFaces );
	m_PatchList.reserve( nNewFaces );
#endif

	int ind = 0;
	pT = i_pSubdivNetwork->m_Levels[sublevel]->m_pTList;
	for (int f=0; f<nNewFaces; f++, pT++)
	{
#ifdef NEWPATCHES
#ifdef QUAD_PATCHES
		m_BPatchList[f].ConvertFromFace( pT );
#endif//QUAD_PATCHES
#else
		qPatch patch;
		patch.a = &m_VList[ pT->m_VertList[0]->m_Index ];
		patch.b = &m_VList[ pT->m_VertList[1]->m_Index ];
		patch.c = &m_VList[ pT->m_VertList[2]->m_Index ];
		patch.d = &m_VList[ pT->m_VertList[3]->m_Index ];

		patch.ConvertFromPatch( m_SubPatches[f] );
		m_PatchList.push_back( patch );
#endif


/*
#define EPSILON 0.1f
//		if ( m_SubPatches[f].GetType() != bPatch::Discontinuous )
		{
			//compare Tangents
			for ( int i = 0; i < 16; i++ )
			{
				const float3& a = patch.GetUTangentPoints()[i];
				const float3& b = m_BPatchList[f].GetUTangentPoints()[i];

				if ( (a-b).LengthSqr() > EPSILON )
				{
					int breakme = 1;
				}
				const float3& c = patch.GetVTangentPoints()[i];
				const float3& d = m_BPatchList[f].GetVTangentPoints()[i];
				if ( (c-d).LengthSqr() > EPSILON )
				{
					int breakme = 1;
				}
			}
		}
*/
/*
		tQuad quad;

		int nv = pT->m_NumVerts;

		DBG_ASSERT( 4 == nv, "smdlPatchTessellator, only quad meshes supported." );

		quad.m_pVert[0] = &m_VList[ pT->m_VertList[0]->m_Index ];
		quad.m_pVert[1] = &m_VList[ pT->m_VertList[1]->m_Index ];
		quad.m_pVert[2] = &m_VList[ pT->m_VertList[2]->m_Index ];
		quad.m_pVert[3] = &m_VList[ pT->m_VertList[3]->m_Index ];
		m_QList.push_back( quad );

		qPatch patch;
		patch.a = *quad.m_pVert[0];
		patch.b = *quad.m_pVert[1];
		patch.c = *quad.m_pVert[2];
		patch.d = *quad.m_pVert[3];
		if ( true )	//always calculate control points
		{
			//patch.CalculateControlPoints();
			patch.ConvertFromPatch( m_SubPatches[f] );
		}
		m_PatchList.push_back( patch );
*/
	}

//	i_pSubdivNetwork->SetCurrentSubdivLevel(0);

	Tessellate();

	GatherFragInfo();

	LoadMeshTexture();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
smdlPatchTessellator::~smdlPatchTessellator()
{
	matTextureMgr::ReleaseTexture( m_pMeshTexture );
}

#ifndef NEWPATCHES
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void smdlPatchTessellator::ConvertSubdivs( smdlSubdivUtil::sFace* i_pF, int i_nFaces )
{
	int Ord = 0;
	int Extr = 0;
	int Disc = 0;
	for ( int f = 0; f < i_nFaces; f++, i_pF++)
	{
		int nv = i_pF->m_NumVerts;
		DBG_ASSERT( 4 == nv, "smdlPatchTessellator, only quad meshes supported." );
		if ( nv != 4 ) continue;		//ignore non quads

		bPatch ssPatch;

		//add interior vertices
		for ( int q = 0; q < 4; q++ )
		{
			//			ssPatch.AddUnique( pT->m_VertList[q]->m_Index );
			ssPatch.AddUnique( &m_VList[ i_pF->m_VertList[q]->m_Index ] );
		}

		//walk vertices for 1 ring neighborhood, record valence and prefix for each interior corner
		bool bDiscontinuous = false;
		int lastvert = 3;
		for ( int q = 0; q < 4; q++ )
		{
			int nOrigCount = ssPatch.size();

			//start with prior edge
			smdlSubdivUtil::sVert* pPrev = i_pF->m_VertList[lastvert];
			smdlSubdivUtil::sVert* pCur = i_pF->m_VertList[q];
			//next edge
			smdlSubdivUtil::sVert* pNext = i_pF->m_VertList[(q+1)&3];

			//find face that shares next edge
			smdlSubdivUtil::sVert* pLCur = pCur;	//current vertex for last face (could be changed)
			smdlSubdivUtil::sFace* pLastFace = FindEdgeFace( i_pF, &pLCur, &pNext );
			//find face that shares prior edge
			smdlSubdivUtil::sFace* pCurFace = FindEdgeFace( i_pF, &pPrev, &pCur );
			if ( pCurFace )
			{
				//add prev adjacent vertex to pCur of second face
				pNext = pCurFace->PrevVert( pCur );
				if ( pNext )
				{
					//					ssPatch.AddUnique( pNext->m_Index );
					ssPatch.AddUnique( &m_VList[ pNext->m_Index ] );
				}
				else bDiscontinuous = true;

				//proceed to adjacent face of shared edge
				pCurFace = FindEdgeFace( pCurFace, &pNext, &pCur );
			}
			else bDiscontinuous = true;

			if ( pCurFace )
			{
				while ( pCurFace && pCurFace != pLastFace )		//unsafe, consider alternative algorithm
				{
					//add next adjacent vertex to pNext of current face
					pNext = pCurFace->NextVert( pNext );
					if ( pNext )
					{
						//					ssPatch.AddUnique( pNext->m_Index );
						ssPatch.AddUnique( &m_VList[ pNext->m_Index ] );
					}
					else bDiscontinuous = true;

					//add next adjacent vertex to pNext of current face
					pNext = pCurFace->NextVert( pNext );
					if ( pNext )
					{
						//					ssPatch.AddUnique( pNext->m_Index );
						ssPatch.AddUnique( &m_VList[ pNext->m_Index ] );
					}
					else bDiscontinuous = true;

					//proceed to adjacent face of shared edge
					pCurFace = FindEdgeFace( pCurFace, &pNext, &pCur );
				}
			}
			else if ( pLastFace )	//no start face but an ending face
			{
				//add next adjacent vertex to pNext of current face
				pNext = pLastFace->NextVert( pCur );
				if ( pNext )
				{
					//					ssPatch.AddUnique( pNext->m_Index );
					ssPatch.AddUnique( &m_VList[ pNext->m_Index ] );
				}
				else bDiscontinuous = true;
			}
			
			uint32 Valence = ((ssPatch.size() - nOrigCount)+5)>>1;
			if ( Valence <= 2 ) bDiscontinuous = true;	//boundary or edge condition
			ssPatch.SetValence( q, Valence ); //valence = (newcount+5)/2
			ssPatch.SetPrefix( q, ssPatch.size() );	//set prefix to size of vertex array
			ssPatch.SetBoundaryEdge( q, pLastFace == 0 );	//lastface has no adjacent face so its a boundary edge

			lastvert = (lastvert+1) & 3;
		}

		if ( bDiscontinuous )
		{
			ssPatch.SetType( bPatch::Discontinuous );
			Disc++;
		}
		else if ( ssPatch.IsOrdinary() )
		{
			ssPatch.SetType( bPatch::Ordinary );
			Ord++;
		}
		else
		{
			ssPatch.SetType( bPatch::Extraordinary );
			Extr++;
		}

		m_SubPatches.push_back( ssPatch );
	}

	DBG_LOG( "PatchTessellator Convert--Faces:" << i_nFaces << ", O:" << Ord << ", E:" << Extr << ", D:" << Disc );

}
#endif//!NEWPATCHES


//============================================================================
//this algorithm fit the mesh best into a square
//============================================================================
#define MESHALGORITHM 1

//============================================================================
//this algorithm fits the mesh into the best dimensions that minimizes waste
//============================================================================
//#define MESHALGORITHM 2


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void smdlPatchTessellator::UpdateMeshTexture()
{
	if ( m_pMeshTexture )
	{
#ifdef NEWPATCHES
		uint32 PatchCount = m_BPatchList.size();
#else
		uint32 PatchCount = m_PatchList.size();
#endif

		std::vector<maFloatRGBA> ControlPoints;

//		int width = m_pMeshTexture->GetWidth();
//		int height = m_pMeshTexture->GetHeight();
//		int size = width * height;
		ControlPoints.reserve( PatchCount * 48 );

		int p = 0;
		for ( ; p < PatchCount; p++ )
		{
#ifdef NEWPATCHES
			BezierPatch& patch = m_BPatchList[ p ];
#else
			qPatch& patch = m_PatchList[ p ];
#endif
			const float3 (&cpoints)[16] = patch.GetControlPoints();
			for ( int cp = 0; cp < 16; cp++ )
			{
				ControlPoints.push_back( maFloatRGBA( cpoints[ cp ].m_X, cpoints[ cp ].m_Y, cpoints[ cp ].m_Z, 0.0f ));
			}
			const float3 (&upoints)[16] = patch.GetUTangentPoints();
			for ( int up = 0; up < 16; up++ )
			{
				ControlPoints.push_back( maFloatRGBA( upoints[ up ].m_X, upoints[ up ].m_Y, upoints[ up ].m_Z, 0.0f ));
			}
			const float3 (&vpoints)[16] = patch.GetVTangentPoints();
			for ( int vp = 0; vp < 16; vp++ )
			{
				ControlPoints.push_back( maFloatRGBA( vpoints[ vp ].m_X, vpoints[ vp ].m_Y, vpoints[ vp ].m_Z, 0.0f ));
			}
		}
/*		//fill extra space
		for ( int e = p*48; e < size; e++ )
		{
			ControlPoints.push_back( maFloatRGBA( 0, 0, 0, 0 ));
		}
*/
		matTextureMgr::UpdateSurface( m_pMeshTexture, (unsigned char*)(&ControlPoints[0]), ControlPoints.size()*sizeof(maFloatRGBA) );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void smdlPatchTessellator::LoadMeshTexture()
{
#ifdef NEWPATCHES
	uint32 PatchCount = m_BPatchList.size();
#else
	uint32 PatchCount = m_PatchList.size();
#endif

	//smartly calculate the texture dimensions
	//width must be a multiple of 16
	//height can be any value
	//minimize wasted space
	//square is faster caching
	uint32 PatchMeshSize = PatchCount * 48;

	//square algorithm (wastes space)
#if MESHALGORITHM == 1
	uint32 fsize = (uint32)ceil(sqrt((float)PatchMeshSize));
	uint32 width = (fsize + 15) & 0xfffffff0;
	uint32 height = (uint32)ceil(PatchMeshSize / (float)width);

#endif
	//smallest algorithm (hurts cache)
#if MESHALGORITHM == 2
	uint32 width = 16;
	uint32 height = 4096;
	do
	{
		height = ceil( PatchMeshSize / (float)width );
	}while ( height > 4096, width += 16 );
#endif

	m_pMeshTexture = matTextureMgr::CreateTexture( width, height, &g2dPFD( g2dPFD::e_RGBA32f, 128));

	UpdateMeshTexture();
}

//-----------------------------------------------------------------------------------------
//find the face that shares the edge defined by the two vertices and is not the input face (an edge can only share two faces)
// This method also searches the shared vertices to find a matching face (pNext of a vertex)
//-----------------------------------------------------------------------------------------
smdlSubdivUtil::sFace* smdlPatchTessellator::FindEdgeFace( smdlSubdivUtil::sFace* i_NotFace, smdlSubdivUtil::sVert** io_V1, smdlSubdivUtil::sVert** io_V2 )
{
	smdlSubdivUtil::sVert* pV2_Cur = *io_V2;
	do 
	{
		smdlSubdivUtil::sVert* pV1_Cur = *io_V1;
		do 
		{
			int faces = pV1_Cur->m_FaceList.size();
			for ( int f = 0; f < faces; f++ )
			{
				smdlSubdivUtil::sFace* pFace = pV1_Cur->m_FaceList[ f ];
				if ( pFace == i_NotFace ) continue;
				if ( pFace->Contains( pV2_Cur ))
				{
					*io_V1 = pV1_Cur;
					*io_V2 = pV2_Cur;
					return pFace;
				}
			}
			pV1_Cur = pV1_Cur->m_pNext;
		} while ( pV1_Cur && pV1_Cur != *io_V1 );
		pV2_Cur = pV2_Cur->m_pNext;
	} while ( pV2_Cur && pV2_Cur != *io_V2 );
	return NULL;
}

//--------------------------------------------------------------------
// Return current subdivision level being used.
//--------------------------------------------------------------------
int smdlPatchTessellator::GetCurrentSubdivLevel() const
{
	return m_CurSubdivLevel;
}

//--------------------------------------------------------------------
// Return maximum level of subdivision for which the network 
//	has been created
//--------------------------------------------------------------------
int smdlPatchTessellator::GetMaxSubdivLevel() const
{
	return m_MaxSubdivLevel;
}

//--------------------------------------------------------------------
// Set the current subdivision level being used, this should be 
// a level less than or equal to the return value of 
// GetMaxSubdivLevel()
//--------------------------------------------------------------------
void smdlPatchTessellator::SetCurrentSubdivLevel(int i_SubdivLevel)
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
const mdlSplitFragInfo& smdlPatchTessellator::GetSubdivFragInfo() const
{
	return m_SubdivFragInfo;
}

//--------------------------------------------------------------------
// Return number of vertices in the subdivided model
// at the current subdivision level, used for sizing
// the array for GetSubdivVertices()
//--------------------------------------------------------------------
int smdlPatchTessellator::GetNumFacesAtLevel(int i_SubdivLevel) const
{
#ifdef NEWPATCHES
	return m_BPatchList.size();
#else
	return GetTessTriangles( i_SubdivLevel, m_PatchList.size() );
#endif
}

#ifndef NEWPATCHES
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int smdlPatchTessellator::GetTessTriangles(int level, int tris ) const
{
	if ( level <= 0 ) return tris;
	if ( level == 1 ) return tris * 6;
	else return tris*level*24;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int smdlPatchTessellator::GetTessVertices(int level, int tris ) const
{
	int newtris = GetTessTriangles( level, tris );

	if ( level <= 0 ) return tris*3;
	if ( level == 1 ) return tris*7;
	else return tris*7*level;
}
#endif//!NEWPATCHES

//--------------------------------------------------------------------
// Return number of vertices in base mesh
//--------------------------------------------------------------------
int smdlPatchTessellator::GetNumBaseMeshVertices()
{
	return m_VList.size();
}

//--------------------------------------------------------------------
// Return number of vertices in the subdivided model
//--------------------------------------------------------------------
int smdlPatchTessellator::GetNumTessellatedVertices() const
{
	return m_VList_Tess.size();
}

//--------------------------------------------------------------------
// Return direct access to the vertex list in the current
//	subdivision level.
//--------------------------------------------------------------------
const std::vector<tVert>& smdlPatchTessellator::GetTessellatedVertices() const
{
	return m_VList_Tess;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
matTexture* smdlPatchTessellator::GetTessellatorMeshTexture()
{
	return m_pMeshTexture;
}


//--------------------------------------------------------------------
// Given the new array of mesh vertices, compute the new positions
//	of the subdivided mesh. The number of vertices passed in
//	should match the number of vertices in the original mesh.
//--------------------------------------------------------------------
void smdlPatchTessellator::AlterBaseMesh( const std::vector<maPoint3d>& i_Vertices )
{
	// Put these new vertex locations into the base mesh
//	bool hasNormals = !i_Normals.empty();
	if ( m_bSubRequired ) m_pSubdivNetwork->SetCurrentSubdivLevel( 1 );
	else  m_pSubdivNetwork->SetCurrentSubdivLevel( 0 );

	m_pSubdivNetwork->AlterBaseMesh( &i_Vertices[0], i_Vertices.size() );

	if ( GetHardwareTessellate() )
	{
		DBG_ASSERT( m_pSubdivNetwork->GetNumSubdivVertices() == m_VList.size(), "AlterBaseMesh, number of vertices don't match." );
		const smdlSubdivUtil::sVert* Verts = m_pSubdivNetwork->GetSubdivVertices();
		for ( int i = 0; i < m_pSubdivNetwork->GetNumSubdivVertices(); i++, Verts++ )
		{
			m_VList[ i ].Position = Verts->m_Loc;
		}

#ifdef NEWPATCHES
#ifdef QUAD_PATCHES
		DBG_ASSERT( m_BPatchList.size() == m_pSubdivNetwork->GetNumFacesAtLevel(m_pSubdivNetwork->GetCurrentSubdivLevel()), "AlterBaseMesh, number of patches don't don't match subdiv faces." );
		smdlSubdivUtil::sFace* pFace = m_pSubdivNetwork->m_Levels[m_pSubdivNetwork->GetCurrentSubdivLevel()]->m_pTList;
		std::vector<BezierPatch>::iterator it = m_BPatchList.begin();
		for ( ; it != m_BPatchList.end(); it++, pFace++ )
		{
			it->ConvertFromFace( pFace );
		}
#endif//QUAD_PATCHES
#else
		DBG_ASSERT( m_PatchList.size() == m_pSubdivNetwork->GetNumFacesAtLevel(m_pSubdivNetwork->GetCurrentSubdivLevel()), "AlterBaseMesh, number of patches don't don't match subdiv faces." );
		for ( int t = 0; t < m_PatchList.size(); t++ )
		{
			m_PatchList[ t ].ConvertFromPatch( m_SubPatches[t] );
		}
#endif

		Tessellate();

		UpdateMeshTexture();
	}
}


//--------------------------------------------------------------------
// Subdiv networks can be shared, but only one can be actively
// updating positions at a time. So, you need to wrap
// calls to "AlterBaseMesh" and "GetSubdivVertices" with
// a envScopedLock using this envMutex.
//--------------------------------------------------------------------
//envMutex& smdlPatchTessellator::GetMutex()
//{
//	return m_Mutex;
//}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int smdlPatchTessellator::AddTessellatedVertex( const qPatch& patch, const float4& barycenter )
{
//	m_VList_Tess.push_back( patch.ComputeTessVertex( barycenter ));
	return m_VList_Tess.size()-1;;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int smdlPatchTessellator::OriginalVerts( const qPatch& patch )
{
	int ID = m_VList_Tess.size();

	AddTessellatedVertex( patch, maVector4d( 1, 0, 0, 0 ));
	AddTessellatedVertex( patch, maVector4d( 0, 1, 0, 0 ));
	AddTessellatedVertex( patch, maVector4d( 0, 0, 1, 0 ));
	AddTessellatedVertex( patch, maVector4d( 0, 0, 0, 1 ));

	return ID;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int smdlPatchTessellator::PreTessellate( const qPatch& patch )
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
int smdlPatchTessellator::ComputeSubVertices( const qPatch& patch, float tessellate  )
{
	int ID = m_VList_Tess.size();
	static int index[9][4] =
	{
		{0,1,5,4},
		{1,2,6,5},
		{2,3,7,6},
		{4,5,9,8},
		{5,6,10,9},
		{6,7,11,10},
		{8,9,13,12},
		{9,10,14,13},
		{10,11,15,14},
	};

	if ( m_bDisplayControlMesh )
	{
//		const std::vector<float3>& CP = patch.GetControlPoints();
		const float3 (&CP)[16] = patch.GetControlPoints();
/*		tVert V = patch.a;

//		DBG_ASSERT( CP.size() == 16, "Must have 16 control points!");
		for ( int q = 0; q < 9; q++ )
		{
			V.Position = CP[ index[q][0] ];
			m_VList_Tess.push_back( V );
			V.Position = CP[ index[q][1] ];
			m_VList_Tess.push_back( V );
			V.Position = CP[ index[q][2] ];
			m_VList_Tess.push_back( V );
			V.Position = CP[ index[q][3] ];
			m_VList_Tess.push_back( V );
		}
*/
	}
	else
	{
		if ( tessellate <= 1.0f )
		{
			m_VList_Tess.push_back( *patch.a );
			m_VList_Tess.push_back( *patch.b );
			m_VList_Tess.push_back( *patch.c );
			m_VList_Tess.push_back( *patch.d );
//			AddTessellatedVertex( patch, maVector4d( 1, 0, 0, 0 ));
//			AddTessellatedVertex( patch, maVector4d( 0, 1, 0, 0 ));
//			AddTessellatedVertex( patch, maVector4d( 0, 0, 1, 0 ));
//			AddTessellatedVertex( patch, maVector4d( 0, 0, 0, 1 ));
		}
		else
		{
			float d = 1.0f / tessellate;

			float v = 0;
			int xcount = ceil(tessellate);
			int ycount = xcount;
			for ( int y = 0; y <= ycount; y++ )
			{
				float u = 0;
				for ( int x = 0; x <= xcount; x++ )
				{
					AddTessellatedVertex( patch, maVector4d( u, 1-u, v, 1-v ));
					u += d;
					if ( u >= 1 ) u = 1;
				}
				v += d;
				if ( v >= 1 ) v = 1;
			}
		}
	}

	return ID;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void smdlPatchTessellator::CreatePatchIndices( int StartIndex, float tessellate )
{
	if ( tessellate <= 1.0f )
	{
		m_IList_Tess.push_back( StartIndex++ );
		m_IList_Tess.push_back( StartIndex++ );
		m_IList_Tess.push_back( StartIndex++ );
		m_IList_Tess.push_back( StartIndex++ );
	}
	else
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
}

//--------------------------------------------------------------------
// Subdivide given number of times and generate mesh from the 
// highest level. Keep around the network for updating.
//--------------------------------------------------------------------
void smdlPatchTessellator::Tessellate()
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
	if ( m_bDisplayControlMesh ) tess = 1;

#ifdef NEWPATCHES
	std::vector<BezierPatch>::iterator it = m_BPatchList.begin();
	for ( ; it != m_BPatchList.end(); it++ )
	{
		tVert V;
		V.Normal = float3(0,1,0);
		V.Position = it->GetControlPoints()[0];
		V.UV = it->GetUV()[0];
		m_VList_Tess.push_back( V );
		V.Position = it->GetControlPoints()[3];
		V.UV = it->GetUV()[1];
		m_VList_Tess.push_back( V );
		V.Position = it->GetControlPoints()[15];
		V.UV = it->GetUV()[2];
		m_VList_Tess.push_back( V );
		V.Position = it->GetControlPoints()[12];
		V.UV = it->GetUV()[3];
		m_VList_Tess.push_back( V );
	}
	m_IList_Tess.resize( m_BPatchList.size() );
#else
	std::vector<qPatch>::iterator it = m_PatchList.begin();
	for ( int f = 0; f < m_PatchList.size(); f++, it++ )
	{
		int ID = ComputeSubVertices( *it, tess );
		CreatePatchIndices( ID, tess );
	}
#endif
}

//--------------------------------------------------------------------
// Generate fragment info from a subdivision level.
//--------------------------------------------------------------------
void smdlPatchTessellator::GatherFragInfo()
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
//		m_SubdivFragInfo.m_Vertices[i] = float3(0,0,0);	//jcs - zero out for tessellator bug
		m_SubdivFragInfo.m_Normals[i] = pV->Normal;
		if (m_bHasTextureCoords)
		{
			m_SubdivFragInfo.m_UVs[i] = pV->UV;
			m_SubdivFragInfo.m_Ss[i] = pV->Tangent;
			m_SubdivFragInfo.m_Ts[i] = pV->Binormal;
		}
	}
}

#ifndef NEWPATCHES
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void smdlPatchTessellator::AverageNormals()
{
	// reset all vertex normals
	for ( int i = 0; i < m_VList.size(); i++ )
	{
		m_VList[i].Normal.Set(0,0,0);
	}

	// find all triangle normals
	for ( int i = 0; i < m_QList.size(); i++ )
	{
		maVector3d faceNormal = m_QList[i].ComputeNormal();

		// add the normal to each vertex of the face
		m_QList[i].m_pVert[0]->Normal += faceNormal;
		m_QList[i].m_pVert[1]->Normal += faceNormal;
		m_QList[i].m_pVert[2]->Normal += faceNormal;
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
// Compare this code with that in bumpTriMeshBumpFrag.cpp. Could probably be factored together.
//----------------------------------------------------------------------------
void smdlPatchTessellator::ComputeBasisVectors()
{
	// Clear the basis vectors
	for ( int i = 0; i < m_VList.size(); i++ )
	{
		m_VList[i].Tangent.Set(0,0,0);
		m_VList[i].Binormal.Set(0,0,0);
	}

	// Walk through the triangle list and calculate gradients for each triangle.
	for ( int i = 0; i < m_QList.size(); i++ )
	{
		tVert& v0 = *(m_QList[i].m_pVert[0]);
		tVert& v1 = *(m_QList[i].m_pVert[1]);
		tVert& v2 = *(m_QList[i].m_pVert[2]);

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
#endif//!NEWPATCHES

