/*****************************************************************************
**  g3dMaterialsOptiXTest.cpp
**
**      Part 3c of the OptiX plan (docs/plans/2026-11-20-optix-materials.md):
**
**	- Texture sharing (gtOptiX device, no layer): an exportable image of
**	  each format the texture classes make, with mips, uploaded in Vulkan
**	  and sampled in CUDA (optxTextureProbe) at texel centres of every mip,
**	  reads as the uploaded values decode (sRGB to linear, block-compressed
**	  formats from solid blocks, packed formats through the cache's RGBA
**	  copy); generations change on an upload, mip generation, a clear and
**	  rendering, and not otherwise.
**	- With the whole layer (gtBackend::RunOptiX): the debug view's base
**	  colour of a checker-textured plane with a UV transform, and shading
**	  normals of a normal-mapped plane against the tangent-space formula;
**	  a MASK plane with holes is seen through and casts a shadow with the
**	  same holes; a 50% transparent plane converges to half shadow; an
**	  opaque material disables the any-hit program; a projector's texture
**	  colours Illumination Only (a delta and an area light, and a ramp),
**	  and leaves the Shadow Mask alone; and a material colour, a texture's
**	  contents and a projector's image each restart accumulation, while an
**	  unrelated change doesn't.
**
**	With MSP_GT_AO_IMAGES set to a folder, the pictures are saved there.
**
**	Extra Large Technology
\****************************************************************************/
#include "Ox/gtOptiX.hpp"
#include "gtBackend.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"
#include "gtTempDir.hpp"

#ifndef GOX_CUEXTERNALIMAGECACHE_HPP
#include "GraphicsOptiX/cu/cuExternalImageCache.hpp"
#endif
#ifndef GOX_G3DTRACERENDEREROPTIX_HPP
#include "GraphicsOptiX/G3d/g3dTraceRendererOptiX.hpp"
#endif
#ifndef GOX_OPTXTEXTUREPROBE_HPP
#include "GraphicsOptiX/optx/optxTextureProbe.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_TMESH_FRAG_HPP
#include "GraphicsVulkan/tmesh/tmeshFrag.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef EFF_PBRDATA_HPP
#include "Graphics/Eff/effPbrData.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
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
#ifndef G3D_TARGETRENDERER_HPP
#include "Graphics/G3d/g3dTargetRenderer.hpp"
#endif
#ifndef G3D_TYPE_HPP
#include "Graphics/G3d/g3dType.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif

#include <gtest/gtest.h>
#include <stb_image_write.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>

using namespace gox;

namespace
{
	//------------------------------------------------------------------------
	//	Texture sharing
	//------------------------------------------------------------------------

	uint16_t to_half(float i_Value)
	{
		//	values of the tests: 0 or normal, exactly representable
		uint32_t bits;
		std::memcpy(&bits, &i_Value, 4);
		if ((bits & 0x7FFFFFFF) == 0)
		{
			return uint16_t(bits >> 16);
		}
		const uint32_t sign = (bits >> 16) & 0x8000;
		const int exponent = int((bits >> 23) & 0xFF) - 127 + 15;
		const uint32_t mantissa = (bits >> 13) & 0x3FF;
		return uint16_t(sign | (uint32_t(exponent) << 10) | mantissa);
	}

	float srgb_to_linear(float c)
	{
		return c <= 0.04045f ? c / 12.92f : std::pow((c + 0.055f) / 1.055f, 2.4f);
	}

	struct Rgba
	{
		float m_C[4];
	};

	//	A format under test: its texel (or block) bytes from a seed, and what
	//	a sample of them reads as
	struct FormatCase
	{
		VkFormat	m_Format;
		const char*	m_pName;
		bool		m_bBlock;		// 4x4 blocks, each solid
		float		m_Tolerance;
	};

	//	Deterministic bytes
	uint32_t mix(uint32_t v)
	{
		v ^= v >> 16;
		v *= 0x7feb352d;
		v ^= v >> 15;
		v *= 0x846ca68b;
		v ^= v >> 16;
		return v;
	}

