/****************************************************************************\
**	mdlTessellatorBase.hpp
**
**	A smdlTessellatorBase subdivides a mesh into a certain number of levels
**	and then maintains information so that it can update the mesh when the
**	base mesh's vertices are animated. 
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SMDLTESSELLATORBASE_HPP
#error smdlTessellatorBase.hpp multiply included
#endif
#define SMDL_SMDLTESSELLATORBASE_HPP

#ifndef MA_VECTOR2D_HPP
#include "Core/ma/maVector2d.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif
#ifndef MA_VECTOR4D_HPP
#include "Core/ma/maVector4d.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
typedef maVector2d float2;
typedef maVector3d float3;
typedef maVector4d float4;


//============================================================================
//============================================================================
struct tVert		//structure to store our tessellated vertices
{
	float3 Position;
	float3 Normal;
	float2 UV;
	float3 Tangent;
	float3 Binormal;

	void ComputeMidpoint( const tVert& a, const tVert& b );
	void Interpolate( const tVert& a, const tVert& b, float t );	//t must be between 0-1, a-b respectively
	void ComputeAverage( const tVert& a, const tVert& b, const tVert& c );
	void ComputeLinearPoint( const tVert& a, const tVert& b, const tVert& c, const maVector3d& i_barycenter );	//barycenter (x,y,z=1-x-y)
/*
	const tVert& operator * (const tVert& a);
	const tVert& operator * (float x);
	const tVert& operator + (const tVert& a);*/
	const tVert& operator = (const tVert& a);
};


//============================================================================
//============================================================================
struct tTri		//structure to store our base mesh triangles
{
	tVert* m_pVert[3];

//	tVert ComputeLinearPoint( maVector3d& i_barycenter );	//x,y,z=1-x-y
	maVector3d ComputeNormal() const;
};


//============================================================================
//============================================================================
struct tQuad		//structure to store our base mesh quadrilaterals
{
	tVert* m_pVert[4];

	//	tVert ComputeLinearPoint( maVector3d& i_barycenter );	//x,y,z=1-x-y
	maVector3d ComputeNormal() const;
};

//normal patch (triangle)


//============================================================================
//============================================================================
class nPatch
{
public:
	tVert a, b, c;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void FillFromTri( const tTri& t, int order );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float3 GetLinearTriangleVertex( const float3& barycenter ) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float3 GetLinearTriangleNormal( const float3& barycenter ) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float3 GetLinearTriangleTangent( const float3& barycenter ) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float3 GetLinearTriangleBinormal( const float3& barycenter ) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float2 GetLinearTriangleTexture( const float3& barycenter ) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float3 GetPNTriangleVertex( const float3& barycenter ) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float3 GetPNTriangleVertexQuad( const float3& barycenter ) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float3 GetPNTriangleNormal( const float3& barycenter ) const;

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	tVert ComputeTessVertex( const float3& barycenter ) const;

	//Control Point Order
	//b300, b030, b003, b102, b210, b021, b120, b012, b201, b111
	void CalculateControlPoints();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	const std::vector<float3>& GetControlPoints() const{ return m_ControlPoints; }

private:
	std::vector<float3> m_ControlPoints;
};


//============================================================================
//	class used to store and convert from subdivision surface quads to bicubic patches
//============================================================================
class bPatch
{
public:
	typedef std::vector<tVert*> ListType;
	enum PatchType
	{
		Ordinary,
		Extraordinary,
		Discontinuous,
	};

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	ListType& GetNeighborhood(){ return m_Neighborhood; }

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
//	void AddUnique( int i_Index );
	void AddUnique( tVert* i_Vert );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	int  size(){ return m_Neighborhood.size(); }

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void SetValence( int i_Vert, int i_Count ){ m_Valence[ i_Vert ] = i_Count; }
	int  GetValence( int i_Vert ){ return m_Valence[ i_Vert ]; }
	const int (&Valences())[4]{ return m_Valence; }

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void SetPrefix( int i_Vert, int i_Prefix ){ m_Prefix[ i_Vert ] = i_Prefix; }
	int  GetPrefix( int i_Vert ){ return m_Prefix[ i_Vert ]; }
	const int (&Prefixes())[4]{ return m_Prefix; }

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void SetBoundaryEdge( int i_Vert, bool i_bBoundary ){ m_EdgeBoundary[ i_Vert ] = i_bBoundary; }
	bool GetBoundaryEdge( int i_Vert ){ return m_EdgeBoundary[ i_Vert ]; }
	const bool (&BoundaryEdges())[4]{ return m_EdgeBoundary; }

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void SetBoundaryVertex( int i_Vert, bool i_bBoundary ){ m_VertexBoundary[ i_Vert ] = i_bBoundary; }
	bool GetBoundaryVertex( int i_Vert ){ return m_VertexBoundary[ i_Vert ]; }
	const bool (&BoundaryVertices())[4]{ return m_VertexBoundary; }

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void SetType( PatchType i_Type ){ m_Type = i_Type; }
	PatchType GetType(){ return m_Type; }

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool IsOrdinary();

private:
	ListType m_Neighborhood;	//first 4 are the interior quad, outside is the perimeter adjacent vertices.
	int m_Valence[4];			//Edges connected to each corner.
	int m_Prefix[4];			//Index of last vertex sequence for each corner.
	bool m_EdgeBoundary[4];		//Edge is a boundary, has no neighbor face. (first edge is vertex 0-1 in sequence)
	bool m_VertexBoundary[4];	//Vertex is on a boundary, one or more shared edges is a boundary. (it's possible to have a boundary vertex but no edge boundary)
	PatchType m_Type;
};


