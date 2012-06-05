/*****************************************************************************
**	gpxEffectDisplacement.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxEffectDisplacement.hpp"

#include "Graphics/Eff/effDisplacementData.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxEffectDisplacement::gpxEffectDisplacement(effDisplacementData &i_Effect)
:	m_Effect(i_Effect)
{
#if USE_PROXIES
	m_Scale = i_Effect.m_Scale;
	m_Bias = i_Effect.m_Bias;
	m_Blur = i_Effect.m_Blur;
	m_TessellationValue = i_Effect.m_TessellationValue;
	m_ObjUVScale = i_Effect.m_ObjUVScale;
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxEffectDisplacement::~gpxEffectDisplacement()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxEffectDisplacement::SetScale(float i_Scale)
{
	PROXY_SET_VARIABLE(m_Effect.m_Scale, m_Scale, i_Scale);
}
void gpxEffectDisplacement::SetBias(float i_Bias)
{
	PROXY_SET_VARIABLE(m_Effect.m_Bias, m_Bias, i_Bias);
}
void gpxEffectDisplacement::SetBlur(float i_Blur)
{
	PROXY_SET_VARIABLE(m_Effect.m_Blur, m_Blur, i_Blur);
}
void gpxEffectDisplacement::SetTessellationValue(float i_TessellationValue)
{
	PROXY_SET_VARIABLE(m_Effect.m_TessellationValue, m_TessellationValue, i_TessellationValue);
}
void gpxEffectDisplacement::SetObjUVScale(const maVector2d& i_ObjUVScale)
{
	PROXY_SET_VARIABLE(m_Effect.m_ObjUVScale, m_ObjUVScale, i_ObjUVScale);
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxEffectDisplacement::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Effect.m_Scale = m_Scale;
	m_Effect.m_Bias = m_Bias;
	m_Effect.m_Blur = m_Blur;
	m_Effect.m_TessellationValue = m_TessellationValue;
	m_Effect.m_ObjUVScale = m_ObjUVScale;

	this->SetNeedsUpdate(false);
#endif

	return true;
}
