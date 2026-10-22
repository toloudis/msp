/*****************************************************************************
**  g2dFontUtilVk.cpp
**
**      GraphicsVulkan port of GraphicsDX11's g2dFontUtilDX11.cpp (the
**	BITMAP_FONT_SPRITE path; the DirectWrite path and the DXUT text code
**	after it are compiled out or unused in DX11 and not ported).
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G2DFONTUTILVK_HPP
#include "GraphicsVulkan/G2d/private/g2dFontUtilVk.hpp"
#endif

#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/g2d/g2dExceptionX.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif
#ifndef SCR_TEXT_HPP
#include "Graphics/scr/scrText.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/mat/matTextureMgr.hpp"
#endif
#ifndef GVK_TMESH_FRAG_HPP
#include "GraphicsVulkan/tmesh/tmeshFrag.hpp"
#endif
#ifndef GVK_TMESH_VERTEXBUFFER_HPP
#include "GraphicsVulkan/tmesh/tmeshVertexBuffer.hpp"
#endif
#ifndef GVK_TMESH_INDEXBUFFER_HPP
#include "GraphicsVulkan/tmesh/tmeshIndexBuffer.hpp"
#endif
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3DVKBUFFERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkBufferUtil.hpp"
#endif
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
#ifndef GVK_VKDSHADER_HPP
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#endif

#include <cfloat>
#include <map>
#include <memory>
#include <string>
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{
	//	The font shaders, embedded at build time by EmbedShadersVk.ps1 with
	//	g2dFullscreenQuad's (table g2dQuadShadersVk, from G2d/private/Shaders)
	namespace g2dQuadShadersVk
	{
		extern const fxEmbeddedEffect k_Effects[];
		extern const int k_NumEffects;
	}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
namespace
{
g3dBlendStateMgr::BlendState* stp_Blend;
g3dDepthStencilStateMgr::DepthStencilState* dsp_State;
float l_baseFontSize = 10.0f;
fsLocator l_GloablFontMap;

struct CB_PS_Color
{
	float m_Color[4];
};

// The shaders and input layouts (one per vertex stride: Vulkan bakes the
// stride into the pipeline, D3D11 takes it with the vertex buffer)
std::unique_ptr<vkdShader> m_pQuadVS;
std::unique_ptr<vkdShader> m_pQuadPS;
std::map<uint32_t, std::unique_ptr<vkdVertexLayout>> m_QuadLayouts;
uint32_t m_DefaultSampler = 0;

std::unique_ptr<vkdShader> load_shader(const char* i_Effect, VkShaderStageFlagBits i_Stage)
{
	for (int e = 0; e < g2dQuadShadersVk::k_NumEffects; e++)
	{
		const fxEmbeddedEffect& effect = g2dQuadShadersVk::k_Effects[e];
		if (std::string(effect.m_Name) == i_Effect && effect.m_NumBytecode == 1)
		{
			const fxEmbeddedBytecode& code = effect.m_pBytecode[0];
			std::string error;
			std::unique_ptr<vkdShader> shader = vkdShader::Create(&vkdGlobal::Device(), i_Stage, code.m_pData, code.m_Size,
				code.m_Entry, (std::string("g2dFontUtil ") + i_Effect).c_str(), error);
			if (!shader)
				DBG_ERROR(error);
			return shader;
		}
	}
	DBG_ERROR("no embedded shader " << i_Effect);
	return nullptr;
}

// the dx input layout, corresponds to the struct in the shader
const vkdVertexLayout* quad_layout(uint32_t i_Stride)
{
	std::unique_ptr<vkdVertexLayout>& layout = m_QuadLayouts[i_Stride];
	if (!layout)
	{
		layout.reset(new vkdVertexLayout({ { "SV_POSITION", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 0 },
										   { "NORMAL", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 12 },
										   { "TEXCOORD", 0, VK_FORMAT_R32G32_SFLOAT, 0, 24 } },
										 { i_Stride }));
	}
	return layout.get();
}

// the fragment's PrimitiveType (D3D11 topology numbers) as Vulkan's
VkPrimitiveTopology to_topology(tmeshFrag::PrimitiveType i_Type)
{
	switch (i_Type)
	{
		case tmeshFrag::e_LineList:			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case tmeshFrag::e_TriangleStrip:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		default:							return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}
}

}

//------------------------------------------------------------------------
//	g2dFontObjectVk
//------------------------------------------------------------------------
g2dFontObjectVk::g2dFontObjectVk(matTexture* i_FontMap):
	m_TextSize(10), m_FontMap(i_FontMap)
{
	m_TextSprite = new scrText();
	m_TextSprite->SetFontMapImage(i_FontMap, 320, 256);
	m_TextSprite->SetText(itString(""));
	m_TextSprite->SetScreenBased(true);
	m_TextSprite->SetTextSize( m_TextSize / l_baseFontSize);
	m_TextSprite->SetBackgroundColor(maFloatRGBA(0,0,0,0));
	m_TextSprite->SetForegroundColor(maFloatRGBA(1,1,1,1));
}

g2dFontObjectVk::~g2dFontObjectVk()
{
	m_FontMap = NULL;
	if (m_TextSprite)
		delete m_TextSprite;
	m_TextSprite = NULL;
}

void g2dFontObjectVk::DrawText(const itString& i_Text, const maFloatRGBA& i_Color, int i_OffsetX, int i_OffsetY)
{
	if (!m_pQuadVS || !m_pQuadPS)
		return;

	vkdContext& context = vkdGlobal::Context();

	// Save the old viewport
	vkdViewport vpOld[vkdContext::c_MaxViewports];
	const uint32_t nViewPorts = context.GetViewports( vpOld );

	// Setup Render state
	g3dBlendStateMgr::SetBlendState(stp_Blend);
	g3dDepthStencilStateMgr::SetDepthStencilState(dsp_State);
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// Setup the viewport to match the backbuffer
	vkdViewport vp;
	vp.m_Width = (float)200;
	vp.m_Height = (float)150;
	vp.m_MinDepth = 0.0f;
	vp.m_MaxDepth = 1.0f;
	vp.m_X = (float)i_OffsetX;
	vp.m_Y = (float)i_OffsetY;
	context.SetViewports( 1, &vp );

	m_TextSprite->SetTextSize( m_TextSize / l_baseFontSize);
	m_TextSprite->SetForegroundColor(i_Color);
	m_TextSprite->SetText(i_Text);

	// (DX11 dereferences the fragment unchecked; an empty text has none)
	const tmeshFrag* pFrag = dynamic_cast<const tmeshFrag*>( m_TextSprite->GetSceneNode()->GetFragment() );
	if (!pFrag)
	{
		context.SetViewports( nViewPorts, vpOld );
		return;
	}

	int numInds = 0;
	if( pFrag->GetPrimitiveType() == tmeshFrag::e_TriangleList )
	{
		numInds = pFrag->GetNumNonShadowIndices();
	}
	else
	{
		numInds = pFrag->GetNumIndices();
	}

	int startIndex = 0;
	if (pFrag->GetSubFragments().size() > 0)
	{
		startIndex = pFrag->GetSubFragments()[m_TextSprite->GetSceneNode()->GetSubFragment()].m_startIndex;
		numInds = pFrag->GetSubFragments()[m_TextSprite->GetSceneNode()->GetSubFragment()].m_numTris * 3; // assumes tri list.
	}

	context.SetVertexLayout(quad_layout((uint32_t)pFrag->GetVertexStride()));
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceSize offset = 0;
	pFrag->GetVertexBuffer()->GetVertexBuffer()->GetBinding(context, buffer, offset);
	context.SetVertexBuffer(0, buffer, offset);
	pFrag->GetIndexBuffer()->GetIndexBuffer()->GetBinding(context, buffer, offset);
	context.SetIndexBuffer(buffer, offset,
		(pFrag->GetIndexBuffer()->GetSizeOfIndex() == 2) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
	context.SetTopology(to_topology(pFrag->GetPrimitiveType()));

	const vkdShader* stages[vkdPipelineKey::c_Stages] = { m_pQuadVS.get(), nullptr, nullptr, nullptr, m_pQuadPS.get() };
	context.SetShaders( stages );

	context.SetShaderResource(0, g3dVkTextureUtil::GetImage(m_FontMap));
	// DX11 binds no sampler and samples with whatever is at s0; this binds
	// D3D11's default sampler state (what a context with none bound uses)
	context.Bindings().SetSampler(0, m_DefaultSampler);

	// set draw color into constnat buffer
	CB_PS_Color PSColor;
	PSColor.m_Color[0] = i_Color.GetRed();
	PSColor.m_Color[1] = i_Color.GetGreen();
	PSColor.m_Color[2] = i_Color.GetBlue();
	PSColor.m_Color[3] = i_Color.GetAlpha();
	vkdRingBuffer::Allocation cb = context.Upload(&PSColor, sizeof(PSColor));
	context.Bindings().SetConstantBuffer(0, cb.m_Gpu, sizeof(PSColor));

	context.DrawIndexed(numInds, startIndex, 0);
	G3D_PROFILE_DRAWS(1);

	// Restore the Old viewport
	context.SetViewports( nViewPorts, vpOld );
}

void g2dFontObjectVk::SetFontMap(matTexture* i_fontMap)
{
	m_FontMap = i_fontMap;

	if (m_TextSprite)
		m_TextSprite->SetFontMapImage(i_fontMap, 320, 256);
}

void g2dFontObjectVk::Init()
{
	stp_Blend = new g3dBlendStateMgr::BlendState(false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT);

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );
	dsp_State = new g3dDepthStencilStateMgr::DepthStencilState(
		false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);

	InitDefaultShader();
}
void g2dFontObjectVk::Cleanup()
{
	delete dsp_State;
	delete stp_Blend;
	dsp_State = NULL;
	stp_Blend = NULL;

	CleanupDefaultShader();
}

void g2dFontObjectVk::InitDefaultShader()
{
	// the shaders are compiled at build time (FontVS.hlsl, FontPS.hlsl)
	m_pQuadVS = load_shader("FontVS", VK_SHADER_STAGE_VERTEX_BIT);
	m_pQuadPS = load_shader("FontPS", VK_SHADER_STAGE_FRAGMENT_BIT);

	//	D3D11's default sampler: MIN_MAG_MIP_LINEAR, clamp, MipLODBias 0,
	//	MaxAnisotropy 1, NEVER, border (1,1,1,1), LOD -FLT_MAX..FLT_MAX
	VkSamplerCreateInfo sampler = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
	sampler.magFilter = sampler.minFilter = VK_FILTER_LINEAR;
	sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler.addressModeU = sampler.addressModeV = sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler.maxAnisotropy = 1.0f;
	sampler.compareOp = VK_COMPARE_OP_NEVER;
	sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	sampler.minLod = -FLT_MAX;
	sampler.maxLod = VK_LOD_CLAMP_NONE;
	m_DefaultSampler = vkdGlobal::Bindings().AddSampler(sampler);
}

void g2dFontObjectVk::CleanupDefaultShader()
{
	m_QuadLayouts.clear();
	m_pQuadPS.reset();
	m_pQuadVS.reset();
}

//------------------------------------------------------------------------
//	g2dFontUtilVk
//------------------------------------------------------------------------
g2dFontUtilVk* g2dFontUtilVk::Implementation()
{
	return dynamic_cast<g2dFontUtilVk*>(g2dFontUtil::Implementation());
}

//------------------------------------------------------------------------
//	(DX11 adds a g2dResetHandler whose Deallocate/Reallocate only log;
//	Vulkan has no device reset)
//------------------------------------------------------------------------
g2dFontUtilVk::g2dFontUtilVk()
{
}

g2dFontUtilVk::~g2dFontUtilVk()
{
}

//------------------------------------------------------------------------
//	LoadFont make the given font name available for use and returns
//	a g2dFontHandle which can be used later.  When you are done with the
//	font, call ReleaseFont.
//------------------------------------------------------------------------
g2dFontHandle g2dFontUtilVk::LoadFont(const itString& /*i_Name*/, int i_Size)
{
	fsLocator filePath = l_GloablFontMap;
	//fsFileUtil::UnicodeStringToLocator(i_Name, filePath);

	if (!fsFileUtil::FileExists(filePath))
		return NULL;

	bool bWasSkipTextures = matTextureMgr::IsSkipAllTextures();
	matTextureMgr::SetSkipAllTextures(false);
	matTexture* pTextFormat = matTextureMgr::LoadTexture(filePath);
	matTextureMgr::SetSkipAllTextures(bWasSkipTextures);

	if (!pTextFormat)
		throw g2dUnknownFontX(filePath.GetLastName());

	g2dFontObjectVk* fontObj = new g2dFontObjectVk(pTextFormat);
	fontObj->SetTextSize( (float)i_Size );

	g2dFontHandle ret_val = reinterpret_cast<g2dFontHandle>(fontObj);

	FontMapIt it = m_Fonts.find(ret_val);
	if( it != m_Fonts.end() )
	{
		//	increase ref count
		++(it->second.m_Reference);
	}
	else
	{
		//	make new font and add
		FontInfo info;
		info.m_Reference = 1;
		info.m_Font = fontObj;

		m_Fonts[ret_val] = info;
	}

	return ret_val;
}

