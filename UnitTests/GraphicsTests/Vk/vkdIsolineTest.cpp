/*****************************************************************************
**  vkdIsolineTest.cpp
**
**      Isoline tessellation on Vulkan against D3D11, for the hair's hull
**      shader: ProcessIsolineTessFactors (which DXC's SPIR-V backend lacks;
**      GraphicsVulkan's Eff/private/ShadersVk/IsolineTessFactors.vulkan-
**      only.hlsli gives it) with fractional_odd partitioning, and DXC's
**      mapping of SV_TessFactor[0]/[1] to TessLevelOuter[0]/[1] with
**      D3D11's isoline meaning (line density, then line detail). The same
**      HLSL (gtIsoline.hlsl) draws every tessellated point at a pixel of
**      its own on both APIs; the targets must be equal.
**
**	Extra Large Technology
\****************************************************************************/
#include "Vk/gtVulkan.hpp"
#include "Dx11/gtD3D11.hpp"
#include "gtPlatform.hpp"

#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDSHADER_HPP
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#endif

#include "gtIsolineVS.h"
#include "gtIsolineHS.h"
#include "gtIsolineDS.h"
#include "gtIsolinePS.h"

#include <d3dcompiler.h>

#include <cmath>
#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace gvk;

namespace
{
	//	gtIsoline.hlsl's target: 1024 columns for u, 64 rows of v for each of 2 patches
	const uint32_t c_Width = 1024, c_Height = 128, c_Patches = 2;

	struct IsolineConstants
	{
		float m_Raw[2];			// x: tessellation (density), y: interpolation count (detail)
		uint32_t m_CulledPatch;	// c_Patches: none
		uint32_t m_Pad;
	};

	struct Case
	{
		float m_Tessellation, m_Interpolation;
		uint32_t m_CulledPatch;
	};

	std::string describe(const Case& i_Case)
	{
		std::ostringstream text;
		text << "tessellation " << i_Case.m_Tessellation << ", interpolation " << i_Case.m_Interpolation;
		if (i_Case.m_CulledPatch < c_Patches)
			text << ", patch " << i_Case.m_CulledPatch << " culled";
		return text.str();
	}

