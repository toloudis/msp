/*****************************************************************************
**  shdwMaterialsVkTest.cpp
**
**      Every material shader through the HDR renderer on Vulkan against
**      DX11, both made by the creator (g3dSceneRendererCreate::
**      CreateDefaultRenderer) as MachStudio's viewport makes them, compared
**      with gfxcompare's tolerance.
**
**      The materials are every material shader matShaderMgr registers from
**      Eff/private/Shaders/Materials: the ones the material picker lists
**      (GetMaterialShaders) and the hidden Skin, BlinnSkin, Default and
**      testtess, less Hair_SH (Plan 7) and Bake (the baking renderer's,
**      Plan 6). The list must be the same on both backends.
**
**      Each material's scene: a 3x3 grid of spheres, rows with non-default,
**      zero and very large values; columns without textures, with a
**      checker on every texture parameter (a normal map where the parameter
**      is one), and with the material's normal map too (NormalsData, which
**      every material shader reads); lit by a point, a directional and a
**      shadowed projected light (all casting shadows; the spheres cast and
**      receive them) and the global environment.
**
**      The tessellation test gives the materials that tessellate
**      (HasHardwareTessellation) a displacement map, with hardware
**      tessellation off and on.
**
**      Exceptions to gfxcompare's tolerance (max 2/255, at most 0.1% of
**      pixels over 1/255):
**
**        case                       measured                reason
**        every case                 max 52/255, 2.03% over  mip selection:
**            the checker is hard-edged (textures are never softened to hide
**            a difference), and on a texture seen at an angle D3D11 and
**            Vulkan compute a different level of detail (the driver's, per
**            API: vkdAnisotropicTest; head-on they're identical). Where the
**            mip level differs the hard edge differs. Mean error stays under
**            0.12/255. Allowed: 64/255, 2.5% (c_MaxErr, c_MaxPctOver).
**        SpecularFresnel            max 228/255 on 4-5       the same, at the
**            (and tess_off/on)      isolated pixels         sphere tops: the
**            checker is minified so much that neighbouring pixels range
**            over 33..255, and a different mip level lands a pixel in
**            another cell. Allowed: any max, 2.5% (c_MaxErrIsolated).
**        Skin, BlinnSkin, testtess  DX11 draws nothing     their PTess
**            tess_on (no coverage)  (Vulkan neither)       passes don't draw
**            on DX11 (Skin and BlinnSkin's has a vertex shader only, from
**            the old mesh-texture tessellation); the images still compare.
**
**	Extra Large Technology
\****************************************************************************/
#include "Vk/gtVulkanOnly.hpp"
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/Mat/matShaderMgr.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/Mat/matShaderEffect.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <set>
#include <string>
#include <vector>

namespace
{
	//	What the brief names: the picker's materials and the hidden ones,
	//	less Hair_SH and Bake
	const std::set<std::string> c_Expected =
	{
		"Anisotropic.fx", "Blinn.fx", "BlinnReflection.fx", "BlinnSkin.fx", "CarPaint.fx", "Cartoon.fx",
		"Default.fx", "FC3DBlinn.fx", "FC3DPhong.fx", "FC3DPhongTransparent.fx", "FC3DSpecularFresnel.fx",
		"FC3DSubSurfaceScatter_wBlinnSpecular.fx", "Lambert.fx", "Phong.fx", "PhongReflection.fx",
		"Phong_wBump.fx", "Simple.fx", "Skin.fx", "SpecularFresnel.fx", "SubSurfaceScatter.fx",
		"SubSurfaceScatter_wBlinnSpecular.fx", "testtess.fx",
	};

	//	Not offered in the picker, but registered and used (old names the
	//	material parser renames, the default material, the tessellation test)
	const char* const c_Hidden[] = { "Skin.fx", "BlinnSkin.fx", "Default.fx", "testtess.fx" };

	//	The exception table's tolerance (mip selection), and the materials
	//	whose isolated pixels may differ by any amount
	const float c_MaxErr = 64 / 255.0f;
	const float c_MaxErrIsolated = 1.0f;
	const float c_MaxPctOver = 2.5f;
	const std::set<std::string> c_IsolatedFlips = { "SpecularFresnel.fx" };