//------------------------------------------------------------------------
//	ReleaseFont informs the system that you are done with the given
//	g2dFontHandle.
//	(DX11 also calls Win32's DeleteObject on the handle, which isn't a GDI
//	object; not ported.)
//------------------------------------------------------------------------
void g2dFontUtilVk::ReleaseFont(g2dFontHandle i_Handle)
{
	if (reinterpret_cast<g2dFontObjectVk*>(i_Handle) == NULL)
		return;

	FontMapIt it = m_Fonts.find(i_Handle);

	if( it != m_Fonts.end() )
	{
		--(it->second.m_Reference);
		if( it->second.m_Reference == 0 )
		{
			delete it->second.m_Font;
			it->second.m_Font = NULL;
			m_Fonts.erase(it);
		}
	}
	else
		DBG_ASSERT(false, "Unknown g2dFontHandle");
}

//------------------------------------------------------------------------
// GetLoadedFontName returns the name of the font being used for the given
// g2dFontHandle.  It looks this up in the system to learn exactly what
// the system loaded.
//------------------------------------------------------------------------
void g2dFontUtilVk::GetLoadedFontName( g2dFontHandle /*i_Font*/, itString& /*o_Name*/ )
{
	// does this function need to be implemented?
}

void g2dFontUtilVk::GetLoadedFontName( g2dFontHandle /*i_Font*/, std::string& /*o_Name*/ )
{
}

//------------------------------------------------------------------------
//	GetTextSizeInPixels returns the width and height in pixels of the given text and font
//------------------------------------------------------------------------
void g2dFontUtilVk::GetTextSizeInPixels(g2dFontHandle /*i_Font*/, const itString& /*i_Text*/, int& o_Width, int& o_Height)
{
	o_Width = 1;
	o_Height = 1;
}

//------------------------------------------------------------------------
//	Get and set the global font path
//------------------------------------------------------------------------
void g2dFontUtilVk::SetGlobalFontBitmap(const fsLocator& i_FontPath)
{
	l_GloablFontMap = i_FontPath;
}

void g2dFontUtilVk::GetGlobalFontBitmap(fsLocator& o_FontPath)
{
	o_FontPath = l_GloablFontMap;
}

}
