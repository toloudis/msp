/*****************************************************************************
**  effTraceMaterial.cpp
**
**		see .hpp
**
**	Extra Large Technology
\****************************************************************************/
#ifndef EFF_TRACEMATERIAL_HPP
#include "Graphics/Eff/effTraceMaterial.hpp"
#endif
#ifndef EFF_FURDATA_HPP
#include "Graphics/Eff/effFurData.hpp"
#endif
#ifndef EFF_PBRDATA_HPP
#include "Graphics/Eff/effPbrData.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef EFF_SOLIDDATA_HPP
#include "Graphics/Eff/effSolidData.hpp"
#endif
#ifndef EFF_TEXTUREDDATA_HPP
#include "Graphics/Eff/effTexturedData.hpp"
#endif
#ifndef EFF_TOONDATA_HPP
#include "Graphics/Eff/effToonData.hpp"
#endif
#ifndef EFF_WATERDATA_HPP
#include "Graphics/Eff/effWaterData.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <algorithm>
#include <cmath>
#include <mutex>
#include <set>

namespace
{
	//	The environment blur level at roughness 1 (Pbr.hlsl's c_EnvMaxLod):
	//	a reflection map's reflBlur over it is the roughness as sharp
	const float c_EnvMaxLod = 6.0f;

	//	How a .fx shader maps to the model
	enum Rule
	{
		e_RuleBlinn,			// semantic colours; Blinn shininess (g_shininess)
		e_RulePhong,			// semantic colours; Phong exponent (g_shininess)
		e_RuleLambert,			// semantic colours, no specular
		e_RuleBaseOnly,			// base colour only
		e_RuleSkin,				// base colour; g_specColor x g_specPower; Phong on g_specGloss; normalTex
		e_RuleBlinnSkin,		// base colour; Blinn; normalTex
		e_RuleSpecularFresnel,	// base colour; g_specularColor0, Phong on g_specularExp0
		e_RuleNotTraced			// not a surface
	};

	struct RuleRow
	{
		const char*		m_pShader;
		Rule			m_Rule;
		const char*		m_pDropped;		// the shader's own effects the tracer ignores, or NULL
	};

	//	Every .fx material shader (GraphicsVulkan's Materials/*.effect.json)
	//	but Pbr.fx, which converts one to one. A shader not listed gets the
	//	Blinn rule (logged).
	const RuleRow c_Rules[] =
	{
		{ "Blinn.fx",								e_RuleBlinn,			NULL },
		{ "BlinnReflection.fx",						e_RuleBlinn,			"refraction and reflection smearing" },
		{ "FC3DBlinn.fx",							e_RuleBlinn,			NULL },
		{ "CarPaint.fx",							e_RuleBlinn,			"flakes, the second specular lobe and the edge colour" },
		{ "Anisotropic.fx",							e_RuleBlinn,			"anisotropy" },
		{ "Phong.fx",								e_RulePhong,			NULL },
		{ "PhongReflection.fx",						e_RulePhong,			"refraction and reflection smearing" },
		{ "Phong_wBump.fx",							e_RulePhong,			NULL },
		{ "FC3DPhong.fx",							e_RulePhong,			NULL },
		{ "FC3DPhongTransparent.fx",				e_RulePhong,			NULL },
		{ "Lambert.fx",								e_RuleLambert,			NULL },
		{ "Simple.fx",								e_RuleBaseOnly,			NULL },
		{ "Default.fx",								e_RuleBaseOnly,			NULL },
		{ "Cartoon.fx",								e_RuleBaseOnly,			"toon shading" },
		{ "Skin.fx",								e_RuleSkin,				"subsurface scattering" },
		{ "SubSurfaceScatter.fx",					e_RuleSkin,				"subsurface scattering" },
		{ "BlinnSkin.fx",							e_RuleBlinnSkin,		"subsurface scattering" },
		{ "SubSurfaceScatter_wBlinnSpecular.fx",	e_RuleBlinnSkin,		"subsurface scattering" },
		{ "FC3DSubSurfaceScatter_wBlinnSpecular.fx",e_RuleBlinnSkin,		"subsurface scattering" },
		{ "SpecularFresnel.fx",						e_RuleSpecularFresnel,	"the second specular lobe and its shift" },
		{ "FC3DSpecularFresnel.fx",					e_RuleSpecularFresnel,	"the second specular lobe and its shift" },
		{ "Hair_SH.fx",								e_RuleNotTraced,		NULL },
		{ "Bake.fx",								e_RuleNotTraced,		NULL },
	};

