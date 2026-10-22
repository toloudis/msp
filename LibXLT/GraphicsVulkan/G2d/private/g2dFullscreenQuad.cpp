/*****************************************************************************
**  g2dFullscreenQuad.cpp
**
**	GraphicsVulkan port of GraphicsDX11's g2dFullscreenQuad.cpp.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsVulkan/G2d/g2dFullscreenQuad.hpp"
#ifndef GVK_FXEMBEDDEDSHADERSVK_HPP
#include "GraphicsVulkan/Fx/fxEmbeddedShadersVk.hpp"
#endif
#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDSHADER_HPP
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif

#include <memory>
#include <string>

namespace gvk
{
	//	The quad's shaders, embedded at build time by EmbedShadersVk.ps1
	//	(-TableName g2dQuadShadersVk) from G2d/private/Shaders
	namespace g2dQuadShadersVk
	{
		extern const fxEmbeddedEffect k_Effects[];
		extern const int k_NumEffects;
	}
}

#include "../../Fx/private/Generated/g2dQuadShadersVk.inc"

namespace gvk
{
	namespace
	{
		// Stuff used for drawing the "full screen quad"
		g2dFullscreenQuad::SCREEN_VERTEX	g_svQuad[4];
		std::unique_ptr<vkdShader>			g_pQuadVS;
		std::unique_ptr<vkdVertexLayout>	g_pQuadLayout;

		// Stuff used for drawing a texture with the quad
		std::unique_ptr<vkdShader>			g_pTexturedQuadPS;
		uint32_t							g_LinearClampSampler = 0;
		bool								g_bTexturedQuadFailed = false;

		std::unique_ptr<vkdShader> load_shader(const char* i_Effect, VkShaderStageFlagBits i_Stage, std::string& o_Error)
		{
			for (int e = 0; e < g2dQuadShadersVk::k_NumEffects; e++)
			{
				const fxEmbeddedEffect& effect = g2dQuadShadersVk::k_Effects[e];
				if (std::string(effect.m_Name) == i_Effect && effect.m_NumBytecode == 1)
				{
					const fxEmbeddedBytecode& code = effect.m_pBytecode[0];
					return vkdShader::Create(&vkdGlobal::Device(), i_Stage, code.m_pData, code.m_Size, code.m_Entry,
											 (std::string("g2dFullscreenQuad ") + code.m_Entry).c_str(), o_Error);
				}
			}
			o_Error = std::string("no embedded shader ") + i_Effect;
			return nullptr;
		}

		//--------------------------------------------------------------------
		// Create the pixel shader and states used by DrawTexturedQuad the
		// first time they are needed. Returns false if they can't be created.
		//--------------------------------------------------------------------
		bool init_textured_quad()
		{
			if (g_pTexturedQuadPS)
			{
				return true;
			}
			if (g_bTexturedQuadFailed)
			{
				return false;
			}
			std::string error;
			g_pTexturedQuadPS = load_shader("TexturedQuad", VK_SHADER_STAGE_FRAGMENT_BIT, error);
			if (!g_pTexturedQuadPS)
			{
				DBG_ERROR("Failed to create textured quad shader or states: " << error);
				g_bTexturedQuadFailed = true;
				return false;
			}
			//	D3D11_FILTER_MIN_MAG_MIP_LINEAR, clamp, MinLOD 0, MaxLOD D3D11_FLOAT32_MAX
			VkSamplerCreateInfo sampler = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
			sampler.magFilter = sampler.minFilter = VK_FILTER_LINEAR;
			sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			sampler.addressModeU = sampler.addressModeV = sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sampler.maxAnisotropy = 1.0f;
			sampler.minLod = 0.0f;
			sampler.maxLod = VK_LOD_CLAMP_NONE;
			g_LinearClampSampler = vkdGlobal::Bindings().AddSampler(sampler);
			return true;
		}
	}

	void g2dFullscreenQuad::InitFullscreenQuad()
	{
		std::string error;
		g_pQuadVS = load_shader("FullscreenQuad", VK_SHADER_STAGE_VERTEX_BIT, error);
		DBG_ASSERT(g_pQuadVS, "g2dFullscreenQuad: " << error);

		// the input layout, corresponds to the struct in the shader
		g_pQuadLayout.reset(new vkdVertexLayout({ { "SV_POSITION", 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0, 0 },
												  { "TEXCOORD", 0, VK_FORMAT_R32G32_SFLOAT, 0, 16 } },
												{ (uint32_t)sizeof(SCREEN_VERTEX) }));

		// Create a screen quad for render to texture operations
		g_svQuad[0].pos = maVector4d( -1.0f, 1.0f, 0.5f, 1.0f );
		g_svQuad[0].tex = maVector2d( 0.0f, 0.0f );
		g_svQuad[1].pos = maVector4d( -1.0f, -1.0f, 0.5f, 1.0f );
		g_svQuad[1].tex = maVector2d( 0.0f, 1.0f );
		g_svQuad[2].pos = maVector4d( 1.0f, 1.0f, 0.5f, 1.0f );
		g_svQuad[2].tex = maVector2d( 1.0f, 0.0f );
		g_svQuad[3].pos = maVector4d( 1.0f, -1.0f, 0.5f, 1.0f );
		g_svQuad[3].tex = maVector2d( 1.0f, 1.0f );
	}

	void g2dFullscreenQuad::CleanUpFullscreenQuad()
	{
		g_pQuadVS.reset();
		g_pQuadLayout.reset();
		g_pTexturedQuadPS.reset();
		g_bTexturedQuadFailed = false;
	}

	void g2dFullscreenQuad::DrawFullScreenQuad( uint32_t Width, uint32_t Height, float offsetPixelsX, float offsetPixelsY,
												SCREEN_VERTEX* i_InputVtxData )
	{
		vkdContext& context = vkdGlobal::Context();

		// Save the old viewport
		vkdViewport vpOld[vkdContext::c_MaxViewports];
		const uint32_t nViewPorts = context.GetViewports( vpOld );

		// Setup the viewport to match the backbuffer
		vkdViewport vp;
		vp.m_Width = (float)Width;
		vp.m_Height = (float)Height;
		vp.m_MinDepth = 0.0f;
		vp.m_MaxDepth = 1.0f;
		vp.m_X = offsetPixelsX;
		vp.m_Y = offsetPixelsY;
		context.SetViewports( 1, &vp );

		// The vertices go through the ring (DX11 rewrites a dynamic buffer and
		// writes the default quad back afterwards)
		const SCREEN_VERTEX* vertices = i_InputVtxData ? i_InputVtxData : g_svQuad;
		vkdRingBuffer::Allocation vb = context.Upload( vertices, 4 * sizeof(SCREEN_VERTEX), 16 );

		context.SetVertexLayout( g_pQuadLayout.get() );
		context.SetVertexBuffer( 0, vb.m_Buffer, vb.m_Offset );
		context.SetTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
		context.Draw( 4, 0 );
		G3D_PROFILE_DRAWS(1);

		// Restore the Old viewport
		context.SetViewports( nViewPorts, vpOld );
	}

	void g2dFullscreenQuad::DrawTexturedQuad( vkdImage* i_pTexture, uint32_t Width, uint32_t Height,
											  float offsetPixelsX, float offsetPixelsY )
	{
		if (!i_pTexture || !g_pQuadVS || !init_textured_quad())
		{
			return;
		}

		vkdContext& context = vkdGlobal::Context();
		const vkdShader* stages[vkdPipelineKey::c_Stages] = { g_pQuadVS.get(), nullptr, nullptr, nullptr, g_pTexturedQuadPS.get() };
		context.SetShaders( stages );
		vkdBindings& bindings = context.Bindings();
		bindings.SetTexture( 0, i_pTexture->SampledIndex( bindings ) );
		bindings.SetSampler( 0, g_LinearClampSampler );

		// D3D11: FILL_SOLID, CULL_NONE, DepthClipEnable; the rest zero
		vkdRasterizerState noCull;
		noCull.m_Cull = VK_CULL_MODE_NONE;
		context.SetRasterizerState( noCull );
		// OMSetBlendState( NULL, NULL, ... ): the default state, blend factors 1
		context.SetBlendState( vkdBlendState(), NULL );

		DrawFullScreenQuad( Width, Height, offsetPixelsX, offsetPixelsY );

		// unbind the texture so it can be used as a render target again
		bindings.SetTexture( 0, context.NullTexture()->SampledIndex( bindings ) );
	}
}
