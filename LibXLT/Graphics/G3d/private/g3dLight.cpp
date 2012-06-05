/****************************************************************************\
**	g3dLight.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dLight.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dLight::g3dLight()
:	m_Enabled(false),
	m_bShadow(false),
	m_Color(1.0f, 1.0f, 1.0f, 1.0f),
	m_Intensity(1.0f),
	m_ScaledIntensity(1.0f, 1.0f, 1.0f, 1.0f),
	m_FadeFactor(1.0f),
	m_DiffuseEnabled(true),
	m_SpecularEnabled(true),
	m_bAffectsGlow(true)
{
}

//----------------------------------------------------------------------------
//	pure virtual destructor (this class cannot be instantiated directly)
//----------------------------------------------------------------------------
g3dLight::~g3dLight()
{
}

//----------------------------------------------------------------------------
//	IsEnabled returns true if the light should affect the scene.  On
//	creation, lights default to a disabled state.
//----------------------------------------------------------------------------
bool g3dLight::IsEnabled() const
{
	return m_Enabled;
}

//----------------------------------------------------------------------------
//	same as Enable() + Disable, but allows a single interface for doing
//	the same thing.
//----------------------------------------------------------------------------
void g3dLight::SetEnable( bool i_bEnable )
{
	m_Enabled = i_bEnable;
}

//----------------------------------------------------------------------------
//	Enable will cause the effects of the light to be visible in the scene.
//----------------------------------------------------------------------------
void g3dLight::Enable()
{
	m_Enabled = true;
}

//----------------------------------------------------------------------------
//	Disable will cause the effects of the light to be invisible in the
//	scene.
//----------------------------------------------------------------------------
void g3dLight::Disable()
{
	m_Enabled = false;
}

//----------------------------------------------------------------------------
//	GetIntensity returns the intensity value (color) of the light.  Although
//	all lights have this property, it may be interpreted differently for
//	different types of lights for instance, a spotlight may not make
//	make a strong contribution to the lighting at a distant point, but a 
//	directional light may.
//----------------------------------------------------------------------------
const maFloatRGBA& g3dLight::GetIntensity() const
{
	return m_Color;
}

//----------------------------------------------------------------------------
//	SetIntensity sets the intensity value of the light.
//----------------------------------------------------------------------------
void g3dLight::SetIntensity(const maFloatRGBA& i_Intensity)
{
	m_Color = i_Intensity;
	m_ScaledIntensity = m_Color * m_FadeFactor;
}

//----------------------------------------------------------------------------
//	GetIntensityFactor returns the intensity value of the light.
//  This number is multiplied by the light color to intensify the brightness.
//----------------------------------------------------------------------------
float g3dLight::GetIntensityFactor() const
{
	return m_Intensity;
}

//----------------------------------------------------------------------------
//	SetIntensityFactor sets the intensity value of the light.
//----------------------------------------------------------------------------
void g3dLight::SetIntensityFactor(float i_Intensity)
{
	m_Intensity = i_Intensity;
}

//----------------------------------------------------------------------------
//	SetIntensityScalar sets a Scalar the is multiplied by the intensity
//  Useful for animating lights or fading a light in and out.
//	Must be a number in the range 0 - 1
//----------------------------------------------------------------------------
void g3dLight::SetFadeFactor( float i_fScalar )
{
	m_FadeFactor = i_fScalar;
	m_ScaledIntensity = m_Color * m_FadeFactor;
}

//----------------------------------------------------------------------------
//	GetIntensityScalar returns the Scalar of the intensity
//  Useful for animating lights or fading a light in and out.
//	Will be in the range of 0 - 1
//----------------------------------------------------------------------------
float g3dLight::GetFadeFactor() const
{
	return m_FadeFactor;
}

//----------------------------------------------------------------------------
//	GetScaledIntensity returns the intensity multiplied by the Scalar
//----------------------------------------------------------------------------
const maFloatRGBA& g3dLight::GetScaledIntensity() const
{
	return m_ScaledIntensity;
}
