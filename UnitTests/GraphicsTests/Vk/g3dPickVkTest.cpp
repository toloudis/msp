/*****************************************************************************
**  g3dPickVkTest.cpp
**
**      The pick renderer (g3dPickBufferRendererVk) on Vulkan against DX11,
**      made by the creator (g3dSceneRendererCreate::CreatePickRenderer) and
**      driven as pick3dPickBuffer::DoPickRender drives it (GraphicsTests
**      doesn't link Tool): a 1x1 RGBA32F render-target texture, a copy of
**      the viewer's camera narrowed to the picked pixel's sub-viewport,
**      SetPickViewport / RestoreViewport around a g3dTargetRenderer's
**      Render, and GetPickInfo. Every pick's object ID must be equal on
**      both backends, and its depth and UV within 1e-5. On Vulkan alone,
**      a PBR MASK material picks as the same plane with its holes cut out
**      of the mesh (plan 2b).
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"
#include "gtTempDir.hpp"

#ifndef CAM_CAMERA_HPP
#include "Graphics/Cam/camCamera.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef EFF_TEXTUREDDATA_HPP
#include "Graphics/Eff/effTexturedData.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif
#ifndef EFF_PBRDATA_HPP
#include "Graphics/Eff/effPbrData.hpp"
#endif
#ifndef G3D_PICKINFO_HPP
#include "Graphics/G3d/g3dPickInfo.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif
#ifndef G3D_TARGETRENDERER_HPP
#include "Graphics/G3d/g3dTargetRenderer.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif

#include <gtest/gtest.h>

#include <cmath>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace
{
	struct gtPickAt
	{
		int			m_X, m_Y;
		uint32_t	m_Mask;
	};

	//	What pick3dPickBuffer owns: the pick renderer, its 1x1 RGBA32F
	//	target, the pick camera and the target renderer drawing into it
	class gtPicker
	{
		public:
			gtPicker(gtSceneBuilder& io_Builder, int i_Width, int i_Height)
			: m_Builder(io_Builder), m_Width(i_Width), m_Height(i_Height)
			{
				const float clear[4] = { 0, 0, 0, 0 };
				m_pViewer.reset(new gtTarget(gtBackend::Current(), i_Width, i_Height, clear));
				m_pRenderer.reset(g3dSceneRendererCreate::CreatePickRenderer());
				g2dPFD pfd(g2dPFD::e_RGBA32f, 32 * 4);
				m_pTexture = matTextureMgr::CreateRenderTargetTexture(1, 1, false, &pfd, false, true, matTextureMgr::e_Framebuffer);
				EXPECT_TRUE(m_pTexture != NULL);
				m_pTarget = m_pTexture ? m_pTexture->GetRenderTargetAPI() : NULL;
				m_pTargetRenderer.reset(new g3dTargetRenderer(m_pTarget, m_pRenderer.get(), &io_Builder.Scene(), &m_PickCamera));
				m_pTargetRenderer->SetBackgroundColor(g2dRGBColor(0, 0, 0));
			}

			~gtPicker()
			{
				m_pTargetRenderer.reset();
				if (m_pTexture)
					matTextureMgr::ReleaseTexture(m_pTexture);
				m_pRenderer->ReleaseResources();
				m_pRenderer.reset();
				m_pViewer.reset();
			}

			//	pick3dPickBuffer::DoPickRender at (i_X, i_Y) of the viewer
			g3dPickInfo Pick(int i_X, int i_Y, uint32_t i_Mask)
			{
				g3dPickInfo info;
				info.Clear();
				if (!m_pTarget || i_X < 0 || i_Y < 0 || i_X >= m_Width || i_Y >= m_Height)
					return info;
				const float left = -1.0f + 2.0f * (i_X / float(m_Width));
				const float right = -1.0f + 2.0f * (i_X + 1) / float(m_Width);
				const float top = -1.0f + 2.0f * (i_Y / float(m_Height));
				const float bottom = -1.0f + 2.0f * (i_Y + 1) / float(m_Height);
				m_PickCamera = m_Builder.Camera();
				m_PickCamera.SetSubViewport(top, bottom, left, right);
				m_PickCamera.SetMatchAspectToWindow(false);
				m_pRenderer->SetPickViewport(m_pViewer->Target());
				m_pRenderer->SetPickMask(i_Mask);
				m_pTargetRenderer->Render(1.25f, m_Builder.ViewerLayers());
				m_pRenderer->RestoreViewport();
				m_pRenderer->GetPickInfo(m_pTarget, info);
				return info;
			}

		private:
			gtSceneBuilder&						m_Builder;
			int									m_Width, m_Height;
			std::unique_ptr<gtTarget>			m_pViewer;		// the viewer's window (SetPickViewport's)
			std::unique_ptr<g3dPickRenderer>	m_pRenderer;
			matTexture*							m_pTexture = NULL;
			g2dRenderTarget*					m_pTarget = NULL;
			camCamera							m_PickCamera;
			std::unique_ptr<g3dTargetRenderer>	m_pTargetRenderer;
	};

	typedef std::function<void(gtSceneBuilder&)> gtPickScene;

	//	The picks on the backend that is up
	std::vector<g3dPickInfo> pick_all(const gtPickScene& i_Build, int i_Width, int i_Height, const std::vector<gtPickAt>& i_Picks)
	{
		gtPrefsScope prefs;
		gtSceneBuilder builder;
		builder.Camera().SetAspect((float)i_Width / (float)i_Height);
		i_Build(builder);
		gtPicker picker(builder, i_Width, i_Height);
		std::vector<g3dPickInfo> infos;
		for (const gtPickAt& at : i_Picks)
			infos.push_back(picker.Pick(at.m_X, at.m_Y, at.m_Mask));
		return infos;
	}

	//	Picks on both backends and compares; returns DX11's picks (empty
	//	if Vulkan isn't available)
	std::vector<g3dPickInfo> expect_same_picks(const char* i_pName, const gtPickScene& i_Build, int i_Width, int i_Height,
											   const std::vector<gtPickAt>& i_Picks)
	{
		SCOPED_TRACE(i_pName);
		std::vector<g3dPickInfo> dx11, vulkan;
		gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = pick_all(i_Build, i_Width, i_Height, i_Picks); });
		if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = pick_all(i_Build, i_Width, i_Height, i_Picks); }))
			return std::vector<g3dPickInfo>();
		EXPECT_EQ(i_Picks.size(), dx11.size());
		EXPECT_EQ(i_Picks.size(), vulkan.size());
		if (dx11.size() != i_Picks.size() || vulkan.size() != i_Picks.size())
			return dx11;
		int ids_apart = 0;
		float max_depth = 0, max_uv = 0;
		for (size_t i = 0; i < i_Picks.size(); i++)
		{
			const gtPickAt& at = i_Picks[i];
			const g3dPickInfo& d = dx11[i];
			const g3dPickInfo& v = vulkan[i];
			const float depth = std::fabs(d.m_Depth - v.m_Depth);
			const float uv = std::max(std::fabs(d.m_U - v.m_U), std::fabs(d.m_V - v.m_V));
			max_depth = std::max(max_depth, depth);
			max_uv = std::max(max_uv, uv);
			if (d.m_ObjectID != v.m_ObjectID || !(depth <= 1e-5f) || !(uv <= 1e-5f))
			{
				ids_apart += d.m_ObjectID != v.m_ObjectID;
				ADD_FAILURE() << "pick (" << at.m_X << ", " << at.m_Y << ") mask " << at.m_Mask << ": DX11 id " << d.m_ObjectID
							  << " depth " << d.m_Depth << " uv (" << d.m_U << ", " << d.m_V << "), Vulkan id " << v.m_ObjectID
							  << " depth " << v.m_Depth << " uv (" << v.m_U << ", " << v.m_V << ")";
			}
		}
		std::cout << "  " << i_pName << ": " << i_Picks.size() << " picks, " << ids_apart << " ids differ, depth max "
				  << max_depth << ", uv max " << max_uv << std::endl;
		return dx11;
	}

	//	Overlapping Blinn spheres (pick mask 1), a transparent Phong sphere
	//	in front of them (mask 3), and a billboard (a quad on Billboard.fx,
	//	as scBillboard makes it; mask 2) behind
	void pick_scene(gtSceneBuilder& io_Builder)
	{
		const gtMesh sphere = gtScene::Sphere(16, 32);
		matMaterial* blinn = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted));
		io_Builder.Add(sphere, blinn, gtScene::Place(-0.6f, 0.2f, 0, 1.0f))->SetPickMask(1);
		io_Builder.Add(sphere, blinn, gtScene::Place(0.5f, -0.1f, 0.3f, 0.8f))->SetPickMask(1);
		io_Builder.Add(sphere, blinn, gtScene::Place(0.1f, 0.9f, -0.5f, 0.7f))->SetPickMask(1);
		matMaterial* transparent = io_Builder.Keep(gtScene::DataMaterial("Phong.fx", [](effShaderData* i_pData)
		{
			gtScene::SetPhong(i_pData, gtScene::e_Tinted);
			dynamic_cast<effPhongData*>(i_pData)->m_Transparency = 0.4f;
		}));
		io_Builder.Add(sphere, transparent, gtScene::Place(-0.2f, -0.6f, 1.2f, 0.5f))->SetPickMask(3);
		matMaterial* billboard = io_Builder.Keep(gtScene::DataMaterial("Billboard.fx", [](effShaderData* i_pData)
		{
			dynamic_cast<effTexturedData*>(i_pData)->m_Color = maFloatRGBA(1, 1, 1, 1);
		}));
		billboard->SetHasSpecular(false);
		io_Builder.Add(gtScene::Plane(), billboard, gtScene::Place(1.4f, 1.2f, -1.0f, 0.9f))->SetPickMask(2);
	}
}

TEST(g3dPickVkTest, PickMatchesDX11)
{
	const int w = 128, h = 128;
	std::vector<gtPickAt> picks;
	for (uint32_t mask : { 0u, 1u })
	{
		for (int y = 0; y < 16; y++)
			for (int x = 0; x < 16; x++)
				picks.push_back({ x * 8 + 4, y * 8 + 3, mask });
	}
	const std::vector<g3dPickInfo> dx11 = expect_same_picks("PickMatchesDX11", pick_scene, w, h, picks);
	if (dx11.empty())
		return;

	//	guard: DX11 picks objects at different depths, and mask 1 drops the
	//	billboard. (IDs number the nodes drawn into each pick's one-pixel
	//	frustum, the rest being culled, so they aren't the same object from
	//	one pick to the next.)
	const size_t n = picks.size() / 2;
	int hits = 0, masked_away = 0;
	std::set<int> depths;
	for (size_t i = 0; i < n; i++)
	{
		hits += dx11[i].m_ObjectID != 0;
		masked_away += dx11[i].m_ObjectID != 0 && dx11[n + i].m_ObjectID == 0;
		if (dx11[i].m_ObjectID != 0)
			depths.insert((int)std::lround(dx11[i].m_Depth * 4));
	}
	std::cout << "  DX11: " << hits << " picks hit, " << masked_away << " masked away, " << depths.size() << " depths" << std::endl;
	EXPECT_GT(hits, (int)n / 4) << "DX11 picked almost nothing";
	EXPECT_GT(masked_away, 0) << "the pick mask changed nothing on DX11";
	EXPECT_GE(depths.size(), 4u) << "DX11's picks don't reach the objects at different depths";
}

TEST(g3dPickVkTest, PickEdgesAndBackgroundMatchDX11)
{
	//	a window that isn't square: each pick's sub-viewport camera keeps
	//	the viewer camera's aspect
	const int w = 160, h = 96;
	const std::vector<gtPickAt> corners = {
		{ 0, 0, 0 }, { w - 1, 0, 0 }, { 0, h - 1, 0 }, { w - 1, h - 1, 0 }, { w / 2, h / 2, 0 },
		{ -1, 0, 0 }, { w, h - 1, 0 },		// outside: no pick (ID 0) on either
	};

	//	a wall over the whole view behind the spheres: the corners pick it
	const std::vector<g3dPickInfo> walled = expect_same_picks("PickEdgesAndBackgroundMatchDX11/wall", [](gtSceneBuilder& io_Builder)
	{
		pick_scene(io_Builder);
		maMatrix4x4 wall;
		wall.MakeScale(8.0f, 8.0f, 1.0f);
		wall.TranslateBy(0, 0, -3.0f);
		matMaterial* lambert = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Tinted));
		io_Builder.Add(gtScene::Plane(), lambert, wall);
	}, w, h, corners);
	if (walled.empty())
		return;
	for (int i = 0; i < 4; i++)
		EXPECT_NE(0u, walled[i].m_ObjectID) << "DX11 missed the wall at corner " << i;
	EXPECT_EQ(0u, walled[5].m_ObjectID);
	EXPECT_EQ(0u, walled[6].m_ObjectID);

	//	no wall: the corners and the space between the objects are background (ID 0)
	std::vector<gtPickAt> background = corners;
	background.push_back({ 4, h / 2, 0 });
	background.push_back({ w - 3, h - 10, 1 });
	const std::vector<g3dPickInfo> open = expect_same_picks("PickEdgesAndBackgroundMatchDX11/background", pick_scene, w, h, background);
	for (size_t i = 0; i < open.size(); i++)
	{
		if (i != 4)
			EXPECT_EQ(0u, open[i].m_ObjectID) << "DX11 picked something at background pick " << i;
	}
}

//	Plan 2b: a MASK plane (the checker's cleared cells cut out) in front of
//	a sphere picks as the opaque plane with those cells removed from its
//	mesh: background, plane or sphere, at the same depth, at every pick
TEST(g3dPickVkTest, PbrMaskPicksAsHoledGrid)
{
	const int w = 128, h = 128;
	std::vector<gtPickAt> picks;
	for (int y = 0; y < 16; y++)
		for (int x = 0; x < 16; x++)
			picks.push_back({ x * 8 + 4, y * 8 + 3, 0 });
	gtTempDir dir;
	auto scene = [&](int i_Mode, bool i_bHoles)
	{
		return [&dir, i_Mode, i_bHoles](gtSceneBuilder& io_Builder)
		{
			matMaterial* plane = io_Builder.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
			gtScene::SetInt(plane, effPbrData::c_AlphaMode, i_Mode);
			gtScene::SetTexture(plane, effPbrData::c_BaseColorMap,
								io_Builder.Keep(gtScene::CheckerTexture(dir.GetPath(), 64, 64, 8, true)));
			io_Builder.Add(i_bHoles ? gtScene::GridWithHoles(8) : gtScene::Grid(8), plane, gtScene::Place(0, 0, 1.0f, 1.6f));
			matMaterial* behind = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted));
			io_Builder.Add(gtScene::Sphere(16, 32), behind, gtScene::Place(0.4f, 0, -1.0f, 1.0f));
		};
	};
	std::vector<g3dPickInfo> mask, holed, solid;
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]()
		{
			mask = pick_all(scene(effPbrData::e_Mask, false), w, h, picks);
			holed = pick_all(scene(effPbrData::e_Opaque, true), w, h, picks);
			solid = pick_all(scene(effPbrData::e_Opaque, false), w, h, picks);
		}))
		return;
	ASSERT_EQ(picks.size(), mask.size());
	ASSERT_EQ(picks.size(), holed.size());
	ASSERT_EQ(picks.size(), solid.size());
	int through = 0;
	for (size_t i = 0; i < picks.size(); i++)
	{
		const g3dPickInfo& m = mask[i];
		const g3dPickInfo& o = holed[i];
		if ((m.m_ObjectID != 0) != (o.m_ObjectID != 0) || !(std::fabs(m.m_Depth - o.m_Depth) <= 1e-5f))
		{
			ADD_FAILURE() << "pick (" << picks[i].m_X << ", " << picks[i].m_Y << "): MASK id " << m.m_ObjectID << " depth "
						  << m.m_Depth << ", holed mesh id " << o.m_ObjectID << " depth " << o.m_Depth;
		}
		through += !(std::fabs(m.m_Depth - solid[i].m_Depth) <= 1e-5f);
	}
	//	guard: some picks go through the holes (to the sphere or the background)
	std::cout << "  " << through << " of " << picks.size() << " picks through the holes" << std::endl;
	EXPECT_GT(through, (int)picks.size() / 8) << "the holes changed no pick";
}
