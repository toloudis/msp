/*****************************************************************************
**  vkdImageTest.cpp
**
**      vkdImage for every texture shape: 2D with mips, cubes, volumes and
**      arrays in uncompressed and BC formats upload and read back
**      unchanged; GenerateMips matches D3D11's GenerateMips; views of one
**      face render and clear that face only; the depth view of a
**      depth-stencil image samples.
**
**	Extra Large Technology
\****************************************************************************/
#include "Vk/gtVulkanContext.hpp"
#include "Dx11/gtD3D11.hpp"
#include "gtImage.hpp"

#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif
#ifndef GVK_VKDSHADER_HPP
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#endif

#include "gtBindingProtoVS.h"
#include "gtBindingProtoPS.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

using namespace gvk;

namespace
{
	//	Deterministic bytes for subresource i_Seed
	std::vector<uint8_t> pattern(size_t i_Size, uint32_t i_Seed)
	{
		std::vector<uint8_t> bytes(i_Size);
		uint32_t x = 0x9E3779B9u * (i_Seed + 1);
		for (size_t i = 0; i < i_Size; i++)
		{
			x ^= x << 13; x ^= x >> 17; x ^= x << 5;
			bytes[i] = (uint8_t)(x >> 24);
		}
		return bytes;
	}

	//	Bytes of one tightly packed subresource of i_Width x i_Height x i_Depth
	size_t subresource_size(VkFormat i_Format, uint32_t i_Width, uint32_t i_Height, uint32_t i_Depth)
	{
		vkdFormatInfo info = vkdFormatInfoOf(i_Format);
		const size_t bw = (i_Width + info.m_BlockWidth - 1) / info.m_BlockWidth;
		const size_t bh = (i_Height + info.m_BlockHeight - 1) / info.m_BlockHeight;
		return bw * bh * i_Depth * info.m_BlockBytes;
	}

	size_t row_pitch(VkFormat i_Format, uint32_t i_Width)
	{
		vkdFormatInfo info = vkdFormatInfoOf(i_Format);
		return (i_Width + info.m_BlockWidth - 1) / info.m_BlockWidth * info.m_BlockBytes;
	}

	class vkdImageTest : public gtVulkanContext
	{
		protected:
			void TearDown() override
			{
				m_Owned.clear();
				gtVulkanContext::TearDown();
			}

			vkdImage* Make(const vkdImageDesc& i_Desc, const char* i_Name)
			{
				m_Owned.push_back(vkdImage::Create(Device(), i_Desc, i_Name));
				return m_Owned.back().get();
			}

			std::vector<std::unique_ptr<vkdImage>> m_Owned;
	};
}

TEST_F(vkdImageTest, UploadReadBackEveryShape)
{
	struct Shape { const char* m_Name; VkImageType m_Type; uint32_t m_W, m_H, m_D, m_Mips, m_Layers; bool m_bCube; };
	const Shape c_Shapes[] =
	{
		{ "2D with mips",	VK_IMAGE_TYPE_2D, 64, 32, 1, 7, 1, false },
		{ "cube",			VK_IMAGE_TYPE_2D, 16, 16, 1, 5, 6, true },
		{ "3D",				VK_IMAGE_TYPE_3D, 16, 8, 4, 3, 1, false },
		{ "2D array",		VK_IMAGE_TYPE_2D, 20, 12, 1, 2, 3, false },
	};
	const VkFormat c_Formats[] = { VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R16_SFLOAT, VK_FORMAT_R32G32B32A32_SFLOAT,
								   VK_FORMAT_BC1_RGBA_UNORM_BLOCK, VK_FORMAT_BC3_UNORM_BLOCK };
	for (const Shape& s : c_Shapes)
	{
		for (VkFormat format : c_Formats)
		{
			const bool bc = vkdFormatInfoOf(format).m_bCompressed;
			if (bc && s.m_Type == VK_IMAGE_TYPE_3D)
			{
				continue;		// D3D11 has BC volume textures, but no loader makes them
			}
			vkdImageDesc desc;
			desc.m_Type = s.m_Type;
			desc.m_Format = format;
			desc.m_Width = s.m_W;
			desc.m_Height = s.m_H;
			desc.m_Depth = s.m_D;
			desc.m_Mips = s.m_Mips;
			desc.m_Layers = s.m_Layers;
			desc.m_bCube = s.m_bCube;
			desc.m_Usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			vkdImage* image = Make(desc, "UploadReadBackEveryShape");
			EXPECT_EQ(s.m_Mips, image->Desc().m_Mips);

			std::vector<std::vector<uint8_t>> expected;
			for (uint32_t layer = 0; layer < s.m_Layers; layer++)
			{
				for (uint32_t mip = 0; mip < s.m_Mips; mip++)
				{
					const uint32_t w = (std::max)(1u, s.m_W >> mip), h = (std::max)(1u, s.m_H >> mip), d = (std::max)(1u, s.m_D >> mip);
					expected.push_back(pattern(subresource_size(format, w, h, d), (uint32_t)expected.size() * 31 + format));
					const size_t row = row_pitch(format, w);
					image->Upload(Context(), mip, layer, expected.back().data(), row, subresource_size(format, w, h, 1));
				}
			}
			Context().Flush();
			size_t index = 0;
			for (uint32_t layer = 0; layer < s.m_Layers; layer++)
			{
				for (uint32_t mip = 0; mip < s.m_Mips; mip++, index++)
				{
					EXPECT_TRUE(image->ReadBack(mip, layer) == expected[index])
						<< s.m_Name << ", format " << format << ", mip " << mip << ", layer " << layer;
				}
			}
		}
	}
}

