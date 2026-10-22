/*****************************************************************************
**  vkdLinkStagesTest.cpp
**
**      vkdTrimUnconsumedOutputs and vkdPipelineCache's link step: a
**      producer's outputs the pixel shader doesn't take are removed (and
**      only those; built-ins stay, SV_ClipDistance still clips), every
**      pass of every embedded effect whose pixel shader takes less than
**      its producer writes makes a pipeline without a validation warning
**      (WARNING-Shader-OutputNotConsumed counts again), and scenes that
**      draw with trimmed producers give the same bytes as without
**      trimming.
**
**	Extra Large Technology
\****************************************************************************/
#include "Vk/gtVulkanContext.hpp"
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtRender.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"
#include "gtTempDir.hpp"

#ifndef GVK_FXEMBEDDEDSHADERSVK_HPP
#include "GraphicsVulkan/Fx/fxEmbeddedShadersVk.hpp"
#endif
#ifndef GVK_VKDLINKSTAGES_HPP
#include "GraphicsVulkan/vkd/private/vkdLinkStages.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif
#ifndef GVK_VKDPIPELINECACHE_HPP
#include "GraphicsVulkan/vkd/vkdPipelineCache.hpp"
#endif
#ifndef GVK_VKDSHADER_HPP
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif

#include <spirv-reflect/spirv_reflect.h>

#include "gtLinkVS.h"
#include "gtLinkPS.h"
#include "gtLinkAllPS.h"

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

using namespace gvk;

namespace
{
	std::vector<uint32_t> words(const void* i_pData, size_t i_Size)
	{
		const uint32_t* p = static_cast<const uint32_t*>(i_pData);
		return std::vector<uint32_t>(p, p + i_Size / sizeof(uint32_t));
	}

	//	A module's interface, as SPIRV-Reflect sees it
	struct interface_info
	{
		std::set<uint32_t>	m_Locations;	// non-built-in variables' first locations
		std::set<int>		m_BuiltIns;		// SpvBuiltIn values
		std::map<uint32_t, uint32_t> m_TypeFlags;	// by location
		std::vector<uint32_t> m_ArraySizes;	// non-built-in variables' first dimensions
		std::vector<SpvExecutionMode> m_Modes;
	};

	interface_info reflect(const std::vector<uint32_t>& i_Spirv, bool i_bInputs)
	{
		interface_info info;
		SpvReflectShaderModule module;
		EXPECT_EQ(SPV_REFLECT_RESULT_SUCCESS, spvReflectCreateShaderModule(i_Spirv.size() * 4, i_Spirv.data(), &module));
		uint32_t count = 0;
		std::vector<SpvReflectInterfaceVariable*> vars;
		if (i_bInputs)
		{
			spvReflectEnumerateInputVariables(&module, &count, NULL);
			vars.resize(count);
			spvReflectEnumerateInputVariables(&module, &count, vars.data());
		}
		else
		{
			spvReflectEnumerateOutputVariables(&module, &count, NULL);
			vars.resize(count);
			spvReflectEnumerateOutputVariables(&module, &count, vars.data());
		}
		for (const SpvReflectInterfaceVariable* var : vars)
		{
			if (var->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
			{
				info.m_BuiltIns.insert((int)var->built_in);
				continue;
			}
			info.m_Locations.insert(var->location);
			info.m_TypeFlags[var->location] = var->type_description ? var->type_description->type_flags : 0;
			if (var->array.dims_count)
			{
				info.m_ArraySizes.push_back(var->array.dims[0]);
			}
		}
		if (module.entry_point_count)
		{
			const SpvReflectEntryPoint& entry = module.entry_points[0];
			info.m_Modes.assign(entry.execution_modes, entry.execution_modes + entry.execution_mode_count);
		}
		spvReflectDestroyShaderModule(&module);
		return info;
	}

	bool has_mode(const interface_info& i_Info, SpvExecutionMode i_Mode)
	{
		for (SpvExecutionMode mode : i_Info.m_Modes)
		{
			if (mode == i_Mode)
			{
				return true;
			}
		}
		return false;
	}
}

TEST(vkdLinkStagesTest, TrimsOnlyUnconsumed)
{
	const std::vector<uint32_t> vs = words(g_gtLinkVS, sizeof(g_gtLinkVS));
	const std::vector<uint32_t> ps = words(g_gtLinkPS, sizeof(g_gtLinkPS));
	const interface_info before = reflect(vs, false);
	ASSERT_EQ((std::set<uint32_t>{ 0, 1, 2, 3 }), before.m_Locations);

	const std::vector<uint32_t> trimmed = vkdTrimUnconsumedOutputs(vs, ps);
	ASSERT_NE(vs, trimmed);
	const interface_info after = reflect(trimmed, false);
	EXPECT_EQ((std::set<uint32_t>{ 1, 3 }), after.m_Locations);
	//	built-ins stay, the clip distance included (the rasterizer reads it)
	EXPECT_EQ(before.m_BuiltIns, after.m_BuiltIns);
	EXPECT_EQ(1u, after.m_BuiltIns.count(SpvBuiltInPosition));
	EXPECT_EQ(1u, after.m_BuiltIns.count(SpvBuiltInClipDistance));

	//	nothing unconsumed: the producer as it is
	EXPECT_EQ(vs, vkdTrimUnconsumedOutputs(vs, words(g_gtLinkAllPS, sizeof(g_gtLinkAllPS))));
	//	not SPIR-V: as it is
	EXPECT_EQ(vs, vkdTrimUnconsumedOutputs(vs, std::vector<uint32_t>{ 1, 2, 3 }));
}

namespace
{
	class vkdLinkStagesDrawTest : public gtVulkanContext
	{
		protected:
			void TearDown() override
			{
				m_pVS.reset();
				m_pPS.reset();
				gtVulkanContext::TearDown();
			}

