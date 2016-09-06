/****************************************************************************\
**  matTextureLoaderTGAWin.hpp
**
**      matTextureLoaderTGAWin.hpp contains some helper functions for loading
**	.TGA textures in windows.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/mat/private/matTextureLoaderTGAWin.hpp"

//#include "GraphicsDX11/mat/private/tga.h"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itString.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/private/tga.h"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/mat/matMipTexture.hpp"
#include "GraphicsDX11/mat/matPlainTexture.hpp"

namespace 
{
	ID3D11Texture2D* load_tga_data(const fsLocator& i_Locator)
	{
		itString filename;
		fsFileUtil::LocatorToUnicodeString(i_Locator, filename);

		// Read in the TGA light probe.
		FILE* fp = ::_wfopen( filename.GetString(), L"rb" );
		int fileWidth=0, fileHeight=0;
		envType::UInt32* fTGAPixels = NULL;

		if (TGA_Read(filename, fp, &fileWidth, &fileHeight, &fTGAPixels) == 0)
		{
			DBG_ERROR("Invalid TGA file. Can not read.");
			::fclose(fp);
			return NULL;
		}

		::fclose(fp);

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = fTGAPixels;
		initData.SysMemPitch = fileWidth * sizeof(envType::UInt32);
		initData.SysMemSlicePitch = 0;

		// create a disposable staging texture to get the bits up into the real texture.
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = fileWidth;
		desc.Height = fileHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		desc.MiscFlags = 0;

		//	create the surface
		ID3D11Texture2D* pStagingTexture = NULL;
		HRESULT op_result = g2dDX11Global::g_pDevice->CreateTexture2D(
		  &desc,
		  &initData,
		  &pStagingTexture
		);
		if( !SUCCEEDED(op_result) )
		{
			g2dDX11Global::PrintDXError(op_result);
			DBG_ASSERT( SUCCEEDED(op_result), "Error allocating offscreen surface (w" << fileWidth << "-h" << fileHeight << "-f" << desc.Format << ")");
		}

		delete[] fTGAPixels;

		return pStagingTexture;
	}

}; // namespace


namespace matTextureLoaderTGA
{
	//------------------------------------------------------------------------
	// Load without mipmaps
	//------------------------------------------------------------------------
	void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce)
	{
        itString filename;
        fsFileUtil::LocatorToUnicodeString(i_Locator, filename);

        DirectX::TexMetadata info;
        std::unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage);
        HRESULT hr = DirectX::LoadFromTGAFile(filename.GetString(), &info, *image);
        std::unique_ptr<DirectX::ScratchImage> imageResult(new DirectX::ScratchImage);
        hr = DirectX::Resize(image->GetImages(), image->GetImageCount(),
            info,
            info.width >> i_WidthReduce, info.height >> i_HeightReduce, DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT,
            *imageResult);

        info.mipLevels = 1;
//        g2dD3D11TexturePtr new_texture = NULL;
        ID3D11Resource* new_texture = NULL;
        HRESULT op_result = DirectX::CreateTexture(g2dDX11Global::g_pDevice, imageResult->GetImages(), imageResult->GetImageCount(), info, &new_texture);
		if( !SUCCEEDED(op_result) )
		{
			g2dDX11Global::PrintDXError(op_result);
			if ( E_OUTOFMEMORY == op_result )
				throw g2dOutOfVideoMemoryX();

			DBG_ASSERT(SUCCEEDED(op_result), "Unhandled error creating texture");
		}

		// Set the new surface
		io_Texture->SetSurface( new_texture );

		// set size info into the texture
		io_Texture->ReloadInfo();

	}

	//------------------------------------------------------------------------
	// Load with mipmaps.
	//------------------------------------------------------------------------
	void LoadTGA(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
	{
        itString filename;
        fsFileUtil::LocatorToUnicodeString(i_Locator, filename);

        DirectX::TexMetadata info;
        std::unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage);
        
        // load from file
        HRESULT hr = DirectX::LoadFromTGAFile(filename.GetString(), &info, *image);

        std::unique_ptr<DirectX::ScratchImage> imageResult(new DirectX::ScratchImage);
        
        // resize to width/height reduce
        hr = DirectX::Resize(image->GetImages(), image->GetImageCount(),
            info,
            info.width >> i_WidthReduce, info.height >> i_HeightReduce, DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT,
            *imageResult);

        // generate mipmaps
        std::unique_ptr<DirectX::ScratchImage> mipChain(new DirectX::ScratchImage);
        HRESULT h = DirectX::GenerateMipMaps(*(imageResult->GetImage(0, 0, 0)), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_FANT,
            i_bIsMipMap ? 0 : 1,
            *mipChain);

        // fill in d3d texture
        ID3D11Resource* new_texture = NULL;
        HRESULT op_result = DirectX::CreateTexture(g2dDX11Global::g_pDevice, mipChain->GetImages(), mipChain->GetImageCount(), mipChain->GetMetadata(), &new_texture);
        if (!SUCCEEDED(op_result))
        {
            g2dDX11Global::PrintDXError(op_result);
            if (E_OUTOFMEMORY == op_result)
                throw g2dOutOfVideoMemoryX();

            DBG_ASSERT(SUCCEEDED(op_result), "Unhandled error creating texture");
        }

        // Set the new surface
        io_Texture->SetSurface(new_texture);

        // set size info into the texture
        io_Texture->ReloadInfo();
	}

}// namespace matTextureLoaderTGA
