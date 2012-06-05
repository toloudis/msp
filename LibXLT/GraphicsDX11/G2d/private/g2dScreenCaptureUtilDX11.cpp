/****************************************************************************\
**  g2dScreenCaptureUtilDX11.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g2d/private/g2dScreenCaptureUtilDX11.hpp"

#include <windows.h>
#undef CreateFile
#undef DeleteFile

#include "GraphicsDX11/g2d/private/g2dDX11SurfaceUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dImageSave.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dImageDX11.hpp"
#include "GraphicsDX11/g2d/g2dWindowDX11.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"


//============================================================================
//============================================================================
namespace
{
/*
//
//	BMP functions
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fill_bmp_header(BITMAPINFOHEADER &i_Header, 
					 int i_Width, 
					 int i_Height,
					 int i_DestBitsPerPixel, 
					 int i_DestImageSize)
{
	//--------------------------------------------------------------//
	// BitmapInfoHeader
	//
	i_Header.biSize				= sizeof( BITMAPINFOHEADER );
	i_Header.biWidth			= i_Width;
	i_Header.biHeight			= i_Height;
	i_Header.biPlanes			= 1;
	i_Header.biBitCount			= (unsigned short)i_DestBitsPerPixel;
	i_Header.biCompression		= BI_RGB;
	i_Header.biSizeImage		= i_DestImageSize;
	i_Header.biXPelsPerMeter	= i_Width;
	i_Header.biYPelsPerMeter	= i_Height;
	i_Header.biClrUsed			= 0;
	i_Header.biClrImportant		= 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void write_bmp_header(fsFileStream &i_File, 
					  int i_Width, 
					  int i_Height,
					  int i_DestBitsPerPixel, 
					  int i_PaletteSize,
					  int i_DestImageSize)
{
	//--------------------------------------------------------------//
	// BitmapFileHeader
	//
	BITMAPINFOHEADER	bih;
	BITMAPFILEHEADER	bfh;

	bfh.bfType		= 0x4d42;
	bfh.bfSize		= sizeof( BITMAPFILEHEADER ) + sizeof( bih ) +
					  i_PaletteSize + i_DestImageSize;
	bfh.bfReserved1	= 0;
	bfh.bfReserved2	= 0;
	bfh.bfOffBits	= sizeof( BITMAPFILEHEADER ) + sizeof( bih ) +
					  i_PaletteSize;

	i_File.Write( sizeof( bfh ), &bfh );

	//--------------------------------------------------------------//
	// BitmapInfoHeader
	//
	fill_bmp_header(bih, i_Width, i_Height, i_DestBitsPerPixel, i_DestImageSize);

	i_File.Write( sizeof( bih ), &bih );
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void write_bmp_header(	fsFileStream &i_File,
						g2dD3D11SurfacePtr i_pSurface,
						bool	i_bGreyscale = false,
						int		i_Sampling = 1,
						RECT*	i_pRect = NULL )
{
	D3DSURFACE_DESC	SDesc;
	HRESULT	op_result;

	op_result = i_pSurface->GetDesc( &SDesc );
	if ( op_result != D3D_OK )
	{
		DBG_ASSERT(op_result == D3D_OK, "Couldn't get surface info");
	}

	//--------------------------------------------------------------//
	// Get width and height from rect or surface
	//
	int Width	= (i_pRect) ? (i_pRect->right - i_pRect->left) : SDesc.Width;
	int Height	= (i_pRect) ? (i_pRect->bottom - i_pRect->top) : SDesc.Height;

	if (i_Sampling > 1)
	{
		Width /= i_Sampling;
		Height /= i_Sampling;
	}

	if (i_bGreyscale)
	{
		//--------------------------------------------------------------//
		// 8-bit format
		//
		int DestBitsPerPixel = 8;
		int PaletteSize = 256;
		int DestImageSize	= Width * Height;

		write_bmp_header(i_File, Width, Height,
					DestBitsPerPixel, PaletteSize, DestImageSize);

		//--------------------------------------------------------------//
		// Palette
		//
		PALETTEENTRY TempColor;
		for ( int loop = 0; loop < 256; ++loop )
		{
			TempColor.peBlue = (envType::UInt8) loop;
			TempColor.peRed = (envType::UInt8) loop;
			TempColor.peGreen = (envType::UInt8) loop;
			i_File.Write( sizeof(PALETTEENTRY), &TempColor );
		}
	}
	else
	{
		//--------------------------------------------------------------//
		// 24-bit format
		//
		int DestBitsPerPixel = 24;
		int PaletteSize = 0;
		int DestImageSize	= 3 * Width * Height;

		write_bmp_header(i_File, Width, Height,
					DestBitsPerPixel, PaletteSize, DestImageSize);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void write_bmp_data(fsFileStream &i_File,
					g2dD3D11SurfacePtr i_pSurface,
					bool	i_bGreyscale = false,
					int		i_Sampling = 1,
					RECT *i_pRect = NULL)
{
	int wid=0, ht=0, bpp=0;
	int bufferSize = 0;
	envType::UInt8* pBuffer = NULL;
	g2dDX11SurfaceUtil::GetSurfacePixels(i_pSurface,
									 &pBuffer,
									 &bufferSize,
									 &wid,
									 &ht,
									 &bpp,
									 i_Sampling,
									 i_bGreyscale,
									 i_pRect);
	i_File.Write( bufferSize, pBuffer );
	delete [] pBuffer;
}
*/

