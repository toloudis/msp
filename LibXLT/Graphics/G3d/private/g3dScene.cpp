/****************************************************************************\
**	g3dScene.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dScene.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dScene::g3dScene()
{
}

//--------------------------------------------------------------------
// creates scene with single layer and sets it to be the world root
//--------------------------------------------------------------------
g3dScene::g3dScene(g3dLayer* i_Layer)
:	g3dLayerContainer(i_Layer)
{
}

//--------------------------------------------------------------------
// construct scene with given layer scheme.
//	the scene takes ownership of these layers.
//--------------------------------------------------------------------
g3dScene::g3dScene(const std::vector<g3dLayer*> &i_Layers)
:	g3dLayerContainer(i_Layers)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dScene::~g3dScene()
{
}

//------------------------------------------------------------------------
//	SetFog sets all fog parameters
//------------------------------------------------------------------------
void g3dScene::SetFog(const fogParams& i_FogParams)
{
	m_FogParams = i_FogParams;
}


//------------------------------------------------------------------------
//	Returns true if fog mode is an enabled state
//------------------------------------------------------------------------
bool g3dScene::IsFogEnabled() const
{
	return (m_FogParams.m_nMode != g3dType::e_FogModeNone);
}

//------------------------------------------------------------------------
// Returns color of fog
//------------------------------------------------------------------------
const maFloatRGBA& g3dScene::GetFogColor() const
{
	return m_FogParams.m_Color;
}

//------------------------------------------------------------------------
// Returns orientation of fog
//------------------------------------------------------------------------
const maVector3d& g3dScene::GetFogOrientation() const
{
	return m_FogParams.m_Orientation;
}

//------------------------------------------------------------------------
// Returns the preference to use world orientation for fog
//------------------------------------------------------------------------
bool g3dScene::IsFogWorldOriented() const
{
	return m_FogParams.m_bUseWorld;
}

//------------------------------------------------------------------------
// Returns start, end, density settings of fog
//------------------------------------------------------------------------
void g3dScene::GetFogSettings(fogParams& o_FogParams) const
{
	o_FogParams = m_FogParams;
}

//------------------------------------------------------------------------
// Returns settings of ssao
//------------------------------------------------------------------------
void g3dScene::SetSSAOSettings(const ssaoParams& i_SSAOParams)
{
	m_SSAOParams = i_SSAOParams;
}
void g3dScene::GetSSAOSettings(ssaoParams& o_SSAOParams) const
{
	o_SSAOParams = m_SSAOParams;
}

//------------------------------------------------------------------------
// Returns settings of ssgi
//------------------------------------------------------------------------
void g3dScene::SetSSGISettings(const ssgiParams& i_SSGIParams)
{
	m_SSGIParams = i_SSGIParams;
}
void g3dScene::GetSSGISettings(ssgiParams& o_SSGIParams) const
{
	o_SSGIParams = m_SSGIParams;
}

//------------------------------------------------------------------------
// Returns settings of motion blur
//------------------------------------------------------------------------
void g3dScene::SetMotionBlurSettings(const MotionBlurParams& i_MotionBlurParams)
{
	m_MotionBlurParams = i_MotionBlurParams;
}
void g3dScene::GetMotionBlurSettings( MotionBlurParams& o_MotionBlurParams) const
{
	o_MotionBlurParams = m_MotionBlurParams;
}

//------------------------------------------------------------------------
// Settings of global ambient state
//------------------------------------------------------------------------
void g3dScene::SetGlobalAmbient(const g3dAmbientEnvState& i_GlobalAmbient)
{
	m_GlobalAmbient = i_GlobalAmbient;
}
void g3dScene::GetGlobalAmbient(g3dAmbientEnvState& o_GlobalAmbient) const
{
	o_GlobalAmbient = m_GlobalAmbient;
}
