/*****************************************************************************
**	gpxEnvironment.hpp
**
**	This class is a thread-safe proxy for a api3dObjectSingle.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_ENVIRONMENT_HPP
#error gpxEnvironment.hpp multiply included
#endif
#define GPX_ENVIRONMENT_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 

#ifndef G3D_RENDERSTATE_HPP
#include "Graphics/G3d/g3dRenderState.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class api3dObjectSingle;


//============================================================================
//============================================================================
class gpxEnvironment : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxEnvironment(api3dObjectSingle &i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxEnvironment();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the character when "Update()" is called.
	//-------------------------------------------------------------------	
	void SetRenderStateNameEnv(std::string i_Name);
	void SetRenderStateDiffuseEnv(matTexture* i_Map, float i_Weight, float i_Angle,
		const maFloatRGBA& i_Color);
	void SetRenderStateSpecularEnv(matTexture* i_Map, float i_Weight, float i_Angle,
		const maFloatRGBA& i_Color);
	void SetSwlData(bool i_bEnable);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	api3dObjectSingle &m_Environment;

#if USE_PROXIES
	// Just using g3dAmbientEnvState to hold data
	g3dAmbientEnvState m_EnvState;
#endif
};
