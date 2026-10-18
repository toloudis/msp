/*****************************************************************************
**  fxParityTest.cpp
**
**      Every embedded effect pass that can draw as a full-screen pass, drawn
**      by GraphicsDX11 (fxc bytecode, fxEffectDX11, g2dFullscreenQuad) and
**      by GraphicsVulkan (DXC SPIR-V, fxEffectVk, vkdContext) with the same
**      inputs, must give the same image within the parity tolerance (the
**      GraphicsVulkan design, "Proving parity").
**
**      A pass qualifies when its only stages are a vertex and a pixel
**      shader, its vertex shader reads at most the quad's SV_POSITION and
**      TEXCOORD0 - in the quad's registers in the fxc bytecode too (v0, v1),
**      since D3D11 feeds them by register through the input layout made
**      for the quad's own vertex shader - its pixel shader writes one
**      target, and it reads only 2D textures (no UAVs or buffers). Inputs: one gradient texture bound to
**      every texture, a non-symmetric matrix for every matrix constant,
**      every other float constant at its default (0.5 where that is 0),
**      ints and bools at their defaults. No culling, no blending, no depth.
**
**	Extra Large Technology
\****************************************************************************/
#include "Vk/gtVulkanContext.hpp"
#include "Dx11/gtD3D11.hpp"
#include "gtImage.hpp"

#ifndef GVK_FXEMBEDDEDSHADERSVK_HPP
#include "GraphicsVulkan/Fx/fxEmbeddedShadersVk.hpp"
#endif
#ifndef GVK_G2DFULLSCREENQUAD_HPP
#include "GraphicsVulkan/G2d/g2dFullscreenQuad.hpp"
#endif
#ifndef FX_EMBEDDEDSHADERS_HPP
#include "GraphicsDX11/Fx/fxEmbeddedShaders.hpp"
#endif
#include "GraphicsDX11/G2d/g2dFullscreenQuad.hpp"

