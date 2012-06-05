/****************************************************************************\
**  matTextureLoaderWin.hpp
**
**      matTextureLoaderWin.hpp contains some helper functions for loading
**	textures in windows.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_TEXTURELOADERWIN_HPP
#error matTextureLoaderWin.hpp multiply included
#endif
#define MAT_TEXTURELOADERWIN_HPP

class matPlainTexture;
class matMipTexture;
class matStaticCubeTexture;
class fsLocator;

namespace matTextureLoaderWin
{
	//------------------------------------------------------------------------
	// Use D3DX function to load images of many file types
	//------------------------------------------------------------------------
	void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator);

	//------------------------------------------------------------------------
	//	if i_TrueColor is true the texture will be returned in 32 bit format.
	//------------------------------------------------------------------------
	void LoadPNG(matPlainTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce);
	void ReloadPNG(matPlainTexture& io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce);
	void LoadPNG(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);
	void ReloadPNG(matMipTexture& io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);

	//------------------------------------------------------------------------
	//	i_Files must point to an array of 6 locators.  The order:
	//  +X -X +Y -Y +Z -Z.
	//------------------------------------------------------------------------
	void LoadPNG(	matStaticCubeTexture* io_Texture,
					const fsLocator* i_Files,
					bool i_TrueColor);
	void ReloadPNG(	matStaticCubeTexture& io_Texture,
					const fsLocator* i_Files,
					bool i_TrueColor);

	//------------------------------------------------------------------------
	//	if i_TrueColor is true the texture will be returned in 32 bit format.
	//------------------------------------------------------------------------
	void LoadBMP(matPlainTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce);
	void ReloadBMP(matPlainTexture& io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce);

	void LoadBMP(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);
	void ReloadBMP(matMipTexture& io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);

	//------------------------------------------------------------------------
	//	i_Files must point to an array of 6 locators.  The order:
	//  +X -X +Y -Y +Z -Z.
	//------------------------------------------------------------------------
	void LoadBMP(	matStaticCubeTexture* io_Texture,
					const fsLocator* i_Files,
					bool i_TrueColor);
	void ReloadBMP(	matStaticCubeTexture& io_Texture,
					const fsLocator* i_Files,
					bool i_TrueColor);

	//------------------------------------------------------------------------
	// Simply use D3DX to load the texture, ignoring the format request for now. 
	// I needed a quick method to load TGA files.
	//------------------------------------------------------------------------
	void LoadTGA(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);

	//------------------------------------------------------------------------
	//	Load 32-bit normal data to bump map texture.
	//------------------------------------------------------------------------
	void LoadNormalData(matPlainTexture* io_Texture, const fsLocator& i_Locator);

	//------------------------------------------------------------------------
	//	Reload 32-bit normal data to bump map texture.
	//------------------------------------------------------------------------
	void ReloadNormalData(matPlainTexture& io_Texture, const fsLocator& i_Locator);

	//------------------------------------------------------------------------
	//	Load 32-bit normal data to mipmap texture.
	//------------------------------------------------------------------------
	void LoadNormalData(matMipTexture* io_Texture, const fsLocator& i_Locator,
						int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);

	//------------------------------------------------------------------------
	//	Reload 32-bit normal data to mipmap texture.
	//------------------------------------------------------------------------
	void ReloadNormalData(matMipTexture& io_Texture, const fsLocator& i_Locator,
						int i_WidthReduce, int i_HeightReduce);

	//------------------------------------------------------------------------
	//	Load 16-bit reflection offset data to bump map texture.
	//------------------------------------------------------------------------
	void LoadReflectionData(matPlainTexture* io_Texture, const fsLocator& i_Locator);
	void ReloadReflectionData(matPlainTexture& io_Texture, const fsLocator& i_Locator);

	//------------------------------------------------------------------------
	//	Load 16-bit reflection data to mipmap texture.
	//------------------------------------------------------------------------
	void LoadReflectionData(matMipTexture* io_Texture, const fsLocator& i_Locator,
							int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);
	void ReloadReflectionData(matMipTexture& io_Texture, const fsLocator& i_Locator,
							int i_WidthReduce, int i_HeightReduce);

	//------------------------------------------------------------------------
	//	if i_TrueColor is true the texture will be returned in 32 bit format.
	//------------------------------------------------------------------------
	void LoadTIFF(matPlainTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce);
	void ReloadTIFF(matPlainTexture& io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce);
	void LoadTIFF(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);
	void ReloadTIFF(matMipTexture& io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);

	//------------------------------------------------------------------------
	// Use D3DX function to load a resource.  The locator is assumed to be
	//	the ID number in string form (e.g. "102")
	//------------------------------------------------------------------------
	void LoadResource(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce);
	void ReloadResource(matPlainTexture& io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce);

}
