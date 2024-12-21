/*****************************************************************************
**	g3dRenderState.hpp
**
**		Represents the current render state
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_RENDERSTATE_HPP
#error g3dRenderState.hpp multiply included
#endif
#define G3D_RENDERSTATE_HPP

#ifndef G3D_LIGHT_HPP
#include "Graphics/g3d/g3dLight.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
class matTexture;


//============================================================================
//============================================================================
struct g3dAmbientEnvState
{
	inline g3dAmbientEnvState();
	inline bool operator == ( const g3dAmbientEnvState& i_AmbientState ) const;
	inline g3dAmbientEnvState& operator = (const g3dAmbientEnvState& i_CopyFrom);

	std::string m_Name;
	bool m_bInherit;
	matTexture* m_DiffuseMap;
	float m_DiffuseFactor;
	float m_DiffuseAngle;
	maFloatRGBA m_DiffuseColor;
	matTexture* m_SpecularMap;
	float m_SpecularFactor;
	float m_SpecularAngle;
	maFloatRGBA m_SpecularColor;

	// Mental ray environmental light setting
	bool m_bEnableSwlEnv;
	bool m_bEnableSwlEnvBG;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline g3dAmbientEnvState::g3dAmbientEnvState()
:	m_bInherit(true),
	m_Name(""),
	m_DiffuseMap(NULL),
	m_DiffuseFactor(0),
	m_SpecularMap(NULL),
	m_SpecularFactor(0),
	m_DiffuseAngle(0),
	m_SpecularAngle(0),
	m_DiffuseColor(1,1,1,1),
	m_SpecularColor(1,1,1,1),
	// Mental ray environmental light
	m_bEnableSwlEnv(false),
	m_bEnableSwlEnvBG(false)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline bool g3dAmbientEnvState::operator == ( const g3dAmbientEnvState& i_AmbientState ) const
{
	// ignore the inherit and name flag??
	return	m_DiffuseMap == i_AmbientState.m_DiffuseMap &&
		m_DiffuseFactor == i_AmbientState.m_DiffuseFactor &&
		m_SpecularMap == i_AmbientState.m_SpecularMap &&
		m_SpecularFactor == i_AmbientState.m_SpecularFactor &&
		m_DiffuseAngle == i_AmbientState.m_DiffuseAngle && 
		m_SpecularAngle == i_AmbientState.m_SpecularAngle &&
		m_DiffuseColor == i_AmbientState.m_DiffuseColor &&
		m_SpecularColor == i_AmbientState.m_SpecularColor &&
		m_bEnableSwlEnv == i_AmbientState.m_bEnableSwlEnv &&
		m_bEnableSwlEnvBG == i_AmbientState.m_bEnableSwlEnvBG;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline g3dAmbientEnvState& g3dAmbientEnvState::operator = (const g3dAmbientEnvState& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	m_Name = i_CopyFrom.m_Name;
	m_bInherit = i_CopyFrom.m_bInherit;
	m_DiffuseMap = i_CopyFrom.m_DiffuseMap;
	m_DiffuseFactor = i_CopyFrom.m_DiffuseFactor;
	m_SpecularMap = i_CopyFrom.m_SpecularMap;
	m_SpecularFactor = i_CopyFrom.m_SpecularFactor;
	m_DiffuseAngle = i_CopyFrom.m_DiffuseAngle;
	m_SpecularAngle = i_CopyFrom.m_SpecularAngle;
	m_DiffuseColor = i_CopyFrom.m_DiffuseColor;
	m_SpecularColor = i_CopyFrom.m_SpecularColor;
	m_bEnableSwlEnv = i_CopyFrom.m_bEnableSwlEnv;
	m_bEnableSwlEnvBG = i_CopyFrom.m_bEnableSwlEnvBG;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
struct g3dRenderState
{
	inline g3dRenderState();
	inline bool operator == ( const g3dRenderState& i_RenderState ) const;

	typedef std::vector<g3dLight*> Lights;
	Lights m_Lights;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline g3dRenderState::g3dRenderState()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline bool g3dRenderState::operator == ( const g3dRenderState& i_RenderState ) const
{
	return	m_Lights == i_RenderState.m_Lights;
}