			std::unique_ptr<vkdShader> m_pVS, m_pPS;
	};
}

//	The trimmed vertex shader in a real draw: the pixel shader's inputs
//	arrive (locations 1 and 3 add up to 64, 128, 64), and the clip
//	distance still clips the right half away
TEST_F(vkdLinkStagesDrawTest, TrimmedDrawIsRight)
{
	if (!m_pDevice)
		return;
	std::string error;
	m_pVS = vkdShader::Create(&Device(), VK_SHADER_STAGE_VERTEX_BIT, g_gtLinkVS, sizeof(g_gtLinkVS), "VSMain", "gtLink VS", error);
	ASSERT_TRUE(m_pVS != nullptr) << error;
	m_pPS = vkdShader::Create(&Device(), VK_SHADER_STAGE_FRAGMENT_BIT, g_gtLinkPS, sizeof(g_gtLinkPS), "PSMain", "gtLink PS", error);
	ASSERT_TRUE(m_pPS != nullptr) << error;

	const uint32_t c_Size = 16;
	vkdImage* target = Target(c_Size, c_Size);
	vkdContext& c = Context();
	c.SetRenderTargets(1, &target, nullptr);
	vkdViewport viewport;
	viewport.m_Width = (float)c_Size;
	viewport.m_Height = (float)c_Size;
	c.SetViewports(1, &viewport);
	const vkdShader* stages[5] = { m_pVS.get(), nullptr, nullptr, nullptr, m_pPS.get() };
	c.SetShaders(stages);
	c.SetVertexLayout(nullptr);
	c.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	vkdRasterizerState raster;
	raster.m_Cull = VK_CULL_MODE_NONE;
	c.SetRasterizerState(raster);
	const float clear[4] = { 0, 0, 0, 0 };
	c.ClearRenderTarget(target, clear);
	c.Draw(3, 0);
	const std::vector<uint8_t> rgba = Read(target);
	EXPECT_EQ(1u, m_pPipelines->Trimmed());

	int wrong = 0;
	for (uint32_t y = 0; y < c_Size; y++)
	{
		for (uint32_t x = 0; x < c_Size; x++)
		{
			const uint8_t* p = &rgba[(y * c_Size + x) * 4];
			const bool left = x < c_Size / 2;
			const int expected[4] = { left ? 64 : 0, left ? 128 : 0, left ? 64 : 0, left ? 255 : 0 };
			for (int i = 0; i < 4; i++)
				wrong += std::abs((int)p[i] - expected[i]) > 1;
		}
	}
	EXPECT_EQ(0, wrong) << "left " << (int)rgba[0] << " " << (int)rgba[1] << " " << (int)rgba[2] << " " << (int)rgba[3]
		<< ", right " << (int)rgba[(c_Size - 1) * 4] << " " << (int)rgba[(c_Size - 1) * 4 + 1] << " "
		<< (int)rgba[(c_Size - 1) * 4 + 2] << " " << (int)rgba[(c_Size - 1) * 4 + 3];
}

namespace
{
	//	The embedded effects' shaders, made once per (effect, entry, profile)
	class vkdLinkStagesEffectsTest : public gtVulkanContext
	{
		protected:
			void TearDown() override
			{
				m_Shaders.clear();
				m_Layouts.clear();
				gtVulkanContext::TearDown();
			}

