/*****************************************************************************
**  vkdAnisotropicTest.cpp
**
**      Texture filtering on plain D3D11 against plain Vulkan, with none of
**      MachStudio's code in between (Plan 7 Task 1; the roadmap's "Open
**      investigations"): one tilted quad, drawn with hand-made samplers on
**      a D3D11 device (gtD3D11, gtAniso.hlsl compiled by fxc) and on
**      vkdContext (gtAniso.hlsl compiled by DXC). Both get the same
**      vertices (projected on the CPU) and the same texels in every mip
**      (made on the CPU), so any difference is the sampler's.
**
**	  - a hard checker (RGBA8), as MachStudio's textures are sampled;
**	  - the level of detail the sampler computes (CalculateLevelOfDetail);
**	  - a "levels" texture (RGBA16F, every texel of mip m holds m), whose
**	    trilinear sample is the level the filter actually blends at.
**
**	What it found (Plan 7 Task 1, 2026-10-21; Quadro RTX 5000, driver
**	596.59, "Texture filtering - Quality" = High quality): head-on the two
**	APIs are identical. On a texture seen at an angle they compute a
**	different level of detail: D3D11's is the textbook isotropic one
**	(within 0.015), Vulkan's is 0.04-0.07 of a level higher on average (up
**	to 0.25; with 16x anisotropy at 80 degrees, 0.04 lower). Both blend
**	exactly at the level they compute (no "brilinear"). It isn't the
**	viewport's Y flip, the descriptor path (a classic descriptor set gives
**	the same level as the descriptor heap), the sampler, the mips or the
**	UVs. Shader derivatives differ too: D3D11's ddx/ddy are coarse,
**	Vulkan's (DXC: OpDPdx) fine. Only the head-on case is asserted; the
**	rest prints its numbers, for further filtering tests.
**
**	Extra Large Technology
\****************************************************************************/
#include "Vk/gtVulkanContext.hpp"
#include "Dx11/gtD3D11.hpp"
#include "gtImage.hpp"
#include "gtPlatform.hpp"

#ifndef GVK_VKDSHADER_HPP
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#endif

#include "gtAnisoVS.h"
#include "gtAnisoPS.h"
#include "gtAnisoLodPS.h"
#include "gtAnisoDerivPS.h"

#include <DirectXPackedVector.h>
#include <d3dcompiler.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace gvk;

namespace
{
	const uint32_t c_Target = 256;		// render target size
	const uint32_t c_Tex = 256;			// texture size (mip 0)
	const uint32_t c_Cell = 32;			// checker cell size, in texels
	const float c_Repeat = 8.0f;		// texture repeats across the quad

	struct Vertex
	{
		float m_Pos[4];
		float m_UV[2];
	};

	struct Case
	{
		int		m_Anisotropy;		// 1: trilinear (no anisotropic filtering); 0: bilinear, nearest mip
		float	m_TiltDegrees;
		bool	m_bMips;
	};

	std::string name(const Case& i_Case)
	{
		std::ostringstream s;
		s << (i_Case.m_Anisotropy == 0 ? std::string("mippoint") : i_Case.m_Anisotropy == 1 ? std::string("trilinear") : "aniso" + std::to_string(i_Case.m_Anisotropy))
		  << " tilt " << i_Case.m_TiltDegrees << (i_Case.m_bMips ? " mips" : " no mips");
		return s.str();
	}

	//	What a draw samples
	struct TexData
	{
		bool	m_bHalf = false;							// RGBA16F, else RGBA8
		std::vector<std::vector<uint8_t>>	m_Levels;		// each mip, rows tightly packed
		uint32_t PixelBytes() const { return m_bHalf ? 8 : 4; }
	};

	enum Output { e_Color, e_FloatColor, e_Lod, e_Deriv };	// RGBA8 target; RGBA32F target; RGBA32F with PSLod, PSDeriv

	//	A quad of half-size 2 at distance 3 from the eye, tilted about X by
	//	i_Degrees (0: facing the eye), projected with a 1.5 focal length, as
	//	two triangles; z is w/2 (depth 0.5 everywhere)
	std::vector<Vertex> tilted_quad(float i_Degrees)
	{
		const float a = i_Degrees * 3.14159265358979f / 180.0f;
		const float f = 1.5f, d = 3.0f, s = 2.0f;
		auto corner = [&](float x, float y, float u, float v)
		{
			const float vy = y * std::cos(a);
			const float vz = d + y * std::sin(a);
			Vertex out = { { x * f, vy * f, 0.5f * vz, vz }, { u, v } };
			return out;
		};
		const Vertex tl = corner(-s, s, 0, 0), tr = corner(s, s, c_Repeat, 0);
		const Vertex bl = corner(-s, -s, 0, c_Repeat), br = corner(s, -s, c_Repeat, c_Repeat);
		return { tl, tr, bl, bl, tr, br };
	}