#include <d3dcompiler.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace
{
	const int c_Size = 64;
	const float c_Clear[4] = { 0.1f, 0.2f, 0.3f, 1.0f };
	const float c_Matrix[16] = { 1.0f, 0.1f, 0.0f, 0.0f,  0.2f, 0.9f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.05f, -0.05f, 0.0f, 1.0f };

	//	Accepted differences, "<effect>/<technique>/<pass>" -> reason (with the
	//	measured error). Empty: every compared pass is within the tolerance.
	const char* const c_Exceptions[][2] =
	{
		{ nullptr, nullptr },
	};

	const char* exception(const std::string& i_Pass)
	{
		for (const auto& e : c_Exceptions)
		{
			if (e[0] && i_Pass == e[0])
			{
				return e[1];
			}
		}
		return nullptr;
	}

	std::vector<std::string> effect_names()
	{
		std::vector<std::string> names;
		for (int i = 0; i < gvk::fxEmbeddedShadersVk::k_NumEffects; i++)
		{
			names.push_back(gvk::fxEmbeddedShadersVk::k_Effects[i].m_Name);
		}
		return names;
	}

	std::vector<uint8_t> gradient(int i_Size)
	{
		std::vector<uint8_t> rgba(i_Size * i_Size * 4);
		for (int y = 0; y < i_Size; y++)
		{
			for (int x = 0; x < i_Size; x++)
			{
				uint8_t* p = &rgba[(y * i_Size + x) * 4];
				p[0] = (uint8_t)(x * 255 / (i_Size - 1));
				p[1] = (uint8_t)(y * 255 / (i_Size - 1));
				p[2] = (uint8_t)((x + y) * 255 / (2 * i_Size - 2));
				p[3] = (uint8_t)(255 - x * 2);
			}
		}
		return rgba;
	}

	//	Why the pass can't be drawn as a full-screen pass; empty if it can
	std::string unqualified(const gvk::fxEffectVk& i_Effect, int i_Technique, int i_Pass)
	{
		if (i_Effect.IsPassDeferred(i_Technique, i_Pass))
		{
			return "deferred (hair hull shader)";
		}
		for (gvk::fxStage s : { gvk::fx_HS, gvk::fx_DS, gvk::fx_GS })
		{
			if (i_Effect.GetStageReflection(i_Technique, i_Pass, s))
			{
				return "has a hull, domain or geometry shader";
			}
		}
		const gvk::vkdShaderReflection* vs = i_Effect.GetStageReflection(i_Technique, i_Pass, gvk::fx_VS);
		const gvk::vkdShaderReflection* ps = i_Effect.GetStageReflection(i_Technique, i_Pass, gvk::fx_PS);
		if (!vs || !ps)
		{
			return "no vertex or pixel shader";
		}
		for (const gvk::vkdShaderInput& input : vs->m_Inputs)
		{
			const bool position = (input.m_Semantic == "SV_POSITION" && input.m_Index == 0);
			const bool texcoord = (input.m_Semantic == "TEXCOORD" && input.m_Index == 0);
			if (!position && !texcoord)
			{
				return "vertex input " + input.m_Semantic + std::to_string(input.m_Index);
			}
		}
		if (ps->m_ColorOutputs > 1)
		{
			return "writes " + std::to_string(ps->m_ColorOutputs) + " targets";
		}
		for (const gvk::vkdShaderReflection* stage : { vs, ps })
		{
			for (const gvk::vkdShaderResource& r : stage->m_Resources)
			{
				if (r.m_Kind == gvk::vkdShaderResource::e_Uav)
				{
					return "writes a UAV";
				}
				if (r.m_Kind == gvk::vkdShaderResource::e_Texture &&
					(r.m_bBuffer || r.m_Dim != 1 /* SpvDim2D */ || r.m_bArrayed || r.m_bMultisampled))
				{
					return "reads " + r.m_Name + ", not a 2D texture";
				}
			}
		}
		return std::string();
	}

	//	Why D3D11 can't feed the fxc vertex shader from the quad's input
	//	layout (SV_POSITION in v0, TEXCOORD0 in v1); empty if it can
	std::string dx11_inputs(const fxEmbeddedEffect& i_Effect, const std::string& i_Entry, const std::string& i_Profile)
	{
		for (int i = 0; i < i_Effect.m_NumBytecode; i++)
		{
			const fxEmbeddedBytecode& b = i_Effect.m_pBytecode[i];
			if (i_Entry != b.m_Entry || i_Profile != b.m_Profile)
			{
				continue;
			}
			Microsoft::WRL::ComPtr<ID3D11ShaderReflection> refl;
			if (FAILED(D3DReflect(b.m_pData, b.m_Size, __uuidof(ID3D11ShaderReflection), (void**)refl.GetAddressOf())))
			{
				return "D3DReflect failed";
			}
			D3D11_SHADER_DESC desc;
			refl->GetDesc(&desc);
			for (UINT p = 0; p < desc.InputParameters; p++)
			{
				D3D11_SIGNATURE_PARAMETER_DESC param;
				refl->GetInputParameterDesc(p, &param);
				const std::string name = param.SemanticName;
				const bool position = (_stricmp(param.SemanticName, "SV_POSITION") == 0 && param.SemanticIndex == 0 && param.Register == 0);
				const bool texcoord = (_stricmp(param.SemanticName, "TEXCOORD") == 0 && param.SemanticIndex == 0 && param.Register == 1);
				if (!position && !texcoord && param.SystemValueType == D3D_NAME_UNDEFINED)
				{
					return "fxc vertex input " + name + std::to_string(param.SemanticIndex) + " in v" + std::to_string(param.Register);
				}
				if (!position && !texcoord && param.SystemValueType != D3D_NAME_UNDEFINED && name != "SV_VertexID" && name != "SV_InstanceID")
				{
					return "fxc vertex input " + name + " in v" + std::to_string(param.Register);
				}
			}
			return std::string();
		}
		return "no fxc bytecode for " + i_Entry;
	}

	//	The same inputs, on either effect class (fxEffectDX11 or fxEffectVk)
	template <class Effect>
	void set_inputs(Effect& io_Effect, const std::vector<std::string>& i_Constants)
	{
		for (const std::string& name : i_Constants)
		{
			const auto* type = io_Effect.FindConstantType(name);
			const auto constant = io_Effect.FindConstant(name);
			if (!type || type->m_Elements > 0)
			{
				continue;
			}
			if ((type->m_Class == gvk::fx_SVC_MATRIX_ROWS || type->m_Class == gvk::fx_SVC_MATRIX_COLUMNS) &&
				type->m_Rows == 4 && type->m_Columns == 4)
			{
				io_Effect.SetMatrix(constant, c_Matrix);
			}
			else if ((type->m_Class == gvk::fx_SVC_SCALAR || type->m_Class == gvk::fx_SVC_VECTOR) && type->m_Type == gvk::fx_SVT_FLOAT)
			{
				float values[4] = {};
				io_Effect.GetConstant(constant, values, type->m_Columns * 4);
				for (uint32_t c = 0; c < type->m_Columns && c < 4; c++)
				{
					values[c] = values[c] == 0.0f ? 0.5f : values[c];
				}
				io_Effect.SetConstant(constant, values, type->m_Columns * 4);
			}
		}
	}

	class fxParity : public gtVulkanContext, public ::testing::WithParamInterface<std::string>
	{
		protected:
			void SetUp() override
			{
				gtVulkanContext::SetUp();
				if (m_pDevice)
				{
					gvk::g2dFullscreenQuad::InitFullscreenQuad();
				}
			}

			void TearDown() override
			{
				if (m_pDevice)
				{
					gvk::g2dFullscreenQuad::CleanUpFullscreenQuad();
				}
				gtVulkanContext::TearDown();
			}
	};
}

