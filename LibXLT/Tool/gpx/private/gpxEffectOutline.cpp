/*****************************************************************************
**	gpxEffectOutline.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxEffectOutline.hpp"

#include "Graphics/Eff/effOutlineData.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxEffectOutline::gpxEffectOutline(effOutlineData &i_Effect)
:	m_Effect(i_Effect)
{
#if USE_PROXIES
	m_OutlineDepthScale = i_Effect.m_OutlineDepthScale;
	m_OutlineMinAngle = i_Effect.m_OutlineMinAngle;
	m_OutlineMaxAngle = i_Effect.m_OutlineMaxAngle;
	m_OutlineThickness = i_Effect.m_OutlineThickness;
	m_OutlineColor = i_Effect.m_OutlineColor;
	m_bUseDepths = i_Effect.m_bUseDepths;
	m_bUseNormals = i_Effect.m_bUseNormals;
	m_OutlineMinWidth = i_Effect.m_OutlineMinWidth;
	m_OutlineMaxWidth = i_Effect.m_OutlineMaxWidth;
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxEffectOutline::~gpxEffectOutline()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxEffectOutline::SetOutlineDepthScale(float i_OutlineDepthScale)
{
	PROXY_SET_VARIABLE(m_Effect.m_OutlineDepthScale, m_OutlineDepthScale, i_OutlineDepthScale);
}
void gpxEffectOutline::SetOutlineMinAngle(float i_OutlineMinAngle)
{
	PROXY_SET_VARIABLE(m_Effect.m_OutlineMinAngle, m_OutlineMinAngle, i_OutlineMinAngle);
}
void gpxEffectOutline::SetOutlineMaxAngle(float i_OutlineMaxAngle)
{
	PROXY_SET_VARIABLE(m_Effect.m_OutlineMaxAngle, m_OutlineMaxAngle, i_OutlineMaxAngle);
}
void gpxEffectOutline::SetOutlineThickness(float i_OutlineThickness)
{
	PROXY_SET_VARIABLE(m_Effect.m_OutlineThickness, m_OutlineThickness, i_OutlineThickness);
}
void gpxEffectOutline::SetOutlineColor(const maFloatRGBA &i_OutlineColor)
{
	PROXY_SET_VARIABLE(m_Effect.m_OutlineColor, m_OutlineColor, i_OutlineColor);
}
void gpxEffectOutline::SetUseDepths(bool i_bUseDepths)
{
	PROXY_SET_VARIABLE(m_Effect.m_bUseDepths, m_bUseDepths, i_bUseDepths);
}
void gpxEffectOutline::SetUseNormals(bool i_bUseNormals)
{
	PROXY_SET_VARIABLE(m_Effect.m_bUseNormals, m_bUseNormals, i_bUseNormals);
}
void gpxEffectOutline::SetOutlineMinWidth(float i_OutlineMinWidth)
{
	PROXY_SET_VARIABLE(m_Effect.m_OutlineMinWidth, m_OutlineMinWidth, i_OutlineMinWidth);
}
void gpxEffectOutline::SetOutlineMaxWidth(float i_OutlineMaxWidth)
{
	PROXY_SET_VARIABLE(m_Effect.m_OutlineMaxWidth, m_OutlineMaxWidth, i_OutlineMaxWidth);
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxEffectOutline::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Effect.m_OutlineDepthScale = m_OutlineDepthScale;
	m_Effect.m_OutlineMinAngle = m_OutlineMinAngle;
	m_Effect.m_OutlineMaxAngle = m_OutlineMaxAngle;
	m_Effect.m_OutlineThickness = m_OutlineThickness;
	m_Effect.m_OutlineColor = m_OutlineColor;
	m_Effect.m_bUseDepths = m_bUseDepths;
	m_Effect.m_bUseNormals = m_bUseNormals;
	m_Effect.m_OutlineMinWidth = m_OutlineMinWidth;
	m_Effect.m_OutlineMaxWidth = m_OutlineMaxWidth;

	this->SetNeedsUpdate(false);
#endif

	return true;
}
