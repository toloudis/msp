/****************************************************************************\
**  matTextureMgr.hpp
**
**      The matTextureMgr handles loading, creation, and deletion of
**	textures.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_TEXTUREMGR_HPP
#error matTextureMgr.hpp multiply included
#endif
#define MAT_TEXTUREMGR_HPP

#ifndef G2D_FONTHANDLE_HPP
#include "Graphics/g2d/g2dFontHandle.hpp"
#endif
#ifndef MAT_TEXTURETYPE_HPP
#include "Graphics/Mat/matTextureType.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class chReader;
class chWriter;
class fsLocator;
class fsResourceFinder;
class g2dPFD;
class g2dRenderTarget;
class itString;
class maFloatRGBA;
class matTexture;
class matTextureMgrImpl;
class matUVATexture;

//============================================================================
//	matTextureAdjuster provides an interface to be overridden at the game level
//	(when necessary) to adjust the loading of a texture
//============================================================================
class matTextureAdjuster
{
public:
	//------------------------------------------------------------------------
	//	GetUseMip returns true if a mip texture should be made as a default
	//	texture, false if a plain texture should be made
	//------------------------------------------------------------------------
	virtual inline bool GetUseMip(const fsLocator& i_Locator) const {return true;}

	//------------------------------------------------------------------------
	//	GetReduce returns the amount the texture should be reduced in each dimension
	//------------------------------------------------------------------------
	virtual inline void GetReduce(const fsLocator& i_Locator, int& o_WidthReduce, int& o_HeightReduce) const
					{o_WidthReduce = 0; o_HeightReduce = 0;}
};

//============================================================================
//============================================================================
namespace matTextureMgr
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	enum eResourceCategory
	{
		e_Framebuffer,
		e_ShadowMap,
		e_SceneTexture
	};

	void GetTextureName(const fsLocator& i_TextureFileName);
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetImplementation(matTextureMgrImpl* i_pImpl);

	//------------------------------------------------------------------------
	// If AllowNullTextures is true, the Load() function will return
	// NULL if the texture isn't found.  If it is false, it will
	// through an exception.  The default is false.
	//------------------------------------------------------------------------
	void SetAllowNullTextures(bool i_bAllow);
	bool IsAllowNullTextures();

	//------------------------------------------------------------------------
	// If SkipAllTextures is true, the Load() function will return
	// NULL for all textures without even trying to load them.
	//------------------------------------------------------------------------
	void SetSkipAllTextures(bool i_bSkip);
	bool IsSkipAllTextures();

	//------------------------------------------------------------------------
	// AutoCompress Textures
	//------------------------------------------------------------------------
	void SetCompressTextures(bool i_bSkip);
	bool IsCompressTextures();


	//------------------------------------------------------------------------
	//	GetTotalTextureMemory returns alleged amount of texture memory
	//	made available by the hardware.  Take this number with a grain of
	//	salt.
	//------------------------------------------------------------------------
	unsigned int GetTotalTextureMemory();

	//--------------------------------------------------------------------
	// LoadTexture will attempt to load a texture based on the given
	// locator. The version which takes a directory and itString is the primary
	// implementation of this function. The other versions are provided as a
	// convenience and simply forward on to the primary version.
	// It will attempt to lookup the texture in the loaded list
	// if not found it will load from disk.
	//--------------------------------------------------------------------
	matTexture* LoadTexture(const fsLocator& i_Locator, const TEXTURE_TYPE i_Type = TEXTURE_TYPE_2D, const bool mipmap_if_2D = true);
	matTexture* LoadTexture(const fsLocator& i_Directory, const itString& i_Name, const TEXTURE_TYPE i_Type = TEXTURE_TYPE_2D, const bool mipmap_if_2D = true, const bool i_bIgnoreTUVSuffix = false);
	matTexture* LoadTexture(const fsResourceFinder& i_Directory, const itString& i_Name, const TEXTURE_TYPE i_Type = TEXTURE_TYPE_2D, const bool mipmap_if_2D = true);

	//--------------------------------------------------------------------
	//	WriteUVATexture writes the given UVA to a binary file with the
	//	name i_Locator.  The number of std::strings in the i_Textures
	//	array must be the same as the number of texture pages in the
	//	texture.
	//--------------------------------------------------------------------
	void WriteUVATexture(	const matUVATexture& i_Texture,
							const fsLocator& i_Locator,
							const itString* i_Textures);

	//--------------------------------------------------------------------
	//	WriteUVATexture writes the given UVA using the given chWriter.
	//	The number of std::strings in the i_Textures
	//	array must be the same as the number of texture pages in the
	//	texture.
	//--------------------------------------------------------------------
	void WriteUVATexture(	const matUVATexture& i_Texture,
							chWriter& i_Writer,
							const itString* i_Textures);

	//--------------------------------------------------------------------
	//	ReadUVATexture reads the given UVA from a binary file with the
	//	name i_Locator.  The number of std::strings in the i_Textures
	//	array must be the same as the number of texture pages in the
	//	texture.
	//--------------------------------------------------------------------
	matUVATexture* ReadUVATexture(	const fsLocator& i_Locator,
									std::vector<itString>& o_Textures);

	//--------------------------------------------------------------------
	//	CreateUVATexture creates a "blank" UVA texture.  When this option
	//	is used to create a UVA texture, the textures later added to the
	//	UVA must be manually destroyed by the user.
	//--------------------------------------------------------------------
	matUVATexture* CreateUVATexture();

	//--------------------------------------------------------------------
	//	CreateShadowMap creates a shadow map texture that can 
	//	be rendered to. Use the GetRenderTargetAPI() function
	//	on the returned texture in order to render to it.
	//--------------------------------------------------------------------
	matTexture* CreateShadowMap(int i_nTextureWidth, int i_nTextureHeight);

	//--------------------------------------------------------------------
	//	CreateShadowMap creates a reflective shadow map texture that can 
	//	be rendered to. Use the GetRenderTargetAPI() function
	//	on the returned texture in order to render to it.
	//--------------------------------------------------------------------
	matTexture* CreateReflectiveShadowMap(int i_nTextureWidth, int i_nTextureHeight);

	//--------------------------------------------------------------------
	//	CreateRenderTargetTexture creates a texture that can 
	//	be rendered to. Use the GetRenderTargetAPI() function
	//	on the returned texture in order to render to it.
	//	If depth map is true, then the depth values rae written 
	//	to the texture.
	//--------------------------------------------------------------------
	matTexture* CreateRenderTargetTexture(int i_nTextureWidth,
										  int i_nTextureHeight,
										  bool i_bFloatingPoint = false,
										  const g2dPFD* i_PFD = NULL,
										  bool i_bAutoGenMipmap = false,
										  bool i_bAllocDepthBuffer = true,
										  eResourceCategory i_ResourceCategory = e_SceneTexture,
										  bool i_bFloatDepth = false,
										  bool i_bAntiAlias = false );

	//--------------------------------------------------------------------
	//	CreateRenderTargetTexture creates a texture that can 
	//	be rendered to. Use the GetRenderTargetAPI() function
	//	on the returned texture in order to render to it.
	//	The texture is initialized with data from the given input texture.
	//--------------------------------------------------------------------
	matTexture* CreateRenderTargetTexture(matTexture* i_Texture,
										  bool i_bAutoGenMipmap = false,
										  eResourceCategory i_ResourceCategory = e_SceneTexture);

	//--------------------------------------------------------------------
	//	CreateCubeRenderTargetTexture creates a cubemap texture that can 
	//	be rendered to. 
	//--------------------------------------------------------------------
	matTexture* CreateCubeRenderTargetTexture(int i_nTextureWidth,
											  int i_nTextureHeight,
											  g2dPFD* i_PFD = NULL);

	//--------------------------------------------------------------------
	//	IncrementReference - adds usage reference to the given texture.
	//	A call to ReleaseTexture() will decrement the reference.
	//	Returns false if texture could not be found.
	//--------------------------------------------------------------------
	bool IncrementReference(matTexture* i_Texture);

	//--------------------------------------------------------------------
	//	ReleaseTexture causes the texture object to be destroyed and
	//	the texture information to be removed from memory.
	//--------------------------------------------------------------------
	void ReleaseTexture(matTexture* i_Texture);

	//--------------------------------------------------------------------
	//	DestroyAllTextures causes all textures to be destroyed.
	//--------------------------------------------------------------------
	void DestroyAllTextures();

	//------------------------------------------------------------------------
	// DumpTextureList will DBG_WARNING all of the loaded textures
	//------------------------------------------------------------------------
	void DumpTextureList();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReloadTexture( const fsLocator& i_Locator, bool i_bIsMipMap = true );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UnloadTexture( const fsLocator& i_Locator);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SaveTextureToFile(matTexture* i_pTexture, 
		const fsLocator& i_FilePathLocator);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SaveTextureToRgbaTiff(matTexture* i_pTexture, 
		const fsLocator& i_FilePathLocator);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SaveTextureToRgbaPNG(matTexture* i_pTexture, 
		const fsLocator& i_FilePathLocator);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void FillTexture(matTexture* i_pTexture, const maFloatRGBA& i_Color);

	//------------------------------------------------------------------------
	// Filltexture fill texture with input pixel data
	// Note: i_Size shuold be the byte that i_PixelData contains
	//------------------------------------------------------------------------
	void FillTexture(matTexture* i_pTexture, void* i_PixelData, int i_nByte);

	void UpdateTexture( matTexture* i_pTexture, unsigned char* i_Data, int i_Size, int nMipLevel = 0 );

	//--------------------------------------------------------------------
	//	CreateTexture creates a texture 
	//  NOTE: The texture will be initialized if i_PixelData and i_Size
	//	are set.
	//--------------------------------------------------------------------
	matTexture* CreateTexture(int i_nTextureWidth,
		int i_nTextureHeight,
		const g2dPFD* i_PFD = NULL,
		bool i_bMipmap = false,
		void* i_PixelData = NULL);

	//--------------------------------------------------------------------
	//	CreateTexture creates a texture 
	//  NOTE: The texture will be initialized if i_PixelData and i_Size
	//	are set.
	//--------------------------------------------------------------------
	matTexture* CreateTexture3D(int i_nTextureWidth,
		int i_nTextureHeight,
		int i_nTextureDepth,
		const g2dPFD* i_PFD = NULL,
		BIND_TYPE i_Bindings = BIND_SHADER_RESOURCE,
		void* i_PixelData = NULL);

	//--------------------------------------------------------------------
	//	CopyTexture copy one texture to another
	//--------------------------------------------------------------------
	void CopyTexture(matTexture* i_SrcTex, matTexture* i_DstTex);

	//------------------------------------------------------------------------
	//	SetTextureAdjuster replaces the currently used matTextureAdjuster
	//	NOTE: a default one is created automatically on Init, this is not
	//	necessary for a game to do unless it requires better decision-making
	//	than that which is provided by default above
	//------------------------------------------------------------------------
	void SetTextureAdjuster(matTextureAdjuster *i_pTextureAdjuster);
	const matTextureAdjuster * GetTextureAdjuster();

	//--------------------------------------------------------------------
	//	combine 2 textures on render target
	//--------------------------------------------------------------------
	void MergeTransparentTextures(matTexture* i_pTexO, matTexture* i_pTexI, g2dRenderTarget* io_pTarget);
}
/*
class SurfaceLock
{
public:
	SurfaceLock( matTexture* i_pTexture, int nLevel );
	~SurfaceLock();
	void Unlock();
};
*/

