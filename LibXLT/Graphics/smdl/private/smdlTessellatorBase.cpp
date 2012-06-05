/*****************************************************************************
**	smdlTessellatorBase.cpp
**
**		see .hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/smdl/private/smdlTessellatorBase.hpp"

#include "Core/Ma/maConstants.hpp"


//============================================================================
//============================================================================
#define NEW_SUB


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tVert::ComputeMidpoint( const tVert& a, const tVert& b )
{
	Position = (a.Position + b.Position) * 0.5f;
	Normal = (a.Normal + b.Normal) * 0.5f;
	UV = (a.UV + b.UV) * 0.5f;
	Tangent = (a.Tangent + b.Tangent) * 0.5f;
	Binormal = (a.Binormal + b.Binormal) * 0.5f;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tVert::Interpolate( const tVert& a, const tVert& b, float t )
{
	float it = 1-t;
	Position = a.Position*it + b.Position*t;
	Normal = a.Normal*it + b.Normal*t;
	UV = a.UV*it + b.UV*t;
	Tangent = a.Tangent*it + b.Tangent*t;
	Binormal = a.Binormal*it + b.Binormal*t;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tVert::ComputeAverage( const tVert& a, const tVert& b, const tVert& c )
{
	Position = (a.Position + b.Position + c.Position) / 3;
	Normal = (a.Normal + b.Normal + c.Normal) / 3;
	UV = (a.UV + b.UV + c.UV) / 3;
	Tangent = (a.Tangent + b.Tangent + c.Tangent) / 3;
	Binormal = (a.Binormal + b.Binormal + c.Binormal) / 3;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tVert::ComputeLinearPoint( const tVert& a, const tVert& b, const tVert& c, const maVector3d& i_barycenter )
{
	Position  = a.Position  * i_barycenter.m_X + b.Position  * i_barycenter.m_Y + c.Position  * i_barycenter.m_Z;
	Normal = a.Normal * i_barycenter.m_X + b.Normal * i_barycenter.m_Y + c.Normal * i_barycenter.m_Z;
	UV   = a.UV   * i_barycenter.m_X + b.UV   * i_barycenter.m_Y + c.UV   * i_barycenter.m_Z;
	Tangent    = a.Tangent    * i_barycenter.m_X + b.Tangent    * i_barycenter.m_Y + c.Tangent    * i_barycenter.m_Z;
	Binormal    = a.Binormal    * i_barycenter.m_X + b.Binormal    * i_barycenter.m_Y + c.Binormal    * i_barycenter.m_Z;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const tVert& tVert::operator = (const tVert& a)
{
	Position = a.Position;
	Normal = a.Normal;
	UV = a.UV;
	Tangent = a.Tangent;
	Binormal = a.Binormal;
	return *this;
}

/*
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const tVert& tVert::operator * (const tVert& a)
{
//	Position = Position * a.Position;
	Normal = Normal * a.Normal;
	UV = UV * a.UV;
	Tangent = Tangent* a.Tangent;
	Binormal = Binormal * a.Binormal;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const tVert& tVert::operator * (float x)
{
	Position = Position * x;
	Normal = Normal * x;
	UV = UV * x;
	Tangent = Tangent * x;
	Binormal = Binormal * x;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const tVert& tVert::operator + (const tVert& a)
{

	Position = Position + a.Position;
	Normal = Normal + a.Normal;
	UV = UV + a.UV;
	Tangent = Tangent + a.Tangent;
	Binormal = Binormal + a.Binormal;

	return *this;
}

/*
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tVert tTri::ComputeLinearPoint( maVector3d& i_barycenter )	//x,y,z=1-x-y
{
	tVert V;

	V.ComputeLinearPoint( *m_pVert[0], *m_pVert[1], *m_pVert[2], i_barycenter );

	return V;
}
*/

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
maVector3d tTri::ComputeNormal() const
{
	maVector3d norm = (m_pVert[1]->Position - m_pVert[0]->Position) / (m_pVert[2]->Position - m_pVert[0]->Position);
	norm.Normalize();
	return norm;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
maVector3d tQuad::ComputeNormal() const
{
	maVector3d norm = (m_pVert[1]->Position - m_pVert[0]->Position) / (m_pVert[2]->Position - m_pVert[0]->Position);
	norm.Normalize();
	return norm;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void nPatch::FillFromTri(const tTri& t, int order )
{
	int x, y, z;
	switch( order )
	{
		case 0:{ x = 2; y = 1; z = 0; break;}
		case 1:{ x = 0; y = 2; z = 1; break;}
		case 2:{ x = 1; y = 0; z = 2; break;}
		case 3:{ x = 0; y = 1; z = 2; break;}
	}

	a.Position = t.m_pVert[x]->Position;
	b.Position = t.m_pVert[y]->Position;
	c.Position = t.m_pVert[z]->Position;

	a.UV = t.m_pVert[x]->UV;
	b.UV = t.m_pVert[y]->UV;
	c.UV = t.m_pVert[z]->UV;

	a.Normal = t.m_pVert[x]->Normal;
	b.Normal = t.m_pVert[y]->Normal;
	c.Normal = t.m_pVert[z]->Normal;

	a.Tangent = t.m_pVert[x]->Tangent;
	b.Tangent = t.m_pVert[y]->Tangent;
	c.Tangent = t.m_pVert[z]->Tangent;

	a.Binormal = t.m_pVert[x]->Binormal;
	b.Binormal = t.m_pVert[y]->Binormal;
	c.Binormal = t.m_pVert[z]->Binormal;
}

//----------------------------------------------------------------------------
//---------------Linear Interpolation----------------
//----------------------------------------------------------------------------
float3 nPatch::GetLinearTriangleVertex( const float3& barycenter ) const
{
	//trilinear interpolate
	return a.Position * barycenter.m_X + b.Position * barycenter.m_Y + c.Position * barycenter.m_Z;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float3 nPatch::GetLinearTriangleNormal( const float3& barycenter ) const
{
	//trilinear interpolate
	float3 V = a.Normal * barycenter.m_X + b.Normal * barycenter.m_Y + c.Normal * barycenter.m_Z;
	V.Normalize();
	return V;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float3 nPatch::GetLinearTriangleTangent( const float3& barycenter ) const
{
	//trilinear interpolate
	float3 V = a.Tangent * barycenter.m_X + b.Tangent * barycenter.m_Y + c.Tangent * barycenter.m_Z;
	V.Normalize();
	return V;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float3 nPatch::GetLinearTriangleBinormal( const float3& barycenter ) const
{
	//trilinear interpolate
	float3 V = a.Binormal * barycenter.m_X + b.Binormal * barycenter.m_Y + c.Binormal * barycenter.m_Z;
	V.Normalize();
	return V;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float2 nPatch::GetLinearTriangleTexture( const float3& barycenter ) const
{
	//trilinear interpolate
	return a.UV * barycenter.m_X + b.UV * barycenter.m_Y + c.UV * barycenter.m_Z;
}

//----------------------------------------------------------------------------
//---------------Cubic Interpolation----------------
//----------------------------------------------------------------------------
float3 nPatch::GetPNTriangleVertex( const float3& barycenter ) const
{
	float3 Vert = maVector3d(0,0,0);

	// Tricubic Bezier Triangle Patch
	//               3
	//   B(u,v,w) = SUM( Bijk * u^i * v^j * w^k * (3!/(i!*j!*k!)) )
	//             i=j=k=0
	//   w=1-u-v

	//Parametric terms
	float3 I = barycenter;
	float3 I2 = I;
	I2.Mul( I );
	float3 I3 = I2;
	I3.Mul( I );

	//coordinates
	float3 P1 = a.Position;
	float3 P2 = b.Position;
	float3 P3 = c.Position;
	float3 N1 = a.Normal;
	float3 N2 = b.Normal;
	float3 N3 = c.Normal;

	//tangent scalars
	float s12 = N1.Dot(P2 - P1);
	float s21 = N2.Dot(P1 - P2);
	float s23 = N2.Dot(P3 - P2);
	float s32 = N3.Dot(P2 - P3);
	float s31 = N3.Dot(P1 - P3);
	float s13 = N1.Dot(P3 - P1);

	//cubic bezier cooeficients
	float3 b210 = (2 * P1 + P2 - s12 * N1);      //Tangent Coefficients
	float3 b120 = (2 * P2 + P1 - s21 * N2);
	float3 b021 = (2 * P2 + P3 - s23 * N2);
	float3 b012 = (2 * P3 + P2 - s32 * N3);
	float3 b102 = (2 * P3 + P1 - s31 * N3);
	float3 b201 = (2 * P1 + P3 - s13 * N1);

	float3 b111 = ((b210 + b120 + b021 + b012 + b102 + b201)/2) - (P1 + P2 + P3);   //Center Coefficient

	//displacement field - 10D vector product (less instructions but uglier)
	Vert = (I3.m_X) * P1 + (I3.m_Y) * P2 + (I3.m_Z) * P3 +
		(I.m_X*I2.m_Z) * b102 + (I.m_Y*I2.m_X) * b210 + (I.m_Z*I2.m_Y) * b021 +
		(I.m_X*I2.m_Y) * b120 + (I.m_Y*I2.m_Z) * b012 + (I.m_Z*I2.m_X) * b201 +
		(I.m_X*I.m_Y*I.m_Z) * b111;   

	return Vert;
}

//----------------------------------------------------------------------------
//---------------Cubic Interpolation----------------
//----------------------------------------------------------------------------
float3 nPatch::GetPNTriangleVertexQuad( const float3& barycenter ) const
{
	float3 Vert = maVector3d(0,0,0);

	// TriQuadratic Bezier Triangle Patch
	//               2
	//   B(u,v,w) = SUM( Bijk * u^i * v^j * w^k * (2!/(i!*j!*k!)) )
	//             i=j=k=0
	//   w=1-u-v

	//Parametric terms
	float x = barycenter.m_X;
	float x2 = x * x;
	float y = barycenter.m_Y;
	float y2 = y * y;
	float z = barycenter.m_Z;
	float z2 = z * z;

	//coordinates
	float3 P1 = a.Position;
	float3 P2 = b.Position;
	float3 P3 = c.Position;
	float3 N1 = a.Normal;
	float3 N2 = b.Normal;
	float3 N3 = c.Normal;

	//edge scalars
	float3 Va = (P2 + P1) / 2;
	float3 Vb = (P3 + P2) / 2;
	float3 Vc = (P1 + P3) / 2;
	//tangent scalars
	float s12 = N1.Dot(Va - P1)/2;
	float s21 = N2.Dot(Va - P2)/2;
	float s23 = N2.Dot(Vb - P2)/2;
	float s32 = N3.Dot(Vb - P3)/2;
	float s31 = N3.Dot(Vc - P3)/2;
	float s13 = N1.Dot(Vc - P1)/2;

	//quadratic Bezier coefficients
	float3 b110 = Va - ((s12 * N1) + (s21 * N2));
	float3 b011 = Vb - ((s23 * N2) + (s32 * N3));
	float3 b101 = Vc - ((s31 * N3) + (s13 * N1));

	//displacement field - 10D vector product (less instructions but uglier)
	Vert = P1*x2 + P1*y2 + P3*z2 +
		   2*x*y*b110 + 2*y*z*b011 + 2*x*z*b101;

	return Vert;
}


//----------------------------------------------------------------------------
//-------------Quadratic Interpolation
//----------------------------------------------------------------------------
float3 nPatch::GetPNTriangleNormal( const float3& barycenter ) const
{
	float3 Norm = float3(0,0,0);

	// triquadratic Bezier Triangle Patch
	//               2
	//   N(u,v,w) = SUM( Vijk * u^i * v^j * w^k )
	//            i=j=k=0
	//   w=1-u-v

	//Parametric terms
	float x = barycenter.m_X;
	float x2 = x * x;
	float y = barycenter.m_Y;
	float y2 = y * y;
	float z = barycenter.m_Z;
	float z2 = z * z;

	//coordinates
	float3 P1 = a.Position;
	float3 P2 = b.Position;
	float3 P3 = c.Position;
	float3 N1 = a.Normal;
	float3 N2 = b.Normal;
	float3 N3 = c.Normal;

	//edge scalars
	float3 Va = P2 - P1;
	float3 Vb = P3 - P2;
	float3 Vc = P1 - P3;
	float v12 = (Va.Dot( N2+N1 ) / Va.Dot( Va )) * 4;
	float v23 = (Vb.Dot( N3+N2 ) / Vb.Dot( Vb )) * 4;
	float v31 = (Vc.Dot( N1+N3 ) / Vc.Dot( Vc )) * 4;

	//quadratic bezier cooeficients
	float3 n110 = (N1 + N2 - v12 * (P2 - P1));
	float3 n011 = (N2 + N3 - v23 * (P3 - P2));
	float3 n101 = (N3 + N1 - v31 * (P1 - P3));

	//displacement field
	Norm = N1 * x2 + N2 * y2 + N3 * z2 + n110 * x * y + n011 * y * z + n101 * x * z;
	Norm.Normalize();

	return Norm;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tVert nPatch::ComputeTessVertex( const float3& barycenter ) const
{
	tVert Out;

/*	if ( l_bFlatTessellate )
	{
		Out.Position = GetLinearTriangleVertex( barycenter );
	}
	else
	{*/
		Out.Position = GetPNTriangleVertex( barycenter );
//	}
//	Out.Position = GetPNTriangleVertexQuad( barycenter );
	Out.UV       = GetLinearTriangleTexture( barycenter );
	Out.Normal   = GetLinearTriangleNormal( barycenter );
//	Out.Normal   = GetPNTriangleNormal( barycenter );
	Out.Tangent  = GetLinearTriangleTangent( barycenter );
	Out.Binormal = GetLinearTriangleBinormal( barycenter );

	return Out;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void nPatch::CalculateControlPoints()
{
	float3 P1 = a.Position;
	float3 P2 = b.Position;
	float3 P3 = c.Position;
	float3 N1 = a.Normal;
	float3 N2 = b.Normal;
	float3 N3 = c.Normal;

	//cubic mesh
	//tangent scalars
	float s12 = N1.Dot(P2 - P1);
	float s21 = N2.Dot(P1 - P2);
	float s23 = N2.Dot(P3 - P2);
	float s32 = N3.Dot(P2 - P3);
	float s31 = N3.Dot(P1 - P3);
	float s13 = N1.Dot(P3 - P1);

	//cubic Bezier coefficients
	float3 b210 = (2 * P1 + P2 - s12 * N1)/3;      //Tangent Coefficients
	float3 b120 = (2 * P2 + P1 - s21 * N2)/3;
	float3 b021 = (2 * P2 + P3 - s23 * N2)/3;
	float3 b012 = (2 * P3 + P2 - s32 * N3)/3;
	float3 b102 = (2 * P3 + P1 - s31 * N3)/3;
	float3 b201 = (2 * P1 + P3 - s13 * N1)/3;

	float3 b111 = ((b210 + b120 + b021 + b012 + b102 + b201)/4) - ((P1 + P2 + P3)/6);   //Center Coefficient

	m_ControlPoints.resize( 10 );
	m_ControlPoints[ 0 ] = P1;

	m_ControlPoints[ 1 ] = b201;
	m_ControlPoints[ 2 ] = b210;

	m_ControlPoints[ 3 ] = b102;
	m_ControlPoints[ 4 ] = b111;
	m_ControlPoints[ 5 ] = b120;

	m_ControlPoints[ 6 ] = P3;
	m_ControlPoints[ 7 ] = b012;
	m_ControlPoints[ 8 ] = b021;
	m_ControlPoints[ 9 ] = P2;

/*
	//edge scalars
	float3 Va = (P2 + P1) / 2;
	float3 Vb = (P3 + P2) / 2;
	float3 Vc = (P1 + P3) / 2;
	//tangent scalars
	float s12 = N1.Dot(Va - P1)/2;
	float s21 = N2.Dot(Va - P2)/2;
	float s23 = N2.Dot(Vb - P2)/2;
	float s32 = N3.Dot(Vb - P3)/2;
	float s31 = N3.Dot(Vc - P3)/2;
	float s13 = N1.Dot(Vc - P1)/2;

	//quadratic Bezier coefficients
	float3 b110 = Va - ((s12 * N1) + (s21 * N2));
	float3 b011 = Vb - ((s23 * N2) + (s32 * N3));
	float3 b101 = Vc - ((s31 * N3) + (s13 * N1));

	m_ControlPoints.resize( 6 );
	m_ControlPoints[ 0 ] = P1;

	m_ControlPoints[ 1 ] = b101;
	m_ControlPoints[ 2 ] = b110;

	m_ControlPoints[ 3 ] = P3;
	m_ControlPoints[ 4 ] = b011;
	m_ControlPoints[ 5 ] = P2;
*/
}

//----------------------------------------------------------------------------
//--------------quads------------------------------
//----------------------------------------------------------------------------
float4 qPatch::CalculateWeightVector( const int (&Val)[4] )
{
	float4 weights;
	for( int i = 0; i < 4; i++ )
	{
		weights[i] = cos( maConstants::c_fPI_Times_2 / (float)Val[i]);
	}
	return weights;
}

#ifndef NEW_SUB
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void qPatch::FillFromQuad(const tQuad& t, int order )
{
	int x, y, z, w = 3;
	switch( order )
	{
		case 0:{ x = 2; y = 1; z = 0; break;}
		case 1:{ x = 0; y = 2; z = 1; break;}
		case 2:{ x = 1; y = 0; z = 2; break;}
		case 3:{ x = 0; y = 1; z = 2; w = 3; break;}
	}

	a.Position = t.m_pVert[x]->Position;
	b.Position = t.m_pVert[y]->Position;
	c.Position = t.m_pVert[z]->Position;
	d.Position = t.m_pVert[w]->Position;

	a.UV = t.m_pVert[x]->UV;
	b.UV = t.m_pVert[y]->UV;
	c.UV = t.m_pVert[z]->UV;
	d.UV = t.m_pVert[w]->UV;

	a.Normal = t.m_pVert[x]->Normal;
	b.Normal = t.m_pVert[y]->Normal;
	c.Normal = t.m_pVert[z]->Normal;
	d.Normal = t.m_pVert[w]->Normal;

	a.Tangent = t.m_pVert[x]->Tangent;
	b.Tangent = t.m_pVert[y]->Tangent;
	c.Tangent = t.m_pVert[z]->Tangent;
	d.Tangent = t.m_pVert[w]->Tangent;

	a.Binormal = t.m_pVert[x]->Binormal;
	b.Binormal = t.m_pVert[y]->Binormal;
	c.Binormal = t.m_pVert[z]->Binormal;
	d.Binormal = t.m_pVert[w]->Binormal;
}

//----------------------------------------------------------------------------
//---------------Linear Interpolation----------------
//----------------------------------------------------------------------------
float3 qPatch::GetLinearQuadVertex( const float4& barycenter ) const
{
	//quadlinear interpolate
	return a.Position * barycenter.m_X + b.Position * barycenter.m_Y + c.Position * barycenter.m_Z + d.Position * barycenter.m_W;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float3 qPatch::GetLinearQuadNormal( const float4& barycenter ) const
{
	//quadlinear interpolate
	float3 V = a.Normal * barycenter.m_X + b.Normal * barycenter.m_Y + c.Normal * barycenter.m_Z + d.Normal * barycenter.m_W;
	V.Normalize();
	return V;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float3 qPatch::GetLinearQuadTangent( const float4& barycenter ) const
{
	//quadlinear interpolate
	float3 V = a.Tangent * barycenter.m_X + b.Tangent * barycenter.m_Y + c.Tangent * barycenter.m_Z + d.Tangent * barycenter.m_W;
	V.Normalize();
	return V;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float3 qPatch::GetLinearQuadBinormal( const float4& barycenter ) const
{
	//quadlinear interpolate
	float3 V = a.Binormal * barycenter.m_X + b.Binormal * barycenter.m_Y + c.Binormal * barycenter.m_Z + d.Binormal * barycenter.m_W;
	V.Normalize();
	return V;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float2 qPatch::GetLinearQuadTexture( const float4& barycenter ) const
{
	//quadlinear interpolate
	return a.UV * barycenter.m_X + b.UV * barycenter.m_Y + c.UV * barycenter.m_Z + d.UV * barycenter.m_W;
}

//----------------------------------------------------------------------------
//---------------Cubic Interpolation----------------
//----------------------------------------------------------------------------
float3 qPatch::GetPNQuadVertex( const float4& barycenter ) const
{
	float3 Vert = maVector3d(0,0,0);

	// Tricubic Bezier Triangle Patch
	//               3
	//   B(u,v,w) = SUM( Bijk * u^i * v^j * w^k * (3!/(i!*j!*k!)) )
	//             i=j=k=0
	//   w=1-u-v
/*
	//Parametric terms
	float3 I = barycenter;
	float3 I2 = I;
	I2.Mul( I );
	float3 I3 = I2;
	I3.Mul( I );

	//coordinates
	float3 P1 = a.Position;
	float3 P2 = b.Position;
	float3 P3 = c.Position;
	float3 N1 = a.Normal;
	float3 N2 = b.Normal;
	float3 N3 = c.Normal;

	//tangent scalars
	float s12 = N1.Dot(P2 - P1);
	float s21 = N2.Dot(P1 - P2);
	float s23 = N2.Dot(P3 - P2);
	float s32 = N3.Dot(P2 - P3);
	float s31 = N3.Dot(P1 - P3);
	float s13 = N1.Dot(P3 - P1);

	//cubic bezier cooeficients
	float3 b210 = (2 * P1 + P2 - s12 * N1);      //Tangent Coefficients
	float3 b120 = (2 * P2 + P1 - s21 * N2);
	float3 b021 = (2 * P2 + P3 - s23 * N2);
	float3 b012 = (2 * P3 + P2 - s32 * N3);
	float3 b102 = (2 * P3 + P1 - s31 * N3);
	float3 b201 = (2 * P1 + P3 - s13 * N1);

	float3 b111 = ((b210 + b120 + b021 + b012 + b102 + b201)/2) - (P1 + P2 + P3);   //Center Coefficient

	//displacement field - 10D vector product (less instructions but uglier)
	Vert = (I3.m_X) * P1 + (I3.m_Y) * P2 + (I3.m_Z) * P3 +
		(I.m_X*I2.m_Z) * b102 + (I.m_Y*I2.m_X) * b210 + (I.m_Z*I2.m_Y) * b021 +
		(I.m_X*I2.m_Y) * b120 + (I.m_Y*I2.m_Z) * b012 + (I.m_Z*I2.m_X) * b201 +
		(I.m_X*I.m_Y*I.m_Z) * b111;   
*/
	return Vert;
}

//----------------------------------------------------------------------------
//---------------Cubic Interpolation----------------
//----------------------------------------------------------------------------
float3 qPatch::GetPNQuadVertexQuad( const float4& barycenter ) const
{
	float3 Vert = maVector3d(0,0,0);

	// TriQuadratic Bezier Triangle Patch
	//               2
	//   B(u,v,w) = SUM( Bijk * u^i * v^j * w^k * (2!/(i!*j!*k!)) )
	//             i=j=k=0
	//   w=1-u-v
/*
	//Parametric terms
	float x = barycenter.m_X;
	float x2 = x * x;
	float y = barycenter.m_Y;
	float y2 = y * y;
	float z = barycenter.m_Z;
	float z2 = z * z;

	//coordinates
	float3 P1 = a.Position;
	float3 P2 = b.Position;
	float3 P3 = c.Position;
	float3 N1 = a.Normal;
	float3 N2 = b.Normal;
	float3 N3 = c.Normal;

	//edge scalars
	float3 Va = (P2 + P1) / 2;
	float3 Vb = (P3 + P2) / 2;
	float3 Vc = (P1 + P3) / 2;
	//tangent scalars
	float s12 = N1.Dot(Va - P1)/2;
	float s21 = N2.Dot(Va - P2)/2;
	float s23 = N2.Dot(Vb - P2)/2;
	float s32 = N3.Dot(Vb - P3)/2;
	float s31 = N3.Dot(Vc - P3)/2;
	float s13 = N1.Dot(Vc - P1)/2;

	//quadratic Bezier coefficients
	float3 b110 = Va - ((s12 * N1) + (s21 * N2));
	float3 b011 = Vb - ((s23 * N2) + (s32 * N3));
	float3 b101 = Vc - ((s31 * N3) + (s13 * N1));

	//displacement field - 10D vector product (less instructions but uglier)
	Vert = P1*x2 + P1*y2 + P3*z2 +
		2*x*y*b110 + 2*y*z*b011 + 2*x*z*b101;
*/
	return Vert;
}


//----------------------------------------------------------------------------
//-------------Quadratic Interpolation
//----------------------------------------------------------------------------
float3 qPatch::GetPNQuadNormal( const float4& barycenter ) const
{
	float3 Norm = float3(0,0,0);

	// triquadratic Bezier Triangle Patch
	//               2
	//   N(u,v,w) = SUM( Vijk * u^i * v^j * w^k )
	//            i=j=k=0
	//   w=1-u-v
/*
	//Parametric terms
	float x = barycenter.m_X;
	float x2 = x * x;
	float y = barycenter.m_Y;
	float y2 = y * y;
	float z = barycenter.m_Z;
	float z2 = z * z;

	//coordinates
	float3 P1 = a.Position;
	float3 P2 = b.Position;
	float3 P3 = c.Position;
	float3 N1 = a.Normal;
	float3 N2 = b.Normal;
	float3 N3 = c.Normal;

	//edge scalars
	float3 Va = P2 - P1;
	float3 Vb = P3 - P2;
	float3 Vc = P1 - P3;
	float v12 = (Va.Dot( N2+N1 ) / Va.Dot( Va )) * 4;
	float v23 = (Vb.Dot( N3+N2 ) / Vb.Dot( Vb )) * 4;
	float v31 = (Vc.Dot( N1+N3 ) / Vc.Dot( Vc )) * 4;

	//quadratic bezier cooeficients
	float3 n110 = (N1 + N2 - v12 * (P2 - P1));
	float3 n011 = (N2 + N3 - v23 * (P3 - P2));
	float3 n101 = (N3 + N1 - v31 * (P1 - P3));

	//displacement field
	Norm = N1 * x2 + N2 * y2 + N3 * z2 + n110 * x * y + n011 * y * z + n101 * x * z;
	Norm.Normalize();
*/
	return Norm;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tVert qPatch::ComputeTessVertex( const float4& barycenter ) const
{
	tVert Out;

	Out.Position = GetLinearQuadVertex( barycenter );
/*	if ( l_bFlatTessellate )
	{
		Out.Position = GetLinearQuadVertex( barycenter );
	}
	else
	{
		Out.Position = GetPNQuadVertex( barycenter );
	}*/
	//	Out.Position = GetPNTriangleVertexQuad( barycenter );
	Out.UV       = GetLinearQuadTexture( barycenter );
	Out.Normal   = GetLinearQuadNormal( barycenter );
	//	Out.Normal   = GetPNQuadNormal( barycenter );
	Out.Tangent  = GetLinearQuadTangent( barycenter );
	Out.Binormal = GetLinearQuadBinormal( barycenter );

	return Out;
}
#endif//!NEW_SUB

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline const float4 BarycenterFromUV( const float& u, const float& v )
{
	return float4( (1-u)*(1-v), u*(1-v), u*v, (1-u)*v );
}


//----------------------------------------------------------------------------
//project a point P into the plane defined by it Normal passing through Center
//projection is the smallest distance
//----------------------------------------------------------------------------
inline const float3 ProjectToPlane( const float3& Point, const float3& Normal, const float3& Center )
{
	return Point - Normal.Dot(Point - Center) * Normal;
}

#ifndef NEW_SUB
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void qPatch::CalculateControlPoints()
{
	//coordinates
	float3 P1 = a.Position;
	float3 P2 = b.Position;
	float3 P3 = c.Position;
	float3 P4 = d.Position;
	float3 N1 = a.Normal;
	float3 N2 = b.Normal;
	float3 N3 = c.Normal;
	float3 N4 = d.Normal;

	//cubic bezier cooeficients
	//control points	(16), C = corners, E = Edges, I = Interior
	float3 P11 = P1;	//C
	float3 P21 = (2 * P1 + P2 - N1.Dot(P2 - P11) * N1)/3;	//E
	float3 P31 = (2 * P2 + P1 - N2.Dot(P1 - P2) * N2)/3;	//E
	float3 P41 = P2;	//C		
	float3 P12 = (2 * P1 + P4 - N1.Dot(P4 - P1) * N1)/3;	//E
	float3 P42 = (2 * P2 + P3 - N2.Dot(P3 - P2) * N2)/3;	//E
	float3 P13 = (2 * P4 + P1 - N4.Dot(P1 - P4) * N4)/3;	//E
	float3 P43 = (2 * P3 + P2 - N3.Dot(P2 - P3) * N3)/3;	//E
	float3 P14 = P4;	//C		
	float3 P24 = (2 * P4 + P3 - N4.Dot(P3 - P4) * N4)/3;	//E
	float3 P34 = (2 * P3 + P4 - N3.Dot(P4 - P3) * N3)/3;	//E
	float3 P44 = P3;	//C

	//interior points weighted average of corner points
	const float4 V = float4(4,4,4,4);		//all vertices are valence 4
	float3 P23 = (P1*V.m_X + P2*2   + P4     + P3*2)   / (5+V.m_X);
	float3 P33 = (P1*2   + P2*V.m_Y + P4*2   + P3)     / (5+V.m_Y);
	float3 P22 = (P1     + P2*2   + P4*V.m_Z + P3*2)   / (5+V.m_Z);
	float3 P32 = (P1*2   + P2     + P4*2   + P3*V.m_W) / (5+V.m_W);

	m_ControlPoints[ 0 ] = P11;
	m_ControlPoints[ 1 ] = P21;
	m_ControlPoints[ 2 ] = P31;
	m_ControlPoints[ 3 ] = P41;

	m_ControlPoints[ 4 ] = P12;
	m_ControlPoints[ 5 ] = P22;
	m_ControlPoints[ 6 ] = P32;
	m_ControlPoints[ 7 ] = P42;

	m_ControlPoints[ 8 ] = P13;
	m_ControlPoints[ 9 ] = P23;
	m_ControlPoints[ 10 ] = P33;
	m_ControlPoints[ 11 ] = P43;

	m_ControlPoints[ 12 ] = P14;
	m_ControlPoints[ 13 ] = P24;
	m_ControlPoints[ 14 ] = P34;
	m_ControlPoints[ 15 ] = P44;
}
#endif//!NEW_SUB

//--------------------------------------------------------------------------------------
// Computes the interior vertices of the output bicubic patch.  The interior vertices
// (5,6,10,9) are a weighted combination of interior 4 vertices of the
// subdivision patch.
//--------------------------------------------------------------------------------------
void qPatch::ComputeInteriorPoints( bPatch::ListType& Vert, const int (&Val)[4], const bool (&Bound)[4] )
{
	// Precompute some weight values that we use multiple times below
	float a = Val[0];
	float b = Val[1];
	float c = Val[2];
	float d = Val[3];

	if ( Bound[0] && Bound[3] ) a = 4;
	else if ( Bound[0] || Bound[3] ) a = (a-1)*2;

	if ( Bound[1] && Bound[0] ) b = 4;
	else if ( Bound[1] || Bound[0] ) b = (b-1)*2;

	if ( Bound[2] && Bound[1] ) c = 4;
	else if ( Bound[2] || Bound[1] ) c = (c-1)*2;

	if ( Bound[3] && Bound[2] ) d = 4;
	else if ( Bound[3] || Bound[2] ) d = (d-1)*2;

	m_ControlPoints[ 5 ]  = (a*Vert[0]->Position + 2*Vert[1]->Position + Vert[2]->Position + 2*Vert[3]->Position) / (a+5);
	m_ControlPoints[ 6 ]  = (2*Vert[0]->Position + b*Vert[1]->Position + 2*Vert[2]->Position + Vert[3]->Position) / (b+5);
	m_ControlPoints[ 10 ] = (Vert[0]->Position + 2*Vert[1]->Position + c*Vert[2]->Position + 2*Vert[3]->Position) / (c+5);
	m_ControlPoints[ 9 ]  = (2*Vert[0]->Position + Vert[1]->Position + 2*Vert[2]->Position + d*Vert[3]->Position) / (d+5);
}

//--------------------------------------------------------------------------------------
// Computes the edge vertices of the output bicubic patch.  The edge vertices
// (1,2,13,14,4,8,7,11) are a weighted (by valence) combination of 6 interior and 1-ring
// neighborhood points.  However, we don't have to do the walk on this one since we
// don't need all of the neighbor points attached to this vertex.
//--------------------------------------------------------------------------------------
void qPatch::ComputeEdgePoints( bPatch::ListType& Vert, const int (&Val)[4], const int (&Pref)[4], const bool (&Bound)[4] )
{
	// Precompute some weight values that we use multiple times below
	float na = 2 * Val[0];
	float nb = 2 * Val[1];
	float nd = 2 * Val[2];
	float nc = 2 * Val[3];

	float diva = na + 10;
	float divb = nb + 10;
	float divd = nd + 10;
	float divc = nc + 10;

	// compute edge points - horizontal
	if ( Bound[0] )
	{
		m_ControlPoints[1] = (2*Vert[0]->Position + Vert[1]->Position) / 3;
		m_ControlPoints[2] = (2*Vert[1]->Position + Vert[0]->Position) / 3;
	}
	else
	{
		m_ControlPoints[1] = (na*Vert[0]->Position + 4*Vert[1]->Position + Vert[2]->Position + Vert[3]->Position*2 + 2*Vert[Pref[0]-1]->Position + Vert[Pref[0]]->Position) / diva;
		m_ControlPoints[2] = (4*Vert[0]->Position + nb*Vert[1]->Position + Vert[2]->Position*2 + Vert[3]->Position + Vert[Pref[0]-1]->Position + 2*Vert[Pref[0]]->Position) / divb;
	}

	if ( Bound[2] )
	{
		m_ControlPoints[13] = (2*Vert[3]->Position + Vert[2]->Position) / 3;
		m_ControlPoints[14] = (2*Vert[2]->Position + Vert[3]->Position) / 3;
	}
	else
	{
		m_ControlPoints[13] = (2*Vert[0]->Position + Vert[1]->Position + 4*Vert[2]->Position + nc*Vert[3]->Position + 2*Vert[Pref[2]]->Position + Vert[Pref[2]-1]->Position) / divc;
		m_ControlPoints[14] = (Vert[0]->Position + 2*Vert[1]->Position + nd*Vert[2]->Position + Vert[3]->Position*4 + Vert[Pref[2]]->Position + 2*Vert[Pref[2]-1]->Position) / divd;
	}

	// edge points - vertical
	if ( Bound[3] )
	{
		m_ControlPoints[4] = (2*Vert[0]->Position + Vert[3]->Position) / 3;
		m_ControlPoints[8] = (2*Vert[3]->Position + Vert[0]->Position) / 3;
	}
	else
	{
		m_ControlPoints[4] = (na*Vert[0]->Position + 2*Vert[1]->Position + Vert[2]->Position + Vert[3]->Position*4 + 2*Vert[4]->Position + Vert[Pref[3]-1]->Position) / diva;
		m_ControlPoints[8] = (4*Vert[0]->Position + Vert[1]->Position + 2*Vert[2]->Position + nc*Vert[3]->Position + Vert[4]->Position + 2*Vert[Pref[3]-1]->Position) / divc;
	}

	if ( Bound[1] )
	{
		m_ControlPoints[7]  = ( 2*Vert[1]->Position + Vert[2]->Position ) / 3;
		m_ControlPoints[11] = ( 2*Vert[2]->Position + Vert[1]->Position ) / 3;
	}
	else
	{
		m_ControlPoints[7] = (2*Vert[0]->Position + nb*Vert[1]->Position + 4*Vert[2]->Position + Vert[3]->Position + 2*Vert[Pref[1]-1]->Position + Vert[Pref[1]]->Position) / divb;
		m_ControlPoints[11] = (Vert[0]->Position + 4*Vert[1]->Position + nd*Vert[2]->Position + 2*Vert[3]->Position + Vert[Pref[1]-1]->Position + 2*Vert[Pref[1]]->Position) / divd;
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float qPatch::CornerPoint::CalcTanMask( unsigned int i_CPIndex, bool bInv )
{
	float index = i_CPIndex % Valence;
	float CosfPIV = cos( maConstants::c_fPI / Valence );
	float VSqrtTerm = ( Valence * sqrt( 4.0f + CosfPIV * CosfPIV ) );

	float val = 0;
	if ( bInv )
	{
		val = (1.0f / VSqrtTerm) * cos( (maConstants::c_fPI_Times_2 * index + maConstants::c_fPI) / (float)Valence );
	}
	else
	{
		val = ((1.0f / Valence) + CosfPIV / VSqrtTerm ) * cos( (maConstants::c_fPI_Times_2 * index) / (float)Valence );
	}

	return val;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void qPatch::CornerPoint::Accumulate( bPatch::ListType& Vert, unsigned int i_Index, float i_Weight, unsigned int i_TUindex, unsigned int i_TVOffset, bool i_bInv )
{
	float3 pos = Vert[i_Index]->Position;
	P += pos * i_Weight;
	U += pos * CalcTanMask( i_TUindex, i_bInv );
	V += pos * CalcTanMask( i_TUindex + i_TVOffset, i_bInv );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float3 qPatch::ComputeTangent( unsigned int i_ID, bool i_bTurn )
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


// Helps with getting tangent stencils from the g_TanM constant array
#define TANM(a,v) ( s_TangentStencils[ Val[v]*64 + (a) ] )

//--------------------------------------------------------------------------------------
// Computes the corner vertices of the output UV patch.  The corner vertices are
// a weighted combination of all points that are "connected" to that corner by an edge.
// The interior 4 points of the original subd quad are easy to get.  The points in the
// 1-ring neighborhood around the interior quad are not.
//
// Because the valence of that corner could be any number between 3 and 16, we need to
// walk around the subd patch vertices connected to that point.  This is there the
// Pref (prefix) values come into play.  Each corner has a prefix value that is the index
// of the last value around the 1-ring neighborhood that should be used in calculating
// the coefficient of that corner.  The walk goes from the prefix value of the previous
// corner to the prefix value of the current corner.
//--------------------------------------------------------------------------------------
float g_scale = 1.8f;

#ifdef NEW_SUB
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void qPatch::ComputeCornerPoints( bPatch::ListType& Vert, const int (&Val)[4], const int (&Pref)[4], const bool (&Bound)[4],
								 float3 (&CornersU)[4], float3 (&CornersV)[4])
{
	CornerPoint Corners[4];

	// Loop through all four corners
	for( int i = 0; i < 4; i++) 
	{ 
		unsigned int Valence = Val[i];
		CornerPoint& CP = Corners[i];
		CP.Valence = Valence;
		CP.P = float3(0,0,0);
		CP.U = float3(0,0,0);
		CP.V = float3(0,0,0);

		if ( Valence == 2 )	//no neighbors
		{
			CP.Accumulate( Vert, i, 1, 0, Valence, false );
		}
		else if ( Bound[i] ) //this edge has a boundary
		{
			int ustart = 4;
			if ( i ) ustart = Pref[i-1];
			CP.Accumulate( Vert, i, 4, 0, Valence, false );	//corner
			CP.Accumulate( Vert, ustart, 1, 0, Valence, false );	//prev off quad
			CP.Accumulate( Vert, (i+1)&3, 1, 0, Valence, false );	//next
			CP.P *= 1.0f / 6; // normalize
		}
		else if ( Bound[(i+3)&3] )	//previous edge has a boundary
		{
			int ustart = 4;
			if ( i ) ustart = Pref[i-1];
			CP.Accumulate( Vert, i, 4, 0, Valence, false );	//corner
			CP.Accumulate( Vert, ustart, 1, 0, Valence, false );	//next off quad
			CP.Accumulate( Vert, (i+3)&3, 1, 0, Valence, false );	//prev
			CP.P *= 1.0f / 6; // normalize
		}
		else
		{
			// Figure out where to start the walk by using the previous corner's prefix value
			unsigned int PrefIm1 = 0;
			unsigned int uStart = 4;
			const unsigned int uVOff = Valence + (i&1 ? 1 : -1);
			if ( i )
			{
				PrefIm1 = Pref[i-1];
				uStart = PrefIm1;
			}

			//Texture Indices
			unsigned int uTIndexStart = 2 - (i&1);
			unsigned int uTIndex = uTIndexStart;

			// Calculate the N*N weight for the final value
			CP.P = (Valence*Valence)*Vert[i]->Position; // n^2 part

			// Start the walk with the uStart prefix (the prefix of the corner before us)
			CP.Accumulate( Vert, uStart, 4, uTIndex, uVOff, false );

			// Gather all vertices between the previous corner's prefix and our own prefix
			// We'll do two at a time, since they always come in twos
			while(uStart < Pref[i]-1)
			{
				++uStart;
				CP.Accumulate( Vert, uStart, 1, uTIndex, uVOff, true );

				++uStart;
				++uTIndex;
				CP.Accumulate( Vert, uStart, 4, uTIndex, uVOff, false );
			}
			++uStart;

			// Add in the last guy and make sure to wrap to the beginning if we're the last corner
			if ( i == 3 ) uStart = 4; 

			CP.Accumulate( Vert, uStart, 1, uTIndex, uVOff, true );

			// Add in the guy before the prefix as well
			if ( i ) uStart = PrefIm1-1;
			else    uStart = Pref[3]-1;

			uTIndex = uTIndexStart-1;
			CP.Accumulate( Vert, uStart, 1, uTIndex, uVOff, true );

			// We're done with the walk now.  Now we need to add the contributions of the original subd quad.
			uStart = (i+1)&3;
			uTIndex = (i&1)*(Valence-1);
			CP.Accumulate( Vert, uStart, 4, uTIndex, uVOff, false );

			uStart = (i+2)&3;
			CP.Accumulate( Vert, uStart, 1, uTIndex, uVOff, true );

			uStart = (i+3)&3;
			uTIndex = (uTIndex+1)%Valence;
			CP.Accumulate( Vert, uStart, 4, uTIndex, uVOff, false );

			// Normalize the corner weights
			CP.P *= 1.0f / ( Valence * Valence + 5 * Valence ); // normalize
		}
	}

	//assign corner points
	m_ControlPoints[0] = Corners[0].P;
	m_ControlPoints[3] = Corners[1].P;
	m_ControlPoints[15] = Corners[2].P;
	m_ControlPoints[12] = Corners[3].P;
	//assign corner tangents, flip directions for opposite edges
	CornersU[0] = Corners[0].U;
	CornersU[1] = -Corners[1].U;
	CornersU[2] = -Corners[2].U;
	CornersU[3] = Corners[3].U;
	CornersV[0] = Corners[0].V;
	CornersV[1] = Corners[1].V;
	CornersV[2] = -Corners[2].V;
	CornersV[3] = -Corners[3].V;
}

//--------------------------------------------------------------------------------------
// Helper function
//--------------------------------------------------------------------------------------
void BezierRaise( const float3 (&pQ)[3], float3 (&pC)[4])
{
	pC[0] = pQ[0];
	pC[3] = pQ[2];

	for( int i=1; i<3; i++ ) 
	{
		pC[i] = (pQ[i - 1] * i + (3.0f - i) * pQ[i]) / 3;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float3 ProjectTangent( const float3& Tan, const float3& P)
{
	return (2 * Tan + P) / 3;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void qPatch::ComputeTanPatch2( float3 (&vOut)[16], const float4& fCWts, const float3 (&vCorner)[4], const float3 (&vCornerLocal)[4], bool i_bTurn )
{
	float3 vQuad[3];
	float3 vQuadB[3];
	float3 vCubic[4];

	const unsigned int Stride = i_bTurn ? 4 : 1;

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
	const unsigned int xLoc = Stride;
	vOut[xLoc] = vCubic[1];	//x=1, y=4
	vOut[xLoc + Stride] = vCubic[2];	//x=2, y=8

	vQuad[0] = vCornerLocal[2];
	vQuad[1] = ETanD;
	vQuad[2] = vCornerLocal[3];

	BezierRaise(vQuad,vCubic);
	const unsigned int yLoc = i_bTurn ? 7 : 13;
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
	for( int i = 0; i < 4; i++, V += Stride ) *V = vCubic[i];

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
	for( int i = 0; i < 4; i++, V += Stride ) *V = vCubic[i];
}

//--------------------------------------------------------------------------------------
// Computes the tangent patch from the input bezier patch
//--------------------------------------------------------------------------------------

#else//NEW_SUB
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void qPatch::ComputeCornerPoints( bPatch::ListType& Vert, const int (&Val)[4], const int (&Pref)[4],
                                  float3 (&CornersU)[4], float3 (&CornersV)[4])
{
	float3 CornersB[4];

	// Loop through all four corners
	for( int i = 0; i < 4; i++) 
	{ 
		float Valence = Val[i];
		// Figure out where to start the walk by using the previous corner's prefix value
		uint32 PrefIm1 = 0;
		uint32 uStart = 4;
		const uint32 uVOff = Valence + (i&1 ? 1 : -1);
		if ( i )
		{
			PrefIm1 = Pref[i-1];
			uStart = PrefIm1;
		}

		//Texture Indices
		uint32 uTIndexStart = 2 - (i&1);
		uint32 uTIndex = uTIndexStart;

		// Calculate the N*N weight for the final value
		CornersB[i] = (Valence*Valence)*Vert[i]->Position; // n^2 part

		// Start the walk with the uStart prefix (the prefix of the corner before us)
		CornersB[i] += Vert[uStart]->Position * 4;
		CornersU[i]  = Vert[uStart]->Position * TANM( uTIndex * 2, i );
		CornersV[i]  = Vert[uStart]->Position * TANM( ( ( uTIndex + uVOff ) % Val[i] ) * 2, i);

		// Gather all vertices between the previous corner's prefix and our own prefix
		// We'll do two at a time, since they always come in twos
		while(uStart < Pref[i]-1) 
		{
			++uStart;
			CornersB[i] += Vert[uStart]->Position;
			CornersU[i] += Vert[uStart]->Position * TANM( uTIndex * 2 + 1, i );
			CornersV[i] += Vert[uStart]->Position * TANM( ( ( uTIndex + uVOff ) % Val[i] ) * 2 + 1, i );

			++uStart;
			CornersB[i] += Vert[uStart]->Position * 4;
			++uTIndex;
			CornersU[i] += Vert[uStart]->Position * TANM( ( uTIndex % Val[i] ) * 2, i );
			CornersV[i] += Vert[uStart]->Position * TANM( ( ( uTIndex+uVOff)%Val[i]) * 2, i );
		}
		++uStart;

		// Add in the last guy and make sure to wrap to the beginning if we're the last corner
		if ( i == 3 ) uStart = 4; 

		CornersB[i] += Vert[uStart]->Position;
		CornersU[i] += Vert[uStart]->Position * TANM( ( uTIndex % Val[i] ) * 2 + 1, i );
		CornersV[i] += Vert[uStart]->Position * TANM( ( ( uTIndex + uVOff ) % Val[i] ) * 2 + 1, i );

		// Add in the guy before the prefix as well
		if ( i ) uStart = PrefIm1-1;
		else    uStart = Pref[3]-1;

		CornersB[i] += Vert[uStart]->Position;
		uTIndex = uTIndexStart-1;
		CornersU[i] += Vert[uStart]->Position * TANM( ( uTIndex % Val[i] ) * 2 + 1, i );
		CornersV[i] += Vert[uStart]->Position * TANM( ( ( uTIndex + uVOff ) % Val[i] ) * 2 + 1, i );

		// We're done with the walk now.  Now we need to add the contributions of the original subd quad.
		uStart = (i+1)&3;
		CornersB[i] += Vert[uStart]->Position * 4;
		uTIndex = (i&1)*(Val[i]-1);
		CornersU[i] += Vert[uStart]->Position * TANM( ( uTIndex % Val[i] ) * 2, i );
		CornersV[i] += Vert[uStart]->Position * TANM( ( ( uTIndex + uVOff ) % Val[i] ) * 2, i );

		uStart = (i+2)&3;
		CornersB[i] += Vert[uStart]->Position;
		CornersU[i] += Vert[uStart]->Position * TANM( ( uTIndex % Val[i] ) * 2 + 1, i );
		CornersV[i] += Vert[uStart]->Position * TANM( ( ( uTIndex + uVOff ) % Val[i] ) * 2 + 1, i );

		uStart = (i+3)&3;
		CornersB[i] += Vert[uStart]->Position * 4;
		uTIndex = (uTIndex+1)%Val[i];
		CornersU[i] += Vert[uStart]->Position * TANM( ( uTIndex % Val[i] ) * 2, i );
		CornersV[i] += Vert[uStart]->Position * TANM( ( ( uTIndex + uVOff ) % Val[i] ) * 2, i );

		// Normalize the corner weights
		CornersB[i] *= 1.0f / ( Valence * Valence + 5 * Valence ); // normalize
	}

	//assign corner points
	m_ControlPoints[0] = CornersB[0];
	m_ControlPoints[3] = CornersB[1];
	m_ControlPoints[15] = CornersB[2];
	m_ControlPoints[12] = CornersB[3];
	//assign corner tangents, flip directions for opposite edges
	CornersV[2] *= -1;
	CornersV[3] *= -1;
	CornersU[1] *= -1;
	CornersU[2] *= -1;
}

//--------------------------------------------------------------------------------------
// Helper function
//--------------------------------------------------------------------------------------
void BezierRaise( const float3 (&pQ)[3], float3 (&pC)[4])
{
	pC[0] = pQ[0];
	pC[3] = pQ[2];

	for( int i=1; i<3; i++ ) 
	{
		pC[i] = ( 1.0f / 3.0f ) * ( pQ[i - 1] * i + ( 3.0f - i ) * pQ[i] );
	}
}

//--------------------------------------------------------------------------------------
// Computes the tangent patch from the input bezier patch
//--------------------------------------------------------------------------------------
void qPatch::ComputeTanPatch( float3 (&vOut)[16], const float4& fCWts, const float3 (&vCorner)[4], const float3 (&vCornerLocal)[4], const unsigned int cX, const unsigned int cY )
{
	float3 vQuad[3];
	float3 vQuadB[3];
	float3 vCubic[4];

	// boundary edges are really simple...
	vQuad[0] = vCornerLocal[0];
	vQuad[2] = vCornerLocal[1];
	vQuad[1] = 3.0f*(m_ControlPoints[2*cX+0*cY]-m_ControlPoints[1*cX+0*cY]);

	BezierRaise(vQuad,vCubic);
	vOut[1*cX + 0*cY] = vCubic[1];
	vOut[2*cX + 0*cY] = vCubic[2];

	vQuad[0] = vCornerLocal[2];
	vQuad[2] = vCornerLocal[3];
	vQuad[1] = 3.0f*(m_ControlPoints[2*cX+3*cY]-m_ControlPoints[1*cX+3*cY]);

	BezierRaise(vQuad,vCubic);
	vOut[1*cX + 3*cY] = vCubic[1];
	vOut[2*cX + 3*cY] = vCubic[2];

	// two internal edges - this is where work happens...
	float3 vA,vB,vC,vD,vE;
	float fC0,fC1;
	vQuad[1] = 3.0f*(m_ControlPoints[2*cX+2*cY]-m_ControlPoints[1*cX+2*cY]);
	// also do "second" scan line
	vQuadB[1] = 3.0f*(m_ControlPoints[2*cX+1*cY]-m_ControlPoints[1*cX+1*cY]);

	vD = 3.0f*(m_ControlPoints[1*cX + 2*cY] - m_ControlPoints[0*cX + 2*cY]);
	vE = 3.0f*(m_ControlPoints[1*cX + 1*cY] - m_ControlPoints[0*cX + 1*cY]); // used later...

	fC0 = fCWts.m_W;
	fC1 = fCWts.m_X;

	// sign flip
	vA = -vCorner[3];
	vB = 3.0f*(m_ControlPoints[0*cX + 1*cY] - m_ControlPoints[0*cX + 2*cY]);
	vC = -vCorner[0];

	vQuad[0] = 1.0f/3.0f*(2.0f*fC0*vB - fC1*vA) + vD;
	vQuadB[0] = 1.0f/3.0f*(fC0*vC - 2.0f*fC1*vB) + vE;

	// do end of strip - same as before, but stuff is switched around...
	vC = vCorner[2];
	vB = 3.0f*(m_ControlPoints[3*cX + 2*cY] - m_ControlPoints[3*cX + 1*cY]);
	vA = vCorner[1];

	vD = 3.0f*(m_ControlPoints[2*cX + 1*cY] - m_ControlPoints[3*cX + 1*cY]);
	vE = 3.0f*(m_ControlPoints[2*cX + 2*cY] - m_ControlPoints[3*cX + 2*cY]);

	fC0 = fCWts.m_Y;
	fC1 = fCWts.m_Z;

	vQuadB[2] = 1.0f/3.0f*(2.0f*fC0*vB - fC1*vA) + vD;
	vQuad[2] = 1.0f/3.0f*(fC0*vC - 2.0f*fC1*vB) + vE;

	vQuadB[2] *= -1.0f;
	vQuad[2] *= -1.0f;

	BezierRaise(vQuad,vCubic);

	vOut[0*cX + 2*cY] = vCubic[0];
	vOut[1*cX + 2*cY] = vCubic[1];
	vOut[2*cX + 2*cY] = vCubic[2];
	vOut[3*cX + 2*cY] = vCubic[3];

	BezierRaise(vQuadB,vCubic);

	vOut[0*cX + 1*cY] = vCubic[0];
	vOut[1*cX + 1*cY] = vCubic[1];
	vOut[2*cX + 1*cY] = vCubic[2];
	vOut[3*cX + 1*cY] = vCubic[3];
}

#endif//NEW_SUB


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void qPatch::ConvertFromPatch( bPatch& i_Patch )
{
	float3 CornersU[4];
	float3 CornersV[4];
	bPatch::ListType& points = i_Patch.GetNeighborhood();

	ComputeEdgePoints( points, i_Patch.Valences(), i_Patch.Prefixes(), i_Patch.BoundaryEdges() );

	ComputeInteriorPoints( points, i_Patch.Valences(), i_Patch.BoundaryEdges() );

	ComputeCornerPoints( points, i_Patch.Valences(), i_Patch.Prefixes(), i_Patch.BoundaryEdges(), CornersU, CornersV );

//	if ( i_Patch.GetType() != bPatch::Discontinuous )
	{

		float4 fCWts = CalculateWeightVector( i_Patch.Valences() );

		float3 vCornerLocal[4];
		m_UTangentPoints[0] = CornersU[0];
		m_UTangentPoints[3] = CornersU[1];
		m_UTangentPoints[15] = CornersU[2];
		m_UTangentPoints[12] = CornersU[3];
		vCornerLocal[0] = CornersU[0];
		vCornerLocal[1] = CornersU[1];
		vCornerLocal[2] = CornersU[3];
		vCornerLocal[3] = CornersU[2];
#ifdef NEW_SUB
		ComputeTanPatch2( m_UTangentPoints,fCWts,CornersV,vCornerLocal,false );
#else
		ComputeTanPatch( m_UTangentPoints,fCWts,CornersV,vCornerLocal,1,4);
#endif
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
#ifdef NEW_SUB
		ComputeTanPatch2( m_VTangentPoints,fCWts,vCorner,vCornerLocal,true );
#else
		ComputeTanPatch( m_VTangentPoints,fCWts,vCorner,vCornerLocal,4,1);
#endif
	}
}

/*
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void bPatch::AddUnique( int i_Index )
{
	//search for existing item
	ListType::iterator it = m_Neighborhood.begin();
	for( ; it != m_Neighborhood.end(); it++ )
	{
		if ( *it == i_Index ) break;
	}
	if ( it == m_Neighborhood.end() )	//not found so add
	{
		m_Neighborhood.push_back( i_Index );
	}
}
*/

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void bPatch::AddUnique( tVert* i_Vert )
{
	if ( !i_Vert ) return;
	//search for existing item
	ListType::iterator it = m_Neighborhood.begin();
	for( ; it != m_Neighborhood.end(); it++ )
	{
		if ( *it == i_Vert ) break;
	}
	if ( it == m_Neighborhood.end() )	//not found so add
	{
		m_Neighborhood.push_back( i_Vert );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool bPatch::IsOrdinary()
{
	if ( m_Valence[0] == 4 &&
		m_Valence[1] == 4 &&
		m_Valence[2] == 4 &&
		m_Valence[3] == 4 ) return true;

	return false;
}

