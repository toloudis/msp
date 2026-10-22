/*****************************************************************************
**  shdwBakeVkTest.cpp
**
**      Baking (g3dBake, shdwBakingRenderer) against GraphicsDX11's: each
**      case runs g3dBake::Init and BakeNextNode until it returns false, as
**      api3dBakeScene does, on each backend into its own temporary folder;
**      both must write the same files (and give the same texture name
**      map), and each file must decode to the same pixels within the
**      image tolerance. The scene has a node for each of the baker's
**      branches: opaque nodes (the shadow-node path, or the non-shadow
**      path with single-light rendering off), a wireframe node (non-solid)
**      and a transparent one; the cases cover LPV GI, AO (which DX11's
**      bake leaves out), HDR AA, normals, no lit pass, 512 texels, a
**      texture reduction (which DX11 ignores), RGBA16F targets and TGA.
**      Then fragments with
**      odd UVs (Review Focus 5): no UVs, UVs partly outside 0..1, and UVs
**      in -1..2 (a UV area of 2 or more, which the baker skips).
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
#include "gtTempDir.hpp"

#ifndef MA_CONSTANTS_HPP
#include "Core/Ma/maConstants.hpp"
#endif
#ifndef MA_FUNCTIONS_HPP
#include "Core/Ma/maFunctions.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef G3D_BAKE_HPP
#include "Graphics/G3d/g3dBake.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_LIGHTMGR_HPP
#include "Graphics/G3d/g3dLightMgr.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/G3d/g3dSingleLightRendering.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_TGA
#include <stb_image.h>

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

namespace
{
	//	What one bake wrote: each file's bytes by name, and the texture
	//	name map
	struct BakeResult
	{
		std::map<std::string, std::vector<uint8_t>>	m_Files;
		std::vector<std::string>					m_NameMap;	// the texture name map's names
		int											m_Count = 0;	// g3dBake::Init's
		int											m_Calls = 0;	// BakeNextNode calls
	};

	struct BakeCase
	{
		std::string		m_Name;
		gtSceneFunction	m_Build;
		std::function<void(g3dPrefs::g3dRenderPrefs&)>	m_Prefs;
		bool			m_bSingleLight = true;
		std::string		m_Format = "png";
		int				m_Res = 128;
		int				m_TextureReduce = 0;
		bool			m_bHalfFloat = false;	// RGBA16F (api3dBakeScene's e_RGBA16f), else RGBA8
		//	DX11 draws on a good part of every file (else of one at least)
		bool			m_bEveryFileCovered = true;
	};

	//	A material shader's material with its own values and its diffuse
	//	colour (g_diffuse) set to i_Color
	matMaterial* coloured(const char* i_pShader, const maFloatRGBA& i_Color)
	{
		matMaterial* material = gtScene::ShaderMaterial(i_pShader, gtScene::e_Defaults);
		std::vector<effParamColor*> colors;
		material->GetShaderParams()->GetAllColorParams(colors);
		for (effParamColor* color : colors)
		{
			if (color->GetName() == "g_diffuse")
				color->Property().SetValue(i_Color);
		}
		return material;
	}

	g3dSceneNode* shadowed(g3dSceneNode* io_pNode, const char* i_pBakeName)
	{
		g3dFragment* fragment = const_cast<g3dFragment*>(io_pNode->GetFragment());
		fragment->SetCastsShadow(true);
		fragment->SetReceivesShadow(true);
		io_pNode->SetBakeName(i_pBakeName);
		return io_pNode;
	}

	//	A white floor, a red wall on its left and a white sphere on it, a
	//	wireframe sphere and a transparent one, the usual two lights, a GI
	//	projected light with a shadow map (LPV GI's light) and a global
	//	environment (the only pass the wireframe node, the non-shadow path
	//	and the bake without the lit pass draw). Every node has its own bake
	//	name (the file name's start).
	void bake_room(gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
	{
		io_Builder.LightsCastShadows(true);
		matMaterial* white = io_Builder.Keep(coloured("Lambert.fx", maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f)));
		matMaterial* red = io_Builder.Keep(coloured("Lambert.fx", maFloatRGBA(0.8f, 0.08f, 0.04f, 1.0f)));
		matMaterial* blinn = io_Builder.Keep(coloured("Blinn.fx", maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f)));
		const gtMesh plane = gtScene::Plane();
		maMatrix4x4 floor;
		floor.MakeScale(2.0f, 2.0f, 2.0f);
		floor.RotateBy(-maConstants::c_fPI_Div_2, maVector3d(1, 0, 0));
		floor.TranslateBy(0, -1.0f, 0);
		shadowed(io_Builder.Add(plane, white, floor), "floor");
		maMatrix4x4 wall;
		wall.MakeScale(2.0f, 2.0f, 2.0f);
		wall.RotateBy(maConstants::c_fPI_Div_2, maVector3d(0, 1, 0));
		wall.TranslateBy(-2.0f, 0, 0);
		shadowed(io_Builder.Add(plane, red, wall), "wall");
		const gtMesh sphere = gtScene::Sphere(16, 32);
		shadowed(io_Builder.Add(sphere, blinn, gtScene::Place(-0.4f, -0.4f, 0.2f, 0.6f)), "sphere");
		g3dSceneNode* wire = shadowed(io_Builder.Add(sphere, white, gtScene::Place(0.9f, -0.6f, -0.5f, 0.4f)), "wire");
		wire->SetDrawStyle(g3dSceneNode::e_Wireframe);
		matMaterial* glass = io_Builder.Keep(gtScene::DataMaterial("Phong.fx", [](effShaderData* io_pData)
		{
			gtScene::SetPhong(io_pData, gtScene::e_NonDefault);		// transparency 0.7
		}));
		shadowed(io_Builder.Add(sphere, glass, gtScene::Place(0.8f, -0.6f, 0.8f, 0.35f)), "glass");
		io_Builder.TwoLights();

		g3dProjectedLight* light = io_Builder.ProjectedLight(maPoint3d(0.6f, 2.0f, 2.5f), maPoint3d(-0.5f, -1.0f, 0),
															 maFloatRGBA(1.0f, 0.95f, 0.9f, 1.0f), 256);
		light->SetGIEnabled(true);
		matTexture* diffuse = io_Builder.Keep(gtScene::HdrTexture(i_Dir, 128, 64, 2.0f));
		matTexture* specular = io_Builder.Keep(gtScene::HdrTexture(i_Dir, 256, 128, 2.0f));
		io_Builder.GlobalEnvironment(diffuse, specular, false);

		ssgiParams params;
		io_Builder.Scene().GetSSGISettings(params);
		params.m_LPVVolumeSize = 32;
		params.m_LPVRSMSize = 512;
		params.m_LPVIteration = 2;
		io_Builder.Scene().SetSSGISettings(params);
		io_Builder.Camera().LookAt(maPoint3d(2.0f, 2.0f, 5.0f), maPoint3d(0, -0.5f, 0), maVector3d(0, 1, 0));
	}

	//	A 4x4-quad plane with its UVs edited by i_Edit (bake name "odd") and
	//	an ordinary sphere
	gtSceneFunction odd_uvs(const std::function<void(gtMesh&)>& i_Edit)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path&)
		{
			io_Builder.LightsCastShadows(true);
			matMaterial* white = io_Builder.Keep(coloured("Lambert.fx", maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f)));
			gtMesh mesh = gtScene::Grid(4);
			i_Edit(mesh);
			shadowed(io_Builder.Add(mesh, white, gtScene::Place(0, 0, 0, 1.0f)), "odd");
			//	next to it, so each case writes something
			shadowed(io_Builder.Add(gtScene::Sphere(16, 32), white, gtScene::Place(0, 0, 0.8f, 0.4f)), "sphere");
			io_Builder.TwoLights();
		};
	}

	//	Runs one case on the backend that is up, into a fresh folder
	BakeResult bake(const BakeCase& i_Case, const std::filesystem::path& i_Dir)
	{
		BakeResult result;
		const std::filesystem::path out = i_Dir / i_Case.m_Name;
		std::filesystem::create_directories(out);

		gtPrefsScope prefs;
		maFunctions::ResetRand();
		g3dSingleLightRendering::SetDoSingleLightRendering(i_Case.m_bSingleLight);
		g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();
		//	cptrRenderBakeDataUtil's bake layer: multipass, the bake data's
		//	switches (all on by default), then the pass's renderer
		p.m_bMultipassOn = true;
		p.m_bEnableShadows = true;
		p.m_bEnableEnvironment = true;
		p.m_bEnableLitPass = true;
		p.m_bHDRAA = false;
		p.m_bAOInvalid = false;
		p.m_bGIInvalid = false;
		p.m_RendererType = g3dSceneRendererTypes::e_HDR;
		if (i_Case.m_Prefs)
			i_Case.m_Prefs(p);

		gtSceneBuilder builder;
		g3dLightMgr::SetAmbient(maFloatRGBA(0.1f, 0.15f, 0.2f, 1));
		i_Case.m_Build(builder, i_Dir);
		builder.RenderTargets();		// the projected light's shadow map

		g2dPFD pfd(g2dPFD::e_Color, 32);
		if (i_Case.m_bHalfFloat)
		{
			pfd = g2dPFD(g2dPFD::e_RGBA16f, 16 * 4);
		}
		else
		{
			pfd.Set(16, 8, 8, 8, 0, 8, 24, 8, 32);		// as api3dBakeScene's e_RGBA8
		}

		fsLocator locator;
		fsFileUtil::UnicodeStringToLocator(itString(out.wstring().c_str()), locator);
		std::map<const g3dFragment*, std::string> names;
		result.m_Count = g3dBake::Init(pfd, &builder.Scene(), locator, 1.25f, builder.Camera(), true,
									   i_Case.m_Format, i_Case.m_Res, i_Case.m_TextureReduce);
		do
		{
			result.m_Calls++;
		} while (g3dBake::BakeNextNode(names) && result.m_Calls < 1000);
		g3dBake::CleanUp();

		for (const auto& entry : names)
			result.m_NameMap.push_back(entry.second);
		for (const auto& entry : std::filesystem::directory_iterator(out))
		{
			std::ifstream file(entry.path(), std::ios::binary);
			result.m_Files[entry.path().filename().string()] =
				std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		}
		return result;
	}

	//	The file decoded to RGBA8 (PNG and TGA only), or empty
	std::vector<uint8_t> decode(const std::vector<uint8_t>& i_File, int& o_Width, int& o_Height)
	{
		int channels = 0;
		stbi_uc* pixels = stbi_load_from_memory(i_File.data(), (int)i_File.size(), &o_Width, &o_Height, &channels, 4);
		std::vector<uint8_t> rgba;
		if (pixels)
		{
			rgba.assign(pixels, pixels + (size_t)o_Width * o_Height * 4);
			stbi_image_free(pixels);
		}
		return rgba;
	}

	void compare(const std::string& i_Test, const BakeCase& i_Case, const BakeResult& i_DX11, const BakeResult& i_Vulkan)
	{
		const std::string name = i_Test + "/" + i_Case.m_Name;
		SCOPED_TRACE(name);
		EXPECT_EQ(i_DX11.m_Count, i_Vulkan.m_Count);
		EXPECT_EQ(i_DX11.m_Calls, i_Vulkan.m_Calls);
		EXPECT_EQ(i_DX11.m_NameMap, i_Vulkan.m_NameMap);
		std::vector<std::string> dx11Names, vulkanNames;
		for (const auto& f : i_DX11.m_Files)
			dx11Names.push_back(f.first);
		for (const auto& f : i_Vulkan.m_Files)
			vulkanNames.push_back(f.first);
		EXPECT_EQ(dx11Names, vulkanNames);
		EXPECT_FALSE(dx11Names.empty()) << "DX11 baked nothing";

		int coveredFiles = 0;
		for (const auto& f : i_DX11.m_Files)
		{
			auto other = i_Vulkan.m_Files.find(f.first);
			if (other == i_Vulkan.m_Files.end())
				continue;
			const std::string fileName = name + "/" + f.first;
			int w[2] = { 0, 0 }, h[2] = { 0, 0 };
			std::vector<uint8_t> a = decode(f.second, w[0], h[0]);
			std::vector<uint8_t> b = decode(other->second, w[1], h[1]);
			EXPECT_FALSE(a.empty()) << fileName << ": DX11's file doesn't decode";
			EXPECT_FALSE(b.empty()) << fileName << ": Vulkan's file doesn't decode";
			if (a.empty() || b.empty())
				continue;
			ASSERT_EQ(w[0], w[1]) << fileName;
			ASSERT_EQ(h[0], h[1]) << fileName;
			EXPECT_EQ(w[0], i_Case.m_Res) << fileName;

			//	the bake drew on a good part of the texture
			int covered = 0;
			for (size_t i = 0; i < a.size(); i += 4)
				covered += (a[i] | a[i + 1] | a[i + 2]) != 0;
			coveredFiles += covered > w[0] * h[0] / 20;
			if (i_Case.m_bEveryFileCovered)
				EXPECT_GT(covered, w[0] * h[0] / 20) << fileName << ": DX11 baked almost nothing";

			const gtCompareResult result = gtCompareImages(a.data(), b.data(), w[0], h[0]);
			std::cout << "  " << fileName << ": " << result.Describe() << std::endl;
			if (!result.m_bPass)
			{
				ADD_FAILURE() << fileName << ": " << result.Describe() << " (images in "
					<< gtSaveComparison(fileName, a.data(), b.data(), w[0], h[0]) << ")";
			}
		}
		EXPECT_GT(coveredFiles, 0) << name << ": DX11 baked nothing visible";
	}

	//	Every case on DX11, then every case on Vulkan, each backend's in one
	//	run (so one bake's clean-up meets the next bake, as in MachStudio:
	//	an LPV bake followed by one without LPV crashed DX11 until
	//	shdwBakingRenderer::CleanUp cleared m_GIDepthBuffer)
	void expect_same_bakes(const char* i_pTest, const std::vector<BakeCase>& i_Cases)
	{
		gtTempDir dir;
		std::vector<BakeResult> dx11(i_Cases.size()), vulkan(i_Cases.size());
		gtBackend::Run(gtBackend::e_DX11, [&]()
		{
			for (size_t i = 0; i < i_Cases.size(); i++)
				dx11[i] = bake(i_Cases[i], dir.GetPath() / "dx11");
		});
		if (!gtBackend::Run(gtBackend::e_Vulkan, [&]()
		{
			for (size_t i = 0; i < i_Cases.size(); i++)
				vulkan[i] = bake(i_Cases[i], dir.GetPath() / "vulkan");
		}))
			return;
		for (size_t i = 0; i < i_Cases.size(); i++)
			compare(i_pTest, i_Cases[i], dx11[i], vulkan[i]);
	}

	gtSceneFunction room()
	{
		return [](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir) { bake_room(io_Builder, i_Dir); };
	}

	BakeCase room_case(const char* i_pName, const std::function<void(g3dPrefs::g3dRenderPrefs&)>& i_Prefs)
	{
		BakeCase c;
		c.m_Name = i_pName;
		c.m_Build = room();
		c.m_Prefs = i_Prefs;
		return c;
	}
}

TEST(shdwBakeVkTest, BakeMatchesDX11)
{
	std::vector<BakeCase> cases;
	cases.push_back(room_case("plain", nullptr));
	//	DX11's bake has its AO volumes commented out: AO changes nothing
	cases.push_back(room_case("ao", [](g3dPrefs::g3dRenderPrefs& p) { p.m_bAOInvalid = true; p.m_bEnableSSAO = true; }));
	cases.push_back(room_case("lpv", [](g3dPrefs::g3dRenderPrefs& p) { p.m_bGIInvalid = true; p.m_bEnableSSGI = true; }));
	cases.push_back(room_case("ao_lpv", [](g3dPrefs::g3dRenderPrefs& p)
	{
		p.m_bAOInvalid = true; p.m_bEnableSSAO = true;
		p.m_bGIInvalid = true; p.m_bEnableSSGI = true;
	}));
	cases.push_back(room_case("hdraa", [](g3dPrefs::g3dRenderPrefs& p) { p.m_bHDRAA = true; }));
	cases.push_back(room_case("normals", [](g3dPrefs::g3dRenderPrefs& p) { p.m_RendererType = g3dSceneRendererTypes::e_Normals; }));
	cases.push_back(room_case("no_lit_pass", [](g3dPrefs::g3dRenderPrefs& p) { p.m_bEnableLitPass = false; }));
	{
		BakeCase c = room_case("res512", nullptr);
		c.m_Res = 512;
		cases.push_back(c);
	}
	{
		//	DX11's GetTextureInfo has the reduction commented out
		BakeCase c = room_case("reduce1", nullptr);
		c.m_TextureReduce = 1;
		cases.push_back(c);
	}
	{
		BakeCase c = room_case("half_float", nullptr);
		c.m_bHalfFloat = true;
		cases.push_back(c);
	}
	{
		//	every opaque node through the non-shadow path: DX11 bakes the
		//	material-shader nodes (floor, wall, sphere) black there; the
		//	wireframe and transparent ones draw
		BakeCase c = room_case("non_shadow", nullptr);
		c.m_bSingleLight = false;
		c.m_bEveryFileCovered = false;
		cases.push_back(c);
	}
	{
		BakeCase c = room_case("tga", nullptr);
		c.m_Format = "tga";
		cases.push_back(c);
	}
	expect_same_bakes("BakeMatchesDX11", cases);
}

TEST(shdwBakeVkTest, BakeOddUVsMatchesDX11)
{
	std::vector<BakeCase> cases;
	auto odd = [&](const char* i_pName, const std::function<void(gtMesh&)>& i_Edit)
	{
		BakeCase c;
		c.m_Name = i_pName;
		c.m_Build = odd_uvs(i_Edit);
		c.m_Prefs = [](g3dPrefs::g3dRenderPrefs& p) { p.m_bGIInvalid = false; };
		cases.push_back(c);
	};
	//	a fragment made without UVs (g3dFragmentCreate gets NULL)
	odd("no_uvs", [](gtMesh& io_Mesh) { io_Mesh.m_UVs.clear(); });
	//	UVs -0.5..0.9: partly outside the square, UV area 1.96 (the baker
	//	skips fragments whose UV area, GetUVOverlapFactor, is 2 or more)
	odd("uvs_partly_outside", [](gtMesh& io_Mesh)
	{
		for (maPoint2d& uv : io_Mesh.m_UVs)
			uv = maPoint2d(uv.m_X * 1.4f - 0.5f, uv.m_Y * 1.4f - 0.5f);
	});
	//	UVs -1..2: UV area 9, so the baker skips the plane (and makes its
	//	node inactive in the render layer)
	odd("uvs_minus1_to_2", [](gtMesh& io_Mesh)
	{
		for (maPoint2d& uv : io_Mesh.m_UVs)
			uv = maPoint2d(uv.m_X * 3.0f - 1.0f, uv.m_Y * 3.0f - 1.0f);
	});
	expect_same_bakes("BakeOddUVsMatchesDX11", cases);
}