	const RuleRow* find_rule(const std::string& i_Shader)
	{
		for (const RuleRow& row : c_Rules)
		{
			if (envString::EqualNoCase(i_Shader, row.m_pShader))
			{
				return &row;
			}
		}
		return NULL;
	}

	float clamp01(float i_Value)
	{
		return std::min(1.0f, std::max(0.0f, i_Value));
	}

	maFloatRGBA rgb(const maFloatRGBA& i_Color)
	{
		return maFloatRGBA(i_Color.GetRed(), i_Color.GetGreen(), i_Color.GetBlue(), 1.0f);
	}

	//	Bag lookups: false (outputs untouched) if the bag lacks the parameter
	bool get_color(const effShaderParams& i_Params, const char* i_pName, maFloatRGBA& o_Color)
	{
		const effParamColor* param = i_Params.FindColorParam(i_pName);
		if (param)
		{
			o_Color = rgb(param->GetProperty().GetValue());
		}
		return param != NULL;
	}

	bool get_float(const effShaderParams& i_Params, const char* i_pName, float& o_Value)
	{
		const effParamFloat* param = i_Params.FindFloatParam(i_pName);
		if (param)
		{
			o_Value = param->GetProperty().GetValue();
		}
		return param != NULL;
	}

	matTexture* get_texture(const effShaderParams& i_Params, const char* i_pName)
	{
		const effParamTexture* param = i_Params.FindTextureParam(i_pName);
		return param ? param->GetTexture() : NULL;
	}

	void drop(effTraceMaterial& io_Material, const std::string& i_What)
	{
		if (std::find(io_Material.m_Dropped.begin(), io_Material.m_Dropped.end(), i_What) == io_Material.m_Dropped.end())
		{
			io_Material.m_Dropped.push_back(i_What);
		}
	}

	//	The specular lobe of a Blinn or Phong shader: colour x map, factor,
	//	the highlight's roughness, a gloss map
	void specular_lobe(const effShaderParams& i_Params, effTraceMaterial& io_Material, effTraceMaterial::Gloss i_Gloss,
					   const char* i_pColor, const char* i_pColorMap, const char* i_pFactor, const char* i_pShininess,
					   const char* i_pGlossMap)
	{
		io_Material.m_Specular = 1.0f;
		get_color(i_Params, i_pColor, io_Material.m_SpecularColor);
		if (i_pColorMap)
		{
			io_Material.m_pSpecularColorMap = get_texture(i_Params, i_pColorMap);
		}
		float factor = 1.0f;
		if (i_pFactor && get_float(i_Params, i_pFactor, factor))
		{
			io_Material.m_Specular = clamp01(factor);
		}
		float shininess = (i_Gloss == effTraceMaterial::e_GlossBlinn) ? 0.5f : 20.0f;
		get_float(i_Params, i_pShininess, shininess);
		if (i_Gloss == effTraceMaterial::e_GlossBlinn)
		{
			shininess = clamp01(shininess);
		}
		io_Material.m_Shininess = shininess;
		io_Material.m_pGlossMap = i_pGlossMap ? get_texture(i_Params, i_pGlossMap) : NULL;
		io_Material.m_GlossMode = io_Material.m_pGlossMap ? i_Gloss : effTraceMaterial::e_GlossNone;
		io_Material.m_Roughness = (i_Gloss == effTraceMaterial::e_GlossBlinn) ? effTraceMaterial::RoughnessFromShininess(shininess)
																			  : effTraceMaterial::RoughnessFromExponent(shininess);
	}

