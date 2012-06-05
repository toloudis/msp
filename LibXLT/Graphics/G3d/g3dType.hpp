/****************************************************************************\
**	g3dType.hpp
**
**		g3dType.hpp defines public geometry data types. They should be platform
**	independent.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_TYPE_HPP
#error g3dType.hpp multiply included
#endif
#define G3D_TYPE_HPP

#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_POINT4D_HPP
#include "Core/ma/maPoint4d.hpp"
#endif


//============================================================================
//============================================================================
namespace g3dType
{
	//----------------------------------------------------------------------------
	// This format enumerates the vertex buffer formats that could be used in g3dFragment.
	// When new structures are added to this namespace, please add them to this 
	// enumeration also. This enumeration is subject to change, so do not rely on the 
	// numbers that correspond to the vertex formats (i.e. don't write it to a file).
	//----------------------------------------------------------------------------
	enum VertexFormat
	{
		e_Undefined = -1,
		e_NonTexVertex = 0,
		e_Tex1Vertex,
		e_Tex2Vertex,
		e_Tex3Vertex,
		e_NonTexTLVertex,
		e_Tex1TLVertex,
		e_Tex2TLVertex,
		e_Tex3TLVertex,
		e_Tex1ColorVertex,
		e_NonTexPreLitColorVertex,
		e_Tex1PreLitColorVertex,
		e_Tex2PreLitColorVertex,
		e_Tex3PreLitColorVertex,
		e_BumpTex1Vertex,
		e_BumpLitTex1Vertex,
		e_BumpLitTex2Vertex,
		e_SkinVertex,
		e_VelocityVertex,
		e_HairVertex
	};

	struct NonTexVertex
	{
		maPoint3d m_Vertex;
		maPoint3d m_Normal;
	};

	struct Tex1Vertex
	{
		maPoint3d m_Vertex;
		maPoint3d m_Normal;
		maPoint2d m_TexCoord;
	};

	struct Tex2Vertex
	{
		maPoint3d m_Vertex;
		maPoint3d m_Normal;
		maPoint2d m_TexCoord1;
		maPoint2d m_TexCoord2;
	};

	struct Tex3Vertex
	{
		maPoint3d m_Vertex;
		maPoint3d m_Normal;
		maPoint2d m_TexCoord1;
		maPoint2d m_TexCoord2;
		maPoint2d m_TexCoord3;
	};

	struct NonTexTLVertex
	{
		maPoint4d m_Vertex;
		unsigned int m_Diffuse;
		unsigned int m_Specular;
	};

	struct Tex1TLVertex
	{
		maPoint4d m_Vertex;
		unsigned int m_Diffuse;
		unsigned int m_Specular;
		maPoint2d m_TexCoord;
	};

	struct Tex2TLVertex
	{
		maPoint4d m_Vertex;
		unsigned int m_Diffuse;
		unsigned int m_Specular;
		maPoint2d m_TexCoord1;
		maPoint2d m_TexCoord2;
	};

	struct Tex3TLVertex
	{
		maPoint4d m_Vertex;
		unsigned int m_Diffuse;
		unsigned int m_Specular;
		maPoint2d m_TexCoord1;
		maPoint2d m_TexCoord2;
		maPoint2d m_TexCoord3;
	};

	struct Tex1ColorVertex
	{
		maPoint3d m_Vertex;
		maPoint3d m_Normal;
		unsigned int m_Diffuse;
		unsigned int m_Specular;
		maPoint2d m_TexCoord;
	};

	struct NonTexPreLitColorVertex
	{
		maPoint3d m_Vertex;
		unsigned int m_Diffuse;
		maPoint2d m_TexCoord;
	};

	struct Tex1PreLitColorVertex
	{
		maPoint3d m_Vertex;
		unsigned int m_Diffuse;
		maPoint2d m_TexCoord;
	};

	struct Tex2PreLitColorVertex
	{
		maPoint3d m_Vertex;
		unsigned int m_Diffuse;
		maPoint2d m_TexCoord1;
		maPoint2d m_TexCoord2;
	};

	struct Tex3PreLitColorVertex
	{
		maPoint3d m_Vertex;
		unsigned int m_Diffuse;
		maPoint2d m_TexCoord1;
		maPoint2d m_TexCoord2;
		maPoint2d m_TexCoord3;
	};

	struct BumpTex1Vertex
	{
		maPoint3d m_Vertex;
		maPoint3d m_Normal;
		maPoint2d m_TexCoord;
		maPoint3d m_S;
		maPoint3d m_T;
	};

	struct BumpTex2Vertex
	{
		maPoint3d m_Vertex;
		maPoint3d m_Normal;
		maPoint2d m_TexCoord0;
		maPoint3d m_S;
		maPoint3d m_T;
		maPoint2d m_TexCoord1;
	};

	struct BumpLitTex1Vertex
	{
		maPoint3d m_Vertex;
		maPoint3d m_Normal;
		unsigned int m_Color;
		maPoint2d m_TexCoord;
		maPoint3d m_S;
		maPoint3d m_T;
	};

	struct BumpLitTex2Vertex
	{
		maPoint3d m_Vertex;
		maPoint3d m_Normal;
		unsigned int m_Color;
		maPoint2d m_TexCoord0;
		maPoint3d m_S;
		maPoint3d m_T;
		maPoint2d m_TexCoord1;
	};

	struct SkinVertex
	{
		maVector4d m_Bones; // indices, could also be 4 bytes packed into a DWORD.
		maVector4d m_Weights;
	};

	struct HairVertex
	{
		maPoint4d m_Vertex;
		maPoint3d m_Tangent;
		unsigned int m_Diffuse;
		float     m_Interpolant;	//varies from 0 to 1 down the hair
		float     m_Radius;			//Half with of hair at this vertex
		float	  m_Opacity;
		float     m_Specular;
		float     m_Gloss;
		float     m_AmbientDiffuse;
	};

	enum FogMode
	{
		e_FogModeNone = 0,
		e_FogModeExp,
		e_FogModeExp2,
		e_FogModeLinear,
	};	
}