	//	One texel's (or block's) bytes, and what it reads as
	void texel_case(VkFormat i_Format, uint32_t i_Seed, std::vector<uint8_t>& o_Bytes, Rgba& o_Read)
	{
		const uint32_t r = mix(i_Seed), g = mix(i_Seed * 3 + 1), b = mix(i_Seed * 7 + 2), a = mix(i_Seed * 13 + 3);
		const uint8_t r8 = uint8_t(r), g8 = uint8_t(g), b8 = uint8_t(b), a8 = uint8_t(a);
		auto unorm = [](uint32_t v, uint32_t max) { return float(v) / float(max); };
		o_Bytes.clear();
		Rgba out = { { 0, 0, 0, 1 } };
		switch (i_Format)
		{
			case VK_FORMAT_R8G8B8A8_UNORM:
			case VK_FORMAT_R8G8B8A8_SRGB:
			{
				o_Bytes = { r8, g8, b8, a8 };
				const bool srgb = i_Format == VK_FORMAT_R8G8B8A8_SRGB;
				for (int c = 0; c < 3; ++c)
				{
					out.m_C[c] = srgb ? srgb_to_linear(unorm(o_Bytes[c], 255)) : unorm(o_Bytes[c], 255);
				}
				out.m_C[3] = unorm(a8, 255);
				break;
			}
			case VK_FORMAT_B8G8R8A8_UNORM:
			case VK_FORMAT_B8G8R8A8_SRGB:
			{
				o_Bytes = { b8, g8, r8, a8 };
				const bool srgb = i_Format == VK_FORMAT_B8G8R8A8_SRGB;
				const uint8_t rgb[3] = { r8, g8, b8 };
				for (int c = 0; c < 3; ++c)
				{
					out.m_C[c] = srgb ? srgb_to_linear(unorm(rgb[c], 255)) : unorm(rgb[c], 255);
				}
				out.m_C[3] = unorm(a8, 255);
				break;
			}
			case VK_FORMAT_R8G8_UNORM:
				o_Bytes = { r8, g8 };
				out = { { unorm(r8, 255), unorm(g8, 255), 0, 1 } };
				break;
			case VK_FORMAT_R8_UNORM:
				o_Bytes = { r8 };
				out = { { unorm(r8, 255), 0, 0, 1 } };
				break;
			case VK_FORMAT_A8_UNORM:
				o_Bytes = { a8 };
				out = { { 0, 0, 0, unorm(a8, 255) } };
				break;
			case VK_FORMAT_R16G16B16A16_UNORM:
			{
				const uint16_t v[4] = { uint16_t(r), uint16_t(g), uint16_t(b), uint16_t(a) };
				o_Bytes.resize(8);
				std::memcpy(o_Bytes.data(), v, 8);
				for (int c = 0; c < 4; ++c)
				{
					out.m_C[c] = unorm(v[c], 65535);
				}
				break;
			}
			case VK_FORMAT_R16G16B16A16_SFLOAT:
			case VK_FORMAT_R16_SFLOAT:
			{
				const float f[4] = { float(r8) / 64.0f, float(g8) / 64.0f, float(b8) / 64.0f, float(a8) / 256.0f };
				const int n = i_Format == VK_FORMAT_R16_SFLOAT ? 1 : 4;
				for (int c = 0; c < n; ++c)
				{
					const uint16_t h = to_half(f[c]);
					o_Bytes.push_back(uint8_t(h));
					o_Bytes.push_back(uint8_t(h >> 8));
					out.m_C[c] = f[c];
				}
				break;
			}
			case VK_FORMAT_R32G32B32A32_SFLOAT:
			case VK_FORMAT_R32_SFLOAT:
			{
				const float f[4] = { float(r8) / 7.0f, float(g8) / 64.0f, float(b8) / 64.0f, float(a8) / 256.0f };
				const int n = i_Format == VK_FORMAT_R32_SFLOAT ? 1 : 4;
				o_Bytes.resize(size_t(n) * 4);
				std::memcpy(o_Bytes.data(), f, size_t(n) * 4);
				for (int c = 0; c < n; ++c)
				{
					out.m_C[c] = f[c];
				}
				break;
			}
			case VK_FORMAT_R5G6B5_UNORM_PACK16:		// through the RGBA8 copy
			{
				const uint16_t v = uint16_t(((r & 31) << 11) | ((g & 63) << 5) | (b & 31));
				o_Bytes = { uint8_t(v), uint8_t(v >> 8) };
				out = { { unorm(r & 31, 31), unorm(g & 63, 63), unorm(b & 31, 31), 1 } };
				break;
			}
			case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
			case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
			case VK_FORMAT_BC2_UNORM_BLOCK:
			case VK_FORMAT_BC3_UNORM_BLOCK:
			{
				//	a solid colour block: endpoint 0 (above endpoint 1, so four
				//	colours), every index 0
				uint16_t c0 = uint16_t(((r & 31) << 11) | ((g & 63) << 5) | (b & 31)) | 0x0841;
				const uint16_t c1 = 0;
				std::vector<uint8_t> color = { uint8_t(c0), uint8_t(c0 >> 8), uint8_t(c1), uint8_t(c1 >> 8), 0, 0, 0, 0 };
				const bool srgb = i_Format == VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
				const float rgb[3] = { unorm((c0 >> 11) & 31, 31), unorm((c0 >> 5) & 63, 63), unorm(c0 & 31, 31) };
				for (int c = 0; c < 3; ++c)
				{
					out.m_C[c] = srgb ? srgb_to_linear(rgb[c]) : rgb[c];
				}
				if (i_Format == VK_FORMAT_BC2_UNORM_BLOCK)
				{
					const uint8_t a4 = uint8_t(a & 15);
					o_Bytes.assign(8, uint8_t(a4 | (a4 << 4)));
					out.m_C[3] = unorm(a4, 15);
				}
				else if (i_Format == VK_FORMAT_BC3_UNORM_BLOCK)
				{
					o_Bytes = { a8, uint8_t(a8 ^ 0x80), 0, 0, 0, 0, 0, 0 };
					out.m_C[3] = unorm(a8, 255);
				}
				o_Bytes.insert(o_Bytes.end(), color.begin(), color.end());
				break;
			}
			case VK_FORMAT_BC4_UNORM_BLOCK:
				o_Bytes = { r8, uint8_t(r8 ^ 0x80), 0, 0, 0, 0, 0, 0 };
				out = { { unorm(r8, 255), 0, 0, 1 } };
				break;
			case VK_FORMAT_BC5_UNORM_BLOCK:
				o_Bytes = { r8, uint8_t(r8 ^ 0x80), 0, 0, 0, 0, 0, 0, g8, uint8_t(g8 ^ 0x80), 0, 0, 0, 0, 0, 0 };
				out = { { unorm(r8, 255), unorm(g8, 255), 0, 1 } };
				break;
			default:
				ADD_FAILURE() << "no test data for format " << int(i_Format);
				break;
		}
		o_Read = out;
	}

	const FormatCase c_Formats[] =
	{
		{ VK_FORMAT_R8G8B8A8_UNORM,			"R8G8B8A8_UNORM",		false, 1.0f / 255 },
		{ VK_FORMAT_R8G8B8A8_SRGB,			"R8G8B8A8_SRGB",		false, 1.5f / 255 },
		{ VK_FORMAT_B8G8R8A8_UNORM,			"B8G8R8A8_UNORM",		false, 1.0f / 255 },
		{ VK_FORMAT_B8G8R8A8_SRGB,			"B8G8R8A8_SRGB",		false, 1.5f / 255 },
		{ VK_FORMAT_R8G8_UNORM,				"R8G8_UNORM",			false, 1.0f / 255 },
		{ VK_FORMAT_R8_UNORM,				"R8_UNORM",				false, 1.0f / 255 },
		{ VK_FORMAT_A8_UNORM,				"A8_UNORM",				false, 1.0f / 255 },
		{ VK_FORMAT_R16G16B16A16_UNORM,		"R16G16B16A16_UNORM",	false, 1e-4f },
		{ VK_FORMAT_R16G16B16A16_SFLOAT,	"R16G16B16A16_SFLOAT",	false, 1e-3f },
		{ VK_FORMAT_R16_SFLOAT,				"R16_SFLOAT",			false, 1e-3f },
		{ VK_FORMAT_R32G32B32A32_SFLOAT,	"R32G32B32A32_SFLOAT",	false, 1e-4f },
		{ VK_FORMAT_R32_SFLOAT,				"R32_SFLOAT",			false, 1e-4f },
		{ VK_FORMAT_R5G6B5_UNORM_PACK16,	"R5G6B5 (copied)",		false, 1.0f / 255 },
		{ VK_FORMAT_BC1_RGBA_UNORM_BLOCK,	"BC1_UNORM",			true,  1.5f / 255 },
		{ VK_FORMAT_BC1_RGBA_SRGB_BLOCK,	"BC1_SRGB",				true,  2.5f / 255 },
		{ VK_FORMAT_BC2_UNORM_BLOCK,		"BC2_UNORM",			true,  1.5f / 255 },
		{ VK_FORMAT_BC3_UNORM_BLOCK,		"BC3_UNORM",			true,  1.5f / 255 },
		{ VK_FORMAT_BC4_UNORM_BLOCK,		"BC4_UNORM",			true,  1.5f / 255 },
		{ VK_FORMAT_BC5_UNORM_BLOCK,		"BC5_UNORM",			true,  1.5f / 255 },
	};

