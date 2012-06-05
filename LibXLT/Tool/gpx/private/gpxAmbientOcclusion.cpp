/*****************************************************************************
**	gpxAmbientOcclusion.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxAmbientOcclusion.hpp"

#include "Tool/api3d/api3dScene.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxAmbientOcclusion::gpxAmbientOcclusion()
{
//#if USE_PROXIES
//	api3dScene::GetSSAOSettings(m_AOParams);
//#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxAmbientOcclusion::~gpxAmbientOcclusion()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the character when "Update()" is called.
//--------------------------------------------------------------------
void gpxAmbientOcclusion::SetSSAO(const ssaoParams& i_SSAOParams)
{
#if USE_PROXIES
	m_AOParams = i_SSAOParams;
	this->SetNeedsUpdate(true);
#else
	api3dScene::SetSSAO(i_SSAOParams);
#endif
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxAmbientOcclusion::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	api3dScene::SetSSAO(m_AOParams);

	this->SetNeedsUpdate(false);
#endif

	return true;
}
