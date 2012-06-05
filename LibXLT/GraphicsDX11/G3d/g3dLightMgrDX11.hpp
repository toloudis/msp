/****************************************************************************\
**  g3dLightMgrDX11.hpp
**
**	The g3dLightMgrDX11 component contains the Windows (Direct3D)
**	implementation of the g3dLightMgr.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_LIGHTMGRDX11_HPP
#error g3dLightMgrDX11.hpp multiply included
#endif
#define G3D_LIGHTMGRDX11_HPP

#ifndef G3D_LIGHTMGR_HPP
#include "Graphics/g3d/g3dLightMgr.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef G2D_DX11GLOBALWIN_HPP
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#endif

#include <map>
#include <set>

class g3dLightMgrDX11 : public g3dLightMgrImpl
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g3dLightMgrDX11();
	~g3dLightMgrDX11();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	static g3dLightMgrDX11* Implementation();

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
	//	For some passes, it is desirable to turn off the ambient.
	//  This allows the renderer to have control over the current
	//	ambient setting without altering the ambient color itself.
	//------------------------------------------------------------------------
	void EnableAmbient(bool i_bEnable);

	//------------------------------------------------------------------------
	//	GetMaxLights returns the maximum number of lights that can be
	//	displayed by the hardware.  If more lights than this number are
	//	enabled, they will probably not affect the scene.
	//------------------------------------------------------------------------
	int GetMaxLights();

	//------------------------------------------------------------------------
	//	SetLight - sets the d3d light
	//------------------------------------------------------------------------
	void SetLight( g3dLight* i_pLight );

	//------------------------------------------------------------------------
	//	SetLightJittered - sets the d3d light info for a jittered pass
	//------------------------------------------------------------------------
	void SetLightJittered( g3dLight* i_pLight, int i_Pass );

	//------------------------------------------------------------------------
	//	EnableLight - enables the d3d light
	//------------------------------------------------------------------------
	void EnableLight( g3dLight* i_pLight );

	//------------------------------------------------------------------------
	//	DisableLight - disables the d3d light
	//------------------------------------------------------------------------
	void DisableLight( g3dLight* i_pLight );

	//------------------------------------------------------------------------
	//	DisableAllLights - disables all d3d lights and sets the ambient to black
	//------------------------------------------------------------------------
	void DisableAllLights();

	//------------------------------------------------------------------------
	// Returns direction of light from light that has biggest
	// influence on the given point.  This could be used to
	// determine direction of shadows or bump mapping.
	//------------------------------------------------------------------------
	maVector3d GetPrimaryLightDirection(const maPoint3d &i_Pos);

	//------------------------------------------------------------------------
	// Gets vector of all lights
	//------------------------------------------------------------------------
	const std::vector<g3dLight*>& GetLights();
	//void GetLights( std::vector<g3dLight*> &o_Lights );

	//------------------------------------------------------------------------
	// Gets list of all lights that are currently enabled
	//------------------------------------------------------------------------
	void GetEnabledLights(std::vector<g3dLight*> &o_Lights);

	//------------------------------------------------------------------------
	//	EnableOnlyNonShadowLights causes only non-shadow lights and ambient
	//	light to be D3D-enabled.
	//------------------------------------------------------------------------
	void EnableOnlyNonShadowLights();

	//------------------------------------------------------------------------
	//	CountNumShadowLights returns the number of enabled shadow lights in
	//	the scene.
	//------------------------------------------------------------------------
	int CountNumShadowLights();

	//------------------------------------------------------------------------
	//	EnableOnlyShadowLight causes only the shadow light with the given
	//	number to be enabled (ambient is disabled too).
	//------------------------------------------------------------------------
	g3dLight* EnableOnlyShadowLight(int i_Num);

	//------------------------------------------------------------------------
	// returns global ambient light color for the scene.
	//------------------------------------------------------------------------
	const maFloatRGBA&  GetAmbient();

	//------------------------------------------------------------------------
	// Enable or disable a directional light that faces in the direction 
	// of the camera view. Enabling the headlight also turns off all 
	// other lights.
	//------------------------------------------------------------------------
	void EnableHeadlight(bool i_bEnable);
	bool IsHeadlightEnabled() const;

	//------------------------------------------------------------------------
	// Sets direction of the headlight - the directional light that
	//	faces in the direction of the camera view.
	//------------------------------------------------------------------------
	void SetHeadlightDirection( const maVector3d& i_LightDir );

private:
	std::vector<g3dLight*> m_Lights;
	std::map<g3dLight*, bool> m_Enabled;
	int m_nLightsEnabled;

	maFloatRGBA m_Ambient;

	// Headlight
	g3dDirectionalLight* m_pHeadlight;
	std::vector<g3dLight*> m_HeadlightList;	// dummy light list for when headlight is enabled
	bool m_bHeadlightEnabled;
};



