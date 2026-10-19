/*****************************************************************************
**  g2dStubsVk.hpp
**
**      Vulkan implementations of the 2D graphics hooks that aren't ported
**	yet: they keep MachStudio running (every call returns a usable object
**	or a harmless value) and log GVK_NOT_YET the first time a call that
**	should do real work runs. Each class moves to its real package when a
**	later plan ports it.
**
**	  g2dResourceCounterVk		memory statistics (all zero)
**	  g2dFontUtilVk				font handles and the font bitmap path (no text drawing yet)
**	  g2dImageVk, g2dImageCreateVk	CPU images: pixels kept and readable; file loading not yet
**	  g2dImageSaveVk			saving images and windows (not yet)
**	  g2dScreenCaptureUtilVk	window and texture capture (not yet)
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G2DSTUBSVK_HPP
#error g2dStubsVk.hpp multiply included
#endif
#define GVK_G2DSTUBSVK_HPP

#ifndef G2D_RESOURCECOUNTER_HPP
#include "Graphics/G2d/g2dResourceCounter.hpp"
#endif
#ifndef G2D_FONTUTIL_HPP
#include "Graphics/G2d/g2dFontUtil.hpp"
#endif
#ifndef G2D_IMAGE_HPP
#include "Graphics/G2d/g2dImage.hpp"
#endif
#ifndef G2D_IMAGECREATE_HPP
#include "Graphics/G2d/g2dImageCreate.hpp"
#endif
#ifndef G2D_IMAGESAVE_HPP
#include "Graphics/G2d/g2dImageSave.hpp"
#endif
#ifndef G2D_SCREENCAPTUREUTIL_HPP
#include "Graphics/G2d/g2dScreenCaptureUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <map>
#include <string>
#include <vector>

namespace gvk
{
	class g2dResourceCounterVk : public g2dResourceCounterImpl
	{
		public:
			//	g2dResourceCounterDX11's categories: the texture and render
			//	target classes take one (counting comes with this class's port)
			enum eResourceCategory
			{
				eWindow,
				eRenderTarget,
				eShadow,
				eTexture
			};

			virtual float GetTotalVideoMemory();
			virtual float GetTotalSceneTextureMemory();
			virtual int GetNumSceneTextures();
			virtual float GetTotalFramebufferMemory();
			virtual int GetNumFramebufferTextures();
			virtual float GetTotalShadowMapMemory();
			virtual int GetNumShadowMapTextures();
			virtual float GetTotalVertexBufferMemory();
			virtual int GetNumVertexBuffers();
			virtual float GetTotalIndexBufferMemory();
			virtual int GetNumIndexBuffers();
	};

	class g2dFontUtilVk : public g2dFontUtilImpl
	{
		public:
			virtual ~g2dFontUtilVk();
			virtual g2dFontHandle LoadFont(const itString& i_Name, int i_Size);
			virtual void ReleaseFont(g2dFontHandle i_Handle);
			virtual void GetLoadedFontName(g2dFontHandle i_Font, itString& o_Name);
			virtual void GetLoadedFontName(g2dFontHandle i_Font, std::string& o_Name);
			virtual void GetTextSizeInPixels(g2dFontHandle i_Font, const itString& i_Text, int& o_Width, int& o_Height);
			virtual void SetGlobalFontBitmap(const fsLocator& i_FontPath);
			virtual void GetGlobalFontBitmap(fsLocator& o_FontPath);

		private:
			std::map<g2dFontHandle, std::string> m_Fonts;
			fsLocator m_FontBitmap;
	};

	class g2dImageVk : public g2dImage
	{
		public:
			virtual void Make(int i_Width, int i_Height, const g2dPFD& i_PFD, CreateLoc i_Loc = e_VideoMemory);
			virtual const g2dPFD& GetPixelFormat() const;
			virtual int GetWidth() const;
			virtual int GetHeight() const;
			virtual int GetStride() const;
			virtual void* Lock();
			virtual void Release();
			virtual void CopyImage(const g2dImage& i_SrcImage, const maFilter* i_Filter = NULL, float i_FilterWidth = 1,
								   float i_FilterHeight = 1, int i_SampleRateX = 1, int i_SampleRateY = 1);
			virtual void DrawImage(int i_DX1, int i_DY1, const g2dImage& i_SrcImage);
			virtual void DrawImage(int i_DX1, int i_DY1, int i_SX1, int i_SY1, int i_SX2, int i_SY2, const g2dImage& i_SrcImage);
			virtual void GetPixels(uint8_t** o_pBuffer, int* o_bufSize, int* o_width, int* o_height, int* o_bpp,
								   int i_sampling = 1, bool i_bGreyscale = false, const maIntRect* i_pRect = NULL);
			virtual void GetPixelsTGA(uint8_t** o_pBuffer, int* o_bufSize, int* o_width, int* o_height, int* o_bpp,
									  int i_Sampling = 1, const std::string& i_CompressionCode = std::string("None"),
									  const maIntRect* i_pRect = NULL);
			virtual void GetPixels(g2dPixelR16G16B16A16** o_pBuffer, int* o_width, int* o_height);
			virtual void GetPixels(g2dPixelR8G8B8A8** o_pBuffer, int* o_width, int* o_height);
			virtual void GetPixels(g2dPixelRGBA32F** o_pBuffer, int* o_width, int* o_height);
			virtual void GetPixels(g2dPixelRGBA32F* o_pBuffer, int i_Width, int i_Height);
			virtual void SwapChannels(int i_ChannelOne, int i_ChannelTwo);

		private:
			int bytes_per_pixel() const;

			g2dPFD m_PFD;
			int m_Width = 0;
			int m_Height = 0;
			std::vector<uint8_t> m_Pixels;
	};

	class g2dImageCreateVk : public g2dImageCreateImpl
	{
		public:
			virtual g2dImage* Make(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_Loc);
			virtual g2dImage* Load(const fsLocator& i_Locator, g2dImage::CreateLoc i_Loc);
	};

	class g2dImageSaveVk : public g2dImageSaveImpl
	{
		public:
			virtual void Save(const fsLocator& i_FileName, g2dWindow* i_pWin);
			virtual void Save(const fsLocator& i_FileName, const g2dImage* i_pImage);
	};

	class g2dScreenCaptureUtilVk : public g2dScreenCaptureImpl
	{
		public:
			virtual void CaptureWindowToFile(g2dWindow& i_Window, const fsLocator& i_Locator, const int i_Sampling);
			virtual void CaptureImageToFile(g2dImage& i_Image, const fsLocator& i_Locator, const int i_Sampling);
			virtual void CaptureWindowToImage(g2dWindow& i_Window, g2dImage*& img, bool backBuffer = true);
			virtual void CaptureRenderTargetToImage(g2dRenderTarget& i_Window, g2dImage*& img);
			virtual void Capture2DTextureToImage(matTexture* i_pTexture, g2dImage*& o_Image);
	};
}