			const vkdShader* Shader(const fxEmbeddedEffect& i_Effect, const fxStageDesc& i_Desc, VkShaderStageFlagBits i_Stage)
			{
				const std::string key = std::string(i_Effect.m_Name) + "|" + i_Desc.m_Entry + "|" + i_Desc.m_Profile;
				auto found = m_Shaders.find(key);
				if (found != m_Shaders.end())
					return found->second.get();
				for (int b = 0; b < i_Effect.m_NumBytecode; b++)
				{
					const fxEmbeddedBytecode& code = i_Effect.m_pBytecode[b];
					if (i_Desc.m_Entry == code.m_Entry && i_Desc.m_Profile == code.m_Profile)
					{
						std::string error;
						std::unique_ptr<vkdShader> shader = vkdShader::Create(&Device(), i_Stage, code.m_pData, code.m_Size,
							code.m_Entry, key.c_str(), error);
						EXPECT_TRUE(shader != nullptr) << key << ": " << error;
						return (m_Shaders[key] = std::move(shader)).get();
					}
				}
				ADD_FAILURE() << key << ": no bytecode";
				return nullptr;
			}

			//	The vertex shader's inputs, packed in one buffer
			const vkdVertexLayout* Layout(const vkdShader& i_VS)
			{
				std::vector<vkdVertexElement> elements;
				uint32_t offset = 0;
				for (const vkdShaderInput& input : i_VS.Inputs())
				{
					vkdVertexElement e;
					e.m_Semantic = input.m_Semantic;
					e.m_SemanticIndex = input.m_Index;
					e.m_Format = input.m_Format;
					e.m_Offset = offset;
					offset += vkdFormatInfoOf(input.m_Format).m_BlockBytes;
					elements.push_back(e);
				}
				m_Layouts.emplace_back(new vkdVertexLayout(elements, { offset ? offset : 4 }));
				return m_Layouts.back().get();
			}

			std::map<std::string, std::unique_ptr<vkdShader>> m_Shaders;
			std::vector<std::unique_ptr<vkdVertexLayout>> m_Layouts;
	};