TEST_F(vkdImageTest, GenerateMipsMatchesD3D)
{
	GT_D3D11(d3d);
	const uint32_t c_Size = 256, c_Mips = 9;
	std::vector<uint8_t> level0(c_Size * c_Size * 4);
	const std::vector<uint8_t> noise = pattern(level0.size(), 7);
	for (uint32_t y = 0; y < c_Size; y++)
	{
		for (uint32_t x = 0; x < c_Size; x++)
		{
			uint8_t* p = &level0[(y * c_Size + x) * 4];
			const uint8_t* n = &noise[(y * c_Size + x) * 4];
			p[0] = (uint8_t)(std::min)(255, (int)x + (n[0] & 31));
			p[1] = (uint8_t)(std::min)(255, (int)y + (n[1] & 31));
			p[2] = (uint8_t)((x ^ y) & 0xFF);
			p[3] = (uint8_t)(128 + (n[3] & 127));
		}
	}

	//	D3D11
	D3D11_TEXTURE2D_DESC td = {};
	td.Width = td.Height = c_Size;
	td.MipLevels = c_Mips;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc.Count = 1;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	td.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	ASSERT_TRUE(SUCCEEDED(d3d.Device()->CreateTexture2D(&td, NULL, &texture)));
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
	ASSERT_TRUE(SUCCEEDED(d3d.Device()->CreateShaderResourceView(texture.Get(), NULL, &view)));
	d3d.Context()->UpdateSubresource(texture.Get(), 0, NULL, level0.data(), c_Size * 4, 0);
	d3d.Context()->GenerateMips(view.Get());
	td.BindFlags = 0;
	td.MiscFlags = 0;
	td.Usage = D3D11_USAGE_STAGING;
	td.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> staging;
	ASSERT_TRUE(SUCCEEDED(d3d.Device()->CreateTexture2D(&td, NULL, &staging)));
	d3d.Context()->CopyResource(staging.Get(), texture.Get());

	//	Vulkan
	vkdImageDesc desc;
	desc.m_Format = VK_FORMAT_R8G8B8A8_UNORM;
	desc.m_Width = desc.m_Height = c_Size;
	desc.m_Mips = c_Mips;
	desc.m_Usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	vkdImage* image = Make(desc, "GenerateMipsMatchesD3D");
	image->Upload(Context(), 0, 0, level0.data(), c_Size * 4, c_Size * c_Size * 4);
	image->GenerateMips(Context());
	Context().Flush();

	for (uint32_t mip = 1; mip < c_Mips; mip++)
	{
		const uint32_t size = c_Size >> mip;
		std::vector<uint8_t> dx11(size * size * 4);
		D3D11_MAPPED_SUBRESOURCE mapped;
		ASSERT_TRUE(SUCCEEDED(d3d.Context()->Map(staging.Get(), mip, D3D11_MAP_READ, 0, &mapped)));
		for (uint32_t y = 0; y < size; y++)
		{
			memcpy(&dx11[y * size * 4], (const uint8_t*)mapped.pData + y * mapped.RowPitch, size * 4);
		}
		d3d.Context()->Unmap(staging.Get(), mip);
		const std::vector<uint8_t> vulkan = image->ReadBack(mip, 0);
		ASSERT_EQ(dx11.size(), vulkan.size());
		const gtCompareResult result = gtCompareImages(dx11.data(), vulkan.data(), size, size);
		EXPECT_TRUE(result.m_bPass) << "mip " << mip << ": " << result.Describe() << " ("
			<< gtSaveComparison("GenerateMipsMatchesD3D_" + std::to_string(mip), dx11.data(), vulkan.data(), size, size) << ")";
		std::printf("GenerateMips mip %u: %s\n", mip, result.Describe().c_str());
	}
}

