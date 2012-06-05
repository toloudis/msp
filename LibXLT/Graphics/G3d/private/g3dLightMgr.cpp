/****************************************************************************\
**	g3dLightMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dLightMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
namespace g3dLightMgr
{
namespace
{
	int l_MaxLights = 0;
	g3dLightMgrImpl* l_pImpl = NULL;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void SetImplementation(g3dLightMgrImpl* i_pImpl)
{
	l_pImpl = i_pImpl;
}
g3dLightMgrImpl* Implementation()
{
	return l_pImpl;
}

//------------------------------------------------------------------------
//	CreatePointLight creates a point light object.  This object can
//	be manipulated by the client (including enabling/disabling) to change
//	the properties of the light.  A point light has a isotropic influence
//	in a limited area.
//------------------------------------------------------------------------
g3dPointLight* CreatePointLight()
{
	DBG_ASSERT(l_pImpl, "No light manager implementation.");
	if (!l_pImpl)
		return NULL;
	return l_pImpl->CreatePointLight();
}

//------------------------------------------------------------------------
//	CreateDirectionalLight creates a directional light object.  A
//	directional light has no range limit and always radiates in one
//	direction.
//------------------------------------------------------------------------
g3dDirectionalLight* CreateDirectionalLight()
{
	DBG_ASSERT(l_pImpl, "No light manager implementation.");
	if (!l_pImpl)
		return NULL;
	return l_pImpl->CreateDirectionalLight();
}

//------------------------------------------------------------------------
//	CreateSpotLight creates a spot light object.  This object can
//	be manipulated by the client (including enabling/disabling) to change
//	the properties of the light.
//------------------------------------------------------------------------
g3dSpotLight* CreateSpotLight()
{
	DBG_ASSERT(l_pImpl, "No light manager implementation.");
	if (!l_pImpl)
		return NULL;
	return l_pImpl->CreateSpotLight();
}

//------------------------------------------------------------------------
//	CreateProjectedLight creates a projected light object.  This object 
//	projects a texture along a direction like a slide projector.
//------------------------------------------------------------------------
g3dProjectedLight* CreateProjectedLight()
{
	DBG_ASSERT(l_pImpl, "No light manager implementation.");
	if (!l_pImpl)
		return NULL;
	return l_pImpl->CreateProjectedLight();
}

//------------------------------------------------------------------------
//	DestroyLight should be called to destroy any light object (no matter
//	which function created it).
//------------------------------------------------------------------------
void DestroyLight(g3dLight* i_Light)
{
	DBG_ASSERT(l_pImpl, "No light manager implementation.");
	if (!l_pImpl)
		return;
	l_pImpl->DestroyLight(i_Light);
}

//------------------------------------------------------------------------
//	SetAmbient sets the global ambient light color for the scene.
//------------------------------------------------------------------------
void SetAmbient(const maFloatRGBA& i_Color)
{
	DBG_ASSERT(l_pImpl, "No light manager implementation.");
	if (!l_pImpl)
		return;
	l_pImpl->SetAmbient(i_Color);
}

//------------------------------------------------------------------------
//	GetMaxLights returns the maximum number of lights that can be
//	displayed by the hardware.  If more lights than this number are
//	enabled, they will probably not affect the scene.
//------------------------------------------------------------------------
int GetMaxLights()
{
	DBG_ASSERT(l_pImpl, "No light manager implementation.");
	if (!l_pImpl)
		return l_MaxLights;


	//	we'll cache this
	//
	if( l_MaxLights == 0 )
		l_MaxLights = l_pImpl->GetMaxLights();

	return l_MaxLights;
}

//------------------------------------------------------------------------
// Enable or disable a directional light that faces in the direction 
// of the camera view. Enabling the headlight also turns off all 
// other lights.
//------------------------------------------------------------------------
void EnableHeadlight(bool i_bEnable)
{
	DBG_ASSERT(l_pImpl, "No light manager implementation.");
	if (!l_pImpl)
		return;
	l_pImpl->EnableHeadlight( i_bEnable );
}
bool IsHeadlightEnabled()
{
	DBG_ASSERT(l_pImpl, "No light manager implementation.");
	if (l_pImpl)
		return l_pImpl->IsHeadlightEnabled();
	return false;
}

//------------------------------------------------------------------------
// Sets direction of the headlight - the directional light that
//	faces in the direction of the camera view.
//------------------------------------------------------------------------
void SetHeadlightDirection( const maVector3d& i_LightDir )
{
	DBG_ASSERT(l_pImpl, "No light manager implementation.");
	if (!l_pImpl)
		return;
	l_pImpl->SetHeadlightDirection( i_LightDir );
}

}