	//	Their tessellated pass draws nothing on DX11 (see the exception table)
	const std::set<std::string> c_NoTessellatedDraw = { "Skin.fx", "BlinnSkin.fx", "testtess.fx" };

	//	Left out: Plan 7 (hair) and Plan 6 (baking)
	const char* const c_Excluded[] = { "Hair_SH.fx", "Bake.fx" };

	struct Material
	{
		std::string	m_Shader;
		bool		m_bTessellates = false;
	};

	//	The material shaders the backend that is up registers, by name
	std::vector<Material> registered_materials()
	{
		std::set<std::string> names;
		for (const matShaderInfo& info : matShaderMgr::GetMaterialShaders())
			names.insert(info.m_Name);
		for (const char* hidden : c_Hidden)
		{
			if (matShaderMgr::GetEffect(fsLocator(std::string(hidden))))
				names.insert(hidden);
		}
		for (const char* excluded : c_Excluded)
			names.erase(excluded);
		// DX11 has no Vulkan-only material to compare with
		for (auto it = names.begin(); it != names.end(); )
			it = gtVulkanOnly::IsVulkanOnlyMaterial(*it) ? names.erase(it) : std::next(it);
		std::vector<Material> materials;
		for (const std::string& name : names)
		{
			Material material;
			material.m_Shader = name;
			matShaderEffect* effect = matShaderMgr::GetEffect(fsLocator(name));
			EXPECT_TRUE(effect) << name;
			material.m_bTessellates = effect && effect->HasHardwareTessellation();
			materials.push_back(material);
		}
		return materials;
	}

	std::string describe(const std::vector<Material>& i_Materials)
	{
		std::string out;
		for (const Material& m : i_Materials)
			out += m.m_Shader + (m.m_bTessellates ? " (tessellates)\n" : "\n");
		return out;
	}

