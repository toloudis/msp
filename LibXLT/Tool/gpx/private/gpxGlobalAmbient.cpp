/*****************************************************************************
**	gpxGlobalAmbient.cpp
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxGlobalAmbient.hpp"

#include "Tool/api3d/api3dScene.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxGlobalAmbient::gpxGlobalAmbient()
{
	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxGlobalAmbient::~gpxGlobalAmbient()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the character when "Update()" is called.
//--------------------------------------------------------------------
void gpxGlobalAmbient::SetGlobalAmbient(const g3dAmbientEnvState& i_GlobalAmbient)
{
#if USE_PROXIES
	m_GlobalAmbient = i_GlobalAmbient;
	this->SetNeedsUpdate(true);
#else
	api3dScene::SetGlobalAmbient(i_GlobalAmbient);
#endif
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxGlobalAmbient::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	api3dScene::SetGlobalAmbient(m_GlobalAmbient);

	this->SetNeedsUpdate(false);
#endif

	return true;
}