//----------------------------------------------------------------------------
// capture back buffer to surface in memory
// the returned value must be released() by the caller.
//----------------------------------------------------------------------------
ID3D11Texture2D* capture_rendertarget(ID3D11Resource* i_pSource, DXGI_FORMAT format, int i_Width, int i_Height,
									  bool i_bFirstMipmap = false)
{
//	1. Create a USAGE_STAGING texture with the CPU_ACCESS_READ flag. 
//		This will allocate memory optimized for CPU reads -- likely in cached system RAM.
	//	create the surface
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = i_Width;
	desc.Height = i_Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;

	ID3D11Texture2D* pSurface = NULL;
	HRESULT op_result = g2dDX11Global::g_pDevice->CreateTexture2D(
	  &desc,
	  NULL,
	  &pSurface
	);
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError(op_result);
		DBG_ASSERT( false, "Error allocating offscreen surface (w" << i_Width << "-h" << i_Height << "-f" << format << ")");
	}
	
//	2. Call CopyResource() or CopySubresourceRegion() to copy from 
//		whatever the source resource is (e.g. your rendertarget) into the staging resource.

	if (i_bFirstMipmap)
	{
		g2dDX11Global::g_pDeviceContext->CopySubresourceRegion(pSurface, D3D11CalcSubresource(0,0,1), 
			0, 0, 0, i_pSource, 0, NULL);
	}
	else
	{
		g2dDX11Global::g_pDeviceContext->CopyResource(pSurface, i_pSource);
	}
	
//	3. (optional) Call Flush() to make sure the copy command gets sent to the GPU immediately.

	g2dDX11Global::g_pDeviceContext->Flush();

//	4. (optional) Call Map() on the staging resource with MAP_READ and 
//		the DO_NOT_WAIT flag. This will return E_WASSTILLRENDERING if the copy hasn't 
//		finished yet. You can then go do something else for a while and try again later.
//	5. Once you've got nothing else to do, wait for the copy to finish by 
//		calling Map() without the DO_NOT_WAIT flag.

	return pSurface;
}

} // end of anonymous namespace




//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dScreenCaptureUtilDX11::g2dScreenCaptureUtilDX11()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dScreenCaptureUtilDX11::~g2dScreenCaptureUtilDX11()
{
}

//------------------------------------------------------------------------
//	CaptureWindowToFile writes screen to given filename, the
//	format of this image is up to the implementation.
//  This should be used for screen captures for PR shots.
//
//	the sample rate is how many input pixels per output pixel
//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
//------------------------------------------------------------------------
//virtual 
void g2dScreenCaptureUtilDX11::CaptureWindowToFile( g2dWindow& i_Window,
						const fsLocator& i_Locator, 
						const int i_Sampling )
{
	if (fsFileUtil::FileExists(i_Locator))
		fsFileUtil::DeleteFile(i_Locator);

	g2dImageSave::Save( i_Locator, &i_Window );
}

