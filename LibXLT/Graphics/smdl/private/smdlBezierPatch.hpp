/****************************************************************************\
**	smdlBezierPatch.hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SMDLBEZIERPATCH_HPP
#error smdlBezierPatch.hpp multiply included
#endif
#define SMDL_SMDLBEZIERPATCH_HPP

#ifndef MA_VECTOR2D_HPP
#include "Core/ma/maVector2d.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif
#ifndef MA_VECTOR4D_HPP
#include "Core/ma/maVector4d.hpp"
#endif
#ifndef SMDL_SUBDIVUTIL_HPP
#include "Graphics/smdl/private/smdlSubdivUtil.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
typedef maVector2d float2;
typedef maVector3d float3;
typedef maVector4d float4;

using namespace smdlSubdivUtil;


//============================================================================
//	quadrilaterial patch
//============================================================================
class BezierPatch
{
	enum MaskType
	{
		ALPHA,
		BETA,
		GAMMA,
	};
	//Control Point Order
	//12,13,14,15,
	// 8, 9,10,11,
	// 4, 5, 6, 7,
	// 0, 1, 2, 3,
public:

	//----------------------------------------------------------------------------
	//Converts the input face to a Bicubic Bezier Patch using ACC algorithm
	//Input face must be a quad
	//----------------------------------------------------------------------------
	bool ConvertFromFace( sFace* pFace );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	const float3 (&GetControlPoints() const)[16]{ return m_ControlPoints; }
	const float3 (&GetUTangentPoints() const)[16]{ return m_UTangentPoints; }
	const float3 (&GetVTangentPoints() const)[16]{ return m_VTangentPoints; }
	const float2 (&GetUV() const)[4]{ return m_UVs;}

	//updates the patch from the cached vertices
	//if the original vertices have changed from animation.
//	void UpdatePatch();

private:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	struct sInternalQuad
	{
		sVert* pVert;
		sVert* pNext;
		sVert* pOther;
		sVert* pPrev;
		int Valence;
	};

	//----------------------------------------------------------------------------
	//These 3 functions compute the 4 points in the quadrant of the patch from the vertex of the quad
	//operates on the 0, 1, 4, 5 control points
	//----------------------------------------------------------------------------
	void ComputeEdgePoints( sFace* pFace, int i_Valence, sVert* pVert, sVert* pNext, sVert* pOther, sVert* pPrev, sEdge* pNextEdge, sEdge* pPrevEdge, bool bNextBound, bool bPrevBound );	//the two adjacent edge points
	void ComputeInteriorPoint( sFace* pFace, int i_Valence, sVert* pVert, sVert* pNext, sVert* pOther, sVert* pPrev, sEdge* pNextEdge, sEdge* pPrevEdge, bool bNextBound, bool bPrevBound );//the interior point
	void ComputeCornerPoint( sFace* pFace, int i_Valence, sVert* pVert, sVert* pNext, sVert* pOther, sVert* pPrev, sEdge* pNextEdge, sEdge* pPrevEdge, bool bNextBound, bool bPrevBound, float3& UTan, float3& VTan, bool bEdgeParity );	//the corner point

	//----------------------------------------------------------------------------
	//Rotates the Control and Tangets points clockwise (increments through corner vertices for CCW faces)
	//This is so we only operate on one quadrant at a time, simplifying redundant code
	//----------------------------------------------------------------------------
	void RotatePoints();
	void RotateTangents();

	//----------------------------------------------------------------------------
	//Calculates the weighted amount for a neighbor vertex for tangents
	// Direct neighbors (shared edges) are alpha's (bBeta = false)
	// Far neighbors (opposite vertex of a shared face) are Beta's. (bBeta = true)
	//----------------------------------------------------------------------------
	float CalcTanMask( int i_Index, int i_Valence, MaskType i_eMask );
	float CalcBoundaryTangentMask( int i_Index, int i_Valence, MaskType i_eMask, bool i_bVTan );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float3 AccumulateEdges( sVert* pVert, int i_Valence, float3& UTan, float3& VTan, bool bEdgeParity );
	float3 AccumulateFaces( sVert* pVert, int i_Valence, float3& UTan, float3& VTan, bool bEdgeParity );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void CalculateUTan( float3 (&UTan)[4] );
	void CalculateVTan( float3 (&VTan)[4] );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float3 ComputeTangent( int i_ID, bool i_bTurn );
	void ComputeTanPatch2( float3 (&vOut)[16], const float4& fCWts, const float3 (&vCorner)[4], const float3 (&vCornerLocal)[4], bool i_bTurn );
	void CalculateTangents( const int (&Val)[4], const float3 (&CornersU)[4], const float3 (&CornersV)[4] );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void BezierRaise( const float3 (&pQ)[3], float3 (&pC)[4]);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float3 ProjectTangent( const float3& Tan, const float3& P);

private:
	float2 m_UVs[4];	//corner uv coordinates
	float3 m_ControlPoints[16];
	float3 m_UTangentPoints[16];
	float3 m_VTangentPoints[16];

//	std::vector<tVert*> m_ControlVertices;	//list of vertices used to generate the patch
};
