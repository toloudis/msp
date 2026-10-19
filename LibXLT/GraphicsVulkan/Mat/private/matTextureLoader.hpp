/****************************************************************************\
**  matTextureLoader.hpp
**
**	GraphicsVulkan ports of GraphicsDX11's texture loaders
**	(matTextureLoaderWin, matTextureLoaderTGAWin, matTextureLoaderHDRWin,
**	matTextureLoaderDDSWin), on matImageDecode and matMipFilter instead of
**	DirectXTex and WIC. Each does what its DX11 counterpart does: decode
**	(the file type picked the same way), reduce by the texture adjuster's
**	width/height reduction, make the mips with the same filter, and put the
**	result in the texture.
**
**	Where DX11 asks DirectXTex for a one-level chain (GenerateMipMaps with
**	levels 1, which DirectXTex rejects, so DX11's load fails), the reduced
**	image is kept as a one-level texture. A load that fails (a missing,
**	corrupt or unsupported file) goes the way of matTextureLoaderWin's
**	failures: a missing texture if matTextureMgr allows them, else
**	fsUnknownX.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_MATTEXTURELOADER_HPP
#error matTextureLoader.hpp multiply included
#endif
#define GVK_MATTEXTURELOADER_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif

#include <cstdint>
#include <memory>

namespace gvk
{
	class matMipTexture;
	class matPlainTexture;
	class vkdImage;
	struct matDecodedImage;

	namespace matTextureLoader
	{
		//------------------------------------------------------------------------
		//	DirectX::CreateTexture's counterpart: an image of the decoded shape
		//	and format (vkdFormat's, from its DXGI format) holding every
		//	subresource. NULL, with an error logged, for a format Vulkan can't
		//	take.
		//------------------------------------------------------------------------
		std::unique_ptr<vkdImage> CreateImage(const matDecodedImage& i_Image, const char* i_Name);

		//------------------------------------------------------------------------
		// Load a file of any kind DX11 hands to DirectXTex and WIC (picked by
		// its extension), without mipmaps
		//------------------------------------------------------------------------
		void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator);

		//------------------------------------------------------------------------
		//	With mip levels (all of them, or one if i_bIsMipMap is false)
		//------------------------------------------------------------------------
		void LoadPNG(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);
		void LoadTGA(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);
		void LoadBMP(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);
		void LoadTIFF(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);

		//------------------------------------------------------------------------
		//	TIFF without mipmaps
		//------------------------------------------------------------------------
		void LoadTIFF(matPlainTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce);

		//------------------------------------------------------------------------
		//	A Windows resource (the locator is the ID number in string form,
		//	e.g. "102"): not loaded on Vulkan, the texture stays empty
		//------------------------------------------------------------------------
		void LoadResource(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce);
	}

	//	.TGA through DirectXTex's TGA reader (matImageDecode's port of it),
	//	mips with its box (TEX_FILTER_FANT) filter
	namespace matTextureLoaderTGA
	{
		void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce);
		void LoadTGA(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);
	}

	//	.HDR (Radiance RGBE) as R16G16B16A16_FLOAT, mips with the box filter
	namespace matTextureLoaderHDR
	{
		void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce);
		void LoadHDR(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap);
	}

	namespace matTextureLoaderDDS
	{
		// The types of compressed textures that can be created by this namespace
		typedef enum
		{
			e_VolumeTexture,
			e_CubeTexture,
			e_MipTexture,
		} eTextureType;

		// The file information provided in the compressed texture file
		struct DDSInfo
		{
			fsLocator		s_Locator;	// the locator to the file itself
			int				s_eType;
			int				s_nWidth;		// the loaded width (based on s_nWidthReduce)
			int				s_nHeight;		// the loaded height (based on s_nHeightReduce)
			bool			s_bHasAlpha;
			g2dPFD			s_Format;
			int				s_nMipCount;
			int				s_nWidthReduce;
			int				s_nHeightReduce;
		};

		//------------------------------------------------------------------------
		// Loads a .DDS file as it is (mips, faces, slices; block-compressed
		// formats stay compressed). Fills in o_Info (s_eType says which
		// texture class it is for) and o_DxgiFormat; NULL if the file can't
		// be read. The reduction and mipmap_if_2D are ignored, as in DX11.
		//------------------------------------------------------------------------
		std::unique_ptr<vkdImage> LoadDDS( struct DDSInfo& o_Info, const fsLocator& i_Locator,
			int i_WidthReduce, int i_HeightReduce, uint32_t& o_DxgiFormat,
			bool mipmap_if_2D = true);
	}
}
