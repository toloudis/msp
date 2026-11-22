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
#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif

//============================================================================
//============================================================================
namespace matTextureLoaderWin
{

namespace
{

const DirectX::TEX_FILTER_FLAGS l_FilterType = DirectX::TEX_FILTER_FLAGS::TEX_FILTER_TRIANGLE;

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

enum FileType
{
    e_DDS,
    e_TGA,
    e_BMP,
    e_JPG,
    e_PNG,
    e_TIFF,
    e_GIF,
    e_WMP
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
FileType pick_format(const fsLocator& i_FileName)
{
    //	decide what kind of file we have
    std::string fname = i_FileName.GetLastName();
    std::string ext;
    envString::GetExtension(fname, ext);

    if ((ext == "bmp") || (ext == "BMP"))
    {
        return e_BMP;
    }
    else if ((ext == "jpg") || (ext == "JPG") || (ext == "JPEG") || (ext == "jpeg"))
    {
        return e_JPG;
    }
    else if ((ext == "tga") || (ext == "TGA"))
    {
        return e_TGA;
    }
    else if ((ext == "png") || (ext == "PNG"))
    {
        return e_PNG;
    }
    else if ((ext == "dds") || (ext == "DDS"))
    {
        return e_DDS;
    }
    else if ((ext == "tif") || (ext == "TIF") || (ext == "Tif"))
    {
        return e_TIFF;
    }
    else if ((ext == "tiff") || (ext == "TIFF") || (ext == "Tiff"))
    {
        return e_TIFF;
    }
    throw g2dUnknownImageFileTypeX();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT do_load_texture(ID3D11Resource*& io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
    std::string filename;
    fsFileUtil::LocatorToUnicodeString(i_Locator, filename);

    DirectX::TexMetadata info;
    std::unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage);
    HRESULT hr = E_FAIL;

    FileType file_type = pick_format(i_Locator);
    switch (file_type) {
    case e_DDS:
        hr = DirectX::LoadFromDDSFile(envString::UTF8ToWideChar(filename).c_str(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &info, *image);
        break;
    case e_TGA:
        hr = DirectX::LoadFromTGAFile(envString::UTF8ToWideChar(filename).c_str(), &info, *image);
        break;
    case e_BMP:
    case e_JPG:
    case e_PNG:
    case e_TIFF:
    case e_GIF:
    case e_WMP:
        hr = DirectX::LoadFromWICFile(envString::UTF8ToWideChar(filename).c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &info, *image);
        break;
    default:
        break;
    }

    std::unique_ptr<DirectX::ScratchImage> imageResult(new DirectX::ScratchImage);

    // resize to width/height reduce
    hr = DirectX::Resize(image->GetImages(), image->GetImageCount(),
        info,
        info.width >> i_WidthReduce, info.height >> i_HeightReduce, DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT,
        *imageResult);

    // generate mipmaps
    std::unique_ptr<DirectX::ScratchImage> mipChain(new DirectX::ScratchImage);
    HRESULT h = DirectX::GenerateMipMaps(*(imageResult->GetImage(0, 0, 0)), l_FilterType,
        i_bIsMipMap ? 0 : 1,
        *mipChain);

    // fill in d3d texture
    hr = DirectX::CreateTexture(g2dDX11Global::g_pDevice, mipChain->GetImages(), mipChain->GetImageCount(),
        mipChain->GetMetadata(), &io_Texture);
    return hr;
}
void load_mip_texture(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, bool i_PNG, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
{
	fsResourceTracker::MarkBegin(i_Locator);

    ID3D11Resource* pTexture = NULL;
    HRESULT hr = do_load_texture(pTexture, i_Locator, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
    if ( !SUCCEEDED(hr) )
	{
		fsResourceTracker::Remove(i_Locator);

		g2dDX11Global::PrintDXError(hr);

        std::string filename;
        fsFileUtil::LocatorToUnicodeString(i_Locator, filename);
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

    ID3D11Resource* pTexture = NULL;
    HRESULT hr = do_load_texture(pTexture, i_Locator, i_WidthReduce, i_HeightReduce, false);
    if (!SUCCEEDED(hr))
    {
        fsResourceTracker::Remove(i_Locator);

        g2dDX11Global::PrintDXError(hr);

        std::string filename;
        fsFileUtil::LocatorToUnicodeString(i_Locator, filename);
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


}	// end of namespace



//------------------------------------------------------------------------
// Use D3DX function to load images of many file types
//------------------------------------------------------------------------
void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator)
{
    ID3D11Resource* pTexture = NULL;
    HRESULT hr = do_load_texture(pTexture, i_Locator, 0, 0, false);

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
    load_plain_texture(io_Texture, i_Locator, i_TrueColor, false, i_WidthReduce, i_HeightReduce);
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
    load_mip_texture(io_Texture, i_Locator, i_TrueColor, false, i_WidthReduce, i_HeightReduce, i_bIsMipMap);
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
    // LOAD RESOURCE DATA INTO MEMORY PTR
    std::string file_name;
    fsFileUtil::LocatorToUnicodeString(i_Locator, file_name);
    DBG_LOG("loading resource " << i_Locator << " (" << file_name << ")");

    HRSRC myResource = ::FindResource(NULL, MAKEINTRESOURCE(envString::GetInt(file_name)), RT_RCDATA);
    unsigned int size = ::SizeofResource(NULL, myResource);
    HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
    void* pSource = ::LockResource(myResourceData);

    // THEN CALL DirectX::LoadFromXXXMemory
    // NEED TO KNOW TYPE FIRST... ASSUME WIC.
    DirectX::TexMetadata metadata;
    std::unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage);
    HRESULT hr = DirectX::LoadFromWICMemory(static_cast<const uint8_t*>(pSource), size, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &metadata, *image);
    // mip levels 1 only.
    metadata.mipLevels = 1;

    // fill in d3d texture
    ID3D11Resource* pTexture = NULL;
    hr = DirectX::CreateTexture(g2dDX11Global::g_pDevice, image->GetImages(), image->GetImageCount(),
        metadata, &pTexture);
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

