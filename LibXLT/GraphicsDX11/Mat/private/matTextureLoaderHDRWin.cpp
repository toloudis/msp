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

//#include <d3dx10math.h>
//#pragma comment(lib, "d3dx10.lib")
#include <DirectXPackedVector.h>

namespace 
{
	ID3D11Texture2D* load_hdr_data(const fsLocator& i_Locator)
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
		DirectX::PackedVector::HALF* XMConvertFloatToHalfStream(
			fHDR,
			4 * 2,
			fHDRPixels,
			4*4,
			filewidth*fileHeight
		);
		for( int i = 0; i < 4 * fileWidth * fileHeight; i += 4 )
		{
			fHDR[i] = fHDRPixels[i - j];
			fHDR[i + 1] = fHDRPixels[i + 1 - j];
			fHDR[i + 2] = fHDRPixels[i + 2 - j];
			fHDR[i + 3] = 1.0f;
			j++;
		}

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = fHDR;
		initData.SysMemPitch = fileWidth*4*sizeof(HALF);
		initData.SysMemSlicePitch = 0;

		// create a disposable staging texture to get the bits up into the real texture.
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = fileWidth;
		desc.Height = fileHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
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
	//	HRESULT op_result = g2dDX11Global::g_pDevice->CreateOffscreenPlainSurface( i_Width, i_Height, format, pool, &m_pSurface, NULL );
		if( !SUCCEEDED(op_result) )
		{
			g2dDX11Global::PrintDXError(op_result);
			DBG_ASSERT( SUCCEEDED(op_result), "Error allocating offscreen surface (w" << fileWidth << "-h" << fileHeight << "-f" << desc.Format << ")");
		}

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
		ID3D11Texture2D* pStagingTexture = load_hdr_data(i_Locator);

		D3D11_TEXTURE2D_DESC desc;
		pStagingTexture->GetDesc(&desc);

		desc.Width = desc.Width >> i_WidthReduce;
		desc.Height = desc.Height >> i_HeightReduce;
	//	desc.Width = maFunctions::Lowest(desc.Width, int(g2dDX11Global::g_Caps.MaxTextureWidth));
	//	desc.Height = maFunctions::Lowest(desc.Height, int(g2dDX11Global::g_Caps.MaxTextureHeight));
		desc.MipLevels = 1;
		desc.ArraySize = 1;
//		desc.Format = desc.Format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0; // no cpu access
		desc.MiscFlags = 0;

		g2dD3D11TexturePtr new_texture = NULL;
		HRESULT op_result = g2dDX11Global::g_pDevice->CreateTexture2D(&desc, NULL, &new_texture);
		if( !SUCCEEDED(op_result) )
		{
			g2dDX11Global::PrintDXError(op_result);
			if ( E_OUTOFMEMORY == op_result )
				throw g2dOutOfVideoMemoryX();

			DBG_ASSERT(SUCCEEDED(op_result), "Unhandled error creating texture");
		}

		D3DX11_TEXTURE_LOAD_INFO loadInfo;
		op_result = D3DX11LoadTextureFromTexture(g2dDX11Global::g_pDeviceContext, 
			pStagingTexture,
			&loadInfo,
			new_texture
		);

		// Set the new surface
		io_Texture->SetSurface( new_texture );

		// set size info into the texture
		io_Texture->ReloadInfo();

		// discard staging texture
		pStagingTexture->Release();
		pStagingTexture = NULL;
	}

	//------------------------------------------------------------------------
	// Load with mipmaps.
	//------------------------------------------------------------------------
	void LoadHDR(matMipTexture* io_Texture, const fsLocator& i_Locator, bool i_TrueColor, int i_WidthReduce, int i_HeightReduce, bool i_bIsMipMap)
	{
		ID3D11Texture2D* pStagingTexture = load_hdr_data(i_Locator);

		D3D11_TEXTURE2D_DESC desc;
		pStagingTexture->GetDesc(&desc);

		desc.Width = desc.Width >> i_WidthReduce;
		desc.Height = desc.Height >> i_HeightReduce;
	//	desc.Width = maFunctions::Lowest(desc.Width, int(g2dDX11Global::g_Caps.MaxTextureWidth));
	//	desc.Height = maFunctions::Lowest(desc.Height, int(g2dDX11Global::g_Caps.MaxTextureHeight));
		desc.MipLevels = i_bIsMipMap ? 0 : 1; // complete set of mip levels!
		desc.ArraySize = 1;
//		desc.Format = desc.Format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0; // no cpu access
		desc.MiscFlags = 0;

		g2dD3D11TexturePtr new_texture = NULL;
		HRESULT op_result = g2dDX11Global::g_pDevice->CreateTexture2D(&desc, NULL, &new_texture);
		if( !SUCCEEDED(op_result) )
		{
			g2dDX11Global::PrintDXError(op_result);
			if ( E_OUTOFMEMORY == op_result )
				throw g2dOutOfVideoMemoryX();

			DBG_ASSERT(SUCCEEDED(op_result), "Unhandled error creating texture");
		}

		D3DX11_TEXTURE_LOAD_INFO loadInfo;
		loadInfo.NumMips = i_bIsMipMap ? 0 : 1;
		op_result = D3DX11LoadTextureFromTexture(g2dDX11Global::g_pDeviceContext, 
			pStagingTexture,
			&loadInfo,
			new_texture
		);
		op_result = ::D3DX11FilterTexture(g2dDX11Global::g_pDeviceContext, new_texture, 0, D3DX11_DEFAULT);

		// Set the new surface
		io_Texture->SetSurface( new_texture );

		// set size info into the texture
		io_Texture->ReloadInfo();

		// discard staging texture
		pStagingTexture->Release();
		pStagingTexture = NULL;
	}

}// namespace matTextureLoaderHDR