	//	The checker and its box-filtered mips
	TexData checker(bool i_bMips)
	{
		TexData t;
		t.m_Levels.assign(1, std::vector<uint8_t>(c_Tex * c_Tex * 4));
		for (uint32_t y = 0; y < c_Tex; y++)
		{
			for (uint32_t x = 0; x < c_Tex; x++)
			{
				const bool white = ((x / c_Cell) + (y / c_Cell)) % 2 == 0;
				uint8_t* p = &t.m_Levels[0][(y * c_Tex + x) * 4];
				p[0] = white ? 255 : 0;
				p[1] = white ? 255 : 32;
				p[2] = white ? 255 : 64;
				p[3] = 255;
			}
		}
		for (uint32_t size = c_Tex / 2; i_bMips && size >= 1; size /= 2)
		{
			const std::vector<uint8_t>& src = t.m_Levels.back();
			const uint32_t src_size = size * 2;
			std::vector<uint8_t> dst(size * size * 4);
			for (uint32_t y = 0; y < size; y++)
			{
				for (uint32_t x = 0; x < size; x++)
				{
					for (uint32_t c = 0; c < 4; c++)
					{
						const uint32_t sum = src[((2 * y) * src_size + 2 * x) * 4 + c] + src[((2 * y) * src_size + 2 * x + 1) * 4 + c] +
											 src[((2 * y + 1) * src_size + 2 * x) * 4 + c] + src[((2 * y + 1) * src_size + 2 * x + 1) * 4 + c];
						dst[(y * size + x) * 4 + c] = (uint8_t)((sum + 2) / 4);
					}
				}
			}
			t.m_Levels.push_back(std::move(dst));
		}
		return t;
	}

	//	RGBA16F, every texel of mip m holding (m, m, m, 1)
	TexData levels()
	{
		TexData t;
		t.m_bHalf = true;
		for (uint32_t size = c_Tex, m = 0; size >= 1; size /= 2, m++)
		{
			std::vector<uint16_t> texels(size * size * 4);
			const uint16_t value = DirectX::PackedVector::XMConvertFloatToHalf((float)m);
			const uint16_t one = DirectX::PackedVector::XMConvertFloatToHalf(1.0f);
			for (uint32_t i = 0; i < size * size; i++)
			{
				texels[i * 4 + 0] = texels[i * 4 + 1] = texels[i * 4 + 2] = value;
				texels[i * 4 + 3] = one;
			}
			t.m_Levels.emplace_back((const uint8_t*)texels.data(), (const uint8_t*)(texels.data() + texels.size()));
		}
		return t;
	}

	struct Difference
	{
		int		m_MaxError = 0;		// largest channel difference, in 1/255
		double	m_PctOver = 0;		// percent of pixels with a channel more than 1/255 apart
	};

	Difference difference(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b)
	{
		Difference d;
		size_t over = 0;
		const size_t pixels = std::min(a.size(), b.size()) / 4;
		for (size_t i = 0; i < pixels; i++)
		{
			int worst = 0;
			for (int c = 0; c < 4; c++)
			{
				worst = std::max(worst, std::abs((int)a[i * 4 + c] - (int)b[i * 4 + c]));
			}
			d.m_MaxError = std::max(d.m_MaxError, worst);
			over += worst > 1;
		}
		d.m_PctOver = pixels ? 100.0 * over / pixels : 0;
		return d;
	}

	//	Channel i_Channel of two RGBA32F images, over the pixels both cover
	//	(alpha 1; the clear is 0)
	struct FloatDifference
	{
		float	m_MaxAbs = 0;		// largest |b - a|
		double	m_Mean = 0;			// mean (b - a)
		double	m_PctOver = 0;		// percent of covered pixels with |b - a| > 1/256
		double	m_PctWholeA = 0;	// percent of covered pixels whose a is a whole number
		double	m_PctWholeB = 0;
	};

	FloatDifference float_difference(const std::vector<uint8_t>& i_A, const std::vector<uint8_t>& i_B, int i_Channel)
	{
		FloatDifference d;
		const float* a = (const float*)i_A.data();
		const float* b = (const float*)i_B.data();
		const size_t pixels = std::min(i_A.size(), i_B.size()) / 16;
		size_t covered = 0, over = 0, whole_a = 0, whole_b = 0;
		double sum = 0;
		for (size_t i = 0; i < pixels; i++)
		{
			if (a[i * 4 + 3] != 1.0f || b[i * 4 + 3] != 1.0f)
			{
				continue;
			}
			const float va = a[i * 4 + i_Channel], vb = b[i * 4 + i_Channel];
			covered++;
			d.m_MaxAbs = std::max(d.m_MaxAbs, std::fabs(vb - va));
			sum += vb - va;
			over += std::fabs(vb - va) > 1.0f / 256;
			whole_a += va == std::floor(va);
			whole_b += vb == std::floor(vb);
		}
		if (covered)
		{
			d.m_Mean = sum / covered;
			d.m_PctOver = 100.0 * over / covered;
			d.m_PctWholeA = 100.0 * whole_a / covered;
			d.m_PctWholeB = 100.0 * whole_b / covered;
		}
		return d;
	}

	std::string describe(const FloatDifference& i_D)
	{
		std::ostringstream s;
		s << "max |vk-dx| " << i_D.m_MaxAbs << ", mean (vk-dx) " << i_D.m_Mean << ", " << i_D.m_PctOver
		  << "% over 1/256; whole numbers dx " << i_D.m_PctWholeA << "%, vk " << i_D.m_PctWholeB << "%";
		return s.str();
	}