	//	A reflective Blinn or Phong material (see the header): its reflection
	//	map's strength, Fresnel bias and sharpness through KHR specular
	void reflective(const effShaderParams& i_Params, effTraceMaterial& io_Material)
	{
		float reflectivity = 1.0f, bias = 0.2f, blur = 0.0f;
		get_float(i_Params, "g_reflectivity", reflectivity);
		get_float(i_Params, "fresnelBias", bias);
		get_float(i_Params, "reflBlur", blur);
		const float f0 = clamp01(reflectivity * clamp01(bias));
		const float f90 = clamp01(reflectivity);
		io_Material.m_Specular = f90;
		const float r = (io_Material.m_Ior - 1.0f) / (io_Material.m_Ior + 1.0f);
		const float dielectric = r * r * f90;
		const float grey = dielectric > 0.0f ? f0 / dielectric : 1.0f;
		io_Material.m_SpecularColor = maFloatRGBA(grey, grey, grey, 1.0f);
		if (matTexture* factor = get_texture(i_Params, "reflectFactorMap"))
		{
			if (!io_Material.m_pSpecularColorMap)
			{
				io_Material.m_pSpecularColorMap = factor;
			}
			else
			{
				drop(io_Material, "the reflection factor map (the specular map is the specular colour)");
			}
		}
		const float sharp = clamp01(std::max(0.0f, blur) / c_EnvMaxLod);
		io_Material.m_Roughness = std::min(io_Material.m_Roughness, sharp);
		io_Material.m_ReflectionRoughness = sharp;
	}

	//	Emission: g_emissive x g_emissiveIntensity x emissiveMap
	void emission(const effShaderParams& i_Params, effTraceMaterial& io_Material)
	{
		get_color(i_Params, "g_emissive", io_Material.m_Emission);
		get_float(i_Params, "g_emissiveIntensity", io_Material.m_EmissionStrength);
		io_Material.m_pEmissionMap = get_texture(i_Params, "emissiveMap");
	}

	void from_pbr(effShaderParams& i_Params, effTraceMaterial& io_Material)
	{
		const effPbrData pbr = effPbrData::FromParams(i_Params);
		io_Material.m_BaseColor = rgb(pbr.m_BaseColor);
		io_Material.m_pBaseColorMap = get_texture(i_Params, effPbrData::c_BaseColorMap);
		io_Material.m_Metallic = clamp01(pbr.m_Metallic);
		io_Material.m_Roughness = clamp01(pbr.m_Roughness);
		io_Material.m_pMetallicRoughnessMap = get_texture(i_Params, effPbrData::c_MetallicRoughnessMap);
		io_Material.m_Specular = 1.0f;		// glTF's: F0 4% for dielectrics
		io_Material.m_Emission = rgb(pbr.m_Emissive);
		io_Material.m_EmissionStrength = pbr.m_EmissiveStrength;
		io_Material.m_pEmissionMap = get_texture(i_Params, effPbrData::c_EmissiveMap);
		if (get_texture(i_Params, effPbrData::c_OcclusionMap))
		{
			drop(io_Material, "the occlusion map (traced occlusion replaces it)");
		}
	}