TEST_P(fxParity, FullScreenPassesMatchDX11)
{
	GT_D3D11(d3d);
	gtD3D11::Globals globals(d3d);
	g2dFullscreenQuad::InitFullscreenQuad();
	std::string error;

	const gvk::fxEmbeddedEffect* vkEmbedded = gvk::fxEmbeddedShadersVk::Find(GetParam());
	const fxEmbeddedEffect* dxEmbedded = fxEmbeddedShaders::Find(GetParam());
	std::unique_ptr<gvk::fxEffectVk> vk = gvk::fxEmbeddedShadersVk::CreateVk(*vkEmbedded, &Device(), error);
	ASSERT_TRUE(vk != nullptr) << error;
	std::unique_ptr<fxEffectDX11> dx = fxEmbeddedShaders::CreateDX11(*dxEmbedded, d3d.Device(), error);
	ASSERT_TRUE(dx != nullptr) << error;
	set_inputs(*vk, vk->GetConstantNames());
	set_inputs(*dx, dx->GetConstantNames());

	const std::vector<uint8_t> texels = gradient(c_Size);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dxTexture;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> dxTextureObject = d3d.CreateTexture(c_Size, c_Size, texels.data(), false, &dxTexture, NULL);
	gvk::vkdImage* vkTexture = Texture(c_Size, c_Size, texels.data());
	for (const std::string& name : vk->GetResourceNames())
	{
		vk->SetResource(vk->FindResource(name), vkTexture);
		dx->SetResource(dx->FindResource(name), dxTexture.Get());
	}

	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthClipEnable = TRUE;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> noCull;
	d3d.Device()->CreateRasterizerState(&rasterDesc, &noCull);
	gvk::vkdRasterizerState vkNoCull;
	vkNoCull.m_Cull = VK_CULL_MODE_NONE;

	gvk::vkdImage* vkTarget = Target(c_Size, c_Size);
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> dxView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> dxTarget = d3d.CreateTexture(c_Size, c_Size, NULL, true, NULL, &dxView);

	int compared = 0, drawn = 0;
	for (int t = 0; t < vk->GetTechniqueCount(); t++)
	{
		for (int p = 0; p < vk->GetPassCount(t); p++)
		{
			const std::string pass = GetParam() + "/" + vk->GetDesc().m_Techniques[t].m_Name + "/" +
									 vk->GetDesc().m_Techniques[t].m_Passes[p].m_Name;
			std::string why = unqualified(*vk, t, p);
			if (why.empty())
			{
				const fxStageDesc& vs = dx->GetDesc().m_Techniques[t].m_Passes[p].m_Stages[fx_VS];
				why = dx11_inputs(*dxEmbedded, vs.m_Entry, vs.m_Profile);
			}
			if (!why.empty())
			{
				std::cout << "  not compared: " << pass << " (" << why << ")" << std::endl;
				continue;
			}

			// DX11
			ID3D11DeviceContext* c = d3d.Context();
			c->ClearState();
			c->ClearRenderTargetView(dxView.Get(), c_Clear);
			c->OMSetRenderTargets(1, dxView.GetAddressOf(), NULL);
			c->RSSetState(noCull.Get());
			dx->Apply(t, p, c);
			g2dFullscreenQuad::DrawFullScreenQuad11(c_Size, c_Size);
			const std::vector<uint8_t> dxImage = d3d.ReadBack(dxTarget.Get());

			// Vulkan
			gvk::vkdContext& context = Context();
			context.ResetState();
			context.SetRenderTargets(1, &vkTarget, nullptr);
			context.ClearRenderTarget(vkTarget, c_Clear);
			context.SetRasterizerState(vkNoCull);
			vk->Apply(t, p);
			gvk::g2dFullscreenQuad::DrawFullScreenQuad(c_Size, c_Size);
			const std::vector<uint8_t> vkImage = Read(vkTarget);

			compared++;
			const uint8_t clear[4] = { 26, 51, 77, 255 };
			for (size_t i = 0; i < dxImage.size(); i += 4)
			{
				if (memcmp(&dxImage[i], clear, 4) != 0)
				{
					drawn++;
					break;
				}
			}
			const gtCompareResult r = gtCompareImages(dxImage.data(), vkImage.data(), c_Size, c_Size);
			std::cout << "  compared: " << pass << ": " << r.Describe() << std::endl;
			if (r.m_bPass)
			{
				continue;
			}
			const std::string dir = gtSaveComparison("fxParity/" + pass, dxImage.data(), vkImage.data(), c_Size, c_Size);
			if (const char* reason = exception(pass))
			{
				std::cout << "  accepted: " << pass << ": " << r.Describe() << " (" << reason << ")" << std::endl;
				continue;
			}
			ADD_FAILURE() << pass << ": " << r.Describe() << "; images in " << dir;
		}
	}
	std::cout << "  " << GetParam() << ": " << compared << " pass(es) compared, " << drawn << " drew something" << std::endl;
	g2dFullscreenQuad::CleanUpFullscreenQuad();
}

INSTANTIATE_TEST_SUITE_P(Embedded, fxParity, ::testing::ValuesIn(effect_names()),
						 [](const ::testing::TestParamInfo<std::string>& i_Info) { return i_Info.param; });
