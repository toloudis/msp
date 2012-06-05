/****************************************************************************\
**  matTextureLoaderHDRWin.hpp
**
**      matTextureLoaderHDRWin.hpp contains some helper functions for loading
**	HDR files data to surfaces in windows.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_TEXTURELOADERHDRWIN_HPP
#error matTextureLoaderHDRWin.hpp multiply included
#endif
#define MAT_TEXTURELOADERHDRWIN_HPP

class fsLocator;
class matMipTexture;
class matPlainTexture;

namespace matTextureLoaderHDR
{
	//------------------------------------------------------------------------
	// Load without mipmaps
	//------------------------------------------------------------------------
	void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce);

	//------------------------------------------------------------------------
	// Load with mipmaps.
	//------------------------------------------------------------------------
	void LoadHDR(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);
}