	void from_bag(effShaderParams& i_Params, const std::string& i_Shader, effTraceMaterial& io_Material)
	{
		const RuleRow* row = find_rule(i_Shader);
		if (!row)
		{
			static std::set<std::string> s_Logged;
			static std::mutex s_Lock;
			std::lock_guard<std::mutex> lock(s_Lock);
			if (s_Logged.insert(i_Shader).second)
			{
				DBG_LOG("effTraceMaterial: shader " << i_Shader << " has no tracer rule; read as Blinn");
			}
		}
		const Rule rule = row ? row->m_Rule : e_RuleBlinn;
		if (rule == e_RuleNotTraced)
		{
			io_Material.m_bTraced = false;
			return;
		}
		if (row && row->m_pDropped)
		{
			drop(io_Material, row->m_pDropped);
		}

		//	Base colour: the MaterialDiffuse and DiffuseTexture semantics
		if (i_Params.m_pDiffuseColor)
		{
			io_Material.m_BaseColor = rgb(i_Params.m_pDiffuseColor->GetProperty().GetValue());
		}
		if (i_Params.m_pDiffuseMap)
		{
			io_Material.m_pBaseColorMap = i_Params.m_pDiffuseMap->GetTexture();
		}

		switch (rule)
		{
			case e_RuleBlinn:
				specular_lobe(i_Params, io_Material, effTraceMaterial::e_GlossBlinn, "g_specular", "specularMap",
							  "g_specularPower", "g_shininess", "glossMap");
				emission(i_Params, io_Material);
				break;
			case e_RulePhong:
				specular_lobe(i_Params, io_Material, effTraceMaterial::e_GlossPhong, "g_specular", "specularMap",
							  "g_specularPower", "g_shininess", "glossMap");
				emission(i_Params, io_Material);
				break;
			case e_RuleLambert:
				emission(i_Params, io_Material);
				break;
			case e_RuleSkin:
				specular_lobe(i_Params, io_Material, effTraceMaterial::e_GlossPhong, "g_specColor", "specTex",
							  "g_specPower", "g_specGloss", NULL);
				break;
			case e_RuleBlinnSkin:
				specular_lobe(i_Params, io_Material, effTraceMaterial::e_GlossBlinn, "g_specular", "specularMap",
							  "g_specularPower", "g_shininess", "glossMap");
				break;
			case e_RuleSpecularFresnel:
				specular_lobe(i_Params, io_Material, effTraceMaterial::e_GlossPhong, "g_specularColor0", NULL,
							  NULL, "g_specularExp0", NULL);
				break;
			default:	// base colour only
				break;
		}

		//	A physical index where the shader has one (the reflection shaders' refraction)
		float ior = 0.0f;
		if (get_float(i_Params, "IOR", ior) && ior > 0.0f)
		{
			io_Material.m_Ior = ior;
		}

		//	The Skin shaders' own normal map
		if (rule == e_RuleSkin || rule == e_RuleBlinnSkin)
		{
			if (matTexture* normals = get_texture(i_Params, "normalTex"))
			{
				io_Material.m_pNormalMap = normals;
				io_Material.m_NormalScale = 1.0f;
				get_float(i_Params, "g_bumpMapScale", io_Material.m_NormalScale);
			}
		}

		if (get_texture(i_Params, "cubeMap") || get_texture(i_Params, "CubeReflMap") || get_texture(i_Params, "cubeTex"))
		{
			if (rule == e_RuleBlinn || rule == e_RulePhong || rule == e_RuleBlinnSkin)
			{
				reflective(i_Params, io_Material);
			}
			drop(io_Material, "the reflection map (the BRDF reflects the scene)");
		}
	}

	//	A built-in effect's data (no .fx)
	void from_data(const effShaderData* i_pData, effTraceMaterial& io_Material)
	{
		if (const effPhongData* phong = dynamic_cast<const effPhongData*>(i_pData))
		{
			io_Material.m_BaseColor = rgb(phong->m_ColorDiffuse);
			io_Material.m_pBaseColorMap = phong->m_TextureDiffuse;
			io_Material.m_Emission = rgb(phong->m_ColorEmissive);
			io_Material.m_Specular = 1.0f;
			io_Material.m_SpecularColor = rgb(phong->m_ColorSpecular);
			io_Material.m_pSpecularColorMap = phong->m_TextureSpecular;
			io_Material.m_Shininess = phong->m_SpecularPower;
			io_Material.m_pGlossMap = phong->m_TextureGloss;
			io_Material.m_GlossMode = phong->m_TextureGloss ? effTraceMaterial::e_GlossPhong : effTraceMaterial::e_GlossNone;
			io_Material.m_Roughness = effTraceMaterial::RoughnessFromExponent(phong->m_SpecularPower);
			if (phong->m_TextureEnvironment)
			{
				drop(io_Material, "cube-map reflections (the BRDF reflects)");
			}
		}
		else if (const effSolidData* solid = dynamic_cast<const effSolidData*>(i_pData))
		{
			io_Material.m_BaseColor = rgb(solid->m_Color);
		}
		else if (const effTexturedData* textured = dynamic_cast<const effTexturedData*>(i_pData))
		{
			io_Material.m_BaseColor = rgb(textured->m_Color);
			io_Material.m_pBaseColorMap = textured->m_Texture;
		}
		else if (const effToonData* toon = dynamic_cast<const effToonData*>(i_pData))
		{
			io_Material.m_BaseColor = rgb(toon->m_ColorDiffuse);
			drop(io_Material, "toon shading");
		}
		else if (const effWaterData* water = dynamic_cast<const effWaterData*>(i_pData))
		{
			io_Material.m_BaseColor = rgb(water->m_WaterColor);
			drop(io_Material, "the water's waves");
		}
		else if (dynamic_cast<const effFurData*>(i_pData))
		{
			drop(io_Material, "fur (the surface is traced without it)");
		}
		else if (i_pData)
		{
			drop(io_Material, "the effect's own data (white is traced)");
		}
	}

