/****************************************************************************\
**  matShaderIndex.hpp
**
**      matShaderIndex defines enumerations for the available
**	vertex and pixel shaders.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_SHADERINDEX_HPP
#error matShaderIndex.hpp multiply included
#endif
#define MAT_SHADERINDEX_HPP

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace matShaderIndex
{

enum SpecialEffect
{
	e_Normal = 0,
	e_UserDefined,
	e_Anisotropic,
	e_Membrane,
	e_Rainbow,
	e_ReflectRefract,
	e_Toon,
	e_CookTorrance,
	e_BumpDiffuse,
	e_BumpSpecularMap,
	e_NumShaders
};

}

