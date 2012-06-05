/****************************************************************************\
**	g3dSceneRendererTypes.hpp
**
**
**	StudioGPU
**	Copyright(C) 2010. - All Rights Reserved
\****************************************************************************/
#ifdef G3D_SCENERENDERERTYPES_HPP
#error g3dSceneRendererTypes.hpp multiply included
#endif
#define G3D_SCENERENDERERTYPES_HPP

//============================================================================
//============================================================================
namespace g3dSceneRendererTypes
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	enum RendererType
	{
		e_Default,
		e_HDR,
		e_CubeMap,
		e_PlanarReflection,
		e_DepthMap,
		e_AmbientOcclusion,
		e_Depth,
		e_ShadowMask,
		e_Materials,
		e_VelocityMap,
		e_Normals, 
		e_IlluminationOnly,
		e_DirtyMatte,
		e_Wireframe,
		e_ReflectionOnly,
		e_GlobalIllumination,
		e_Glow,
		e_RmanColorBleed,
		e_MrayFinalGather
	};
};
