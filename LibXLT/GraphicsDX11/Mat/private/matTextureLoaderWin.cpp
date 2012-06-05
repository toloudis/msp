/****************************************************************************\
**  matTextureLoaderWin.hpp
**
**      matTextureLoaderWin.hpp contains some helper functions for loading
**	textures in windows.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/mat/private/matTextureLoaderWin.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g2d/g2dARGBColor.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/mat/matExceptionX.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mat/matTextureTracking.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dImageDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dImageDrawUtilDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dSurfaceLoaderDX11.hpp"
#include "GraphicsDX11/mat/matDX11GlobalWin.hpp"
//#include "GraphicsDX11/mat/matBumpMapUtil.hpp"
#include "GraphicsDX11/mat/matMipTexture.hpp"
#include "GraphicsDX11/mat/matPlainTexture.hpp"
#include "GraphicsDX11/mat/matStaticCubeTexture.hpp"

//============================================================================
//============================================================================
namespace matTextureLoaderWin
{

namespace
{

const DWORD l_FilterType = D3DX11_FILTER_TRIANGLE;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int choose_num_mip_levels(int i_Width, int i_Height)
{
	int smallest_dim = (i_Width < i_Height) ? i_Width : i_Height;

	int power_of_2 = 0;

	while( (smallest_dim & 0x01) == 0 )
	{
		smallest_dim >>= 1;
		power_of_2++;
	}

	return power_of_2 + 1;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void load_mip_texture(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, bool i_PNG, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	fsResourceTracker::MarkBegin(i_Locator);

	itString filename;
	fsFileUtil::LocatorToUnicodeString(i_Locator, filename);

	D3DX11_IMAGE_INFO srcInfo;
	HRESULT hr = D3DX11GetImageInfoFromFile(
	  filename.GetString(),
	  NULL,
	  &srcInfo,
	  NULL
	);

	int width = srcInfo.Width >> i_WidthReduce;
	int height = srcInfo.Height >> i_HeightReduce;
//	width = maFunctions::Lowest(width, int(g2dDX11Global::g_Caps.MaxTextureWidth));
//	height = maFunctions::Lowest(height, int(g2dDX11Global::g_Caps.MaxTextureHeight));

	D3DX11_IMAGE_LOAD_INFO loadInfo;
	loadInfo.Width = width;
	loadInfo.Height = height;
	loadInfo.Filter = l_FilterType;
	loadInfo.MipLevels = (i_bIsMipMap) ? 0 : 1;

	ID3D11Resource* pTexture = NULL;
	hr = D3DX11CreateTextureFromFile(
		g2dDX11Global::g_pDevice,
		filename.GetString(),
		&loadInfo,
		NULL,
		&pTexture,
		NULL
	);

	if ( !SUCCEEDED(hr) )
	{
		fsResourceTracker::Remove(i_Locator);

		g2dDX11Global::PrintDXError(hr);

		if (matTextureMgr::IsAllowNullTextures())
		{
			DBG_WARNING("Error in loading texture: " << filename);
			matTextureTracking::AddMissingTexture(i_Locator);
			if (pTexture)
			{
				pTexture->Release();
				pTexture = NULL;
			}
			return;
		}
		else
		{
			DBG_ERROR("Error in loading texture: " << filename);
			throw fsUnknownX(i_Locator);
		}
	}

	// clear out current io_Texture, and replace with pTexture
	io_Texture->SetSurface(pTexture);
	io_Texture->ReloadInfo();
	fsResourceTracker::MarkEnd(i_Locator);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void load_plain_texture(matPlainTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, bool i_PNG, int i_WidthReduce, int i_HeightReduce)
{
	fsResourceTracker::MarkBegin(i_Locator);

	itString filename;
	fsFileUtil::LocatorToUnicodeString( i_Locator, filename );

	D3DX11_IMAGE_INFO srcInfo;
	HRESULT hr = D3DX11GetImageInfoFromFile(
	  filename.GetString(),
	  NULL,
	  &srcInfo,
	  NULL
	);

	int width = srcInfo.Width >> i_WidthReduce;
	int height = srcInfo.Height >> i_HeightReduce;
//	width = maFunctions::Lowest(width, int(g2dDX11Global::g_Caps.MaxTextureWidth));
//	height = maFunctions::Lowest(height, int(g2dDX11Global::g_Caps.MaxTextureHeight));

	D3DX11_IMAGE_LOAD_INFO loadInfo;
	loadInfo.Width = width;
	loadInfo.Height = height;
	loadInfo.MipLevels = 1;

	ID3D11Resource* pTexture = NULL;
	hr = D3DX11CreateTextureFromFile(
		g2dDX11Global::g_pDevice,
		filename.GetString(),
		&loadInfo,
		NULL,
		&pTexture,
		NULL
	);
	if ( !SUCCEEDED(hr) )
	{
		fsResourceTracker::Remove(i_Locator);

		g2dDX11Global::PrintDXError(hr);

		if (matTextureMgr::IsAllowNullTextures())
		{
			DBG_WARNING("Error in loading texture: " << filename);
			matTextureTracking::AddMissingTexture(i_Locator);
			if (pTexture)
			{
				pTexture->Release();
				pTexture = NULL;
			}
			return;
		}
		else
		{
			DBG_ERROR("Error in loading texture: " << filename);
			throw fsUnknownX(i_Locator);
		}
	}

	// Set the new surface
	io_Texture->SetSurface( pTexture );

	// set size info into the texture
	io_Texture->ReloadInfo();

	fsResourceTracker::MarkEnd(i_Locator);
}


}	// end of namespace



//------------------------------------------------------------------------
// Use D3DX function to load images of many file types
//------------------------------------------------------------------------
void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator)
{
	itString filename;
	fsFileUtil::LocatorToUnicodeString( i_Locator, filename );

	D3DX11_IMAGE_LOAD_INFO loadInfo;
	loadInfo.MipLevels = 1;

	ID3D11Resource* pTexture = NULL;
	HRESULT hr = D3DX11CreateTextureFromFile(
		g2dDX11Global::g_pDevice,
		filename.GetString(),
		&loadInfo,
		NULL,
		&pTexture,
		NULL
	);
	if ( !SUCCEEDED(hr) )
	{
		g2dDX11Global::PrintDXError(hr);
		if (E_OUTOFMEMORY == hr)
			throw g2dOutOfVideoMemoryX();
		DBG_ASSERT(false, "Error loading texture using D3DXCreateTextureFromFile");
	}

	// Set the new surface
	io_Texture->SetSurface( pTexture );

	// set size info into the texture
	io_Texture->ReloadInfo();
}

//------------------------------------------------------------------------
//	We can't lock textures in D3DPOOL_DEFAULT, so we'll create a g2dImageDX11
//	and load the image data into it, then copy it to the texture.
//------------------------------------------------------------------------
void LoadPNG(matPlainTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce)
{
	load_plain_texture(io_Texture, i_Locator, i_TrueColor, true, i_WidthReduce, i_HeightReduce);
}
void ReloadPNG(matPlainTexture& io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce)
{
	LoadPNG(&io_Texture, i_Locator, i_TrueColor, i_WidthReduce, i_HeightReduce);
}

//------------------------------------------------------------------------
//	We can't generate mip levels for a D3DPOOL_DEFAULT texture, so we'll
//	make a temporary texture in D3DPOOL_SYSTEMMEM.
//------------------------------------------------------------------------
void LoadPNG(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	load_mip_texture(io_Texture, i_Locator, i_TrueColor, true, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
}
void ReloadPNG(matMipTexture& io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	load_mip_texture(&io_Texture, i_Locator, i_TrueColor, true, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
}

//------------------------------------------------------------------------
// Simply use D3DX to load the texture, ignoring the format request for now. 
// I needed a quick method to load TGA files.
// NOTE! WARNING! this code also is used to load .HDR format!
//------------------------------------------------------------------------
void LoadTGA(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	load_mip_texture(io_Texture, i_Locator, i_TrueColor, false, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void LoadBMP(matPlainTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce)
{
	load_plain_texture(io_Texture, i_Locator, i_TrueColor, false, i_WidthReduce, i_HeightReduce);
}
void ReloadBMP(matPlainTexture& io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce)
{
	LoadBMP(&io_Texture, i_Locator, i_TrueColor, i_WidthReduce, i_HeightReduce);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void LoadBMP(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	load_mip_texture(io_Texture, i_Locator, i_TrueColor, false, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
}
void ReloadBMP(matMipTexture& io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	load_mip_texture(&io_Texture, i_Locator, i_TrueColor, false, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
}

//------------------------------------------------------------------------
//	We can't lock textures in D3DPOOL_DEFAULT, so we'll create a g2dImageDX11
//	and load the image data into it, then copy it to the texture.
//------------------------------------------------------------------------
void LoadTIFF(matPlainTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce)
{
	// D3DX11CreateTextureFromFile is loading all kind of tiff file into A8R8G8B8 format,
	// this causes losing precision for some cases and we have to bring back old DX9 code
	//load_plain_texture(io_Texture, i_Locator, i_TrueColor, false, i_WidthReduce, i_HeightReduce);

	//	choose a texture image format
	g2dImageDX11 temp_image;
	HRESULT op_result;
	if ( i_TrueColor )
	{
		g2dSurfaceLoader::LoadTIFF(	&temp_image,
									i_Locator,
									matD3DGlobal::TrueAlphaTextureFormat(),
									matD3DGlobal::TrueNonAlphaTextureFormat());
	}
	else
	{
		g2dSurfaceLoader::LoadTIFF(	&temp_image,
									i_Locator,
									matD3DGlobal::AlphaTextureFormat(),
									matD3DGlobal::NonAlphaTextureFormat());
	}

	int dest_width = temp_image.GetWidth() >> i_WidthReduce;
	int dest_height = temp_image.GetHeight() >> i_HeightReduce;

	/*dest_width = maFunctions::Lowest(dest_width, int(g2dDX11Global::g_Caps.MaxTextureWidth));
	dest_height = maFunctions::Lowest(dest_height, int(g2dDX11Global::g_Caps.MaxTextureHeight));*/

	if ( NULL == io_Texture->GetSurface() )
	{
		io_Texture->Make(	dest_width,
							dest_height,
							temp_image.GetPixelFormat());
		io_Texture->ReloadInfo();
	}

	D3DX11_TEXTURE_LOAD_INFO load_info;
	op_result = D3DX11LoadTextureFromTexture(g2dDX11Global::g_pDeviceContext,
												temp_image.GetSurface(),
												&load_info,
												io_Texture->GetResource());
	//dest_level->Release();

	if ( op_result != S_OK )
	{
		g2dDX11Global::PrintDXError(op_result);
		DBG_ASSERT(op_result == S_OK, "Error creating temporary texture");
	}
}

