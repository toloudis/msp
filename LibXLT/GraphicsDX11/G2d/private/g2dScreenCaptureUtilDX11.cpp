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

#include <DirectXTex.h>

#include <utility>
#include <vector>


//============================================================================
//============================================================================
namespace
{

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
		return NULL;
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

//----------------------------------------------------------------------------
//	A 2D texture's mip 0 of layer 0 in a staging texture, as DirectXTex's
//	CaptureTexture reads it: in the resource's format (i_ViewFormat, the
//	texture's PFD's, if that is typeless), a multisampled texture resolved.
//	B8G8R8A8 and B8G8R8X8 become R8G8B8A8 (alpha 255 for X8), as on Vulkan.
//	Throws g2dCaptureX if there is nothing to read back.
//----------------------------------------------------------------------------
ID3D11Texture2D* capture_texture(ID3D11Resource* i_pSource, DXGI_FORMAT i_ViewFormat)
{
	D3D11_RESOURCE_DIMENSION dimension = D3D11_RESOURCE_DIMENSION_UNKNOWN;
	if (i_pSource)
		i_pSource->GetType(&dimension);
	ID3D11Texture2D* source = NULL;
	if (dimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D ||
		!SUCCEEDED(i_pSource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&source)))
	{
		DBG_TRACE("Capture2DTextureToImage: no 2D texture to read back");
		throw g2dCaptureX();
	}
	D3D11_TEXTURE2D_DESC desc;
	source->GetDesc(&desc);
	const DXGI_FORMAT format = DirectX::IsTypeless(desc.Format) ? i_ViewFormat : desc.Format;

	ID3D11Texture2D* resolved = NULL;
	if (desc.SampleDesc.Count > 1)
	{
		D3D11_TEXTURE2D_DESC single = desc;
		single.MipLevels = 1;
		single.ArraySize = 1;
		single.Format = format;
		single.SampleDesc.Count = 1;
		single.SampleDesc.Quality = 0;
		single.Usage = D3D11_USAGE_DEFAULT;
		single.BindFlags = 0;
		single.CPUAccessFlags = 0;
		single.MiscFlags = 0;
		if (!SUCCEEDED(g2dDX11Global::g_pDevice->CreateTexture2D(&single, NULL, &resolved)))
		{
			source->Release();
			DBG_TRACE("Capture2DTextureToImage: couldn't make the texture to resolve into");
			throw g2dCaptureX();
		}
		g2dDX11Global::g_pDeviceContext->ResolveSubresource(resolved, 0, source, 0, format);
	}

	ID3D11Texture2D* staging = capture_rendertarget(resolved ? resolved : source, format, desc.Width, desc.Height, true);
	if (resolved)
		resolved->Release();
	source->Release();
	if (!staging)
	{
		throw g2dCaptureX();
	}

	const bool bgra = (format == DXGI_FORMAT_B8G8R8A8_UNORM || format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB);
	const bool bgrx = (format == DXGI_FORMAT_B8G8R8X8_UNORM || format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB);
	if (!bgra && !bgrx)
	{
		return staging;
	}

	//	swizzle into an R8G8B8A8 staging texture
	std::vector<uint8_t> pixels((size_t)desc.Width * desc.Height * 4);
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (!SUCCEEDED(g2dDX11Global::g_pDeviceContext->Map(staging, 0, D3D11_MAP_READ, 0, &mapped)))
	{
		staging->Release();
		throw g2dCaptureX();
	}
	for (UINT y = 0; y < desc.Height; y++)
	{
		const uint8_t* row = static_cast<const uint8_t*>(mapped.pData) + (size_t)y * mapped.RowPitch;
		uint8_t* out = &pixels[(size_t)y * desc.Width * 4];
		for (UINT x = 0; x < desc.Width; x++)
		{
			out[x * 4 + 0] = row[x * 4 + 2];
			out[x * 4 + 1] = row[x * 4 + 1];
			out[x * 4 + 2] = row[x * 4 + 0];
			out[x * 4 + 3] = bgrx ? 255 : row[x * 4 + 3];
		}
	}
	g2dDX11Global::g_pDeviceContext->Unmap(staging, 0);
	staging->Release();

	D3D11_TEXTURE2D_DESC rgba;
	rgba.Width = desc.Width;
	rgba.Height = desc.Height;
	rgba.MipLevels = 1;
	rgba.ArraySize = 1;
	rgba.Format = (format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB || format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB)
				  ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	rgba.SampleDesc.Count = 1;
	rgba.SampleDesc.Quality = 0;
	rgba.Usage = D3D11_USAGE_STAGING;
	rgba.BindFlags = 0;
	rgba.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	rgba.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA data = { pixels.data(), desc.Width * 4, 0 };
	ID3D11Texture2D* converted = NULL;
	if (!SUCCEEDED(g2dDX11Global::g_pDevice->CreateTexture2D(&rgba, &data, &converted)))
	{
		throw g2dCaptureX();
	}
	return converted;
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
	if (texture == NULL)
		throw g2dCaptureX();

	g2dD3D11TexturePtr lpSurface = capture_texture(texture->GetResource(),
		g2dDX11Global::D3DFormatFromPFD( texture->GetPixelFormat() ));
	// ownership of lpSurface goes to o_Image.
	o_Image = new g2dImageDX11(lpSurface);
}