void g2dScreenCaptureUtilDX11::CaptureImageToFile( g2dImage& i_Image,
						const fsLocator& i_Locator, 
						const int i_Sampling )
{
	g2dImageDX11* pImageD3D = dynamic_cast<g2dImageDX11*>(&i_Image);
	if (pImageD3D != NULL)
	{
		if (fsFileUtil::FileExists(i_Locator))
			fsFileUtil::DeleteFile(i_Locator);

		g2dD3D11TexturePtr lpSurface = pImageD3D->GetSurface();

		bool bGreyScale = false;
		int  Sampling	= i_Sampling;

		std::string dir;
		fsFileUtil::LocatorToANSIFilename( i_Locator, dir );
		g2dImageSave::Save(i_Locator, &i_Image);

		// don't need to release surface since the image owns it.
		//lpSurface->Release();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g2dScreenCaptureUtilDX11::CaptureWindowToImage( g2dWindow& i_Window,
													g2dImage*& o_Image, 
													bool backBuffer /*= true*/ )
{
	g2dWindowDX11* window = dynamic_cast<g2dWindowDX11*>(&i_Window);
	DBG_ASSERT(window != NULL, "Expected a D3D Window");

	int width, height;
	window->GetDimensions(width, height);

	DXGI_FORMAT format;
	format = g2dDX11Global::D3DFormatFromPFD( window->GetBackBufferPixelFormat() );

	g2dD3D11RenderTargetPtr buffer = window->GetBackBuffer();
	DBG_ASSERT(buffer, "error getting back buffer");
	ID3D11Resource* pBufferResource = NULL;
	buffer->GetResource(&pBufferResource);
	DBG_ASSERT(pBufferResource, "error getting back buffer resource");

	g2dD3D11TexturePtr lpSurface = NULL;
	lpSurface = capture_rendertarget(pBufferResource, format, width, height);
	// ownership of lpSurface goes to o_Image.
	o_Image = new g2dImageDX11(lpSurface);

	pBufferResource->Release();
	buffer->Release();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g2dScreenCaptureUtilDX11::CaptureRenderTargetToImage( g2dRenderTarget& i_Window,
															g2dImage*& o_Image)
{
	matRenderTargetTexture* window = dynamic_cast<matRenderTargetTexture*>(&i_Window);
	DBG_ASSERT(window != NULL, "Expected a D3D Window");

	int width, height;
	window->GetDimensions(width, height);

	DXGI_FORMAT format;
	format = g2dDX11Global::D3DFormatFromPFD( window->GetPixelFormat() );

	g2dD3D11TexturePtr buffer = window->GetTextureSurface();

	g2dD3D11TexturePtr lpSurface = NULL;
	lpSurface = capture_rendertarget(buffer, format, width, height);
	if (lpSurface != NULL)
	{
		// ownership of lpSurface goes to o_Image.
		o_Image = new g2dImageDX11(lpSurface);
	}
	else
	{
		DBG_ERROR("Couldn't copy surface");
		throw g2dCaptureX();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g2dScreenCaptureUtilDX11::Capture2DTextureToImage( matTexture* i_pTexture,
														g2dImage*& o_Image)
{
	matTextureDX11* texture = dynamic_cast<matTextureDX11*>(i_pTexture);
	DBG_ASSERT(texture != NULL, "Expected a D3D texture");

	int width = texture->GetWidth();
	int height = texture->GetHeight();

	DXGI_FORMAT format;
	format = g2dDX11Global::D3DFormatFromPFD( texture->GetPixelFormat() );

	g2dD3D11ResourcePtr buffer = texture->GetResource();

	g2dD3D11TexturePtr lpSurface = NULL;
	lpSurface = capture_rendertarget(buffer, format, width, height, true);
	if (lpSurface != NULL)
	{
		// ownership of lpSurface goes to o_Image.
		o_Image = new g2dImageDX11(lpSurface);
	}
	else
	{
		DBG_ERROR("Couldn't copy surface");
		throw g2dCaptureX();
	}
}