	const VkShaderStageFlagBits c_Stages[5] =
	{
		VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
		VK_SHADER_STAGE_GEOMETRY_BIT, VK_SHADER_STAGE_FRAGMENT_BIT
	};
}

//	Every pass of every embedded effect whose pixel shader takes fewer
//	locations than its last pre-rasterization stage writes gets its
//	pipeline made, with WARNING-Shader-OutputNotConsumed counted: none
TEST_F(vkdLinkStagesEffectsTest, NoWarningAcrossEffects)
{
	if (!m_pDevice)
		return;
	int passes = 0, linked = 0;
	for (int e = 0; e < fxEmbeddedShadersVk::k_NumEffects; e++)
	{
		const fxEmbeddedEffect& effect = fxEmbeddedShadersVk::k_Effects[e];
		fxEffectDesc desc;
		std::string error;
		ASSERT_TRUE(fxEffectDesc::Parse(effect.m_Manifest, desc, error)) << effect.m_Name << ": " << error;
		for (const fxTechniqueDesc& technique : desc.m_Techniques)
		{
			for (const fxPassDesc& pass : technique.m_Passes)
			{
				const std::string name = std::string(effect.m_Name) + "/" + technique.m_Name + "/" + pass.m_Name;
				const vkdShader* stages[5] = {};
				for (int s = fx_VS; s <= fx_PS; s++)
				{
					if (pass.m_Stages[s].IsUsed())
						stages[s] = Shader(effect, pass.m_Stages[s], c_Stages[s]);
				}
				if (!stages[fx_VS] || !stages[fx_PS])
					continue;
				passes++;
				int producer = fx_GS;
				while (!stages[producer])
					producer--;
				const interface_info written = reflect(stages[producer]->Spirv(), false);
				const interface_info taken = reflect(stages[fx_PS]->Spirv(), true);
				bool unconsumed = false;
				for (uint32_t location : written.m_Locations)
					unconsumed = unconsumed || taken.m_Locations.count(location) == 0;
				if (!unconsumed)
					continue;
				linked++;

				vkdPipelineKey key;
				for (int s = fx_VS; s <= fx_PS; s++)
					key.m_Shaders[s] = stages[s] ? stages[s]->Id() : 0;
				const vkdVertexLayout* layout = Layout(*stages[fx_VS]);
				key.m_VertexLayout = layout->Id();
				if (stages[fx_HS])
				{
					const interface_info hs = reflect(stages[fx_HS]->Spirv(), true);
					key.m_TopologyClass = vkd_Patches;
					key.m_PatchControlPoints = (uint8_t)(hs.m_ArraySizes.empty() ? 1 : hs.m_ArraySizes[0]);
				}
				else if (stages[fx_GS])
				{
					const interface_info gs = reflect(stages[fx_GS]->Spirv(), true);
					key.m_TopologyClass = has_mode(gs, SpvExecutionModeInputPoints) ? vkd_Points
										: has_mode(gs, SpvExecutionModeInputLines) ? vkd_Lines : vkd_Triangles;
				}
				//	a target per output, of the output's numeric type
				const interface_info outputs = reflect(stages[fx_PS]->Spirv(), false);
				for (uint32_t location : outputs.m_Locations)
				{
					const uint32_t flags = outputs.m_TypeFlags.at(location);
					const VkFormat format = (flags & SPV_REFLECT_TYPE_FLAG_FLOAT) ? VK_FORMAT_R32G32B32A32_SFLOAT
						: VK_FORMAT_R32G32B32A32_UINT;
					key.m_ColorCount = std::max<uint8_t>(key.m_ColorCount, (uint8_t)(location + 1));
					key.m_Colors[location] = format;
				}
				for (uint32_t c = 0; c < key.m_ColorCount; c++)
				{
					if (key.m_Colors[c] == VK_FORMAT_UNDEFINED)
						key.m_Colors[c] = VK_FORMAT_R8G8B8A8_UNORM;
				}
				key.m_Depth = VK_FORMAT_D32_SFLOAT;

				const int warnings = m_Validation.Stats().m_Warnings, errors = m_Validation.Stats().m_Errors;
				EXPECT_NE(VkPipeline(VK_NULL_HANDLE), m_pPipelines->Get(key, stages, layout)) << name;
				EXPECT_EQ(warnings, m_Validation.Stats().m_Warnings) << name << ": validation warnings (see the log)";
				EXPECT_EQ(errors, m_Validation.Stats().m_Errors) << name << ": validation errors (see the log)";
			}
		}
	}
	std::cout << "  " << passes << " passes, " << linked << " with outputs the pixel shader doesn't take, "
			  << m_pPipelines->Trimmed() << " producers trimmed" << std::endl;
	EXPECT_GT(linked, 0);
	EXPECT_GT(m_pPipelines->Trimmed(), 0u);
}

namespace
{
	const int c_MapSize = 512, c_OpacitySize = 256;

	void casts_shadow(g3dSceneNode* io_pNode)
	{
		g3dFragment* fragment = const_cast<g3dFragment*>(io_pNode->GetFragment());
		fragment->SetCastsShadow(true);
		fragment->SetReceivesShadow(true);
	}

	//	shdwOpacityVkTest's scene: hair on a head (hull, domain and
	//	geometry shaders), a transparent sphere, a wall, and a projected
	//	light with a shadow map and 4 opacity maps (the opacity pass's
	//	geometry shader)
	void opacity_scene(gtSceneBuilder& io_Builder, const std::filesystem::path&)
	{
		const maMatrix4x4 place = gtScene::Place(0, -0.35f, 0, 0.9f);
		matMaterial* head = io_Builder.Keep(gtScene::ShaderMaterial("Blinn.fx", gtScene::e_Tinted));
		casts_shadow(io_Builder.Add(gtScene::Sphere(16, 32), head, place * gtScene::Place(0, 0, 0, 0.97f)));
		gtHairShape shape;
		shape.m_nStrands = 64;
		shape.m_nVerticesPerStrand = 8;
		shape.m_Length = 1.0f;
		shape.m_RootRadius = 0.02f;
		shape.m_TipRadius = 0.006f;
		casts_shadow(io_Builder.Hair(shape, place));
		matMaterial* glass = io_Builder.Keep(gtScene::DataMaterial("Phong.fx", [](effShaderData* d)
		{
			gtScene::SetPhong(d, gtScene::e_NonDefault);		// transparency 0.7
		}));
		casts_shadow(io_Builder.Add(gtScene::Sphere(12, 24), glass, gtScene::Place(0.7f, 0.9f, 2.2f, 0.35f)));
		matMaterial* wall = io_Builder.Keep(gtScene::ShaderMaterial("Lambert.fx", gtScene::e_Defaults));
		casts_shadow(io_Builder.Add(gtScene::Plane(), wall, gtScene::Place(0, 0, -1.5f, 4.0f)));
		g3dProjectedLight* light = io_Builder.ProjectedLight(maPoint3d(1.5f, 2.5f, 7.0f), maPoint3d(0, 0, -1.5f),
															 maFloatRGBA(1.0f, 0.95f, 0.85f, 1), c_MapSize);
		io_Builder.HairShadows(light, HAIR_SHADOW_OSM4, c_OpacitySize);
	}