	//	The list from both backends; fails if they differ or it isn't the
	//	brief's. Empty if a backend's layer couldn't start.
	std::vector<Material> materials()
	{
		std::vector<Material> dx11, vulkan;
		gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = registered_materials(); });
		if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = registered_materials(); }))
			return std::vector<Material>();
		EXPECT_EQ(describe(dx11), describe(vulkan)) << "the backends register different material shaders";
		std::set<std::string> names;
		for (const Material& m : dx11)
			names.insert(m.m_Shader);
		EXPECT_EQ(c_Expected, names);
		return dx11;
	}

	bool is_normal_map(const std::string& i_Param)
	{
		std::string lower = i_Param;
		std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return (char)tolower(c); });
		return lower.find("normal") != std::string::npos;
	}

	//	Casts and receives shadows (fragments do neither by default)
	void casts_shadow(g3dSceneNode* io_pNode)
	{
		g3dFragment* fragment = const_cast<g3dFragment*>(io_pNode->GetFragment());
		fragment->SetCastsShadow(true);
		fragment->SetReceivesShadow(true);
	}

	//	The material's scene (see the header); i_bDisplace: every sphere gets
	//	a displacement map
	gtSceneFunction material_scene(const std::string& i_Shader, bool i_bDisplace)
	{
		return [=](gtSceneBuilder& io_Builder, const std::filesystem::path& i_Dir)
		{
			io_Builder.LightsCastShadows(true);
			const gtMesh sphere = gtScene::Sphere(16, 32);
			const gtScene::Values values[3] = { gtScene::e_NonDefault, gtScene::e_Zero, gtScene::e_Huge };
			matTexture* checker = io_Builder.Keep(gtScene::CheckerTexture(i_Dir, 128, 128, 8));
			matTexture* normals = io_Builder.Keep(gtScene::NormalTexture(i_Dir, 128, 4));
			matTexture* heights = i_bDisplace ? io_Builder.Keep(gtScene::HeightTexture(i_Dir, 128, 3)) : NULL;
			for (int row = 0; row < 3; row++)
			{
				for (int column = 0; column < 3; column++)
				{
					matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(i_Shader, values[row]));
					if (column >= 1)
					{
						std::vector<effParamTexture*> textures;
						material->GetShaderParams()->GetAllTextureParams(textures);
						for (effParamTexture* texture : textures)
							texture->SetTexture(is_normal_map(texture->GetName()) ? normals : checker);
					}
					if (column == 2)
					{
						material->NormalsData().m_pNormalMap = normals;
						material->NormalsData().m_BumpScale = 1.0f;
					}
					if (i_bDisplace)
					{
						material->SetHasDisplacement(true);
						effDisplacementData& displacement = material->DisplacementData();
						displacement.m_pDisplacementMap = heights;
						displacement.m_Scale = 0.15f;
						displacement.m_Bias = 0.5f;
						displacement.m_Blur = 0.0f;
						displacement.m_TessellationValue = 4.0f;
					}
					casts_shadow(io_Builder.Add(sphere, material,
						gtScene::Place(-1.5f + 1.5f * column, 1.25f - 1.25f * row, 0, 0.55f)));
				}
			}
			io_Builder.PointLight(maPoint3d(2, 3, 4), maFloatRGBA(0.9f, 0.8f, 0.7f, 1), 25.0f);
			io_Builder.DirectionalLight(maVector3d(-0.3f, -0.8f, -0.5f), maFloatRGBA(0.4f, 0.5f, 0.6f, 1));
			io_Builder.ProjectedLight(maPoint3d(-2.5f, 2.0f, 7.0f), maPoint3d(0, 0, 0), maFloatRGBA(0.8f, 0.85f, 1.0f, 1), 512);
			matTexture* diffuse = io_Builder.Keep(gtScene::HdrTexture(i_Dir, 128, 64, 4.0f));
			matTexture* specular = io_Builder.Keep(gtScene::HdrTexture(i_Dir, 256, 128, 4.0f));
			io_Builder.GlobalEnvironment(diffuse, specular, false);
		};
	}

	//	Shadow maps rendered before each frame; the exception table's tolerance
	gtRenderOptions options(const std::string& i_Shader, bool i_bTessellation = true)
	{
		gtRenderOptions o;
		o.m_BeforeFrame = [](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); };
		o.m_Prefs = [i_bTessellation](g3dPrefs::g3dRenderPrefs& p) { p.m_bUseHardwareTessellation = i_bTessellation; };
		o.m_MaxErr = c_IsolatedFlips.count(i_Shader) ? c_MaxErrIsolated : c_MaxErr;
		o.m_MaxPctOver = c_MaxPctOver;
		return o;
	}

	//	"Blinn.fx" -> "Blinn"
	std::string case_name(const std::string& i_Shader)
	{
		return i_Shader.substr(0, i_Shader.rfind('.'));
	}
}

TEST(shdwMaterialsVkTest, EveryMaterialMatchesDX11)
{
	const std::vector<Material> list = materials();
	if (list.empty())
		return;
	std::vector<gtCase> cases;
	for (const Material& m : list)
		cases.push_back({ case_name(m.m_Shader), material_scene(m.m_Shader, false), options(m.m_Shader) });
	gtExpectSameImages("EveryMaterialMatchesDX11", cases);
}

TEST(shdwMaterialsVkTest, TessellatedMaterialsMatchDX11)
{
	const std::vector<Material> list = materials();
	if (list.empty())
		return;
	std::vector<gtCase> cases;
	for (const Material& m : list)
	{
		if (!m.m_bTessellates)
			continue;
		cases.push_back({ case_name(m.m_Shader) + "/tess_off", material_scene(m.m_Shader, true), options(m.m_Shader, false) });
		gtRenderOptions on = options(m.m_Shader, true);
		on.m_bCheckCoverage = c_NoTessellatedDraw.count(m.m_Shader) == 0;
		cases.push_back({ case_name(m.m_Shader) + "/tess_on", material_scene(m.m_Shader, true), on });
	}
	EXPECT_FALSE(cases.empty()) << "no material tessellates";
	gtExpectSameImages("TessellatedMaterialsMatchDX11", cases);
}
