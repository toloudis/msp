/****************************************************************************\
**  matUVATextureParseUtil.hpp
**
**      matUVATextureParseUtil is a namespace that reads and writes .tuv files
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_UVATEXTUREPARSEUTIL_HPP
#error matUVATextureParseUtil.hpp multiply included
#endif
#define MAT_UVATEXTUREPARSEUTIL_HPP

#ifndef MAT_UVATEXTURE_HPP
#include "matUVATexture.hpp"
#endif

#include <vector>
#include <string>

class chReader;
class chWriter;
class fsLocator;

namespace matUVATextureParseUtil
{

//--------------------------------------------------------------------
//	ReadUVATexture reads the given UVA using the given chReader.
//--------------------------------------------------------------------
matUVATexture* ReadUVATexture(	chReader& i_Reader,
								std::vector<std::string>& o_Names);

//--------------------------------------------------------------------
//	WriteUVATexture writes the given UVA using the given chWriter.
//--------------------------------------------------------------------
void WriteUVATexture(chWriter& i_Writer, const matUVATexture& i_Texture, const std::string* i_Textures);

}