	//------------------------------------------------------------------------
	//	Scenes
	//------------------------------------------------------------------------

	const int c_Size = 64;
	const float c_Clear[4] = { 0, 0, 0, 0 };

	void save_if_asked(const gtPicture& i_Picture, const char* i_pName)
	{
		const char* dir = std::getenv("MSP_GT_AO_IMAGES");
		if (!dir || !i_Picture.Valid())
		{
			return;
		}
		std::filesystem::create_directories(std::filesystem::u8path(dir));
		const std::string file = std::string(dir) + "/" + ::testing::UnitTest::GetInstance()->current_test_info()->name() +
								 "_" + i_pName + ".png";
		stbi_write_png(file.c_str(), i_Picture.m_Width, i_Picture.m_Height, 4, i_Picture.m_Rgba.data(), i_Picture.m_Width * 4);
	}

	std::unique_ptr<g3dTraceRendererOptiX> traced(g3dSceneRenderer* i_pRenderer)
	{
		g3dTraceRendererOptiX* renderer = dynamic_cast<g3dTraceRendererOptiX*>(i_pRenderer);
		EXPECT_NE(nullptr, renderer) << "the type isn't the OptiX renderer";
		if (!renderer)
		{
			delete i_pRenderer;
		}
		return std::unique_ptr<g3dTraceRendererOptiX>(renderer);
	}

	void look_down(camCamera& io_Camera, float x, float y, float i_Height)
	{
		io_Camera.SetFOV(60.0f);
		io_Camera.SetAspect(1.0f);
		io_Camera.SetClip(1.0f, 5000.0f);
		io_Camera.LookAt(maPoint3d(x, y, i_Height), maPoint3d(x, y, 0), maVector3d(0, 1, 0));
	}

	maMatrix4x4 scale_translate(float s, float tx, float ty, float tz)
	{
		return maMatrix4x4(s, 0, 0, 0,  0, s, 0, 0,  0, 0, s, 0,  tx, ty, tz, 1);
	}

	//	The raster's world view-projection and its inverse, in double, row vectors
	void view_projection(const camCamera& i_Camera, double o_ViewProj[16])
	{
		maMatrix4x4 camera, projection;
		i_Camera.GetCameraMatrix(camera);
		i_Camera.GetProjectionMatrix(projection);
		projection.ScaleBy(-1.0f, 1.0f, 1.0f);
		for (int r = 0; r < 4; ++r)
		{
			for (int c = 0; c < 4; ++c)
			{
				double sum = 0;
				for (int k = 0; k < 4; ++k)
				{
					sum += double(camera(r, k)) * double(projection(k, c));
				}
				o_ViewProj[r * 4 + c] = sum;
			}
		}
	}

	void transform(const double i_M[16], const double i_P[3], double o_H[4])
	{
		for (int c = 0; c < 4; ++c)
		{
			o_H[c] = i_P[0] * i_M[c] + i_P[1] * i_M[4 + c] + i_P[2] * i_M[8 + c] + i_M[12 + c];
		}
	}

	void invert(const double i_M[16], double o_Inverse[16])
	{
		double a[4][8];
		for (int r = 0; r < 4; ++r)
		{
			for (int c = 0; c < 4; ++c)
			{
				a[r][c] = i_M[r * 4 + c];
				a[r][c + 4] = (r == c) ? 1.0 : 0.0;
			}
		}
		for (int c = 0; c < 4; ++c)
		{
			int pivot = c;
			for (int r = c + 1; r < 4; ++r)
			{
				if (std::fabs(a[r][c]) > std::fabs(a[pivot][c]))
				{
					pivot = r;
				}
			}
			for (int k = 0; k < 8; ++k)
			{
				std::swap(a[c][k], a[pivot][k]);
			}
			const double scale = 1.0 / a[c][c];
			for (int k = 0; k < 8; ++k)
			{
				a[c][k] *= scale;
			}
			for (int r = 0; r < 4; ++r)
			{
				if (r != c)
				{
					const double f = a[r][c];
					for (int k = 0; k < 8; ++k)
					{
						a[r][k] -= f * a[c][k];
					}
				}
			}
		}
		for (int r = 0; r < 4; ++r)
		{
			for (int c = 0; c < 4; ++c)
			{
				o_Inverse[r * 4 + c] = a[r][c + 4];
			}
		}
	}

	//	The point of the plane z = i_Z a pixel's centre sees
	maPoint3d plane_point(const camCamera& i_Camera, int x, int y, float i_Z = 0.0f)
	{
		double vp[16], inverse[16];
		view_projection(i_Camera, vp);
		invert(vp, inverse);
		const double ndc[2] = { (x + 0.5) / c_Size * 2 - 1, 1 - (y + 0.5) / c_Size * 2 };
		double ends[2][3];
		for (int e = 0; e < 2; ++e)
		{
			const double p[3] = { ndc[0], ndc[1], double(e) };
			double h[4];
			transform(inverse, p, h);
			for (int k = 0; k < 3; ++k)
			{
				ends[e][k] = h[k] / h[3];
			}
		}
		const double t = (i_Z - ends[0][2]) / (ends[1][2] - ends[0][2]);
		return maPoint3d(float(ends[0][0] + t * (ends[1][0] - ends[0][0])), float(ends[0][1] + t * (ends[1][1] - ends[0][1])), i_Z);
	}

	const uint8_t* texel(const gtPicture& i_Picture, int x, int y)
	{
		return &i_Picture.m_Rgba[(size_t(y) * i_Picture.m_Width + x) * 4];
	}

	int to_byte(float i_Value)
	{
		return int(std::min(1.0f, std::max(0.0f, i_Value)) * 255.0f + 0.5f);
	}

