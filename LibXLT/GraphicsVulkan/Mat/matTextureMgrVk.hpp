/****************************************************************************\
**  matTextureMgrVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11's matTextureMgrDX11, the
**	implementation of matTextureMgr: it loads (matTextureLoader), creates,
**	reloads and destroys the Vulkan texture classes and keeps the list of
**	them by kind. matTextureMgr itself does the reference counting by
**	locator.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_MATTEXTUREMGRVK_HPP
#error matTextureMgrVk.hpp multiply included
#endif
#define GVK_MATTEXTUREMGRVK_HPP

#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif

namespace gvk
{
	class matTextureVk;
	class matPlainTexture;
	class matMipTexture;
	class matRenderTargetTexture;

	class matTextureMgrVk : public matTextureMgrImpl
	{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		matTextureMgrVk();

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		~matTextureMgrVk();

		//------------------------------------------------------------------------
		//	Initialize makes the Poisson disk texture, DeInitialize releases it
		//------------------------------------------------------------------------
		void Initialize();
		void DeInitialize();

		static matTexture* GetPoisson();

		//------------------------------------------------------------------------
		//	The blend and depth states MergeTransparentTextures uses (made and
		//	released by g3dSystemVk, as DX11's)
		//------------------------------------------------------------------------
		static void InitStates();
		static void CleanupStates();

		//------------------------------------------------------------------------
		//	GetTotalTextureMemory returns the device-local memory, in bytes
		//	(DX11: DXGI's dedicated video memory)
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

		virtual matTexture* CreateShadowMap(int i_nTextureWidth, int i_nTextureHeight);
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
		//	UnloadTextures does nothing, as in DX11
		//--------------------------------------------------------------------
		virtual void UnloadTexture( const fsLocator& i_Locator );

		//------------------------------------------------------------------------
		//	FillTexture writes RGBA floats to every texel (as DX11: right only
		//	for R32G32B32A32_FLOAT textures)
		//------------------------------------------------------------------------
		virtual void FillTexture(matTexture* i_pTexture, const maFloatRGBA& i_Color);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		virtual void FillTexture(matTexture* i_pTexture, void* i_PixelData, int i_nByte);

		//------------------------------------------------------------------------
		//	UpdateTexture replaces mip 0 (as DX11, whatever nMipLevel says)
		//------------------------------------------------------------------------
		virtual void UpdateTexture( matTexture* i_pTexture, unsigned char* i_Data, int i_Size, int nMipLevel = 0 );

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
		// Note:  The matTexture returned pointer can be a matMipTexture
		//		matStaticCubeTexture, or a matVolumeTexture.
		//--------------------------------------------------------------------
		static matTexture* LoadCompressedTexture( const fsLocator& i_Locator, TEXTURE_TYPE i_Type = TEXTURE_TYPE_UNKNOWN, bool mipmap_if_2D = true );

		//--------------------------------------------------------------------
		// ReloadCompressedTexture reloads the given texture file into the
		// given texture object.
		//--------------------------------------------------------------------
		static void ReloadCompressedTexture( const fsLocator& i_Locator, matTextureVk* io_Texture, bool i_bIsMipMap);

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
		//	the given i_Texture.
		//--------------------------------------------------------------------
		static void ReloadPlainTexture(matPlainTexture* io_Texture,
			const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		//	LoadResourceTexture creates a texture from the given fsLocator.
		//	The locator is assumed to be the ID number in string form (e.g. "102")
		//--------------------------------------------------------------------
		static matPlainTexture* LoadResourceTexture(const fsLocator& i_Locator,
													int i_WidthReduce, int i_HeightReduce);
	};
}
