/*****************************************************************************
**	mdlHairInfo.hpp
**
**		mdlHairInfo defines unattached hairs described by splines
**	along with color and thickness information.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_HAIRINFO_HPP
#error mdlHairInfo.hpp multiply included
#endif
#define MDL_HAIRINFO_HPP

#ifndef MDL_MATINFO_HPP
#include "Graphics/mdl/mdlMatInfo.hpp"
#endif


//============================================================================
//============================================================================
struct mdlHairVertex
{
	maVector3d Position;
//	maVector3d Velocity;
//	maVector3d UVW;
};

struct mdlHairMaterial
{
	mdlHairMaterial()
		: m_RootRadius(0),
		m_TipRadius(0),
		m_Opacity( 1.0 ),
		m_Specular( 0 ),
		m_Gloss( 0 ),
		m_AmbientDiffuse( 0 )
	{}

	float m_RootRadius;
	float m_TipRadius;
	maVector3d m_RootColor;
	maVector3d m_TipColor;
	maVector3d m_SurfaceNormal;
	float m_Opacity;
	float m_Specular;
	float m_Gloss;
	float m_AmbientDiffuse;
};



//============================================================================
//============================================================================
struct mdlHairStrand
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	std::vector< mdlHairVertex > m_ControlPoints;
	mdlHairMaterial Material;
};

//============================================================================
//============================================================================
struct mdlHairInfo
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	mdlHairInfo() :	m_nHairVertices( 0 ), m_nVerticesPerStrand( 0 ) {}

	std::string m_HairName;
	std::string m_SceneNodeName;
	std::vector< mdlHairStrand > m_Strands;
	int m_nHairVertices;
	int m_nVerticesPerStrand;

	shared_ptr<mdlMatInfo> m_Material;
};
