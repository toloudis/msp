/*****************************************************************************
**  shdwTransparentVkTest.cpp
**
**      The HDR renderer's transparent pass (shdwPassTransparent) against
**      GraphicsDX11's, both through the creator: overlapping translucent
**      Phong spheres in front of opaque material-shader spheres, drawn
**      sorted (m_TransparencyMode 0), depth-peeled (mode 1, with two
**      translucent planes that cut through each other and the spheres, so
**      no order of whole objects is right), and deferred
**      (m_bEnableDeferredTransparency: after the alpha fill and HDR
**      backup). HDR anti-aliasing is off in those, and on in
**      HDRAATransparencyMatchDX11 (sorted and peeled: the alpha fill's MSAA
**      branch and the peel's depth copies from a multisampled target).
**      Single-light rendering off (the multi-light blend) is covered too.
**
**      Exceptions to gfxcompare's tolerance (max 2/255, at most 0.1% of
**      pixels over 1/255): none.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"

#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif

#include <gtest/gtest.h>

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace
{
	//	A Phong data material with the tinted colours and i_Transparency
	//	(1 opaque); i_Tint picks one of a few diffuse colours
	matMaterial* translucent(float i_Transparency, int i_Tint)
	{
		return gtScene::DataMaterial("Phong.fx", [=](effShaderData* io_pData)
		{
			gtScene::SetPhong(io_pData, gtScene::e_Tinted);
			if (effPhongData* phong = dynamic_cast<effPhongData*>(io_pData))
			{
				const maFloatRGBA tints[3] = { maFloatRGBA(0.6f, 0.2f, 0.15f, 1), maFloatRGBA(0.15f, 0.5f, 0.25f, 1),
											   maFloatRGBA(0.2f, 0.25f, 0.6f, 1) };
				phong->m_ColorDiffuse = tints[i_Tint % 3];
				phong->m_Transparency = i_Transparency;
			}
		});
	}

	//	Opaque Blinn and Lambert spheres behind, three translucent Phong
	//	spheres in front of them overlapping each other; lit by shadow-
	//	casting lights
	void spheres(gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		io_Builder.LightsCastShadows(true);
		const gtMesh sphere = gtScene::Sphere(16, 32);
		const char* shaders[2] = { "Blinn.fx", "Lambert.fx" };
		for (int i = 0; i < 4; i++)
		{
			matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(shaders[i % 2], gtScene::e_Tinted));
			io_Builder.Add(sphere, material, gtScene::Place(-1.5f + 1.0f * i, 0.6f - 0.4f * (i % 2), -1.5f, 0.7f));
		}
		const float transparency[3] = { 0.5f, 0.35f, 0.7f };
		for (int i = 0; i < 3; i++)
		{
			matMaterial* material = io_Builder.Keep(translucent(transparency[i], i));
			io_Builder.Add(sphere, material, gtScene::Place(-0.6f + 0.6f * i, -0.2f + 0.3f * (i % 2), 0.4f * i, 0.65f));
		}
		io_Builder.TwoLights();
	}

	//	The spheres, plus two translucent planes rotated about Y by +-40
	//	degrees, crossing each other and the front spheres
	void spheres_and_planes(gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		spheres(io_Builder, i_Dir);
		const gtMesh plane = gtScene::Plane();
		for (int i = 0; i < 2; i++)
		{
			maMatrix4x4 world;
			world.MakeScale(1.3f, 1.1f, 1.3f);
			world.RotateBy((i == 0 ? 40.0f : -40.0f) * 3.14159265f / 180.0f, maVector3d(0, 1, 0));
			world.TranslateBy(0.1f, -0.1f, 0.3f);
			matMaterial* material = io_Builder.Keep(translucent(i == 0 ? 0.45f : 0.6f, i + 1));
			io_Builder.Add(plane, material, world);
		}
	}

	gtRenderOptions prefs(int i_Mode, bool i_bAA, bool i_bDeferred = false)
	{
		gtRenderOptions options;
		options.m_Prefs = [=](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableTransparent = true;
			p.m_TransparencyMode = i_Mode;
			p.m_bHDRAA = i_bAA;
			p.m_bEnableDeferredTransparency = i_bDeferred;
		};
		return options;
	}
}

TEST(shdwTransparentVkTest, TransparentSortedMatchDX11)
{
	gtRenderOptions multi_light = prefs(0, false);
	multi_light.m_bSingleLight = false;
	gtExpectSameImages("TransparentSortedMatchDX11",
	{
		{ "spheres", spheres, prefs(0, false) },
		{ "planes", spheres_and_planes, prefs(0, false) },
		{ "multi_light", spheres, multi_light },
	});
}

TEST(shdwTransparentVkTest, TransparentDepthPeeledMatchDX11)
{
	gtExpectSameImages("TransparentDepthPeeledMatchDX11",
	{
		{ "spheres", spheres, prefs(1, false) },
		{ "planes", spheres_and_planes, prefs(1, false) },
	});
}

TEST(shdwTransparentVkTest, TransparentDeferredMatchDX11)
{
	gtExpectSameImage("TransparentDeferredMatchDX11", spheres_and_planes, prefs(0, false, true));
}

TEST(shdwTransparentVkTest, HDRAATransparencyMatchDX11)
{
	gtExpectSameImages("HDRAATransparencyMatchDX11",
	{
		{ "sorted", spheres_and_planes, prefs(0, true) },
		{ "peeled", spheres_and_planes, prefs(1, true) },
	});
}
