/*****************************************************************************\
**	mrayLive.hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MRAY_LIVE_HPP
#error mrayLive.hpp multiply included
#endif
#define MRAY_LIVE_HPP

#ifdef USE_MRAY_LIVE

class fsLocator;

namespace mrayLiveRenderPasses
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
		e_FinalGather = 13
	};
}

//============================================================================
//============================================================================
namespace mrayLive
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

#endif // USE_MRAY_LIVE
