/****************************************************************************\
**  matTextureMgrGL.hpp
**
**      matTextureMgrGL.hpp is the windows implementation of the
**	Terawatt matTextureMgr.  In addition to just managing the list of
**	textures, the Windows matTextureMgr will reload them from disk
**	and restore the surfaces when the app has been suspended
**	(which usually results in the surfaces being lost).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_TEXTUREMGRGL_HPP
#error matTextureMgrGL.hpp multiply included
#endif
#define MAT_TEXTUREMGRGL_HPP

#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/mat/matTextureMgr.hpp"
#endif

//============================================================================
//============================================================================
class g2dRGBColor;
class matTextureGL;
class matPlainTexture;
class matMipTexture;
class matRenderTargetTexture;

//============================================================================
//============================================================================
class matTextureMgrGL : public matTextureMgrImpl
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	matTextureMgrGL();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~matTextureMgrGL();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Initialize();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeInitialize();

	static matTexture* GetPoisson();

	//------------------------------------------------------------------------
	//	GetTotalTextureMemory returns alleged amount of texture memory
	//	made available by the hardware.  Take this number with a grain of
	//	salt.
	//------------------------------------------------------------------------
	virtual unsigned int GetTotalTextureMemory();

	//--------------------------------------------------------------------
	// Get current video mem usage totals
	//--------------------------------------------------------------------
	static unsigned int GetNumRenderTargets();
	static unsigned int GetNumTextures();
	static unsigned int GetNumShadowMaps();
	static float GetTotalRSMapMemoryUsed();
	static unsigned int GetNumRSMaps();
	static float GetTotalFramebufferMemoryUsed();
	static unsigned int GetNumFramebufferTargets();
	static float GetTotalTextureMemoryUsed();

	//--------------------------------------------------------------------
	// LoadTexture will attempt to load a texture based on the given
	// locator, appending extensions until a file is found
	// i_bIgnoreTUVSuffix makes it skip the .tuv test, used to allow for
	// loading of textures w/ the same name as a tuv while maintaining
	// a single codebase for this function
	//--------------------------------------------------------------------
	virtual matTexture* LoadTexture(const fsLocator& i_Directory,
							const itString& i_Name,
							std::vector<itString>& o_UVANames,
							TEXTURE_TYPE i_Type = TEXTURE_TYPE_2D,
							bool i_bIgnoreTUVSuffix = false,
							bool mipmap_if_2D = true);

	//--------------------------------------------------------------------
	//	CreateShadowMap creates a shadow map texture that can 
	//	be rendered to. Use the GetRenderTargetAPI() function
	//	on the returned texture in order to render to it.
	//--------------------------------------------------------------------
	virtual matTexture* CreateShadowMap(int i_nTextureWidth, int i_nTextureHeight);

	//--------------------------------------------------------------------
	//	CreateShadowMap creates a reflective shadow map texture that can 
	//	be rendered to. Use the GetRenderTargetAPI() function
	//	on the returned texture in order to render to it.
	//--------------------------------------------------------------------
	virtual matTexture* CreateReflectiveShadowMap(int i_nTextureWidth, int i_nTextureHeight);

	//--------------------------------------------------------------------
	//	CreateRenderTargetTexture creates a texture that can 
	//	be rendered to
	//--------------------------------------------------------------------
	virtual matTexture* CreateRenderTargetTexture(int i_nTextureWidth,
												  int i_nTextureHeight,
												  bool i_bFloatingPoint,
												  const g2dPFD* i_PFD,
												  bool i_bAutoGenMipmap,
												  bool i_bAllocDepthBuffer,
												  matTextureMgr::eResourceCategory i_ResourceCategory,
												  bool i_bFloatDepth = false,
												  bool i_bAntiAlias = false );

	//--------------------------------------------------------------------
	//	CreateRenderTargetTexture creates a texture that can 
	//	be rendered to. The texture is initialized using data from the 
	//	input texture.
	//--------------------------------------------------------------------
	virtual matTexture* CreateRenderTargetTexture(matTexture* i_Texture,
												  bool i_bAutoGenMipmap,
												  matTextureMgr::eResourceCategory i_ResourceCategory);

	//--------------------------------------------------------------------
	//	CreateCubeRenderTargetTexture creates a cubemap texture that can 
	//	be rendered to. 
	//--------------------------------------------------------------------
	virtual matTexture* CreateCubeRenderTargetTexture(int i_nTextureWidth,
													  int i_nTextureHeight,
													  g2dPFD* i_PFD);

	//--------------------------------------------------------------------
	//	DestroyTexture causes the texture object to be destroyed and
	//	the texture information to be removed from memory.
	//--------------------------------------------------------------------
	virtual void DestroyTexture(matTexture* i_Texture);

	//--------------------------------------------------------------------
	//	DestroyAllTextures causes all textures to be destroyed.
	//--------------------------------------------------------------------
	virtual void DestroyAllTextures();

	//--------------------------------------------------------------------
	//	ReloadTextures reloads a texture
	//--------------------------------------------------------------------
	virtual void ReloadTexture( const fsLocator& i_Locator, bool i_bIsMipMap = true );

	//--------------------------------------------------------------------
	//	UnloadTextures resets the d3d surface of a texture
	//--------------------------------------------------------------------
	virtual void UnloadTexture( const fsLocator& i_Locator );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void SaveTextureToRgbaTiff(matTexture* i_pTexture, 
		const fsLocator& i_FilePathLocator);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void SaveTextureToRgbaPNG(matTexture* i_pTexture, 
		const fsLocator& i_FilePathLocator);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void SaveTextureToFile(matTexture* i_pTexture, 
		const fsLocator& i_FilePathLocator);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void FillTexture(matTexture* i_pTexture, const maFloatRGBA& i_Color);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void FillTexture(matTexture* i_pTexture, void* i_PixelData, int i_nByte);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void UpdateSurface( matTexture* i_pTexture, unsigned char* i_Data, int i_Size, int nMipLevel = 0 );

	//--------------------------------------------------------------------
	//	CreateTexture creates a uninitialized texture 
	//--------------------------------------------------------------------
	virtual matTexture* CreateTexture(int i_nTextureWidth,
		int i_nTextureHeight,
		const g2dPFD* i_PFD,
		bool i_bMipmap);

	//--------------------------------------------------------------------
	//	CreateTexture creates an initialized texture 
	//--------------------------------------------------------------------
	virtual matTexture* CreateTexture(int i_nTextureWidth,
		int i_nTextureHeight,
		const g2dPFD* i_PFD,
		bool i_bMipmap,
		void* i_PixelData);

	//--------------------------------------------------------------------
	//	CreateTexture3D creates a uninitialized volume texture 
	//--------------------------------------------------------------------
	virtual matTexture* CreateTexture3D(int i_nTextureWidth,
		int i_nTextureHeight,
		int i_nTextureDepth,
		const g2dPFD* i_PFD,
		BIND_TYPE i_Bindings );

	
	//--------------------------------------------------------------------
	//	CopyTexture copy one texture to another
	//--------------------------------------------------------------------
	virtual void CopyTexture(matTexture* i_SrcTex, matTexture* i_DstTex);

	//--------------------------------------------------------------------
	//	CreateTexture3D creates an initialized volume texture 
	//--------------------------------------------------------------------
	virtual matTexture* CreateTexture3D(int i_nTextureWidth,
		int i_nTextureHeight,
		int i_nTextureDepth,
		const g2dPFD* i_PFD,
		BIND_TYPE i_Bindings,
		void* i_PixelData);

	//--------------------------------------------------------------------
	//	combine 2 textures on render target
	//--------------------------------------------------------------------
	void MergeTransparentTextures(matTexture* i_pTexO, matTexture* i_pTexI, g2dRenderTarget* io_pTarget);

	//------------------------------------------------------------------------
	//	GetStaticCubeMapSupported returns true if static cubic environment
	//	maps can be created.
	//------------------------------------------------------------------------
	static bool GetStaticCubeMapSupported();

	//------------------------------------------------------------------------
	//	GetEnvironmentBumpMappingSupported()
	//
	//	Returns true if environment bump mapping is supported.
	//------------------------------------------------------------------------
	static bool GetEnvironmentBumpMappingSupported();

	//------------------------------------------------------------------------
	//	GetHeightMapBumpMappingSupported()
	//
	//	Returns true if bump mapping using height map is supported.
	//------------------------------------------------------------------------
	static bool GetHeightMapBumpMappingSupported();

	//------------------------------------------------------------------------
	//	GetHardwareTextureCompressionSupported()
	//
	//	Returns true if the video card supports hardware decompression of dds textures
	//------------------------------------------------------------------------
	static bool GetHardwareTextureCompressionSupported();

	//------------------------------------------------------------------------
	//	GetMaxTextureSize returns the maximum size allowed for a texture.
	//------------------------------------------------------------------------
	static int GetMaxTextureSize();

	//--------------------------------------------------------------------
	//	LoadMipTexture creates a texture from the given fsLocator.
	//	This can be used to load textures that are not in any type of
	//	pak file (ie usual image file formats - bmp, png, tga, jpg...).
	//	Mip map levels will be created for the texture.
	//--------------------------------------------------------------------
	static matMipTexture* LoadMipTexture(	const fsLocator& i_Locator,
									int i_WidthReduce,
									int i_HeightReduce);
	static void ReloadMipTexture(	const fsLocator& i_Locator, matMipTexture& io_Texture);

	//--------------------------------------------------------------------
	//	LoadCompressedTexture creates a texture from the given fsLocator.
	//	This can be used to load textures that are in .dds format.
	//	Mip map levels will be created for the texture.
	// Note:  The matTexture returned pointer can be a matMipTexture
	//		matStaticCubeTexture, or a matVolumeTexture (which doesn't exist yet).
	//--------------------------------------------------------------------
	static matTexture* LoadCompressedTexture( const fsLocator& i_Locator, TEXTURE_TYPE i_Type = TEXTURE_TYPE_UNKNOWN, bool mipmap_if_2D = true );

	//--------------------------------------------------------------------
	// ReloadCompressedTexture reloads the given texture file into the
	// given texture object.  If the attributes don't match up, this will
	// crash.
	//--------------------------------------------------------------------
	static void ReloadCompressedTexture( const fsLocator& i_Locator, matTextureGL* io_Texture, bool i_bIsMipMap);


	////--------------------------------------------------------------------
	////	CreatePlainTextureFromText creates a texture from the given text.
	////--------------------------------------------------------------------
	//static matPlainTexture* CreatePlainTextureFromText(g2dFontHandle i_Font,
	//											const std::vector<itString>& i_Texts,
	//											const std::vector<int>& i_XOffsets,
	//											const std::vector<int>& i_YOffsets,
	//											const std::vector<g2dRGBColor>& i_Colors,
	//											int i_nTextureWidth,
	//											int i_nTextureHeight );

	////--------------------------------------------------------------------
	////	CreatePlainTextureFromText creates a texture from the given text.
	////--------------------------------------------------------------------
	//static matPlainTexture* CreatePlainTextureFromText(const std::vector<g2dFontHandle>& i_Fonts,
	//											const std::vector<itString>& i_Texts,
	//											const std::vector<int>& i_XOffsets,
	//											const std::vector<g2dRGBColor>& i_Colors,
	//											int i_nTextureWidth,
	//											int i_nTextureHeight );

	////--------------------------------------------------------------------
	////	CreatePlainTextureFromText creates a texture from the given text.
	////--------------------------------------------------------------------
	//static matPlainTexture* CreatePlainTextureFromText(g2dFontHandle i_Font,
	//											const itString& i_Text,
	//											const g2dRGBColor& i_Color,
	//											int i_nTextureWidth,
	//											int i_nTextureHeight );

	////--------------------------------------------------------------------
	////	DrawTextToTexture causes the given text to be rendered to the
	////	texture.  The texture must be a matPlainTexture, or a assertion
	////	will be triggered.
	////--------------------------------------------------------------------
	//static void DrawTextToTexture(	matTexture* i_Texture,
	//						g2dFontHandle i_Font,
	//						const std::vector<itString>& i_Texts,
	//						const std::vector<int>& i_XOffsets,
	//						const std::vector<int>& i_YOffsets,
	//						const std::vector<g2dRGBColor>& i_Colors);

	////--------------------------------------------------------------------
	////	DrawTextToTexture causes the given text to be rendered to the
	////	texture.  The texture must be a matPlainTexture, or a assertion
	////	will be triggered.
	////--------------------------------------------------------------------
	//static void DrawTextToTexture(	matTexture* i_Texture,
	//						const std::vector<g2dFontHandle>& i_Fonts,
	//						const std::vector<itString>& i_Texts,
	//						const std::vector<int>& i_XOffsets,
	//						const std::vector<g2dRGBColor>& i_Colors);

	////--------------------------------------------------------------------
	////	DrawTextToTexture causes the given text to be rendered to the
	////	texture.  The texture must be a matPlainTexture, or a assertion
	////	will be triggered.
	////--------------------------------------------------------------------
	//static void DrawTextToTexture(	matTexture* i_Texture,
	//						g2dFontHandle i_Font,
	//						const itString& i_Text,
	//						const g2dRGBColor& i_Color);

	static void InitStates();
	static void CleanupStates();

private:
	//--------------------------------------------------------------------
	//	LoadPlainTexture creates a texture from the given fsLocator.
	//	This can be used to load textures that are not in any type of
	//	pak file (ie usual image file formats - bmp, png, tga, jpg...).
	//--------------------------------------------------------------------
	static matPlainTexture* LoadPlainTexture(	const fsLocator& i_Locator,
		int i_WidthReduce, int i_HeightReduce);

	//--------------------------------------------------------------------
	//	ReloadPlainTexture will load the data in the i_Locator file into
	//	the given i_Texture.  The dimensions must be the same or an
	//	exception will be thrown.  Also, there should be no width
	//	or height reduction on the original texture.
	//--------------------------------------------------------------------
	static void ReloadPlainTexture(matPlainTexture* io_Texture,
		const fsLocator& i_Locator);

	//--------------------------------------------------------------------
	//  LoadPlainTexture()
	//
	//	Creates a texture from the given data. This can be used to load
	//	bump map textures.
	//--------------------------------------------------------------------
	static matPlainTexture* LoadPlainTexture( int i_Width, int i_Height,
		const g2dPFD& i_Format, const char* i_pData );

	//--------------------------------------------------------------------
	//	LoadResourceTexture creates a texture from the given fsLocator.
	//	The locator is assumed to be the ID number in string form (e.g. "102")
	//--------------------------------------------------------------------
	static matPlainTexture* LoadResourceTexture(const fsLocator& i_Locator,
												int i_WidthReduce, int i_HeightReduce);

	//--------------------------------------------------------------------
	//	ReloadResourceTexture will load the data in the i_Locator file into
	//	the given i_Texture.  The dimensions must be the same or an
	//	exception will be thrown.  Also, there should be no width
	//	or height reduction on the original texture.
	//--------------------------------------------------------------------
	static void ReloadResourceTexture(matPlainTexture* io_Texture,
										const fsLocator& i_Locator);

};