	//	gtIsoline.hlsl compiled with fxc; draws each case into its own
	//	cleared RGBA32F target and reads it back
	class D3DIsoline
	{
		public:
			bool Create(gtD3D11& i_D3D, std::string& o_Error)
			{
				using Microsoft::WRL::ComPtr;
				m_pD3D = &i_D3D;
				const std::string root = gtPlatform::RepoRoot();
				const std::wstring file = std::wstring(root.begin(), root.end()) +
										  L"UnitTests\\GraphicsTests\\Vk\\Shaders\\gtIsoline.hlsl";
				ComPtr<ID3DBlob> vs, hs, ds, ps, errors;
				if (FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "VSMain", "vs_5_0", 0, 0, &vs, &errors)) ||
					FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "HSMain", "hs_5_0", 0, 0, &hs, &errors)) ||
					FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "DSMain", "ds_5_0", 0, 0, &ds, &errors)) ||
					FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "PSMain", "ps_5_0", 0, 0, &ps, &errors)))
				{
					o_Error = errors ? (const char*)errors->GetBufferPointer() : "can't compile gtIsoline.hlsl";
					return false;
				}
				ID3D11Device* d = i_D3D.Device();
				d->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &m_pVS);
				d->CreateHullShader(hs->GetBufferPointer(), hs->GetBufferSize(), NULL, &m_pHS);
				d->CreateDomainShader(ds->GetBufferPointer(), ds->GetBufferSize(), NULL, &m_pDS);
				d->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &m_pPS);
				D3D11_TEXTURE2D_DESC desc = { c_Width, c_Height, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, { 1, 0 },
											  D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET, 0, 0 };
				d->CreateTexture2D(&desc, NULL, &m_pTarget);
				if (m_pTarget)
					d->CreateRenderTargetView(m_pTarget.Get(), NULL, &m_pView);
				desc.Usage = D3D11_USAGE_STAGING;
				desc.BindFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				d->CreateTexture2D(&desc, NULL, &m_pStaging);
				D3D11_BUFFER_DESC cb = { sizeof(IsolineConstants), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
				d->CreateBuffer(&cb, NULL, &m_pConstants);
				if (!m_pVS || !m_pHS || !m_pDS || !m_pPS || !m_pView || !m_pStaging || !m_pConstants)
				{
					o_Error = "can't make the D3D11 isoline objects";
					return false;
				}
				return true;
			}

			bool Draw(const Case& i_Case, std::vector<float>& o_Texels, std::string& o_Error)
			{
				ID3D11DeviceContext* c = m_pD3D->Context();
				const IsolineConstants constants = { { i_Case.m_Tessellation, i_Case.m_Interpolation }, i_Case.m_CulledPatch, 0 };
				c->ClearState();
				c->UpdateSubresource(m_pConstants.Get(), 0, NULL, &constants, 0, 0);
				const float clear[4] = { 0, 0, 0, 0 };
				c->ClearRenderTargetView(m_pView.Get(), clear);
				c->OMSetRenderTargets(1, m_pView.GetAddressOf(), NULL);
				D3D11_VIEWPORT viewport = { 0, 0, (float)c_Width, (float)c_Height, 0, 1 };
				c->RSSetViewports(1, &viewport);
				c->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
				c->VSSetShader(m_pVS.Get(), NULL, 0);
				c->HSSetShader(m_pHS.Get(), NULL, 0);
				c->HSSetConstantBuffers(0, 1, m_pConstants.GetAddressOf());
				c->DSSetShader(m_pDS.Get(), NULL, 0);
				c->PSSetShader(m_pPS.Get(), NULL, 0);
				c->Draw(c_Patches, 0);
				c->CopyResource(m_pStaging.Get(), m_pTarget.Get());
				D3D11_MAPPED_SUBRESOURCE mapped;
				if (FAILED(c->Map(m_pStaging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
				{
					o_Error = "can't map the D3D11 target";
					return false;
				}
				const size_t row = c_Width * 4;		// floats
				o_Texels.resize(row * c_Height);
				for (uint32_t y = 0; y < c_Height; y++)
					memcpy(&o_Texels[y * row], (const uint8_t*)mapped.pData + y * mapped.RowPitch, row * sizeof(float));
				c->Unmap(m_pStaging.Get(), 0);
				c->ClearState();
				return true;
			}

		private:
			gtD3D11*									m_pD3D = nullptr;
			Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_pVS;
			Microsoft::WRL::ComPtr<ID3D11HullShader>	m_pHS;
			Microsoft::WRL::ComPtr<ID3D11DomainShader>	m_pDS;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pPS;
			Microsoft::WRL::ComPtr<ID3D11Texture2D>		m_pTarget, m_pStaging;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pView;
			Microsoft::WRL::ComPtr<ID3D11Buffer>		m_pConstants;
	};

	class vkdIsolineTest : public gtVulkan
	{
		protected:
			void SetUp() override
			{
				gtVulkan::SetUp();
				if (!m_pDevice)
				{
					return;
				}
				m_pBindings.reset(new vkdBindings(Device()));
				m_pRing.reset(new vkdRingBuffer(Device(), 1 << 20));
				m_pPipelines.reset(new vkdPipelineCache(Device(), *m_pBindings, ""));
				m_pContext.reset(new vkdContext(Device(), *m_pRing, *m_pBindings, *m_pPipelines));
				std::string error;
				m_pVS = vkdShader::Create(&Device(), VK_SHADER_STAGE_VERTEX_BIT, g_gtIsolineVS, sizeof(g_gtIsolineVS), "VSMain", "gtIsoline VS", error);
				m_pHS = vkdShader::Create(&Device(), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, g_gtIsolineHS, sizeof(g_gtIsolineHS), "HSMain", "gtIsoline HS", error);
				m_pDS = vkdShader::Create(&Device(), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, g_gtIsolineDS, sizeof(g_gtIsolineDS), "DSMain", "gtIsoline DS", error);
				m_pPS = vkdShader::Create(&Device(), VK_SHADER_STAGE_FRAGMENT_BIT, g_gtIsolinePS, sizeof(g_gtIsolinePS), "PSMain", "gtIsoline PS", error);
				ASSERT_TRUE(m_pVS && m_pHS && m_pDS && m_pPS) << error;
				m_pTarget = vkdImage::Create2D(Device(), c_Width, c_Height, VK_FORMAT_R32G32B32A32_SFLOAT,
											   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
											   "vkdIsolineTest target", VK_IMAGE_ASPECT_COLOR_BIT);
				ASSERT_TRUE(m_pTarget);
			}

			void TearDown() override
			{
				m_pTarget.reset();
				m_pVS.reset();
				m_pHS.reset();
				m_pDS.reset();
				m_pPS.reset();
				m_pContext.reset();
				m_pPipelines.reset();
				m_pRing.reset();
				m_pBindings.reset();
				gtVulkan::TearDown();
			}

			std::vector<float> Draw(const Case& i_Case)
			{
				vkdContext& c = *m_pContext;
				vkdImage* target = m_pTarget.get();
				c.SetRenderTargets(1, &target, nullptr);
				const float clear[4] = { 0, 0, 0, 0 };
				c.ClearRenderTarget(target, clear);
				vkdViewport viewport;
				viewport.m_Width = (float)c_Width;
				viewport.m_Height = (float)c_Height;
				c.SetViewports(1, &viewport);
				const vkdShader* stages[5] = { m_pVS.get(), m_pHS.get(), m_pDS.get(), nullptr, m_pPS.get() };
				c.SetShaders(stages);
				c.SetVertexLayout(nullptr);
				c.SetTopology(VK_PRIMITIVE_TOPOLOGY_PATCH_LIST, 1);
				const IsolineConstants constants = { { i_Case.m_Tessellation, i_Case.m_Interpolation }, i_Case.m_CulledPatch, 0 };
				vkdRingBuffer::Allocation cb = c.Upload(&constants, sizeof(constants));
				c.Bindings().SetConstantBuffer(0, cb.m_Gpu, sizeof(constants));
				c.Draw(c_Patches, 0);
				c.Flush();
				const std::vector<uint8_t> bytes = target->ReadBack();
				std::vector<float> texels(bytes.size() / sizeof(float));
				memcpy(texels.data(), bytes.data(), texels.size() * sizeof(float));
				c.SetRenderTargets(0, nullptr, nullptr);
				return texels;
			}

			std::unique_ptr<vkdBindings>		m_pBindings;
			std::unique_ptr<vkdRingBuffer>		m_pRing;
			std::unique_ptr<vkdPipelineCache>	m_pPipelines;
			std::unique_ptr<vkdContext>			m_pContext;
			std::unique_ptr<vkdShader>			m_pVS, m_pHS, m_pDS, m_pPS;
			std::unique_ptr<vkdImage>			m_pTarget;
	};

	//	The points drawn for patch i_Patch (texels with alpha 1)
	uint32_t points(const std::vector<float>& i_Texels, uint32_t i_Patch)
	{
		uint32_t count = 0;
		const size_t rows = c_Height / c_Patches;
		for (size_t t = i_Patch * rows * c_Width; t < (i_Patch + 1) * rows * c_Width; t++)
			count += (i_Texels[t * 4 + 3] == 1.0f) ? 1 : 0;
		return count;
	}
}

TEST_F(vkdIsolineTest, IsolineFactorsMatchD3D11)
{
	GT_D3D11(d3d);
	D3DIsoline dx;
	std::string error;
	ASSERT_TRUE(dx.Create(d3d, error)) << error;

	//	m_HairTessellation and m_HairInterpolationCount at and past their limits
	const float tessellation[] = { 0, 0.5f, 1, 1.3f, 2, 2.5f, 7.5f, 63.9f, 64, 100 };
	const float interpolation[] = { 0, 1, 3, 64, 100 };
	std::vector<Case> cases;
	for (float x : tessellation)
		for (float y : interpolation)
			cases.push_back({ x, y, c_Patches });
	cases.push_back({ 7.5f, 3, 1 });

	for (const Case& test : cases)
	{
		std::vector<float> d3d_texels;
		ASSERT_TRUE(dx.Draw(test, d3d_texels, error)) << error;
		const std::vector<float> vk_texels = Draw(test);
		ASSERT_EQ(d3d_texels.size(), vk_texels.size());

		//	D3D11 draws every patch that isn't culled (its factors are clamped to at least 1)
		for (uint32_t p = 0; p < c_Patches; p++)
		{
			if (p == test.m_CulledPatch)
				EXPECT_EQ(0u, points(d3d_texels, p)) << describe(test);
			else
				EXPECT_LT(0u, points(d3d_texels, p)) << describe(test);
		}

		size_t differ = 0, first = 0;
		for (size_t i = 0; i < d3d_texels.size(); i++)
		{
			if (std::fabs(d3d_texels[i] - vk_texels[i]) > 1e-6f)
			{
				if (differ++ == 0)
					first = i / 4;
			}
		}
		EXPECT_EQ(0u, differ) << describe(test) << ": D3D11 " << points(d3d_texels, 0) << " + " << points(d3d_texels, 1)
							  << " points, Vulkan " << points(vk_texels, 0) << " + " << points(vk_texels, 1)
							  << "; first difference at pixel (" << first % c_Width << ", " << first / c_Width << "): D3D11 ("
							  << d3d_texels[first * 4] << ", " << d3d_texels[first * 4 + 1] << ", " << d3d_texels[first * 4 + 2]
							  << "), Vulkan (" << vk_texels[first * 4] << ", " << vk_texels[first * 4 + 1] << ", "
							  << vk_texels[first * 4 + 2] << ")";
	}
}
