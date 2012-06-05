/*****************************************************************************
**	gpxEnvironment.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxEnvironment.hpp"

#include "Tool/api3d/api3dObjectSingle.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxEnvironment::gpxEnvironment(api3dObjectSingle &i_Environment)
:	m_Environment(i_Environment)
{
	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxEnvironment::~gpxEnvironment()
{
	PROXY_REMOVE();
}



//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the character when "Update()" is called.
//--------------------------------------------------------------------
void gpxEnvironment::SetRenderStateNameEnv(std::string i_Name)
{
#if USE_PROXIES
	m_EnvState.m_Name = i_Name;
	this->SetNeedsUpdate(true);
#else
	m_Environment.SetRenderStateNameEnv(i_Name);
#endif

}

void gpxEnvironment::SetRenderStateDiffuseEnv(matTexture* i_Map, 
											  float i_Weight, 
											  float i_Angle,
											  const maFloatRGBA& i_Color)
{
#if USE_PROXIES
	m_EnvState.m_DiffuseMap = i_Map;
	m_EnvState.m_DiffuseFactor = i_Weight;
	m_EnvState.m_DiffuseAngle = i_Angle;
	m_EnvState.m_DiffuseColor = i_Color;
	this->SetNeedsUpdate(true);
#else
	m_Environment.SetRenderStateNameEnv(i_Map, i_Weight, i_Angle, i_Color);
#endif
}

void gpxEnvironment::SetRenderStateSpecularEnv(matTexture* i_Map, 
											   float i_Weight, 
											   float i_Angle,
											   const maFloatRGBA& i_Color)
{
#if USE_PROXIES
	m_EnvState.m_SpecularMap = i_Map;
	m_EnvState.m_SpecularFactor = i_Weight;
	m_EnvState.m_SpecularAngle = i_Angle;
	m_EnvState.m_SpecularColor = i_Color;
	this->SetNeedsUpdate(true);
#else
	m_Environment.SetRenderStateSpecularEnv(i_Map, i_Weight, i_Angle, i_Color);
#endif
}

void gpxEnvironment::SetSwlData(bool i_bEnable)
{
	m_Environment.SetSwlData(i_bEnable);
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxEnvironment::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Environment.SetRenderStateNameEnv(m_EnvState.m_Name);
	m_Environment.SetRenderStateDiffuseEnv(m_EnvState.m_DiffuseMap,
											m_EnvState.m_DiffuseFactor,
											m_EnvState.m_DiffuseAngle,
											m_EnvState.m_DiffuseColor);
	m_Environment.SetRenderStateSpecularEnv(m_EnvState.m_SpecularMap,
											m_EnvState.m_SpecularFactor,
											m_EnvState.m_SpecularAngle,
											m_EnvState.m_SpecularColor);

	this->SetNeedsUpdate(false);
#endif

	return true;
}