	//	gtScene::CheckerTexture's texel colour at texel (x, y)
	void checker_rgb(int i_Size, int i_Cells, int x, int y, uint8_t o_Rgb[3])
	{
		const int cx = x * i_Cells / i_Size, cy = y * i_Cells / i_Size;
		const bool odd = ((cx + cy) & 1) != 0;
		o_Rgb[0] = uint8_t(odd ? 230 : 40 + 20 * (cx % 8));
		o_Rgb[1] = uint8_t(odd ? 200 : 60 + 25 * (cy % 8));
		o_Rgb[2] = uint8_t(odd ? 60 : 210);
	}

	//	How far (in cells) a texture coordinate is from the nearest cell edge
	float edge_distance(float u, int i_Cells)
	{
		const float c = u * i_Cells;
		const float f = c - std::floor(c);
		return std::min(f, 1.0f - f);
	}

	//	A floor (z = 0) of a grey material that receives shadows
	g3dSceneNode* add_floor(gtSceneBuilder& io_Builder, float i_Half = 1000.0f)
	{
		matMaterial* material = io_Builder.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		gtScene::SetColor(material, effPbrData::c_BaseColor, maFloatRGBA(0.5f, 0.5f, 0.5f, 1));
		g3dSceneNode* node = io_Builder.Add(gtScene::Plane(), material, scale_translate(i_Half, 0, 0, 0));
		node->GetFragment()->SetCastsShadow(true);
		node->GetFragment()->SetReceivesShadow(true);
		return node;
	}

	struct View
	{
		gtTarget			m_Target;
		g3dTargetRenderer	m_View;
		gtSceneBuilder&		m_Builder;

		View(gtSceneBuilder& io_Builder, g3dSceneRenderer* i_pRenderer)
			: m_Target(gtBackend::e_Vulkan, c_Size, c_Size, c_Clear)
			, m_View(m_Target.Target(), i_pRenderer, &io_Builder.Scene(), &io_Builder.Camera())
			, m_Builder(io_Builder)
		{
		}

		gtPicture Render()
		{
			m_View.Render(1.0f, m_Builder.ViewerLayers());
			gtPicture picture = m_Target.Read();
			EXPECT_TRUE(picture.Valid());
			return picture;
		}
	};

	//	A projected light straight down from i_Height, its frustum covering
	//	the view
	g3dProjectedLight* projector(gtSceneBuilder& io_Builder, float i_Height, float i_Angle = 90.0f)
	{
		g3dProjectedLight* light = io_Builder.ProjectedLight(maPoint3d(0, 0, i_Height), maPoint3d(0, 0, 0),
															 maFloatRGBA(0.8f, 0.8f, 0.8f, 1), 0);
		light->SetRange(1000.0f);
		light->SetAngle(i_Angle);
		light->SetInnerAngle(i_Angle + 1.0f);	// a square frustum, no penumbra
		return light;
	}

	//	A floor point's projector texture coordinates (the raster's
	//	projTexCoord.xy / w)
	bool projector_uv(const g3dProjectedLight& i_Light, const maPoint3d& i_P, float& o_U, float& o_V)
	{
		const maMatrix4x4& M = i_Light.GetTotalMatrix();
		const float p[3] = { i_P.GetX(), i_P.GetY(), i_P.GetZ() };
		float h[4];
		for (int c = 0; c < 4; ++c)
		{
			h[c] = p[0] * M(0, c) + p[1] * M(1, c) + p[2] * M(2, c) + M(3, c);
		}
		if (h[3] == 0.0f)
		{
			return false;
		}
		o_U = h[0] / h[3];
		o_V = h[1] / h[3];
		return o_U > 0.0f && o_U < 1.0f && o_V > 0.0f && o_V < 1.0f;
	}
}

//============================================================================
//	Texture sharing (a gtOptiX device: no layer)
//============================================================================

class g3dMaterialsOptiXTest : public gtOptiX
{
};

TEST_F(g3dMaterialsOptiXTest, EveryTextureFormatSamplesInCuda)
{
	optxTextureProbe probe;
	std::string error;
	ASSERT_TRUE(probe.Create(error)) << error;
	cuExternalImageCache& images = Cache().GetImageCache();
	const uint32_t size = 16, mips = 3;

	for (const FormatCase& format : c_Formats)
	{
		SCOPED_TRACE(format.m_pName);
		gvk::vkdImageDesc desc;
		desc.m_Format = format.m_Format;
		desc.m_Width = size;
		desc.m_Height = size;
		desc.m_Mips = mips;
		desc.m_Usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		desc.m_bExportable = true;
		std::unique_ptr<gvk::vkdImage> image = gvk::vkdImage::Create(Device(), desc, format.m_pName);
		ASSERT_TRUE(image->IsExportable());

		//	Every mip: a value per texel (per 4x4 block)
		std::vector<std::vector<Rgba>> expected(mips);
		const uint32_t step = format.m_bBlock ? 4 : 1;
		for (uint32_t mip = 0; mip < mips; ++mip)
		{
			const uint32_t w = std::max(1u, size >> mip);
			const uint32_t units = (w + step - 1) / step;
			std::vector<uint8_t> bytes;
			std::vector<uint8_t> unit;
			expected[mip].resize(size_t(units) * units);
			for (uint32_t y = 0; y < units; ++y)
			{
				for (uint32_t x = 0; x < units; ++x)
				{
					texel_case(format.m_Format, (mip * 997 + y * 61 + x) * 2654435761u + uint32_t(format.m_Format), unit,
							   expected[mip][size_t(y) * units + x]);
					bytes.insert(bytes.end(), unit.begin(), unit.end());
				}
			}
			const size_t row = bytes.size() / units;
			image->Upload(Context(), mip, 0, bytes.data(), row, bytes.size());
		}

		const cuExternalImageCache::Image* mapped = images.Map(*image);
		ASSERT_NE(nullptr, mapped);
		const CUtexObject object = images.Texture(*image, cuExternalImageCache::e_Material);
		ASSERT_NE(0u, object);
		ASSERT_TRUE(Cache().WaitForVulkan(mapped->m_WriteValue));
		optxTexture record = {};
		record.m_Object = object;
		record.m_Width = mapped->m_Width;
		record.m_Height = mapped->m_Height;
		record.m_Swizzle = mapped->m_Swizzle;
		cuBuffer table;
		ASSERT_TRUE(table.Upload(&record, sizeof(record)));

		for (uint32_t mip = 0; mip < mips; ++mip)
		{
			const uint32_t w = std::max(1u, size >> mip);
			std::vector<optxTextureProbe::Query> queries;
			for (uint32_t y = 0; y < w; ++y)
			{
				for (uint32_t x = 0; x < w; ++x)
				{
					queries.push_back({ (x + 0.5f) / w, (y + 0.5f) / w, float(mip), 0.0f });
				}
			}
			const std::vector<float> got = probe.Sample(table.Get(), 0, queries);
			ASSERT_EQ(queries.size() * 4, got.size());
			const uint32_t units = (w + step - 1) / step;
			int bad = 0;
			for (uint32_t y = 0; y < w; ++y)
			{
				for (uint32_t x = 0; x < w; ++x)
				{
					const Rgba& want = expected[mip][size_t(y / step) * units + x / step];
					for (int c = 0; c < 4; ++c)
					{
						const float value = got[(size_t(y) * w + x) * 4 + c];
						if (std::fabs(value - want.m_C[c]) > format.m_Tolerance && bad++ < 4)
						{
							ADD_FAILURE() << "mip " << mip << " texel (" << x << ", " << y << ") channel " << c << ": " << value
										  << ", expected " << want.m_C[c];
						}
					}
				}
			}
			EXPECT_EQ(0, bad) << "mip " << mip;
		}
	}
	EXPECT_GT(images.GetStats().m_Copies, 0) << "the packed format went through a copy";
}

