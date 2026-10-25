/*****************************************************************************
**  fxEffectVkReflectionTest.cpp
**
**      For every embedded effect, fxEffectVk (SPIR-V from DXC, reflected
**      with SPIRV-Reflect) must describe exactly what fxEffectDX11 (fxc
**      bytecode, D3DReflect) does: techniques, passes, constants with their
**      buffers, offsets, sizes, types and defaults, resources, and the
**      register each stage reads them from. Material code finds parameters
**      by these names and writes them at these offsets, so any difference
**      would be a silent rendering bug.
**
**      Needs no Vulkan device; DX11 runs on the hardware adapter or WARP.
**
**	Extra Large Technology
\****************************************************************************/
#include "Vk/gtVulkanOnly.hpp"
#include "Dx11/gtD3D11.hpp"

#ifndef GVK_FXEMBEDDEDSHADERSVK_HPP
#include "GraphicsVulkan/Fx/fxEmbeddedShadersVk.hpp"
#endif
#ifndef GVK_VKDADAPTER_HPP
#include "GraphicsVulkan/vkd/vkdAdapter.hpp"
#endif
#ifndef FX_EMBEDDEDSHADERS_HPP
#include "GraphicsDX11/Fx/fxEmbeddedShaders.hpp"
#endif

#include <d3dcompiler.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace
{
	//	Differences that are understood and accepted, "<effect>: <what>" -> reason.
	//	Empty: every embedded effect reflects identically.
	const char* const c_Exceptions[][2] =
	{
		{ nullptr, nullptr },
	};

	bool is_exception(const std::string& i_What)
	{
		for (const auto& e : c_Exceptions)
		{
			if (e[0] && i_What == e[0])
			{
				return true;
			}
		}
		return false;
	}

	std::vector<std::string> effect_names()
	{
		std::vector<std::string> names;
		// DX11 has no Vulkan-only effect to compare with
		for (int i = 0; i < gvk::fxEmbeddedShadersVk::k_NumEffects; i++)
		{
			if (!gtVulkanOnly::IsVulkanOnlyEffect(gvk::fxEmbeddedShadersVk::k_Effects[i].m_Name))
				names.push_back(gvk::fxEmbeddedShadersVk::k_Effects[i].m_Name);
		}
		return names;
	}

	std::string describe(const std::vector<std::string>& i_Names)
	{
		std::ostringstream s;
		for (const std::string& n : i_Names)
		{
			s << n << " ";
		}
		return s.str();
	}

	//	(kind, name, slot) as text, sorted, for comparing stage bindings
	typedef std::set<std::string> BindingSet;

	std::string binding_text(char i_Kind, const std::string& i_Name, unsigned i_Slot)
	{
		return std::string(1, i_Kind) + std::to_string(i_Slot) + " " + i_Name;
	}

	BindingSet d3d_bindings(const void* i_pCode, size_t i_Size)
	{
		BindingSet set;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> refl;
		if (FAILED(D3DReflect(i_pCode, i_Size, __uuidof(ID3D11ShaderReflection), (void**)refl.GetAddressOf())))
		{
			set.insert("D3DReflect failed");
			return set;
		}
		D3D11_SHADER_DESC desc;
		refl->GetDesc(&desc);
		for (UINT r = 0; r < desc.BoundResources; r++)
		{
			D3D11_SHADER_INPUT_BIND_DESC bind;
			refl->GetResourceBindingDesc(r, &bind);
			char kind = '?';
			switch (bind.Type)
			{
				case D3D_SIT_CBUFFER:
					kind = 'b';
					break;
				case D3D_SIT_TEXTURE:
				case D3D_SIT_STRUCTURED:
				case D3D_SIT_BYTEADDRESS:
				case D3D_SIT_TBUFFER:
					kind = 't';
					break;
				case D3D_SIT_SAMPLER:
					kind = 's';
					break;
				default:
					kind = 'u';
					break;
			}
			set.insert(binding_text(kind, bind.Name, bind.BindPoint));
		}
		return set;
	}

	BindingSet vk_bindings(const std::vector<gvk::fxEffectVk::StageBinding>& i_Bindings)
	{
		static const char c_Kinds[] = { 'b', 't', 's', 'u' };
		BindingSet set;
		for (const gvk::fxEffectVk::StageBinding& b : i_Bindings)
		{
			set.insert(binding_text(c_Kinds[b.m_Kind], b.m_Name, b.m_Slot));
		}
		return set;
	}

	std::string set_text(const BindingSet& i_Set)
	{
		std::string s;
		for (const std::string& e : i_Set)
		{
			s += "[" + e + "] ";
		}
		return s;
	}

	//	Recursive type comparison; o_Diff names the first difference.
	bool same_type(const fxEffectDX11::Type& i_D3D, const gvk::fxEffectVk::Type& i_Vk, const std::string& i_Path, std::string& o_Diff)
	{
		std::ostringstream s;
		if (i_D3D.m_Class != i_Vk.m_Class || i_D3D.m_Type != i_Vk.m_Type || i_D3D.m_Rows != i_Vk.m_Rows ||
			i_D3D.m_Columns != i_Vk.m_Columns || i_D3D.m_Elements != i_Vk.m_Elements ||
			i_D3D.m_Members.size() != i_Vk.m_Members.size())
		{
			s << i_Path << ": D3D " << i_D3D.m_Name << " class " << i_D3D.m_Class << " type " << i_D3D.m_Type << " "
			  << i_D3D.m_Rows << "x" << i_D3D.m_Columns << " [" << i_D3D.m_Elements << "] members " << i_D3D.m_Members.size()
			  << "; Vulkan " << i_Vk.m_Name << " class " << i_Vk.m_Class << " type " << i_Vk.m_Type << " "
			  << i_Vk.m_Rows << "x" << i_Vk.m_Columns << " [" << i_Vk.m_Elements << "] members " << i_Vk.m_Members.size();
			o_Diff = s.str();
			return false;
		}
		for (size_t m = 0; m < i_D3D.m_Members.size(); m++)
		{
			const auto& a = i_D3D.m_Members[m];
			const auto& b = i_Vk.m_Members[m];
			const std::string path = i_Path + "." + a.m_Name;
			if (a.m_Name != b.m_Name || a.m_Offset != b.m_Offset)
			{
				s << path << ": D3D member " << a.m_Name << " at " << a.m_Offset << "; Vulkan " << b.m_Name << " at " << b.m_Offset;
				o_Diff = s.str();
				return false;
			}
			if (!same_type(a.m_Type, b.m_Type, path, o_Diff))
			{
				return false;
			}
		}
		return true;
	}

	//	Both versions of one embedded effect. The tests only read them, so
	//	each effect is made once for the suite, on one D3D11 device.
	class fxEffectVkReflection : public ::testing::TestWithParam<std::string>
	{
		public:
			static void SetUpTestSuite()
			{
				s_pD3D11.reset(new gtD3D11());
				if (!s_pD3D11->Create(s_D3D11Error))
				{
					s_pD3D11.reset();
				}
			}

			static void TearDownTestSuite()
			{
				s_Effects.clear();
				s_pD3D11.reset();
			}

		protected:
			void SetUp() override
			{
				const std::string name = GetParam();
				m_pVkEmbedded = gvk::fxEmbeddedShadersVk::Find(name);
				m_pDX11Embedded = fxEmbeddedShaders::Find(name);
				ASSERT_TRUE(m_pVkEmbedded != nullptr);
				ASSERT_TRUE(m_pDX11Embedded != nullptr) << name << " is not in GraphicsDX11";

				Effects& effects = s_Effects[name];
				std::string error;
				if (!effects.m_pVk)
				{
					effects.m_pVk = gvk::fxEmbeddedShadersVk::CreateVk(*m_pVkEmbedded, nullptr, error);
					ASSERT_TRUE(effects.m_pVk != nullptr) << error;
				}
				m_pVk = effects.m_pVk.get();

				if (!s_pD3D11)
				{
					GTEST_SKIP() << "No Direct3D 11 device: " << s_D3D11Error;
				}
				if (!effects.m_pDX11)
				{
					effects.m_pDX11 = fxEmbeddedShaders::CreateDX11(*m_pDX11Embedded, s_pD3D11->Device(), error);
					ASSERT_TRUE(effects.m_pDX11 != nullptr) << error;
				}
				m_pDX11 = effects.m_pDX11.get();
			}

			//	The fxc bytecode of one DX11 stage
			bool dx11_bytecode(const std::string& i_Entry, const std::string& i_Profile, const void*& o_pCode, size_t& o_Size) const
			{
				for (int i = 0; i < m_pDX11Embedded->m_NumBytecode; i++)
				{
					const fxEmbeddedBytecode& b = m_pDX11Embedded->m_pBytecode[i];
					if (i_Entry == b.m_Entry && i_Profile == b.m_Profile)
					{
						o_pCode = b.m_pData;
						o_Size = b.m_Size;
						return true;
					}
				}
				return false;
			}

			struct Effects
			{
				std::unique_ptr<gvk::fxEffectVk>	m_pVk;
				std::unique_ptr<fxEffectDX11>		m_pDX11;
			};
			static std::unique_ptr<gtD3D11>			s_pD3D11;
			static std::string						s_D3D11Error;
			static std::map<std::string, Effects>	s_Effects;

			const gvk::fxEmbeddedEffect*		m_pVkEmbedded = nullptr;
			const fxEmbeddedEffect*				m_pDX11Embedded = nullptr;
			gvk::fxEffectVk*					m_pVk = nullptr;
			fxEffectDX11*						m_pDX11 = nullptr;
	};

	std::unique_ptr<gtD3D11> fxEffectVkReflection::s_pD3D11;
	std::string fxEffectVkReflection::s_D3D11Error;
	std::map<std::string, fxEffectVkReflection::Effects> fxEffectVkReflection::s_Effects;
}