	struct trim_case
	{
		const char*		m_Name;
		gtRenderOptions	m_Options;
	};

	std::vector<trim_case> trim_cases()
	{
		std::vector<trim_case> cases;
		gtRenderOptions hdr;
		hdr.m_CreateRenderer = []() { return g3dSceneRendererCreate::CreateHDRRenderer(); };
		hdr.m_Prefs = [](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableHair = true;
			p.m_bEnableShadows = true;
			p.m_bEnableTransparent = true;
			p.m_HairTransparencyMode = 1;
			p.m_HairShadowType = HAIR_SHADOW_OSM4;
		};
		hdr.m_BeforeFrame = [](gtSceneBuilder& io_Builder) { io_Builder.RenderTargets(); };
		cases.push_back({ "hdr_hair_shadows", hdr });
		gtRenderOptions depth = hdr;
		depth.m_CreateRenderer = []() { return g3dSceneRendererCreate::CreateDepthRenderer(); };
		cases.push_back({ "depth", depth });
		gtRenderOptions transparent = hdr;
		transparent.m_Prefs = [](g3dPrefs::g3dRenderPrefs& p)
		{
			p.m_bEnableHair = true;
			p.m_bEnableShadows = true;
			p.m_bEnableTransparent = true;
			p.m_HairTransparencyMode = 2;		// depth peeled
			p.m_bHairDepthPeel = true;
			p.m_HairShadowType = HAIR_SHADOW_OSM4;
		};
		cases.push_back({ "hair_depth_peeled", transparent });
		return cases;
	}
}

//	Scenes whose passes draw with trimmed producers (hair, opacity maps,
//	the depth preview, depth-peeled transparency): the same bytes with
//	trimming as without (gtBackend::RunVulkanUntrimmed)
TEST(vkdLinkStagesTest, TrimmedImagesMatchUntrimmed)
{
	const std::vector<trim_case> cases = trim_cases();
	gtTempDir dir;
	std::vector<gtPicture> untrimmed(cases.size()), trimmed(cases.size());
	std::vector<uint64_t> counts(cases.size());
	if (!gtBackend::RunVulkanUntrimmed([&]()
		{
			for (size_t i = 0; i < cases.size(); i++)
				untrimmed[i] = gtRenderScene(opacity_scene, dir.GetPath(), cases[i].m_Options);
			EXPECT_EQ(0u, vkdGlobal::Pipelines().Trimmed());
		}))
		return;
	gtBackend::Run(gtBackend::e_Vulkan, [&]()
	{
		for (size_t i = 0; i < cases.size(); i++)
		{
			const uint64_t before = vkdGlobal::Pipelines().Trimmed();
			trimmed[i] = gtRenderScene(opacity_scene, dir.GetPath(), cases[i].m_Options);
			counts[i] = vkdGlobal::Pipelines().Trimmed() - before;
		}
	});
	for (size_t i = 0; i < cases.size(); i++)
	{
		SCOPED_TRACE(cases[i].m_Name);
		ASSERT_TRUE(untrimmed[i].Valid() && trimmed[i].Valid());
		std::cout << "  " << cases[i].m_Name << ": " << counts[i] << " producers trimmed" << std::endl;
		EXPECT_GT(counts[i], 0u) << "the case draws nothing with a trimmed producer";
		int differing = 0;
		for (size_t b = 0; b < untrimmed[i].m_Rgba.size(); b++)
			differing += untrimmed[i].m_Rgba[b] != trimmed[i].m_Rgba[b];
		EXPECT_EQ(0, differing) << "bytes differ; images (untrimmed as dx11, trimmed as vulkan) in "
			<< gtSaveComparison(std::string("TrimmedImagesMatchUntrimmed/") + cases[i].m_Name, untrimmed[i].m_Rgba.data(),
								trimmed[i].m_Rgba.data(), untrimmed[i].m_Width, untrimmed[i].m_Height);
	}
}