TEST_F(g3dMaterialsOptiXTest, GenerationsChangeOnWrites)
{
	gvk::vkdImageDesc desc;
	desc.m_Format = VK_FORMAT_R8G8B8A8_UNORM;
	desc.m_Width = 8;
	desc.m_Height = 8;
	desc.m_Mips = 4;
	desc.m_Usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
				   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	desc.m_bExportable = true;
	std::unique_ptr<gvk::vkdImage> image = gvk::vkdImage::Create(Device(), desc, "generations");
	uint64_t generation = image->Generation();
	EXPECT_NE(0u, generation);
	EXPECT_EQ(generation, image->Generation()) << "unchanged";

	const std::vector<uint8_t> pixels(8 * 8 * 4, 128);
	image->Upload(Context(), 0, 0, pixels.data(), 8 * 4, pixels.size());
	EXPECT_NE(generation, image->Generation()) << "upload";
	EXPECT_EQ(Device().PendingValue(), image->WriteValue());
	generation = image->Generation();

	image->GenerateMips(Context());
	EXPECT_NE(generation, image->Generation()) << "mips";
	generation = image->Generation();

	const float red[4] = { 1, 0, 0, 1 };
	Context().ClearRenderTarget(image.get(), red);
	EXPECT_NE(generation, image->Generation()) << "clear";
	generation = image->Generation();

	//	rendering into a view of mip 0 (the image's generation: views share it)
	gvk::vkdImage* target = image->Subresource(0, 0);
	Context().SetRenderTargets(1, &target, nullptr);
	Context().ClearRenderTarget(target, red);	// inside rendering: begins it
	EXPECT_NE(generation, image->Generation()) << "rendering";
	EXPECT_EQ(image->Generation(), target->Generation());
	Context().SetRenderTargets(0, nullptr, nullptr);
	Device().Submit();
	Device().WaitIdle();
}

//============================================================================
//	With the layer
//============================================================================

