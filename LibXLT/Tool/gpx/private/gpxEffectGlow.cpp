/*****************************************************************************
**	gpxEffectGlow.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxEffectGlow.hpp"

#include "Graphics/Eff/effGlowData.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxEffectGlow::gpxEffectGlow(effGlowData &i_Effect)
:	m_Effect(i_Effect)
{
#if USE_PROXIES
	m_GlowAmount = i_Effect.m_GlowAmount;
	m_GlowScale = i_Effect.m_GlowScale;
	m_GlowSize = i_Effect.m_GlowSize;
	m_bConstantGlow = i_Effect.m_bConstantGlow;
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxEffectGlow::~gpxEffectGlow()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxEffectGlow::SetGlowAmount(float i_GlowAmount)
{
	PROXY_SET_VARIABLE(m_Effect.m_GlowAmount, m_GlowAmount, i_GlowAmount);
}
void gpxEffectGlow::SetGlowScale(const maVector4d& i_GlowScale)
{
	PROXY_SET_VARIABLE(m_Effect.m_GlowScale, m_GlowScale, i_GlowScale);
}
void gpxEffectGlow::SetGlowSize(float i_GlowSize)
{
	PROXY_SET_VARIABLE(m_Effect.m_GlowSize, m_GlowSize, i_GlowSize);
}
void gpxEffectGlow::SetConstantGlow(bool i_bConstantGlow)
{
	PROXY_SET_VARIABLE(m_Effect.m_bConstantGlow, m_bConstantGlow, i_bConstantGlow);
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxEffectGlow::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Effect.m_GlowAmount = m_GlowAmount;
	m_Effect.m_GlowScale = m_GlowScale;
	m_Effect.m_GlowSize = m_GlowSize;
	m_Effect.m_bConstantGlow = m_bConstantGlow;

	this->SetNeedsUpdate(false);
#endif

	return true;
}