//============================================================================
//============================================================================
class matTextureMgrImpl
{
public:
	//------------------------------------------------------------------------
	//	GetTotalTextureMemory returns alleged amount of texture memory
	//	made available by the hardware.  Take this number with a grain of
	//	salt.
	//------------------------------------------------------------------------
	virtual unsigned int GetTotalTextureMemory() = 0;

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
									bool mipmap_if_2D = true) = 0;

	//--------------------------------------------------------------------
	//	CreateShadowMap creates a shadow map texture that can 
	//	be rendered to. 
	//--------------------------------------------------------------------
	virtual matTexture* CreateShadowMap(int i_nTextureWidth, int i_nTextureHeight) = 0;

	//--------------------------------------------------------------------
	//	CreateShadowMap creates a reflective shadow map texture that can 
	//	be rendered to. 
	//--------------------------------------------------------------------
	virtual matTexture* CreateReflectiveShadowMap(int i_nTextureWidth, int i_nTextureHeight) = 0;

	//--------------------------------------------------------------------
	//	CreateRenderTargetTexture creates a texture that can 
	//	be rendered to. If depth map is true, then the depth values 
	//	are written to the texture.
	//--------------------------------------------------------------------
	virtual matTexture* CreateRenderTargetTexture(int i_nTextureWidth,
												  int i_nTextureHeight,
												  bool i_bFloatingPoint,
												  const g2dPFD* i_PFD,
												  bool i_bAutoGenMipmap,
												  bool i_bAllocDepthBuffer,
												  matTextureMgr::eResourceCategory i_ResourceCategory,
												  bool i_bFloatDepth,
												  bool i_bAntiAlias ) = 0;

	//--------------------------------------------------------------------
	//	CreateRenderTargetTexture creates a texture that can 
	//	be rendered to. The texture is initialized using data from the 
	//	input texture.
	//--------------------------------------------------------------------
	virtual matTexture* CreateRenderTargetTexture(matTexture* i_Texture,
												  bool i_bAutoGenMipmap,
												  matTextureMgr::eResourceCategory i_ResourceCategory) = 0;

	//--------------------------------------------------------------------
	//	CreateCubeRenderTargetTexture creates a cubemap texture that can 
	//	be rendered to. 
	//--------------------------------------------------------------------
	virtual matTexture* CreateCubeRenderTargetTexture(int i_nTextureWidth,
													  int i_nTextureHeight,
													  g2dPFD* i_PFD) = 0;

	//--------------------------------------------------------------------
	//	DestroyTexture causes the texture object to be destroyed and
	//	the texture information to be removed from memory.
	//--------------------------------------------------------------------
	virtual void DestroyTexture(matTexture* i_Texture) = 0;

	//--------------------------------------------------------------------
	//	DestroyAllTextures causes all textures to be destroyed.
	//--------------------------------------------------------------------
	virtual void DestroyAllTextures() = 0;

	//--------------------------------------------------------------------
	//	ReloadTextures reloads a texture
	//--------------------------------------------------------------------
	virtual void ReloadTexture( const fsLocator& i_Locator, bool i_bIsMipMap = true ) = 0;

	//--------------------------------------------------------------------
	//	UnloadTextures resets the d3d surface of a texture
	//--------------------------------------------------------------------
	virtual void UnloadTexture( const fsLocator& i_Locator ) = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void SaveTextureToRgbaTiff(matTexture* i_pTexture, 
		const fsLocator& i_FilePathLocator) = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void SaveTextureToRgbaPNG(matTexture* i_pTexture, 
		const fsLocator& i_FilePathLocator) = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void SaveTextureToFile(matTexture* i_pTexture, 
		const fsLocator& i_FilePathLocator) = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void FillTexture(matTexture* i_pTexture, const maFloatRGBA& i_Color) = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void FillTexture(matTexture* i_pTexture, void* i_PixelData, int i_nByte) = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void UpdateTexture( matTexture* i_pTexture, unsigned char* i_Data, int i_Size, int nMipLevel = 0 ) = 0;

	//--------------------------------------------------------------------
	//	CreateTexture creates a uninitialized texture 
	//--------------------------------------------------------------------
	virtual matTexture* CreateTexture(int i_nTextureWidth,
		int i_nTextureHeight,
		const g2dPFD* i_PFD,
		bool i_bMipmap) = 0;

	//--------------------------------------------------------------------
	//	CreateTexture creates an initialized texture 
	//--------------------------------------------------------------------
	virtual matTexture* CreateTexture(int i_nTextureWidth,
		int i_nTextureHeight,
		const g2dPFD* i_PFD,
		bool i_bMipmap,
		void* i_PixelData) = 0;

	//--------------------------------------------------------------------
	//	CreateTexture3D creates a uninitialized volume texture 
	//--------------------------------------------------------------------
	virtual matTexture* CreateTexture3D(int i_nTextureWidth,
		int i_nTextureHeight,
		int i_nTextureDepth,
		const g2dPFD* i_PFD,
		BIND_TYPE i_Bindings ) = 0;

	//--------------------------------------------------------------------
	//	CreateTexture3D creates an initialized volume texture 
	//--------------------------------------------------------------------
	virtual matTexture* CreateTexture3D(int i_nTextureWidth,
		int i_nTextureHeight,
		int i_nTextureDepth,
		const g2dPFD* i_PFD,
		BIND_TYPE i_Bindings,
		void* i_PixelData) = 0;

	//--------------------------------------------------------------------
	//	CopyTexture copy one texture to another
	//--------------------------------------------------------------------
	virtual void CopyTexture(matTexture* i_SrcTex, matTexture* i_DstTex) = 0;


	//--------------------------------------------------------------------
	//	combine 2 textures on render target
	//--------------------------------------------------------------------
	virtual void MergeTransparentTextures(matTexture* i_pTexO, matTexture* i_pTexI, g2dRenderTarget* io_pTarget) = 0;
};
