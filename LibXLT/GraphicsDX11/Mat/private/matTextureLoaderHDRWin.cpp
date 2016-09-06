/****************************************************************************\
**  matTextureLoaderHDRWin.hpp
**
**      matTextureLoaderHDRWin.hpp contains some helper functions for loading
**	.HDR textures in windows.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/mat/private/matTextureLoaderHDRWin.hpp"

//#include "GraphicsDX11/mat/private/rgbe.h"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itString.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/private/rgbe.h"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/mat/matMipTexture.hpp"
#include "GraphicsDX11/mat/matPlainTexture.hpp"
#include <DirectXPackedVector.h>
//#include <d3dx10math.h>
//#pragma comment(lib, "d3dx10.lib")

namespace 
{
    std::unique_ptr<DirectX::ScratchImage> load_hdr_data(const fsLocator& i_Locator)
	{
		itString filename;
		fsFileUtil::LocatorToUnicodeString(i_Locator, filename);

		// Read in the HDR light probe.
		FILE* fp = ::_wfopen( filename.GetString(), L"rb" );
		int fileWidth=0, fileHeight=0;
		rgbe_header_info info;
		RGBE_ReadHeader( fp, &fileWidth, &fileHeight, &info );

		// We really don't need this
		float fExposure = info.exposure;
		float fGamma  = info.gamma;

		// Create a float array to read in the RGB components
		float* fHDRPixels = new float[3 * fileWidth * fileHeight];
		memset( fHDRPixels, 0, 3 * fileWidth * fileHeight * sizeof( float ) );

		RGBE_ReadPixels_RLE( fp, fHDRPixels, fileWidth, fileHeight );

		::fclose(fp);

		// Convert the 32-bit floats into 16-bit floats and include the alpha component
		DirectX::PackedVector::HALF* fHDR = new DirectX::PackedVector::HALF[4 * fileWidth * fileHeight];
		int j = 0;
		for( int i = 0; i < 4 * fileWidth * fileHeight; i += 4 )
		{
			fHDR[i] = DirectX::PackedVector::XMConvertFloatToHalf(fHDRPixels[i - j]);
			fHDR[i + 1] = DirectX::PackedVector::XMConvertFloatToHalf(fHDRPixels[i + 1 - j]);
			fHDR[i + 2] = DirectX::PackedVector::XMConvertFloatToHalf(fHDRPixels[i + 2 - j]);
			fHDR[i + 3] = DirectX::PackedVector::XMConvertFloatToHalf(1.0f);
			j++;
		}

        std::unique_ptr<DirectX::ScratchImage> pStagingTexture(new DirectX::ScratchImage);
        HRESULT hr = pStagingTexture->Initialize2D(DXGI_FORMAT_R16G16B16A16_FLOAT, fileWidth, fileHeight, 1, 1);
        uint8_t* p = pStagingTexture->GetPixels();
        memcpy(p, fHDR, pStagingTexture->GetPixelsSize());


		delete[] fHDRPixels;
		delete[] fHDR;

		return pStagingTexture;
	}

}; // namespace


namespace matTextureLoaderHDR
{
	//------------------------------------------------------------------------
	// Load without mipmaps
	//------------------------------------------------------------------------
	void LoadPlainTexture(matPlainTexture* io_Texture, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce)
	{
        std::unique_ptr<DirectX::ScratchImage> scratchImage = load_hdr_data(i_Locator);

        std::unique_ptr<DirectX::ScratchImage> imageResult(new DirectX::ScratchImage);

        // resize to width/height reduce
        HRESULT hr = DirectX::Resize(scratchImage->GetImages(), scratchImage->GetImageCount(),
            scratchImage->GetMetadata(),
            scratchImage->GetMetadata().width >> i_WidthReduce, scratchImage->GetMetadata().height >> i_HeightReduce, 
            DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT,
            *imageResult);

        // generate mipmaps
        std::unique_ptr<DirectX::ScratchImage> mipChain(new DirectX::ScratchImage);
        HRESULT h = DirectX::GenerateMipMaps(*(imageResult->GetImage(0, 0, 0)), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_FANT,
            1,
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

	//------------------------------------------------------------------------
	// Load with mipmaps.
	//------------------------------------------------------------------------
	void LoadHDR(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
	{
        std::unique_ptr<DirectX::ScratchImage> scratchImage = load_hdr_data(i_Locator);

        std::unique_ptr<DirectX::ScratchImage> imageResult(new DirectX::ScratchImage);

        // resize to width/height reduce
        HRESULT hr = DirectX::Resize(scratchImage->GetImages(), scratchImage->GetImageCount(),
            scratchImage->GetMetadata(),
            scratchImage->GetMetadata().width >> i_WidthReduce, scratchImage->GetMetadata().height >> i_HeightReduce,
            DirectX::TEX_FILTER_FLAGS::TEX_FILTER_DEFAULT,
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

}// namespace matTextureLoaderHDR