//============================================================================
//============================================================================
extern float g_scale;


//============================================================================
//quadrilateral patch
//============================================================================
class qPatch
{
private:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
//	static float s_TangentStencils[1024];
//	static float s_TangentWeights[16];
	struct CornerPoint
	{
		float3 P;
		float3 U;
		float3 V;
		unsigned char Valence;

		void Accumulate( bPatch::ListType& Vert, unsigned int i_Index, float i_Weight, unsigned int i_TUindex, unsigned int i_TVOffset, bool i_bInv );
		float CalcTanMask( unsigned int i_CPIndex, bool bInv );
	};

public:
	tVert* a;
	tVert* b;
	tVert* c;
	tVert* d;
/*
	void FillFromQuad( const tQuad& t, int order );
	float3 GetLinearQuadVertex( const float4& barycenter ) const;
	float3 GetLinearQuadNormal( const float4& barycenter ) const;
	float3 GetLinearQuadTangent( const float4& barycenter ) const;
	float3 GetLinearQuadBinormal( const float4& barycenter ) const;
	float2 GetLinearQuadTexture( const float4& barycenter ) const;
	float3 GetPNQuadVertex( const float4& barycenter ) const;
	float3 GetPNQuadVertexQuad( const float4& barycenter ) const;
	float3 GetPNQuadNormal( const float4& barycenter ) const;
	tVert ComputeTessVertex( const float4& barycenter ) const;

	void CalculateControlPoints();
*/

	//Control Point Order
	//12,13,14,15,
	// 8, 9,10,11,
	// 4, 5, 6, 7,
	// 0, 1, 2, 3,

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void ConvertFromPatch( bPatch& i_Patch );	//calculates control points from the patch

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	const float3 (&GetControlPoints() const)[16]{ return m_ControlPoints; }

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	const float3 (&GetUTangentPoints() const)[16]{ return m_UTangentPoints; }

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	const float3 (&GetVTangentPoints() const)[16]{ return m_VTangentPoints; }

//	static void CalculateConstants();

private:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void ComputeEdgePoints( bPatch::ListType& Vert, const int (&Val)[4], const int (&Pref)[4], const bool (&Bound)[4] );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void ComputeCornerPoints( bPatch::ListType& Vert, const int (&Val)[4], const int (&Pref)[4], const bool (&Bound)[4], float3 (&CornersU)[4], float3 (&CornersV)[4]);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void ComputeInteriorPoints( bPatch::ListType& Vert, const int (&Val)[4], const bool (&Bound)[4] );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void ComputeTanPatch( float3 (&vOut)[16], const float4& fCWts, const float3 (&vCorner)[4], const float3 (&vCornerLocal)[4], const unsigned int cX, const unsigned int cY);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void ComputeTanPatch2( float3 (&vOut)[16], const float4& fCWts, const float3 (&vCorner)[4], const float3 (&vCornerLocal)[4], bool i_bTurn );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float3 ComputeTangent( unsigned int i_ID, bool i_bTurn );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	float4 CalculateWeightVector( const int (&Val)[4] );

private:
	float3 m_ControlPoints[16];
	float3 m_UTangentPoints[16];
	float3 m_VTangentPoints[16];
};
