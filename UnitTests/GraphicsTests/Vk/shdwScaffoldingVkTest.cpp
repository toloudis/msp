/*****************************************************************************
**  shdwScaffoldingVkTest.cpp
**
**      The pieces of GraphicsVulkan's shdw package the renderers stand on,
**      against GraphicsDX11's: the frame buffer manager's surfaces (size,
**      format and samples of each, at two sizes, with and without
**      overscan and HDR anti-aliasing, and when the settings change on
**      one manager), the pass traversal's node lists (the same nodes in
**      the same order, with the same draw styles and state caches), and
**      g3dRenderLayer drawing an overlay layer (compared as images).
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"

#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/G3d/g3dLayer.hpp"
#endif
#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif

#ifndef SHDW_FRAMEBUFFERMGR_HPP
#include "GraphicsDX11/shdw/shdwFrameBufferMgr.hpp"
#endif
#ifndef SHDW_PASSTRAVERSAL_HPP
#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"
#endif
#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif
#ifndef G3D_SCENERENDERUTIL_HPP
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef G3D_TRANSPARENCYSORTDX11_HPP
#include "GraphicsDX11/g3d/g3dTransparencySortDX11.hpp"
#endif
#ifndef MAT_RENDERTARGETTEXTURE_HPP
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#endif

#ifndef GVK_SHDW_FRAMEBUFFERMGR_HPP
#include "GraphicsVulkan/shdw/shdwFrameBufferMgr.hpp"
#endif
#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif
#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif

#include <d3d11.h>
#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

//============================================================================
//	Frame buffer surfaces
//============================================================================
namespace
{
	struct Surface
	{
		std::string	m_Name;
		bool		m_bExists = false;
		int			m_Width = 0, m_Height = 0;
		uint32_t	m_Dxgi = 0;			// the colour image's format
		int			m_Samples = 0;
		bool		m_bDepth = false;
		int			m_DepthSamples = 0;

		std::string Describe() const
		{
			std::ostringstream text;
			if (!m_bExists)
				return m_Name + ": none";
			text << m_Name << ": " << m_Width << "x" << m_Height << " DXGI " << m_Dxgi << " x" << m_Samples;
			if (m_bDepth)
				text << ", depth x" << m_DepthSamples;
			return text.str();
		}
	};

	int samples_of(ID3D11View* i_pView)
	{
		ID3D11Resource* resource = NULL;
		i_pView->GetResource(&resource);
		ID3D11Texture2D* texture = NULL;
		int samples = 0;
		if (resource && SUCCEEDED(resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&texture)))
		{
			D3D11_TEXTURE2D_DESC desc;
			texture->GetDesc(&desc);
			samples = (int)desc.SampleDesc.Count;
			texture->Release();
		}
		if (resource)
			resource->Release();
		return samples;
	}

	Surface describe(::matRenderTargetTexture* i_pTexture)
	{
		Surface s;
		if (!i_pTexture)
			return s;
		s.m_bExists = true;
		i_pTexture->GetDimensions(s.m_Width, s.m_Height);
		if (ID3D11RenderTargetView* color = i_pTexture->GetColorBuffer())
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			color->GetDesc(&desc);
			s.m_Dxgi = (uint32_t)desc.Format;
			s.m_Samples = samples_of(color);
		}
		if (ID3D11DepthStencilView* depth = i_pTexture->GetDepthBuffer())
		{
			s.m_bDepth = true;
			s.m_DepthSamples = samples_of(depth);
		}
		return s;
	}

	Surface describe(gvk::matRenderTargetTexture* i_pTexture)
	{
		Surface s;
		if (!i_pTexture)
			return s;
		s.m_bExists = true;
		i_pTexture->GetDimensions(s.m_Width, s.m_Height);
		if (gvk::vkdImage* color = i_pTexture->GetColorBuffer())
		{
			s.m_Dxgi = gvk::vkdDxgiFromFormat(color->Format());
			s.m_Samples = (int)color->Samples();
		}
		if (gvk::vkdImage* depth = i_pTexture->GetDepthBuffer())
		{
			s.m_bDepth = true;
			s.m_DepthSamples = (int)depth->Samples();
		}
		return s;
	}

	//	Every surface the manager has, in a fixed order
	template <class Mgr>
	std::vector<Surface> surfaces(Mgr& i_Mgr)
	{
		std::vector<Surface> list;
		auto add = [&list](const std::string& i_Name, auto* i_pTexture)
		{
			Surface s = describe(i_pTexture);
			s.m_Name = i_Name;
			list.push_back(s);
		};
		add("HDRRenderTarget", i_Mgr.HDRRenderTarget());
		add("HDRRenderTargetTex", i_Mgr.HDRRenderTargetTex());
		add("HDRAATarget", i_Mgr.HDRAATarget());
		add("HDRAAScratchTarget", i_Mgr.HDRAAScratchTarget());
		add("DOFReserveTarget", i_Mgr.DOFReserveTarget());
		add("DOFBlurTarget", i_Mgr.DOFBlurTarget());
		add("DOFHBlurTarget", i_Mgr.DOFHBlurTarget());
		add("HDRBufferBackup", i_Mgr.HDRBufferBackup());
		add("HDRScratchTex0", i_Mgr.HDRScratchTex0());
		add("HDRScratchTex1", i_Mgr.HDRScratchTex1());
		add("HDRScratchTex2", i_Mgr.HDRScratchTex2());
		add("DepthBuffer", i_Mgr.DepthBuffer());
		add("DepthBuffer2", i_Mgr.DepthBuffer2());
		add("MultiDepthBuffer", i_Mgr.MultiDepthBuffer());
		add("NormalsBuffer", i_Mgr.NormalsBuffer());
		add("VelocityBuffer", i_Mgr.VelocityBuffer());
		add("TransDepthBuffer1", i_Mgr.TransDepthBuffer1());
		add("TransDepthBuffer2", i_Mgr.TransDepthBuffer2());
		add("TransDepthAux", i_Mgr.TransDepthAux());
		add("HDRScaledTex", i_Mgr.HDRScaledTex());
		add("TexBrightPass", i_Mgr.TexBrightPass());
		add("TexBloomSource", i_Mgr.TexBloomSource());
		add("TexStarSource", i_Mgr.TexStarSource());
		for (int i = 0; i < i_Mgr.GetNumTonemapTextures(); i++)
			add("TexToneMap" + std::to_string(i), i_Mgr.TexToneMap().m_apTexToneMap[i]);
		for (int i = 0; i < i_Mgr.GetNumBloomTextures(); i++)
			add("TexBloom" + std::to_string(i), i_Mgr.TexBloom().m_apTexBloom[i]);
		for (int i = 0; i < i_Mgr.GetNumStarTextures(); i++)
			add("TexStar" + std::to_string(i), i_Mgr.TexStar().m_apTexStar[i]);
		for (int i = 0; i < 4; i++)
			add("OSM" + std::to_string(i), i_Mgr.OSM().m_pOSM[i]);
		return list;
	}

	struct Setting
	{
		int		m_Width, m_Height, m_Overscan;
		bool	m_bAA;
	};

	std::string name_of(const Setting& i_Setting)
	{
		return std::to_string(i_Setting.m_Width) + "x" + std::to_string(i_Setting.m_Height)
			+ " overscan " + std::to_string(i_Setting.m_Overscan) + (i_Setting.m_bAA ? " AA" : "");
	}

	//	The surfaces for each setting (a new manager each), then for the
	//	settings in turn on one manager (its surfaces recreated)
	template <class Mgr>
	std::vector<std::vector<Surface>> all_surfaces(const std::vector<Setting>& i_Settings)
	{
		std::vector<std::vector<Surface>> result;
		const float clear[4] = { 0, 0, 0, 0 };
		for (const Setting& setting : i_Settings)
		{
			gtPrefsScope prefs;
			g3dPrefs::CurrentPrefs().m_bHDRAA = setting.m_bAA;
			gtTarget target(gtBackend::Current(), setting.m_Width, setting.m_Height, clear);
			Mgr mgr;
			mgr.CreateSurfaces(target.Target(), setting.m_Overscan);
			result.push_back(surfaces(mgr));
		}
		Mgr shared;
		for (const Setting& setting : i_Settings)
		{
			gtPrefsScope prefs;
			g3dPrefs::CurrentPrefs().m_bHDRAA = setting.m_bAA;
			gtTarget target(gtBackend::Current(), setting.m_Width, setting.m_Height, clear);
			shared.CreateSurfaces(target.Target(), setting.m_Overscan);
			result.push_back(surfaces(shared));
		}
		return result;
	}
}

TEST(shdwScaffoldingVkTest, FrameBufferSurfacesMatchDX11)
{
	std::vector<Setting> settings;
	for (const int size : { 0, 1 })
		for (const int overscan : { 0, 16 })
			for (const bool aa : { false, true })
				settings.push_back(Setting{ size ? 1920 : 256, size ? 1080 : 256, overscan, aa });

	std::vector<std::vector<Surface>> dx11, vulkan;
	gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = all_surfaces<::shdwFrameBufferMgr>(settings); });
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = all_surfaces<gvk::shdwFrameBufferMgr>(settings); }))
		return;
	ASSERT_EQ(dx11.size(), vulkan.size());
	for (size_t i = 0; i < dx11.size(); i++)
	{
		const std::string setting = name_of(settings[i % settings.size()]) + (i < settings.size() ? "" : " (shared manager)");
		SCOPED_TRACE(setting);
		ASSERT_EQ(dx11[i].size(), vulkan[i].size());
		for (size_t s = 0; s < dx11[i].size(); s++)
		{
			EXPECT_EQ(dx11[i][s].Describe(), vulkan[i][s].Describe());
		}
		EXPECT_TRUE(dx11[i][0].m_bExists) << "DX11 made no HDR target";
	}
}

//============================================================================
//	Pass traversal
//============================================================================
namespace
{
	matMaterial* phong(float i_Transparency)
	{
		return gtScene::DataMaterial("Phong.fx", [=](effShaderData* i_pData)
		{
			gtScene::SetPhong(i_pData, gtScene::e_NonDefault);
			dynamic_cast<effPhongData*>(i_pData)->m_Transparency = i_Transparency;
		});
	}

	//	Opaque, transparent, non-solid, glowing, outlined, hidden,
	//	inactive (the invisible mask) and out-of-view nodes, some nested
	void traversal_scene(gtSceneBuilder& io_Builder)
	{
		const gtMesh sphere = gtScene::Sphere(8, 12);
		//	(the Phong data materials count as transparent whatever their
		//	transparency: material shaders are the opaque ones)
		matMaterial* opaque = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_NonDefault));
		matMaterial* transparent = io_Builder.Keep(phong(0.5f));
		matMaterial* blinn = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_NonDefault));
		matMaterial* glow = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_NonDefault));
		glow->SetHasGlow(true);
		glow->GlowData().m_GlowAmount = 1.0f;
		matMaterial* outline = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_NonDefault));
		outline->SetHasOutline(true);
		matMaterial* transparent_glow = io_Builder.Keep(phong(0.4f));
		transparent_glow->SetHasGlow(true);
		transparent_glow->GlowData().m_GlowAmount = 0.5f;

		io_Builder.Add(sphere, opaque, gtScene::Place(-1.5f, 1.5f, 0, 0.4f));
		io_Builder.Add(sphere, transparent, gtScene::Place(-0.5f, 1.5f, 0, 0.4f));
		io_Builder.Add(sphere, blinn, gtScene::Place(0.5f, 1.5f, 0, 0.4f));
		io_Builder.Add(sphere, opaque, gtScene::Place(1.5f, 1.5f, 0, 0.4f))->SetDrawStyle(g3dSceneNode::e_Wireframe);
		io_Builder.Add(sphere, glow, gtScene::Place(-1.5f, 0.5f, 0, 0.4f));
		io_Builder.Add(sphere, outline, gtScene::Place(-0.5f, 0.5f, 0, 0.4f));
		io_Builder.Add(sphere, transparent_glow, gtScene::Place(0.5f, 0.5f, 1, 0.4f));
		io_Builder.Add(sphere, opaque, gtScene::Place(1.5f, 0.5f, 0, 0.4f))->SetRenderable(false);
		io_Builder.Add(sphere, opaque, gtScene::Place(-1.5f, -0.5f, 0, 0.4f))->SetActiveInRenderLayer(false);
		io_Builder.Add(sphere, transparent, gtScene::Place(-0.5f, -0.5f, 0, 0.4f))->SetActiveInRenderLayer(false);
		io_Builder.Add(sphere, opaque, gtScene::Place(40, 0, 0, 0.4f));		// out of view
		io_Builder.Add(sphere, transparent, gtScene::Place(0.5f, -0.5f, -2, 0.4f));
		io_Builder.Add(sphere, blinn, gtScene::Place(1.5f, -0.5f, 0, 0.4f))->SetDrawStyle(g3dSceneNode::e_LitWireframe);
		io_Builder.PointLight(maPoint3d(2, 3, 4), maFloatRGBA(1, 1, 1, 1));
	}

	//	Each list as "<node index>:<draw style>:<state cache>" entries
	template <class Traversal, class NodeList, class SortList>
	std::string describe_lists(Traversal& i_Traversal, const std::map<const g3dSceneNode*, int>& i_Index)
	{
		std::ostringstream text;
		auto index_of = [&](const g3dSceneNode* i_pNode)
		{
			auto found = i_Index.find(i_pNode);
			return found == i_Index.end() ? -1 : found->second;
		};
		auto list = [&](const char* i_pName, const NodeList& i_List)
		{
			text << i_pName << ":";
			for (const auto& node : i_List)
				text << " " << index_of(node.m_pNode) << ":" << (int)node.m_drawStyle << ":" << node.m_StateCache;
			text << "\n";
		};
		auto sorted = [&](const char* i_pName, const SortList& i_List)
		{
			text << i_pName << ":";
			for (const auto& node : i_List)
				text << " " << index_of(node.m_pSceneNode) << ":" << node.m_RenderStateCache << ":" << node.m_bIsMask;
			text << "\n";
		};
		list("shadow", i_Traversal.GetShadowNodes());
		list("non-shadow", i_Traversal.GetNonShadowNodes());
		list("non-solid", i_Traversal.GetNonSolidNodes());
		list("glow", i_Traversal.GetGlowNodes());
		list("outline", i_Traversal.GetOutlineNodes());
		list("invisible mask black", i_Traversal.GetInvisibleMaskBlackNodes());
		sorted("transparent", i_Traversal.GetTransparentNodes()->GetTransparentNodes(false));
		sorted("transparent and masks", i_Traversal.GetTransparentNodes()->GetTransparentNodes(true));
		sorted("masks", i_Traversal.GetTransparentNodes()->GetTransparentMaskNodes());
		sorted("hair", i_Traversal.GetHairNodes()->GetTransparentNodes(false));
		text << "materials:";
		for (const std::string& name : *i_Traversal.GetMaterialArray())
			text << " " << name;
		text << "\n";
		return text.str();
	}

	//	The lists for single-light rendering on and off
	template <class Traversal, class NodeList, class SortList, class Util>
	std::vector<std::string> traverse_all()
	{
		std::vector<std::string> result;
		for (const bool single_light : { true, false })
		{
			gtPrefsScope prefs;
			g3dSingleLightRendering::SetDoSingleLightRendering(single_light);
			g3dPrefs::CurrentPrefs().m_bEnableGlow = true;
			g3dPrefs::CurrentPrefs().m_bEnableOutline = true;
			g3dPrefs::CurrentPrefs().m_bEnableInvisibleMaskBlack = true;

			const float clear[4] = { 0, 0, 0, 0 };
			gtTarget target(gtBackend::Current(), 256, 256, clear);
			gtSceneBuilder builder;
			traversal_scene(builder);
			std::map<const g3dSceneNode*, int> index;
			const std::vector<g3dSceneNode*>& nodes = builder.Layer().GetRootNode()->GetChildren();
			for (size_t i = 0; i < nodes.size(); i++)
				index[nodes[i]] = (int)i;

			Util::SetViewingTransforms(builder.Camera());
			Traversal traversal;
			traversal.TraverseLayer(1.25f, &builder.Layer(), &builder.Camera());
			result.push_back(describe_lists<Traversal, NodeList, SortList>(traversal, index));
			traversal.Clear();
		}
		return result;
	}

	struct UtilDX11 { static void SetViewingTransforms(const camCamera& i_Camera) { g3dSceneRenderUtil::SetViewingTransforms(i_Camera); } };
	struct UtilVk { static void SetViewingTransforms(const camCamera& i_Camera) { gvk::g3dSceneRenderUtil::SetViewingTransforms(i_Camera); } };
}

TEST(shdwScaffoldingVkTest, TraversalListsMatchDX11)
{
	std::vector<std::string> dx11, vulkan;
	gtBackend::Run(gtBackend::e_DX11, [&]()
	{
		dx11 = traverse_all<::shdwPassTraversal, ::nodeCacheList, ::TranspNodeVector, UtilDX11>();
	});
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]()
	{
		vulkan = traverse_all<gvk::shdwPassTraversal, gvk::nodeCacheList, gvk::TranspNodeVector, UtilVk>();
	}))
		return;
	ASSERT_EQ(2u, dx11.size());
	ASSERT_EQ(2u, vulkan.size());
	EXPECT_EQ(dx11[0], vulkan[0]) << "single-light rendering on";
	EXPECT_EQ(dx11[1], vulkan[1]) << "single-light rendering off";
	//	the scene reaches every list it was built for
	EXPECT_NE(dx11[0].find("shadow: 0"), std::string::npos) << dx11[0];
	EXPECT_NE(dx11[1].find("non-shadow: 0"), std::string::npos) << dx11[1];
	EXPECT_EQ(dx11[0].find("glow:\n"), std::string::npos) << dx11[0];
	EXPECT_EQ(dx11[0].find("outline:\n"), std::string::npos) << dx11[0];
	EXPECT_EQ(dx11[0].find("non-solid:\n"), std::string::npos) << dx11[0];
	EXPECT_EQ(dx11[0].find("invisible mask black:\n"), std::string::npos) << dx11[0];
	EXPECT_EQ(dx11[0].find("masks:\n"), std::string::npos) << dx11[0];
}

//============================================================================
//	g3dRenderLayer
//============================================================================
namespace
{
	//	Draws the scene's overlay layer with g3dRenderLayer (DX11's or
	//	Vulkan's) over the target, as the HDR renderer draws extra layers
	template <class RenderLayer>
	class LayerRenderer : public g3dSceneRenderer
	{
		public:
			virtual int Render(g2dRenderTarget* i_pWindow, const camCamera& i_Camera, const g3dScene& i_Scene,
							   const std::vector<g3dLayer*>&, float i_fSimTime)
			{
				i_pWindow->MakeCurrent();
				RenderLayer pass;
				pass.Set(const_cast<g3dLayer*>(i_Scene.GetLayer(1)), &i_Camera, i_pWindow);
				return pass.Render(i_fSimTime);
			}
			virtual void ReleaseResources() {}
	};

	g3dSceneRenderer* layer_renderer()
	{
		if (gtBackend::Current() == gtBackend::e_DX11)
			return new LayerRenderer<::g3dRenderLayer>();
		return new LayerRenderer<gvk::g3dRenderLayer>();
	}

	void overlay(gtSceneBuilder& io_Builder, g3dLayer::SortMethod i_Sort, g3dLayer::BlendMethod i_Blend)
	{
		const gtMesh sphere = gtScene::Sphere(16, 32);
		matMaterial* blinn = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_NonDefault));
		matMaterial* transparent = io_Builder.Keep(phong(0.5f));
		io_Builder.Add(sphere, blinn, gtScene::Place(0, 0, 0, 1.0f));		// in the world layer: not drawn
		//	camera space looks down +Z
		io_Builder.AddLayer(sphere, blinn, gtScene::Place(-0.8f, 0.4f, 6, 0.9f));
		io_Builder.AddLayer(sphere, transparent, gtScene::Place(0.6f, -0.3f, 5, 0.9f));
		io_Builder.AddLayer(gtScene::Plane(), blinn, gtScene::Place(0, 0, 8, 3.0f));
		io_Builder.OverlayLayer()->SetSortMethod(i_Sort);
		io_Builder.OverlayLayer()->SetBlendMethod(i_Blend);
		io_Builder.PointLight(maPoint3d(2, 3, 4), maFloatRGBA(0.9f, 0.8f, 0.7f, 1), 25.0f);
		io_Builder.DirectionalLight(maVector3d(-0.3f, -0.8f, -0.5f), maFloatRGBA(0.4f, 0.5f, 0.6f, 1));
	}
}

TEST(shdwScaffoldingVkTest, RenderLayerMatchesDX11)
{
	gtRenderOptions options;
	options.m_CreateRenderer = layer_renderer;
	gtExpectSameImages("RenderLayerMatchesDX11",
	{
		{ "zbuffer", [](gtSceneBuilder& b, const std::filesystem::path&) { overlay(b, g3dLayer::e_ZBuffer, g3dLayer::e_Multiplicative); }, options },
		{ "zsort additive", [](gtSceneBuilder& b, const std::filesystem::path&) { overlay(b, g3dLayer::e_ZSort, g3dLayer::e_Additive); }, options },
	});
}
