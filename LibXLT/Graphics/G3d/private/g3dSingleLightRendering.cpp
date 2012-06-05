/****************************************************************************\
**	g3dSingleLightRendering.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dSingleLightRendering.hpp"

//#include "g3dBumpShaderWin.hpp"
//#include "g3dShadowVShadersWin.hpp"


//============================================================================
//============================================================================
namespace g3dSingleLightRendering
{
namespace
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
const g3dLight* l_ActiveLight = NULL;
bool l_bDoSingleLightRendering = true;
ShadowQualityOverride l_bShadowQualityOverride = SQ_NONE;

// flag when we are doing a particular render pass
bool l_bDOFPrepPass = false;
// flag when we are rendering transparent items
bool l_bTransparentPass = false;
// flag when we are rendering glow effect items
bool l_bGlowPass = false;
// flag when we need to render image-based lighting with environment maps
bool l_bAmbientEnvironmentPass = false;
// flag when we are generating cube map reflections
bool l_bRenderingCubeReflection = false;
// flag when we are generating planar reflections
bool l_bRenderingPlaneReflection = false;
// flag when we are doing a texture bake
bool l_bDoBaking = false;
// flag when we want to render the reflections only
bool l_bIsolateReflections = false;
// flag when the first light is being rendered in multipass
bool l_bFirstLight = false;
}


//------------------------------------------------------------------------
// Returns true if shaders are successfully loaded, so that
// single light rendering is possible on this machine.
//------------------------------------------------------------------------
//bool ShadersAvailable()
//{
//	return (//g3dBumpShaderWin::ShadersAvailable() &&
//			g3dShadowVShadersWin::ShadersAvailable());
//}

//------------------------------------------------------------------------
// If true, activates single light rendering for stencil shadows
// and bump mapping.  This is false by default.
//------------------------------------------------------------------------
bool GetDoSingleLightRendering()
{
	return l_bDoSingleLightRendering;
}
void SetDoSingleLightRendering(bool i_Val)
{
	//l_bDoSingleLightRendering = (i_Val && ShadersAvailable());
	l_bDoSingleLightRendering = i_Val;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ShadowQualityOverride GetShadowQualityOverride()
{
	return l_bShadowQualityOverride;
}
void SetShadowQualityOverride(ShadowQualityOverride i_Val)
{
	l_bShadowQualityOverride = i_Val;
}


//--------------------------------------------------------------------
// GetActiveLight - return pointer to currently active shadow light.
//	This will return NULL if not currently in a shadow casting render
//--------------------------------------------------------------------
const g3dLight*	GetActiveLight()
{
	return l_ActiveLight;
}
void SetActiveLight(const g3dLight* i_Light)
{
	l_ActiveLight = i_Light;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GetDoDOFPrepPass()
{
	return l_bDOFPrepPass;
}
void SetDoDOFPrepPass(bool i_val)
{
	l_bDOFPrepPass = i_val;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GetDoTransparentPass()
{
	return l_bTransparentPass;
}
void SetDoTransparentPass(bool i_Val)
{
	l_bTransparentPass = i_Val;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GetDoGlowPass()
{
	return l_bGlowPass;
}
void SetDoGlowPass(bool i_Val)
{
	l_bGlowPass = i_Val;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GetDoAmbientEnvironmentPass()
{
	return l_bAmbientEnvironmentPass;
}
void SetDoAmbientEnvironmentPass(bool i_Val)
{
	l_bAmbientEnvironmentPass = i_Val;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GetDoCubeReflectionGen()
{
	return l_bRenderingCubeReflection;
}
void SetDoCubeReflectionGen(bool i_Val)
{
	l_bRenderingCubeReflection = i_Val;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GetDoPlaneReflectionGen()
{
	return l_bRenderingPlaneReflection;
}
void SetDoPlaneReflectionGen(bool i_Val)
{
	l_bRenderingPlaneReflection = i_Val;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GetDoReflectionGen() 
{
	return GetDoCubeReflectionGen() || GetDoPlaneReflectionGen();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GetDoBaking()
{
	return l_bDoBaking;
}
void SetDoBaking(bool i_Val)
{
	l_bDoBaking = i_Val;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GetDoIsolateReflections()
{
	return l_bIsolateReflections;
}
void SetDoIsolateReflections(bool i_Val)
{
	l_bIsolateReflections = i_Val;
}

//--------------------------------------------------------------------
// IsFirstLight is used for multi pass rendering
// should only be set true for the first light rendered
//--------------------------------------------------------------------
bool IsFirstLight()
{
	return l_bFirstLight;
}

void SetFirstLight( bool i_bEnable )
{
	l_bFirstLight = i_bEnable;
}
}

