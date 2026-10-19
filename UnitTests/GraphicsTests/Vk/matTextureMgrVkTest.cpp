/*****************************************************************************
**  matTextureMgrVkTest.cpp
**
**      GraphicsVulkan's texture manager and texture classes against
**      GraphicsDX11's: each test runs the same code through
**      matTextureMgr under the DX11 layer, then under the Vulkan layer
**      (gtBackend), and compares what a shader reads from the textures
**      (gtTexel.hlsl: each texel fetched exactly into a render target of
**      the mip's size) or what the render targets hold.
**
**      Skipped without a Vulkan device (the DX11 half still runs).
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtImageFiles.hpp"
#include "gtPlatform.hpp"
#include "gtTempDir.hpp"

#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

//	DX11
#ifndef G2D_DX11GLOBALWIN_HPP
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#endif
#ifndef MAT_TEXTUREDX11_HPP
#include "GraphicsDX11/mat/matTextureDX11.hpp"
#endif
#ifndef MAT_TEXTUREMGRDX11_HPP
#include "GraphicsDX11/mat/matTextureMgrDX11.hpp"
#endif
#ifndef MAT_RENDERTARGETTEXTURE_HPP
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/mat/matCubeRenderTargetTexture.hpp"
#endif

//	Vulkan
#ifndef GVK_MATTEXTUREMGRVK_HPP
#include "GraphicsVulkan/Mat/matTextureMgrVk.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#include "GraphicsVulkan/Mat/matCubeRenderTargetTexture.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDSHADER_HPP
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#endif

#include "gtTexelVS.h"
#include "gtTexelPS.h"
#include "gtTexel3DVS.h"
#include "gtTexel3DPS.h"

#include <d3dcompiler.h>
#include <wrl/client.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace
{
	const g2dPFD c_RGBA8(0, 8, 8, 8, 16, 8, 24, 8, 32);

	struct Picture
	{
		bool					m_bValid = false;
		int						m_Width = 0;
		int						m_Height = 0;
		std::vector<uint8_t>	m_Rgba;
	};

	fsLocator locator_of(const std::filesystem::path& i_Path)
	{
		fsLocator locator;
		fsFileUtil::UnicodeStringToLocator(itString(i_Path.wstring().c_str()), locator);
		return locator;
	}

	//------------------------------------------------------------------------
	//	Draws textures (gtTexel.hlsl) into render targets made by
	//	matTextureMgr, and reads render targets back, on the backend that
	//	is up. Made inside a gtBackend::Run function, gone before it ends.
	//------------------------------------------------------------------------
	class Drawer
	{
		public:
			explicit Drawer(gtBackend::Kind i_Kind) : m_Kind(i_Kind)
			{
				if (m_Kind == gtBackend::e_DX11)
				{
					init_dx11();
				}
				else
				{
					init_vulkan();
				}
			}

			//	One mip of one layer (cube face, array slice), or one slice
			//	of a 3D texture, as a shader reads it
			Picture Texels(const matTexture* i_pTexture, int i_Mip = 0, int i_Layer = 0, bool i_bVolume = false)
			{
				Picture picture;
				if (!i_pTexture)
				{
					return picture;
				}
				const int width = std::max(1, i_pTexture->GetWidth() >> i_Mip);
				const int height = std::max(1, i_pTexture->GetHeight() >> i_Mip);
				matTexture* target = matTextureMgr::CreateRenderTargetTexture(width, height, false, NULL, false, false);
				if (!target || !target->GetRenderTargetAPI())
				{
					ADD_FAILURE() << "no render target";
					return picture;
				}
				Draw(i_pTexture, target, i_Mip, i_Layer, i_bVolume);
				picture = Read(target);
				matTextureMgr::ReleaseTexture(target);
				return picture;
			}

			//	Draws i_pTexture's texels into i_pTarget (its size)
			void Draw(const matTexture* i_pTexture, matTexture* i_pTarget, int i_Mip = 0, int i_Layer = 0, bool i_bVolume = false)
			{
				i_pTarget->GetRenderTargetAPI()->MakeCurrent();
				if (m_Kind == gtBackend::e_DX11)
				{
					draw_dx11(i_pTexture, i_Mip, i_Layer, i_bVolume);
				}
				else
				{
					draw_vulkan(i_pTexture, i_Mip, i_Layer, i_bVolume);
				}
			}

			//	An RGBA8 render target's pixels
			Picture Read(const matTexture* i_pTarget)
			{
				Picture picture;
				picture.m_Width = i_pTarget->GetWidth();
				picture.m_Height = i_pTarget->GetHeight();
				picture.m_Rgba = Raw(i_pTarget, 4);
				picture.m_bValid = picture.m_Rgba.size() == (size_t)picture.m_Width * picture.m_Height * 4;
				return picture;
			}

			//	Mip 0 of layer 0, i_PixelBytes per pixel, rows tightly packed
			std::vector<uint8_t> Raw(const matTexture* i_pTexture, int i_PixelBytes)
			{
				return m_Kind == gtBackend::e_DX11 ? raw_dx11(i_pTexture, i_PixelBytes) : raw_vulkan(i_pTexture);
			}

			//	The number of mips the texture's image has
			int Mips(const matTexture* i_pTexture)
			{
				if (m_Kind == gtBackend::e_DX11)
				{
					ID3D11Resource* resource = dynamic_cast<const matTextureDX11*>(i_pTexture)->GetResource();
					D3D11_RESOURCE_DIMENSION dimension;
					resource->GetType(&dimension);
					if (dimension == D3D11_RESOURCE_DIMENSION_TEXTURE3D)
					{
						D3D11_TEXTURE3D_DESC desc;
						static_cast<ID3D11Texture3D*>(resource)->GetDesc(&desc);
						return desc.MipLevels;
					}
					D3D11_TEXTURE2D_DESC desc;
					static_cast<ID3D11Texture2D*>(resource)->GetDesc(&desc);
					return desc.MipLevels;
				}
				gvk::vkdImage* image = gvk::g3dVkTextureUtil::GetImage(i_pTexture);
				return image ? (int)image->Desc().m_Mips : 0;
			}

		private:
			//--------------------------------------------------------------------
			//	DX11: the HLSL compiled with fxc at run time, GT_DX11 defined
			//--------------------------------------------------------------------
			static ComPtr<ID3DBlob> compile(bool i_bVolume, const char* i_Entry, const char* i_Profile)
			{
				static std::map<std::string, ComPtr<ID3DBlob>> s_Blobs;
				const std::string key = std::string(i_bVolume ? "3d:" : "2d:") + i_Entry;
				auto found = s_Blobs.find(key);
				if (found != s_Blobs.end())
				{
					return found->second;
				}
				const std::filesystem::path file = std::filesystem::u8path(gtPlatform::RepoRoot()) / "UnitTests" / "GraphicsTests" / "Vk" /
												   "Shaders" / (i_bVolume ? "gtTexel3D.hlsl" : "gtTexel.hlsl");
				const D3D_SHADER_MACRO defines[] = { { "GT_DX11", "1" }, { NULL, NULL } };
				ComPtr<ID3DBlob> blob, errors;
				if (FAILED(D3DCompileFromFile(file.wstring().c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, i_Entry, i_Profile,
											  0, 0, &blob, &errors)))
				{
					ADD_FAILURE() << "can't compile " << file.u8string() << ": " << (errors ? (const char*)errors->GetBufferPointer() : "");
					return NULL;
				}
				s_Blobs[key] = blob;
				return blob;
			}

			void init_dx11()
			{
				ID3D11Device* device = g2dDX11Global::g_pDevice;
				for (int volume = 0; volume < 2; volume++)
				{
					ComPtr<ID3DBlob> vs = compile(volume != 0, "VSMain", "vs_5_0");
					ComPtr<ID3DBlob> ps = compile(volume != 0, "PSMain", "ps_5_0");
					if (vs && ps)
					{
						device->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &m_DxVS[volume]);
						device->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &m_DxPS[volume]);
					}
				}
				D3D11_BUFFER_DESC cb = { 16, D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
				device->CreateBuffer(&cb, NULL, &m_DxSelect);
				D3D11_RASTERIZER_DESC raster = {};
				raster.FillMode = D3D11_FILL_SOLID;
				raster.CullMode = D3D11_CULL_NONE;
				raster.DepthClipEnable = TRUE;
				device->CreateRasterizerState(&raster, &m_DxRaster);
				D3D11_DEPTH_STENCIL_DESC depth = {};
				depth.DepthEnable = FALSE;
				depth.DepthFunc = D3D11_COMPARISON_ALWAYS;
				device->CreateDepthStencilState(&depth, &m_DxDepth);
			}

			void draw_dx11(const matTexture* i_pTexture, int i_Mip, int i_Layer, bool i_bVolume)
			{
				ID3D11Device* device = g2dDX11Global::g_pDevice;
				ID3D11DeviceContext* context = g2dDX11Global::g_pDeviceContext;
				const matTextureDX11* texture = dynamic_cast<const matTextureDX11*>(i_pTexture);
				ASSERT_TRUE(texture && texture->GetResource());
				ID3D11Resource* resource = texture->GetResource();

				D3D11_SHADER_RESOURCE_VIEW_DESC view = {};
				if (i_bVolume)
				{
					D3D11_TEXTURE3D_DESC desc;
					static_cast<ID3D11Texture3D*>(resource)->GetDesc(&desc);
					view.Format = desc.Format;
					view.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
					view.Texture3D.MipLevels = desc.MipLevels;
				}
				else
				{
					D3D11_TEXTURE2D_DESC desc;
					static_cast<ID3D11Texture2D*>(resource)->GetDesc(&desc);
					view.Format = desc.Format;
					view.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
					view.Texture2DArray.MipLevels = desc.MipLevels;
					view.Texture2DArray.ArraySize = desc.ArraySize;
				}
				ComPtr<ID3D11ShaderResourceView> srv;
				ASSERT_TRUE(SUCCEEDED(device->CreateShaderResourceView(resource, &view, &srv)));

				const int select[4] = { i_Layer, i_Mip, 0, 0 };
				context->UpdateSubresource(m_DxSelect.Get(), 0, NULL, select, 0, 0);
				context->IASetInputLayout(NULL);
				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				context->VSSetShader(m_DxVS[i_bVolume].Get(), NULL, 0);
				context->HSSetShader(NULL, NULL, 0);
				context->DSSetShader(NULL, NULL, 0);
				context->GSSetShader(NULL, NULL, 0);
				context->PSSetShader(m_DxPS[i_bVolume].Get(), NULL, 0);
				context->PSSetShaderResources(0, 1, srv.GetAddressOf());
				context->PSSetConstantBuffers(0, 1, m_DxSelect.GetAddressOf());
				context->RSSetState(m_DxRaster.Get());
				context->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
				context->OMSetDepthStencilState(m_DxDepth.Get(), 0);
				context->Draw(3, 0);
				ID3D11ShaderResourceView* none = NULL;
				context->PSSetShaderResources(0, 1, &none);
			}

			std::vector<uint8_t> raw_dx11(const matTexture* i_pTexture, int i_PixelBytes)
			{
				std::vector<uint8_t> bytes;
				const matTextureDX11* texture = dynamic_cast<const matTextureDX11*>(i_pTexture);
				if (!texture || !texture->GetResource())
				{
					ADD_FAILURE() << "not a DX11 texture";
					return bytes;
				}
				ID3D11Device* device = g2dDX11Global::g_pDevice;
				ID3D11DeviceContext* context = g2dDX11Global::g_pDeviceContext;
				D3D11_TEXTURE2D_DESC desc;
				static_cast<ID3D11Texture2D*>(texture->GetResource())->GetDesc(&desc);
				D3D11_TEXTURE2D_DESC staging_desc = desc;
				staging_desc.MipLevels = staging_desc.ArraySize = 1;
				staging_desc.SampleDesc.Count = 1;
				staging_desc.SampleDesc.Quality = 0;
				staging_desc.Usage = D3D11_USAGE_STAGING;
				staging_desc.BindFlags = 0;
				staging_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				staging_desc.MiscFlags = 0;
				ComPtr<ID3D11Texture2D> staging;
				if (FAILED(device->CreateTexture2D(&staging_desc, NULL, &staging)))
				{
					ADD_FAILURE() << "no staging texture";
					return bytes;
				}
				context->CopySubresourceRegion(staging.Get(), 0, 0, 0, 0, texture->GetResource(), 0, NULL);
				D3D11_MAPPED_SUBRESOURCE mapped;
				if (SUCCEEDED(context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
				{
					const size_t row = (size_t)desc.Width * i_PixelBytes;
					bytes.resize(row * desc.Height);
					for (UINT y = 0; y < desc.Height; y++)
					{
						memcpy(&bytes[y * row], (const uint8_t*)mapped.pData + y * mapped.RowPitch, row);
					}
					context->Unmap(staging.Get(), 0);
				}
				return bytes;
			}

			//--------------------------------------------------------------------
			//	Vulkan: the SPIR-V GraphicsTests.vcxproj compiled
			//--------------------------------------------------------------------
			void init_vulkan()
			{
				gvk::vkdDevice* device = &gvk::vkdGlobal::Device();
				std::string error;
				m_VkVS[0] = gvk::vkdShader::Create(device, VK_SHADER_STAGE_VERTEX_BIT, g_gtTexelVS, sizeof(g_gtTexelVS), "VSMain", "gtTexel VS", error);
				m_VkPS[0] = gvk::vkdShader::Create(device, VK_SHADER_STAGE_FRAGMENT_BIT, g_gtTexelPS, sizeof(g_gtTexelPS), "PSMain", "gtTexel PS", error);
				m_VkVS[1] = gvk::vkdShader::Create(device, VK_SHADER_STAGE_VERTEX_BIT, g_gtTexel3DVS, sizeof(g_gtTexel3DVS), "VSMain", "gtTexel3D VS", error);
				m_VkPS[1] = gvk::vkdShader::Create(device, VK_SHADER_STAGE_FRAGMENT_BIT, g_gtTexel3DPS, sizeof(g_gtTexel3DPS), "PSMain", "gtTexel3D PS", error);
				EXPECT_TRUE(m_VkVS[0] && m_VkPS[0] && m_VkVS[1] && m_VkPS[1]) << error;
			}

			void draw_vulkan(const matTexture* i_pTexture, int i_Mip, int i_Layer, bool i_bVolume)
			{
				gvk::vkdContext& context = gvk::vkdGlobal::Context();
				gvk::vkdImage* image = gvk::g3dVkTextureUtil::GetImage(i_pTexture);
				ASSERT_TRUE(image != NULL);
				gvk::vkdImage* view = i_bVolume ? image : image->Subresource(i_Mip, i_Layer);
				ASSERT_TRUE(view != NULL);

				const gvk::vkdShader* stages[gvk::vkdPipelineKey::c_Stages] = { m_VkVS[i_bVolume].get(), NULL, NULL, NULL, m_VkPS[i_bVolume].get() };
				context.SetShaders(stages);
				context.SetVertexLayout(NULL);
				context.SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
				gvk::vkdRasterizerState raster;
				raster.m_Cull = VK_CULL_MODE_NONE;
				context.SetRasterizerState(raster);
				const float factor[4] = { 0, 0, 0, 0 };
				context.SetBlendState(gvk::vkdBlendState(), factor);
				gvk::vkdDepthStencilState depth;
				depth.m_bDepth = false;
				context.SetDepthStencilState(depth, 0);
				const int select[4] = { i_Layer, i_Mip, 0, 0 };
				gvk::vkdRingBuffer::Allocation constants = context.Upload(select, sizeof(select));
				context.Bindings().SetConstantBuffer(0, constants.m_Gpu, sizeof(select));
				context.Bindings().SetTexture(0, view->SampledIndex(context.Bindings()));
				context.Draw(3, 0);
			}

			std::vector<uint8_t> raw_vulkan(const matTexture* i_pTexture)
			{
				gvk::vkdImage* image = gvk::g3dVkTextureUtil::GetImage(i_pTexture);
				if (!image)
				{
					ADD_FAILURE() << "not a Vulkan texture";
					return std::vector<uint8_t>();
				}
				gvk::vkdGlobal::Context().Flush();
				return image->ReadBack(0, 0);
			}

			gtBackend::Kind						m_Kind;
			ComPtr<ID3D11VertexShader>			m_DxVS[2];
			ComPtr<ID3D11PixelShader>			m_DxPS[2];
			ComPtr<ID3D11Buffer>				m_DxSelect;
			ComPtr<ID3D11RasterizerState>		m_DxRaster;
			ComPtr<ID3D11DepthStencilState>		m_DxDepth;
			std::unique_ptr<gvk::vkdShader>		m_VkVS[2];
			std::unique_ptr<gvk::vkdShader>		m_VkPS[2];
	};

	//	Compares two pictures with the spec's tolerance; i_What names the case
	//	Exceptions to the spec's tolerance (max 2/255, at most 0.1% of pixels
	//	over 1/255), by case name prefix, each with its reason
	struct Exception
	{
		const char*	m_Prefix;
		float		m_MaxErr;			// in 1/255
		float		m_MaxPctOver;
		const char*	m_Reason;
	};
	const Exception c_Exceptions[] =
	{
		//	WIC's and stb_image's JPEG decoders (IDCT, chroma upsampling)
		//	differ by up to 2/255 on many pixels (Task 3, DecodeMatchesDirectXTex);
		//	the mips and reductions are filtered from those values
		{ "jpg_rgb",	2.0f, 100.0f, "WIC and stb_image JPEG decoding differ by up to 2/255" },
		//	The same differences in sRGB-encoded values, read (linear) through
		//	the _SRGB format: the curve's slope near white is up to ~2.4
		{ "jpg_srgb",	5.0f, 100.0f, "the JPEG decoding difference (2/255) read through the sRGB curve" },
	};

	void expect_same(const Picture& i_DX11, const Picture& i_Vulkan, const std::string& i_What)
	{
		ASSERT_TRUE(i_DX11.m_bValid) << i_What << ": no DX11 picture";
		ASSERT_TRUE(i_Vulkan.m_bValid) << i_What << ": no Vulkan picture";
		ASSERT_EQ(i_DX11.m_Width, i_Vulkan.m_Width) << i_What;
		ASSERT_EQ(i_DX11.m_Height, i_Vulkan.m_Height) << i_What;
		float max_err = 2 / 255.0f, max_pct_over = 0.1f;
		const char* reason = NULL;
		for (const Exception& e : c_Exceptions)
		{
			if (i_What.compare(0, strlen(e.m_Prefix), e.m_Prefix) == 0)
			{
				max_err = e.m_MaxErr / 255.0f;
				max_pct_over = e.m_MaxPctOver;
				reason = e.m_Reason;
			}
		}
		const gtCompareResult result = gtCompareImages(i_DX11.m_Rgba.data(), i_Vulkan.m_Rgba.data(), i_DX11.m_Width, i_DX11.m_Height,
													   max_err, 1 / 255.0f, max_pct_over);
		if (reason && result.m_bPass)
		{
			std::cout << "  " << i_What << ": " << result.Describe() << " (exception: " << reason << ")" << std::endl;
			return;
		}
		if (!result.m_bPass)
		{
			std::string test = ::testing::UnitTest::GetInstance()->current_test_info()->name();
			std::string dir = gtSaveComparison(test + "/" + i_What, i_DX11.m_Rgba.data(), i_Vulkan.m_Rgba.data(),
											   i_DX11.m_Width, i_DX11.m_Height);
			ADD_FAILURE() << i_What << ": " << result.Describe() << " (images in " << dir << ")";
		}
		else if (result.m_MaxErr > 0)
		{
			std::cout << "  " << i_What << ": " << result.Describe() << std::endl;
		}
	}

	//	Runs i_Function under both backends, the backend's kind as argument;
	//	false if either couldn't run (the test is then skipped)
	bool run_both(const std::function<void(gtBackend::Kind)>& i_Function)
	{
		for (gtBackend::Kind kind : { gtBackend::e_DX11, gtBackend::e_Vulkan })
		{
			if (!gtBackend::Run(kind, [&]() { i_Function(kind); }))
			{
				return false;
			}
		}
		return true;
	}

	//	The texture image files: one picture per file and backend
	struct Loaded
	{
		bool				m_bLoaded = false;
		int					m_Width = 0;
		int					m_Height = 0;
		int					m_Mips = 0;
		std::vector<Picture> m_Pictures;		// what the test drew
	};

	bool is_volume(const gtImageFile& i_File)	{ return i_File.m_Name.find("volume") != std::string::npos; }
	bool is_cube(const gtImageFile& i_File)		{ return i_File.m_Name.find("cube") != std::string::npos; }

	std::vector<uint8_t> pattern(int i_Width, int i_Height, int i_Seed)
	{
		std::vector<uint8_t> rgba((size_t)i_Width * i_Height * 4);
		uint32_t state = 0x9E3779B9u * (i_Seed + 1);
		for (uint8_t& b : rgba)
		{
			state = state * 1664525u + 1013904223u;
			b = (uint8_t)(state >> 24);
		}
		return rgba;
	}

	//	Sets a texture adjuster that reduces every texture (matTextureMgr
	//	owns it until GraphicsLayer::CleanUp)
	class ReduceAdjuster : public matTextureAdjuster
	{
		public:
			ReduceAdjuster(int i_Width, int i_Height) : m_Width(i_Width), m_Height(i_Height) {}
			virtual void GetReduce(const fsLocator&, int& o_WidthReduce, int& o_HeightReduce) const
			{
				o_WidthReduce = m_Width;
				o_HeightReduce = m_Height;
			}
		private:
			int m_Width, m_Height;
	};
}

//----------------------------------------------------------------------------
//	Every texture file of the decoder tests, loaded through
//	matTextureMgr::LoadTexture (DX11: DirectXTex and WIC; Vulkan:
//	matImageDecode), drawn and compared: mip 0 of the first layer, or the
//	first slice of a volume. A file one backend loads, the other must too.
//----------------------------------------------------------------------------
TEST(matTextureMgrVkTest, LoadedFormatsMatchDX11)
{
	gtTempDir dir;
	gtComScope com;
	const std::vector<gtImageFile> files = gtWriteImageFiles(dir.GetPath());
	std::map<std::string, Loaded> results[2];
	const bool ran = run_both([&](gtBackend::Kind i_Kind)
	{
		Drawer drawer(i_Kind);
		for (const gtImageFile& file : files)
		{
			matTexture* texture = matTextureMgr::LoadTexture(locator_of(file.m_Path), TEXTURE_TYPE_UNKNOWN);
			Loaded& loaded = results[i_Kind][file.m_Name];
			loaded.m_bLoaded = texture != NULL;
			if (texture)
			{
				loaded.m_Width = texture->GetWidth();
				loaded.m_Height = texture->GetHeight();
				loaded.m_Pictures.push_back(drawer.Texels(texture, 0, 0, is_volume(file)));
				matTextureMgr::ReleaseTexture(texture);
			}
		}
	});
	if (!ran)
	{
		return;
	}
	int loaded_files = 0;
	for (const gtImageFile& file : files)
	{
		const Loaded& dx11 = results[gtBackend::e_DX11][file.m_Name];
		const Loaded& vulkan = results[gtBackend::e_Vulkan][file.m_Name];
		EXPECT_EQ(dx11.m_bLoaded, vulkan.m_bLoaded) << file.m_Name << " loads on " << (dx11.m_bLoaded ? "DX11" : "Vulkan") << " only";
		if (dx11.m_bLoaded && vulkan.m_bLoaded)
		{
			loaded_files++;
			EXPECT_EQ(dx11.m_Width, vulkan.m_Width) << file.m_Name;
			EXPECT_EQ(dx11.m_Height, vulkan.m_Height) << file.m_Name;
			expect_same(dx11.m_Pictures[0], vulkan.m_Pictures[0], file.m_Name);
		}
		else if (!dx11.m_bLoaded)
		{
			std::cout << "  " << file.m_Name << ": not loaded by DX11" << std::endl;
		}
	}
	std::cout << "  " << loaded_files << " of " << files.size() << " files loaded by both" << std::endl;
}

//----------------------------------------------------------------------------
//	Textures whose mips are made at load time (and DDS files that carry
//	them): every level, also with the texture adjuster's width/height
//	reduction, and loaded without mips (mipmap_if_2D false).
//
//	DX11 can't load without mips (it asks DirectXTex for a one-level chain,
//	which DirectXTex rejects, then dereferences the texture it didn't make),
//	so a "plain" load is compared with mip 0 of DX11's mip load. An .hdr
//	file isn't loaded without mips on either (the plain path picks the
//	decoder by extension, and .hdr isn't one).
//----------------------------------------------------------------------------
TEST(matTextureMgrVkTest, MipChainsMatchDX11)
{
	gtTempDir dir;
	gtComScope com;
	const char* const c_Names[] = { "png_rgba8", "png_srgb_chunk", "png_grey8", "png_rgba16", "tif_rgbaf32", "jpg_rgb",
									"bmp_rgb24", "tga_bgra32", "hdr_radiance", "dds_rgba8_mips", "dds_bc1" };
	std::vector<gtImageFile> files;
	for (const gtImageFile& file : gtWriteImageFiles(dir.GetPath()))
	{
		if (std::find_if(std::begin(c_Names), std::end(c_Names), [&](const char* n) { return file.m_Name == n; }) != std::end(c_Names))
		{
			files.push_back(file);
		}
	}
	ASSERT_EQ(std::size(c_Names), files.size());

	struct Variant { const char* m_Name; int m_Reduce; bool m_bMips; };
	const Variant c_Variants[] = { { "mips", 0, true }, { "reduced", 1, true }, { "plain", 0, false } };
	std::map<std::string, Loaded> results[2];
	const bool ran = run_both([&](gtBackend::Kind i_Kind)
	{
		Drawer drawer(i_Kind);
		for (const Variant& variant : c_Variants)
		{
			matTextureMgr::SetTextureAdjuster(variant.m_Reduce ? new ReduceAdjuster(variant.m_Reduce, variant.m_Reduce) : new matTextureAdjuster);
			for (const gtImageFile& file : files)
			{
				const bool hdr = file.m_Kind == gtImageFile::e_HDR;
				//	(DDS files keep their own mips either way, on both)
				const bool dx11_plain = !variant.m_bMips && i_Kind == gtBackend::e_DX11 && file.m_Kind != gtImageFile::e_DDS;
				if (dx11_plain && hdr)
				{
					continue;
				}
				matTexture* texture = matTextureMgr::LoadTexture(locator_of(file.m_Path), TEXTURE_TYPE_2D, variant.m_bMips || dx11_plain);
				if (!variant.m_bMips && hdr)
				{
					EXPECT_EQ(NULL, texture) << file.m_Name << " loads without mips";
					matTextureMgr::ReleaseTexture(texture);
					continue;
				}
				Loaded& loaded = results[i_Kind][file.m_Name + "_" + variant.m_Name];
				loaded.m_bLoaded = texture != NULL;
				if (texture)
				{
					loaded.m_Width = texture->GetWidth();
					loaded.m_Height = texture->GetHeight();
					loaded.m_Mips = dx11_plain ? 1 : drawer.Mips(texture);
					for (int mip = 0; mip < loaded.m_Mips; mip++)
					{
						loaded.m_Pictures.push_back(drawer.Texels(texture, mip));
					}
					matTextureMgr::ReleaseTexture(texture);
				}
			}
		}
	});
	if (!ran)
	{
		return;
	}
	for (auto& entry : results[gtBackend::e_DX11])
	{
		const Loaded& dx11 = entry.second;
		const Loaded& vulkan = results[gtBackend::e_Vulkan][entry.first];
		EXPECT_EQ(dx11.m_bLoaded, vulkan.m_bLoaded) << entry.first << " loads on " << (dx11.m_bLoaded ? "DX11" : "Vulkan") << " only";
		if (!dx11.m_bLoaded || !vulkan.m_bLoaded)
		{
			continue;
		}
		EXPECT_EQ(dx11.m_Width, vulkan.m_Width) << entry.first;
		EXPECT_EQ(dx11.m_Height, vulkan.m_Height) << entry.first;
		EXPECT_EQ(dx11.m_Mips, vulkan.m_Mips) << entry.first;
		for (int mip = 0; mip < std::min(dx11.m_Mips, vulkan.m_Mips); mip++)
		{
			expect_same(dx11.m_Pictures[mip], vulkan.m_Pictures[mip], entry.first + "_mip" + std::to_string(mip));
		}
	}
}

//----------------------------------------------------------------------------
//	Cube maps (each face, each mip), DDS arrays (each layer) and volume
//	textures (each slice), from DDS files and from CreateTexture3D
//----------------------------------------------------------------------------
TEST(matTextureMgrVkTest, CubeAndVolumeMatchDX11)
{
	gtTempDir dir;
	gtComScope com;
	const std::vector<gtImageFile> all = gtWriteImageFiles(dir.GetPath());
	std::vector<gtImageFile> files;
	for (const gtImageFile& file : all)
	{
		if (is_cube(file) || is_volume(file) || file.m_Name == "dds_array_rgba8")
		{
			files.push_back(file);
		}
	}
	ASSERT_EQ(6u, files.size());
	const int c_W = 8, c_H = 8, c_D = 4;
	const std::vector<uint8_t> voxels = pattern(c_W, c_H * c_D, 3);

	std::map<std::string, Picture> results[2];
	const bool ran = run_both([&](gtBackend::Kind i_Kind)
	{
		Drawer drawer(i_Kind);
		for (const gtImageFile& file : files)
		{
			matTexture* texture = matTextureMgr::LoadTexture(locator_of(file.m_Path), TEXTURE_TYPE_UNKNOWN);
			if (!texture)
			{
				ADD_FAILURE() << gtBackend::Name(i_Kind) << " didn't load " << file.m_Name;
				continue;
			}
			if (is_volume(file))
			{
				for (int slice = 0; slice < c_D; slice++)
				{
					results[i_Kind][file.m_Name + "_slice" + std::to_string(slice)] = drawer.Texels(texture, 0, slice, true);
				}
			}
			else
			{
				const int layers = is_cube(file) ? 6 : 3;
				const int mips = drawer.Mips(texture);
				for (int layer = 0; layer < layers; layer++)
				{
					for (int mip = 0; mip < mips; mip++)
					{
						results[i_Kind][file.m_Name + "_layer" + std::to_string(layer) + "_mip" + std::to_string(mip)] =
							drawer.Texels(texture, mip, layer);
					}
				}
			}
			matTextureMgr::ReleaseTexture(texture);
		}
		matTexture* volume = matTextureMgr::CreateTexture3D(c_W, c_H, c_D, &c_RGBA8, BIND_SHADER_RESOURCE, (void*)voxels.data());
		ASSERT_TRUE(volume != NULL);
		for (int slice = 0; slice < c_D; slice++)
		{
			results[i_Kind]["CreateTexture3D_slice" + std::to_string(slice)] = drawer.Texels(volume, 0, slice, true);
		}
		matTextureMgr::ReleaseTexture(volume);
	});
	if (!ran)
	{
		return;
	}
	EXPECT_EQ(results[gtBackend::e_DX11].size(), results[gtBackend::e_Vulkan].size());
	for (auto& entry : results[gtBackend::e_DX11])
	{
		expect_same(entry.second, results[gtBackend::e_Vulkan][entry.first], entry.first);
	}
}

//----------------------------------------------------------------------------
//	Textures made by CreateTexture (with data), filled (FillTexture),
//	updated (UpdateTexture) and copied (CopyTexture, CreateRenderTargetTexture
//	from a texture); the Poisson disk texture (GetPoisson)
//----------------------------------------------------------------------------
TEST(matTextureMgrVkTest, CreateFillUpdateCopyMatchDX11)
{
	const int c_W = 37, c_H = 23;
	const std::vector<uint8_t> first = pattern(c_W, c_H, 1), second = pattern(c_W, c_H, 2);
	std::map<std::string, Picture> results[2];
	std::vector<uint8_t> poisson[2];
	unsigned int memory[2] = {};
	const bool ran = run_both([&](gtBackend::Kind i_Kind)
	{
		Drawer drawer(i_Kind);
		std::map<std::string, Picture>& r = results[i_Kind];
		memory[i_Kind] = matTextureMgr::GetTotalTextureMemory();

		matTexture* plain = matTextureMgr::CreateTexture(c_W, c_H, &c_RGBA8, false, (void*)first.data());
		ASSERT_TRUE(plain != NULL);
		r["CreateTexture"] = drawer.Texels(plain);
		matTextureMgr::UpdateTexture(plain, (unsigned char*)second.data(), (int)second.size());
		r["UpdateTexture"] = drawer.Texels(plain);

		const g2dPFD rgba32f(g2dPFD::e_RGBA32f, 128);
		matTexture* filled = matTextureMgr::CreateTexture(16, 16, &rgba32f, false);
		ASSERT_TRUE(filled != NULL);
		matTextureMgr::FillTexture(filled, maFloatRGBA(0.25f, 0.5f, 0.75f, 1.0f));
		r["FillTexture"] = drawer.Texels(filled);

		matTexture* source = matTextureMgr::CreateRenderTargetTexture(c_W, c_H, false, NULL, false, true);
		matTexture* copy = matTextureMgr::CreateRenderTargetTexture(c_W, c_H, false, NULL, false, true);
		ASSERT_TRUE(source && copy);
		drawer.Draw(plain, source);
		matTextureMgr::CopyTexture(source, copy);
		r["CopyTexture"] = drawer.Read(copy);

		matTexture* from_texture = matTextureMgr::CreateRenderTargetTexture(plain, false, matTextureMgr::e_SceneTexture);
		ASSERT_TRUE(from_texture != NULL);
		r["CreateRenderTargetTexture_from_texture"] = drawer.Read(from_texture);

		matTexture* disk = (i_Kind == gtBackend::e_DX11) ? matTextureMgrDX11::GetPoisson() : gvk::matTextureMgrVk::GetPoisson();
		ASSERT_TRUE(disk != NULL);
		EXPECT_EQ(256, disk->GetWidth());
		EXPECT_EQ(1, disk->GetHeight());
		poisson[i_Kind] = drawer.Raw(disk, 8);

		for (matTexture* t : { plain, filled, source, copy, from_texture })
		{
			matTextureMgr::ReleaseTexture(t);
		}
	});
	if (!ran)
	{
		return;
	}
	for (auto& entry : results[gtBackend::e_DX11])
	{
		expect_same(entry.second, results[gtBackend::e_Vulkan][entry.first], entry.first);
	}
	EXPECT_EQ(poisson[gtBackend::e_DX11], poisson[gtBackend::e_Vulkan]) << "Poisson disk texture";
	EXPECT_GT(memory[gtBackend::e_DX11], 0u);
	EXPECT_GT(memory[gtBackend::e_Vulkan], 0u);
}

//----------------------------------------------------------------------------
//	A render target texture: cleared (colour, and depth), cleared with an
//	RGB colour, drawn into, and a multisampled one resolved
//----------------------------------------------------------------------------
TEST(matTextureMgrVkTest, RenderTargetTextureClearsAndDraws)
{
	const int c_Size = 32;
	const std::vector<uint8_t> texels = pattern(c_Size, c_Size, 4);
	std::map<std::string, Picture> results[2];
	const bool ran = run_both([&](gtBackend::Kind i_Kind)
	{
		Drawer drawer(i_Kind);
		std::map<std::string, Picture>& r = results[i_Kind];
		matTexture* target = matTextureMgr::CreateRenderTargetTexture(c_Size, c_Size, false, NULL, false, true);
		ASSERT_TRUE(target != NULL);
		g2dRenderTarget* api = target->GetRenderTargetAPI();
		ASSERT_TRUE(api != NULL);
		EXPECT_TRUE(api->GetHasDepthBuffer());
		int width = 0, height = 0;
		api->GetDimensions(width, height);
		EXPECT_EQ(c_Size, width);
		EXPECT_EQ(c_Size, height);

		api->Clear(maFloatRGBA(0.2f, 0.4f, 0.6f, 0.8f), true, 1.0f, true, 0);
		r["Clear"] = drawer.Read(target);
		api->Clear(g2dRGBColor(10, 20, 30));
		r["Clear_RGB"] = drawer.Read(target);
		api->ClearDepthStencil(0.5f, true, 3);

		matTexture* texture = matTextureMgr::CreateTexture(c_Size, c_Size, &c_RGBA8, false, (void*)texels.data());
		ASSERT_TRUE(texture != NULL);
		drawer.Draw(texture, target);
		r["Drawn"] = drawer.Read(target);

		//	Multisampled: cleared, then resolved into a single-sampled target
		matTexture* msaa = matTextureMgr::CreateRenderTargetTexture(c_Size, c_Size, false, NULL, false, true,
																	matTextureMgr::e_SceneTexture, false, true);
		matTexture* resolved = matTextureMgr::CreateRenderTargetTexture(c_Size, c_Size, false, NULL, false, false);
		ASSERT_TRUE(msaa && resolved);
		msaa->GetRenderTargetAPI()->Clear(maFloatRGBA(0.9f, 0.1f, 0.3f, 1.0f), true);
		if (i_Kind == gtBackend::e_DX11)
		{
			dynamic_cast<::matRenderTargetTexture*>(msaa)->Resolve(dynamic_cast<::matRenderTargetTexture*>(resolved));
		}
		else
		{
			dynamic_cast<gvk::matRenderTargetTexture*>(msaa)->Resolve(dynamic_cast<gvk::matRenderTargetTexture*>(resolved));
		}
		r["Resolved"] = drawer.Read(resolved);

		for (matTexture* t : { target, texture, msaa, resolved })
		{
			matTextureMgr::ReleaseTexture(t);
		}
	});
	if (!ran)
	{
		return;
	}
	for (auto& entry : results[gtBackend::e_DX11])
	{
		expect_same(entry.second, results[gtBackend::e_Vulkan][entry.first], entry.first);
	}
	//	And the values themselves: clear colours (alpha 0 for an RGB clear, as DX11)
	const Picture& clear = results[gtBackend::e_Vulkan]["Clear"];
	ASSERT_TRUE(clear.m_bValid);
	EXPECT_NEAR(0.2 * 255, clear.m_Rgba[0], 1);
	EXPECT_NEAR(0.8 * 255, clear.m_Rgba[3], 1);
	const Picture& rgb = results[gtBackend::e_Vulkan]["Clear_RGB"];
	ASSERT_TRUE(rgb.m_bValid);
	EXPECT_EQ(10, rgb.m_Rgba[0]);
	EXPECT_EQ(0, rgb.m_Rgba[3]);
}

//----------------------------------------------------------------------------
//	A cube render target texture (reflection maps): a render target with a
//	depth buffer and a full mip chain, zeros when made; an RGB clear clears
//	every face, a colour clear and a draw only the current face
//----------------------------------------------------------------------------
TEST(matTextureMgrVkTest, CubeRenderTargetTextureMatchesDX11)
{
	const int c_Size = 16;
	const std::vector<uint8_t> texels = pattern(c_Size, c_Size, 5);
	std::map<std::string, Picture> results[2];
	int mips[2] = {};
	const bool ran = run_both([&](gtBackend::Kind i_Kind)
	{
		Drawer drawer(i_Kind);
		std::map<std::string, Picture>& r = results[i_Kind];
		matTexture* cube = matTextureMgr::CreateCubeRenderTargetTexture(c_Size, c_Size, NULL);
		ASSERT_TRUE(cube != NULL);
		g2dRenderTarget* api = cube->GetRenderTargetAPI();
		ASSERT_TRUE(api != NULL);
		EXPECT_TRUE(api->GetHasDepthBuffer());
		int width = 0, height = 0;
		api->GetDimensions(width, height);
		EXPECT_EQ(c_Size, width);
		EXPECT_EQ(c_Size, height);
		mips[i_Kind] = drawer.Mips(cube);
		auto set_face = [&](int i_Face)
		{
			if (i_Kind == gtBackend::e_DX11)
				dynamic_cast<::matCubeRenderTargetTexture*>(cube)->SetFaceTarget(i_Face);
			else
				dynamic_cast<gvk::matCubeRenderTargetTexture*>(cube)->SetFaceTarget(i_Face);
		};
		auto read_all = [&](const std::string& i_Step)
		{
			for (int face = 0; face < 6; face++)
			{
				for (int mip = 0; mip < 2; mip++)
				{
					r[i_Step + "_face" + std::to_string(face) + "_mip" + std::to_string(mip)] = drawer.Texels(cube, mip, face);
				}
			}
		};
		read_all("Made");
		api->Clear(g2dRGBColor(10, 20, 30));
		read_all("Clear_RGB");
		set_face(4);
		api->Clear(maFloatRGBA(0.2f, 0.4f, 0.6f, 0.8f), true, 1.0f, true, 0);
		read_all("Clear_face4");
		set_face(1);
		matTexture* texture = matTextureMgr::CreateTexture(c_Size, c_Size, &c_RGBA8, false, (void*)texels.data());
		ASSERT_TRUE(texture != NULL);
		drawer.Draw(texture, cube);
		read_all("Drawn_face1");

		for (matTexture* t : { cube, texture })
		{
			matTextureMgr::ReleaseTexture(t);
		}
	});
	if (!ran)
	{
		return;
	}
	EXPECT_EQ(mips[gtBackend::e_DX11], mips[gtBackend::e_Vulkan]);
	EXPECT_EQ(5, mips[gtBackend::e_Vulkan]);
	EXPECT_EQ(results[gtBackend::e_DX11].size(), results[gtBackend::e_Vulkan].size());
	for (auto& entry : results[gtBackend::e_DX11])
	{
		expect_same(entry.second, results[gtBackend::e_Vulkan][entry.first], entry.first);
	}
	//	And the values themselves
	const Picture& rgb = results[gtBackend::e_Vulkan]["Clear_RGB_face3_mip0"];
	ASSERT_TRUE(rgb.m_bValid);
	EXPECT_EQ(10, rgb.m_Rgba[0]);
	const Picture& face4 = results[gtBackend::e_Vulkan]["Clear_face4_face4_mip0"];
	ASSERT_TRUE(face4.m_bValid);
	EXPECT_NEAR(0.2 * 255, face4.m_Rgba[0], 1);
}

//----------------------------------------------------------------------------
//	The same locator gives the same texture (reference counted); a
//	reload picks up the file's new content; unload doesn't break it
//----------------------------------------------------------------------------
TEST(matTextureMgrVkTest, ReloadAndUnload)
{
	gtTempDir dir;
	std::filesystem::path first_content, second_content;
	for (const gtImageFile& f : gtWriteImageFiles(dir.GetPath() / "files"))
	{
		if (f.m_Name == "tga_bgra32")
			first_content = f.m_Path;
		if (f.m_Name == "tga_bgr24")
			second_content = f.m_Path;		// the same size
	}
	ASSERT_FALSE(first_content.empty() || second_content.empty());
	const std::filesystem::path file = dir.GetPath() / "reload.tga";
	std::map<std::string, Picture> results[2];
	const bool ran = run_both([&](gtBackend::Kind i_Kind)
	{
		Drawer drawer(i_Kind);
		std::filesystem::copy_file(first_content, file, std::filesystem::copy_options::overwrite_existing);
		const fsLocator locator = locator_of(file);
		matTexture* first = matTextureMgr::LoadTexture(locator);
		matTexture* second = matTextureMgr::LoadTexture(locator);
		ASSERT_TRUE(first != NULL);
		EXPECT_EQ(first, second);
		results[i_Kind]["loaded"] = drawer.Texels(first);

		std::filesystem::copy_file(second_content, file, std::filesystem::copy_options::overwrite_existing);
		matTextureMgr::ReloadTexture(locator);
		results[i_Kind]["reloaded"] = drawer.Texels(first);
		matTextureMgr::UnloadTexture(locator);
		results[i_Kind]["unloaded"] = drawer.Texels(first);

		matTextureMgr::ReleaseTexture(second);
		matTextureMgr::ReleaseTexture(first);
		matTexture* again = matTextureMgr::LoadTexture(locator);
		EXPECT_TRUE(again != NULL);
		matTextureMgr::ReleaseTexture(again);
	});
	if (!ran)
	{
		return;
	}
	for (auto& entry : results[gtBackend::e_DX11])
	{
		expect_same(entry.second, results[gtBackend::e_Vulkan][entry.first], entry.first);
	}
	//	And the reload changed the texture (unload left it as it was, as DX11)
	const Picture& loaded = results[gtBackend::e_Vulkan]["loaded"];
	const Picture& reloaded = results[gtBackend::e_Vulkan]["reloaded"];
	const Picture& unloaded = results[gtBackend::e_Vulkan]["unloaded"];
	ASSERT_TRUE(loaded.m_bValid && reloaded.m_bValid && unloaded.m_bValid);
	EXPECT_NE(loaded.m_Rgba, reloaded.m_Rgba);
	EXPECT_EQ(reloaded.m_Rgba, unloaded.m_Rgba);
}

//----------------------------------------------------------------------------
//	A texture released right after a draw that reads it is kept until the
//	GPU is done (synchronization validation would report it otherwise)
//----------------------------------------------------------------------------
TEST(matTextureMgrVkTest, DestroyAfterDrawIsSafe)
{
	const int c_Size = 16;
	const std::vector<uint8_t> texels = pattern(c_Size, c_Size, 5);
	Picture results[2];
	const bool ran = run_both([&](gtBackend::Kind i_Kind)
	{
		Drawer drawer(i_Kind);
		matTexture* target = matTextureMgr::CreateRenderTargetTexture(c_Size, c_Size, false, NULL, false, false);
		ASSERT_TRUE(target != NULL);
		for (int i = 0; i < 4; i++)
		{
			matTexture* texture = matTextureMgr::CreateTexture(c_Size, c_Size, &c_RGBA8, false, (void*)texels.data());
			ASSERT_TRUE(texture != NULL);
			drawer.Draw(texture, target);
			matTextureMgr::ReleaseTexture(texture);
		}
		results[i_Kind] = drawer.Read(target);
		matTextureMgr::ReleaseTexture(target);
	});
	if (!ran)
	{
		return;
	}
	expect_same(results[gtBackend::e_DX11], results[gtBackend::e_Vulkan], "drawn");
}
