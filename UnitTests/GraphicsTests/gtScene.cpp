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
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef G3D_RENDERSTATE_HPP
#include "Graphics/G3d/g3dRenderState.hpp"
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
	params->SetShaderName(itString(i_Shader.c_str()));
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

matTexture* gtScene::CheckerTexture(const std::filesystem::path& i_Dir, int i_Width, int i_Height, int i_Cells)
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
			p[3] = 255;
		}
	}
	const std::filesystem::path path = i_Dir / ("checker_" + std::to_string(i_Width) + "x" + std::to_string(i_Height) + "_" + std::to_string(i_Cells) + ".png");
	if (!stbi_write_png(path.u8string().c_str(), i_Width, i_Height, 4, rgba.data(), i_Width * 4))
	{
		ADD_FAILURE() << "can't write " << path.u8string();
		return NULL;
	}
	fsLocator locator;
	fsFileUtil::UnicodeStringToLocator(itString(path.wstring().c_str()), locator);
	matTexture* texture = matTextureMgr::LoadTexture(locator, TEXTURE_TYPE_2D, true);
	EXPECT_TRUE(texture != NULL) << path.u8string();
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
	fsFileUtil::UnicodeStringToLocator(itString(path.wstring().c_str()), locator);
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

gtSceneBuilder::~gtSceneBuilder()
{
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
