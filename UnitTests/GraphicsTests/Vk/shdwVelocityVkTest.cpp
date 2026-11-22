/*****************************************************************************
**  shdwVelocityVkTest.cpp
**
**      The velocity map renderer (shdwVelocityMapRendererVk with
**      shdwPassVelocity) on Vulkan against DX11, made by the creator
**      (g3dSceneRendererCreate::CreateVelocityMapRenderer) as MachStudio's
**      capture thread makes it for the velocity pass:
**      - over three frames in which one sphere moves, another turns and the
**        camera moves;
**      - on the first frame alone (no previous transforms), over frames in
**        which nothing moves, and with the default (HDR) renderer, which
**        shares its buffers (cptrRenderThread), drawn between two velocity
**        frames.
**
**      Each case compares the image the renderer draws (the velocity map
**      scaled by 100 around grey) and the velocity map itself (RG32F),
**      read back after the last frame: in the image's units (x 100) within
**      the spec's tolerance, with no NaN on either backend except on the
**      first frame, where DX11 writes some and Vulkan must write them in
**      the same texels.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"

#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/G3d/g3dLayer.hpp"
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
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace
{
	const int c_Size = 256;

	//	What a run keeps between the options' hooks: the renderer made, the
	//	HDR renderer drawn between frames (and its target), the frame
	//	number, and the velocity map read after the last frame, per backend
	struct VelocityRun
	{
		g3dSceneRenderer*					m_pVelocity = NULL;
		std::unique_ptr<g3dSceneRenderer>	m_pHDR;
		std::unique_ptr<gtTarget>			m_pHDRTarget;
		int									m_Frame = 0;
		std::vector<float>					m_Map[2];		// x, y per texel, top row first
	};

	//	The velocity renderer's options: i_Frames frames, a time step
	//	between them (the pass takes the camera's previous position only
	//	when the time moves); i_BeforeFrame(builder, frame) runs before
	//	each, and with i_bDefaultBetween the default renderer (the HDR
	//	renderer, sharing the velocity renderer's buffers as in
	//	cptrRenderThread) draws the scene before every frame after the first. The velocity map is read into the run after the last frame.
	gtRenderOptions velocity_options(const std::shared_ptr<VelocityRun>& i_Run, int i_Frames, float i_Step,
									 const std::function<void(gtSceneBuilder&, int)>& i_BeforeFrame, bool i_bDefaultBetween)
	{
		gtRenderOptions options;
		options.m_Frames = i_Frames;
		options.m_SimTimeStep = i_Step;
		options.m_CreateRenderer = [=]()
		{
			i_Run->m_Frame = 0;
			i_Run->m_pVelocity = g3dSceneRendererCreate::CreateVelocityMapRenderer();
			if (i_bDefaultBetween)
			{
				i_Run->m_pHDR.reset(g3dSceneRendererCreate::CreateDefaultRenderer());
				i_Run->m_pHDR->ShareBuffers(i_Run->m_pVelocity);
				const float clear[4] = { 0, 0, 0, 0 };
				i_Run->m_pHDRTarget.reset(new gtTarget(gtBackend::Current(), c_Size, c_Size, clear));
			}
			return i_Run->m_pVelocity;
		};
		options.m_BeforeFrame = [=](gtSceneBuilder& io_Builder)
		{
			const int frame = i_Run->m_Frame++;
			if (i_BeforeFrame)
				i_BeforeFrame(io_Builder, frame);
			if (i_Run->m_pHDR && frame > 0)
			{
				g2dRenderTarget* target = i_Run->m_pHDRTarget->Target();
				target->MakeCurrent();
				target->Clear(maFloatRGBA(0, 0, 0, 0), true, 1.0f);
				i_Run->m_pHDR->Render(target, io_Builder.Camera(), io_Builder.Scene(), io_Builder.ViewerLayers(),
									  1.25f + frame * i_Step - 0.5f * i_Step);
			}
		};
		options.m_AfterRender = [=](gtSceneBuilder&)
		{
			const matTexture* map = dynamic_cast<const matTexture*>(i_Run->m_pVelocity->GetRawBuffer());
			EXPECT_TRUE(map != NULL);
			//	the format reported is the buffer's (RG32F, shdwFrameBufferMgr's
			//	e_GR32f): cptrRenderUtil::GetCapturePFD makes the capture's
			//	images from it
			EXPECT_EQ(g2dPFD::e_GR32f, i_Run->m_pVelocity->GetRawBufferPFD());
			if (const g2dRenderTarget* buffer = i_Run->m_pVelocity->GetRawBuffer())
				EXPECT_EQ(buffer->GetPixelFormat().GetPixelFormat(), i_Run->m_pVelocity->GetRawBufferPFD());
			std::vector<float>& out = i_Run->m_Map[gtBackend::Current()];
			out.clear();
			if (map)
			{
				const std::vector<uint8_t> bytes = gtReadTexture(gtBackend::Current(), map, 8);
				EXPECT_EQ((size_t)c_Size * c_Size * 8, bytes.size());
				out.resize(bytes.size() / sizeof(float));
				if (!out.empty())
					std::memcpy(out.data(), bytes.data(), out.size() * sizeof(float));
			}
			i_Run->m_pHDR.reset();
			i_Run->m_pHDRTarget.reset();
		};
		return options;
	}

	//	Spheres before a wall: the moving and turning spheres are returned
	struct VelocityScene
	{
		g3dSceneNode*	m_pMoving = NULL;
		g3dSceneNode*	m_pTurning = NULL;
	};

	VelocityScene velocity_scene(gtSceneBuilder& io_Builder)
	{
		VelocityScene scene;
		const gtMesh sphere = gtScene::Sphere(16, 32);
		matMaterial* wall = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Tinted));
		io_Builder.Add(gtScene::Plane(), wall, gtScene::Place(0, 0, -2.0f, 3.0f));
		matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted));
		scene.m_pMoving = io_Builder.Add(sphere, material, gtScene::Place(-1.2f, 0.3f, 0, 0.6f));
		scene.m_pTurning = io_Builder.Add(sphere, material, gtScene::Place(1.2f, -0.2f, 0, 0.7f));
		io_Builder.Add(sphere, material, gtScene::Place(0, -0.8f, 0.5f, 0.4f));
		io_Builder.TwoLights();
		return scene;
	}

	//	Moves the first sphere, turns the second (about its vertical axis,
	//	so its surface moves across the view) and moves the camera, by
	//	i_Frame
	void move(gtSceneBuilder& io_Builder, const VelocityScene& i_Scene, int i_Frame)
	{
		const float t = (float)i_Frame;
		i_Scene.m_pMoving->SetTransform(gtScene::Place(-1.2f + 0.08f * t, 0.3f - 0.05f * t, 0.1f * t, 0.6f));
		maMatrix4x4 turn;
		turn.MakeRotateY(0.15f * t);
		turn.ScaleBy(0.7f, 0.7f, 0.7f);
		turn.TranslateBy(1.2f, -0.2f, 0);
		i_Scene.m_pTurning->SetTransform(turn);
		io_Builder.Scene().UpdateWorldData();
		io_Builder.Camera().LookAt(maPoint3d(0.05f * t, 0.02f * t, 5.0f - 0.1f * t), maPoint3d(0, 0, 0), maVector3d(0, 1, 0));
	}

	//	Compares the two backends' velocity maps in the image's units
	//	(x 100) within the spec's tolerance. Neither has a NaN, or with
	//	i_bNaNsAsDX11 the NaNs are in the same places on both. Returns
	//	DX11's values with a velocity over 1/255 in those units.
	int expect_same_maps(const std::string& i_Name, const VelocityRun& i_Run, bool i_bNaNsAsDX11)
	{
		SCOPED_TRACE(i_Name);
		const std::vector<float>& dx11 = i_Run.m_Map[gtBackend::e_DX11];
		const std::vector<float>& vulkan = i_Run.m_Map[gtBackend::e_Vulkan];
		EXPECT_EQ((size_t)c_Size * c_Size * 2, dx11.size());
		EXPECT_EQ(dx11.size(), vulkan.size());
		if (dx11.size() != (size_t)c_Size * c_Size * 2 || dx11.size() != vulkan.size())
			return 0;
		int nan_dx11 = 0, nan_vulkan = 0, nan_apart = 0, over = 0, moving = 0;
		float max_err = 0;
		for (size_t i = 0; i < dx11.size(); i++)
		{
			nan_dx11 += std::isnan(dx11[i]);
			nan_vulkan += std::isnan(vulkan[i]);
			nan_apart += std::isnan(dx11[i]) != std::isnan(vulkan[i]);
			if (std::isnan(dx11[i]) || std::isnan(vulkan[i]))
				continue;
			const float err = std::fabs(dx11[i] - vulkan[i]) * 100.0f;
			max_err = std::max(max_err, err);
			over += err > 1 / 255.0f;
			if (std::fabs(dx11[i]) * 100.0f > 1 / 255.0f)
				moving++;
		}
		const float pct_over = 100.0f * over / (float)dx11.size();
		std::cout << "  " << i_Name << " (velocity map): max " << max_err * 255.0f << "/255, " << pct_over
				  << "% over 1/255, " << moving << " moving values on DX11, NaNs " << nan_dx11 << " / " << nan_vulkan << std::endl;
		EXPECT_EQ(0, nan_apart);
		if (!i_bNaNsAsDX11)
		{
			EXPECT_EQ(0, nan_dx11);
			EXPECT_EQ(0, nan_vulkan);
		}
		EXPECT_LE(max_err, 2 / 255.0f);
		EXPECT_LE(pct_over, 0.1f);
		return moving;
	}

	struct VelocityCase
	{
		std::string						m_Name;
		std::shared_ptr<VelocityRun>	m_Run;
		gtRenderOptions					m_Options;
		bool							m_bNaNsAsDX11 = false;
	};

	void expect_cases(const char* i_pTest, const std::vector<VelocityCase>& i_Cases,
					  const std::function<void(const VelocityCase&, int)>& i_Check = nullptr)
	{
		std::vector<gtCase> cases;
		for (const VelocityCase& c : i_Cases)
		{
			cases.push_back({ c.m_Name, [](gtSceneBuilder& io_Builder, const std::filesystem::path&) { velocity_scene(io_Builder); },
							  c.m_Options });
		}
		gtExpectSameImages(i_pTest, cases);
		for (const VelocityCase& c : i_Cases)
		{
			const int moving = expect_same_maps(std::string(i_pTest) + "/" + c.m_Name, *c.m_Run, c.m_bNaNsAsDX11);
			if (i_Check)
				i_Check(c, moving);
		}
	}

	//	The nodes velocity_scene made, found again in the builder (the
	//	moving sphere is the root's second child, the turning one its third)
	VelocityScene find_scene(gtSceneBuilder& io_Builder)
	{
		VelocityScene scene;
		g3dSceneNode* root = io_Builder.Layer().GetRootNode();
		scene.m_pMoving = root->GetChild(1);
		scene.m_pTurning = root->GetChild(2);
		return scene;
	}
}

TEST(shdwVelocityVkTest, VelocityMapMatchesDX11)
{
	std::shared_ptr<VelocityRun> run(new VelocityRun);
	VelocityCase moving{ "moving", run, velocity_options(run, 3, 0.04f, [](gtSceneBuilder& io_Builder, int i_Frame)
	{
		move(io_Builder, find_scene(io_Builder), i_Frame);
	}, false) };
	expect_cases("VelocityMapMatchesDX11", { moving }, [](const VelocityCase&, int i_Moving)
	{
		//	guard: DX11 sees the motion
		EXPECT_GT(i_Moving, c_Size * c_Size / 50) << "nothing moves in DX11's velocity map";
	});
}

TEST(shdwVelocityVkTest, VelocityFirstAndStillFramesMatchDX11)
{
	std::vector<VelocityCase> cases;
	{
		//	DX11's first frame takes the previous camera to be a default
		//	camCamera (VelocityStateManager's), at the origin looking down +Z:
		//	triangles that cross its plane get infinite previous positions,
		//	which the rasterizer interpolates into NaNs (89984 values here,
		//	in the same texels on both backends; the image shows them as 0)
		std::shared_ptr<VelocityRun> run(new VelocityRun);
		cases.push_back({ "first_frame", run, velocity_options(run, 1, 0.04f, nullptr, false), true });
	}
	{
		std::shared_ptr<VelocityRun> run(new VelocityRun);
		cases.push_back({ "still", run, velocity_options(run, 3, 0.04f, nullptr, false) });
	}
	{
		std::shared_ptr<VelocityRun> run(new VelocityRun);
		cases.push_back({ "default_between", run, velocity_options(run, 2, 0.04f, [](gtSceneBuilder& io_Builder, int i_Frame)
		{
			move(io_Builder, find_scene(io_Builder), i_Frame);
		}, true) });
	}
	expect_cases("VelocityFirstAndStillFramesMatchDX11", cases, [](const VelocityCase& i_Case, int i_Moving)
	{
		//	after the first frame, nothing that stays still has a velocity
		if (i_Case.m_Name == "still")
			EXPECT_EQ(0, i_Moving);
	});
}
