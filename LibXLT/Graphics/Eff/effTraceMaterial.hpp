/*****************************************************************************
**  effTraceMaterial.hpp
**
**      effTraceMaterial is a material as a ray tracer reads it (Graphics-
**	Optix): one model for every MachStudio material shader, glTF's
**	metallic-roughness (Pbr.fx), with the values and textures each shader
**	maps to it. Plain data, no GPU: Of converts a material's first layer.
**
**	Conversion (Of), by the material's shader (its .fx file, or for a
**	built-in effect its effect ID and data):
**	- Pbr.fx: one to one (effPbrData). The occlusion map is dropped: a path
**	  tracer computes occlusion.
**	- The other .fx material shaders, by semantic: base colour =
**	  MaterialDiffuse x DiffuseTexture; emission = g_emissive x
**	  g_emissiveIntensity x emissiveMap; specular colour = g_specular x
**	  specularMap; specular factor = g_specularPower clamped to [0, 1];
**	  metallic 0; IOR 1.5 (the Blinn family's g_IOR is a 0-1 Fresnel
**	  control, not an index), or the "IOR" of the reflection shaders.
**	  Roughness from the highlight (Gloss): Blinn's 0-1 shininess s gives
**	  1 - s; a Phong exponent n gives GGX alpha = sqrt(2 / (n + 2)),
**	  roughness = sqrt(alpha); a glossMap's red scales s or n first. Which
**	  rule a shader gets is one table (Rules in the .cpp); per shader:
**	  - Blinn, BlinnReflection, FC3DBlinn, CarPaint (its first lobe),
**	    Anisotropic (its g_shininess clamped to 1): the Blinn rule.
**	  - Phong, PhongReflection, Phong_wBump, FC3DPhong(Transparent): Phong.
**	  - Lambert: no specular.
**	  - Simple, Default, Cartoon: base colour only.
**	  - Skin, SubSurfaceScatter: base colour, g_specColor x g_specPower,
**	    the Phong rule on g_specGloss, and normalTex; BlinnSkin and the
**	    _wBlinnSpecular ones: base colour, the Blinn rule, and normalTex.
**	    No subsurface scattering.
**	  - SpecularFresnel, FC3DSpecularFresnel: base colour and the first
**	    specular lobe (g_specularColor0, the Phong rule on g_specularExp0).
**	  - Hair_SH (strand hair) and Bake: not surfaces, not traced
**	    (m_bTraced false).
**	- Built-in effects (no .fx; effect data): Phong ("default",
**	  testtess.fx: effPhongData, a Phong exponent), Solid and Textured
**	  (base colour), Toon and Water (base colour; Water's waves dropped).
**	- Every material: the normal map from NormalsData (scale m_BumpScale;
**	  a Skin shader's normalTex instead when set, scale g_bumpMapScale),
**	  opacity from effOpacity::Of, the UV transform from UVTransform(0).
**	- Dropped, listed in m_Dropped (Of logs each shader's list once): other
**	  material layers; fur and displacement (the surface is traced bare,
**	  undisplaced); glow and outline (post effects, not light); cube-map
**	  reflections (the BRDF reflects in Part 3d); and the shader's own
**	  effects named above (subsurface scattering, flakes, anisotropy,
**	  second lobes, water waves).
**
**	Double-sidedness is the fragment's (g3dFragment::GetDoubleSided), and
**	the tracer sees every triangle from both sides, so it isn't here.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef EFF_TRACEMATERIAL_HPP
#error effTraceMaterial.hpp multiply included
#endif
#define EFF_TRACEMATERIAL_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef EFF_OPACITY_HPP
#include "Graphics/Eff/effOpacity.hpp"
#endif

#include <string>
#include <vector>

class matMaterial;
class matTexture;


//============================================================================
//============================================================================
struct effTraceMaterial
{
	//	How the highlight gives roughness when there is a gloss map
	enum Gloss
	{
		e_GlossNone,	// m_Roughness as it is
		e_GlossBlinn,	// m_Shininess is Blinn's 0-1 shininess
		e_GlossPhong	// m_Shininess is a Phong exponent
	};

	bool			m_bTraced = true;		// false: not a surface (strand hair, bake)
	std::string		m_Shader;				// the shader's file name, or the built-in effect's ID

	maFloatRGBA		m_BaseColor = maFloatRGBA(1, 1, 1, 1);	// linear rgb; alpha unused (see m_Opacity)
	matTexture*		m_pBaseColorMap = nullptr;

	float			m_Metallic = 0.0f;
	float			m_Roughness = 1.0f;
	matTexture*		m_pMetallicRoughnessMap = nullptr;	// glTF: B metallic, G roughness

	Gloss			m_GlossMode = e_GlossNone;
	float			m_Shininess = 0.0f;					// with m_pGlossMap: what its red scales
	matTexture*		m_pGlossMap = nullptr;

	float			m_Specular = 0.0f;					// KHR_materials_specular's factor
	maFloatRGBA		m_SpecularColor = maFloatRGBA(1, 1, 1, 1);
	matTexture*		m_pSpecularColorMap = nullptr;
	float			m_Ior = 1.5f;						// KHR_materials_ior

	maFloatRGBA		m_Emission = maFloatRGBA(0, 0, 0, 1);	// times m_EmissionStrength
	float			m_EmissionStrength = 1.0f;
	matTexture*		m_pEmissionMap = nullptr;

	matTexture*		m_pNormalMap = nullptr;
	float			m_NormalScale = 1.0f;				// the raster's bump scale on the map's x and y

	effOpacity		m_Opacity;

	//	u' = [0] u + [1] v + [2], v' = [3] u + [4] v + [5] (effUVTransform)
	float			m_UVTransform[6] = { 1, 0, 0, 0, 1, 0 };

	std::vector<std::string>	m_Dropped;			// what the tracer ignores, in words

	//------------------------------------------------------------------------
	// Of - i_Material's first layer converted (see above)
	//------------------------------------------------------------------------
	static effTraceMaterial Of(const matMaterial& i_Material);

	//------------------------------------------------------------------------
	// The roughness of Blinn's 0-1 shininess, and of a Phong exponent
	//------------------------------------------------------------------------
	static float RoughnessFromShininess(float i_Shininess);
	static float RoughnessFromExponent(float i_Exponent);

	//------------------------------------------------------------------------
	// The roughness m_GlossMode gives for a gloss map value (its red, 1
	// without a map)
	//------------------------------------------------------------------------
	float RoughnessForGloss(float i_Gloss) const;

	//------------------------------------------------------------------------
	// Every texture it reads (each once, no nulls)
	//------------------------------------------------------------------------
	std::vector<matTexture*> GetTextures() const;
};