	void log_dropped_once(const effTraceMaterial& i_Material)
	{
		if (i_Material.m_Dropped.empty())
		{
			return;
		}
		static std::set<std::string> s_Logged;
		static std::mutex s_Lock;
		std::lock_guard<std::mutex> lock(s_Lock);
		if (!s_Logged.insert(i_Material.m_Shader).second)
		{
			return;
		}
		std::string list;
		for (const std::string& what : i_Material.m_Dropped)
		{
			list += (list.empty() ? "" : "; ") + what;
		}
		DBG_LOG("effTraceMaterial: the tracer ignores, in " << i_Material.m_Shader << " materials: " << list);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float effTraceMaterial::RoughnessFromShininess(float i_Shininess)
{
	return clamp01(1.0f - i_Shininess);
}

float effTraceMaterial::RoughnessFromExponent(float i_Exponent)
{
	const float alpha = std::sqrt(2.0f / (std::max(i_Exponent, 0.0f) + 2.0f));
	return clamp01(std::sqrt(alpha));
}

float effTraceMaterial::RoughnessForGloss(float i_Gloss) const
{
	switch (m_GlossMode)
	{
		case e_GlossBlinn:
			return RoughnessFromShininess(clamp01(m_Shininess * i_Gloss));
		case e_GlossPhong:
			return RoughnessFromExponent(m_Shininess * i_Gloss);
		default:
			return m_Roughness;
	}
}

std::vector<matTexture*> effTraceMaterial::GetTextures() const
{
	std::vector<matTexture*> textures;
	for (matTexture* texture : { m_pBaseColorMap, m_pMetallicRoughnessMap, m_pGlossMap, m_pSpecularColorMap, m_pEmissionMap,
								 m_pNormalMap, m_Opacity.m_pMap })
	{
		if (texture && std::find(textures.begin(), textures.end(), texture) == textures.end())
		{
			textures.push_back(texture);
		}
	}
	return textures;
}

effTraceMaterial effTraceMaterial::Of(const matMaterial& i_Material)
{
	effTraceMaterial material;
	shared_ptr<effShaderParams> params = i_Material.GetShaderParams(0);
	if (params && params->GetShaderName().GetNumNames() > 0)
	{
		material.m_Shader = params->GetShaderName().GetLastName();
		if (envString::EqualNoCase(material.m_Shader, effPbrData::c_ShaderName))
		{
			from_pbr(*params, material);
		}
		else
		{
			from_bag(*params, material.m_Shader, material);
		}
	}
	else
	{
		material.m_Shader = i_Material.GetEffectID();
		from_data(i_Material.GetEffectData(0), material);
	}
	if (!material.m_bTraced)
	{
		return material;
	}

	//	Material-level data. A Skin shader's normalTex wins over NormalsData.
	const effNormalsData& normals = i_Material.GetNormalsData();
	if (!material.m_pNormalMap && normals.m_pNormalMap)
	{
		material.m_pNormalMap = normals.m_pNormalMap;
		material.m_NormalScale = normals.m_BumpScale;
	}
	material.m_Opacity = effOpacity::Of(i_Material);
	const maMatrix4x4 uv = i_Material.GetUVTransform(0).MakeUVTransform();	// transposed: column vectors
	material.m_UVTransform[0] = uv(0, 0);
	material.m_UVTransform[1] = uv(0, 1);
	material.m_UVTransform[2] = uv(0, 3);
	material.m_UVTransform[3] = uv(1, 0);
	material.m_UVTransform[4] = uv(1, 1);
	material.m_UVTransform[5] = uv(1, 3);

	if (i_Material.GetNumMaterialLayers() > 1)
	{
		drop(material, "material layers after the first");
	}
	if (i_Material.GetHasDisplacement())
	{
		drop(material, "displacement (the surface is traced undisplaced)");
	}
	if (i_Material.GetHasGlow())
	{
		drop(material, "glow (a post effect)");
	}
	if (i_Material.GetHasOutline())
	{
		drop(material, "outline (a post effect)");
	}
	for (int layer = 0; layer < i_Material.GetNumMaterialLayers(); ++layer)
	{
		if (dynamic_cast<const effFurData*>(i_Material.GetEffectData(layer)))
		{
			drop(material, "fur (the surface is traced without it)");
		}
	}
	log_dropped_once(material);
	return material;
}
