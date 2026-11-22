/*****************************************************************************
**  g2dFullscreenQuadTest.cpp
**
**      GraphicsVulkan's g2dFullscreenQuad against GraphicsDX11's, the same
**      calls on both backends: the textured quad (a texture stretched over
**      a viewport rectangle), and a draw with custom vertices followed by
**      default ones (the custom vertices are for that draw only).
**
**	Extra Large Technology
\****************************************************************************/
#include "Vk/gtVulkanContext.hpp"
#include "Dx11/gtD3D11.hpp"
#include "gtImage.hpp"

#ifndef GVK_G2DFULLSCREENQUAD_HPP
#include "GraphicsVulkan/G2d/g2dFullscreenQuad.hpp"
#endif
#ifndef G2D_FULLSCREENQUAD_HPP
#include "GraphicsDX11/G2d/g2dFullscreenQuad.hpp"
#endif

#include <vector>

namespace
{
	const int c_Width = 128, c_Height = 96;
	const float c_Clear[4] = { 0.1f, 0.2f, 0.3f, 1.0f };

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
				p[3] = 255;
			}
		}
		return rgba;
	}

	//	The left half of the target, as a triangle strip
	template <class V> void left_half(V (&o_Vertices)[4])
	{
		const float p[4][4] = { { -1, 1, 0.5f, 1 }, { -1, -1, 0.5f, 1 }, { 0, 1, 0.5f, 1 }, { 0, -1, 0.5f, 1 } };
		const float t[4][2] = { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } };
		for (int i = 0; i < 4; i++)
		{
			o_Vertices[i].pos = maVector4d(p[i][0], p[i][1], p[i][2], p[i][3]);
			o_Vertices[i].tex = maVector2d(t[i][0], t[i][1]);
		}
	}

	class g2dFullscreenQuadTest : public gtVulkanContext
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

			void Compare(const std::vector<uint8_t>& i_DX11, const std::vector<uint8_t>& i_Vulkan)
			{
				const gtCompareResult r = gtCompareImages(i_DX11.data(), i_Vulkan.data(), c_Width, c_Height);
				if (!r.m_bPass)
				{
					const std::string dir = gtSaveComparison(::testing::UnitTest::GetInstance()->current_test_info()->name(),
															 i_DX11.data(), i_Vulkan.data(), c_Width, c_Height);
					ADD_FAILURE() << r.Describe() << "; images in " << dir;
				}
			}
	};
}

TEST_F(g2dFullscreenQuadTest, TexturedQuadMatchesDX11)
{
	GT_D3D11(d3d);
	gtD3D11::Globals globals(d3d);
	const std::vector<uint8_t> texels = gradient(64);

	// DX11
	g2dFullscreenQuad::InitFullscreenQuad();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture = d3d.CreateTexture(64, 64, texels.data(), false, &view, NULL);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> target = d3d.CreateTexture(c_Width, c_Height, NULL, true, NULL, &rtv);
	d3d.Context()->ClearState();
	d3d.Context()->ClearRenderTargetView(rtv.Get(), c_Clear);
	d3d.Context()->OMSetRenderTargets(1, rtv.GetAddressOf(), NULL);
	g2dFullscreenQuad::DrawTexturedQuad11(view.Get(), 100, 80, 10, 6);
	const std::vector<uint8_t> dx = d3d.ReadBack(target.Get());
	g2dFullscreenQuad::CleanUpFullscreenQuad();

	// Vulkan
	gvk::vkdImage* vkTexture = Texture(64, 64, texels.data());
	gvk::vkdImage* vkTarget = Target(c_Width, c_Height);
	Context().SetRenderTargets(1, &vkTarget, nullptr);
	Context().ClearRenderTarget(vkTarget, c_Clear);
	gvk::g2dFullscreenQuad::DrawTexturedQuad(vkTexture, 100, 80, 10, 6);
	const std::vector<uint8_t> vk = Read(vkTarget);

	Compare(dx, vk);
	EXPECT_NE(dx[(40 * c_Width + 50) * 4], (uint8_t)(c_Clear[0] * 255 + 0.5f)) << "the quad wasn't drawn";
}

//	A draw with custom vertices, then a textured quad with the default ones
TEST_F(g2dFullscreenQuadTest, CustomVerticesRestored)
{
	GT_D3D11(d3d);
	gtD3D11::Globals globals(d3d);
	const std::vector<uint8_t> texels = gradient(64);

	// DX11
	g2dFullscreenQuad::InitFullscreenQuad();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture = d3d.CreateTexture(64, 64, texels.data(), false, &view, NULL);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> target = d3d.CreateTexture(c_Width, c_Height, NULL, true, NULL, &rtv);
	d3d.Context()->ClearState();
	d3d.Context()->ClearRenderTargetView(rtv.Get(), c_Clear);
	d3d.Context()->OMSetRenderTargets(1, rtv.GetAddressOf(), NULL);
	g2dFullscreenQuad::DrawTexturedQuad11(view.Get(), c_Width, c_Height);			// the texture over everything
	g2dFullscreenQuad::SCREEN_VERTEX dxLeft[4];
	left_half(dxLeft);
	g2dFullscreenQuad::DrawFullScreenQuad11(c_Width, c_Height, 0, 0, dxLeft);		// the left half, texture unbound: black
	g2dFullscreenQuad::DrawTexturedQuad11(view.Get(), 40, 30, 70, 50);			// default vertices again
	const std::vector<uint8_t> dx = d3d.ReadBack(target.Get());
	g2dFullscreenQuad::CleanUpFullscreenQuad();

	// Vulkan
	gvk::vkdImage* vkTexture = Texture(64, 64, texels.data());
	gvk::vkdImage* vkTarget = Target(c_Width, c_Height);
	Context().SetRenderTargets(1, &vkTarget, nullptr);
	Context().ClearRenderTarget(vkTarget, c_Clear);
	gvk::g2dFullscreenQuad::DrawTexturedQuad(vkTexture, c_Width, c_Height);
	gvk::g2dFullscreenQuad::SCREEN_VERTEX vkLeft[4];
	left_half(vkLeft);
	gvk::g2dFullscreenQuad::DrawFullScreenQuad(c_Width, c_Height, 0, 0, vkLeft);
	gvk::g2dFullscreenQuad::DrawTexturedQuad(vkTexture, 40, 30, 70, 50);
	const std::vector<uint8_t> vk = Read(vkTarget);

	Compare(dx, vk);
}
