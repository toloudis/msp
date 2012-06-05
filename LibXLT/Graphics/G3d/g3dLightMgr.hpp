/****************************************************************************\
**	g3dLightMgr.hpp
**
**		The g3dLightMgr handles the lights in the scene.  It is
**	responsible for creation and deletion of lights.
**		It is possible to turn on more lights than can actually be displayed
**	with hardware acceleration; in such a case, some lights will not affect
**	the scene.  The maximum number of hardware lights available can be found
**	with the GetMaxLights function.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_LIGHTMGR_HPP
#error g3dLightMgr.hpp multiply included
#endif
#define G3D_LIGHTMGR_HPP

#include <vector>


//============================================================================
//============================================================================
class maFloatRGBA;
class g3dLight;
class g3dPointLight;
class g3dDirectionalLight;
class g3dSpotLight;
class g3dProjectedLight;
class maVector3d;


//============================================================================
//============================================================================
class g3dLightMgrImpl
{
public:
	//------------------------------------------------------------------------
	//	CreatePointLight creates a point light object.  This object can
	//	be manipulated by the client (including enabling/disabling) to change
	//	the properties of the light.  A point light has a isotropic influence
	//	in a limited area.
	//------------------------------------------------------------------------
	virtual g3dPointLight* CreatePointLight() = 0;

	//------------------------------------------------------------------------
	//	CreateDirectionalLight creates a directional light object.  A
	//	directional light has no range limit and always radiates in one
	//	direction.
	//------------------------------------------------------------------------
	virtual g3dDirectionalLight* CreateDirectionalLight() = 0;

	//------------------------------------------------------------------------
	//	CreateSpotLight creates a spot light object.  This object can
	//	be manipulated by the client (including enabling/disabling) to change
	//	the properties of the light.
	//------------------------------------------------------------------------
	virtual g3dSpotLight* CreateSpotLight() = 0;

	//------------------------------------------------------------------------
	//	CreateProjectedLight creates a projected light object.  This object 
	//	projects a texture along a direction like a slide projector.
	//------------------------------------------------------------------------
	virtual g3dProjectedLight* CreateProjectedLight() = 0;

	//------------------------------------------------------------------------
	//	DestroyLight should be called to destroy any light object (no matter
	//	which function created it).
	//------------------------------------------------------------------------
	virtual void DestroyLight(g3dLight* i_Light) = 0;

	//------------------------------------------------------------------------
	//	SetAmbient sets the global ambient light color for the scene.
	//------------------------------------------------------------------------
	virtual void SetAmbient(const maFloatRGBA& i_Color) = 0;

	//------------------------------------------------------------------------
	//	GetMaxLights returns the maximum number of lights that can be
	//	displayed by the hardware.  If more lights than this number are
	//	enabled, they will probably not affect the scene.
	//------------------------------------------------------------------------
	virtual int GetMaxLights() = 0;

	//------------------------------------------------------------------------
	// Enable or disable a directional light that faces in the direction 
	// of the camera view. Enabling the headlight also turns off all 
	// other lights.
	//------------------------------------------------------------------------
	virtual void EnableHeadlight(bool i_bEnable) = 0;
	virtual bool IsHeadlightEnabled() const = 0;

	//------------------------------------------------------------------------
	// Sets direction of the headlight - the directional light that
	//	faces in the direction of the camera view.
	//------------------------------------------------------------------------
	virtual void SetHeadlightDirection( const maVector3d& i_LightDir ) = 0;
};


//============================================================================
//============================================================================
namespace g3dLightMgr
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetImplementation(g3dLightMgrImpl* i_pImpl);
	g3dLightMgrImpl* Implementation();

	//------------------------------------------------------------------------
	//	CreatePointLight creates a point light object.  This object can
	//	be manipulated by the client (including enabling/disabling) to change
	//	the properties of the light.  A point light has a isotropic influence
	//	in a limited area.
	//------------------------------------------------------------------------
	g3dPointLight* CreatePointLight();

	//------------------------------------------------------------------------
	//	CreateDirectionalLight creates a directional light object.  A
	//	directional light has no range limit and always radiates in one
	//	direction.
	//------------------------------------------------------------------------
	g3dDirectionalLight* CreateDirectionalLight();

	//------------------------------------------------------------------------
	//	CreateSpotLight creates a spot light object.  This object can
	//	be manipulated by the client (including enabling/disabling) to change
	//	the properties of the light.
	//------------------------------------------------------------------------
	g3dSpotLight* CreateSpotLight();

	//------------------------------------------------------------------------
	//	CreateProjectedLight creates a projected light object.  This object 
	//	projects a texture along a direction like a slide projector.
	//------------------------------------------------------------------------
	g3dProjectedLight* CreateProjectedLight();

	//------------------------------------------------------------------------
	//	DestroyLight should be called to destroy any light object (no matter
	//	which function created it).
	//------------------------------------------------------------------------
	void DestroyLight(g3dLight* i_Light);

	//------------------------------------------------------------------------
	//	SetAmbient sets the global ambient light color for the scene.
	//------------------------------------------------------------------------
	void SetAmbient(const maFloatRGBA& i_Color);

	//------------------------------------------------------------------------
	//	GetMaxLights returns the maximum number of lights that can be
	//	displayed by the hardware.  If more lights than this number are
	//	enabled, they will probably not affect the scene.
	//------------------------------------------------------------------------
	int GetMaxLights();

	//------------------------------------------------------------------------
	// Enable or disable a directional light that faces in the direction 
	// of the camera view. Enabling the headlight also turns off all 
	// other lights.
	//------------------------------------------------------------------------
	void EnableHeadlight(bool i_bEnable);
	bool IsHeadlightEnabled();

	//------------------------------------------------------------------------
	// Sets direction of the headlight - the directional light that
	//	faces in the direction of the camera view.
	//------------------------------------------------------------------------
	void SetHeadlightDirection( const maVector3d& i_LightDir );
}


