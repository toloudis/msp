/*****************************************************************************
**	effPbrData.cpp
**
**		see .hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Graphics/Eff/effPbrData.hpp"

#include "Graphics/Eff/effBlinnData.hpp"
#include "Graphics/Eff/effShaderParams.hpp"

#include "Core/fs/fsFileUtil.hpp"

#include <algorithm>


const char* const effPbrData::c_ShaderName = "Pbr.fx";
const char* const effPbrData::c_BaseColor = "g_baseColor";
const char* const effPbrData::c_BaseColorMap = "baseColorMap";
const char* const effPbrData::c_Metallic = "g_metallic";
const char* const effPbrData::c_Roughness = "g_roughness";
const char* const effPbrData::c_MetallicRoughnessMap = "metallicRoughnessMap";
const char* const effPbrData::c_OcclusionMap = "occlusionMap";
const char* const effPbrData::c_OcclusionStrength = "g_occlusionStrength";
const char* const effPbrData::c_Emissive = "g_emissive";
const char* const effPbrData::c_EmissiveMap = "emissiveMap";
const char* const effPbrData::c_EmissiveStrength = "g_emissiveStrength";
const char* const effPbrData::c_AlphaMode = "g_alphaMode";
const char* const effPbrData::c_AlphaCutoff = "g_alphaCutoff";
const char* const effPbrData::c_ReflectionMap = "cubeMap";
const char* const effPbrData::c_ReflectionMapAngle = "g_reflMapAngle";


namespace
{
	//------------------------------------------------------------------------
	// a texture parameter holding an ANSI path ("" for none)
	//------------------------------------------------------------------------
	effParamTexture* make_texture_param(const char* i_pName, const std::string& i_Path)
	{
		fsLocator loc;
		if (!i_Path.empty())
			fsFileUtil::ANSIFilenameToLocator(i_Path, loc);
		return new effParamTexture(i_pName, i_pName, loc);
	}

	//------------------------------------------------------------------------
	// read parameters that are there; keep io_Value otherwise
	//------------------------------------------------------------------------
	void read_texture(const effShaderParams& i_Params, const char* i_pName, std::string& io_Path)
	{
		if (const effParamTexture* param = i_Params.FindTextureParam(i_pName))
		{
			const fsLocator& loc = param->GetProperty().GetValue();
			io_Path.clear();
			if (loc.GetNumNames() > 0)
				fsFileUtil::LocatorToANSIFilename(loc, io_Path);
		}
	}

	void read_float(const effShaderParams& i_Params, const char* i_pName, float& io_Value)
	{
		if (const effParamFloat* param = i_Params.FindFloatParam(i_pName))
			io_Value = param->GetProperty().GetValue();
	}

	void read_color(const effShaderParams& i_Params, const char* i_pName, maFloatRGBA& io_Value)
	{
		if (const effParamColor* param = i_Params.FindColorParam(i_pName))
			io_Value = param->GetProperty().GetValue();
	}

	float clamp01(float i_Value)
	{
		return (std::min)((std::max)(i_Value, 0.0f), 1.0f);
	}

	//------------------------------------------------------------------------
	// io_To's texture parameter i_pToName gets i_From's i_pFromName texture,
	// ramp and file name
	//------------------------------------------------------------------------
	void copy_texture(const effShaderParams& i_From, const char* i_pFromName,
					  effShaderParams& io_To, const char* i_pToName)
	{
		effParamTexture* to = io_To.FindTextureParam(i_pToName);
		if (!to)
			return;
		effParamTexture* from = i_From.FindTextureParam(i_pFromName);
		to->SetTexture(from ? from->GetTexture() : NULL);
		to->SetRampTexture(from ? from->GetRampTexture() : NULL);
		to->SetRampProperty(from ? from->GetSmartRampProperty() : shared_ptr<prtyObject>());
		if (from)
			to->Property().SetValueWithoutNotify(from->GetProperty().GetFullValue());
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effPbrData::effPbrData()
:	m_BaseColor(1.0f, 1.0f, 1.0f, 1.0f),
	m_Metallic(1.0f),
	m_Roughness(1.0f),
	m_OcclusionStrength(1.0f),
	m_Emissive(0.0f, 0.0f, 0.0f, 1.0f),
	m_EmissiveStrength(1.0f),
	m_AlphaMode(e_Opaque),
	m_AlphaCutoff(0.5f),
	m_ReflectionMapAngle(0.0f)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effPbrData::operator == (const effPbrData& i_Other) const
{
	return m_BaseColor == i_Other.m_BaseColor
		&& m_BaseColorMap == i_Other.m_BaseColorMap
		&& m_Metallic == i_Other.m_Metallic
		&& m_Roughness == i_Other.m_Roughness
		&& m_MetallicRoughnessMap == i_Other.m_MetallicRoughnessMap
		&& m_OcclusionMap == i_Other.m_OcclusionMap
		&& m_OcclusionStrength == i_Other.m_OcclusionStrength
		&& m_Emissive == i_Other.m_Emissive
		&& m_EmissiveMap == i_Other.m_EmissiveMap
		&& m_EmissiveStrength == i_Other.m_EmissiveStrength
		&& m_AlphaMode == i_Other.m_AlphaMode
		&& m_AlphaCutoff == i_Other.m_AlphaCutoff
		&& m_ReflectionMap == i_Other.m_ReflectionMap
		&& m_ReflectionMapAngle == i_Other.m_ReflectionMapAngle;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effPbrData::operator != (const effPbrData& i_Other) const
{
	return !(*this == i_Other);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effPbrData::AddToParams(effShaderParams& o_Params) const
{
	o_Params.SetVersion(1);
	o_Params.AddParam(new effParamColor(c_BaseColor, c_BaseColor, m_BaseColor));
	o_Params.AddParam(make_texture_param(c_BaseColorMap, m_BaseColorMap));
	o_Params.AddParam(new effParamFloat(c_Metallic, c_Metallic, m_Metallic));
	o_Params.AddParam(new effParamFloat(c_Roughness, c_Roughness, m_Roughness));
	o_Params.AddParam(make_texture_param(c_MetallicRoughnessMap, m_MetallicRoughnessMap));
	o_Params.AddParam(make_texture_param(c_OcclusionMap, m_OcclusionMap));
	o_Params.AddParam(new effParamFloat(c_OcclusionStrength, c_OcclusionStrength, m_OcclusionStrength));
	o_Params.AddParam(new effParamColor(c_Emissive, c_Emissive, m_Emissive));
	o_Params.AddParam(make_texture_param(c_EmissiveMap, m_EmissiveMap));
	o_Params.AddParam(new effParamFloat(c_EmissiveStrength, c_EmissiveStrength, m_EmissiveStrength));
	o_Params.AddParam(new effParamInt(c_AlphaMode, c_AlphaMode, (int)m_AlphaMode));
	o_Params.AddParam(new effParamFloat(c_AlphaCutoff, c_AlphaCutoff, m_AlphaCutoff));
	o_Params.AddParam(make_texture_param(c_ReflectionMap, m_ReflectionMap));
	o_Params.AddParam(new effParamFloat(c_ReflectionMapAngle, c_ReflectionMapAngle, m_ReflectionMapAngle));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effPbrData effPbrData::FromParams(const effShaderParams& i_Params)
{
	effPbrData data;
	read_color(i_Params, c_BaseColor, data.m_BaseColor);
	read_texture(i_Params, c_BaseColorMap, data.m_BaseColorMap);
	read_float(i_Params, c_Metallic, data.m_Metallic);
	read_float(i_Params, c_Roughness, data.m_Roughness);
	read_texture(i_Params, c_MetallicRoughnessMap, data.m_MetallicRoughnessMap);
	read_texture(i_Params, c_OcclusionMap, data.m_OcclusionMap);
	read_float(i_Params, c_OcclusionStrength, data.m_OcclusionStrength);
	read_color(i_Params, c_Emissive, data.m_Emissive);
	read_texture(i_Params, c_EmissiveMap, data.m_EmissiveMap);
	read_float(i_Params, c_EmissiveStrength, data.m_EmissiveStrength);
	if (const effParamInt* mode = i_Params.FindIntParam(c_AlphaMode))
	{
		const int value = mode->GetProperty().GetValue();
		data.m_AlphaMode = (value >= e_Opaque && value <= e_Blend) ? (AlphaMode)value : e_Opaque;
	}
	read_float(i_Params, c_AlphaCutoff, data.m_AlphaCutoff);
	read_texture(i_Params, c_ReflectionMap, data.m_ReflectionMap);
	read_float(i_Params, c_ReflectionMapAngle, data.m_ReflectionMapAngle);
	return data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBlinnData effPbrData::MakeBlinnFallback() const
{
	effBlinnData blinn;

	const float metallic = clamp01(m_Metallic);
	const float roughness = clamp01(m_Roughness);
	const float r = m_BaseColor.GetRed();
	const float g = m_BaseColor.GetGreen();
	const float b = m_BaseColor.GetBlue();

	// Only BLEND materials are see-through; Blinn has no alpha cutoff, so
	// MASK is drawn opaque
	const float alpha = (m_AlphaMode == e_Blend) ? m_BaseColor.GetAlpha() : 1.0f;
	blinn.m_Transparency = alpha;
	blinn.m_ColorDiffuse.Set(r, g, b, alpha);

	blinn.m_ColorAmbient.Set(1, 1, 1, 1);
	blinn.m_ColorEmissive.Set(m_Emissive.GetRed() * m_EmissiveStrength,
							  m_Emissive.GetGreen() * m_EmissiveStrength,
							  m_Emissive.GetBlue() * m_EmissiveStrength, 1);

	// Dielectrics reflect ~4% untinted, metals reflect their base color
	const float dielectric_spec = 0.04f;
	blinn.m_ColorSpecular.Set(dielectric_spec + (r - dielectric_spec) * metallic,
							  dielectric_spec + (g - dielectric_spec) * metallic,
							  dielectric_spec + (b - dielectric_spec) * metallic, 1);
	// Blinn.fx shininess is 0..1, larger is a tighter highlight
	blinn.m_SpecularPower = 1.0f - roughness;
	blinn.m_DiffuseRoughness = roughness;
	blinn.m_Reflectivity = metallic;

	// The base color factor multiplies the texture (Blinn.fx does the same)
	blinn.m_NameDiffuse = m_BaseColorMap;
	blinn.m_NameEnvironment = m_ReflectionMap;
	return blinn;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::shared_ptr<effShaderParams> effPbrData::MakeBlinnFallbackParams(const effShaderParams& i_Pbr)
{
	std::shared_ptr<effShaderParams> blinn(new effShaderParams());
	FromParams(i_Pbr).MakeBlinnFallback().AddToParams(*blinn);
	CopyTexturesToBlinnFallback(i_Pbr, *blinn);
	return blinn;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effPbrData::CopyTexturesToBlinnFallback(const effShaderParams& i_Pbr, effShaderParams& io_Blinn)
{
	copy_texture(i_Pbr, c_BaseColorMap, io_Blinn, "diffuseMap");
	copy_texture(i_Pbr, c_ReflectionMap, io_Blinn, "cubeMap");
}
