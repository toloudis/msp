/****************************************************************************\
**	g3dSceneRendererCreateVk.cpp
**
**	See g3dSceneRendererCreateVk.hpp.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G3D_SCENERENDERERCREATEVK_HPP
#include "GraphicsVulkan/G3d/g3dSceneRendererCreateVk.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dSceneRendererVk.hpp"
#endif
#ifndef GVK_G3DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g3dStubsVk.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif

namespace gvk
{

namespace
{
	//	The forward renderer standing in for a renderer type not ported yet
	g3dSceneRenderer* forward(const char* i_pType)
	{
		GVK_NOT_YET(i_pType);
		return new g3dSceneRendererVk();
	}
}

g3dSceneRenderer* g3dSceneRendererCreateVk::CreateDefaultRenderer()		{ return forward("default renderer (shdw; forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateHDRRenderer()			{ return forward("HDR renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateSimpleRenderer()		{ return forward("simple renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateTextureRenderer()		{ return forward("texture renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateDepthMapRenderer()	{ return forward("depth map renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateRSMRenderer()			{ return forward("RSM renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateOpacityMapRenderer(g3dProjectedLight*)	{ return forward("opacity map renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateCubeMapRenderer(scObject*, matMaterial*, bool)	{ return forward("cube map renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreatePlanarReflectionRenderer(scObject*, matMaterial*)	{ return forward("planar reflection renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateAmbientOcclusionRenderer()	{ return forward("ambient occlusion renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateGlobalIlluminationRenderer()	{ return forward("global illumination renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateDepthRenderer()		{ return forward("depth renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateUVRenderer()			{ return forward("UV renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateShadowMaskRenderer()	{ return forward("shadow mask renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateIlluminationRenderer()	{ return forward("illumination renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateNormalRenderer()		{ return forward("normal renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateVelocityMapRenderer()	{ return forward("velocity map renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateMaterialsRenderer()	{ return forward("materials renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateReflectionOnlyRenderer()	{ return forward("reflection-only renderer (forward renderer used)"); }
g3dSceneRenderer* g3dSceneRendererCreateVk::CreateGlowRenderer()		{ return forward("glow renderer (forward renderer used)"); }

//	Not a scene renderer: the stub picks nothing (Plan 6)
g3dPickRenderer* g3dSceneRendererCreateVk::CreatePickRenderer()			{ return new g3dPickRendererVk; }

}
