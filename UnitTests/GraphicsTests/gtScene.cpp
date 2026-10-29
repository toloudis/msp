/*****************************************************************************
**  gtScene.cpp
**
**      See gtScene.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtScene.hpp"

#ifndef G3D_FRAGMENTCREATE_HPP
#include "Graphics/G3d/g3dFragmentCreate.hpp"
#endif
#ifndef G3D_INDEXPTR_HPP
#include "Graphics/G3d/g3dIndexPtr.hpp"
#endif

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/G3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/G3d/g3dLayer.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif
#ifndef G3D_LIGHTMGR_HPP
#include "Graphics/G3d/g3dLightMgr.hpp"
#endif
#ifndef G3D_POINTLIGHT_HPP
#include "Graphics/G3d/g3dPointLight.hpp"
#endif
#ifndef G3D_DIRECTIONALLIGHT_HPP
#include "Graphics/G3d/g3dDirectionalLight.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/Mat/matShaderMgr.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/Mat/matShaderEffect.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#include <string>
#ifndef G3D_RENDERSTATE_HPP
#include "Graphics/G3d/g3dRenderState.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif
#ifndef MDL_FRAGCREATE_HPP
#include "Graphics/mdl/mdlFragCreate.hpp"
#endif
#ifndef MDL_HAIRINFO_HPP
#include "Graphics/mdl/mdlHairInfo.hpp"
#endif
#ifndef MA_FUNCTIONS_HPP
#include "Core/Ma/maFunctions.hpp"
#endif
#ifndef MA_RAND_HPP
#include "Core/Ma/maRand.hpp"
#endif
#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif
#ifndef G3D_TARGETRENDERER_HPP
#include "Graphics/G3d/g3dTargetRenderer.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef G3D_PLANARREFLECTIONTARGETRENDERER_HPP
#include "Graphics/G3d/g3dPlanarReflectionTargetRenderer.hpp"
#endif
#ifndef G3D_CUBICREFLECTIONTARGETRENDERER_HPP
#include "Graphics/G3d/g3dCubicReflectionTargetRenderer.hpp"
#endif
#ifndef EFF_REFLDATA_HPP
#include "Graphics/Eff/effReflData.hpp"
#endif
#ifndef SC_OBJECT_HPP
#include "Graphics/Sc/scObject.hpp"
#endif

#include <gtest/gtest.h>
#include <stb_image_write.h>

#include "Graphics/G2d/private/rgbe.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>

g3dFragment* gtMesh::CreateFragment(matMaterial* i_pMaterial, bool i_bMorphable) const
{
	if (g3dIndexPtr::Needs32Bit(VertexCount()))
	{
		return g3dFragmentCreate::CreateFragment(m_Positions.data(), m_Normals.data(), m_UVs.data(), VertexCount(),
												 g3dIndexPtr(m_Indices.data()), IndexCount(), i_pMaterial, i_bMorphable);
	}
	std::vector<uint16_t> indices(m_Indices.begin(), m_Indices.end());
	return g3dFragmentCreate::CreateFragment(m_Positions.data(), m_Normals.data(), m_UVs.data(), VertexCount(),
											 g3dIndexPtr(indices.data()), IndexCount(), i_pMaterial, i_bMorphable);
}

namespace
{
	//	A (i_Columns + 1) x (i_Rows + 1) vertex lattice, two triangles per cell
	void lattice_indices(int i_Columns, int i_Rows, std::vector<uint32_t>& o_Indices)
	{
		const int stride = i_Columns + 1;
		for (int r = 0; r < i_Rows; r++)
		{
			for (int c = 0; c < i_Columns; c++)
			{
				const uint32_t a = r * stride + c;
				const uint32_t b = a + 1;
				const uint32_t d = a + stride;
				const uint32_t e = d + 1;
				o_Indices.insert(o_Indices.end(), { a, d, b, b, d, e });
			}
		}
	}
}

gtMesh gtScene::Sphere(int i_Rings, int i_Segments)
{
	gtMesh mesh;
	const double pi = 3.14159265358979323846;
	for (int r = 0; r <= i_Rings; r++)
	{
		const double theta = pi * r / i_Rings;			// from +Z down
		for (int s = 0; s <= i_Segments; s++)
		{
			const double phi = 2 * pi * s / i_Segments;
			const float x = (float)(std::sin(theta) * std::cos(phi));
			const float y = (float)(std::sin(theta) * std::sin(phi));
			const float z = (float)std::cos(theta);
			mesh.m_Positions.push_back(maPoint3d(x, y, z));
			mesh.m_Normals.push_back(maPoint3d(x, y, z));
			mesh.m_UVs.push_back(maPoint2d((float)s / i_Segments, (float)r / i_Rings));
		}
	}
	lattice_indices(i_Segments, i_Rings, mesh.m_Indices);
	return mesh;
}

gtMesh gtScene::Grid(int i_N)
{
	gtMesh mesh;
	for (int r = 0; r <= i_N; r++)
	{
		for (int c = 0; c <= i_N; c++)
		{
			const float u = (float)c / i_N;
			const float v = (float)r / i_N;
			mesh.m_Positions.push_back(maPoint3d(-1 + 2 * u, 1 - 2 * v, 0));
			mesh.m_Normals.push_back(maPoint3d(0, 0, 1));
			mesh.m_UVs.push_back(maPoint2d(u, v));
		}
	}
	lattice_indices(i_N, i_N, mesh.m_Indices);
	return mesh;
}

gtMesh gtScene::GridWithHoles(int i_N)
{
	gtMesh mesh = Grid(i_N);
	std::vector<uint32_t> kept;
	for (int r = 0; r < i_N; r++)
	{
		for (int c = 0; c < i_N; c++)
		{
			if (((c + r) & 1) != 0)
				continue;
			const size_t quad = ((size_t)r * i_N + c) * 6;
			kept.insert(kept.end(), mesh.m_Indices.begin() + quad, mesh.m_Indices.begin() + quad + 6);
		}
	}
	mesh.m_Indices = kept;
	return mesh;
}

gtMesh gtScene::Plane()
{
	return Grid(1);
}

namespace
{
	float float_value(gtScene::Values i_Values, float i_Default, int i_Index)
	{
		switch (i_Values)
		{
			case gtScene::e_Zero:	return 0.0f;
			case gtScene::e_Huge:	return 1.0e6f;
			case gtScene::e_Defaults:
			case gtScene::e_Tinted:	return i_Default;
			default:				return i_Default * 1.37f + 0.11f + 0.01f * i_Index;
		}
	}

	maFloatRGBA color_value(gtScene::Values i_Values, int i_Index)
	{
		switch (i_Values)
		{
			case gtScene::e_Zero:	return maFloatRGBA(0, 0, 0, 0);
			case gtScene::e_Huge:	return maFloatRGBA(1, 1, 1, 1);
			case gtScene::e_Tinted:	return maFloatRGBA(0.5f - 0.08f * (i_Index % 4), 0.3f + 0.06f * (i_Index % 4), 0.2f, 1.0f);
			default:				return maFloatRGBA(0.2f + 0.05f * i_Index, 0.5f, 0.8f - 0.05f * i_Index, 0.9f);
		}
	}
}

matMaterial* gtScene::ShaderMaterial(const std::string& i_Shader, Values i_Values, const std::vector<matTexture*>& i_Textures)
{
	matMaterial* material = new matMaterial();
	shared_ptr<effShaderParams> params(new effShaderParams());
	params->SetShaderName(i_Shader);
	material->SetShaderParams(params);
	matShaderEffect* effect = params->GetShader();
	EXPECT_TRUE(effect) << i_Shader;
	if (effect)
		effect->BuildPrtyObject(params.get());
	std::vector<effParamFloat*> floats;
	params->GetAllFloatParams(floats);
	std::vector<effParamColor*> colors;
	params->GetAllColorParams(colors);
	if (i_Values != e_Defaults)
	{
		for (size_t i = 0; i < floats.size(); i++)
			floats[i]->Property().SetValue(float_value(i_Values, floats[i]->GetProperty().GetValue(), (int)i));
		for (size_t i = 0; i < colors.size(); i++)
			colors[i]->Property().SetValue(color_value(i_Values, (int)i));
	}
	std::vector<effParamTexture*> textures;
	params->GetAllTextureParams(textures);
	for (size_t i = 0; i < textures.size() && i < i_Textures.size(); i++)
		textures[i]->SetTexture(i_Textures[i]);
	return material;
}

matMaterial* gtScene::DataMaterial(const std::string& i_Shader, const std::function<void(effShaderData*)>& i_Setup)
{
	effShaderData* data = matShaderMgr::CreateData(i_Shader);
	if (i_Setup)
		i_Setup(data);
	return new matMaterial(i_Shader, data);
}

void gtScene::SetPhong(effShaderData* io_pData, Values i_Values)
{
	effPhongData* phong = dynamic_cast<effPhongData*>(io_pData);
	EXPECT_TRUE(phong != NULL);
	if (!phong)
		return;
	phong->m_ColorAmbient = color_value(i_Values, 0);
	phong->m_ColorDiffuse = color_value(i_Values, 1);
	phong->m_ColorSpecular = color_value(i_Values, 2);
	phong->m_ColorEmissive = color_value(i_Values, 3);
	phong->m_SpecularPower = float_value(i_Values, phong->m_SpecularPower, 0);
	phong->m_Transparency = i_Values == e_Zero ? 0.0f : (i_Values == e_Huge ? 1.0f : 0.7f);
}

void gtScene::SetFloat(matMaterial* io_pMaterial, const char* i_pName, float i_Value)
{
	std::vector<effParamFloat*> floats;
	io_pMaterial->GetShaderParams(0)->GetAllFloatParams(floats);
	for (effParamFloat* param : floats)
	{
		if (param->GetName() == i_pName)
		{
			param->Property().SetValue(i_Value);
			return;
		}
	}
	ADD_FAILURE() << "no parameter " << i_pName;
}

void gtScene::SetInt(matMaterial* io_pMaterial, const char* i_pName, int i_Value)
{
	std::vector<effParamInt*> ints;
	io_pMaterial->GetShaderParams(0)->GetAllIntParams(ints);
	for (effParamInt* param : ints)
	{
		if (param->GetName() == i_pName)
		{
			param->Property().SetValue(i_Value);
			return;
		}
	}
	ADD_FAILURE() << "no parameter " << i_pName;
}

void gtScene::SetTexture(matMaterial* io_pMaterial, const char* i_pName, matTexture* i_pTexture)
{
	std::vector<effParamTexture*> textures;
	io_pMaterial->GetShaderParams(0)->GetAllTextureParams(textures);
	std::string names;
	for (effParamTexture* param : textures)
	{
		if (param->GetName() == i_pName)
		{
			param->SetTexture(i_pTexture);
			return;
		}
		names += " " + param->GetName();
	}
	ADD_FAILURE() << "no texture parameter " << i_pName << " (has" << names << ")";
}

void gtScene::SetColor(matMaterial* io_pMaterial, const char* i_pName, const maFloatRGBA& i_Value)
{
	std::vector<effParamColor*> colors;
	io_pMaterial->GetShaderParams(0)->GetAllColorParams(colors);
	for (effParamColor* param : colors)
	{
		if (param->GetName() == i_pName)
		{
			param->Property().SetValue(i_Value);
			return;
		}
	}
	ADD_FAILURE() << "no parameter " << i_pName;
}

namespace
{
	//	i_Rgba (RGBA8) written to i_Path as a PNG and loaded through
	//	matTextureMgr, mips generated
	matTexture* load_png(const std::filesystem::path& i_Path, int i_Width, int i_Height, const std::vector<uint8_t>& i_Rgba)
	{
		if (!stbi_write_png(i_Path.u8string().c_str(), i_Width, i_Height, 4, i_Rgba.data(), i_Width * 4))
		{
			ADD_FAILURE() << "can't write " << i_Path.u8string();
			return NULL;
		}
		fsLocator locator;
		fsFileUtil::UnicodeStringToLocator(i_Path.u8string(), locator);
		matTexture* texture = matTextureMgr::LoadTexture(locator, TEXTURE_TYPE_2D, true);
		EXPECT_TRUE(texture != NULL) << i_Path.u8string();
		return texture;
	}
}

matTexture* gtScene::CheckerTexture(const std::filesystem::path& i_Dir, int i_Width, int i_Height, int i_Cells,
									 bool i_bClearOddCells)
{
	std::vector<uint8_t> rgba((size_t)i_Width * i_Height * 4);
	for (int y = 0; y < i_Height; y++)
	{
		for (int x = 0; x < i_Width; x++)
		{
			const int cx = x * i_Cells / i_Width;
			const int cy = y * i_Cells / i_Height;
			uint8_t* p = &rgba[((size_t)y * i_Width + x) * 4];
			const bool odd = ((cx + cy) & 1) != 0;
			p[0] = (uint8_t)(odd ? 230 : 40 + 20 * (cx % 8));
			p[1] = (uint8_t)(odd ? 200 : 60 + 25 * (cy % 8));
			p[2] = (uint8_t)(odd ? 60 : 210);
			p[3] = (uint8_t)(odd && i_bClearOddCells ? 0 : 255);
		}
	}
	return load_png(i_Dir / ("checker_" + std::to_string(i_Width) + "x" + std::to_string(i_Height) + "_" + std::to_string(i_Cells)
					+ (i_bClearOddCells ? "_clear" : "") + ".png"),
					i_Width, i_Height, rgba);
}

matTexture* gtScene::WebTexture(const std::filesystem::path& i_Dir, int i_Size, int i_Strands)
{
	const int cell = i_Size / i_Strands;
	std::vector<uint8_t> rgba((size_t)i_Size * i_Size * 4);
	for (int y = 0; y < i_Size; y++)
	{
		for (int x = 0; x < i_Size; x++)
		{
			const bool strand = (x % cell) < cell / 4 || (y % cell) < cell / 4;
			uint8_t* p = &rgba[((size_t)y * i_Size + x) * 4];
			p[0] = p[1] = p[2] = strand ? 255 : 0;
			p[3] = 255;
		}
	}
	return load_png(i_Dir / ("web_" + std::to_string(i_Size) + "_" + std::to_string(i_Strands) + ".png"),
					i_Size, i_Size, rgba);
}

matTexture* gtScene::CheckerMaskTexture(const std::filesystem::path& i_Dir, int i_Width, int i_Height, int i_Cells)
{
	std::vector<uint8_t> rgba((size_t)i_Width * i_Height * 4);
	for (int y = 0; y < i_Height; y++)
	{
		for (int x = 0; x < i_Width; x++)
		{
			const int cx = x * i_Cells / i_Width;
			const int cy = y * i_Cells / i_Height;
			uint8_t* p = &rgba[((size_t)y * i_Width + x) * 4];
			const bool odd = ((cx + cy) & 1) != 0;
			p[0] = p[1] = p[2] = odd ? 0 : 255;
			p[3] = 255;
		}
	}
	return load_png(i_Dir / ("checker_mask_" + std::to_string(i_Width) + "x" + std::to_string(i_Height) + "_" + std::to_string(i_Cells) + ".png"),
					i_Width, i_Height, rgba);
}

namespace
{
	//	Smooth bumps, 0..1: i_Bumps x i_Bumps of them over the texture
	float bump_height(float i_U, float i_V, int i_Bumps)
	{
		const float k = 6.2831853f * i_Bumps;
		return 0.5f + 0.5f * sinf(k * i_U) * sinf(k * i_V);
	}
}

matTexture* gtScene::HeightTexture(const std::filesystem::path& i_Dir, int i_Size, int i_Bumps)
{
	std::vector<uint8_t> rgba((size_t)i_Size * i_Size * 4);
	for (int y = 0; y < i_Size; y++)
	{
		for (int x = 0; x < i_Size; x++)
		{
			const uint8_t h = (uint8_t)(255.0f * bump_height((x + 0.5f) / i_Size, (y + 0.5f) / i_Size, i_Bumps) + 0.5f);
			uint8_t* p = &rgba[((size_t)y * i_Size + x) * 4];
			p[0] = p[1] = p[2] = h;
			p[3] = 255;
		}
	}
	return load_png(i_Dir / ("height_" + std::to_string(i_Size) + "_" + std::to_string(i_Bumps) + ".png"), i_Size, i_Size, rgba);
}

matTexture* gtScene::NormalTexture(const std::filesystem::path& i_Dir, int i_Size, int i_Bumps)
{
	std::vector<uint8_t> rgba((size_t)i_Size * i_Size * 4);
	const float d = 1.0f / i_Size;
	for (int y = 0; y < i_Size; y++)
	{
		for (int x = 0; x < i_Size; x++)
		{
			//	the height's slope, as a tangent-space normal packed into 0..255
			const float u = (x + 0.5f) * d, v = (y + 0.5f) * d;
			const float dx = (bump_height(u + d, v, i_Bumps) - bump_height(u - d, v, i_Bumps)) * 0.5f * i_Size / (6.2831853f * i_Bumps);
			const float dy = (bump_height(u, v + d, i_Bumps) - bump_height(u, v - d, i_Bumps)) * 0.5f * i_Size / (6.2831853f * i_Bumps);
			const float length = sqrtf(dx * dx + dy * dy + 1.0f);
			uint8_t* p = &rgba[((size_t)y * i_Size + x) * 4];
			p[0] = (uint8_t)(127.5f + 127.5f * -dx / length);
			p[1] = (uint8_t)(127.5f + 127.5f * -dy / length);
			p[2] = (uint8_t)(127.5f + 127.5f * 1.0f / length);
			p[3] = 255;
		}
	}
	return load_png(i_Dir / ("normal_" + std::to_string(i_Size) + "_" + std::to_string(i_Bumps) + ".png"), i_Size, i_Size, rgba);
}

matTexture* gtScene::RampTexture(int i_Width)
{
	std::vector<float> rgba((size_t)i_Width * 4);
	for (int x = 0; x < i_Width; x++)
	{
		//	dark blue, through red-orange, to pale yellow
		const float u = (x + 0.5f) / i_Width;
		float* p = &rgba[(size_t)x * 4];
		p[0] = (20 + 225 * std::min(1.0f, 1.6f * u)) / 255.0f;
		p[1] = (30 + 200 * u * u) / 255.0f;
		p[2] = (120 - 80 * u + 100 * u * u * u) / 255.0f;
		p[3] = 1.0f;
	}
	g2dPFD pfd(g2dPFD::e_RGBA32f, 32 * 4);
	matTexture* texture = matTextureMgr::CreateTexture(i_Width, 1, &pfd, false, rgba.data());
	EXPECT_TRUE(texture != NULL);
	return texture;
}

matTexture* gtScene::HdrTexture(const std::filesystem::path& i_Dir, int i_Width, int i_Height, float i_Peak)
{
	//	the sun: a disc around (u, v) = (0.295, 0.43), which the camera (at +Z
	//	looking down -Z, u = 0.25 in the shaders' CartesianToPolar) sees
	//	about 16 degrees right and 12 up, beside the builders' spheres
	const float sun_u = 0.295f, sun_v = 0.43f, sun_radius = 0.015f;
	std::vector<float> rgb((size_t)i_Width * i_Height * 3);
	for (int y = 0; y < i_Height; y++)
	{
		const float v = (y + 0.5f) / i_Height;
		for (int x = 0; x < i_Width; x++)
		{
			const float u = (x + 0.5f) / i_Width;
			float* p = &rgb[((size_t)y * i_Width + x) * 3];
			//	no hard edges anywhere (see the sun below): the sky, bright pale
			//	blue at the horizon to deep blue at the top, blends into the
			//	ground, brown and darker downwards, with soft stripes around
			const float t_sky = std::min(v / 0.5f, 1.0f);
			const float t_ground = std::max((v - 0.5f) / 0.5f, 0.0f);
			const float stripe = 0.85f + 0.15f * sinf(u * 6.2831853f * 8.0f);
			const float sky[3] = { 0.15f + 0.6f * t_sky, 0.3f + 0.55f * t_sky, 0.9f + 0.3f * t_sky };
			const float ground[3] = { (0.35f - 0.25f * t_ground) * stripe, (0.25f - 0.18f * t_ground) * stripe, (0.12f - 0.08f * t_ground) * stripe };
			float blend = std::min(std::max((v - 0.45f) / 0.1f, 0.0f), 1.0f);
			blend = blend * blend * (3.0f - 2.0f * blend);
			for (int c = 0; c < 3; c++)
				p[c] = sky[c] + (ground[c] - sky[c]) * blend;
			//	a soft (Gaussian) edge, as a real sun has: a hard 50:1 step would
			//	turn the last-bit differences between the backends' direction
			//	to lat-long maths into a whole step on the pixels at its edge
			const float du = u - sun_u, dv = (v - sun_v) * 0.5f;
			const float sun = expf(-(du * du + dv * dv) / (sun_radius * sun_radius));
			p[0] += (i_Peak - p[0]) * sun;
			p[1] += (i_Peak - p[1]) * sun;
			p[2] += (i_Peak * 0.8f - p[2]) * sun;
		}
	}
	//	"#?RADIANCE": GraphicsDX11's texture manager only takes files that start so
	std::ostringstream os(std::ios::binary);
	rgbe_header_info info = {};
	info.valid = RGBE_VALID_PROGRAMTYPE;
	strncpy(info.programtype, "RADIANCE", sizeof(info.programtype) - 1);
	RGBE_WriteHeader(os, i_Width, i_Height, &info);
	RGBE_WritePixels_RLE(os, rgb.data(), i_Width, i_Height);
	const std::string bytes = os.str();

	std::ostringstream name;
	name << "sky_" << i_Width << "x" << i_Height << "_" << i_Peak << ".hdr";
	const std::filesystem::path path = i_Dir / name.str();
	{
		std::ofstream out(path, std::ios::binary);
		out.write(bytes.data(), (std::streamsize)bytes.size());
		if (!out)
		{
			ADD_FAILURE() << "can't write " << path.u8string();
			return NULL;
		}
	}
	fsLocator locator;
	fsFileUtil::UnicodeStringToLocator(path.u8string(), locator);
	matTexture* texture = matTextureMgr::LoadTexture(locator, TEXTURE_TYPE_2D, true);
	EXPECT_TRUE(texture != NULL) << path.u8string();
	return texture;
}

maMatrix4x4 gtScene::Place(float i_X, float i_Y, float i_Z, float i_Scale)
{
	maMatrix4x4 m;
	m.MakeScale(i_Scale, i_Scale, i_Scale);
	m.TranslateBy(i_X, i_Y, i_Z);
	return m;
}

matMaterial* gtScene::OpaquePhong(const std::string& i_Shader, Values i_Values)
{
	return DataMaterial(i_Shader, [i_Values](effShaderData* i_pData)
	{
		SetPhong(i_pData, i_Values);
		if (effPhongData* phong = dynamic_cast<effPhongData*>(i_pData))
			phong->m_Transparency = 1.0f;
	});
}

//============================================================================
//	gtSceneBuilder
//============================================================================
gtSceneBuilder::gtSceneBuilder()
{
	m_Camera.SetFOV(45.0f);
	m_Camera.SetAspect(1.0f);
	m_Camera.SetClip(0.1f, 100.0f);
	m_Camera.LookAt(maPoint3d(0.0f, 0.0f, 6.0f), maPoint3d(0, 0, 0), maVector3d(0, 1, 0));

	m_pRoot.reset(new g3dSceneNode());
	m_pLayer = new g3dLayer(m_pRoot.get());
	m_pScene.reset(new g3dScene(m_pLayer));
}

//	A light's hair shadows (HairShadows)
struct gtSceneBuilder::OpacityShadows
{
	g3dProjectedLight*					m_pLight = NULL;
	camCamera							m_Camera;		// the light's shadow camera
	matTexture*							m_pMaps[MAX_OPACITY_MAPS] = {};
	matTexture*							m_pVolume = NULL;
	std::unique_ptr<g3dSceneRenderer>	m_pRenderer;
	std::unique_ptr<g3dTargetRenderer>	m_pTarget;

	//	The maps, volume and target renderer (the renderer stays, as the
	//	light object's does)
	void Release()
	{
		m_pTarget.reset();
		for (int i = 0; i < MAX_OPACITY_MAPS; i++)
		{
			if (m_pMaps[i])
				matTextureMgr::ReleaseTexture(m_pMaps[i]);
			m_pMaps[i] = NULL;
			m_pLight->SetOpacityShadowMap(NULL, i);
		}
		if (m_pVolume)
			matTextureMgr::ReleaseTexture(m_pVolume);
		m_pVolume = NULL;
		m_pLight->SetOpacityVolume(NULL);
	}
};

gtSceneBuilder::~gtSceneBuilder()
{
	m_TargetRenderers.clear();
	for (auto& shadows : m_OpacityShadows)
	{
		shadows->Release();
		shadows->m_pRenderer->ReleaseResources();
	}
	m_OpacityShadows.clear();
	for (auto& renderer : m_DepthRenderers)
		renderer->ReleaseResources();
	m_DepthRenderers.clear();
	for (auto& renderer : m_ReflectionRenderers)
		renderer->ReleaseResources();
	m_ReflectionRenderers.clear();
	m_Reflectors.clear();
	m_pScene.reset();
	m_pRoot.reset();
	m_pOverlayRoot.reset();
	m_pViewerScene.reset();
	m_pViewerRoot.reset();
	m_pEnvironment.reset();
	m_pRenderState.reset();
	m_Fragments.clear();
	m_Materials.clear();
	for (matTexture* texture : m_Textures)
		matTextureMgr::ReleaseTexture(texture);
	for (g3dLight* light : m_Lights)
		g3dLightMgr::DestroyLight(light);
}

matMaterial* gtSceneBuilder::Keep(matMaterial* i_pMaterial)
{
	m_Materials.emplace_back(i_pMaterial);
	return i_pMaterial;
}

matTexture* gtSceneBuilder::Keep(matTexture* i_pTexture)
{
	if (i_pTexture)
		m_Textures.push_back(i_pTexture);
	return i_pTexture;
}

g3dSceneNode* gtSceneBuilder::Add(const gtMesh& i_Mesh, matMaterial* i_pMaterial, const maMatrix4x4& i_World)
{
	g3dFragment* fragment = i_Mesh.CreateFragment(i_pMaterial);
	m_Fragments.emplace_back(fragment);
	g3dSceneNode* node = new g3dSceneNode(fragment, i_World);
	m_pRoot->AddChild(node);
	m_pScene->UpdateWorldData();		// transforms and world boxes, as the scene manager does
	return node;
}

g3dSceneNode* gtSceneBuilder::Hair(const gtHairShape& i_Shape, const maMatrix4x4& i_World)
{
	maRand32 rand(0x2F6B1C3Du);
	mdlHairInfo info;
	info.m_HairName = "gtHair";
	info.m_nVerticesPerStrand = i_Shape.m_nVerticesPerStrand;
	info.m_nHairVertices = i_Shape.m_nStrands * i_Shape.m_nVerticesPerStrand;
	info.m_Strands.resize(i_Shape.m_nStrands);
	const maVector3d c_Gravity(0, -1, 0);
	for (mdlHairStrand& strand : info.m_Strands)
	{
		//	a root on the upper half sphere, and two directions across the strand
		maVector3d normal;
		do
		{
			normal = maVector3d(maFunctions::FloatRand(-1, 1, rand), maFunctions::FloatRand(0, 1, rand),
								maFunctions::FloatRand(-1, 1, rand));
		} while (normal.Length() < 0.1f || normal.Length() > 1.0f);
		normal.Normalize();
		maVector3d across = normal.Cross(fabsf(normal.GetY()) < 0.9f ? maVector3d(0, 1, 0) : maVector3d(1, 0, 0));
		across.Normalize();
		const maVector3d across2 = normal.Cross(across);
		const float length = i_Shape.m_Length * maFunctions::FloatRand(0.75f, 1.0f, rand);
		const float phase = maFunctions::FloatRand(0, 6.2831853f, rand);

		strand.m_ControlPoints.resize(i_Shape.m_nVerticesPerStrand);
		for (int v = 0; v < i_Shape.m_nVerticesPerStrand; v++)
		{
			const float t = i_Shape.m_nVerticesPerStrand > 1 ? v / float(i_Shape.m_nVerticesPerStrand - 1) : 0.0f;
			const float turn = phase + 3 * 6.2831853f * t;
			const float curl = i_Shape.m_Curl * length * 0.1f * t;
			strand.m_ControlPoints[v].Position = normal * (1.0f + length * t) + c_Gravity * (0.5f * length * t * t)
												 + across * (curl * cosf(turn)) + across2 * (curl * sinf(turn));
		}
		mdlHairMaterial& material = strand.Material;
		material.m_RootRadius = i_Shape.m_RootRadius;
		material.m_TipRadius = i_Shape.m_TipRadius;
		material.m_RootColor = maVector3d(i_Shape.m_RootColor.GetRed(), i_Shape.m_RootColor.GetGreen(), i_Shape.m_RootColor.GetBlue());
		material.m_TipColor = maVector3d(i_Shape.m_TipColor.GetRed(), i_Shape.m_TipColor.GetGreen(), i_Shape.m_TipColor.GetBlue());
		material.m_SurfaceNormal = normal;
		material.m_Opacity = i_Shape.m_Opacity;
		material.m_Specular = 0.5f;
		material.m_Gloss = 40.0f;
		material.m_AmbientDiffuse = 0.3f;
	}

	//	hairImport's material: Hair_SH.fx with the parameters it builds
	matMaterial* material = Keep(gtScene::ShaderMaterial("Hair_SH.fx", gtScene::e_Defaults));
	g3dFragment* fragment = mdlFragCreate::CreateHairFragment(info, material);
	m_Fragments.emplace_back(fragment);
	g3dSceneNode* node = new g3dSceneNode(fragment, i_World);
	m_pRoot->AddChild(node);
	m_pScene->UpdateWorldData();
	return node;
}

g3dSceneNode* gtSceneBuilder::AddLayer(const gtMesh& i_Mesh, matMaterial* i_pMaterial, const maMatrix4x4& i_Camera)
{
	if (!m_pOverlayLayer)
	{
		m_pOverlayRoot.reset(new g3dSceneNode());
		m_pOverlayLayer = new g3dLayer(m_pOverlayRoot.get(), g3dLayer::e_ZBuffer, g3dLayer::e_Camera);
		m_pScene->AppendLayer(m_pOverlayLayer);
	}
	g3dFragment* fragment = i_Mesh.CreateFragment(i_pMaterial);
	m_Fragments.emplace_back(fragment);
	g3dSceneNode* node = new g3dSceneNode(fragment, i_Camera);
	m_pOverlayRoot->AddChild(node);
	m_pScene->UpdateWorldData();
	return node;
}

g3dSceneNode* gtSceneBuilder::AddViewerLayer(const gtMesh& i_Mesh, matMaterial* i_pMaterial, const maMatrix4x4& i_Camera)
{
	if (!m_pViewerScene)
	{
		m_pViewerRoot.reset(new g3dSceneNode());
		g3dLayer* layer = new g3dLayer(m_pViewerRoot.get(), g3dLayer::e_ZBuffer, g3dLayer::e_Camera);
		m_pViewerScene.reset(new g3dScene(layer));
		m_ViewerLayers.push_back(layer);
	}
	g3dFragment* fragment = i_Mesh.CreateFragment(i_pMaterial);
	m_Fragments.emplace_back(fragment);
	g3dSceneNode* node = new g3dSceneNode(fragment, i_Camera);
	m_pViewerRoot->AddChild(node);
	m_pViewerScene->UpdateWorldData();
	return node;
}

g3dProjectedLight* gtSceneBuilder::ProjectedLight(const maPoint3d& i_Position, const maPoint3d& i_Target,
												  const maFloatRGBA& i_Color, int i_ShadowMapSize)
{
	g3dProjectedLight* light = g3dLightMgr::CreateProjectedLight();
	light->SetCastsShadow(true);
	light->SetIntensity(i_Color);
	light->SetEnable(true);
	light->SetPosition(i_Position);
	light->SetTarget(i_Target);
	light->SetRange(30.0f);
	light->SetAngle(60.0f);
	light->SetInnerAngle(60.0f);
	m_Lights.push_back(light);

	m_ShadowCameras.emplace_back(new camCamera());
	camCamera* camera = m_ShadowCameras.back().get();
	light->OrientCamera(*camera);
	if (i_ShadowMapSize > 0)
	{
		matTexture* map = Keep(matTextureMgr::CreateShadowMap(i_ShadowMapSize, i_ShadowMapSize));
		light->SetShadowMap(map);
		m_DepthRenderers.emplace_back(g3dSceneRendererCreate::CreateDepthMapRenderer());
		m_TargetRenderers.emplace_back(new g3dTargetRenderer(map->GetRenderTargetAPI(), m_DepthRenderers.back().get(),
															 m_pScene.get(), camera));
		m_TargetRenderers.back()->SetBackgroundColor(g2dRGBColor(0xff, 0xff, 0xff));
	}
	return light;
}

namespace
{
	//	A scene object whose base is a builder's node (the reflection
	//	renderers find the reflector's fragments under GetBase()); the node
	//	stays the builder's
	class gtNodeObject : public scObject
	{
		public:
			explicit gtNodeObject(g3dSceneNode* i_pNode) : m_pNode(i_pNode) {}
			virtual const g3dSceneNode* GetBase() const	{ return m_pNode; }
			virtual g3dSceneNode* GetBase()				{ return m_pNode; }

		private:
			g3dSceneNode*	m_pNode;
	};
}

void gtSceneBuilder::Reflective(g3dSceneNode* i_pNode, bool i_bPlanar, bool i_bPerFrame)
{
	matMaterial* material = i_pNode->GetFragment()->GetMaterial();
	material->SetHasReflection(true);
	effReflectionMap& data = material->ReflectionData();
	data.m_bIsPlanar = i_bPlanar;
	data.m_bAutoGenEnvMap = i_bPerFrame;
	data.m_ReflectionMap = Keep(i_bPlanar
		? matTextureMgr::CreateRenderTargetTexture(data.m_ReflMapResolution, data.m_ReflMapResolution, false, NULL, true)
		: matTextureMgr::CreateCubeRenderTargetTexture(data.m_ReflMapResolution, data.m_ReflMapResolution, NULL));

	m_Reflectors.emplace_back(new gtNodeObject(i_pNode));
	g2dRenderTarget* target = data.m_ReflectionMap->GetRenderTargetAPI();
	if (i_bPlanar)
	{
		m_ReflectionRenderers.emplace_back(g3dSceneRendererCreate::CreatePlanarReflectionRenderer(m_Reflectors.back().get(), material));
		m_TargetRenderers.emplace_back(new g3dPlanarReflectionTargetRenderer(target, m_ReflectionRenderers.back().get(),
																			  m_pScene.get(), &m_Camera));
	}
	else
	{
		m_ReflectionRenderers.emplace_back(g3dSceneRendererCreate::CreateCubeMapRenderer(m_Reflectors.back().get(), material, i_bPerFrame));
		m_TargetRenderers.emplace_back(new g3dCubicReflectionTargetRenderer(target, m_ReflectionRenderers.back().get(),
																			 m_pScene.get(), &m_Camera));
	}
	m_TargetRenderers.back()->SetBackgroundColor(g2dRGBColor(0, 0, 0));
}

void gtSceneBuilder::RenderTargets(float i_fSimTime)
{
	for (auto& renderer : m_TargetRenderers)
		renderer->Render(i_fSimTime);
	for (auto& shadows : m_OpacityShadows)
	{
		if (shadows->m_pTarget)
			shadows->m_pTarget->Render(i_fSimTime);
	}
}

void gtSceneBuilder::HairShadows(g3dProjectedLight* io_pLight, HAIR_SHADOW_TYPE i_Type, int i_Size)
{
	OpacityShadows* shadows = NULL;
	for (auto& s : m_OpacityShadows)
	{
		if (s->m_pLight == io_pLight)
			shadows = s.get();
	}
	if (shadows)
	{
		shadows->Release();
	}
	else
	{
		m_OpacityShadows.emplace_back(new OpacityShadows());
		shadows = m_OpacityShadows.back().get();
		shadows->m_pLight = io_pLight;
		shadows->m_pRenderer.reset(g3dSceneRendererCreate::CreateOpacityMapRenderer(io_pLight));
	}
	io_pLight->SetHairShadowType(i_Type);
	io_pLight->OrientCamera(shadows->m_Camera);

	//	prjltProjectedLightObject's l_ldrPFD
	const g2dPFD ldr(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000, 32);
	int count = 1;
	if (i_Type == HAIR_SHADOW_OSM16 || i_Type == HAIR_SHADOW_DOSM16)
		count = 4;
	else if (i_Type == HAIR_SHADOW_OSM32 || i_Type == HAIR_SHADOW_DOSM32)
		count = 8;
	for (int i = 0; i < count; i++)
	{
		shadows->m_pMaps[i] = matTextureMgr::CreateRenderTargetTexture(i_Size, i_Size, false, &ldr, false, true,
																	   matTextureMgr::e_ShadowMap);
	}
	if (i_Type == HAIR_SHADOW_VOLUME4)
	{
		const g2dPFD l8(g2dPFD::e_Luminance8, 8);
		std::vector<unsigned char> data((size_t)i_Size * i_Size * 4, 0x00);
		std::fill(data.begin() + data.size() / 2, data.end(), (unsigned char)0xff);
		shadows->m_pVolume = matTextureMgr::CreateTexture3D(i_Size, i_Size, 4, &l8,
															BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS, data.data());
	}
	for (int i = 0; i < MAX_OPACITY_MAPS; i++)
		io_pLight->SetOpacityShadowMap(shadows->m_pMaps[i], i);
	io_pLight->SetOpacityVolume(shadows->m_pVolume);

	shadows->m_pTarget.reset(new g3dTargetRenderer(shadows->m_pMaps[0]->GetRenderTargetAPI(), shadows->m_pRenderer.get(),
												   m_pScene.get(), &shadows->m_Camera));
}

matTexture* gtSceneBuilder::OpacityMap(const g3dProjectedLight* i_pLight, int i_Index) const
{
	for (const auto& s : m_OpacityShadows)
	{
		if (s->m_pLight == i_pLight)
			return s->m_pMaps[i_Index];
	}
	return NULL;
}

matTexture* gtSceneBuilder::OpacityVolume(const g3dProjectedLight* i_pLight) const
{
	for (const auto& s : m_OpacityShadows)
	{
		if (s->m_pLight == i_pLight)
			return s->m_pVolume;
	}
	return NULL;
}

void gtSceneBuilder::TwoLights()
{
	PointLight(maPoint3d(2, 3, 4), maFloatRGBA(0.9f, 0.8f, 0.7f, 1), 25.0f);
	DirectionalLight(maVector3d(-0.3f, -0.8f, -0.5f), maFloatRGBA(0.4f, 0.5f, 0.6f, 1));
}

void gtSceneBuilder::LitSpheres(bool i_bLights, bool i_bBuiltInPhong)
{
	const gtMesh sphere = gtScene::Sphere(16, 32);
	const gtScene::Values values[3] = { gtScene::e_NonDefault, gtScene::e_Zero, gtScene::e_Huge };
	const char* shaders[3] = { "Blinn.fx", "Phong.fx", "Lambert.fx" };
	for (int row = 0; row < (i_bBuiltInPhong ? 4 : 3); row++)
	{
		for (int column = 0; column < 3; column++)
		{
			matMaterial* material = row < 3
				? gtScene::ShaderMaterial(shaders[row], values[column])
				: gtScene::OpaquePhong("Phong.fx", values[column]);
			Keep(material);
			Add(sphere, material, gtScene::Place(-1.5f + 1.5f * column, 1.8f - 1.2f * row, 0, 0.55f));
		}
	}
	if (i_bLights)
		TwoLights();
}

void gtSceneBuilder::GlobalEnvironment(matTexture* i_pDiffuse, matTexture* i_pSpecular, bool i_bBackground)
{
	g3dAmbientEnvState state;
	state.m_bInherit = false;
	state.m_DiffuseMap = i_pDiffuse;
	state.m_DiffuseFactor = 1.0f;
	state.m_DiffuseAngle = 0.0f;
	state.m_DiffuseColor = maFloatRGBA(1, 1, 1, 1);
	state.m_SpecularMap = i_pSpecular;
	state.m_SpecularFactor = 1.0f;
	state.m_SpecularAngle = 0.0f;
	state.m_SpecularColor = maFloatRGBA(1, 1, 1, 1);
	state.m_bEnableSwlEnv = true;
	state.m_bEnableSwlEnvBG = i_bBackground;
	m_pScene->SetGlobalAmbient(state);
	m_pEnvironment.reset(new g3dAmbientEnvState(state));
	m_pRoot->SetEnvironment(m_pEnvironment.get());
	m_pRenderState.reset(new g3dRenderState());
	m_pRenderState->m_Lights = m_Lights;
	m_pRoot->SetRenderState(m_pRenderState.get());
}

void gtSceneBuilder::PointLight(const maPoint3d& i_Position, const maFloatRGBA& i_Color, float i_Range)
{
	g3dPointLight* light = g3dLightMgr::CreatePointLight();
	light->SetPosition(i_Position);
	light->SetIntensity(i_Color);
	light->SetRange(i_Range);
	light->SetCastsShadow(m_bLightsCastShadows);
	light->SetEnable(true);
	m_Lights.push_back(light);
}

void gtSceneBuilder::DirectionalLight(const maVector3d& i_Direction, const maFloatRGBA& i_Color)
{
	g3dDirectionalLight* light = g3dLightMgr::CreateDirectionalLight();
	light->SetDirection(i_Direction);
	light->SetIntensity(i_Color);
	light->SetCastsShadow(m_bLightsCastShadows);
	light->SetEnable(true);
	m_Lights.push_back(light);
}