	//------------------------------------------------------------------------
	//	The D3D11 side
	//------------------------------------------------------------------------
	class D3DAniso
	{
		public:
			bool Create(gtD3D11& i_D3D, std::string& o_Error)
			{
				m_pD3D = &i_D3D;
				const std::string root = gtPlatform::RepoRoot();
				const std::wstring file = std::wstring(root.begin(), root.end()) +
										  L"UnitTests\\GraphicsTests\\Vk\\Shaders\\gtAniso.hlsl";
				Microsoft::WRL::ComPtr<ID3DBlob> vs, ps, lod, deriv, errors;
				if (FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "VSMain", "vs_5_0", 0, 0, &vs, &errors)) ||
					FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "PSMain", "ps_5_0", 0, 0, &ps, &errors)) ||
					FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "PSLod", "ps_5_0", 0, 0, &lod, &errors)) ||
					FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "PSDeriv", "ps_5_0", 0, 0, &deriv, &errors)))
				{
					o_Error = errors ? (const char*)errors->GetBufferPointer() : "can't compile gtAniso.hlsl";
					return false;
				}
				ID3D11Device* d = i_D3D.Device();
				d->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &m_pVS);
				d->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &m_pPS);
				d->CreatePixelShader(lod->GetBufferPointer(), lod->GetBufferSize(), NULL, &m_pLodPS);
				d->CreatePixelShader(deriv->GetBufferPointer(), deriv->GetBufferSize(), NULL, &m_pDerivPS);
				const D3D11_INPUT_ELEMENT_DESC layout[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				d->CreateInputLayout(layout, 2, vs->GetBufferPointer(), vs->GetBufferSize(), &m_pLayout);
				return m_pVS && m_pPS && m_pLodPS && m_pDerivPS && m_pLayout;
			}

			std::vector<uint8_t> Draw(const Case& i_Case, const TexData& i_Tex, Output i_Output)
			{
				ID3D11Device* d = m_pD3D->Device();
				ID3D11DeviceContext* c = m_pD3D->Context();

				D3D11_TEXTURE2D_DESC td = {};
				td.Width = td.Height = c_Tex;
				td.MipLevels = (UINT)i_Tex.m_Levels.size();
				td.ArraySize = 1;
				td.Format = i_Tex.m_bHalf ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
				td.SampleDesc.Count = 1;
				td.Usage = D3D11_USAGE_IMMUTABLE;
				td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				std::vector<D3D11_SUBRESOURCE_DATA> init(i_Tex.m_Levels.size());
				for (size_t m = 0; m < i_Tex.m_Levels.size(); m++)
				{
					init[m] = { i_Tex.m_Levels[m].data(), (UINT)(std::max(c_Tex >> m, 1u) * i_Tex.PixelBytes()), 0 };
				}
				Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
				d->CreateTexture2D(&td, init.data(), &tex);
				d->CreateShaderResourceView(tex.Get(), NULL, &srv);

				const bool is_float = i_Output != e_Color;
				D3D11_TEXTURE2D_DESC rd = {};
				rd.Width = rd.Height = c_Target;
				rd.MipLevels = rd.ArraySize = 1;
				rd.Format = is_float ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
				rd.SampleDesc.Count = 1;
				rd.BindFlags = D3D11_BIND_RENDER_TARGET;
				Microsoft::WRL::ComPtr<ID3D11Texture2D> target, staging;
				Microsoft::WRL::ComPtr<ID3D11RenderTargetView> view;
				d->CreateTexture2D(&rd, NULL, &target);
				d->CreateRenderTargetView(target.Get(), NULL, &view);
				rd.BindFlags = 0;
				rd.Usage = D3D11_USAGE_STAGING;
				rd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				d->CreateTexture2D(&rd, NULL, &staging);

				D3D11_SAMPLER_DESC sd = {};
				sd.Filter = i_Case.m_Anisotropy > 1 ? D3D11_FILTER_ANISOTROPIC :
							i_Case.m_Anisotropy == 1 ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
				sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				sd.MaxAnisotropy = (UINT)std::max(i_Case.m_Anisotropy, 1);
				sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
				sd.MinLOD = 0;
				sd.MaxLOD = D3D11_FLOAT32_MAX;
				Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
				d->CreateSamplerState(&sd, &sampler);

				D3D11_RASTERIZER_DESC rs = {};
				rs.FillMode = D3D11_FILL_SOLID;
				rs.CullMode = D3D11_CULL_NONE;
				rs.DepthClipEnable = TRUE;
				Microsoft::WRL::ComPtr<ID3D11RasterizerState> raster;
				d->CreateRasterizerState(&rs, &raster);

				const std::vector<Vertex> vertices = tilted_quad(i_Case.m_TiltDegrees);
				D3D11_BUFFER_DESC vbDesc = { (UINT)(vertices.size() * sizeof(Vertex)), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
				D3D11_SUBRESOURCE_DATA vbInit = { vertices.data(), 0, 0 };
				Microsoft::WRL::ComPtr<ID3D11Buffer> vb;
				d->CreateBuffer(&vbDesc, &vbInit, &vb);

				c->ClearState();
				const float clear[4] = { 0, 0, 0, is_float ? 0.0f : 1.0f };
				c->ClearRenderTargetView(view.Get(), clear);
				c->OMSetRenderTargets(1, view.GetAddressOf(), NULL);
				D3D11_VIEWPORT viewport = { 0, 0, (float)c_Target, (float)c_Target, 0, 1 };
				c->RSSetViewports(1, &viewport);
				c->RSSetState(raster.Get());
				UINT stride = sizeof(Vertex), offset = 0;
				c->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
				c->IASetInputLayout(m_pLayout.Get());
				c->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				c->VSSetShader(m_pVS.Get(), NULL, 0);
				c->PSSetShader(i_Output == e_Lod ? m_pLodPS.Get() : i_Output == e_Deriv ? m_pDerivPS.Get() : m_pPS.Get(), NULL, 0);
				c->PSSetShaderResources(0, 1, srv.GetAddressOf());
				c->PSSetSamplers(0, 1, sampler.GetAddressOf());
				c->Draw((UINT)vertices.size(), 0);

				c->CopyResource(staging.Get(), target.Get());
				const uint32_t row = c_Target * (is_float ? 16 : 4);
				std::vector<uint8_t> image(row * c_Target);
				D3D11_MAPPED_SUBRESOURCE mapped;
				if (SUCCEEDED(c->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
				{
					for (uint32_t y = 0; y < c_Target; y++)
					{
						memcpy(&image[y * row], (const uint8_t*)mapped.pData + y * mapped.RowPitch, row);
					}
					c->Unmap(staging.Get(), 0);
				}
				return image;
			}

		private:
			gtD3D11* m_pD3D = nullptr;
			Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_pVS;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_pPS, m_pLodPS, m_pDerivPS;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>		m_pLayout;
	};

	//------------------------------------------------------------------------
	//	The Vulkan side
	//------------------------------------------------------------------------
	class vkdAnisotropicTest : public gtVulkanContext
	{
		protected:
			void SetUp() override
			{
				gtVulkanContext::SetUp();
				if (!m_pDevice)
				{
					return;
				}
				std::string error;
				m_pVS = vkdShader::Create(&Device(), VK_SHADER_STAGE_VERTEX_BIT, g_gtAnisoVS, sizeof(g_gtAnisoVS), "VSMain", "gtAniso VS", error);
				m_pPS = vkdShader::Create(&Device(), VK_SHADER_STAGE_FRAGMENT_BIT, g_gtAnisoPS, sizeof(g_gtAnisoPS), "PSMain", "gtAniso PS", error);
				m_pLodPS = vkdShader::Create(&Device(), VK_SHADER_STAGE_FRAGMENT_BIT, g_gtAnisoLodPS, sizeof(g_gtAnisoLodPS), "PSLod", "gtAniso LodPS", error);
				m_pDerivPS = vkdShader::Create(&Device(), VK_SHADER_STAGE_FRAGMENT_BIT, g_gtAnisoDerivPS, sizeof(g_gtAnisoDerivPS), "PSDeriv", "gtAniso DerivPS", error);
				ASSERT_TRUE(m_pVS && m_pPS && m_pLodPS && m_pDerivPS) << error;
				m_pLayout.reset(new vkdVertexLayout({ { "POSITION", 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0, 0 },
													  { "TEXCOORD", 0, VK_FORMAT_R32G32_SFLOAT, 0, 16 } }, { sizeof(Vertex) }));
			}

			void TearDown() override
			{
				m_Textures.clear();
				m_pVS.reset();
				m_pPS.reset();
				m_pLodPS.reset();
				m_pDerivPS.reset();
				gtVulkanContext::TearDown();
			}

			//	i_bPositiveViewport: a Vulkan viewport with a positive height (no Y
			//	flip by the viewport), the vertices mirrored in Y instead, so the
			//	image is the same
			//	i_bClassic: the draw itself is recorded by hand with a classic
			//	descriptor set (no VK_EXT_descriptor_heap): same shaders, sampler,
			//	image, viewport and vertices (draw_classic)
			std::vector<uint8_t> Draw(const Case& i_Case, const TexData& i_Tex, Output i_Output, bool i_bPositiveViewport = false,
									  bool i_bClassic = false)
			{
				vkdContext& c = Context();
				vkdImageDesc desc;
				desc.m_Format = i_Tex.m_bHalf ? VK_FORMAT_R16G16B16A16_SFLOAT : VK_FORMAT_R8G8B8A8_UNORM;
				desc.m_Width = desc.m_Height = c_Tex;
				desc.m_Mips = (uint32_t)i_Tex.m_Levels.size();
				desc.m_Usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
				m_Textures.push_back(vkdImage::Create(Device(), desc, "vkdAnisotropicTest texture"));
				vkdImage* tex = m_Textures.back().get();
				for (uint32_t m = 0; m < (uint32_t)i_Tex.m_Levels.size(); m++)
				{
					const size_t row = std::max(c_Tex >> m, 1u) * i_Tex.PixelBytes();
					tex->Upload(c, m, 0, i_Tex.m_Levels[m].data(), row, row * std::max(c_Tex >> m, 1u));
				}

				const bool is_float = i_Output != e_Color;
				m_Textures.push_back(vkdImage::Create2D(Device(), c_Target, c_Target,
					is_float ? VK_FORMAT_R32G32B32A32_SFLOAT : VK_FORMAT_R8G8B8A8_UNORM,
					VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
					"vkdAnisotropicTest target"));
				vkdImage* target = m_Textures.back().get();

				VkSamplerCreateInfo info = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
				info.magFilter = info.minFilter = VK_FILTER_LINEAR;
				info.mipmapMode = i_Case.m_Anisotropy == 0 ? VK_SAMPLER_MIPMAP_MODE_NEAREST : VK_SAMPLER_MIPMAP_MODE_LINEAR;
				info.addressModeU = info.addressModeV = info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				info.anisotropyEnable = i_Case.m_Anisotropy > 1 ? VK_TRUE : VK_FALSE;
				info.maxAnisotropy = (float)std::max(i_Case.m_Anisotropy, 1);
				info.minLod = 0;
				info.maxLod = VK_LOD_CLAMP_NONE;

				c.SetRenderTargets(1, &target, nullptr);
				vkdViewport viewport;
				viewport.m_Width = viewport.m_Height = (float)c_Target;
				if (i_bPositiveViewport)
				{
					//	vkdContext passes (m_Y + m_Height, -m_Height) to Vulkan
					viewport.m_Y = (float)c_Target;
					viewport.m_Height = -(float)c_Target;
				}
				c.SetViewports(1, &viewport);
				vkdRasterizerState raster;
				raster.m_Cull = VK_CULL_MODE_NONE;
				c.SetRasterizerState(raster);
				const vkdShader* stages[5] = { m_pVS.get(), nullptr, nullptr, nullptr,
											   i_Output == e_Lod ? m_pLodPS.get() : i_Output == e_Deriv ? m_pDerivPS.get() : m_pPS.get() };
				c.SetShaders(stages);
				c.SetVertexLayout(m_pLayout.get());
				c.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
				const float clear[4] = { 0, 0, 0, is_float ? 0.0f : 1.0f };
				c.ClearRenderTarget(target, clear);
				if (i_bClassic)
				{
					c.Flush();
					draw_classic(i_Case, tex, target, info, i_Output);
					return target->ReadBack();
				}
				c.Bindings().SetTexture(0, tex->SampledIndex(c.Bindings()));
				c.Bindings().SetSampler(0, c.Bindings().AddSampler(info));
				std::vector<Vertex> vertices = tilted_quad(i_Case.m_TiltDegrees);
				for (Vertex& v : vertices)
				{
					v.m_Pos[1] = i_bPositiveViewport ? -v.m_Pos[1] : v.m_Pos[1];
				}
				vkdRingBuffer::Allocation vb = c.Upload(vertices.data(), vertices.size() * sizeof(Vertex), 16);
				c.SetVertexBuffer(0, vb.m_Buffer, vb.m_Offset);
				c.Draw((uint32_t)vertices.size(), 0);
				return Read(target);
			}

			//	The draw recorded by hand: a pipeline with a classic layout (set 0:
			//	the texture at binding 100, the sampler at 200, as DXC's register
			//	shifts put them), its own VkSampler, a negative-height viewport
			//	(as vkdContext's), in a command buffer of its own that never binds
			//	the descriptor heaps. The target was cleared and the texture
			//	uploaded by the context, which has submitted that work.
			void draw_classic(const Case& i_Case, vkdImage* i_pTex, vkdImage* i_pTarget, const VkSamplerCreateInfo& i_Sampler, Output i_Output)
			{
				VkDevice device = Device().Device();
				VkSampler sampler = VK_NULL_HANDLE;
				vkCreateSampler(device, &i_Sampler, nullptr, &sampler);

				VkDescriptorSetLayoutBinding bindings[2] = {};
				bindings[0] = { 100, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr };
				bindings[1] = { 200, VK_DESCRIPTOR_TYPE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr };
				VkDescriptorSetLayoutCreateInfo set_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
				set_info.bindingCount = 2;
				set_info.pBindings = bindings;
				VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
				vkCreateDescriptorSetLayout(device, &set_info, nullptr, &set_layout);
				VkPipelineLayoutCreateInfo layout_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
				layout_info.setLayoutCount = 1;
				layout_info.pSetLayouts = &set_layout;
				VkPipelineLayout layout = VK_NULL_HANDLE;
				vkCreatePipelineLayout(device, &layout_info, nullptr, &layout);

				VkDescriptorPoolSize sizes[2] = { { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1 }, { VK_DESCRIPTOR_TYPE_SAMPLER, 1 } };
				VkDescriptorPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
				pool_info.maxSets = 1;
				pool_info.poolSizeCount = 2;
				pool_info.pPoolSizes = sizes;
				VkDescriptorPool pool = VK_NULL_HANDLE;
				vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);
				VkDescriptorSetAllocateInfo alloc = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
				alloc.descriptorPool = pool;
				alloc.descriptorSetCount = 1;
				alloc.pSetLayouts = &set_layout;
				VkDescriptorSet set = VK_NULL_HANDLE;
				vkAllocateDescriptorSets(device, &alloc, &set);
				//	a view of its own: with descriptor heaps a sampled-only vkdImage
				//	may have no VkImageView (descriptors are made from ViewInfo())
				VkImageView view = VK_NULL_HANDLE;
				vkCreateImageView(device, &i_pTex->ViewInfo(), nullptr, &view);
				const VkDescriptorImageInfo image = { VK_NULL_HANDLE, view, VK_IMAGE_LAYOUT_GENERAL };
				const VkDescriptorImageInfo samp = { sampler, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_UNDEFINED };
				VkWriteDescriptorSet writes[2] = { {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET}, {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET} };
				writes[0].dstSet = set;
				writes[0].dstBinding = 100;
				writes[0].descriptorCount = 1;
				writes[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				writes[0].pImageInfo = &image;
				writes[1].dstSet = set;
				writes[1].dstBinding = 200;
				writes[1].descriptorCount = 1;
				writes[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				writes[1].pImageInfo = &samp;
				vkUpdateDescriptorSets(device, 2, writes, 0, nullptr);

				auto module = [&](const unsigned char* i_pCode, size_t i_Size)
				{
					VkShaderModuleCreateInfo info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
					info.codeSize = i_Size;
					info.pCode = (const uint32_t*)i_pCode;
					VkShaderModule m = VK_NULL_HANDLE;
					vkCreateShaderModule(device, &info, nullptr, &m);
					return m;
				};
				const bool lod = i_Output == e_Lod;
				VkShaderModule vs = module(g_gtAnisoVS, sizeof(g_gtAnisoVS));
				VkShaderModule ps = lod ? module(g_gtAnisoLodPS, sizeof(g_gtAnisoLodPS)) : module(g_gtAnisoPS, sizeof(g_gtAnisoPS));
				VkPipelineShaderStageCreateInfo stages[2] = { {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO},
															  {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO} };
				stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
				stages[0].module = vs;
				stages[0].pName = "VSMain";
				stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				stages[1].module = ps;
				stages[1].pName = lod ? "PSLod" : "PSMain";

				const VkVertexInputBindingDescription vb_binding = { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX };
				const VkVertexInputAttributeDescription attributes[2] = { { 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0 },
																		  { 1, 0, VK_FORMAT_R32G32_SFLOAT, 16 } };
				VkPipelineVertexInputStateCreateInfo vertex_input = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
				vertex_input.vertexBindingDescriptionCount = 1;
				vertex_input.pVertexBindingDescriptions = &vb_binding;
				vertex_input.vertexAttributeDescriptionCount = 2;
				vertex_input.pVertexAttributeDescriptions = attributes;
				VkPipelineInputAssemblyStateCreateInfo assembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
				assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				const VkViewport viewport = { 0, (float)c_Target, (float)c_Target, -(float)c_Target, 0, 1 };
				const VkRect2D scissor = { { 0, 0 }, { c_Target, c_Target } };
				VkPipelineViewportStateCreateInfo viewport_state = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
				viewport_state.viewportCount = 1;
				viewport_state.pViewports = &viewport;
				viewport_state.scissorCount = 1;
				viewport_state.pScissors = &scissor;
				VkPipelineRasterizationStateCreateInfo raster = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
				raster.polygonMode = VK_POLYGON_MODE_FILL;
				raster.cullMode = VK_CULL_MODE_NONE;
				raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
				raster.lineWidth = 1.0f;
				VkPipelineMultisampleStateCreateInfo multisample = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
				multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
				VkPipelineDepthStencilStateCreateInfo depth = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
				VkPipelineColorBlendAttachmentState blend_attachment = {};
				blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				VkPipelineColorBlendStateCreateInfo blend = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
				blend.attachmentCount = 1;
				blend.pAttachments = &blend_attachment;
				const VkFormat color_format = i_pTarget->Format();
				VkPipelineRenderingCreateInfo rendering = {VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
				rendering.colorAttachmentCount = 1;
				rendering.pColorAttachmentFormats = &color_format;
				VkGraphicsPipelineCreateInfo pipeline_info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
				pipeline_info.pNext = &rendering;
				pipeline_info.stageCount = 2;
				pipeline_info.pStages = stages;
				pipeline_info.pVertexInputState = &vertex_input;
				pipeline_info.pInputAssemblyState = &assembly;
				pipeline_info.pViewportState = &viewport_state;
				pipeline_info.pRasterizationState = &raster;
				pipeline_info.pMultisampleState = &multisample;
				pipeline_info.pDepthStencilState = &depth;
				pipeline_info.pColorBlendState = &blend;
				pipeline_info.layout = layout;
				VkPipeline pipeline = VK_NULL_HANDLE;
				ASSERT_EQ(VK_SUCCESS, vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline));

				const std::vector<Vertex> vertices = tilted_quad(i_Case.m_TiltDegrees);
				vkdRingBuffer::Allocation vb = Context().Upload(vertices.data(), vertices.size() * sizeof(Vertex), 16);
				Context().Flush();

				VkCommandBuffer cmd = Device().Commands();
				VkMemoryBarrier2 all = {VK_STRUCTURE_TYPE_MEMORY_BARRIER_2};
				all.srcStageMask = all.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
				all.srcAccessMask = all.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
				VkDependencyInfo dependency = {VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
				dependency.memoryBarrierCount = 1;
				dependency.pMemoryBarriers = &all;
				vkCmdPipelineBarrier2(cmd, &dependency);
				VkRenderingAttachmentInfo color = {VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
				color.imageView = i_pTarget->View();
				color.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
				color.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				VkRenderingInfo info = {VK_STRUCTURE_TYPE_RENDERING_INFO};
				info.renderArea = scissor;
				info.layerCount = 1;
				info.colorAttachmentCount = 1;
				info.pColorAttachments = &color;
				vkCmdBeginRendering(cmd, &info);
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &set, 0, nullptr);
				vkCmdBindVertexBuffers(cmd, 0, 1, &vb.m_Buffer, &vb.m_Offset);
				vkCmdDraw(cmd, (uint32_t)vertices.size(), 1, 0, 0);
				vkCmdEndRendering(cmd);
				vkCmdPipelineBarrier2(cmd, &dependency);
				Device().WaitFor(Device().Submit());

				vkDestroyPipeline(device, pipeline, nullptr);
				vkDestroyShaderModule(device, vs, nullptr);
				vkDestroyShaderModule(device, ps, nullptr);
				vkDestroyDescriptorPool(device, pool, nullptr);
				vkDestroyPipelineLayout(device, layout, nullptr);
				vkDestroyDescriptorSetLayout(device, set_layout, nullptr);
				vkDestroySampler(device, sampler, nullptr);
				vkDestroyImageView(device, view, nullptr);
			}

			std::unique_ptr<vkdShader>			m_pVS, m_pPS, m_pLodPS, m_pDerivPS;
			std::unique_ptr<vkdVertexLayout>	m_pLayout;
			std::vector<std::unique_ptr<vkdImage>> m_Textures;
	};
}

//	The hard checker head-on (asserted: identical within 1/255) and at 60
//	and 80 degrees (printed: max error and % of pixels over 1/255).
TEST_F(vkdAnisotropicTest, AnisotropicMatchesD3D11)
{
	GT_D3D11(d3d);
	D3DAniso dx;
	std::string error;
	ASSERT_TRUE(dx.Create(d3d, error)) << error;

	const TexData mips = checker(true), one = checker(false);
	for (bool with_mips : { true, false })
	{
		for (float tilt : { 0.0f, 60.0f, 80.0f })
		{
			for (int aniso : { 0, 1, 2, 4, 8, 16 })
			{
				const Case c = { aniso, tilt, with_mips };
				const std::vector<uint8_t> a = dx.Draw(c, with_mips ? mips : one, e_Color);
				const std::vector<uint8_t> b = Draw(c, with_mips ? mips : one, e_Color);
				const Difference diff = difference(a, b);
				if (tilt == 0.0f)
				{
					EXPECT_LE(diff.m_MaxError, 1) << name(c);
					if (diff.m_MaxError > 1)
					{
						gtSaveComparison("vkdAnisotropicTest " + name(c), a.data(), b.data(), c_Target, c_Target);
					}
				}
				std::cout << "[ aniso    ] " << name(c) << ": max " << diff.m_MaxError << "/255, "
						  << diff.m_PctOver << "% over 1/255" << std::endl;
			}
		}
	}
}

//	Diagnostics for the investigation: the level of detail each API computes
//	(CalculateLevelOfDetail, clamped and unclamped), and the level each
//	filter blends at (the "levels" texture sampled)
TEST_F(vkdAnisotropicTest, LevelOfDetailMatchesD3D11)
{
	GT_D3D11(d3d);
	D3DAniso dx;
	std::string error;
	ASSERT_TRUE(dx.Create(d3d, error)) << error;

	const TexData mips = checker(true), lv = levels();
	for (float tilt : { 0.0f, 30.0f, 60.0f, 80.0f })
	{
		for (int aniso : { 0, 1, 2, 16 })
		{
			const Case c = { aniso, tilt, true };
			const std::vector<uint8_t> a = dx.Draw(c, mips, e_Lod);
			const std::vector<uint8_t> b = Draw(c, mips, e_Lod);
			std::cout << "[ lod      ] " << name(c) << " clamped:   " << describe(float_difference(a, b, 0)) << std::endl;
			std::cout << "[ lod      ] " << name(c) << " unclamped: " << describe(float_difference(a, b, 1)) << std::endl;
			const std::vector<uint8_t> la = dx.Draw(c, lv, e_FloatColor);
			const std::vector<uint8_t> lb = Draw(c, lv, e_FloatColor);
			std::cout << "[ levels   ] " << name(c) << ": " << describe(float_difference(la, lb, 0)) << std::endl;
			const std::vector<uint8_t> pb = Draw(c, mips, e_Lod, true);
			std::cout << "[ lod +vp  ] " << name(c) << " unclamped, positive viewport: " << describe(float_difference(a, pb, 1)) << std::endl;
			const std::vector<uint8_t> ca = dx.Draw(c, mips, e_Color);
			const std::vector<uint8_t> cb = Draw(c, mips, e_Color, true);
			const Difference cd = difference(ca, cb);
			std::cout << "[ color +vp] " << name(c) << " checker, positive viewport: max " << cd.m_MaxError << "/255, " << cd.m_PctOver << "% over" << std::endl;
		}
	}
}

//	Diagnostics: are the texture coordinate's derivatives the same on both
//	APIs, and which API's level of detail is the textbook isotropic one,
//	log2(max(|d(uv)/dx|, |d(uv)/dy|) * size)?
TEST_F(vkdAnisotropicTest, DerivativesAndLodFormula)
{
	GT_D3D11(d3d);
	D3DAniso dx;
	std::string error;
	ASSERT_TRUE(dx.Create(d3d, error)) << error;

	const TexData mips = checker(true);
	for (float tilt : { 60.0f, 80.0f })
	{
		const Case c = { 1, tilt, true };
		const std::vector<uint8_t> dda = dx.Draw(c, mips, e_Deriv), ddb = Draw(c, mips, e_Deriv);
		const std::vector<uint8_t> la = dx.Draw(c, mips, e_Lod), lb = Draw(c, mips, e_Lod);
		const float* da = (const float*)dda.data();
		const float* db = (const float*)ddb.data();
		const float* lda = (const float*)la.data();
		const float* ldb = (const float*)lb.data();
		float max_deriv = 0, max_rel = 0;
		double sum_a = 0, sum_b = 0, max_a = 0, max_b = 0, sum_bc = 0, max_bc = 0;
		size_t covered = 0, deriv_diff = 0;
		for (size_t i = 0; i < (size_t)c_Target * c_Target; i++)
		{
			if (lda[i * 4 + 3] != 1.0f || ldb[i * 4 + 3] != 1.0f)
			{
				continue;
			}
			covered++;
			bool differs = false;
			for (int k = 0; k < 4; k++)
			{
				const float diff = std::fabs(da[i * 4 + k] - db[i * 4 + k]);
				max_deriv = std::max(max_deriv, diff);
				max_rel = std::max(max_rel, diff / std::max(std::fabs(da[i * 4 + k]), 1e-12f));
				differs |= diff != 0;
			}
			deriv_diff += differs;
			//	the textbook level of detail, from each API's own derivatives
			auto textbook = [](const float* d)
			{
				const double x = std::hypot(d[0], d[1]) * c_Tex, y = std::hypot(d[2], d[3]) * c_Tex;
				return std::log2(std::max(x, y));
			};
			const double ea = lda[i * 4 + 1] - textbook(&da[i * 4]);
			const double eb = ldb[i * 4 + 1] - textbook(&db[i * 4]);
			const double ebc = ldb[i * 4 + 1] - textbook(&da[i * 4]);	// Vulkan's LOD against D3D11's (coarse) derivatives
			sum_bc += ebc;
			max_bc = std::max(max_bc, std::fabs(ebc));
			sum_a += ea;
			sum_b += eb;
			max_a = std::max(max_a, std::fabs(ea));
			max_b = std::max(max_b, std::fabs(eb));
		}
		for (uint32_t y = 200; y < 204; y++)
		{
			std::ostringstream row;
			for (uint32_t x = 128; x < 132; x++)
			{
				const size_t i = (size_t)y * c_Target + x;
				row << " (" << da[i * 4 + 0] << "," << da[i * 4 + 3] << " | " << db[i * 4 + 0] << "," << db[i * 4 + 3] << ")";
			}
			std::cout << "[ block    ] y " << y << " du/dx,dv/dy dx|vk:" << row.str() << std::endl;
		}
		//	Which approximation of the scale factor is Vulkan's? (fit against
		//	D3D11's coarse derivatives, which both samplers see)
		{
			const char* names[] = { "L2 max", "L1 max", "Linf max", "Frobenius", "L2 mean", "L2 geomean" };
			double sum[6] = {}, worst[6] = {};
			size_t n = 0;
			for (size_t i = 0; i < (size_t)c_Target * c_Target; i++)
			{
				if (lda[i * 4 + 3] != 1.0f || ldb[i * 4 + 3] != 1.0f)
				{
					continue;
				}
				n++;
				const float* d = &da[i * 4];
				const double ax = std::fabs(d[0]) * c_Tex, bx = std::fabs(d[1]) * c_Tex, ay = std::fabs(d[2]) * c_Tex, by = std::fabs(d[3]) * c_Tex;
				const double lx = std::hypot(ax, bx), ly = std::hypot(ay, by);
				const double rho[6] = { std::max(lx, ly), std::max(ax + bx, ay + by), std::max(std::max(ax, bx), std::max(ay, by)),
										std::sqrt(lx * lx + ly * ly), (lx + ly) / 2, std::sqrt(lx * ly) };
				for (int f = 0; f < 6; f++)
				{
					const double e = ldb[i * 4 + 1] - std::log2(rho[f]);
					sum[f] += e;
					worst[f] = std::max(worst[f], std::fabs(e));
				}
			}
			for (int f = 0; f < 6; f++)
			{
				std::cout << "[ fit      ] tilt " << tilt << " vk LOD - " << names[f] << ": mean " << sum[f] / n << " max " << worst[f] << std::endl;
			}
		}
		std::cout << "[ deriv    ] tilt " << tilt << ": " << covered << " px, derivatives differ on " << deriv_diff
				  << " (max abs " << max_deriv << ", max rel " << max_rel << ")" << std::endl;
		std::cout << "[ formula  ] tilt " << tilt << ": unclamped LOD - textbook: dx mean " << sum_a / covered << " max " << max_a
				  << "; vk mean " << sum_b / covered << " max " << max_b
				  << "; vk against dx's derivatives mean " << sum_bc / covered << " max " << max_bc << std::endl;
	}
}

//	Investigation, Step 3: does the descriptor path change the level of
//	detail? The same draw with a classic descriptor set, against D3D11 and
//	against the descriptor-heap draw.
TEST_F(vkdAnisotropicTest, DescriptorPathLevelOfDetail)
{
	GT_D3D11(d3d);
	D3DAniso dx;
	std::string error;
	ASSERT_TRUE(dx.Create(d3d, error)) << error;

	const TexData mips = checker(true);
	for (float tilt : { 60.0f, 80.0f })
	{
		for (int aniso : { 1, 16 })
		{
			const Case c = { aniso, tilt, true };
			const std::vector<uint8_t> a = dx.Draw(c, mips, e_Lod);
			const std::vector<uint8_t> heap = Draw(c, mips, e_Lod);
			const std::vector<uint8_t> classic = Draw(c, mips, e_Lod, false, true);
			std::cout << "[ classic  ] " << name(c) << " LOD classic vs dx:   " << describe(float_difference(a, classic, 1)) << std::endl;
			std::cout << "[ classic  ] " << name(c) << " LOD classic vs heap: " << describe(float_difference(heap, classic, 1)) << std::endl;
			const std::vector<uint8_t> ca = dx.Draw(c, mips, e_Color);
			const std::vector<uint8_t> cb = Draw(c, mips, e_Color, false, true);
			const Difference cd = difference(ca, cb);
			std::cout << "[ classic  ] " << name(c) << " checker classic vs dx: max " << cd.m_MaxError << "/255, " << cd.m_PctOver << "% over" << std::endl;
		}
	}
}
