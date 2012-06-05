/*****************************************************************************
**	gpxPointLight.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxPointLight.hpp"
#include "Tool/gpx/gpxProxyMgr.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"

#include "Graphics/G3d/g3dPointLight.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to light it will control
//--------------------------------------------------------------------
gpxPointLight::gpxPointLight(g3dPointLight &i_Light)
:	gpxLight(i_Light),
	m_Light(i_Light)
{
#if USE_PROXIES
	m_Position = i_Light.GetPosition();
	m_f0 = i_Light.GetFalloff0();
	m_f1 = i_Light.GetFalloff1();
	m_f2 = i_Light.GetFalloff2();
	m_f3 = i_Light.GetFalloff3();
	m_fStart = i_Light.GetFalloffStart();
	m_fRange = i_Light.GetRange();
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxPointLight::~gpxPointLight()
{
	PROXY_REMOVE();
}

//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are 
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxPointLight::SetPosition(const maPoint3d& i_Position)
{
	PROXY_SET_OR_STORE(m_Light, SetPosition, m_Position, i_Position);
}
void gpxPointLight::SetFalloff0(float i_Val)
{
	PROXY_SET_OR_STORE(m_Light, SetFalloff0, m_f0, i_Val);
}
void gpxPointLight::SetFalloff1(float i_Val)
{
	PROXY_SET_OR_STORE(m_Light, SetFalloff1, m_f1, i_Val);
}
void gpxPointLight::SetFalloff2(float i_Val)
{
	PROXY_SET_OR_STORE(m_Light, SetFalloff2, m_f2, i_Val);
}
void gpxPointLight::SetFalloff3(float i_Val)
{
	PROXY_SET_OR_STORE(m_Light, SetFalloff3, m_f3, i_Val);
}
void gpxPointLight::SetFalloffStart(float i_Val)
{
	PROXY_SET_OR_STORE(m_Light, SetFalloffStart, m_fStart, i_Val);
}
void gpxPointLight::SetRange(float i_Range)
{
	PROXY_SET_OR_STORE(m_Light, SetRange, m_fRange, i_Range);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const maPoint3d& gpxPointLight::GetPosition() const
{
	return PROXY_GET(m_Light, GetPosition, m_Position);
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxPointLight::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	// The base class will not ask for a lock
	gpxLight::Update();

	m_Light.SetPosition( m_Position );
	m_Light.SetRange( m_fRange );
	m_Light.SetFalloff0( m_f0 );
	m_Light.SetFalloff1( m_f1 );
	m_Light.SetFalloff2( m_f2 );
	m_Light.SetFalloff3( m_f3 );
	m_Light.SetFalloffStart( m_fStart );

	this->SetNeedsUpdate(false);
#endif

	return true;
}

