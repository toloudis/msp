/*****************************************************************************
**  g3dMaterialTableOptiX.cpp
**
**      See g3dMaterialTableOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DMATERIALTABLEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dMaterialTableOptiX.hpp"
#endif
#ifndef GOX_G3DTEXTURETABLEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dTextureTableOptiX.hpp"
#endif
#ifndef EFF_TRACEMATERIAL_HPP
#include "Graphics/Eff/effTraceMaterial.hpp"
#endif

#include <cstring>

namespace gox
{
	static_assert(sizeof(optxMaterial) % 16 == 0, "optxMaterial's size must be a multiple of 16");
	static_assert(sizeof(optxTexture) == 24, "optxTexture is laid out for nvcc");

	namespace
	{
		void set3(float o_V[3], const maFloatRGBA& i_Color, float i_Scale = 1.0f)
		{
			o_V[0] = i_Color.GetRed() * i_Scale;
			o_V[1] = i_Color.GetGreen() * i_Scale;
			o_V[2] = i_Color.GetBlue() * i_Scale;
		}
	}

	optxMaterial g3dMaterialTableOptiX::Describe(const effTraceMaterial& i_Material, g3dTextureTableOptiX& i_Textures)
	{
		const cuExternalImageCache::Mode mode = cuExternalImageCache::e_Material;
		optxMaterial m;
		std::memset(&m, 0, sizeof(m));
		set3(m.m_BaseColor, i_Material.m_BaseColor);
		m.m_BaseColorMap = i_Textures.Add(i_Material.m_pBaseColorMap, mode);
		set3(m.m_Emission, i_Material.m_Emission, i_Material.m_EmissionStrength);
		m.m_EmissionMap = i_Textures.Add(i_Material.m_pEmissionMap, mode);
		set3(m.m_SpecularColor, i_Material.m_SpecularColor);
		m.m_SpecularColorMap = i_Textures.Add(i_Material.m_pSpecularColorMap, mode);
		m.m_Metallic = i_Material.m_Metallic;
		m.m_Roughness = i_Material.m_Roughness;
		m.m_MetallicRoughnessMap = i_Textures.Add(i_Material.m_pMetallicRoughnessMap, mode);
		m.m_Specular = i_Material.m_Specular;
		m.m_Ior = i_Material.m_Ior;
		m.m_Shininess = i_Material.m_Shininess;
		m.m_ReflectionRoughness = i_Material.m_ReflectionRoughness;
		m.m_GlossMap = i_Textures.Add(i_Material.m_pGlossMap, mode);
		if (m.m_GlossMap != c_optxNoTexture)
		{
			if (i_Material.m_GlossMode == effTraceMaterial::e_GlossBlinn)
			{
				m.m_Flags |= e_optxMaterialGlossBlinn;
			}
			else if (i_Material.m_GlossMode == effTraceMaterial::e_GlossPhong)
			{
				m.m_Flags |= e_optxMaterialGlossPhong;
			}
		}
		m.m_NormalMap = i_Textures.Add(i_Material.m_pNormalMap, mode);
		m.m_NormalScale = i_Material.m_NormalScale;

		const effOpacity& opacity = i_Material.m_Opacity;
		m.m_Opacity = opacity.m_Value;
		m.m_Cutoff = opacity.m_Cutoff;
		m.m_OpacityMap = i_Textures.Add(opacity.m_pMap, mode);
		if (opacity.m_bMapAlpha)
		{
			m.m_Flags |= e_optxMaterialOpacityAlpha;
		}
		if (opacity.m_bCutOut)
		{
			m.m_Flags |= e_optxMaterialCutOut;
		}
		else if (opacity.m_pMap || opacity.m_Value < 1.0f)
		{
			m.m_Flags |= e_optxMaterialTransparent;
		}
		for (int i = 0; i < 6; ++i)
		{
			m.m_UV[i] = i_Material.m_UVTransform[i];
		}
		return m;
	}

	g3dMaterialTableOptiX::g3dMaterialTableOptiX()
		: m_bUploaded(false)
		, m_Version(0)
	{
	}

	g3dMaterialTableOptiX::~g3dMaterialTableOptiX()
	{
	}

	bool g3dMaterialTableOptiX::Update(const std::vector<const matMaterial*>& i_Materials, g3dTextureTableOptiX& i_Textures)
	{
		std::vector<optxMaterial> table;
		table.reserve(i_Materials.size());
		for (const matMaterial* material : i_Materials)
		{
			table.push_back(Describe(effTraceMaterial::Of(*material), i_Textures));
		}
		if (m_bUploaded && table.size() == m_Table.size() &&
			(table.empty() || std::memcmp(table.data(), m_Table.data(), table.size() * sizeof(optxMaterial)) == 0))
		{
			return false;
		}
		m_bUploaded = m_Materials.Upload(table.data(), table.size() * sizeof(optxMaterial));
		m_Table.swap(table);
		if (!m_bUploaded)
		{
			m_Table.clear();
		}
		++m_Version;
		return true;
	}

	void g3dMaterialTableOptiX::Release()
	{
		m_Materials.Free();
		m_Table.clear();
		m_bUploaded = false;
	}
}
