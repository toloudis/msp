/*****************************************************************************
**  optxMaterialTypes.h
**
**      The material and texture tables the device programs read
**	(g3dMaterialTableOptiX and g3dTextureTableOptiX build them):
**
**	- optxTexture: a CUDA texture object, its size (for ray-cone mip
**	  levels) and the swizzle that makes a sample read as Vulkan's sampler
**	  gives it (cuExternalImageCache::Swizzle). sRGB decoding is in the
**	  texture object.
**	- optxMaterial: effTraceMaterial (Graphics/Eff) with texture table
**	  indices in place of textures (c_optxNoTexture: none). Each build input
**	  of a GAS names its material (optxGeometryInput::m_Material).
**
**	Plain C structs: the host and nvcc must lay them out the same way.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXMATERIALTYPES_H
#error optxMaterialTypes.h multiply included
#endif
#define GOX_OPTXMATERIALTYPES_H

namespace gox
{
	const unsigned int c_optxNoTexture = 0xFFFFFFFFu;
	const unsigned int c_optxNoMaterial = 0xFFFFFFFFu;

	enum optxSwizzle
	{
		e_optxSwizzleRGBA = 0,
		e_optxSwizzleBGRA = 1,
		e_optxSwizzleR001 = 2,
		e_optxSwizzleRG01 = 3,
		e_optxSwizzle000A = 4,
		e_optxSwizzleRGB1 = 5
	};

	struct optxTexture
	{
		unsigned long long	m_Object;		// CUtexObject
		unsigned int		m_Width;
		unsigned int		m_Height;
		unsigned int		m_Swizzle;		// optxSwizzle
		unsigned int		m_Pad;
	};

	enum optxMaterialFlags
	{
		e_optxMaterialOpacityAlpha = 1u << 0,	// the opacity map's alpha (a PBR base colour map), else its red
		e_optxMaterialCutOut = 1u << 1,			// MASK: below m_Cutoff is a hole
		e_optxMaterialTransparent = 1u << 2,	// opacity below 1 lets light through (stochastically)
		e_optxMaterialGlossBlinn = 1u << 3,		// m_GlossMap scales Blinn shininess m_Shininess
		e_optxMaterialGlossPhong = 1u << 4		// m_GlossMap scales a Phong exponent m_Shininess
	};

	struct optxMaterial
	{
		float			m_BaseColor[3];
		unsigned int	m_BaseColorMap;
		float			m_Emission[3];			// colour times strength
		unsigned int	m_EmissionMap;
		float			m_SpecularColor[3];
		unsigned int	m_SpecularColorMap;
		float			m_Metallic;
		float			m_Roughness;
		unsigned int	m_MetallicRoughnessMap;	// B metallic, G roughness (multiplying)
		float			m_Specular;
		float			m_Ior;
		float			m_Shininess;
		unsigned int	m_GlossMap;
		unsigned int	m_Flags;				// optxMaterialFlags
		float			m_NormalScale;
		unsigned int	m_NormalMap;
		float			m_Opacity;
		float			m_Cutoff;
		unsigned int	m_OpacityMap;
		float			m_UV[6];				// u' = [0]u + [1]v + [2], v' = [3]u + [4]v + [5]
		float			m_ReflectionRoughness;	// a gloss map's roughness is at most this (1: no limit)
		float			m_Pad[4];			// to 144 bytes, a multiple of 16
	};
}
