/*****************************************************************************
**  GraphicsVulkanLayerTest.cpp
**
**      GraphicsVulkanLayer installs a Vulkan implementation for every
**      graphics hook GraphicsDX11 installs (stubs where not ported yet),
**      every scene renderer type can be created, the stubs answer from
**      the data they were given, and clean-up removes everything with no
**      validation messages. Skipped without a Vulkan device.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtPlatform.hpp"

#ifndef GVK_GRAPHICSVULKANLAYER_HPP
#include "GraphicsVulkan/GraphicsVulkanLayer.hpp"
#endif
#ifndef GVK_VKDVALIDATION_HPP
#include "GraphicsVulkan/vkd/vkdValidation.hpp"
#endif
#ifndef G3D_GFXOPTIONS_HPP
#include "Graphics/G3d/g3dGfxOptions.hpp"
#endif
#ifndef G2D_RESOURCECOUNTER_HPP
#include "Graphics/G2d/g2dResourceCounter.hpp"
#endif
#ifndef G2D_FONTUTIL_HPP
#include "Graphics/G2d/g2dFontUtil.hpp"
#endif
#ifndef G2D_IMAGECREATE_HPP
#include "Graphics/G2d/g2dImageCreate.hpp"
#endif
#ifndef G2D_IMAGESAVE_HPP
#include "Graphics/G2d/g2dImageSave.hpp"
#endif
#ifndef G2D_SCREENCAPTUREUTIL_HPP
#include "Graphics/G2d/g2dScreenCaptureUtil.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef G3D_LIGHTMGR_HPP
#include "Graphics/G3d/g3dLightMgr.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif
#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#endif
#ifndef G3D_BAKE_HPP
#include "Graphics/G3d/g3dBake.hpp"
#endif
#ifndef G3D_FRAGMENTCREATE_HPP
#include "Graphics/G3d/g3dFragmentCreate.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/Mat/matShaderMgr.hpp"
#endif
#ifndef MDL_FRAGCREATE_HPP
#include "Graphics/mdl/mdlFragCreate.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif
#ifndef G3D_SCENERENDERERTYPES_HPP
#include "Graphics/G3d/g3dSceneRendererTypes.hpp"
#endif

#include <gtest/gtest.h>

#include <memory>

namespace
{
	//	The hook's current implementation, read by swapping in NULL and back.
	template <typename Impl>
	Impl* installed(Impl* (*i_pSet)(Impl*))
	{
		Impl* current = i_pSet(NULL);
		i_pSet(current);
		return current;
	}

	class gtLayer : public ::testing::Test
	{
		protected:
			void SetUp() override
			{
				m_pWindow = gtPlatform::CreateHiddenWindow(64, 64);
				g3dGfxOptions options;
				options.m_Backend = g3dGfxOptions::e_Vulkan;
				options.m_bValidation = true;
				if (!GraphicsVulkanLayer::Init(m_pWindow, options))
				{
					gtPlatform::DestroyHiddenWindow(m_pWindow);
					m_pWindow = NULL;
					GTEST_SKIP() << "No usable Vulkan device: " << GraphicsVulkanLayer::GetInitError();
				}
				GraphicsVulkanLayer::InitGraphics();
			}

			void TearDown() override
			{
				if (m_pWindow)
				{
					GraphicsVulkanLayer::CleanUpGraphics();
					GraphicsVulkanLayer::CleanUp();
					gtPlatform::DestroyHiddenWindow(m_pWindow);
					gvk::vkdValidationStats stats = GraphicsVulkanLayer::GetValidationStats();
					EXPECT_EQ(0, stats.m_Errors);
					EXPECT_EQ(0, stats.m_Warnings);
				}
			}

			void* m_pWindow = NULL;
	};
}

TEST_F(gtLayer, AllHooksInstalled)
{
	EXPECT_NE(nullptr, GraphicsVulkanLayer::GetSystem2D());
	EXPECT_NE(nullptr, GraphicsVulkanLayer::GetSystem3D());
	//	hooks whose setter returns the previous implementation
	EXPECT_NE(nullptr, (installed(&g2dResourceCounter::SetImplementation)));
	EXPECT_NE(nullptr, (installed(&g2dScreenCaptureUtil::SetImplementation)));
	EXPECT_NE(nullptr, (installed(&g3dSceneRendererCreate::SetImplementation)));
	EXPECT_NE(nullptr, (installed(&g3dBake::SetImplementation)));
	EXPECT_NE(nullptr, (installed(&g3dFragmentCreate::SetImplementation)));
	EXPECT_NE(nullptr, (installed(&mdlFragCreate::SetImplementation)));
	//	hooks with an accessor
	EXPECT_NE(nullptr, g2dFontUtil::Implementation());
	EXPECT_NE(nullptr, g3dLightMgr::Implementation());
	//	hooks that can only be called (they DBG_ASSERT, failing the test, when missing)
	g2dPFD rgba(0, 8, 8, 8, 16, 8, 24, 8, 32);
	std::unique_ptr<g2dImage> image(g2dImageCreate::Make(1, 1, rgba, g2dImage::e_SystemMemory));
	EXPECT_NE(nullptr, image.get());
	matTexture* texture = matTextureMgr::CreateReflectiveShadowMap(4, 4);
	EXPECT_NE(nullptr, texture);
	matTextureMgr::ReleaseTexture(texture);
	//	g2dImageSave and matShaderMgr have no query that is safe to call here;
	//	GraphicsVulkanLayer installs them next to the others (code review).
}

TEST_F(gtLayer, EveryRendererTypeCanBeCreated)
{
	for (int type = g3dSceneRendererTypes::e_Default; type <= g3dSceneRendererTypes::e_RmanColorBleed; ++type)
	{
		std::unique_ptr<g3dSceneRenderer> renderer(
			g3dSceneRendererCreate::CreateRenderer(static_cast<g3dSceneRendererTypes::RendererType>(type)));
		EXPECT_NE(nullptr, renderer.get()) << "renderer type " << type;
	}
	std::unique_ptr<g3dPickRenderer> pick(g3dSceneRendererCreate::CreatePickRenderer());
	EXPECT_NE(nullptr, pick.get());
}

TEST_F(gtLayer, StubsAnswerFromTheirData)
{
	//	an image keeps its pixels
	g2dPFD rgba(0, 8, 8, 8, 16, 8, 24, 8, 32);
	std::unique_ptr<g2dImage> image(g2dImageCreate::Make(4, 4, rgba, g2dImage::e_SystemMemory));
	ASSERT_NE(nullptr, image.get());
	EXPECT_EQ(4, image->GetWidth());
	EXPECT_EQ(4, image->GetHeight());
	uint8_t* pixels = static_cast<uint8_t*>(image->Lock());
	ASSERT_NE(nullptr, pixels);
	pixels[image->GetStride() * 2 + 4 * 3 + 1] = 77;		// (3, 2), green
	image->Release();
	g2dPixelR8G8B8A8* copy = NULL;
	int width = 0, height = 0;
	image->GetPixels(&copy, &width, &height);
	ASSERT_NE(nullptr, copy);
	EXPECT_EQ(4, width);
	EXPECT_EQ(77, copy[2 * 4 + 3].g);
	delete[] copy;

	//	a fragment's bounding box comes from its vertices
	const maPoint3d vertices[3] = {maPoint3d(-1, 0, 2), maPoint3d(3, 5, 0), maPoint3d(0, -2, 1)};
	const maPoint3d normals[3] = {maPoint3d(0, 0, 1), maPoint3d(0, 0, 1), maPoint3d(0, 0, 1)};
	const uint16_t indices[3] = {0, 1, 2};
	std::unique_ptr<g3dFragment> fragment(g3dFragmentCreate::CreateFragment(
		vertices, normals, 3, g3dIndexPtr(indices), 3, NULL, false));
	ASSERT_NE(nullptr, fragment.get());
	EXPECT_EQ(3, fragment->GetNumVertices());
	EXPECT_EQ(3, fragment->GetNumIndices());
	const maAxisBox& box = fragment->GetBoundingBox();
	EXPECT_FLOAT_EQ(-1, box.GetMinX()); EXPECT_FLOAT_EQ(-2, box.GetMinY()); EXPECT_FLOAT_EQ(0, box.GetMinZ());
	EXPECT_FLOAT_EQ(3, box.GetMaxX()); EXPECT_FLOAT_EQ(5, box.GetMaxY()); EXPECT_FLOAT_EQ(2, box.GetMaxZ());
}

TEST(gtLayerLifetime, CleanUpRemovesTheHooks)
{
	void* window = gtPlatform::CreateHiddenWindow(64, 64);
	g3dGfxOptions options;
	options.m_bValidation = true;
	if (!GraphicsVulkanLayer::Init(window, options))
	{
		gtPlatform::DestroyHiddenWindow(window);
		GTEST_SKIP() << "No usable Vulkan device: " << GraphicsVulkanLayer::GetInitError();
	}
	GraphicsVulkanLayer::InitGraphics();
	GraphicsVulkanLayer::CleanUpGraphics();
	GraphicsVulkanLayer::CleanUp();
	gtPlatform::DestroyHiddenWindow(window);

	EXPECT_EQ(nullptr, (installed(&g2dResourceCounter::SetImplementation)));
	EXPECT_EQ(nullptr, (installed(&g2dScreenCaptureUtil::SetImplementation)));
	EXPECT_EQ(nullptr, (installed(&g3dSceneRendererCreate::SetImplementation)));
	EXPECT_EQ(nullptr, (installed(&g3dBake::SetImplementation)));
	EXPECT_EQ(nullptr, (installed(&g3dFragmentCreate::SetImplementation)));
	EXPECT_EQ(nullptr, (installed(&mdlFragCreate::SetImplementation)));
	EXPECT_EQ(nullptr, g2dFontUtil::Implementation());
	EXPECT_EQ(nullptr, g3dLightMgr::Implementation());
	EXPECT_EQ(nullptr, GraphicsVulkanLayer::GetSystem2D());
}
