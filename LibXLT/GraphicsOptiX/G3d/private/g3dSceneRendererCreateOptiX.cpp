/*****************************************************************************
**  g3dSceneRendererCreateOptiX.cpp
**
**      See g3dSceneRendererCreateOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DSCENERENDERERCREATEOPTIX_HPP
#include "GraphicsOptiX/G3d/private/g3dSceneRendererCreateOptiX.hpp"
#endif
#ifndef GOX_G3DSCENERENDEREROPTIX_HPP
#include "GraphicsOptiX/G3d/g3dSceneRendererOptiX.hpp"
#endif

namespace gox
{
	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateDefaultRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateHDRRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateSimpleRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateTextureRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateDepthMapRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateRSMRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateOpacityMapRenderer(g3dProjectedLight* /*pProjLight*/)
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateCubeMapRenderer(scObject* /*i_Obj*/, matMaterial* /*i_Material*/, bool /*i_RenderPerFrame*/)
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreatePlanarReflectionRenderer(scObject* /*i_Obj*/, matMaterial* /*i_Material*/)
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateAmbientOcclusionRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateGlobalIlluminationRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateDepthRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateUVRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dPickRenderer* g3dSceneRendererCreateOptiX::CreatePickRenderer()
	{
		return new g3dPickRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateShadowMaskRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateIlluminationRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateNormalRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateVelocityMapRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateMaterialsRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateReflectionOnlyRenderer()
	{
		return new g3dSceneRendererOptiX;
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateGlowRenderer()
	{
		return new g3dSceneRendererOptiX;
	}
}