TEST_F(vkdImageTest, SubresourceViewsRenderPerFace)
{
	vkdImageDesc desc;
	desc.m_Format = VK_FORMAT_R8G8B8A8_UNORM;
	desc.m_Width = desc.m_Height = 8;
	desc.m_Mips = 2;
	desc.m_Layers = 6;
	desc.m_bCube = true;
	desc.m_Usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
				   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	vkdImage* cube = Make(desc, "SubresourceViewsRenderPerFace");
	const float c_Black[4] = { 0, 0, 0, 0 };
	Context().ClearRenderTarget(cube, c_Black);		// the whole image: every face and mip

	//	Faces 0-2 bound as the render target (cleared while rendering), faces
	//	3-5 not bound (cleared with a transfer); mip 1 is left alone
	for (uint32_t face = 0; face < 6; face++)
	{
		vkdImage* view = cube->Subresource(0, face);
		ASSERT_NE(nullptr, view);
		EXPECT_EQ(view, cube->Subresource(0, face)) << "the view is made once";
		EXPECT_EQ(8u, view->Width());
		const float color[4] = { (face + 1) / 7.0f, 128 / 255.0f, face / 7.0f, 1 };		// no exact halves: their rounding is the GPU's choice
		if (face < 3)
		{
			Context().SetRenderTargets(1, &view, nullptr);
		}
		else
		{
			Context().SetRenderTargets(0, nullptr, nullptr);
		}
		Context().ClearRenderTarget(view, color);
	}
	Context().SetRenderTargets(0, nullptr, nullptr);
	EXPECT_EQ(4u, cube->Subresource(1, 2)->Width());
	Context().Flush();

	for (uint32_t face = 0; face < 6; face++)
	{
		const std::vector<uint8_t> pixels = cube->ReadBack(0, face);
		ASSERT_EQ(8u * 8 * 4, pixels.size());
		const uint8_t expected[4] = { (uint8_t)((face + 1) * 255 / 7.0f + 0.5f), 128, (uint8_t)(face * 255 / 7.0f + 0.5f), 255 };
		for (size_t p = 0; p < pixels.size(); p += 4)
		{
			ASSERT_TRUE(std::equal(expected, expected + 4, &pixels[p])) << "face " << face << ", pixel " << p / 4 << ": "
				<< (int)pixels[p] << " " << (int)pixels[p + 1] << " " << (int)pixels[p + 2] << " " << (int)pixels[p + 3]
				<< ", expected " << (int)expected[0] << " " << (int)expected[1] << " " << (int)expected[2] << " " << (int)expected[3];
		}
		const std::vector<uint8_t> mip1 = cube->ReadBack(1, face);
		EXPECT_TRUE(std::all_of(mip1.begin(), mip1.end(), [](uint8_t v) { return v == 0; })) << "face " << face << " mip 1";
	}
}

TEST_F(vkdImageTest, DepthViewSamples)
{
	vkdImageDesc desc;
	desc.m_Format = VK_FORMAT_D24_UNORM_S8_UINT;
	desc.m_Width = desc.m_Height = 16;
	desc.m_Usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	desc.m_Aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	vkdImage* depth = Make(desc, "DepthViewSamples");
	vkdImage* depthView = depth->DepthView();
	ASSERT_NE(nullptr, depthView);
	EXPECT_EQ((VkImageAspectFlags)VK_IMAGE_ASPECT_DEPTH_BIT, depthView->Aspect());
	EXPECT_EQ(depthView, depth->DepthView());

	vkdImage* target = Target(16, 16);
	vkdContext& c = Context();
	c.SetRenderTargets(0, nullptr, depth);
	c.ClearDepthStencil(depth, true, 0.25f, true, 7);

	std::string error;
	std::unique_ptr<vkdShader> vs = vkdShader::Create(&Device(), VK_SHADER_STAGE_VERTEX_BIT, g_gtBindingProtoVS, sizeof(g_gtBindingProtoVS), "VSMain", "gtBindingProto VS", error);
	std::unique_ptr<vkdShader> ps = vkdShader::Create(&Device(), VK_SHADER_STAGE_FRAGMENT_BIT, g_gtBindingProtoPS, sizeof(g_gtBindingProtoPS), "PSMain", "gtBindingProto PS", error);
	ASSERT_TRUE(vs && ps) << error;
	c.SetRenderTargets(1, &target, nullptr);
	vkdViewport viewport;
	viewport.m_Width = viewport.m_Height = 16;
	c.SetViewports(1, &viewport);
	const vkdShader* stages[5] = { vs.get(), nullptr, nullptr, nullptr, ps.get() };
	c.SetShaders(stages);
	c.SetVertexLayout(nullptr);
	c.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	const float scale[4] = { 1, 1, 1, 1 }, bias[4] = { 0, 0, 0, 0 };
	c.Bindings().SetConstantBuffer(0, c.Upload(scale, 16).m_Gpu, 16);
	c.Bindings().SetConstantBuffer(3, c.Upload(bias, 16).m_Gpu, 16);
	c.Bindings().SetTexture(0, depthView->SampledIndex(*m_pBindings));
	c.Bindings().SetTexture(37, c.NullTexture()->SampledIndex(*m_pBindings));
	VkSamplerCreateInfo point = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
	c.Bindings().SetSampler(13, m_pBindings->AddSampler(point));
	c.Draw(3, 0);
	const std::vector<uint8_t> image = Read(target);
	EXPECT_NEAR(64, image[0], 1) << "red is the depth (0.25)";
	EXPECT_NEAR(64, image[(15 * 16 + 15) * 4], 1);
	c.SetRenderTargets(0, nullptr, nullptr);
	c.Flush();
	vs.reset();
	ps.reset();
}