void ReloadTIFF(matPlainTexture& io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce)
{
	LoadTIFF(&io_Texture, i_Locator, i_TrueColor, i_WidthReduce, i_HeightReduce);
}

//------------------------------------------------------------------------
//	We can't generate mip levels for a D3DPOOL_DEFAULT texture, so we'll
//	make a temporary texture in D3DPOOL_SYSTEMMEM.
//------------------------------------------------------------------------
void LoadTIFF(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	
	//load_mip_texture(io_Texture, i_Locator, i_TrueColor, false, i_WidthReduce, i_HeightReduce);

	g2dImageDX11 temp_image;
	HRESULT op_result;
	if ( i_TrueColor )
	{
		g2dSurfaceLoader::LoadTIFF(	&temp_image,
									i_Locator,
									matD3DGlobal::TrueAlphaTextureFormat(),
									matD3DGlobal::TrueNonAlphaTextureFormat());
	}
	else
	{
		g2dSurfaceLoader::LoadTIFF(	&temp_image,
									i_Locator,
									matD3DGlobal::AlphaTextureFormat(),
									matD3DGlobal::NonAlphaTextureFormat());
	}

	int dest_width = temp_image.GetWidth() >> i_WidthReduce;
	int dest_height = temp_image.GetHeight() >> i_HeightReduce;

	/*dest_width = maFunctions::Lowest(dest_width, int(g2dDX11Global::g_Caps.MaxTextureWidth));
	dest_height = maFunctions::Lowest(dest_height, int(g2dDX11Global::g_Caps.MaxTextureHeight));*/
	int mipLevels = (i_bIsMipMap) ? 0 : 1;

	if ( NULL == io_Texture->GetSurface() || mipLevels != io_Texture->GetMipLevels())
	{
		io_Texture->Make(	dest_width,
							dest_height,
							temp_image.GetPixelFormat(),
							mipLevels);
		io_Texture->ReloadInfo();
	}

	//	copy the g2dImageDX11 to the temp texture

	D3DX11_TEXTURE_LOAD_INFO load_info;
	load_info.NumMips = (i_bIsMipMap) ? 0 : 1;;
	op_result = D3DX11LoadTextureFromTexture(g2dDX11Global::g_pDeviceContext,
												temp_image.GetSurface(),
												&load_info,
												io_Texture->GetResource());
	//dest_level->Release();

	if ( op_result != S_OK )
	{
		g2dDX11Global::PrintDXError(op_result);
		DBG_ASSERT(op_result == S_OK, "Error creating temporary texture");
	}
}
void ReloadTIFF(matMipTexture& io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	LoadTIFF(&io_Texture, i_Locator, i_TrueColor, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
}

//------------------------------------------------------------------------
// Use D3DX function to load a resource.  The locator is assumed to be
//	the ID number in string form (e.g. "102")
//------------------------------------------------------------------------
void LoadResource(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce)
{
	itString file_name;
	fsFileUtil::LocatorToUnicodeString( i_Locator, file_name );
	DBG_LOG("loading resource " << i_Locator << " (" << file_name << ")");

	D3DX11_IMAGE_LOAD_INFO loadInfo = D3DX11_IMAGE_LOAD_INFO();
	loadInfo.FirstMipLevel = max(i_WidthReduce, i_HeightReduce);
	loadInfo.MipLevels = 1;

	HRESULT hr;
	ID3D11Resource* pTexture = NULL;

	hr = D3DX11CreateTextureFromResource(
	  g2dDX11Global::g_pDevice,
	  GetModuleHandle( NULL ),
	  MAKEINTRESOURCE(itStringUtil::GetInt(file_name)),
	  &loadInfo,
	  NULL,
	  &pTexture,
	  NULL
	);

	if ( !SUCCEEDED(hr) )
	{
		g2dDX11Global::PrintDXError(hr);
		if (E_OUTOFMEMORY == hr)
			throw g2dOutOfVideoMemoryX();
		//DBG_ASSERT(hr == D3D_OK, "Error loading texture using D3DXCreateTextureFromResource");
		io_Texture = NULL;
		return;
	}

	// Set the new surface
	io_Texture->SetSurface( pTexture );

	// set size info into the texture
	io_Texture->ReloadInfo();
}
void ReloadResource(matPlainTexture& io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce)
{
	LoadResource(&io_Texture, i_Locator, i_WidthReduce, i_HeightReduce);
}



}

