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
#include "gtTempDir.hpp"

#ifndef GF_PATHS_HPP
#include "Core/Gf/gfPaths.hpp"
#endif

#include <filesystem>

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

#ifndef GVK_EFF_SHADERARRAY_HPP
#include "GraphicsVulkan/Eff/effShaderArray.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif

#ifndef EFF_BILLBOARDDATA_HPP
#include "Graphics/Eff/effBillboardData.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef EFF_TEXTUREDDATA_HPP
#include "Graphics/Eff/effTexturedData.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
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
	EXPECT_EQ(77, copy[(4 - 1 - 2) * 4 + 3].g);		// bottom row first, as DX11's
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

	//	as with DX11's bump fragments: BumpTex1Vertex vertices (callers lock and
	//	read them by format, the camera icon asserts it) and 16-bit indices
	//	unless g3dIndexPtr::Needs32Bit (the RenderMan and FBX exporters)
	EXPECT_EQ(g3dType::e_BumpTex1Vertex, fragment->GetVertexFormat());
	const g3dType::BumpTex1Vertex* locked = reinterpret_cast<const g3dType::BumpTex1Vertex*>(fragment->ReadOnlyLock());
	ASSERT_NE(nullptr, locked);
	EXPECT_FLOAT_EQ(3, locked[1].m_Vertex.GetX());
	EXPECT_FLOAT_EQ(1, locked[2].m_Normal.GetZ());
	fragment->ReadOnlyUnlock();
	ASSERT_FALSE(g3dIndexPtr::Needs32Bit(3));
	const uint16_t* locked_indices = reinterpret_cast<const uint16_t*>(fragment->ReadOnlyLockIndices());
	ASSERT_NE(nullptr, locked_indices);
	EXPECT_EQ(1, locked_indices[1]);
	EXPECT_EQ(2, locked_indices[2]);
	fragment->ReadOnlyUnlockIndices();
}

//	Materials take their data type from the registered built-in shaders
//	(matMaterial("Billboard.fx") gets effBillboardData), so the Vulkan
//	shader manager registers DX11's names and data types, each with an
//	effect (matShaderMgr asserts on a NULL one); effShaderVkTest compares
//	the whole registry with DX11's
TEST_F(gtLayer, ShaderMgrRegistersTheBuiltInShaders)
{
	gvk::effShaderArray mgr;
	std::map<std::string, matShaderInfo> shaders;
	mgr.RegisterEffects(fsLocator(), shaders);

	EXPECT_EQ(42u, shaders.size());
	EXPECT_NE(nullptr, dynamic_cast<effBillboardData*>(shaders["Billboard.fx"].m_DataTemplate));
	EXPECT_NE(nullptr, dynamic_cast<effPhongData*>(shaders["default"].m_DataTemplate));
	EXPECT_NE(nullptr, dynamic_cast<effTexturedData*>(shaders["EnvBackground.fx"].m_DataTemplate));
	EXPECT_EQ(nullptr, shaders["HDRLighting"].m_DataTemplate);
	for (auto& shader : shaders)
	{
		EXPECT_NE(nullptr, shader.second.m_pEffect) << shader.first;
		EXPECT_EQ(shader.first, shader.second.m_Name) << shader.first;
		delete shader.second.m_DataTemplate;
		delete shader.second.m_pEffect;
	}
}

//	As with DX11, the 3D system initializes the shader manager once its
//	implementation is installed, so materials get their data types
//	(scrImage's "Billboard.fx" material needs effBillboardData)
TEST_F(gtLayer, ShaderMgrIsInitialized)
{
	const bool was_enabled = matShaderMgr::GetUseShaderArray();
	matShaderMgr::SetUseShaderArray(true);
	std::unique_ptr<effShaderData> data(matShaderMgr::CreateData("Billboard.fx"));
	EXPECT_NE(nullptr, dynamic_cast<effBillboardData*>(data.get()));
	EXPECT_NE(nullptr, matShaderMgr::GetSpecialEffect("Blur"));
	matShaderMgr::SetUseShaderArray(was_enabled);
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

//	Validation runs in Debug builds and with -gfxValidation; the layer says
//	whether it ran, so a Release run without it isn't reported as clean
TEST(gtLayerLifetime, ValidationRunsInDebugOrWhenAskedFor)
{
	for (bool asked : {false, true})
	{
		void* window = gtPlatform::CreateHiddenWindow(64, 64);
		g3dGfxOptions options;
		options.m_bValidation = asked;
		if (!GraphicsVulkanLayer::Init(window, options))
		{
			gtPlatform::DestroyHiddenWindow(window);
			GTEST_SKIP() << "No usable Vulkan device: " << GraphicsVulkanLayer::GetInitError();
		}
		EXPECT_EQ(asked || ENV_BUILD == ENV_DEBUGBUILD, GraphicsVulkanLayer::IsValidating()) << "asked: " << asked;
		GraphicsVulkanLayer::CleanUp();
		gtPlatform::DestroyHiddenWindow(window);
	}
}

//	Pipelines persist in the user data folder (gfPaths::e_UserDataPath)
TEST(gtLayerLifetime, PipelineCacheKeptInUserData)
{
	gtTempDir dir;
	const fsLocator previous = gfPaths::GetPath(gfPaths::e_UserDataPath);
	gfPaths::ResetPath(gfPaths::e_UserDataPath, dir.GetLocator());
	void* window = gtPlatform::CreateHiddenWindow(64, 64);
	g3dGfxOptions options;
	options.m_bValidation = true;
	const bool ok = GraphicsVulkanLayer::Init(window, options);
	if (ok)
	{
		GraphicsVulkanLayer::CleanUp();
	}
	gtPlatform::DestroyHiddenWindow(window);
	gfPaths::ResetPath(gfPaths::e_UserDataPath, previous);
	if (!ok)
	{
		GTEST_SKIP() << "No usable Vulkan device: " << GraphicsVulkanLayer::GetInitError();
	}
	EXPECT_TRUE(std::filesystem::is_regular_file(dir.GetPath() / "VulkanPipelineCache.bin"));
}
