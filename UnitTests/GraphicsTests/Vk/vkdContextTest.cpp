/*****************************************************************************
**  vkdContextTest.cpp
**
**      vkdContext against the behaviour ported DX11 code expects: draws land
**      where D3D11 puts them (viewport, Y direction, winding and culling,
**      compared with a D3D11 device drawing the same HLSL), depth, stencil
**      and scissor state work, rendering to a target and then sampling it
**      is ordered (synchronization validation is on), and state changes
**      that are dynamic don't create pipelines. Layered rendering (a
**      geometry shader picking the slice of a volume with
**      SV_RenderTargetArrayIndex) matches D3D11's.
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

#include "gtSolidVS.h"
#include "gtSolidPS.h"
#include "gtBindingProtoVS.h"
#include "gtBindingProtoPS.h"
#include "gtLayeredVS.h"
#include "gtLayeredGS.h"
#include "gtLayeredPS.h"

#include <d3dcompiler.h>

#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace gvk;

namespace
{
	const uint32_t c_Size = 64;

	struct Vertex
	{
		float m_Pos[3];
		float m_Color[4];
	};

	//	Two triangles in clip space: the left one clockwise on screen, the
	//	right one counter-clockwise (D3D: +Y up in clip space, down on screen)
	const Vertex c_Wound[] =
	{
		{ { -0.9f, 0.8f, 0.5f }, { 1, 0, 0, 1 } }, { { -0.1f, 0.8f, 0.5f }, { 1, 0, 0, 1 } }, { { -0.9f, -0.8f, 0.5f }, { 1, 0, 0, 1 } },
		{ { 0.1f, 0.8f, 0.5f }, { 0, 1, 0, 1 } }, { { 0.1f, -0.8f, 0.5f }, { 0, 1, 0, 1 } }, { { 0.9f, 0.8f, 0.5f }, { 0, 1, 0, 1 } },
	};

	//	A quad covering the target at depth i_Z, as a triangle list
	std::vector<Vertex> quad(float i_Z, const float i_Color[4], float i_X0 = -1, float i_X1 = 1)
	{
		std::vector<Vertex> v(6);
		const float p[6][2] = { { i_X0, 1 }, { i_X1, 1 }, { i_X0, -1 }, { i_X0, -1 }, { i_X1, 1 }, { i_X1, -1 } };
		for (int i = 0; i < 6; i++)
		{
			v[i] = { { p[i][0], p[i][1], i_Z }, { i_Color[0], i_Color[1], i_Color[2], i_Color[3] } };
		}
		return v;
	}

	uint32_t pixel(const std::vector<uint8_t>& i_Image, uint32_t i_X, uint32_t i_Y)
	{
		uint32_t p;
		memcpy(&p, &i_Image[(i_Y * c_Size + i_X) * 4], 4);
		return p;
	}

	const uint32_t c_Red = 0xFF0000FF, c_Green = 0xFF00FF00, c_Blue = 0xFFFF0000;

	//------------------------------------------------------------------------
	//	The Vulkan side
	//------------------------------------------------------------------------
	class vkdContextTest : public gtVulkan
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
				m_pSolidVS = vkdShader::Create(&Device(), VK_SHADER_STAGE_VERTEX_BIT, g_gtSolidVS, sizeof(g_gtSolidVS), "VSMain", "gtSolid VS", error);
				m_pSolidPS = vkdShader::Create(&Device(), VK_SHADER_STAGE_FRAGMENT_BIT, g_gtSolidPS, sizeof(g_gtSolidPS), "PSMain", "gtSolid PS", error);
				m_pProtoVS = vkdShader::Create(&Device(), VK_SHADER_STAGE_VERTEX_BIT, g_gtBindingProtoVS, sizeof(g_gtBindingProtoVS), "VSMain", "gtBindingProto VS", error);
				m_pProtoPS = vkdShader::Create(&Device(), VK_SHADER_STAGE_FRAGMENT_BIT, g_gtBindingProtoPS, sizeof(g_gtBindingProtoPS), "PSMain", "gtBindingProto PS", error);
				ASSERT_TRUE(m_pSolidVS && m_pSolidPS && m_pProtoVS && m_pProtoPS) << error;
				m_pLayout.reset(new vkdVertexLayout({ { "POSITION", 0, VK_FORMAT_R32G32B32_SFLOAT, 0, 0 },
													  { "COLOR", 0, VK_FORMAT_R32G32B32A32_SFLOAT, 0, 12 } }, { sizeof(Vertex) }));
			}

			void TearDown() override
			{
				m_Images.clear();
				m_pSolidVS.reset();
				m_pSolidPS.reset();
				m_pProtoVS.reset();
				m_pProtoPS.reset();
				m_pContext.reset();
				m_pPipelines.reset();
				m_pRing.reset();
				m_pBindings.reset();
				gtVulkan::TearDown();
			}

			vkdImage* Target(VkFormat i_Format = VK_FORMAT_R8G8B8A8_UNORM)
			{
				const bool depth = (i_Format == VK_FORMAT_D24_UNORM_S8_UINT);
				const VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
					(depth ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
				m_Images.push_back(vkdImage::Create2D(Device(), c_Size, c_Size, i_Format, usage, "vkdContextTest target",
					depth ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT));
				return m_Images.back().get();
			}

			//	Render targets, full viewport, solid shaders and layout, cleared to blue
			void Begin(vkdImage* i_pColor, vkdImage* i_pDepth = nullptr)
			{
				vkdContext& c = *m_pContext;
				c.SetRenderTargets(1, &i_pColor, i_pDepth);
				vkdViewport viewport;
				viewport.m_Width = (float)c_Size;
				viewport.m_Height = (float)c_Size;
				c.SetViewports(1, &viewport);
				const vkdShader* stages[5] = { m_pSolidVS.get(), nullptr, nullptr, nullptr, m_pSolidPS.get() };
				c.SetShaders(stages);
				c.SetVertexLayout(m_pLayout.get());
				c.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
				const float blue[4] = { 0, 0, 1, 1 };
				c.ClearRenderTarget(i_pColor, blue);
				const float one[4] = { 1, 1, 1, 1 };
				vkdRingBuffer::Allocation tint = c.Upload(one, sizeof(one));
				c.Bindings().SetConstantBuffer(0, tint.m_Gpu, sizeof(one));
			}

			void DrawVertices(const Vertex* i_pVertices, uint32_t i_Count)
			{
				vkdRingBuffer::Allocation vb = m_pContext->Upload(i_pVertices, i_Count * sizeof(Vertex), 16);
				m_pContext->SetVertexBuffer(0, vb.m_Buffer, vb.m_Offset);
				m_pContext->Draw(i_Count, 0);
			}

			std::vector<uint8_t> Read(vkdImage* i_pImage)
			{
				m_pContext->Flush();
				return i_pImage->ReadBack();
			}

			std::unique_ptr<vkdBindings>		m_pBindings;
			std::unique_ptr<vkdRingBuffer>		m_pRing;
			std::unique_ptr<vkdPipelineCache>	m_pPipelines;
			std::unique_ptr<vkdContext>			m_pContext;
			std::unique_ptr<vkdShader>			m_pSolidVS, m_pSolidPS, m_pProtoVS, m_pProtoPS;
			std::unique_ptr<vkdVertexLayout>	m_pLayout;
			std::vector<std::unique_ptr<vkdImage>> m_Images;
	};

	//------------------------------------------------------------------------
	//	The D3D11 side: the same HLSL (gtSolid.hlsl) compiled with fxc
	//------------------------------------------------------------------------
	class D3DSolid
	{
		public:
			bool Create(gtD3D11& i_D3D, std::string& o_Error)
			{
				m_pD3D = &i_D3D;
				const std::string root = gtPlatform::RepoRoot();
				const std::wstring file = std::wstring(root.begin(), root.end()) +
										  L"UnitTests\\GraphicsTests\\Vk\\Shaders\\gtSolid.hlsl";
				Microsoft::WRL::ComPtr<ID3DBlob> vs, ps, errors;
				if (FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "VSMain", "vs_5_0", 0, 0, &vs, &errors)) ||
					FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "PSMain", "ps_5_0", 0, 0, &ps, &errors)))
				{
					o_Error = errors ? (const char*)errors->GetBufferPointer() : "can't compile gtSolid.hlsl";
					return false;
				}
				ID3D11Device* d = i_D3D.Device();
				d->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &m_pVS);
				d->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &m_pPS);
				const D3D11_INPUT_ELEMENT_DESC layout[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				d->CreateInputLayout(layout, 2, vs->GetBufferPointer(), vs->GetBufferSize(), &m_pLayout);
				D3D11_TEXTURE2D_DESC desc = {};
				desc.Width = desc.Height = c_Size;
				desc.MipLevels = desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.BindFlags = D3D11_BIND_RENDER_TARGET;
				d->CreateTexture2D(&desc, NULL, &m_pTarget);
				d->CreateRenderTargetView(m_pTarget.Get(), NULL, &m_pView);
				desc.BindFlags = 0;
				desc.Usage = D3D11_USAGE_STAGING;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				d->CreateTexture2D(&desc, NULL, &m_pStaging);
				const float one[4] = { 1, 1, 1, 1 };
				D3D11_BUFFER_DESC cb = { 16, D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
				D3D11_SUBRESOURCE_DATA init = { one, 0, 0 };
				d->CreateBuffer(&cb, &init, &m_pTint);
				return m_pVS && m_pPS && m_pLayout && m_pView && m_pStaging && m_pTint;
			}

			//	Clear to blue, draw i_pVertices with the rasterizer state, read back
			std::vector<uint8_t> Draw(const Vertex* i_pVertices, uint32_t i_Count, const D3D11_RASTERIZER_DESC& i_Raster)
			{
				ID3D11Device* d = m_pD3D->Device();
				ID3D11DeviceContext* c = m_pD3D->Context();
				c->ClearState();
				const float blue[4] = { 0, 0, 1, 1 };
				c->ClearRenderTargetView(m_pView.Get(), blue);
				c->OMSetRenderTargets(1, m_pView.GetAddressOf(), NULL);
				D3D11_VIEWPORT viewport = { 0, 0, (float)c_Size, (float)c_Size, 0, 1 };
				c->RSSetViewports(1, &viewport);
				Microsoft::WRL::ComPtr<ID3D11RasterizerState> raster;
				d->CreateRasterizerState(&i_Raster, &raster);
				c->RSSetState(raster.Get());
				D3D11_BUFFER_DESC vbDesc = { i_Count * (UINT)sizeof(Vertex), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
				D3D11_SUBRESOURCE_DATA init = { i_pVertices, 0, 0 };
				Microsoft::WRL::ComPtr<ID3D11Buffer> vb;
				d->CreateBuffer(&vbDesc, &init, &vb);
				UINT stride = sizeof(Vertex), offset = 0;
				c->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
				c->IASetInputLayout(m_pLayout.Get());
				c->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				c->VSSetShader(m_pVS.Get(), NULL, 0);
				c->PSSetShader(m_pPS.Get(), NULL, 0);
				c->PSSetConstantBuffers(0, 1, m_pTint.GetAddressOf());
				c->Draw(i_Count, 0);
				c->CopyResource(m_pStaging.Get(), m_pTarget.Get());
				D3D11_MAPPED_SUBRESOURCE mapped;
				std::vector<uint8_t> image(c_Size * c_Size * 4);
				if (SUCCEEDED(c->Map(m_pStaging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
				{
					for (uint32_t y = 0; y < c_Size; y++)
					{
						memcpy(&image[y * c_Size * 4], (const uint8_t*)mapped.pData + y * mapped.RowPitch, c_Size * 4);
					}
					c->Unmap(m_pStaging.Get(), 0);
				}
				return image;
			}

		private:
			gtD3D11* m_pD3D = nullptr;
			Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_pVS;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_pPS;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>		m_pLayout;
			Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_pTarget, m_pStaging;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	m_pView;
			Microsoft::WRL::ComPtr<ID3D11Buffer>			m_pTint;
	};

	D3D11_RASTERIZER_DESC d3d_raster(D3D11_CULL_MODE i_Cull, bool i_bFrontCCW)
	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = i_Cull;
		desc.FrontCounterClockwise = i_bFrontCCW;
		desc.DepthClipEnable = TRUE;
		return desc;
	}

	size_t differing_pixels(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b)
	{
		size_t count = 0;
		for (size_t i = 0; i + 3 < a.size() && i + 3 < b.size(); i += 4)
		{
			count += memcmp(&a[i], &b[i], 4) != 0;
		}
		return count + (a.size() != b.size() ? 1 : 0);
	}
}

TEST_F(vkdContextTest, ClearAndDrawTriangle)
{
	vkdImage* target = Target();
	Begin(target);
	const float red[4] = { 1, 0, 0, 1 };
	const std::vector<Vertex> full = quad(0.5f, red);
	DrawVertices(full.data(), (uint32_t)full.size());
	std::vector<uint8_t> image = Read(target);
	EXPECT_EQ(c_Red, pixel(image, 0, 0));
	EXPECT_EQ(c_Red, pixel(image, 63, 63));

	// a viewport of the left half: only that half is drawn
	Begin(target);
	vkdViewport left;
	left.m_Width = c_Size / 2.0f;
	left.m_Height = (float)c_Size;
	m_pContext->SetViewports(1, &left);
	DrawVertices(full.data(), (uint32_t)full.size());
	image = Read(target);
	EXPECT_EQ(c_Red, pixel(image, 0, 32));
	EXPECT_EQ(c_Red, pixel(image, 31, 32));
	EXPECT_EQ(c_Blue, pixel(image, 32, 32));
}

TEST_F(vkdContextTest, ViewportMatchesD3D)
{
	GT_D3D11(d3d);
	D3DSolid solid;
	std::string error;
	ASSERT_TRUE(solid.Create(d3d, error)) << error;

	//	An asymmetric triangle (top-left quadrant), and a quad with an offset viewport
	const Vertex corner[] =
	{
		{ { -1, 1, 0.5f }, { 1, 0, 0, 1 } }, { { 0, 1, 0.5f }, { 1, 0, 0, 1 } }, { { -1, 0, 0.5f }, { 1, 0, 0, 1 } },
	};
	vkdImage* target = Target();
	m_pContext->SetRasterizerState([] { vkdRasterizerState r; r.m_Cull = VK_CULL_MODE_NONE; return r; }());
	Begin(target);
	DrawVertices(corner, 3);
	const std::vector<uint8_t> vk = Read(target);
	const std::vector<uint8_t> dx = solid.Draw(corner, 3, d3d_raster(D3D11_CULL_NONE, false));
	EXPECT_EQ(0u, differing_pixels(dx, vk)) << "D3D adapter: " << d3d.AdapterName();
	EXPECT_EQ(c_Red, pixel(vk, 2, 2));			// top left is covered
	EXPECT_EQ(c_Blue, pixel(vk, 2, 61));		// bottom left is not
}

TEST_F(vkdContextTest, CullingMatchesD3D)
{
	GT_D3D11(d3d);
	D3DSolid solid;
	std::string error;
	ASSERT_TRUE(solid.Create(d3d, error)) << error;

	const struct { D3D11_CULL_MODE m_D3D; VkCullModeFlags m_Vk; } culls[] =
	{
		{ D3D11_CULL_NONE, VK_CULL_MODE_NONE }, { D3D11_CULL_FRONT, VK_CULL_MODE_FRONT_BIT }, { D3D11_CULL_BACK, VK_CULL_MODE_BACK_BIT },
	};
	vkdImage* target = Target();
	for (const auto& cull : culls)
	{
		for (bool ccw : { false, true })
		{
			vkdRasterizerState raster;
			raster.m_Cull = cull.m_Vk;
			raster.m_bFrontCCW = ccw;
			m_pContext->SetRasterizerState(raster);
			Begin(target);
			DrawVertices(c_Wound, 6);
			const std::vector<uint8_t> vk = Read(target);
			const std::vector<uint8_t> dx = solid.Draw(c_Wound, 6, d3d_raster(cull.m_D3D, ccw));
			EXPECT_EQ(0u, differing_pixels(dx, vk)) << "cull " << cull.m_D3D << " FrontCounterClockwise " << ccw;
		}
	}
}

TEST_F(vkdContextTest, DepthTestAndBias)
{
	vkdImage* color = Target();
	vkdImage* depth = Target(VK_FORMAT_D24_UNORM_S8_UINT);
	const float red[4] = { 1, 0, 0, 1 }, green[4] = { 0, 1, 0, 1 };
	const std::vector<Vertex> front = quad(0.5f, red), back = quad(0.6f, green), same = quad(0.5f, green);

	Begin(color, depth);
	m_pContext->ClearDepthStencil(depth, true, 1.0f, true, 0);
	DrawVertices(front.data(), 6);
	DrawVertices(back.data(), 6);			// LESS: behind, hidden
	EXPECT_EQ(c_Red, pixel(Read(color), 32, 32));

	Begin(color, depth);
	m_pContext->ClearDepthStencil(depth, true, 1.0f, true, 0);
	DrawVertices(front.data(), 6);
	vkdDepthStencilState greater;
	greater.m_DepthFunc = VK_COMPARE_OP_GREATER;
	m_pContext->SetDepthStencilState(greater, 0);
	DrawVertices(back.data(), 6);			// GREATER: shown
	EXPECT_EQ(c_Green, pixel(Read(color), 32, 32));

	m_pContext->SetDepthStencilState(vkdDepthStencilState(), 0);
	Begin(color, depth);
	m_pContext->ClearDepthStencil(depth, true, 1.0f, true, 0);
	DrawVertices(front.data(), 6);
	DrawVertices(same.data(), 6);			// equal depth, LESS: hidden
	EXPECT_EQ(c_Red, pixel(Read(color), 32, 32));

	Begin(color, depth);
	m_pContext->ClearDepthStencil(depth, true, 1.0f, true, 0);
	DrawVertices(front.data(), 6);
	vkdRasterizerState biased;
	biased.m_Cull = VK_CULL_MODE_NONE;
	biased.m_DepthBias = -1000;			// pulled forward: shown
	m_pContext->SetRasterizerState(biased);
	DrawVertices(same.data(), 6);
	EXPECT_EQ(c_Green, pixel(Read(color), 32, 32));
	m_pContext->SetRasterizerState(vkdRasterizerState());
}

TEST_F(vkdContextTest, StencilRef)
{
	vkdImage* color = Target();
	vkdImage* depth = Target(VK_FORMAT_D24_UNORM_S8_UINT);
	const float red[4] = { 1, 0, 0, 1 }, green[4] = { 0, 1, 0, 1 };
	Begin(color, depth);
	m_pContext->ClearDepthStencil(depth, true, 1.0f, true, 0);

	vkdDepthStencilState write;
	write.m_bDepth = false;
	write.m_bStencil = true;
	write.m_Front.m_Pass = write.m_Back.m_Pass = VK_STENCIL_OP_REPLACE;
	m_pContext->SetDepthStencilState(write, 5);
	const std::vector<Vertex> left = quad(0.5f, red, -1, 0);
	DrawVertices(left.data(), 6);			// stencil 5 on the left half

	vkdDepthStencilState test = write;
	test.m_Front.m_Pass = test.m_Back.m_Pass = VK_STENCIL_OP_KEEP;
	test.m_Front.m_Func = test.m_Back.m_Func = VK_COMPARE_OP_EQUAL;
	m_pContext->SetDepthStencilState(test, 5);
	const std::vector<Vertex> full = quad(0.5f, green);
	DrawVertices(full.data(), 6);			// only where stencil == 5
	m_pContext->SetDepthStencilState(vkdDepthStencilState(), 0);

	const std::vector<uint8_t> image = Read(color);
	EXPECT_EQ(c_Green, pixel(image, 10, 32));
	EXPECT_EQ(c_Blue, pixel(image, 50, 32));
}

TEST_F(vkdContextTest, ScissorOnlyWhenEnabled)
{
	vkdImage* target = Target();
	const float red[4] = { 1, 0, 0, 1 };
	const std::vector<Vertex> full = quad(0.5f, red);
	const VkRect2D left = { { 0, 0 }, { c_Size / 2, c_Size } };
	m_pContext->SetScissorRects(1, &left);

	Begin(target);
	DrawVertices(full.data(), 6);			// scissor test off: everything
	EXPECT_EQ(c_Red, pixel(Read(target), 50, 32));

	vkdRasterizerState scissor;
	scissor.m_bScissor = true;
	m_pContext->SetRasterizerState(scissor);
	Begin(target);
	DrawVertices(full.data(), 6);			// on: the left half only
	const std::vector<uint8_t> image = Read(target);
	EXPECT_EQ(c_Red, pixel(image, 10, 32));
	EXPECT_EQ(c_Blue, pixel(image, 50, 32));
	m_pContext->SetRasterizerState(vkdRasterizerState());
}

//	Draw red into A, then draw B by sampling A (gtBindingProto: tex0 * scale + tex1.Load * bias)
TEST_F(vkdContextTest, RenderThenSampleIsOrdered)
{
	vkdImage* a = Target();
	vkdImage* b = Target();
	const float red[4] = { 1, 0, 0, 1 };
	const std::vector<Vertex> full = quad(0.5f, red);
	Begin(a);
	DrawVertices(full.data(), 6);

	vkdContext& c = *m_pContext;
	c.SetRenderTargets(1, &b, nullptr);
	const vkdShader* stages[5] = { m_pProtoVS.get(), nullptr, nullptr, nullptr, m_pProtoPS.get() };
	c.SetShaders(stages);
	c.SetVertexLayout(nullptr);
	const float scale[4] = { 1, 1, 1, 1 }, bias[4] = { 0, 0, 0, 0 };
	c.Bindings().SetConstantBuffer(0, c.Upload(scale, 16).m_Gpu, 16);
	c.Bindings().SetConstantBuffer(3, c.Upload(bias, 16).m_Gpu, 16);
	c.Bindings().SetTexture(0, a->SampledIndex(*m_pBindings));
	c.Bindings().SetTexture(37, a->SampledIndex(*m_pBindings));
	VkSamplerCreateInfo point = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
	c.Bindings().SetSampler(13, m_pBindings->AddSampler(point));
	c.Draw(3, 0);
	const std::vector<uint8_t> image = Read(b);
	EXPECT_EQ(c_Red, pixel(image, 0, 0));
	EXPECT_EQ(c_Red, pixel(image, 63, 63));
}

//	A clear outside rendering (a transfer) is ordered with the draws around it
TEST_F(vkdContextTest, ClearOutsideRenderingIsOrdered)
{
	vkdImage* a = Target();
	vkdImage* b = Target();
	const float red[4] = { 1, 0, 0, 1 }, green[4] = { 0, 1, 0, 1 };
	const std::vector<Vertex> full = quad(0.5f, red);
	Begin(a);
	DrawVertices(full.data(), 6);
	m_pContext->SetRenderTargets(1, &b, nullptr);
	m_pContext->ClearRenderTarget(a, green);		// a isn't bound: a transfer clear after the draw into it
	EXPECT_EQ(c_Green, pixel(Read(a), 32, 32));
}

TEST_F(vkdContextTest, StateChangesReuseThePipeline)
{
	vkdImage* color = Target();
	vkdImage* depth = Target(VK_FORMAT_D24_UNORM_S8_UINT);
	const float red[4] = { 1, 0, 0, 1 };
	const std::vector<Vertex> full = quad(0.5f, red);
	Begin(color, depth);
	DrawVertices(full.data(), 6);
	const uint64_t created = m_pPipelines->Creations();

	vkdRasterizerState raster;
	raster.m_Cull = VK_CULL_MODE_FRONT_BIT;
	raster.m_bFrontCCW = true;
	raster.m_DepthBias = 10;
	m_pContext->SetRasterizerState(raster);
	DrawVertices(full.data(), 6);
	vkdDepthStencilState ds;
	ds.m_DepthFunc = VK_COMPARE_OP_ALWAYS;
	ds.m_bStencil = true;
	ds.m_Front.m_Pass = VK_STENCIL_OP_INCREMENT_AND_CLAMP;
	m_pContext->SetDepthStencilState(ds, 3);
	DrawVertices(full.data(), 6);
	m_pContext->SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
	DrawVertices(full.data(), 4);
	const float factor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	m_pContext->SetBlendState(vkdBlendState(), factor);
	DrawVertices(full.data(), 4);
	EXPECT_EQ(created, m_pPipelines->Creations());

	vkdRasterizerState wire;
	wire.m_Fill = VK_POLYGON_MODE_LINE;
	m_pContext->SetRasterizerState(wire);
	DrawVertices(full.data(), 4);			// fill mode is baked in: one more
	EXPECT_EQ(created + 1, m_pPipelines->Creations());
	m_pContext->ResetState();
	Read(color);
}

TEST_F(vkdContextTest, ResetStateRestoresDefaults)
{
	vkdImage* color = Target();
	Begin(color);
	vkdRasterizerState raster;
	raster.m_Cull = VK_CULL_MODE_NONE;
	m_pContext->SetRasterizerState(raster);
	vkdDepthStencilState ds;
	ds.m_bDepth = false;
	m_pContext->SetDepthStencilState(ds, 7);
	vkdBlendState blend;
	blend.m_Targets[0].m_bEnable = true;
	const float factor[4] = { 1, 1, 1, 1 };
	m_pContext->SetBlendState(blend, factor);

	m_pContext->ResetState();
	EXPECT_EQ(VK_CULL_MODE_BACK_BIT, m_pContext->GetRasterizerState().m_Cull);
	EXPECT_TRUE(m_pContext->GetDepthStencilState().m_bDepth);
	EXPECT_EQ(0u, m_pContext->GetStencilRef());
	EXPECT_FALSE(m_pContext->GetBlendState().m_Targets[0].m_bEnable);
	vkdViewport viewports[vkdContext::c_MaxViewports];
	EXPECT_EQ(0u, m_pContext->GetViewports(viewports));
	vkdImage* colors[8];
	vkdImage* depth = color;
	EXPECT_EQ(0u, m_pContext->GetRenderTargets(colors, &depth));
	EXPECT_EQ(nullptr, depth);
	m_pContext->Flush();
}

TEST_F(vkdContextTest, DestroyedTargetsAreUnbound)
{
	//	D3D11 keeps a bound target alive; here the context forgets an image
	//	destroyed while bound, so it never begins rendering with it (a new
	//	image at the same address would otherwise look bound)
	std::unique_ptr<vkdImage> color = vkdImage::Create2D(Device(), c_Size, c_Size, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, "destroyed color");
	std::unique_ptr<vkdImage> depth = vkdImage::Create2D(Device(), c_Size, c_Size, VK_FORMAT_D24_UNORM_S8_UINT,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, "destroyed depth",
		VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
	const float red[4] = { 1, 0, 0, 1 };
	const std::vector<Vertex> front = quad(0.5f, red);
	Begin(color.get(), depth.get());
	m_pContext->ClearDepthStencil(depth.get(), true, 1.0f, true, 0);
	DrawVertices(front.data(), 6);			// rendering is open on both
	depth.reset();
	color.reset();

	vkdImage* colors[8] = {};
	vkdImage* bound_depth = nullptr;
	const uint32_t count = m_pContext->GetRenderTargets(colors, &bound_depth);
	EXPECT_EQ(nullptr, bound_depth);
	for (uint32_t c = 0; c < count; c++)
		EXPECT_EQ(nullptr, colors[c]) << c;

	//	drawing goes on into new targets
	vkdImage* next = Target();
	Begin(next);
	DrawVertices(front.data(), 6);
	EXPECT_EQ(c_Red, pixel(Read(next), 32, 32));
}

namespace
{
	//	Layered rendering: three c_LayerSize x c_LayerSize x i_Slices
	//	R32G32B32A32_FLOAT volumes, as LPV's
	const uint32_t c_LayerSize = 8, c_LayerTargets = 3;
	typedef std::vector<std::vector<float>> Volumes;		// texels of each target, slice by slice

	struct LayeredConstants
	{
		uint32_t m_Slices, m_bPast, m_Pad[2];
	};

	//	gtLayered.hlsl compiled with fxc, drawn once into three volumes
	//	cleared to i_Clear (render target views of the whole volume, as
	//	shdwPassLPVGI makes them), read back
	bool d3d_layered(gtD3D11& i_D3D, uint32_t i_Slices, bool i_bPast, const float i_Clear[4], Volumes& o_Volumes,
					 std::string& o_Error)
	{
		using Microsoft::WRL::ComPtr;
		const std::string root = gtPlatform::RepoRoot();
		const std::wstring file = std::wstring(root.begin(), root.end()) +
								  L"UnitTests\\GraphicsTests\\Vk\\Shaders\\gtLayered.hlsl";
		ComPtr<ID3DBlob> vs, gs, ps, errors;
		if (FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "VSMain", "vs_5_0", 0, 0, &vs, &errors)) ||
			FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "GSMain", "gs_5_0", 0, 0, &gs, &errors)) ||
			FAILED(D3DCompileFromFile(file.c_str(), NULL, NULL, "PSMain", "ps_5_0", 0, 0, &ps, &errors)))
		{
			o_Error = errors ? (const char*)errors->GetBufferPointer() : "can't compile gtLayered.hlsl";
			return false;
		}
		ID3D11Device* d = i_D3D.Device();
		ID3D11DeviceContext* c = i_D3D.Context();
		ComPtr<ID3D11VertexShader> vertex;
		ComPtr<ID3D11GeometryShader> geometry;
		ComPtr<ID3D11PixelShader> pixel_shader;
		d->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &vertex);
		d->CreateGeometryShader(gs->GetBufferPointer(), gs->GetBufferSize(), NULL, &geometry);
		d->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &pixel_shader);
		D3D11_TEXTURE3D_DESC desc = { c_LayerSize, c_LayerSize, i_Slices, 1, DXGI_FORMAT_R32G32B32A32_FLOAT,
									  D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET, 0, 0 };
		ComPtr<ID3D11Texture3D> volumes[c_LayerTargets];
		ComPtr<ID3D11RenderTargetView> views[c_LayerTargets];
		ID3D11RenderTargetView* bound[c_LayerTargets];
		for (uint32_t t = 0; t < c_LayerTargets; t++)
		{
			d->CreateTexture3D(&desc, NULL, &volumes[t]);
			if (volumes[t])
				d->CreateRenderTargetView(volumes[t].Get(), NULL, &views[t]);
			if (!views[t])
			{
				o_Error = "can't make the volumes";
				return false;
			}
			bound[t] = views[t].Get();
		}
		const LayeredConstants constants = { i_Slices, i_bPast ? 1u : 0u, { 0, 0 } };
		D3D11_BUFFER_DESC cb = { sizeof(constants), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
		D3D11_SUBRESOURCE_DATA init = { &constants, 0, 0 };
		ComPtr<ID3D11Buffer> buffer;
		d->CreateBuffer(&cb, &init, &buffer);

		c->ClearState();
		for (uint32_t t = 0; t < c_LayerTargets; t++)
			c->ClearRenderTargetView(bound[t], i_Clear);
		c->OMSetRenderTargets(c_LayerTargets, bound, NULL);
		D3D11_VIEWPORT viewport = { 0, 0, (float)c_LayerSize, (float)c_LayerSize, 0, 1 };
		c->RSSetViewports(1, &viewport);
		c->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		c->VSSetShader(vertex.Get(), NULL, 0);
		c->GSSetShader(geometry.Get(), NULL, 0);
		c->GSSetConstantBuffers(0, 1, buffer.GetAddressOf());
		c->PSSetShader(pixel_shader.Get(), NULL, 0);
		c->Draw(3, 0);

		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		o_Volumes.assign(c_LayerTargets, std::vector<float>());
		const size_t row = c_LayerSize * 4;			// floats
		for (uint32_t t = 0; t < c_LayerTargets; t++)
		{
			ComPtr<ID3D11Texture3D> staging;
			d->CreateTexture3D(&desc, NULL, &staging);
			c->CopyResource(staging.Get(), volumes[t].Get());
			D3D11_MAPPED_SUBRESOURCE mapped;
			if (!staging || FAILED(c->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
			{
				o_Error = "can't map the volume";
				return false;
			}
			o_Volumes[t].resize(row * c_LayerSize * i_Slices);
			for (uint32_t z = 0; z < i_Slices; z++)
			{
				for (uint32_t y = 0; y < c_LayerSize; y++)
				{
					memcpy(&o_Volumes[t][(z * c_LayerSize + y) * row],
						   (const uint8_t*)mapped.pData + z * mapped.DepthPitch + y * mapped.RowPitch, row * sizeof(float));
				}
			}
			c->Unmap(staging.Get(), 0);
		}
		c->ClearState();
		return true;
	}

	class vkdContextLayeredTest : public vkdContextTest
	{
		protected:
			void SetUp() override
			{
				vkdContextTest::SetUp();
				if (!m_pDevice)
				{
					return;
				}
				std::string error;
				m_pLayeredVS = vkdShader::Create(&Device(), VK_SHADER_STAGE_VERTEX_BIT, g_gtLayeredVS, sizeof(g_gtLayeredVS), "VSMain", "gtLayered VS", error);
				m_pLayeredGS = vkdShader::Create(&Device(), VK_SHADER_STAGE_GEOMETRY_BIT, g_gtLayeredGS, sizeof(g_gtLayeredGS), "GSMain", "gtLayered GS", error);
				m_pLayeredPS = vkdShader::Create(&Device(), VK_SHADER_STAGE_FRAGMENT_BIT, g_gtLayeredPS, sizeof(g_gtLayeredPS), "PSMain", "gtLayered PS", error);
				ASSERT_TRUE(m_pLayeredVS && m_pLayeredGS && m_pLayeredPS) << error;
			}

			void TearDown() override
			{
				m_pLayeredVS.reset();
				m_pLayeredGS.reset();
				m_pLayeredPS.reset();
				vkdContextTest::TearDown();
			}

			vkdImage* Volume(uint32_t i_Slices)
			{
				vkdImageDesc desc;
				desc.m_Type = VK_IMAGE_TYPE_3D;
				desc.m_Format = VK_FORMAT_R32G32B32A32_SFLOAT;
				desc.m_Width = desc.m_Height = c_LayerSize;
				desc.m_Depth = i_Slices;
				desc.m_Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
							   VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
				desc.m_bLayeredTarget = true;
				m_Images.push_back(vkdImage::Create(Device(), desc, "vkdContextTest volume"));
				return m_Images.back().get();
			}

			//	Binds i_Count targets with a c_LayerSize viewport and gtLayered's
			//	shaders, and draws once
			void DrawLayered(uint32_t i_Count, vkdImage* const* i_ppTargets, uint32_t i_Slices, bool i_bPast)
			{
				vkdContext& c = *m_pContext;
				c.SetRenderTargets(i_Count, i_ppTargets, nullptr);
				vkdViewport viewport;
				viewport.m_Width = viewport.m_Height = (float)c_LayerSize;
				c.SetViewports(1, &viewport);
				const vkdShader* stages[5] = { m_pLayeredVS.get(), nullptr, nullptr, m_pLayeredGS.get(), m_pLayeredPS.get() };
				c.SetShaders(stages);
				c.SetVertexLayout(nullptr);
				c.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
				const LayeredConstants constants = { i_Slices, i_bPast ? 1u : 0u, { 0, 0 } };
				vkdRingBuffer::Allocation cb = c.Upload(&constants, sizeof(constants));
				c.Bindings().SetConstantBuffer(0, cb.m_Gpu, sizeof(constants));
				c.Draw(3, 0);
			}

			std::vector<float> ReadFloats(vkdImage* i_pImage)
			{
				const std::vector<uint8_t> bytes = Read(i_pImage);
				std::vector<float> floats(bytes.size() / sizeof(float));
				memcpy(floats.data(), bytes.data(), floats.size() * sizeof(float));
				return floats;
			}

			//	The Vulkan side of d3d_layered: the first volume cleared before
			//	it is bound (outside rendering), the others while bound
			Volumes Layered(uint32_t i_Slices, bool i_bPast, const float i_Clear[4])
			{
				vkdImage* views[c_LayerTargets];
				vkdImage* volumes[c_LayerTargets];
				for (uint32_t t = 0; t < c_LayerTargets; t++)
				{
					volumes[t] = Volume(i_Slices);
					views[t] = volumes[t]->LayeredView();
				}
				m_pContext->ClearRenderTarget(views[0], i_Clear);
				m_pContext->SetRenderTargets(c_LayerTargets, views, nullptr);
				for (uint32_t t = 1; t < c_LayerTargets; t++)
					m_pContext->ClearRenderTarget(views[t], i_Clear);
				DrawLayered(c_LayerTargets, views, i_Slices, i_bPast);
				Volumes result;
				for (uint32_t t = 0; t < c_LayerTargets; t++)
					result.push_back(ReadFloats(volumes[t]));
				m_pContext->SetRenderTargets(0, nullptr, nullptr);
				return result;
			}

			std::unique_ptr<vkdShader>	m_pLayeredVS, m_pLayeredGS, m_pLayeredPS;
	};

	//	The first texel of each slice of target i_Target, as text
	std::string slices(const Volumes& i_Volumes, uint32_t i_Target, uint32_t i_Slices)
	{
		std::ostringstream text;
		for (uint32_t z = 0; z < i_Slices && (z + 1) * c_LayerSize * c_LayerSize * 4 <= i_Volumes[i_Target].size(); z++)
		{
			const float* t = &i_Volumes[i_Target][z * c_LayerSize * c_LayerSize * 4];
			text << " [" << t[0] << " " << t[1] << " " << t[2] << " " << t[3] << "]";
		}
		return text.str();
	}

	void expect_same_volumes(const Volumes& i_DX, const Volumes& i_Vk, uint32_t i_Slices)
	{
		ASSERT_EQ(c_LayerTargets, i_Vk.size());
		for (uint32_t t = 0; t < c_LayerTargets; t++)
		{
			ASSERT_EQ(i_DX[t].size(), i_Vk[t].size()) << "target " << t;
			EXPECT_TRUE(i_DX[t] == i_Vk[t]) << "target " << t << ": D3D11" << slices(i_DX, t, i_Slices)
											<< ", Vulkan" << slices(i_Vk, t, i_Slices);
		}
	}
}

TEST_F(vkdContextLayeredTest, LayeredRenderingMatchesD3D11)
{
	GT_D3D11(d3d);
	const uint32_t c_Slices = 4;
	const float clear[4] = { 0.25f, 0.5f, 0.75f, 1.0f };
	Volumes dx;
	std::string error;
	ASSERT_TRUE(d3d_layered(d3d, c_Slices, false, clear, dx, error)) << error;
	const Volumes vk = Layered(c_Slices, false, clear);
	expect_same_volumes(dx, vk, c_Slices);
	//	and every slice was drawn: slice z of target 0 is (z, 0, 0, 1)
	for (uint32_t z = 0; z < c_Slices; z++)
	{
		const float* texel = &vk[0][(z * c_LayerSize * c_LayerSize + 37) * 4];
		EXPECT_EQ((float)z, texel[0]) << "slice " << z;
		EXPECT_EQ(0.0f, texel[1]) << "slice " << z;
		EXPECT_EQ(1.0f, texel[3]) << "slice " << z;
	}
}

TEST_F(vkdContextLayeredTest, LayerPastTheLastSliceMatchesD3D11)
{
	//	LPV's injection rounds a VPL's slice up to the slice count when it
	//	lies in the last half slice
	GT_D3D11(d3d);
	const uint32_t c_Slices = 4;
	const float clear[4] = { 0.25f, 0.5f, 0.75f, 1.0f };
	Volumes dx;
	std::string error;
	ASSERT_TRUE(d3d_layered(d3d, c_Slices, true, clear, dx, error)) << error;
	const Volumes vk = Layered(c_Slices, true, clear);
	expect_same_volumes(dx, vk, c_Slices);
	//	D3D11 renders it into slice 0 (Vulkan leaves a Layer past the last
	//	one undefined: this checks the driver does as D3D11 does)
	const float* texel = &dx[0][37 * 4];
	EXPECT_EQ((float)c_Slices, texel[0]) << "D3D11" << slices(dx, 0, c_Slices);
}

TEST_F(vkdContextLayeredTest, TargetsWithDifferentLayerCountsAreRefused)
{
	//	D3D11 doesn't render into views of different array sizes; nor does
	//	vkdContext (the draw is skipped and logged)
	vkdImage* volume = Volume(4);
	std::unique_ptr<vkdImage> flat = vkdImage::Create2D(Device(), c_LayerSize, c_LayerSize, VK_FORMAT_R32G32B32A32_SFLOAT,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, "flat");
	const float clear[4] = { 0.25f, 0.5f, 0.75f, 1.0f };
	m_pContext->ClearRenderTarget(volume->LayeredView(), clear);
	m_pContext->ClearRenderTarget(flat.get(), clear);
	vkdImage* targets[2] = { volume->LayeredView(), flat.get() };
	DrawLayered(2, targets, 4, false);
	const std::vector<float> both[2] = { ReadFloats(volume), ReadFloats(flat.get()) };
	for (const std::vector<float>& texels : both)
	{
		ASSERT_FALSE(texels.empty());
		for (size_t i = 0; i < texels.size(); i += 4)
		{
			ASSERT_EQ(clear[0], texels[i]) << i;
			ASSERT_EQ(clear[3], texels[i + 3]) << i;
		}
	}
	m_pContext->SetRenderTargets(0, nullptr, nullptr);
}