TEST_P(fxEffectVkReflection, SameTechniquesAndPasses)
{
	ASSERT_EQ(m_pDX11->GetTechniqueCount(), m_pVk->GetTechniqueCount());
	for (int t = 0; t < m_pDX11->GetTechniqueCount(); t++)
	{
		EXPECT_EQ(m_pDX11->GetDesc().m_Techniques[t].m_Name, m_pVk->GetDesc().m_Techniques[t].m_Name);
		EXPECT_EQ(m_pDX11->GetPassCount(t), m_pVk->GetPassCount(t)) << "technique " << t;
	}
}

TEST_P(fxEffectVkReflection, SameConstants)
{
	const std::vector<std::string> d3d = m_pDX11->GetConstantNames();
	const std::vector<std::string> vk = m_pVk->GetConstantNames();
	if (!is_exception(GetParam() + ": constant names"))
	{
		EXPECT_EQ(describe(d3d), describe(vk));
	}

	for (const std::string& name : d3d)
	{
		const fxEffectDX11::Constant a = m_pDX11->FindConstant(name);
		const gvk::fxEffectVk::Constant b = m_pVk->FindConstant(name);
		if (!b.IsValid())
		{
			continue;	// reported by the name comparison
		}
		if (is_exception(GetParam() + ": " + name))
		{
			continue;
		}
		EXPECT_EQ(a.m_Offset, b.m_Offset) << name;
		EXPECT_EQ(a.m_Size, b.m_Size) << name;
		EXPECT_EQ(a.m_bColumnMajor, b.m_bColumnMajor) << name;
		EXPECT_EQ(a.m_Columns, b.m_Columns) << name;
		EXPECT_EQ(a.m_Elements, b.m_Elements) << name;
		// the same buffer, by name (buffer indices depend on reflection order)
		const std::string buffer = m_pVk->GetConstantBufferNames()[b.m_Buffer];
		EXPECT_EQ(m_pDX11->FindConstantBuffer(buffer), a.m_Buffer) << name << " is not in DX11's " << buffer;
		std::string diff;
		EXPECT_TRUE(same_type(*m_pDX11->FindConstantType(name), *m_pVk->FindConstantType(name), name, diff)) << diff;
	}
}

