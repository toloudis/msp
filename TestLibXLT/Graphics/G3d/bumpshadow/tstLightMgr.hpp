/*****************************************************************************
**  tstLightMgr.hpp
**
**     Adds and removes lights from the root render state
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef TST_LIGHTMGR_HPP
#error tstLightMgr.hpp multiply included
#endif
#define TST_LIGHTMGR_HPP

class g3dDirectionalLight;
class g3dPointLight;
class g3dSpotLight;
class g3dLight;

namespace tstLightMgr
{
	//====================================================================
	// Initialize
	//====================================================================
	void Initialize();

	//====================================================================
	// DeInitialize
	//====================================================================
	void DeInitialize();

	//========================================================================
	//	CreatePointLight
	//========================================================================
	g3dPointLight* CreatePointLight();

	//========================================================================
	//	CreateDirectionalLight
	//========================================================================
	g3dDirectionalLight* CreateDirectionalLight();

	//========================================================================
	//	CreateSpotLight
	//========================================================================
	g3dSpotLight* CreateSpotLight();

	//========================================================================
	//	DestroyLight
	//========================================================================
	void DestroyLight( g3dLight* i_Light );
}
