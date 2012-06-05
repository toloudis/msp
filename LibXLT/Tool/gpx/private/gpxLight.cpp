/*****************************************************************************
**	gpxLight.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxLight.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"

#include "Graphics/G3d/g3dLight.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to light it will control.
// Internal values are set to match the light's state.
//--------------------------------------------------------------------
gpxLight::gpxLight(g3dLight &i_Light)
:	m_Light(i_Light)
{
#if USE_PROXIES
	m_bEnabled = i_Light.IsEnabled();
	m_bShadow = i_Light.GetCastsShadow();
	m_Color = i_Light.GetIntensity();
	m_IntensityFactor = i_Light.GetIntensityFactor();
	m_bDiffuseEnabled = i_Light.IsDiffuseEnabled();
	m_bSpecularEnabled = i_Light.IsSpecularEnabled();
	m_bAffectsGlow = i_Light.GetAffectsGlow();
#endif
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxLight::~gpxLight()
{
}

//----------------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are only
//	pushed through to the light when "Update()" is called.
//----------------------------------------------------------------------------
void gpxLight::SetEnable( bool i_bEnable )
{
	PROXY_SET_OR_STORE(m_Light, SetEnable, m_bEnabled, i_bEnable);
}
void gpxLight::SetIntensity(const maFloatRGBA& i_Intensity)
{
	PROXY_SET_OR_STORE(m_Light, SetIntensity, m_Color, i_Intensity);
}
void gpxLight::SetIntensityFactor(float i_IntensityFactor)
{
	PROXY_SET_OR_STORE(m_Light, SetIntensityFactor, m_IntensityFactor, i_IntensityFactor);
}
void gpxLight::SetCastsShadow(bool i_bShadow)
{
	PROXY_SET_OR_STORE(m_Light, SetCastsShadow, m_bShadow, i_bShadow);
}
void gpxLight::SetDiffuseEnabled(bool i_bEnabled)
{
	PROXY_SET_OR_STORE(m_Light, SetDiffuseEnabled, m_bDiffuseEnabled, i_bEnabled);
}
void gpxLight::SetSpecularEnabled(bool i_bEnabled)
{
	PROXY_SET_OR_STORE(m_Light, SetSpecularEnabled, m_bSpecularEnabled, i_bEnabled);
}
void gpxLight::SetAffectsGlow(bool i_bAffectsGlow)
{
	PROXY_SET_OR_STORE(m_Light, SetAffectsGlow, m_bAffectsGlow, i_bAffectsGlow);
}

//--------------------------------------------------------------------
// Update() 
//--------------------------------------------------------------------
//virtual 
bool gpxLight::Update()
{
	// Update() for gpxLight only sets its values and does not
	//	ask for a lock on the mutex. The assumption is that this
	//	will be called from within the lock of the derived class.

#if USE_PROXIES
	m_Light.SetEnable( m_bEnabled );
	m_Light.SetCastsShadow( m_bShadow );
	m_Light.SetIntensity( m_Color );
	m_Light.SetIntensityFactor( m_IntensityFactor );
	m_Light.SetDiffuseEnabled( m_bDiffuseEnabled );
	m_Light.SetSpecularEnabled( m_bSpecularEnabled );
	m_Light.SetAffectsGlow( m_bAffectsGlow );
#endif

	return true;
}