TEST_P(fxEffectVkReflection, SameConstantBuffers)
{
	//	DX11 has no list of buffer names: count its buffers, then find each
	//	Vulkan buffer in it by name
	const std::vector<std::string> names = m_pVk->GetConstantBufferNames();
	int dx11Count = 0;
	while (m_pDX11->GetConstantBufferSize(dx11Count) > 0)
	{
		dx11Count++;
	}
	EXPECT_EQ(dx11Count, (int)names.size()) << describe(names);

	for (int b = 0; b < (int)names.size(); b++)
	{
		const int a = m_pDX11->FindConstantBuffer(names[b]);
		if (is_exception(GetParam() + ": buffer " + names[b]))
		{
			continue;
		}
		ASSERT_GE(a, 0) << names[b] << " is not a DX11 constant buffer";
		const uint32_t size = m_pDX11->GetConstantBufferSize(a);
		EXPECT_EQ(size, m_pVk->GetConstantBufferSize(b)) << names[b];
		if (size == m_pVk->GetConstantBufferSize(b))
		{
			EXPECT_EQ(0, memcmp(m_pDX11->GetConstantBufferDefaults(a), m_pVk->GetConstantBufferDefaults(b), size))
				<< "defaults of " << names[b];
		}
	}
}

TEST_P(fxEffectVkReflection, SameResourcesAndUavs)
{
	EXPECT_EQ(describe(m_pDX11->GetResourceNames()), describe(m_pVk->GetResourceNames()));
	EXPECT_EQ(describe(m_pDX11->GetUnorderedAccessNames()), describe(m_pVk->GetUnorderedAccessNames()));
}