TEST(g3dMaterialsOptiX, DebugViewShowsBaseColourAndNormals)
{
	gtBackend::RunOptiX([]()
	{
		gtTempDir dir;
		gtSceneBuilder builder;
		const int tex = 64, cells = 8;
		matTexture* checker = builder.Keep(gtScene::CheckerTexture(dir.GetPath(), tex, tex, cells));
		matMaterial* material = builder.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		gtScene::SetTexture(material, effPbrData::c_BaseColorMap, checker);
		gtScene::SetColor(material, effPbrData::c_BaseColor, maFloatRGBA(1, 0.5f, 1, 1));
		effUVTransform& uv = material->UVTransform(0);
		uv.m_UScale = 2.0f;
		uv.m_VScale = 2.0f;
		uv.m_UTrans = 0.25f;
		const float half = 10.0f;
		builder.Add(gtScene::Plane(), material, scale_translate(half, 0, 0, 0));
		//	about a texel per pixel (the ray cone picks mip 0), so a sample
		//	well inside a cell reads only that cell
		look_down(builder.Camera(), 0, 0, 8);

		std::unique_ptr<g3dTraceRendererOptiX> renderer = traced(g3dSceneRendererCreate::CreateDefaultRenderer());
		ASSERT_TRUE(renderer);
		renderer->SetDebugView(e_optxDebugBaseColor);
		renderer->SetSampleLimit(1);	// pixel centres
		View view(builder, renderer.get());
		const gtPicture picture = view.Render();
		save_if_asked(picture, "base_colour");
		ASSERT_TRUE(picture.Valid());
		const gvk::vkdImage* image = gvk::g3dVkTextureUtil::GetImage(checker);
		ASSERT_NE(nullptr, image);
		const bool srgb = gvk::vkdFormatInfoOf(image->Format()).m_bSrgb;

		int checked = 0;
		for (int y = 2; y < c_Size; y += 5)
		{
			for (int x = 2; x < c_Size; x += 5)
			{
				const maPoint3d p = plane_point(builder.Camera(), x, y);
				if (std::fabs(p.GetX()) > half * 0.95f || std::fabs(p.GetY()) > half * 0.95f)
				{
					continue;
				}
				const float u0 = (p.GetX() / half + 1) * 0.5f, v0 = (1 - p.GetY() / half) * 0.5f;
				float u = 2 * u0 + 0.25f, v = 2 * v0;
				u -= std::floor(u);
				v -= std::floor(v);
				if (edge_distance(u, cells) < 0.3f || edge_distance(v, cells) < 0.3f)
				{
					continue;
				}
				uint8_t rgb[3];
				checker_rgb(tex, cells, int(u * tex), int(v * tex), rgb);
				const float tint[3] = { 1, 0.5f, 1 };
				const uint8_t* got = texel(picture, x, y);
				for (int c = 0; c < 3; ++c)
				{
					const float value = rgb[c] / 255.0f;
					EXPECT_NEAR(to_byte((srgb ? srgb_to_linear(value) : value) * tint[c]), got[c], 3) << x << ", " << y << " channel " << c;
				}
				++checked;
			}
		}
		EXPECT_GT(checked, 30);

		//	Shading normals: unbumped the plane's +z; with a normal map, the
		//	raster's (map * 2 - 1) * (s, s, 1) in the tangent frame
		renderer->SetDebugView(e_optxDebugNormal);
		const gtPicture flat = view.Render();
		save_if_asked(flat, "normal_flat");
		EXPECT_NEAR(128, texel(flat, c_Size / 2, c_Size / 2)[0], 1);
		EXPECT_NEAR(128, texel(flat, c_Size / 2, c_Size / 2)[1], 1);
		EXPECT_EQ(255, texel(flat, c_Size / 2, c_Size / 2)[2]);

		//	the checker as a "normal map": each cell a constant tangent-space normal
		material->NormalsData().m_pNormalMap = checker;
		material->NormalsData().m_BumpScale = 0.75f;
		const gtPicture bumped = view.Render();
		save_if_asked(bumped, "normal_mapped");
		ASSERT_TRUE(bumped.Valid());
		//	The plane's tangent frame, from its vertices
		const gvk::tmeshFrag* fragment = nullptr;
		for (g3dSceneNode* node : builder.Scene().GetLayers()[0]->GetRootNode()->GetChildren())
		{
			if (!fragment)
			{
				fragment = dynamic_cast<const gvk::tmeshFrag*>(node->GetFragment());
			}
		}
		ASSERT_NE(nullptr, fragment);
		const g3dType::BumpTex1Vertex* vertices =
			reinterpret_cast<const g3dType::BumpTex1Vertex*>(fragment->GetVertexBuffer()->GetVertexBufferCopy());
		ASSERT_NE(nullptr, vertices);
		const maPoint3d S = vertices[0].m_S, T = vertices[0].m_T;	// world = object here (scaled by half: normalized below)
		checked = 0;
		for (int y = 2; y < c_Size; y += 5)
		{
			for (int x = 2; x < c_Size; x += 5)
			{
				const maPoint3d p = plane_point(builder.Camera(), x, y);
				if (std::fabs(p.GetX()) > half * 0.95f || std::fabs(p.GetY()) > half * 0.95f)
				{
					continue;
				}
				const float u0 = (p.GetX() / half + 1) * 0.5f, v0 = (1 - p.GetY() / half) * 0.5f;
				float u = 2 * u0 + 0.25f, v = 2 * v0;
				u -= std::floor(u);
				v -= std::floor(v);
				if (edge_distance(u, cells) < 0.3f || edge_distance(v, cells) < 0.3f)
				{
					continue;
				}
				uint8_t rgb[3];
				checker_rgb(tex, cells, int(u * tex), int(v * tex), rgb);
				const float n[3] = { (rgb[0] / 255.0f * 2 - 1) * 0.75f, (rgb[1] / 255.0f * 2 - 1) * 0.75f, rgb[2] / 255.0f * 2 - 1 };
				//	tangents scale with the node (object to world: x and y by half)
				float w[3] = { n[0] * S.GetX() * half + n[1] * T.GetX() * half,
							   n[0] * S.GetY() * half + n[1] * T.GetY() * half,
							   n[0] * S.GetZ() * half + n[1] * T.GetZ() * half + n[2] };
				float length = std::sqrt(w[0] * w[0] + w[1] * w[1] + w[2] * w[2]);
				if (w[2] / length < 1e-3f)
				{
					//	below the horizon (the odd cells' blue is low): bent back
					//	to just above it, as the tracer does
					for (float& value : w)
					{
						value /= length;
					}
					w[2] += 1e-3f - w[2];
					length = std::sqrt(w[0] * w[0] + w[1] * w[1] + w[2] * w[2]);
				}
				const uint8_t* got = texel(bumped, x, y);
				for (int c = 0; c < 3; ++c)
				{
					EXPECT_NEAR(to_byte(w[c] / length * 0.5f + 0.5f), got[c], 3) << x << ", " << y << " channel " << c;
				}
				++checked;
			}
		}
		EXPECT_GT(checked, 20);
	});
}

TEST(g3dMaterialsOptiX, CutOutsAreSeenThroughAndCastHoledShadows)
{
	gtBackend::RunOptiX([]()
	{
		gtTempDir dir;
		gtSceneBuilder builder;
		add_floor(builder);
		const int tex = 64, cells = 8;
		matTexture* checker = builder.Keep(gtScene::CheckerTexture(dir.GetPath(), tex, tex, cells, true));
		matMaterial* mask = builder.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		gtScene::SetTexture(mask, effPbrData::c_BaseColorMap, checker);
		gtScene::SetInt(mask, effPbrData::c_AlphaMode, effPbrData::e_Mask);
		gtScene::SetFloat(mask, effPbrData::c_AlphaCutoff, 0.5f);
		const float half = 8.0f, height = 1.0f;
		g3dSceneNode* plane = builder.Add(gtScene::Plane(), mask, scale_translate(half, 0, 0, height));
		plane->GetFragment()->SetCastsShadow(true);
		look_down(builder.Camera(), 0, 0, 20);
		EXPECT_TRUE(g3dGasCacheOptiX::NeedsAnyHit(*dynamic_cast<gvk::tmeshFrag*>(plane->GetFragment())));

		//	Seen through: base colour of the checker in the even cells, the
		//	floor's grey (0.5) in the odd ones
		std::unique_ptr<g3dTraceRendererOptiX> debug = traced(g3dSceneRendererCreate::CreateDefaultRenderer());
		ASSERT_TRUE(debug);
		debug->SetDebugView(e_optxDebugBaseColor);
		debug->SetSampleLimit(1);
		View debug_view(builder, debug.get());
		const gtPicture seen = debug_view.Render();
		save_if_asked(seen, "seen_through");
		int holes = 0, solid = 0;
		for (int y = 1; y < c_Size; y += 3)
		{
			for (int x = 1; x < c_Size; x += 3)
			{
				const maPoint3d p = plane_point(builder.Camera(), x, y, height);
				const float u = (p.GetX() / half + 1) * 0.5f, v = (1 - p.GetY() / half) * 0.5f;
				if (u <= 0.02f || u >= 0.98f || v <= 0.02f || v >= 0.98f || edge_distance(u, cells) < 0.2f ||
					edge_distance(v, cells) < 0.2f)
				{
					continue;
				}
				const bool odd = ((int(u * cells) + int(v * cells)) & 1) != 0;
				const uint8_t* got = texel(seen, x, y);
				if (odd)
				{
					EXPECT_NEAR(128, got[0], 2) << x << ", " << y;
					EXPECT_NEAR(128, got[2], 2) << x << ", " << y;
					++holes;
				}
				else
				{
					EXPECT_NEAR(210, got[2], 3) << x << ", " << y;	// a little of the next mip
					++solid;
				}
			}
		}
		EXPECT_GT(holes, 20);
		EXPECT_GT(solid, 20);

		//	The shadow on the floor has the same holes: a light far above, the
		//	plane seen only by shadow rays
		plane->GetFragment()->SetShadowHull(true);
		const float light_height = 400.0f;
		projector(builder, light_height, 30.0f);
		std::unique_ptr<g3dTraceRendererOptiX> lit = traced(g3dSceneRendererCreate::CreateIlluminationRenderer());
		ASSERT_TRUE(lit);
		lit->SetBounces(0);
		lit->SetSampleLimit(1);
		View lit_view(builder, lit.get());
		const gtPicture shadow = lit_view.Render();
		save_if_asked(shadow, "holed_shadow");
		int lit_holes = 0, shadowed = 0;
		const float scale = light_height / (light_height - height);	// the shadow is a little larger
		for (int y = 1; y < c_Size; y += 3)
		{
			for (int x = 1; x < c_Size; x += 3)
			{
				const maPoint3d p = plane_point(builder.Camera(), x, y, 0.0f);
				const float u = (p.GetX() / (half * scale) + 1) * 0.5f, v = (1 - p.GetY() / (half * scale)) * 0.5f;
				if (u <= 0.02f || u >= 0.98f || v <= 0.02f || v >= 0.98f || edge_distance(u, cells) < 0.2f ||
					edge_distance(v, cells) < 0.2f)
				{
					continue;
				}
				const bool odd = ((int(u * cells) + int(v * cells)) & 1) != 0;
				const uint8_t* got = texel(shadow, x, y);
				if (odd)
				{
					EXPECT_GT(got[0], 150) << "a hole lets the light through: " << x << ", " << y;
					++lit_holes;
				}
				else
				{
					EXPECT_EQ(0, got[0]) << "shadowed: " << x << ", " << y;
					++shadowed;
				}
			}
		}
		EXPECT_GT(lit_holes, 20);
		EXPECT_GT(shadowed, 20);
	});
}

