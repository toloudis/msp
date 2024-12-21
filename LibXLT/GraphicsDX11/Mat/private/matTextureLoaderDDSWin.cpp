/****************************************************************************\
**  matTextureLoaderDDSWin.hpp
**
**      matTextureLoaderDDSWin.hpp contains some helper functions for loading
**	textures in windows.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/mat/private/matTextureLoaderDDSWin.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/mat/matExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/mat/matDX11GlobalWin.hpp"

#include "DDS.h"

#include <DirectXTex.h>

#include <fstream>


//============================================================================
//============================================================================
namespace
{
//	const DWORD lc_dwCubeMapAllFaces = DDSCAPS2_CUBEMAP |
//									   DDSCAPS2_CUBEMAP_POSITIVEX |
//									   DDSCAPS2_CUBEMAP_NEGATIVEX |
//									   DDSCAPS2_CUBEMAP_POSITIVEY |
//									   DDSCAPS2_CUBEMAP_NEGATIVEY |
//									   DDSCAPS2_CUBEMAP_POSITIVEZ |
//									   DDSCAPS2_CUBEMAP_NEGATIVEZ;

//	const DWORD lc_CubeMapPositiveX = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX;
//	const DWORD lc_CubeMapPositiveY = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY;
//	const DWORD lc_CubeMapPositiveZ = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ;
//	const DWORD lc_CubeMapNegativeX = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX;
//	const DWORD lc_CubeMapNegativeY = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY;
//	const DWORD lc_CubeMapNegativeZ = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ;

//	const DWORD lc_RGBA = DDPF_RGB | DDPF_ALPHAPIXELS;

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif
	//------------------------------------------------------------------------
	// LoadTextureDataFromFile load header from files
	//------------------------------------------------------------------------
	HRESULT LoadTextureDataFromFile( __in_z WCHAR* szFileName, BYTE** ppHeapData,
		DDS_HEADER** ppHeader,
		BYTE** ppBitData, UINT* pBitSize )
	{
		// open the file
		HANDLE hFile = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL );
		if( INVALID_HANDLE_VALUE == hFile )
			return HRESULT_FROM_WIN32( GetLastError() );

		// Get the file size
		LARGE_INTEGER FileSize;
		GetFileSizeEx( hFile, &FileSize );

		// File is too big for 32-bit allocation, so reject read
		if( FileSize.HighPart > 0 )
			return E_FAIL;

		// Need at least enough data to fill the header and magic number to be a valid DDS
		if( FileSize.LowPart < (sizeof(DDS_HEADER)+sizeof(DWORD)) )
			return E_FAIL;

		// create enough space for the file data
		*ppHeapData = new BYTE[ FileSize.LowPart ];
		if( !( *ppHeapData ) )
			return E_OUTOFMEMORY;

		// read the data in
		DWORD BytesRead = 0;
		if( !ReadFile( hFile, *ppHeapData, FileSize.LowPart, &BytesRead, NULL ) )
			return HRESULT_FROM_WIN32( GetLastError() );

		if( BytesRead < FileSize.LowPart )
			return E_FAIL;

		// DDS files always start with the same magic number ("DDS ")
		DWORD dwMagicNumber = *( DWORD* )( *ppHeapData );
		if( dwMagicNumber != DDS_MAGIC )
			return E_FAIL;

		DDS_HEADER* pHeader = reinterpret_cast<DDS_HEADER*>( *ppHeapData + sizeof( DWORD ) );

		// Verify header to validate DDS file
		if( pHeader->dwSize != sizeof(DDS_HEADER)
			|| pHeader->ddspf.dwSize != sizeof(DDS_PIXELFORMAT) )
			return E_FAIL;

		// Check for DX10 extension
		bool bDXT10Header = false;
		if ( (pHeader->ddspf.dwFlags & DDS_FOURCC)
			&& (MAKEFOURCC( 'D', 'X', '1', '0' ) == pHeader->ddspf.dwFourCC) )
		{
			// Must be long enough for both headers and magic value
			if( FileSize.LowPart < (sizeof(DDS_HEADER)+sizeof(DWORD)+sizeof(DDS_HEADER_DXT10)) )
				return E_FAIL;

			bDXT10Header = true;
		}

		// setup the pointers in the process request
		*ppHeader = pHeader;
		INT offset = sizeof( DWORD ) + sizeof( DDS_HEADER )
			+ (bDXT10Header ? sizeof( DDS_HEADER_DXT10 ) : 0);
		*ppBitData = *ppHeapData + offset;
		*pBitSize = FileSize.LowPart - offset;

		CloseHandle( hFile );

		return S_OK;
	}
	
#define	D3DFMT_CxV8U8 117
	HRESULT CreateTextureFromDDS( ID3D11Device* pDev, DDS_HEADER* pHeader, __inout_bcount(BitSize) BYTE* pBitData,
                                     UINT BitSize, ID3D11Texture2D** ppSRV )
	{
		HRESULT hr = S_OK;

		UINT iWidth = pHeader->dwWidth;
		UINT iHeight = pHeader->dwHeight;
		UINT iMipCount = pHeader->dwMipMapCount;
		if( 0 == iMipCount )
			iMipCount = 1;

		// Bound miplevels (affects the memory usage below)
		if ( iMipCount > D3D11_REQ_MIP_LEVELS )
			return HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );

		D3D11_TEXTURE2D_DESC desc;
		// Only handle CxU8V8 dds format reading
		if (!(pHeader->ddspf.dwFlags & DDS_FOURCC) ||
			!(pHeader->ddspf.dwFourCC == D3DFMT_CxV8U8))
		{
			return E_FAIL;
		}

		// Create a DXGI_FORMAT_R8G8B8A8_UNORM texture to replace CxU8V8
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Width = iWidth;
		desc.Height = iHeight;
		desc.MipLevels = iMipCount;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		
		D3D11_SUBRESOURCE_DATA* pInitData = new D3D11_SUBRESOURCE_DATA[iMipCount * desc.ArraySize];
		if( !pInitData )
			return E_OUTOFMEMORY;

		UINT NumBytes = 0;
		UINT sNumBytes = 0;
		UINT RowBytes = 0;
		UINT NumRows = 0;
		UINT mipMapSize = 0;
		// Create and fill in texture buffer
		{
			// calcuate mipmap array size.
			// make sure texture size at least has size 1x1
			UINT w = iWidth;
			UINT h = iHeight;
			
			for (UINT i = 0; i < iMipCount; i++)
			{
				UINT size = w * h * 4;
				mipMapSize += size;
				w = w >> 1;
				h = h >> 1;
				if( w == 0 )
					w = 1;
				if( h == 0 )
					h = 1;
			}
		}

		BYTE* pSrcBits = new BYTE[mipMapSize];
		memset(pSrcBits, 0, sizeof(BYTE) * mipMapSize);
		if (!pSrcBits)
			return E_OUTOFMEMORY;

		UINT index = 0;
		UINT bpp = 32;	// bpp is 32 for DXGI_FORMAT_R8G8B8A8_UNORM format
		UINT sbpp = 16;
		BYTE* pDstBitsPointer = (BYTE*)pSrcBits;
		char* pSrcBitsPointer = (char*)pBitData;
		for( UINT j = 0; j < desc.ArraySize; j++ )
		{
			UINT w = iWidth;
			UINT h = iHeight;
			for( UINT i = 0; i < iMipCount; i++ )
			{
				for (UINT y = 0; y < h; y++)
				{
					for (UINT x = 0; x < w; x++)
					{
						float r,g,b;
						char rc = (pSrcBitsPointer[y*w*2 + x * 2 + 0]);
						char gc = (pSrcBitsPointer[y*w*2 + x * 2 + 1]);
						r = rc/127.0f * 2.0f - 1.0f;
						g = gc/127.0f * 2.0f - 1.0f;
						b = sqrt(max(0, 1.0f - (r)*(r) - (g)*(g)));

						pDstBitsPointer[y*w*4 + x*4 + 0] = (BYTE)( (r + 1.0f ) * 0.5f * 255);
						pDstBitsPointer[y*w*4 + x*4 + 1] = (BYTE)( (g + 1.0f ) * 0.5f * 255);
						pDstBitsPointer[y*w*4 + x*4 + 2] = (BYTE)( (b + 1.0f ) * 0.5f * 255);
						pDstBitsPointer[y*w*4 + x*4 + 3] = (BYTE)( 255);
					}
				}
				//GetSurfaceInfo( w, h, desc.Format, &NumBytes, &RowBytes, &NumRows );
				RowBytes = ( w * bpp + 7 ) / 8; // round up to nearest byte
				NumRows = h;
				NumBytes = RowBytes * NumRows;
				sNumBytes = ( w * sbpp + 7 ) / 8 * h;
				pInitData[index].pSysMem = ( void* )pDstBitsPointer;
				pInitData[index].SysMemPitch = RowBytes;
				++index;

				pSrcBitsPointer += sNumBytes;
				pDstBitsPointer += NumBytes;
				w = w >> 1;
				h = h >> 1;
				if( w == 0 )
					w = 1;
				if( h == 0 )
					h = 1;
			}
		}

		*ppSRV = NULL;
		hr = pDev->CreateTexture2D( &desc, pInitData, ppSRV );
		/*if( SUCCEEDED( hr ) && pTex2D )
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
			SRVDesc.Format = desc.Format;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = desc.MipLevels;
			hr = pDev->CreateShaderResourceView( pTex2D, &SRVDesc, ppSRV );
			SAFE_RELEASE( pTex2D );
		}*/

		//::D3DX11SaveTextureToFile(g2dDX11Global::g_pDeviceContext, *ppSRV, D3DX11_IFF_DDS, L"C:\\Projects\\testNormal.dds");
		
		SAFE_DELETE_ARRAY( pSrcBits );
		SAFE_DELETE_ARRAY( pInitData );

		return hr;
	}

	HRESULT load_CxU8V8_format(const itString& i_srcFile, ID3D11Texture2D** o_tex2D )
	{
		BYTE* pHeapData = NULL;
		DDS_HEADER* pHeader= NULL;
		BYTE* pBitData = NULL;
		UINT BitSize = 0;

		HRESULT hr = LoadTextureDataFromFile( (WCHAR *)i_srcFile.GetString(), &pHeapData, &pHeader, &pBitData, &BitSize );
		if(FAILED(hr))
		{
			SAFE_DELETE_ARRAY( pHeapData );
			return hr;
		}

		hr = CreateTextureFromDDS( g2dDX11Global::g_pDevice, pHeader, pBitData, BitSize, o_tex2D );
		SAFE_DELETE_ARRAY( pHeapData );

		return hr;
	}

	//------------------------------------------------------------------------
	// load_dds_data from the file.
	//------------------------------------------------------------------------
	struct ID3D11Resource* load_dds_data( struct matTextureLoaderDDS::DDSInfo& io_Info,
												 bool mipmap_if_2D )
	{
		fsResourceTracker::MarkBegin(io_Info.s_Locator);

		itString filename;
		fsFileUtil::LocatorToUnicodeString( io_Info.s_Locator, filename );
		//filename = "\\projects\\MachStudio\\" + filename;	// FIX: - hardcoded path

		HRESULT hr;

		//DBG_LOG1( "loading DDS texture (%s)", filename.c_str() );
        DirectX::TexMetadata srcInfo;
        hr = DirectX::GetMetadataFromDDSFile(filename.GetString(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE,
            srcInfo);

		ID3D11Texture2D* pTex2D = NULL;

		HRESULT hrLoadCxU8V8 = load_CxU8V8_format(filename, &pTex2D);
		
		if (!SUCCEEDED(hr) && !SUCCEEDED(hrLoadCxU8V8))
		{
			DBG_WARNING("A non-DDS file made it into load_dds_data: " << filename);
			fsResourceTracker::Remove(io_Info.s_Locator);
			return NULL;
		}

		bool cm = false;
		bool vol = false;
        cm = srcInfo.IsCubemap();
        vol = srcInfo.IsVolumemap();
		//if ((srcInfo.arraySize == 6) && 
		//	(srcInfo.dimension == DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D) &&
		//	(srcInfo.miscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE))
		//	cm = true;
		//else if (srcInfo.dimension == DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE3D)
		//	vol = true;

		ID3D11Resource* pTexture = NULL;

		if (pTex2D)
		{
			pTexture = pTex2D;
		}
		else
		{
            //TODO: OBEY MIPMAP HINTS mipmap_if_2D and io_info.s_nWidthReduce 
            //DirectX::TexMetadata imageLoadInfo;
			//imageLoadInfo.mipLevels = (mipmap_if_2D && !cm && !vol) ? 0 : 1; // 1 = top mip level only
			//if (io_Info.s_nWidthReduce > 0)
			//	imageLoadInfo.FirstMipLevel = io_Info.s_nWidthReduce;

            DirectX::TexMetadata info;
            std::unique_ptr<DirectX::ScratchImage> image(new DirectX::ScratchImage);
            HRESULT hr = DirectX::LoadFromDDSFile(filename.GetString(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &info, *image);
            hr = DirectX::CreateTexture(g2dDX11Global::g_pDevice, image->GetImages(), image->GetImageCount(), info, &pTexture);

			//	check for errors loading
			if ( !SUCCEEDED(hr) )
			{
				g2dDX11Global::PrintDXError(hr);
				fsResourceTracker::Remove(io_Info.s_Locator);

				std::string txfile;
				fsFileUtil::LocatorToANSIFilename(io_Info.s_Locator, txfile);
				if (cm)
					DBG_ERROR("error creating compressed cubemap texture: " << txfile);
				else if (vol)
					DBG_ERROR("error creating compressed volume texture: " << txfile);
				else
					DBG_ERROR("error creating compressed texture: " << txfile);
				if (pTexture != NULL)
				{
					pTexture->Release();
					pTexture = NULL;
				}
				//DBG_ASSERT(hr == D3D_OK, "error creating compressed texture (%s)", txfile.c_str());
				if (hr == E_OUTOFMEMORY)
				{
					throw g2dOutOfVideoMemoryX();
				}
			}
		}

		if (vol)
			io_Info.s_eType = matTextureLoaderDDS::e_VolumeTexture;
		else if (cm)
			io_Info.s_eType = matTextureLoaderDDS::e_CubeTexture;
		else 
			io_Info.s_eType = matTextureLoaderDDS::e_MipTexture;

		if (pTexture != NULL)
			fsResourceTracker::MarkEnd(io_Info.s_Locator);
		return pTexture;
	}

}

namespace matTextureLoaderDDS
{

//------------------------------------------------------------------------
// Loads a compressed .DDS file that will be decompressed by hardware
// if the device supports it, otherwise in memory like any other graphic.
//------------------------------------------------------------------------
struct ID3D11Resource* LoadDDS( struct DDSInfo& o_Info, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce,
	bool mipmap_if_2D /*=true*/)
{
	//	fill-in part of the info structure
	//
	o_Info.s_Locator		= i_Locator;
	o_Info.s_nWidthReduce	= i_WidthReduce;
	o_Info.s_nHeightReduce	= i_HeightReduce;

	//if ( i_WidthReduce > 0 || i_HeightReduce > 0 )
	//{
	//	DBG_WARNING0("Compressed textures reduction ignored.  This is not currently supported.  We'd have to load it into memory and handle it there, thus defeating the purpose of compressing it in the first place!  Use a non-DDS file format.");

	//	o_Info.s_nWidthReduce  = 0;
	//	o_Info.s_nHeightReduce = 0;
	//}

	return load_dds_data( o_Info, mipmap_if_2D );
}

}

