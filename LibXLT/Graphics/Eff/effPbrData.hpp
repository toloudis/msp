/*****************************************************************************
**  effPbrData.hpp
**
**      effPbrData is a typed view of a Pbr.fx (glTF 2.0 metallic-roughness)
**	material's parameter bag. It is not an effShaderData and is never saved
**	itself: AddToParams writes the bag, FromParams reads one, and
**	MakeBlinnFallback gives the Blinn values DX11 draws the material with;
**	MakeBlinnFallbackParams, the Blinn bag DX11 draws and RenderMan exports.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef EFF_PBRDATA_HPP
#error effPbrData.hpp multiply included
#endif
#define EFF_PBRDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#include <memory>
#include <string>

class effBlinnData;
class effShaderParams;


//============================================================================
//============================================================================
class effPbrData
{
public:
	enum AlphaMode { e_Opaque = 0, e_Mask = 1, e_Blend = 2 };

	// The Pbr.fx parameter names (Pbr.effect.json)
	static const char* const c_ShaderName;
	static const char* const c_BaseColor;
	static const char* const c_BaseColorMap;
	static const char* const c_Metallic;
	static const char* const c_Roughness;
	static const char* const c_MetallicRoughnessMap;
	static const char* const c_OcclusionMap;
	static const char* const c_OcclusionStrength;
	static const char* const c_Emissive;
	static const char* const c_EmissiveMap;
	static const char* const c_EmissiveStrength;
	static const char* const c_AlphaMode;
	static const char* const c_AlphaCutoff;
	static const char* const c_ReflectionMap;
	static const char* const c_ReflectionMapAngle;

	// glTF defaults
	effPbrData();

	maFloatRGBA m_BaseColor;
	std::string m_BaseColorMap;
	float m_Metallic;
	float m_Roughness;
	// glTF channel layout: B = metallic, G = roughness
	std::string m_MetallicRoughnessMap;
	// R = occlusion
	std::string m_OcclusionMap;
	float m_OcclusionStrength;
	maFloatRGBA m_Emissive;
	std::string m_EmissiveMap;
	float m_EmissiveStrength;
	AlphaMode m_AlphaMode;
	float m_AlphaCutoff;
	std::string m_ReflectionMap;
	// degrees, as Blinn's
	float m_ReflectionMapAngle;

	bool operator == (const effPbrData& i_Other) const;
	bool operator != (const effPbrData& i_Other) const;

	//------------------------------------------------------------------------
	// Adds every field to o_Params under the names above (property name =
	// parameter name; the effect relabels them) and sets version 1. Does not
	// set the shader name.
	//------------------------------------------------------------------------
	void AddToParams(effShaderParams& o_Params) const;

	//------------------------------------------------------------------------
	// Reads the fields back; a missing parameter keeps its default, an alpha
	// mode outside 0..2 reads as e_Opaque. Texture names are the parameter
	// property's locator as an ANSI path.
	//------------------------------------------------------------------------
	static effPbrData FromParams(const effShaderParams& i_Params);

	//------------------------------------------------------------------------
	// The Blinn values DX11 draws a Pbr material with:
	//  base color -> diffuse color and texture
	//  metallic   -> specular color (blend of 4% grey and base color)
	//                and environment reflectivity
	//  roughness  -> shininess (1 - roughness)
	//  BLEND alpha -> transparency; MASK is drawn opaque
	//------------------------------------------------------------------------
	effBlinnData MakeBlinnFallback() const;

	//------------------------------------------------------------------------
	// The Blinn bag for a Pbr bag: MakeBlinnFallback's values, with the
	// textures the Pbr bag has loaded (base color map -> diffuseMap,
	// reflection map -> cubeMap) and their file names, which exporters read.
	// A new bag each call, from the Pbr bag's current values; no shader name.
	//
	// Why: Pbr.fx exists only on Vulkan, but a PBR material must still draw
	// and export where there is no PBR shader. Those places stand in Blinn
	// for it, and they use this one conversion so they agree:
	//  - GraphicsDX11's effPbrFallback (blinn_params) draws a Pbr material
	//    with this bag, cached per Pbr bag and rebuilt when its values change
	//  - RenderMan export (rmanExport::ExportShaderParams, called by
	//    ExportShaderInfo and the Character system's GatherTextures) writes
	//    it as Surface "Blinn", since RenderMan has no Pbr shader
	// A new consumer that can't draw Pbr.fx should call this too, rather
	// than converting the parameters itself.
	//------------------------------------------------------------------------
	static std::shared_ptr<effShaderParams> MakeBlinnFallbackParams(const effShaderParams& i_Pbr);

	//------------------------------------------------------------------------
	// io_Blinn (a MakeBlinnFallbackParams bag) gets i_Pbr's textures again:
	// for a cached bag whose Pbr textures were loaded after it was made
	//------------------------------------------------------------------------
	static void CopyTexturesToBlinnFallback(const effShaderParams& i_Pbr, effShaderParams& io_Blinn);
};