TEST(g3dMaterialsOptiX, HalfTransparentConvergesToHalfShadowAndOpaqueSkipsAnyHit)
{
	gtBackend::RunOptiX([]()
	{
		gtSceneBuilder builder;
		g3dSceneNode* floor = add_floor(builder);
		EXPECT_FALSE(g3dGasCacheOptiX::NeedsAnyHit(*dynamic_cast<gvk::tmeshFrag*>(floor->GetFragment())))
			<< "an opaque material disables the any-hit program";
		matMaterial* glass = builder.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		gtScene::SetColor(glass, effPbrData::c_BaseColor, maFloatRGBA(1, 1, 1, 0.5f));
		gtScene::SetInt(glass, effPbrData::c_AlphaMode, effPbrData::e_Blend);
		g3dSceneNode* plane = builder.Add(gtScene::Plane(), glass, scale_translate(3.0f, 0, 0, 2.0f));
		plane->GetFragment()->SetCastsShadow(true);
		plane->GetFragment()->SetShadowHull(true);
		EXPECT_TRUE(g3dGasCacheOptiX::NeedsAnyHit(*dynamic_cast<gvk::tmeshFrag*>(plane->GetFragment())));
		projector(builder, 50.0f, 60.0f);
		look_down(builder.Camera(), 0, 0, 30);

		//	the opaque floor's build input has any-hit disabled, the glass's not
		g3dGasCacheOptiX* cache = g3dGasCacheOptiX::Get();
		ASSERT_NE(nullptr, cache);
		const g3dGasCacheOptiX::Gas* floor_gas = cache->Ensure(*dynamic_cast<gvk::tmeshFrag*>(floor->GetFragment()));
		const g3dGasCacheOptiX::Gas* glass_gas = cache->Ensure(*dynamic_cast<gvk::tmeshFrag*>(plane->GetFragment()));
		ASSERT_TRUE(floor_gas && glass_gas);
		EXPECT_FALSE(floor_gas->m_Inputs[0].m_bAnyHit);
		EXPECT_TRUE(glass_gas->m_Inputs[0].m_bAnyHit);

		std::unique_ptr<g3dTraceRendererOptiX> lit = traced(g3dSceneRendererCreate::CreateIlluminationRenderer());
		ASSERT_TRUE(lit);
		lit->SetBounces(0);
		lit->SetSamplesPerFrame(256);
		lit->SetSampleLimit(256);
		View view(builder, lit.get());
		const gtPicture picture = view.Render();
		save_if_asked(picture, "half_shadow");
		ASSERT_TRUE(picture.Valid());
		const int centre = texel(picture, c_Size / 2, c_Size / 2)[0];
		//	outside the plane's shadow (it is 3 x 50/48 across), the same light
		int open = 0, count = 0;
		for (int y = 0; y < c_Size; ++y)
		{
			for (int x = 0; x < c_Size; ++x)
			{
				const maPoint3d p = plane_point(builder.Camera(), x, y);
				if (std::fabs(p.GetX()) > 4.0f && std::fabs(p.GetX()) < 6.0f && std::fabs(p.GetY()) < 1.0f)
				{
					open += texel(picture, x, y)[0];
					++count;
				}
			}
		}
		ASSERT_GT(count, 0);
		const double unshadowed = double(open) / count;
		EXPECT_GT(unshadowed, 100.0);
		EXPECT_NEAR(unshadowed * 0.5, double(centre), unshadowed * 0.06) << "half the light gets through";
	});
}

