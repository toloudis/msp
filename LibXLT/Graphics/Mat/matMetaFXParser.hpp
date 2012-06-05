/*****************************************************************************
**	matMetaFXParser.hpp
**
**	matMetaFXParser reads and writes the multiple representations of 
**	a effect shader to and from file.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_METAFXPARSER_HPP
#error matMetaFXParser.hpp multiply included
#endif
#define MAT_METAFXPARSER_HPP


//============================================================================
//	forward references
//============================================================================
class fsLocator;
class matMetaFX;

//============================================================================
//============================================================================
namespace matMetaFXParser
{
	//------------------------------------------------------------------------
	// Reads multi-format shader from file. 
	//------------------------------------------------------------------------
	void ReadMetaFX( const fsLocator &i_Locator,
						matMetaFX &o_ShaderData);

	//--------------------------------------------------------------------
	// Writes multi-format shader to file.
	//--------------------------------------------------------------------
	void WriteMetaFX( const fsLocator &i_Locator,
					  const matMetaFX& i_ShaderData );
};
