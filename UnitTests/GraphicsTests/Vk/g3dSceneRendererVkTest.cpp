/*****************************************************************************
**  g3dSceneRendererVkTest.cpp
**
**      The forward renderer (g3dSceneRendererVk) against GraphicsDX11's
**      (g3dSceneRendererDX11): the same synthetic scene (gtSceneBuilder)
**      rendered by each into a 256x256 render-target texture, compared
**      with gfxcompare's tolerance. Lit spheres on every material class
**      (with zero and very large values), a textured scene, sorted
**      transparent nodes, fog, the wireframe draw styles and hardware
**      tessellation (the PTess passes). Also: every renderer type the
**      Vulkan creator can't build yet gives the forward renderer, and the
**      default and HDR types give the HDR renderer.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"
#include "gtTempDir.hpp"

#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/G3d/g3dLayer.hpp"
#endif
#ifndef G3D_LIGHTMGR_HPP
#include "Graphics/G3d/g3dLightMgr.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/Mat/matShaderMgr.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef EFF_SOLIDDATA_HPP
#include "Graphics/Eff/effSolidData.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#ifndef G3D_SCENERENDERERDX11_HPP
#include "GraphicsDX11/g3d/g3dSceneRendererDX11.hpp"
#endif

#ifndef GVK_G3D_SCENERENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dSceneRendererVk.hpp"
#endif
#ifndef GVK_G3D_DEPTHMAPRENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dDepthMapRendererVk.hpp"
#endif
#ifndef GVK_G3D_RSMRENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dRSMRendererVk.hpp"
#endif
#ifndef GVK_SHDW_HDRRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwHDRRendererVk.hpp"
#endif
#ifndef GVK_SHDW_SHADOWLAYERRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwShadowLayerRendererVk.hpp"
#endif
#ifndef GVK_SHDW_PLANARREFLECTIONRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwPlanarReflectionRendererVk.hpp"
#endif
#ifndef GVK_SHDW_CUBEMAPRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwCubeMapRendererVk.hpp"
#endif
#ifndef GVK_SHDW_DEPTHRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwDepthRendererVk.hpp"
#endif
#ifndef GVK_SHDW_NORMALRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwNormalRendererVk.hpp"
#endif
#ifndef GVK_SHDW_MATERIALSRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwMaterialsRendererVk.hpp"
#endif
#ifndef GVK_SHDW_GLOWRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwGlowRendererVk.hpp"
#endif
#ifndef GVK_SHDW_SHADOWSONLYRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwShadowsOnlyRendererVk.hpp"
#endif
#ifndef GVK_SHDW_AOPREVIEWRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwAOPreviewRendererVk.hpp"
#endif
#ifndef GVK_SHDW_GIPREVIEWRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwGIPreviewRendererVk.hpp"
#endif
#ifndef GVK_SHDW_REFLECTIONSONLYRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwReflectionsOnlyRendererVk.hpp"
#endif
#ifndef GVK_SHDW_VELOCITYMAPRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwVelocityMapRendererVk.hpp"
#endif
#ifndef GVK_SHDW_UVRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwUVRendererVk.hpp"
#endif
#ifndef GVK_G3D_TEXTURERENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dTextureRendererVk.hpp"
#endif

#include <gtest/gtest.h>

#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	//	The forward renderer of the backend that is up
	g3dSceneRenderer* forward_renderer()
	{
		if (gtBackend::Current() == gtBackend::e_DX11)
			return new g3dSceneRendererDX11();
		return new gvk::g3dSceneRendererVk();
	}

	gtRenderOptions forward_options()
	{
		gtRenderOptions options;
		options.m_CreateRenderer = forward_renderer;
		return options;
	}

	//	i_Build's scene rendered by the forward renderer of the backend that is up
	gtPicture render(const gtSceneFunction& i_Build, const std::filesystem::path& i_Dir)
	{
		return gtRenderScene(i_Build, i_Dir, forward_options());
	}

	//	Renders on both backends with their forward renderers and compares
	void expect_same_image(const char* i_pTest, const gtSceneFunction& i_Build)
	{
		gtExpectSameImage(i_pTest, i_Build, forward_options());
	}

	//	The usual two lights
	void lights(gtSceneBuilder& io_Builder)
	{
		io_Builder.TwoLights();
	}

	//	Twelve spheres (gtSceneBuilder::LitSpheres)
	void lit_spheres(gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		io_Builder.LitSpheres();
	}
}

TEST(g3dSceneRendererVkTest, LitSpheresMatchDX11)
{
	expect_same_image("LitSpheresMatchDX11", lit_spheres);
}

TEST(g3dSceneRendererVkTest, TexturedMatchDX11)
{
	expect_same_image("TexturedMatchDX11", [](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		matTexture* checker = io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 128, 128, 8));
		matTexture* fine = io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 100, 60, 10));
		matMaterial* blinn = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_NonDefault, { checker }));
		matMaterial* lambert = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_NonDefault, { fine }));
		maMatrix4x4 floor = gtScene::Place(0, 0, -1.5f, 2.5f);
		io_Builder.Add(gtScene::Plane(), lambert, floor);
		io_Builder.Add(gtScene::Sphere(16, 32), blinn, gtScene::Place(-0.9f, 0.3f, 0, 0.8f));
		io_Builder.Add(gtScene::Sphere(16, 32), lambert, gtScene::Place(0.9f, -0.3f, 0.5f, 0.7f));
		lights(io_Builder);
	});
}

TEST(g3dSceneRendererVkTest, TransparencyMatchDX11)
{
	expect_same_image("TransparencyMatchDX11", [](gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		auto transparent = [](float i_R, float i_G, float i_B, float i_Transparency)
		{
			return gtScene::DataMaterial("Phong.fx", [=](effShaderData* i_pData)
			{
				gtScene::SetPhong(i_pData, gtScene::e_NonDefault);
				effPhongData* phong = dynamic_cast<effPhongData*>(i_pData);
				phong->m_ColorDiffuse = maFloatRGBA(i_R, i_G, i_B, 1);
				phong->m_Transparency = i_Transparency;
			});
		};
		matMaterial* back = io_Builder.Keep(gtScene::OpaquePhong("Phong.fx", gtScene::e_NonDefault));
		io_Builder.Add(gtScene::Plane(), back, gtScene::Place(0, 0, -2, 2.5f));
		//	added far to near, near to far and in between: the sort orders them
		io_Builder.Add(gtScene::Sphere(16, 32), io_Builder.Keep(transparent(1, 0.2f, 0.2f, 0.5f)), gtScene::Place(-0.5f, 0.3f, 1.0f, 0.9f));
		io_Builder.Add(gtScene::Sphere(16, 32), io_Builder.Keep(transparent(0.2f, 1, 0.2f, 0.35f)), gtScene::Place(0.5f, 0.3f, -0.5f, 0.9f));
		io_Builder.Add(gtScene::Sphere(16, 32), io_Builder.Keep(transparent(0.2f, 0.2f, 1, 0.65f)), gtScene::Place(0, -0.5f, 0.2f, 0.9f));
		lights(io_Builder);
	});
}

TEST(g3dSceneRendererVkTest, FogMatchDX11)
{
	expect_same_image("FogMatchDX11", [](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		lit_spheres(io_Builder, i_Dir);
		fogParams fog;
		fog.m_nMode = g3dType::e_FogModeLinear;
		fog.m_Color = maFloatRGBA(0.6f, 0.6f, 0.7f, 1);
		fog.m_fStart = 4.0f;
		fog.m_fEnd = 9.0f;
		fog.m_fDensity = 0.3f;
		io_Builder.Scene().SetFog(fog);
		io_Builder.Layer().SetFogEnabled(true);
	});
}

namespace
{
	//	Sets the draw-style preferences while it lives
	class DrawStyleScope
	{
		public:
			DrawStyleScope(g3dSceneNode::DrawStyle i_Style, bool i_bWireframe)
			{
				g3dPrefs::g3dRenderPrefs& prefs = g3dPrefs::CurrentPrefs();
				m_Style = prefs.m_DrawStyle;
				m_bWireframe = prefs.m_bRenderWireframe;
				prefs.m_DrawStyle = i_Style;
				prefs.m_bRenderWireframe = i_bWireframe;
			}
			~DrawStyleScope()
			{
				g3dPrefs::g3dRenderPrefs& prefs = g3dPrefs::CurrentPrefs();
				prefs.m_DrawStyle = m_Style;
				prefs.m_bRenderWireframe = m_bWireframe;
			}
		private:
			g3dSceneNode::DrawStyle	m_Style;
			bool					m_bWireframe;
	};
}

TEST(g3dSceneRendererVkTest, WireframeDrawStyleMatchDX11)
{
	{
		SCOPED_TRACE("wireframe");
		DrawStyleScope style(g3dSceneNode::e_Wireframe, false);
		expect_same_image("WireframeDrawStyleMatchDX11_wireframe", lit_spheres);
	}
	{
		SCOPED_TRACE("lit wireframe (preference)");
		DrawStyleScope style(g3dSceneNode::e_Solid, true);
		expect_same_image("WireframeDrawStyleMatchDX11_lit", lit_spheres);
	}
	{
		SCOPED_TRACE("node draw style");
		DrawStyleScope style(g3dSceneNode::e_Solid, false);
		expect_same_image("WireframeDrawStyleMatchDX11_node", [](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			lit_spheres(io_Builder, i_Dir);
			std::vector<g3dSceneNode*>& nodes = io_Builder.Layer().GetRootNode()->GetChildren();
			for (size_t i = 0; i < nodes.size(); i += 2)
				nodes[i]->SetDrawStyle(i % 4 == 0 ? g3dSceneNode::e_Wireframe : g3dSceneNode::e_LitWireframe);
		});
	}
}

namespace
{
	//	Sets the hardware tessellation preference while it lives
	class TessellationScope
	{
		public:
			TessellationScope(bool i_bOn)
			{
				m_bWas = g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation;
				g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation = i_bOn;
			}
			~TessellationScope()
			{
				g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation = m_bWas;
			}
		private:
			bool m_bWas;
	};

	//	testtess.fx (its PTess pass has a hull and a domain shader) on
	//	displacement materials with three tessellation factors
	void tessellated(gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		const float factors[3] = { 1.0f, 4.0f, 9.5f };
		for (int i = 0; i < 3; i++)
		{
			matMaterial* material = io_Builder.Keep(gtScene::OpaquePhong("testtess.fx", gtScene::e_NonDefault));
			material->SetHasDisplacement(true);
			material->DisplacementData().m_TessellationValue = factors[i];
			io_Builder.Add(gtScene::Sphere(8, 12), material, gtScene::Place(-1.5f + 1.5f * i, 0.6f, 0, 0.6f));
			io_Builder.Add(gtScene::Grid(4), material, gtScene::Place(-1.5f + 1.5f * i, -1.0f, 0, 0.6f));
		}
		lights(io_Builder);
	}

	int lit_pixels(const gtPicture& i_Picture)
	{
		int n = 0;
		for (size_t i = 0; i < i_Picture.m_Rgba.size(); i += 4)
			n += (i_Picture.m_Rgba[i] | i_Picture.m_Rgba[i + 1] | i_Picture.m_Rgba[i + 2]) != 0;
		return n;
	}
}

TEST(g3dSceneRendererVkTest, TessellatedDisplacementMatchDX11)
{
	{
		SCOPED_TRACE("solid");
		TessellationScope tessellation(true);
		expect_same_image("TessellatedDisplacementMatchDX11_solid", tessellated);
	}
	{
		//	(testtess's pixel shader is plain white: the wireframe shows the
		//	triangles the tessellator made)
		SCOPED_TRACE("wireframe");
		TessellationScope tessellation(true);
		DrawStyleScope style(g3dSceneNode::e_Solid, true);
		expect_same_image("TessellatedDisplacementMatchDX11_wireframe", tessellated);
	}
	{
		SCOPED_TRACE("the PTess pass ran");
		DrawStyleScope style(g3dSceneNode::e_Solid, true);
		gtTempDir dir;
		gtPicture with, without;
		{
			TessellationScope tessellation(true);
			if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { with = render(tessellated, dir.GetPath()); }))
				return;
		}
		{
			TessellationScope tessellation(false);
			gtBackend::Run(gtBackend::e_Vulkan, [&]() { without = render(tessellated, dir.GetPath()); });
		}
		EXPECT_GT(lit_pixels(with), lit_pixels(without) * 3 / 2) << "the tessellated wireframe has no more edges";
	}
}

namespace
{
	//	The DBG_LOG lines written while it lives
	class LogCapture
	{
		public:
			LogCapture() : m_Name(L"g3dSceneRendererVkTest")
			{
				dbgMsg::addStream(m_Name, &m_Text);
				dbgMsg::enableStreamLog(m_Name, true);
			}
			~LogCapture()
			{
				dbgMsg::removeStream(m_Name);
			}
			std::string Text() const	{ return m_Text.str(); }
		private:
			std::wstring		m_Name;
			std::ostringstream	m_Text;
	};

	int count_of(const std::string& i_Text, const std::string& i_Part)
	{
		int n = 0;
		for (size_t at = i_Text.find(i_Part); at != std::string::npos; at = i_Text.find(i_Part, at + 1))
			n++;
		return n;
	}
}

TEST(g3dSceneRendererVkTest, UnportedTypesFallBack)
{
	struct Case
	{
		const char* m_pName;
		std::function<g3dSceneRenderer*()> m_Create;
	};
	const std::vector<Case> cases =
	{
		//	Plan 7 (decision 2 of Plan 6: its maps are read only by hair)
		{ "opacity map", []() { return g3dSceneRendererCreate::CreateOpacityMapRenderer(NULL); } },
	};
	//	ported in Plan 6 Tasks 4 to 6
	struct Ported
	{
		g3dSceneRendererTypes::RendererType	m_Type;
		std::function<g3dSceneRenderer*()>	m_Create;
		std::function<bool(g3dSceneRenderer*)>	m_IsPort;
	};
	const std::vector<Ported> ported =
	{
		{ g3dSceneRendererTypes::e_Depth, []() { return g3dSceneRendererCreate::CreateDepthRenderer(); },
		  [](g3dSceneRenderer* r) { return dynamic_cast<gvk::shdwDepthRendererVk*>(r) != NULL; } },
		{ g3dSceneRendererTypes::e_Normals, []() { return g3dSceneRendererCreate::CreateNormalRenderer(); },
		  [](g3dSceneRenderer* r) { return dynamic_cast<gvk::shdwNormalRendererVk*>(r) != NULL; } },
		{ g3dSceneRendererTypes::e_Materials, []() { return g3dSceneRendererCreate::CreateMaterialsRenderer(); },
		  [](g3dSceneRenderer* r) { return dynamic_cast<gvk::shdwMaterialsRendererVk*>(r) != NULL; } },
		{ g3dSceneRendererTypes::e_ReflectionOnly, []() { return g3dSceneRendererCreate::CreateReflectionOnlyRenderer(); },
		  [](g3dSceneRenderer* r) { return dynamic_cast<gvk::shdwReflectionsOnlyRendererVk*>(r) != NULL; } },
		{ g3dSceneRendererTypes::e_Glow, []() { return g3dSceneRendererCreate::CreateGlowRenderer(); },
		  [](g3dSceneRenderer* r) { return dynamic_cast<gvk::shdwGlowRendererVk*>(r) != NULL; } },
		{ g3dSceneRendererTypes::e_ShadowMask, []() { return g3dSceneRendererCreate::CreateShadowMaskRenderer(); },
		  [](g3dSceneRenderer* r) { return dynamic_cast<gvk::shdwShadowsOnlyRendererVk*>(r) != NULL; } },
		{ g3dSceneRendererTypes::e_IlluminationOnly, []() { return g3dSceneRendererCreate::CreateIlluminationRenderer(); },
		  [](g3dSceneRenderer* r) { return dynamic_cast<gvk::shdwShadowsOnlyRendererVk*>(r) != NULL; } },
		{ g3dSceneRendererTypes::e_AmbientOcclusion, []() { return g3dSceneRendererCreate::CreateAmbientOcclusionRenderer(); },
		  [](g3dSceneRenderer* r) { return dynamic_cast<gvk::shdwAOPreviewRendererVk*>(r) != NULL; } },
		{ g3dSceneRendererTypes::e_GlobalIllumination, []() { return g3dSceneRendererCreate::CreateGlobalIlluminationRenderer(); },
		  [](g3dSceneRenderer* r) { return dynamic_cast<gvk::shdwGIPreviewRendererVk*>(r) != NULL; } },
		{ g3dSceneRendererTypes::e_VelocityMap, []() { return g3dSceneRendererCreate::CreateVelocityMapRenderer(); },
		  [](g3dSceneRenderer* r) { return dynamic_cast<gvk::shdwVelocityMapRendererVk*>(r) != NULL; } },
	};
	auto find_ported = [&](int i_Type) -> const Ported*
	{
		for (const Ported& p : ported)
		{
			if (p.m_Type == i_Type)
				return &p;
		}
		return NULL;
	};

	gtBackend::Run(gtBackend::e_Vulkan, [&]()
	{
		std::string log;
		{
			LogCapture capture;
			for (int round = 0; round < 2; round++)
			{
				for (const Case& c : cases)
				{
					SCOPED_TRACE(c.m_pName);
					std::unique_ptr<g3dSceneRenderer> renderer(c.m_Create());
					EXPECT_TRUE(dynamic_cast<gvk::g3dSceneRendererVk*>(renderer.get()) != NULL);
				}
				for (int type = g3dSceneRendererTypes::e_Default; type <= g3dSceneRendererTypes::e_RmanColorBleed; type++)
				{
					SCOPED_TRACE(type);
					std::unique_ptr<g3dSceneRenderer> renderer(g3dSceneRendererCreate::CreateRenderer((g3dSceneRendererTypes::RendererType)type));
					//	default and HDR (and the types CreateRenderer answers with the
					//	default) are the HDR renderer since Plan 5a Task 2
					const bool hdr = type == g3dSceneRendererTypes::e_Default || type == g3dSceneRendererTypes::e_HDR ||
						type == g3dSceneRendererTypes::e_DirtyMatte || type == g3dSceneRendererTypes::e_Wireframe ||
						type == g3dSceneRendererTypes::e_RmanColorBleed;
					if (hdr)
						EXPECT_TRUE(dynamic_cast<gvk::shdwHDRRendererVk*>(renderer.get()) != NULL);
					else if (type == g3dSceneRendererTypes::e_DepthMap)		// since Task 3
						EXPECT_TRUE(dynamic_cast<gvk::g3dDepthMapRendererVk*>(renderer.get()) != NULL);
					else if (type == g3dSceneRendererTypes::e_CubeMap)		// since Task 6
						EXPECT_TRUE(dynamic_cast<gvk::shdwCubeMapRendererVk*>(renderer.get()) != NULL);
					else if (type == g3dSceneRendererTypes::e_PlanarReflection)
						EXPECT_TRUE(dynamic_cast<gvk::shdwPlanarReflectionRendererVk*>(renderer.get()) != NULL);
					else if (const Ported* p = find_ported(type))	// since Plan 6 Task 4
						EXPECT_TRUE(p->m_IsPort(renderer.get()));
					else
						EXPECT_TRUE(dynamic_cast<gvk::g3dSceneRendererVk*>(renderer.get()) != NULL);
				}
				for (auto create : { g3dSceneRendererCreate::CreateDefaultRenderer, g3dSceneRendererCreate::CreateHDRRenderer })
				{
					std::unique_ptr<g3dSceneRenderer> renderer(create());
					EXPECT_TRUE(dynamic_cast<gvk::shdwHDRRendererVk*>(renderer.get()) != NULL);
				}
				{
					std::unique_ptr<g3dSceneRenderer> renderer(g3dSceneRendererCreate::CreateDepthMapRenderer());
					EXPECT_TRUE(dynamic_cast<gvk::g3dDepthMapRendererVk*>(renderer.get()) != NULL);
				}
				{
					std::unique_ptr<g3dSceneRenderer> renderer(g3dSceneRendererCreate::CreateSimpleRenderer());
					EXPECT_TRUE(dynamic_cast<gvk::shdwShadowLayerRendererVk*>(renderer.get()) != NULL);
				}
				{	// since Plan 5b Task 3
					std::unique_ptr<g3dSceneRenderer> renderer(g3dSceneRendererCreate::CreateRSMRenderer());
					EXPECT_TRUE(dynamic_cast<gvk::g3dRSMRendererVk*>(renderer.get()) != NULL);
				}
				{	// since Plan 6 Task 6 (neither has a renderer type)
					std::unique_ptr<g3dSceneRenderer> texture(g3dSceneRendererCreate::CreateTextureRenderer());
					EXPECT_TRUE(dynamic_cast<gvk::g3dTextureRendererVk*>(texture.get()) != NULL);
					std::unique_ptr<g3dSceneRenderer> uv(g3dSceneRendererCreate::CreateUVRenderer());
					EXPECT_TRUE(dynamic_cast<gvk::shdwUVRendererVk*>(uv.get()) != NULL);
				}
				for (const Ported& p : ported)
				{
					SCOPED_TRACE(p.m_Type);
					std::unique_ptr<g3dSceneRenderer> renderer(p.m_Create());
					EXPECT_TRUE(p.m_IsPort(renderer.get()));
				}
			}
			log = capture.Text();
		}
		//	once each, at most (a type logged by an earlier test isn't logged
		//	again); DBG_LOG writes only in Debug builds
		for (const Case& c : cases)
		{
			EXPECT_LE(count_of(log, std::string("GVK_NOT_YET: ") + c.m_pName + " renderer"), 1) << c.m_pName << "\n" << log;
		}
	});
}

