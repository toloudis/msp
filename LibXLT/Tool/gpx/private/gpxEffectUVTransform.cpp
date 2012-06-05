/*****************************************************************************
**	gpxEffectUVTransform.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxEffectUVTransform.hpp"

#include "Graphics/Mat/matShaderEffect.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxEffectUVTransform::gpxEffectUVTransform(effUVTransform &i_Effect)
:	m_Effect(i_Effect)
{
#if USE_PROXIES
	m_UScale = i_Effect.m_UScale;
	m_VScale = i_Effect.m_VScale;
	m_UTrans = i_Effect.m_UTrans;
	m_VTrans = i_Effect.m_VTrans;
	m_UVAngle = i_Effect.m_UVAngle;
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxEffectUVTransform::~gpxEffectUVTransform()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxEffectUVTransform::SetUVTransform(float i_UScale, float i_VScale, 
										  float i_UTrans, float i_VTrans, 
										  float i_UVAngle)
{
	PROXY_SET_VARIABLE(m_Effect.m_UScale, m_UScale, i_UScale);
	PROXY_SET_VARIABLE(m_Effect.m_VScale, m_VScale, i_VScale);
	PROXY_SET_VARIABLE(m_Effect.m_UTrans, m_UTrans, i_UTrans);
	PROXY_SET_VARIABLE(m_Effect.m_VTrans, m_VTrans, i_VTrans);
	PROXY_SET_VARIABLE(m_Effect.m_UVAngle, m_UVAngle, i_UVAngle);
}
//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxEffectUVTransform::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Effect.m_UScale = m_UScale;
	m_Effect.m_VScale = m_VScale;
	m_Effect.m_UTrans = m_UTrans;
	m_Effect.m_VTrans = m_VTrans;
	m_Effect.m_UVAngle = m_UVAngle;

	this->SetNeedsUpdate(false);
#endif

	return true;
}
