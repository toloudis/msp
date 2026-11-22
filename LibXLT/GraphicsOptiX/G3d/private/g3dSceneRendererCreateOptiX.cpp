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
#ifndef GOX_G3DTRACERENDEREROPTIX_HPP
#include "GraphicsOptiX/G3d/g3dTraceRendererOptiX.hpp"
#endif
#ifndef GOX_G3DGASCACHEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dGasCacheOptiX.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gox
{
	namespace
	{
		//	Whether the OptiX renderer can be made: the process's GAS cache
		//	and the Vulkan device exist
		bool tracer_available()
		{
			return g3dGasCacheOptiX::Get() && gvk::vkdGlobal::IsSet();
		}

		//	The OptiX renderer in a mode, on the process's GAS cache and
		//	Vulkan device (NULL without them: the inner creator's renderer
		//	instead)
		g3dSceneRenderer* create_tracer(g3dTraceRendererOptiX::Mode i_Mode)
		{
			g3dGasCacheOptiX* cache = g3dGasCacheOptiX::Get();
			if (!cache || !gvk::vkdGlobal::IsSet())
			{
				return NULL;
			}
			return new g3dTraceRendererOptiX(i_Mode, *cache, gvk::vkdGlobal::Device(), gvk::vkdGlobal::Context());
		}
	}

	g3dSceneRendererCreateOptiX::g3dSceneRendererCreateOptiX(g3dSceneRendererCreateImpl* i_pInner)
		: m_pInner(i_pInner)
	{
		DBG_ASSERT(m_pInner, "g3dSceneRendererCreateOptiX needs GraphicsVulkan's creator");
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateDefaultRenderer()
	{
		g3dSceneRenderer* renderer = create_tracer(g3dTraceRendererOptiX::e_Beauty);
		return renderer ? renderer : m_pInner->CreateDefaultRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateHDRRenderer()
	{
		g3dSceneRenderer* renderer = create_tracer(g3dTraceRendererOptiX::e_Beauty);
		return renderer ? renderer : m_pInner->CreateHDRRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateSimpleRenderer()
	{
		return m_pInner->CreateSimpleRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateTextureRenderer()
	{
		return m_pInner->CreateTextureRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateDepthMapRenderer()
	{
		return m_pInner->CreateDepthMapRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateRSMRenderer()
	{
		return m_pInner->CreateRSMRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateOpacityMapRenderer(g3dProjectedLight* pProjLight)
	{
		return m_pInner->CreateOpacityMapRenderer(pProjLight);
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame)
	{
		return m_pInner->CreateCubeMapRenderer(i_Obj, i_Material, i_RenderPerFrame);
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material)
	{
		return m_pInner->CreatePlanarReflectionRenderer(i_Obj, i_Material);
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateAmbientOcclusionRenderer()
	{
		g3dSceneRenderer* renderer = create_tracer(g3dTraceRendererOptiX::e_AO);
		return renderer ? renderer : m_pInner->CreateAmbientOcclusionRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateGlobalIlluminationRenderer()
	{
		return m_pInner->CreateGlobalIlluminationRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateDepthRenderer()
	{
		return m_pInner->CreateDepthRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateUVRenderer()
	{
		return m_pInner->CreateUVRenderer();
	}

	g3dPickRenderer* g3dSceneRendererCreateOptiX::CreatePickRenderer()
	{
		return m_pInner->CreatePickRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateShadowMaskRenderer()
	{
		g3dSceneRenderer* renderer = create_tracer(g3dTraceRendererOptiX::e_ShadowMask);
		return renderer ? renderer : m_pInner->CreateShadowMaskRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateIlluminationRenderer()
	{
		g3dSceneRenderer* renderer = create_tracer(g3dTraceRendererOptiX::e_Illumination);
		return renderer ? renderer : m_pInner->CreateIlluminationRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateNormalRenderer()
	{
		return m_pInner->CreateNormalRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateVelocityMapRenderer()
	{
		return m_pInner->CreateVelocityMapRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateMaterialsRenderer()
	{
		return m_pInner->CreateMaterialsRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateReflectionOnlyRenderer()
	{
		g3dSceneRenderer* renderer = create_tracer(g3dTraceRendererOptiX::e_ReflectionOnly);
		return renderer ? renderer : m_pInner->CreateReflectionOnlyRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateGlowRenderer()
	{
		return m_pInner->CreateGlowRenderer();
	}

	bool g3dSceneRendererCreateOptiX::NeedsShadowMaps()
	{
		return !tracer_available() && m_pInner->NeedsShadowMaps();
	}

	bool g3dSceneRendererCreateOptiX::NeedsOpacityMaps()
	{
		return !tracer_available() && m_pInner->NeedsOpacityMaps();
	}

	bool g3dSceneRendererCreateOptiX::NeedsReflectionMaps()
	{
		return !tracer_available() && m_pInner->NeedsReflectionMaps();
	}

	bool g3dSceneRendererCreateOptiX::UsesTracerSettings()
	{
		return tracer_available();
	}
}
