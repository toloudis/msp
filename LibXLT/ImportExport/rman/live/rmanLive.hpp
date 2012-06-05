/*****************************************************************************\
**	rmanLive.hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef RMAN_LIVE_HPP
#error rmanLive.hpp multiply included
#endif
#define RMAN_LIVE_HPP

class fsLocator;

namespace rmanLiveRenderPasses
{
	enum renderPasses
	{
		e_Beauty = 0,
		e_Diffuse = 1,
		e_DiffuseEnvironment = 2,
		e_DiffuseLights = 3,
		e_Specular = 4,
		e_SpecularEnvironment = 5,
		e_SpecularLights = 6,
		e_Emissive = 7,
		e_AmbientOcclusion = 8,
		e_ShadowMask = 9,
		e_Illumination = 10,
		e_Normals = 11,
		e_Reflections = 12,
		e_ColorBleed = 13
	};
}

//============================================================================
//============================================================================
namespace rmanLive
{
	//--------------------------------------------------------------------
	// CleanUp()
	//--------------------------------------------------------------------
	void CleanUp();

	//--------------------------------------------------------------------
	// StartRender()
	//--------------------------------------------------------------------
	void StartRender(fsLocator i_MiFile, int i_RenderPass);

	//--------------------------------------------------------------------
	// StopRender()
	//--------------------------------------------------------------------
	void StopRender();

};