TEST(g3dMaterialsOptiX, ProjectorTexturesColourIlluminationNotTheMask)
{
	gtBackend::RunOptiX([]()
	{
		gtTempDir dir;
		gtSceneBuilder builder;
		add_floor(builder);
		const float height = 20.0f;
		g3dProjectedLight* light = projector(builder, height, 60.0f);
		look_down(builder.Camera(), 0, 0, 30);
		const int tex = 64, cells = 4;
		matTexture* checker = builder.Keep(gtScene::CheckerTexture(dir.GetPath(), tex, tex, cells));
		const gvk::vkdImage* image = gvk::g3dVkTextureUtil::GetImage(checker);
		ASSERT_NE(nullptr, image);
		const bool srgb = gvk::vkdFormatInfoOf(image->Format()).m_bSrgb;

		auto render = [&](float i_LightSize, matTexture* i_pTexture, unsigned i_nSamples)
		{
			light->SetLightSize(i_LightSize);
			light->SetTexture(i_pTexture);
			std::unique_ptr<g3dTraceRendererOptiX> lit = traced(g3dSceneRendererCreate::CreateIlluminationRenderer());
			lit->SetBounces(0);
			lit->SetSamplesPerFrame(i_nSamples);
			lit->SetSampleLimit(i_nSamples);
			View view(builder, lit.get());
			return view.Render();
		};

		for (const float size : { 0.0f, 4.0f })
		{
			SCOPED_TRACE(size > 0 ? "area light" : "delta light");
			const unsigned samples = size > 0 ? 512 : 1;
			const gtPicture plain = render(size, nullptr, samples);
			const gtPicture textured = render(size, checker, samples);
			save_if_asked(textured, size > 0 ? "gobo_area" : "gobo");
			ASSERT_TRUE(plain.Valid() && textured.Valid());
			int checked = 0;
			for (int y = 1; y < c_Size; y += 3)
			{
				for (int x = 1; x < c_Size; x += 3)
				{
					float u = 0, v = 0;
					if (!projector_uv(*light, plane_point(builder.Camera(), x, y), u, v) || edge_distance(u, cells) < 0.2f ||
						edge_distance(v, cells) < 0.2f)
					{
						continue;
					}
					uint8_t rgb[3];
					checker_rgb(tex, cells, int(u * tex), int(v * tex), rgb);
					const uint8_t* without = texel(plain, x, y);
					const uint8_t* with = texel(textured, x, y);
					if (without[0] < 60)
					{
						continue;
					}
					for (int c = 0; c < 3; ++c)
					{
						const float value = rgb[c] / 255.0f;
						const float tint = srgb ? srgb_to_linear(value) : value;
						EXPECT_NEAR(without[c] * tint, with[c], size > 0 ? 6.0f : 2.0f) << x << ", " << y << " channel " << c;
					}
					++checked;
				}
			}
			EXPECT_GT(checked, 20);
		}

		//	a ramp (a float texture without mips) colours it too: its red
		//	rises across the light (dark blue to pale yellow), its blue
		//	differs from the checker's
		matTexture* ramp = builder.Keep(gtScene::RampTexture(64));
		const gtPicture plain = render(0.0f, nullptr, 1);
		const gtPicture ramped = render(0.0f, ramp, 1);
		save_if_asked(ramped, "ramp");
		std::vector<std::pair<float, float>> red_by_u;	// (u, red / unlit red)
		for (int x = 0; x < c_Size; ++x)
		{
			float u = 0, v = 0;
			const int without = texel(plain, x, c_Size / 2)[0];
			if (!projector_uv(*light, plane_point(builder.Camera(), x, c_Size / 2), u, v) || without < 60)
			{
				continue;
			}
			red_by_u.push_back(std::make_pair(u, float(texel(ramped, x, c_Size / 2)[0]) / without));
		}
		std::sort(red_by_u.begin(), red_by_u.end());
		ASSERT_GT(red_by_u.size(), 10u);
		int falls = 0;
		for (size_t i = 1; i < red_by_u.size(); ++i)
		{
			falls += red_by_u[i].second + 0.02f < red_by_u[i - 1].second;
		}
		EXPECT_EQ(0, falls) << "the ramp's red rises with u";
		EXPECT_LT(red_by_u.front().second + 0.3f, red_by_u.back().second);

		//	The Shadow Mask doesn't change with the texture
		auto mask = [&](matTexture* i_pTexture)
		{
			light->SetLightSize(0.0f);
			light->SetTexture(i_pTexture);
			std::unique_ptr<g3dTraceRendererOptiX> renderer = traced(g3dSceneRendererCreate::CreateShadowMaskRenderer());
			renderer->SetSampleLimit(1);
			View view(builder, renderer.get());
			return view.Render();
		};
		const gtPicture plain_mask = mask(nullptr);
		const gtPicture textured_mask = mask(checker);
		ASSERT_TRUE(plain_mask.Valid() && textured_mask.Valid());
		EXPECT_EQ(plain_mask.m_Rgba, textured_mask.m_Rgba);
		light->SetTexture(nullptr);
	});
}

TEST(g3dMaterialsOptiX, MaterialAndTextureEditsRestart)
{
	gtBackend::RunOptiX([]()
	{
		gtTempDir dir;
		gtSceneBuilder builder;
		add_floor(builder);
		matTexture* checker = builder.Keep(gtScene::CheckerTexture(dir.GetPath(), 32, 32, 4));
		matTexture* other = builder.Keep(gtScene::CheckerTexture(dir.GetPath(), 32, 32, 2));
		matMaterial* material = builder.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		gtScene::SetTexture(material, effPbrData::c_BaseColorMap, checker);
		builder.Add(gtScene::Plane(), material, scale_translate(3.0f, 0, 0, 1.0f));
		matMaterial* unused = builder.Keep(gtScene::ShaderMaterial(effPbrData::c_ShaderName, gtScene::e_Defaults));
		g3dProjectedLight* light = projector(builder, 20.0f, 60.0f);
		light->SetTexture(checker);
		look_down(builder.Camera(), 0, 0, 30);

		std::unique_ptr<g3dTraceRendererOptiX> renderer = traced(g3dSceneRendererCreate::CreateIlluminationRenderer());
		ASSERT_TRUE(renderer);
		renderer->SetBounces(1);
		renderer->SetSamplesPerFrame(1);
		renderer->SetSampleLimit(4);
		View view(builder, renderer.get());
		view.Render();
		view.Render();
		int restarts = renderer->GetRestartCount();
		auto expect_restart = [&](const char* i_pWhat, bool i_bRestarts)
		{
			SCOPED_TRACE(i_pWhat);
			view.Render();
			if (i_bRestarts)
			{
				EXPECT_EQ(++restarts, renderer->GetRestartCount());
				EXPECT_EQ(1u, renderer->GetSampleCount());
			}
			else
			{
				EXPECT_EQ(restarts, renderer->GetRestartCount());
			}
		};

		gtScene::SetColor(unused, effPbrData::c_BaseColor, maFloatRGBA(0.1f, 0.2f, 0.3f, 1));
		expect_restart("a material the scene doesn't use", false);
		gtScene::SetColor(material, effPbrData::c_BaseColor, maFloatRGBA(0.9f, 0.4f, 0.2f, 1));
		expect_restart("material colour", true);
		gvk::g3dVkTextureUtil::GetImage(checker)->MarkWritten();
		expect_restart("texture contents", true);
		light->SetTexture(other);
		expect_restart("projector image", true);
		expect_restart("nothing", false);
	});
}
