/*****************************************************************************
**	smdlBezierPatch.cpp
**
**		see .hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlBezierPatch.hpp"


//============================================================================
//============================================================================
namespace
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static const int l_From[16] =		//Note last row isn't used, temp is placed instead
	{
		 3, 15, 12, 0,
		 7, 14,  8, 1,
		11, 13,  4, 2,
		 6, 10,  9, 5
	};

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static const int l_To[16] =
	{
		0,  3, 15, 12,
		1,  7, 14,  8,
		2, 11, 13,  4,
		5,  6, 10,  9
	};

	static const double l_PI = 3.1415926535897932384626433832795;
	static const double l_TWOPI = l_PI*2;

	static const double l_TanMaskAlpha[4] =
	{
		0.33333334326744080, 0, -0.33333334326744080, 0
	};
	static const double l_TanMaskBeta[4] =
	{
		0.083333335816860199, -0.083333335816860199, -0.083333328366279602, 0.083333328366279602
	};
}

/*
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
double CalcTM( int i_Index, int i_Valence, bool bBeta )
{
	DBG_ASSERT( i_Index < i_Valence, "Index larger than Valence." );
	double CosfPIV = cos( l_PI / i_Valence );
	double VSqrtTerm = ( i_Valence * sqrt( 4.0 + CosfPIV * CosfPIV ) );

	if ( bBeta ) return (1.0 / VSqrtTerm) * cos( (l_TWOPI * i_Index + l_PI) / (float)i_Valence );
	return ((1.0 / i_Valence) + CosfPIV / VSqrtTerm ) * cos( (l_TWOPI * i_Index) / (float)i_Valence );
}
*/
#ifdef QUAD_PATCHES

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool BezierPatch::ConvertFromFace( sFace* pFace )
{
	DBG_ASSERT( pFace, "Cannot Convert a NULL face! " );
	if (!pFace)
		return false;
	DBG_ASSERT( pFace->m_NumVerts == 4, "Only Quads are supported!" );
	if (pFace->m_NumVerts != 4)
		return false;
	if ( pFace->m_NumVerts != 4 ) return false;
	float3 UTanVerts[4];
	float3 VTanVerts[4];
	int Vals[4];
	for ( int v = 0; v < 4; v++ )
	{
		//four quad vertices
		sVert* pVert = pFace->m_VertList[ v ];
		sVert* pNext = pFace->m_VertList[ (v+1)&3 ];
		sVert* pOther = pFace->m_VertList[ (v+2)&3 ];
		sVert* pPrev = pFace->m_VertList[ (v+3)&3 ];

		m_UVs[v] = pVert->m_UV;

		int Valence = pVert->SharedValence();
		Vals[v] = Valence;

		//adjacent edges
		sEdge* pNextEdge = pVert->FindSharedEdge( pNext );
		sEdge* pPrevEdge = pVert->FindSharedEdge( pPrev );

		DBG_ASSERT( pNextEdge, "No Next Edge Found!" );
		DBG_ASSERT( pPrevEdge, "No Prev Edge Found!" );

		if (pNextEdge && pPrevEdge)
		{
			bool BoundA = pNextEdge->IsSharedBoundary();
			bool BoundB = pPrevEdge->IsSharedBoundary();

			ComputeInteriorPoint( pFace, Valence, pVert, pNext, pOther, pPrev, pNextEdge, pPrevEdge, BoundA, BoundB );
			ComputeEdgePoints( pFace, Valence, pVert, pNext, pOther, pPrev, pNextEdge, pPrevEdge, BoundA, BoundB );
			ComputeCornerPoint( pFace, Valence, pVert, pNext, pOther, pPrev, pNextEdge, pPrevEdge, BoundA, BoundB, UTanVerts[v], VTanVerts[v], v&1 );
			RotatePoints();
		}
	}

	UTanVerts[1] = -UTanVerts[1];	//flip the opposite side tangent vectors
	UTanVerts[2] = -UTanVerts[2];
	VTanVerts[2] = -VTanVerts[2];
	VTanVerts[3] = -VTanVerts[3];

//	CalculateUTan( UTanVerts );
//	CalculateVTan( VTanVerts );
	CalculateTangents( Vals, UTanVerts, VTanVerts );
//	ModifyCorners();
//	ModifyEdges();
	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BezierPatch::ComputeInteriorPoint( sFace* pFace, int i_Valence, sVert* pVert, sVert* pNext, sVert* pOther, sVert* pPrev, sEdge* pNextEdge, sEdge* pPrevEdge, bool bNextBound, bool bPrevBound )
{
	//acquire vertex weight from valence
	float weight = (float)i_Valence;

	//adjust weight for boundary cases
	if ( bNextBound && bPrevBound ) weight = 4;	//corner vertex
	else if ( bNextBound || bPrevBound ) weight = (weight-1)*2;	//edge vertex

	m_ControlPoints[5] = (weight*pVert->m_Loc + 2*pNext->m_Loc + pOther->m_Loc + 2*pPrev->m_Loc) / (weight + 5);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BezierPatch::ComputeEdgePoints( sFace* pFace, int i_Valence, sVert* pVert, sVert* pNext, sVert* pOther, sVert* pPrev, sEdge* pNextEdge, sEdge* pPrevEdge, bool bNextBound, bool bPrevBound )
{
	//acquire vertex weight from valence
	float weight = 2.0f * i_Valence;

	// compute edge points - horizontal
	if ( bNextBound )
	{
		m_ControlPoints[1] = (2*pVert->m_Loc + pNext->m_Loc) / 3.0f;
	}
	else
	{
		sFace* pAdj = pNextEdge->OtherFace( pFace );
		sVert* pShared = pAdj->SharedVert( pVert );
		sVert* pNear = pAdj->NextVert( pShared );
		sVert* pFar = pAdj->NextVert( pNear );
		m_ControlPoints[1] = (weight*pVert->m_Loc + 4*pNext->m_Loc + pOther->m_Loc + 2*pPrev->m_Loc + 2*pNear->m_Loc + pFar->m_Loc) / (weight+10);
	}

	// edge points - vertical
	if ( bPrevBound )
	{
		m_ControlPoints[4] = (2*pVert->m_Loc + pPrev->m_Loc) / 3.0f;
	}
	else
	{
		sFace* pAdj = pPrevEdge->OtherFace( pFace );
		sVert* pShared = pAdj->SharedVert( pVert );
		sVert* pNear = pAdj->PrevVert( pShared );
		sVert* pFar = pAdj->PrevVert( pNear );
		m_ControlPoints[4] = (weight*pVert->m_Loc + 2*pNext->m_Loc + pOther->m_Loc + 4*pPrev->m_Loc + 2*pNear->m_Loc + pFar->m_Loc) / (weight+10);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float3 BezierPatch::AccumulateEdges( sVert* pVert, int i_Valence, float3& UTan, float3& VTan, bool bEdgeParity )
{
	int count = 0;
	float3 Pos = float3(0,0,0);

	//accumulate all shared edge vertices
	EdgeList2::iterator eit = pVert->m_SharedEdgeList.begin();
	for ( ; eit != pVert->m_SharedEdgeList.end(); eit++, count++ )
	{
		sVert* pShared = (*eit)->SharedVert( pVert );
		sVert* pAdjVert = (*eit)->Other( pShared );
		Pos += pAdjVert->m_Loc;
		UTan += pAdjVert->m_Loc * CalcTanMask( count+(bEdgeParity?-1:0), i_Valence, ALPHA );
		VTan += pAdjVert->m_Loc * CalcTanMask( count+(bEdgeParity?0:-1), i_Valence, ALPHA );
	}

	return Pos;
}

//----------------------------------------------------------------------------
//accumulate all far vertices of shared faces
//a far vertex is the diagonal vertex in the quad (2 away)
//----------------------------------------------------------------------------
float3 BezierPatch::AccumulateFaces( sVert* pVert, int i_Valence, float3& UTan, float3& VTan, bool bEdgeParity )
{
	int count = 0;
	float3 Pos = float3(0,0,0);

	FaceList::iterator fit = pVert->m_SharedFaceList.begin();
	for ( ; fit != pVert->m_SharedFaceList.end(); fit++, count++ )
	{
		sVert* pShared = (*fit)->SharedVert( pVert );
		sVert* pAdjVert = (*fit)->NextVert( pShared );
		sVert* pFar = (*fit)->NextVert( pAdjVert );
		Pos += pFar->m_Loc;
		UTan += pFar->m_Loc * CalcTanMask( count+(bEdgeParity?-1:0), i_Valence, BETA );
		VTan += pFar->m_Loc * CalcTanMask( count+(bEdgeParity?0:-1), i_Valence, BETA );
	}
	return Pos;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BezierPatch::ComputeCornerPoint( sFace* pFace, int i_Valence, sVert* pVert, sVert* pNext, sVert* pOther, sVert* pPrev, sEdge* pNextEdge, sEdge* pPrevEdge, bool bNextBound, bool bPrevBound, float3& UTan, float3& VTan, bool bEdgeParity )
{
	UTan = float3(0,0,0);
	VTan = float3(0,0,0);

	if ( i_Valence == 2 )	//corner vertex, only shares one face, valence 2
	{
		m_ControlPoints[0] = pVert->m_Loc;
	}
	else
	{
		sEdge* pBoundEdgeA = NULL;
		sEdge* pBoundEdgeB = NULL;
		//search for bounding edges
		EdgeList2::iterator eit = pVert->m_SharedEdgeList.begin();
		for ( int e = 0; e < pVert->m_SharedEdgeList.size(); eit++, e++ )
		{
			if ( (*eit)->IsSharedBoundary() )
			{
				if ( !pBoundEdgeA ) pBoundEdgeA = *eit;
				else if ( !pBoundEdgeB ) pBoundEdgeB = *eit;
				else
				{
//					DBG_WARNING( "Bad Geometry, Vertex " << pVert->m_Index << " has more than two edge boundaries. Only first two will be used!" );
				}
			}
		}

		if ( bNextBound || bPrevBound )
		{
			m_ControlPoints[0] = pVert->m_Loc;	//test

		}
		else if ( pBoundEdgeA && pBoundEdgeB )
		{
			sVert* pSharedA = pBoundEdgeA->SharedVert( pVert );
			sVert* pSharedB = pBoundEdgeB->SharedVert( pVert );

			m_ControlPoints[0] = (4*pVert->m_Loc + pBoundEdgeA->Other( pSharedA )->m_Loc + pBoundEdgeB->Other( pSharedB )->m_Loc) / 6.0f;
		}
		else
		{
			int nNeighbors = i_Valence*i_Valence;
			//start with current vertex
			m_ControlPoints[0] = nNeighbors * pVert->m_Loc;

			pVert->SortSharedLists( pFace );

			m_ControlPoints[0] += AccumulateEdges( pVert, i_Valence, UTan, VTan, bEdgeParity ) * 4;
			m_ControlPoints[0] += AccumulateFaces( pVert, i_Valence, UTan, VTan, bEdgeParity );
			m_ControlPoints[0] /= (float)(nNeighbors + 5*i_Valence);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BezierPatch::RotatePoints()
{
	float3 temp;
	for ( int i = 0; i < 16; i++ )
	{
		int fID = l_From[i];
		int tID = l_To[i];
		if ( (i&3) == 0 ) temp = m_ControlPoints[ tID ];			//save row temp
		if ( (i&3) == 3 ) m_ControlPoints[ tID ] = temp;			//write row temp
		else m_ControlPoints[ tID ] = m_ControlPoints[ fID ];	//move
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BezierPatch::RotateTangents()
{
	float3 tempU, tempV;
	for ( int i = 0; i < 16; i++ )
	{
		int fID = l_From[i];
		int tID = l_To[i];
		if ( (i&3) == 0 )
		{
			tempU = m_UTangentPoints[ tID ];			//save U row temp
			tempV = m_VTangentPoints[ tID ];			//save V row temp
		}
		if ( (i&3) == 3 )
		{
			m_UTangentPoints[ tID ] = tempU;			//write U row temp
			m_VTangentPoints[ tID ] = tempV;			//write V row temp
		}
		else
		{
			m_UTangentPoints[ tID ] = m_UTangentPoints[ fID ];	//move
			m_VTangentPoints[ tID ] = m_VTangentPoints[ fID ];	//move
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float BezierPatch::CalcTanMask( int i_Index, int i_Valence, MaskType i_eMask )
{
	if ( i_Index < 0 ) i_Index += i_Valence;	//make positive;
	int index = i_Index % i_Valence;
	float CosfPIV = cos( l_PI / i_Valence );
	float VSqrtTerm = ( i_Valence * sqrt( 4.0f + CosfPIV * CosfPIV ) );

	if ( i_eMask == BETA ) return (1.0f / VSqrtTerm) * cos( (l_TWOPI * index + l_PI) / (float)i_Valence );
	return ((1.0f / i_Valence) + CosfPIV / VSqrtTerm ) * cos( (l_TWOPI * index) / (float)i_Valence );
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float BezierPatch::CalcBoundaryTangentMask( int i_Index, int i_Valence, MaskType i_eMask, bool i_bVTan )
{
	if ( i_Index < 0 ) i_Index += i_Valence;	//make positive;
	int index = i_Index % i_Valence;
	float CosfPIV = cos( l_PI / i_Valence );
	float VSqrtTerm = ( i_Valence * sqrt( 4.0f + CosfPIV * CosfPIV ) );

	float S = sin( l_PI / i_Valence );
	float C = cos( l_PI / i_Valence );
	float Si = sin( l_PI * index / i_Valence );
	float Si2 = sin( l_PI * ((index+1)%i_Valence) / i_Valence );

	bool bSimple = (i_Valence <= 2);

	bool boundA = (i_Index == 0);
	bool boundB = (i_Index == i_Valence-1);

	bool bound = false;
	if ( i_Index == 0 || i_Index == i_Valence-1 ) bound = true;

	switch( i_eMask )
	{
		case ALPHA:
		{
			if ( i_bVTan )
			{
				if ( bSimple )
				{
					if ( boundB ) return 1;
					return 0;
				}
				if ( boundA ) return 0.5f;
				if ( boundB ) return -0.5f;
				return 0;
			}
			else
			{
				if ( bSimple )
				{
					if ( boundA ) return 1;
					return 0;
				}
				if ( boundA || boundB ) return -(((1+2*C)*sqrt(1+C))/((3*i_Valence+C)*sqrt(1-C)));
				else return (4*Si) / (3*i_Valence+C);
			}
		}
		case BETA:
		{
			if ( bSimple ) return 0;
			if ( i_bVTan ) return 0;
			return (Si+Si2) / (3*i_Valence+C);
		}
		case GAMMA:
		{
			if ( bSimple ) return -1;
			if ( i_bVTan ) return 0;
			return (-4*S) / (3*i_Valence+C);
		}
	}
	return 0;
}


//--------------------------------------------------------------------------------------
// Helper function
//--------------------------------------------------------------------------------------
void BezierPatch::BezierRaise( const float3 (&pQ)[3], float3 (&pC)[4])
{
	pC[0] = pQ[0];
	pC[3] = pQ[2];

	for ( int i=1; i<3; i++ ) 
	{
		pC[i] = (pQ[i - 1] * i + (3.0f - i) * pQ[i]) / 3;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BezierPatch::CalculateUTan( float3 (&UTan)[4] )
{
	float3 UTans[2][3];
	float3 Cubic[4];

	for ( int u = 0; u < 3; u++ )
	{
		UTans[0][u] = 3 * (m_ControlPoints[u+1] - m_ControlPoints[u]);
		UTans[1][u] = 3 * (m_ControlPoints[12+u+1] - m_ControlPoints[12+u]);
	}
	BezierRaise( UTans[0], Cubic );
	UTan[0] = Cubic[0];
	UTan[1] = Cubic[3];
	BezierRaise( UTans[1], Cubic );
	UTan[2] = Cubic[3];
	UTan[3] = Cubic[0];
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BezierPatch::CalculateVTan( float3 (&VTan)[4] )
{
	float3 VTans[2][3];
	float3 Cubic[4];

	for ( int v = 0; v < 3; v++ )
	{
		VTans[0][v] = 3 * (m_ControlPoints[((v+1)<<2)] - m_ControlPoints[(v<<2)]);
		VTans[1][v] = 3 * (m_ControlPoints[((v+1)<<2)+3] - m_ControlPoints[(v<<2)+3]);
	}
	BezierRaise( VTans[0], Cubic );
	VTan[0] = Cubic[0];
	VTan[1] = Cubic[3];
	BezierRaise( VTans[1], Cubic );
	VTan[2] = Cubic[3];
	VTan[3] = Cubic[0];
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//void BezierPatch::ModifyCorners()
//{
//
//}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//void BezierPatch::ModifyEdges()
//{
//
//}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float3 BezierPatch::ProjectTangent( const float3& Tan, const float3& P)
{
	return (2 * Tan + P) / 3;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float3 BezierPatch::ComputeTangent( int i_ID, bool i_bTurn )
{
	static unsigned char RotateRemap[16] =
	{
		0, 4, 8, 0,
		1, 5, 9,13,
		2, 6,10, 0,
		0, 7, 0, 0,
	};
	if ( i_bTurn )
	{
		unsigned char RID = RotateRemap[ i_ID ];
		return 3.0f * (m_ControlPoints[RID+4]-m_ControlPoints[RID]);	//next in column
	}
	return 3.0f * (m_ControlPoints[i_ID+1]-m_ControlPoints[i_ID]);	//next in row
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BezierPatch::ComputeTanPatch2( float3 (&vOut)[16], const float4& fCWts, const float3 (&vCorner)[4], const float3 (&vCornerLocal)[4], bool i_bTurn )
{
	float3 vQuad[3];
	float3 vQuadB[3];
	float3 vCubic[4];

	const int Stride = i_bTurn ? 4 : 1;

	//edge tangents
	float3 ETanA = ComputeTangent( 1, !i_bTurn );	//DV, x=4-8, y=1-2
	float3 ETanB = ComputeTangent( 13, !i_bTurn );	//DV, x=11-7, y=14-13
	float3 ETanC = ComputeTangent( 1, i_bTurn );	//x=2-1, y=8-4
	float3 ETanD = ComputeTangent( 13, i_bTurn );	//x=14-13, y=11-7

	// boundary edges are really simple...
	vQuad[0] = vCornerLocal[0];
	vQuad[1] = ETanC;
	vQuad[2] = vCornerLocal[1];

	BezierRaise(vQuad,vCubic);
	const int xLoc = Stride;
	vOut[xLoc] = vCubic[1];	//x=1, y=4
	vOut[xLoc + Stride] = vCubic[2];	//x=2, y=8

	vQuad[0] = vCornerLocal[2];
	vQuad[1] = ETanD;
	vQuad[2] = vCornerLocal[3];

	BezierRaise(vQuad,vCubic);
	const int yLoc = i_bTurn ? 7 : 13;
	vOut[yLoc] = vCubic[1];			//x=13, y=7
	vOut[yLoc + Stride] = vCubic[2];//x=14, y=11

	// two internal edges - this is where work happens...
	// also do "second" scan line

	//first line
	float3 RPnt1 = vCorner[1];
	float3 LPnt1 = vCorner[0];
	float3 LTan1 = ComputeTangent( 4, i_bTurn );	// x=5-4, y=5-1
	float3 RTan1 = ComputeTangent( 6, i_bTurn );	//x=6-7, y=9-13
	vQuadB[0] = ProjectTangent( fCWts.m_X*ETanA, -fCWts.m_W*LPnt1 ) + LTan1;
	vQuadB[1] = ComputeTangent( 5, i_bTurn );		//x=6-5, y=9-5
	vQuadB[2] = -ProjectTangent( -fCWts.m_Y*ETanB, -fCWts.m_Z*RPnt1 ) + RTan1;	//flip this vector

	BezierRaise(vQuadB,vCubic);

	float3* V = &vOut[i_bTurn?1:4];
	for ( int i = 0; i < 4; i++, V += Stride ) *V = vCubic[i];

	//second line
	float3 LPnt2 = vCorner[3];
	float3 RPnt2 = vCorner[2];
	float3 LTan2 = ComputeTangent( 8, i_bTurn );	//x=9-8, y=6-2
	float3 RTan2 = ComputeTangent( 10, i_bTurn );	//x=10-11, y=10-14
	vQuad[0] = ProjectTangent( -fCWts.m_W*ETanA, fCWts.m_X*LPnt2 ) + LTan2;
	vQuad[1] = ComputeTangent( 9, i_bTurn );		//x=10-9, y=10-6
	vQuad[2] = -ProjectTangent( fCWts.m_Z*ETanB, fCWts.m_Y*RPnt2 ) + RTan2;	//flip this vector

	BezierRaise(vQuad,vCubic);

	V = &vOut[i_bTurn?2:8];
	for ( int i = 0; i < 4; i++, V += Stride ) *V = vCubic[i];
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float4 CalculateWeightVector( const int (&Val)[4] )
{
	float4 weights;
	for ( int i = 0; i < 4; i++ )
	{
		weights[i] = cos(( 2.0f * l_PI) / (float)Val[i]);
	}
	return weights;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BezierPatch::CalculateTangents( const int (&Val)[4], const float3 (&CornersU)[4], const float3 (&CornersV)[4] )
{
	float4 fCWts = CalculateWeightVector( Val );

	float3 vCornerLocal[4];
	m_UTangentPoints[0] = CornersU[0];
	m_UTangentPoints[3] = CornersU[1];
	m_UTangentPoints[15] = CornersU[2];
	m_UTangentPoints[12] = CornersU[3];
	vCornerLocal[0] = CornersU[0];
	vCornerLocal[1] = CornersU[1];
	vCornerLocal[2] = CornersU[3];
	vCornerLocal[3] = CornersU[2];
	ComputeTanPatch2( m_UTangentPoints,fCWts,CornersV,vCornerLocal,false );

	//swap Y and W weights
	float temp = fCWts.m_Y;
	fCWts.m_Y = fCWts.m_W;
	fCWts.m_W = temp;

	float3 vCorner[4];
	vCorner[0] = CornersU[0]; //0
	vCorner[1] = CornersU[3];	//3
	vCorner[2] = CornersU[2];	//2
	vCorner[3] = CornersU[1];	//1
	vCornerLocal[0] = CornersV[0];	//0
	vCornerLocal[1] = CornersV[3];	//3
	vCornerLocal[2] = CornersV[1];	//1
	vCornerLocal[3] = CornersV[2];	//2
	m_VTangentPoints[0] = CornersV[0];
	m_VTangentPoints[3] = CornersV[1];
	m_VTangentPoints[15] = CornersV[2];
	m_VTangentPoints[12] = CornersV[3];
	ComputeTanPatch2( m_VTangentPoints,fCWts,vCorner,vCornerLocal,true );
}

#endif//QUAD_PATCHES