TEST_P(fxEffectVkReflection, SameStageBindings)
{
	static const char* const c_StageNames[] = { "vs", "hs", "ds", "gs", "ps" };
	const gvk::fxEffectDesc& desc = m_pVk->GetDesc();
	std::set<std::string> done;
	for (int t = 0; t < (int)desc.m_Techniques.size(); t++)
	{
		for (int p = 0; p < (int)desc.m_Techniques[t].m_Passes.size(); p++)
		{
			for (int s = 0; s < gvk::fx_NumGraphicsStages; s++)
			{
				const gvk::fxStageDesc& stage = desc.m_Techniques[t].m_Passes[p].m_Stages[s];
				if (!stage.IsUsed() || !done.insert(stage.m_Entry + "|" + stage.m_Profile).second)
				{
					continue;
				}
				ASSERT_TRUE(m_pVk->GetStageReflection(t, p, (gvk::fxStage)s)) << stage.m_Entry << " has no SPIR-V";
				const void* code = nullptr;
				size_t size = 0;
				ASSERT_TRUE(dx11_bytecode(stage.m_Entry, stage.m_Profile, code, size)) << stage.m_Entry;
				const BindingSet d3d = d3d_bindings(code, size);
				const BindingSet vk = vk_bindings(m_pVk->GetStageBindings(t, p, (gvk::fxStage)s));
				const std::string what = GetParam() + ": " + stage.m_Entry + " bindings";
				if (!is_exception(what))
				{
					EXPECT_EQ(set_text(d3d), set_text(vk)) << stage.m_Entry << " (" << c_StageNames[s] << ")";
				}
			}
		}
	}
}

TEST_P(fxEffectVkReflection, VertexInputsNamed)
{
	for (int t = 0; t < m_pVk->GetTechniqueCount(); t++)
	{
		for (int p = 0; p < m_pVk->GetPassCount(t); p++)
		{
			const gvk::vkdShaderReflection* vs = m_pVk->GetStageReflection(t, p, gvk::fx_VS);
			if (!vs)
			{
				continue;
			}
			for (const gvk::vkdShaderInput& input : vs->m_Inputs)
			{
				EXPECT_FALSE(input.m_Semantic.empty()) << "location " << input.m_Location;
				EXPECT_NE(VK_FORMAT_UNDEFINED, input.m_Format) << input.m_Semantic << input.m_Index;
			}
		}
	}
}

//	Every SPIR-V capability an embedded shader declares needs its device
//	feature enabled (or is core): a capability missing from this table is a
//	new shader requirement to add to vkdRequiredFeatures.
TEST(fxEffectVkCapabilities, AllHaveRequiredFeatures)
{
	static const struct { uint32_t m_Capability; const char* m_pFeature; } c_Capabilities[] =
	{
		{ 0, "" },		// Matrix
		{ 1, "" },		// Shader
		{ 2, "geometryShader" },
		{ 3, "tessellationShader" },
		{ 25, "shaderImageGatherExtended" },
		{ 32, "shaderClipDistance" },
		{ 33, "shaderCullDistance" },
		{ 34, "imageCubeArray" },
		{ 35, "sampleRateShading" },
		{ 43, "" },		// Sampled1D
		{ 44, "" },		// Image1D
		{ 46, "" },		// SampledBuffer
		{ 47, "" },		// ImageBuffer
		{ 50, "" },		// ImageQuery
		{ 51, "" },		// DerivativeControl
		{ 4427, "shaderDrawParameters" },				// DrawParameters (-fvk-support-nonzero-base-*)
		{ 5379, "shaderDemoteToHelperInvocation" },	// DemoteToHelperInvocation (discard, Vulkan 1.3)
	};
	const std::vector<std::string>& required = gvk::vkdRequiredFeatures();
	std::set<uint32_t> used;
	for (int e = 0; e < gvk::fxEmbeddedShadersVk::k_NumEffects; e++)
	{
		const gvk::fxEmbeddedEffect& effect = gvk::fxEmbeddedShadersVk::k_Effects[e];
		for (int b = 0; b < effect.m_NumBytecode; b++)
		{
			gvk::vkdShaderReflection reflection;
			std::string error;
			ASSERT_TRUE(gvk::vkdReflect(effect.m_pBytecode[b].m_pData, effect.m_pBytecode[b].m_Size, reflection, error)) << error;
			used.insert(reflection.m_Capabilities.begin(), reflection.m_Capabilities.end());
		}
	}
	for (uint32_t capability : used)
	{
		const char* feature = nullptr;
		for (const auto& c : c_Capabilities)
		{
			if (c.m_Capability == capability)
			{
				feature = c.m_pFeature;
			}
		}
		ASSERT_TRUE(feature != nullptr) << "SPIR-V capability " << capability << " is not in the table";
		if (*feature)
		{
			EXPECT_TRUE(std::find(required.begin(), required.end(), feature) != required.end())
				<< "capability " << capability << " needs " << feature;
		}
	}
}

INSTANTIATE_TEST_SUITE_P(Embedded, fxEffectVkReflection, ::testing::ValuesIn(effect_names()),
						 [](const ::testing::TestParamInfo<std::string>& i_Info) { return i_Info.param; });
