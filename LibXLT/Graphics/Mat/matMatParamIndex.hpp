/****************************************************************************\
**  matMatParamIndex.hpp
**
**      matMatParamIndex.hpp defines a system for indexing the parameters
**	of the matMaterial.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_MATPARAMINDEX_HPP
#error matMatParamIndex.hpp multiply included
#endif
#define MAT_MATPARAMINDEX_HPP

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace matMatParamIndex
{

enum matMatParamIndexType
{
	e_Ambient = 0,
	e_Diffuse,
	e_Specular,
	e_Emissive,
	e_SpecularPower,
	e_TextureTranslation0,
	e_TextureTranslation1,
	e_TextureTranslation2,
	e_TextureTranslation3,
	e_TextureScale0,
	e_TextureScale1,
	e_TextureScale2,
	e_TextureScale3,
	e_TextureRotation0,
	e_TextureRotation1,
	e_TextureRotation2,
	e_TextureRotation3
};

}

