/****************************************************************************\
**  matTextureLoaderTGAWin.hpp
**
**      matTextureLoaderTGAWin.hpp contains some helper functions for loading
**	TGA files data to surfaces in windows.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_TEXTURELOADERTGAWIN_HPP
#error matTextureLoaderTGAWin.hpp multiply included
#endif
#define MAT_TEXTURELOADERTGAWIN_HPP

class fsLocator;
class matMipTexture;
class matPlainTexture;

namespace matTextureLoaderTGA
{
	//------------------------------------------------------------------------
	// Load without mipmaps
	//------------------------------------------------------------------------
	void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce);

	//------------------------------------------------------------------------
	// Load with mipmaps.
	//------------------------------------------------------------------------
	void LoadTGA(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);
}
