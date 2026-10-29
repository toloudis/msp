/*****************************************************************************
**  g2dResourceCounterVkTest.cpp
**
**      g2dResourceCounterVk counts what g2dResourceCounterDX11 counts: the
**      KB and number of textures, render targets, shadow maps and window
**      buffers made through the g2d texture, render-target and depth
**      classes, and the vertex and index buffers. A scene with a shadowed
**      projected light, a planar reflector, the global environment and the
**      HDR renderer's frame buffers, drawn into a 256x256 window, gives
**      the same counts and sizes on both backends, and releasing it all
**      gives back the counts from before. Skipped without a Vulkan device.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"
#include "gtSceneWindow.hpp"
#include "gtTempDir.hpp"

#ifndef G2D_FONTUTIL_HPP
#include "Graphics/G2d/g2dFontUtil.hpp"
#endif
#include <string>
#ifndef G2D_RESOURCECOUNTER_HPP
#include "Graphics/G2d/g2dResourceCounter.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/G2d/g2dWindow.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif

#include <gtest/gtest.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

namespace
{
	//	The eleven getters, in g2dResourceCounter's order
	struct counts
	{
		float	m_Video = 0;
		float	m_TextureKB = 0;
		int		m_Textures = 0;
		float	m_FramebufferKB = 0;
		int		m_Framebuffers = 0;
		float	m_ShadowKB = 0;
		int		m_Shadows = 0;
		float	m_VertexKB = 0;
		int		m_VertexBuffers = 0;
		float	m_IndexKB = 0;
		int		m_IndexBuffers = 0;

		static counts Now()
		{
			counts c;
			c.m_Video = g2dResourceCounter::GetTotalVideoMemory();
			c.m_TextureKB = g2dResourceCounter::GetTotalSceneTextureMemory();
			c.m_Textures = g2dResourceCounter::GetNumSceneTextures();
			c.m_FramebufferKB = g2dResourceCounter::GetTotalFramebufferMemory();
			c.m_Framebuffers = g2dResourceCounter::GetNumFramebufferTextures();
			c.m_ShadowKB = g2dResourceCounter::GetTotalShadowMapMemory();
			c.m_Shadows = g2dResourceCounter::GetNumShadowMapTextures();
			c.m_VertexKB = g2dResourceCounter::GetTotalVertexBufferMemory();
			c.m_VertexBuffers = g2dResourceCounter::GetNumVertexBuffers();
			c.m_IndexKB = g2dResourceCounter::GetTotalIndexBufferMemory();
			c.m_IndexBuffers = g2dResourceCounter::GetNumIndexBuffers();
			return c;
		}

		counts operator - (const counts& i_rOther) const
		{
			counts c;
			c.m_Video = m_Video - i_rOther.m_Video;
			c.m_TextureKB = m_TextureKB - i_rOther.m_TextureKB;
			c.m_Textures = m_Textures - i_rOther.m_Textures;
			c.m_FramebufferKB = m_FramebufferKB - i_rOther.m_FramebufferKB;
			c.m_Framebuffers = m_Framebuffers - i_rOther.m_Framebuffers;
			c.m_ShadowKB = m_ShadowKB - i_rOther.m_ShadowKB;
			c.m_Shadows = m_Shadows - i_rOther.m_Shadows;
			c.m_VertexKB = m_VertexKB - i_rOther.m_VertexKB;
			c.m_VertexBuffers = m_VertexBuffers - i_rOther.m_VertexBuffers;
			c.m_IndexKB = m_IndexKB - i_rOther.m_IndexKB;
			c.m_IndexBuffers = m_IndexBuffers - i_rOther.m_IndexBuffers;
			return c;
		}
	};

	std::ostream& operator << (std::ostream& o, const counts& c)
	{
		return o << "video " << c.m_Video << " KB, textures " << c.m_Textures << " / " << c.m_TextureKB
				 << " KB, framebuffers " << c.m_Framebuffers << " / " << c.m_FramebufferKB
				 << " KB, shadows " << c.m_Shadows << " / " << c.m_ShadowKB
				 << " KB, vertex buffers " << c.m_VertexBuffers << " / " << c.m_VertexKB
				 << " KB, index buffers " << c.m_IndexBuffers << " / " << c.m_IndexKB << " KB";
	}

	//	KB within 0.5%
	void expect_kb(const char* i_pName, float i_DX11, float i_Vulkan)
	{
		EXPECT_LE(std::fabs(i_DX11 - i_Vulkan), 0.005f * std::fabs(i_DX11) + 1e-3f)
			<< i_pName << ": DX11 " << i_DX11 << " KB, Vulkan " << i_Vulkan << " KB";
	}

	//	What the counters say while the scene is up, and after it has all
	//	gone, both relative to before it was made (other tests in the run
	//	may leave resources counted). The window's debug font is a texture
	//	if an earlier test has set the font bitmap, and none otherwise: it
	//	is held here for the whole run, so the window only adds a reference
	//	to it and the counts don't depend on the tests run before.
	struct run
	{
		counts	m_Up;
		counts	m_After;
	};

	run count_scene(gtBackend::Kind i_Kind, const std::filesystem::path& i_Dir)
	{
		run result;
		const g2dFontHandle font = g2dFontUtil::LoadFont(std::string(), 14);	// g2dDebugDisplay's
		const counts before = counts::Now();
		{
			gtPrefsScope prefs;
			gtSceneWindow window(i_Kind, 256, 256);
			{
				gtSceneBuilder builder;
				builder.LightsCastShadows(true);
				builder.LitSpheres(true, false);
				builder.ProjectedLight(maPoint3d(0, 4, 4), maPoint3d(0, 0, 0), maFloatRGBA(0.8f, 0.8f, 0.7f, 1), 512);

				//	a planar reflector under the spheres
				maMatrix4x4 floor;
				floor.MakeRotate(maVector3d(0, 0, 1), maVector3d(0, 1, 0));
				floor.ScaleBy(4, 4, 4);
				floor.TranslateBy(0, -1.5f, 0);
				g3dSceneNode* node = builder.Add(gtScene::Plane(),
					builder.Keep(gtScene::ShaderMaterial("BlinnReflection.fx", gtScene::e_Defaults)), floor);
				builder.Reflective(node, true, false);

				matTexture* diffuse = builder.Keep(gtScene::HdrTexture(i_Dir, 128, 64, 2.0f));
				matTexture* specular = builder.Keep(gtScene::HdrTexture(i_Dir, 256, 128, 2.0f));
				builder.GlobalEnvironment(diffuse, specular, false);

				std::unique_ptr<g3dSceneRenderer> renderer(g3dSceneRendererCreate::CreateDefaultRenderer());
				g2dWindow& w = window.Window();
				builder.RenderTargets();
				w.BeginScene();
				w.MakeCurrent();
				w.Clear(g2dRGBColor(0, 0, 0));
				renderer->Render(&w, builder.Camera(), builder.Scene(), builder.ViewerLayers(), 1.25f);
				w.EndScene();
				window.Present();

				result.m_Up = counts::Now() - before;
				renderer->ReleaseResources();
			}
		}
		result.m_After = counts::Now() - before;
		g2dFontUtil::ReleaseFont(font);
		return result;
	}
}

TEST(g2dResourceCounterVkTest, CountsMatchDX11)
{
	gtTempDir dir;
	run dx11, vulkan;
	if (!gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = count_scene(gtBackend::e_DX11, dir.GetPath()); }))
		return;
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = count_scene(gtBackend::e_Vulkan, dir.GetPath()); }))
		return;
	std::cout << "  DX11:   " << dx11.m_Up << std::endl;
	std::cout << "  Vulkan: " << vulkan.m_Up << std::endl;

	//	the scene made something of each kind on DX11
	EXPECT_GT(dx11.m_Up.m_Textures, 0);
	EXPECT_GT(dx11.m_Up.m_Framebuffers, 0);
	EXPECT_GT(dx11.m_Up.m_Shadows, 0);
	EXPECT_GT(dx11.m_Up.m_VertexBuffers, 0);		// (the test meshes have no index buffers)

	EXPECT_EQ(dx11.m_Up.m_Textures, vulkan.m_Up.m_Textures);
	EXPECT_EQ(dx11.m_Up.m_Framebuffers, vulkan.m_Up.m_Framebuffers);
	EXPECT_EQ(dx11.m_Up.m_Shadows, vulkan.m_Up.m_Shadows);
	EXPECT_EQ(dx11.m_Up.m_VertexBuffers, vulkan.m_Up.m_VertexBuffers);
	EXPECT_EQ(dx11.m_Up.m_IndexBuffers, vulkan.m_Up.m_IndexBuffers);
	expect_kb("video", dx11.m_Up.m_Video, vulkan.m_Up.m_Video);
	expect_kb("textures", dx11.m_Up.m_TextureKB, vulkan.m_Up.m_TextureKB);
	expect_kb("framebuffers", dx11.m_Up.m_FramebufferKB, vulkan.m_Up.m_FramebufferKB);
	expect_kb("shadows", dx11.m_Up.m_ShadowKB, vulkan.m_Up.m_ShadowKB);
	expect_kb("vertex buffers", dx11.m_Up.m_VertexKB, vulkan.m_Up.m_VertexKB);
	expect_kb("index buffers", dx11.m_Up.m_IndexKB, vulkan.m_Up.m_IndexKB);

	//	everything released: back where it started, on both
	for (const run* r : { &dx11, &vulkan })
	{
		SCOPED_TRACE(r == &dx11 ? "DX11" : "Vulkan");
		EXPECT_EQ(0, r->m_After.m_Textures);
		EXPECT_EQ(0, r->m_After.m_Framebuffers);
		EXPECT_EQ(0, r->m_After.m_Shadows);
		EXPECT_EQ(0, r->m_After.m_VertexBuffers);
		EXPECT_EQ(0, r->m_After.m_IndexBuffers);
		EXPECT_NEAR(0, r->m_After.m_Video, 0.01f);
		EXPECT_NEAR(0, r->m_After.m_TextureKB, 0.01f);
		EXPECT_NEAR(0, r->m_After.m_FramebufferKB, 0.01f);
		EXPECT_NEAR(0, r->m_After.m_ShadowKB, 0.01f);
		EXPECT_NEAR(0, r->m_After.m_VertexKB, 0.01f);
		EXPECT_NEAR(0, r->m_After.m_IndexKB, 0.01f);
	}
}
