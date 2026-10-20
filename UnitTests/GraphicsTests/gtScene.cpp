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

#include <gtest/gtest.h>
#include <stb_image_write.h>

#include <algorithm>
#include <cmath>

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
			default:				return i_Default * 1.37f + 0.11f + 0.01f * i_Index;
		}
	}

	maFloatRGBA color_value(gtScene::Values i_Values, int i_Index)
	{
		switch (i_Values)
		{
			case gtScene::e_Zero:	return maFloatRGBA(0, 0, 0, 0);
			case gtScene::e_Huge:	return maFloatRGBA(1, 1, 1, 1);
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
	for (size_t i = 0; i < floats.size(); i++)
		floats[i]->Property().SetValue(float_value(i_Values, floats[i]->GetProperty().GetValue(), (int)i));
	std::vector<effParamColor*> colors;
	params->GetAllColorParams(colors);
	for (size_t i = 0; i < colors.size(); i++)
		colors[i]->Property().SetValue(color_value(i_Values, (int)i));
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

maMatrix4x4 gtScene::Place(float i_X, float i_Y, float i_Z, float i_Scale)
{
	maMatrix4x4 m;
	m.MakeScale(i_Scale, i_Scale, i_Scale);
	m.TranslateBy(i_X, i_Y, i_Z);
	return m;
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

void gtSceneBuilder::PointLight(const maPoint3d& i_Position, const maFloatRGBA& i_Color, float i_Range)
{
	g3dPointLight* light = g3dLightMgr::CreatePointLight();
	light->SetPosition(i_Position);
	light->SetIntensity(i_Color);
	light->SetRange(i_Range);
	light->SetEnable(true);
	m_Lights.push_back(light);
}

void gtSceneBuilder::DirectionalLight(const maVector3d& i_Direction, const maFloatRGBA& i_Color)
{
	g3dDirectionalLight* light = g3dLightMgr::CreateDirectionalLight();
	light->SetDirection(i_Direction);
	light->SetIntensity(i_Color);
	light->SetEnable(true);
	m_Lights.push_back(light);
}
