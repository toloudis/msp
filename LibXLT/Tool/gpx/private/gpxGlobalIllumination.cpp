/*****************************************************************************
**	gpxGlobalIllumination.cpp
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxGlobalIllumination.hpp"

#include "Tool/api3d/api3dScene.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxGlobalIllumination::gpxGlobalIllumination()
{
//#if USE_PROXIES
//	api3dScene::GetSSAOSettings(m_AOParams);
//#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxGlobalIllumination::~gpxGlobalIllumination()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the character when "Update()" is called.
//--------------------------------------------------------------------
void gpxGlobalIllumination::SetSSGI(const ssgiParams& i_SSGIParams)
{
#if USE_PROXIES
	m_GIParams = i_SSGIParams;
	this->SetNeedsUpdate(true);
#else
	api3dScene::SetSSGI(i_SSGIParams);
#endif
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxGlobalIllumination::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	api3dScene::SetSSGI(m_GIParams);

	this->SetNeedsUpdate(false);
#endif

	return true;
}
