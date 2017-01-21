/****************************************************************************\
**  g2dImageDX11.cpp
**
**      g2dImageDX11.hpp is the D3D implementation of g2dImageDX11.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Core/dbg/dbgMsg.hpp"
#include "Core/ma/maFloatRGBA.hpp"
#include "Core/ma/maSampling.hpp"
#include "Core/ma/maVector3d.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dImageDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dDX11SurfaceUtil.hpp"
#include "GraphicsDX11/g2d/private/g2dImageDrawUtilDX11.hpp"

#include <DirectXTex/DirectXTex.h>

namespace
{
	inline DWORD make_mask(int i_FirstBit, int i_BitCount)
	{
		DWORD ret_val;

		if( (i_FirstBit + i_BitCount) == 32 )
			ret_val = 0xffffffff;
		else
			ret_val = (0x01 << (i_FirstBit + i_BitCount)) - 1;

		if( i_FirstBit != 0 )
		{
			DWORD lo_blank = ~((0x01 << i_FirstBit) - 1);
			ret_val &= lo_blank;
		}

		return ret_val;
	}


	//----------------------------------------------------------------------------
	//	convert from a pool enum to a D3DPOOL type
	//----------------------------------------------------------------------------
#if 0
	D3DPOOL convert_to_pool( g2dImage::CreateLoc i_CreateLoc )
	{
		D3DPOOL pool = D3DPOOL_DEFAULT;

		switch (i_CreateLoc)
		{
			case g2dImage::e_SystemMemory:
				pool = D3DPOOL_SYSTEMMEM;
				//DBG_LOG("Pool: System");
				break;
			case g2dImage::e_ScratchMemory:
				pool = D3DPOOL_SCRATCH;
				//DBG_LOG("Pool: Scratch");
				break;
			case g2dImage::e_ManagedMemory:
				pool = D3DPOOL_MANAGED;
				//DBG_LOG("Pool: Managed");
				break;
			default:
				//DBG_LOG("Pool: default");
				break;
		}

		return pool;
	}
#endif
}	// end of namespace

//--------------------------------------------------------------------
//	This constructor makes an "empty" surface
//--------------------------------------------------------------------
g2dImageDX11::g2dImageDX11()
:	m_pSurface(NULL),
	m_Stride(0),
	m_Width(0),
	m_Height(0)
{
}

//--------------------------------------------------------------------
//	This constructor makes a surface with the given dimensions
//	and pixel format.
//--------------------------------------------------------------------
g2dImageDX11::g2dImageDX11(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_CreateLoc )
:	m_pSurface(NULL),
	m_Stride(i_Width),
	m_PixelFormat(i_PFD),
	m_Width(i_Width),
	m_Height(i_Height)
{
	DXGI_FORMAT format = g2dDX11Global::D3DFormatFromPFD(i_PFD);

	//	configure the pool
//	D3DPOOL pool = convert_to_pool( i_CreateLoc );
//	DBG_ASSERT0( pool != D3DPOOL_MANAGED, "Cannot create a surface here that has a managed pool" );

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
	desc.BindFlags = 0;//D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;

	HRESULT op_result = g2dDX11Global::g_pDevice->CreateTexture2D(
	  &desc,
	  NULL,
	  &m_pSurface
	);

//	HRESULT op_result = g2dDX11Global::g_pDevice->CreateOffscreenPlainSurface( i_Width, i_Height, format, pool, &m_pSurface, NULL );

	if( ! SUCCEEDED(op_result) )
	{
		m_pSurface = NULL;
		g2dDX11Global::PrintDXError(op_result);
		if (op_result == E_OUTOFMEMORY)
		{
			throw g2dOutOfSystemMemoryX();
		}
		else //if ( op_result == DXGI_ERROR_INVALID_CALL )
		{
			throw g2dGeneralX();
		}
		DBG_ASSERT( SUCCEEDED(op_result), "Error allocating surface" );
	}
}
g2dImageDX11::g2dImageDX11(g2dD3D11TexturePtr i_surface)
{
	// surface WILL be released in destructor!!
	m_pSurface = i_surface;

	D3D11_TEXTURE2D_DESC desc;
	i_surface->GetDesc(&desc);
	m_Height = desc.Height;
	m_Width = desc.Width;
	m_Stride = m_Width;
	g2dDX11Global::PFDFromD3DFormat(desc.Format, m_PixelFormat);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
g2dImageDX11::~g2dImageDX11()
{
	if( m_pSurface )
	{
		// Release returns the reference count, not an error code.
		m_pSurface->Release();
	}
}

//--------------------------------------------------------------------
//	Make makes the surface into one with the given dimensions and
//	pixel format.  If the pixel format is not supported it will
//	throw a g2dUnsupportedPixelFormatX, and leave the old surface
//	intact.
//--------------------------------------------------------------------
void g2dImageDX11::Make(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_CreateLoc )
{
	if( m_pSurface )
	{
		HRESULT op_result = m_pSurface->Release();

		if( !SUCCEEDED(op_result) )
		{
			g2dDX11Global::PrintDXError(op_result);
			DBG_ASSERT( SUCCEEDED(op_result), "Error releasing surface" );
		}
	}

	DXGI_FORMAT format = g2dDX11Global::D3DFormatFromPFD(i_PFD);

	//	configure the pool
//	D3DPOOL pool = convert_to_pool( i_CreateLoc );
//	DBG_ASSERT0( pool != D3DPOOL_MANAGED, "Cannot create a surface here that has a managed pool" );

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

	//	create the surface
	HRESULT op_result = g2dDX11Global::g_pDevice->CreateTexture2D(
	  &desc,
	  NULL,
	  &m_pSurface
	);
//	HRESULT op_result = g2dDX11Global::g_pDevice->CreateOffscreenPlainSurface( i_Width, i_Height, format, pool, &m_pSurface, NULL );
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError(op_result);
		DBG_ASSERT( SUCCEEDED(op_result), "Error allocating offscreen surface (w" << i_Width << "-h" << i_Height << "-f" << format << "-p" << i_CreateLoc << ")");
	}

	// if we get here, the surface creation succeeded, and we can go
	// ahead and copy the values
	m_Width = i_Width;
	m_Height = i_Height;
	m_PixelFormat = i_PFD;
}


//--------------------------------------------------------------------
//	GetPixelFormat returns the current pixel format of the surface.
//--------------------------------------------------------------------
const g2dPFD& g2dImageDX11::GetPixelFormat() const
{
	return m_PixelFormat;
}

//--------------------------------------------------------------------
//	The width of the surface, in pixels (not bytes!).
//	Note that this is not the same as the stride see below.
//--------------------------------------------------------------------
int g2dImageDX11::GetWidth() const
{
	return m_Width;
}

//--------------------------------------------------------------------
//	The height of the surface.
//--------------------------------------------------------------------
int g2dImageDX11::GetHeight() const
{
	return m_Height;
}

//--------------------------------------------------------------------
//	Lock returns a pointer to the surface memory, allowing it to be
//	modified directly.  This should not be done often, as a surface
//	may be residing in video memory.  Call Release() when you are
//	done reading or modifying the surface.
//--------------------------------------------------------------------
void* g2dImageDX11::Lock()
{
	D3D11_MAPPED_SUBRESOURCE mappedTex2D;
	HRESULT op_result = g2dDX11Global::g_pDeviceContext->Map(m_pSurface,
		D3D11CalcSubresource(0,0,1),
		D3D11_MAP_READ_WRITE,
		0,
		&mappedTex2D
		);
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError(op_result);
		DBG_ASSERT(SUCCEEDED(op_result), "Error locking surface");
	}

	m_Stride = mappedTex2D.RowPitch;

	return mappedTex2D.pData;
}

//--------------------------------------------------------------------
//	Release should be called when you are finished with Lock() (or
//	future operations with the surface may fail).
//--------------------------------------------------------------------
void g2dImageDX11::Release()
{
	DBG_ASSERT(m_pSurface, "Surface not allocated");
	g2dDX11Global::g_pDeviceContext->Unmap(m_pSurface, D3D11CalcSubresource(0,0,1));
	m_Stride = 0;
}

//------------------------------------------------------------------------
//	This CopyImage makes a possibly resized copy of the original g2dImage
//------------------------------------------------------------------------
void g2dImageDX11::CopyImage( const g2dImage& i_SrcImage, 
	const maFilter* i_Filter /*= NULL*/, 
	float i_FilterWidth /*= 1*/, float i_FilterHeight /*= 1*/,
	int i_SampleRateX /*= 1*/, int i_SampleRateY /*= 1*/)
{
	const g2dImageDX11 *src_image = dynamic_cast<const g2dImageDX11*>(&i_SrcImage);

	if (i_Filter == NULL)
	{
        // assert same size and format?
        DBG_ASSERT((src_image->GetWidth() == this->GetWidth()) && (src_image->GetHeight() == this->GetHeight()), "CopyImage images must match in size");

        g2dD3D11TexturePtr src_surface = src_image->GetSurface();
		g2dD3D11TexturePtr dest_surface = this->GetSurface();

        g2dDX11Global::g_pDeviceContext->CopyResource(dest_surface, src_surface);

        HRESULT hr = S_OK;
		if (!SUCCEEDED(hr))
		{
			g2dDX11Global::PrintDXError(hr);
			if (hr == E_OUTOFMEMORY)
			{
				throw g2dOutOfSystemMemoryX();
			}
			else if ( hr == DXGI_ERROR_INVALID_CALL)
			{
				throw g2dGeneralX();
			}
			DBG_ASSERT( SUCCEEDED(hr), "Error copying surface" );
		}
	}
	else
	{
		int srcW = i_SrcImage.GetWidth();
		int srcH = i_SrcImage.GetHeight();
		DBG_ASSERT(srcW == this->GetWidth() * i_SampleRateX, "downsampling inconsistent input widths");
		DBG_ASSERT(srcH == this->GetHeight() * i_SampleRateY, "downsampling inconsistent input heights");

		// make sure there's at least 1 sample
		int filterSamplesX = max((int)ceil(i_FilterWidth * i_SampleRateX), 1);
		int filterSamplesY = max((int)ceil(i_FilterHeight * i_SampleRateY), 1);

		std::vector<maVector3d> samples;
		// returns samples positions centered on 0.5, spanning half the filterwidth in each direction
		maSampling::GetSamples2D(filterSamplesX, filterSamplesY, samples, 0, i_FilterWidth, i_FilterHeight);
		maSampling::WeightSamples(samples, i_Filter, i_FilterWidth, i_FilterHeight);

		maFloatRGBA sample;
		float r,g,b,a;
		// for each pixel in dest image, apply filter region over pixels in src image.
		SurfaceLocker dst_surf_locker(this->GetSurface(), true);
		SurfaceLocker src_surf_locker(src_image->GetSurface());
		if (src_surf_locker.IsLocked() && dst_surf_locker.IsLocked())
		{
			SurfaceIterator& src_surf_it = *(src_surf_locker.GetIterator());
			SurfaceIterator& dst_surf_it = *(dst_surf_locker.GetIterator());

			// loop over every pixel of dest image (this)
			for (int i = 0; i < this->GetHeight(); i++)
			{
				dst_surf_it.SetPosition(i, 0);
				for (int j = 0; j < this->GetWidth(); j++)
				{
					// init to 0
					maFloatRGBA finalPixel;

// filter is always centered over a destination pixel.

// 3 samples per pixel (ssx = 3)
// filter width = 3 pixels (radius=1.5):
// |***|***|***|***|***|
//     {-----=-----}
// note ctr of filter is right on a sample.

// 4 samples per pixel (ssx = 4)
// filter width = 3 pixels (radius=1.5):
// |****|****|****|****|****|
//      {------==------}
// note ctr of filter is between samples!

					// accumulate 2d filtered samples from src image:
					// center the filter over the pixel we want, based on sampling rate...
					// We will take filterSamplesX * filterSamplesY actual samples to compute the final pixel.

					// must account for odd and even numbers!!
					
					// each dst pixel spans sampleratex * sampleratey src samples.
					int filterCenterColumn = j*i_SampleRateX + (i_SampleRateX/2);
					int filterCenterRow = i*i_SampleRateY + (i_SampleRateY/2);

					// edges lack filter support!  positions will be out of bounds for some samples.
					int filterStartColumn = filterCenterColumn - (filterSamplesX/2) ;
					int filterStartRow = filterCenterRow - (filterSamplesY/2);

					for (int k = 0; k < filterSamplesY; k++)
					{
						for (int l = 0; l < filterSamplesX; l++)
						{
							src_surf_it.SetPosition(filterStartRow + k, 
								filterStartColumn + l);

							src_surf_it.GetValue(r,g,b,a);
							sample.Set(r,g,b,a);
							// weight
							sample *= samples[l + filterSamplesX*k].m_Z;
							// add to total
							finalPixel += sample;
						}
					}

					// now set the finalPixel into the dest image:
					dst_surf_it.SetValue(finalPixel.GetRed(), finalPixel.GetGreen(), finalPixel.GetBlue(), finalPixel.GetAlpha());
					
					dst_surf_it.IncCol();
				}
			}
		}
	}
}

//------------------------------------------------------------------------
//	This DrawImage blits the source image to this image.
//	The source image upper left corner will be located at (i_DX1, i_DY1)
//	on the destination image.
//------------------------------------------------------------------------
void g2dImageDX11::DrawImage(	int i_DX1,
								int i_DY1,
								const g2dImage& i_SrcImage)
{
	g2dD3D11TexturePtr dest_surface = this->GetSurface();
	DBG_ASSERT(dest_surface, "Image has no surface");

	const g2dImageDX11 *src_image = dynamic_cast<const g2dImageDX11*>(&i_SrcImage);
	DBG_ASSERT(src_image, "Image needs to be D3D Image");
	g2dD3D11TexturePtr source_surface = src_image->GetSurface();
	DBG_ASSERT(source_surface, "Image has no surface");

	g2dImageDrawUtilDX11::DrawImage(	dest_surface,
									i_DX1,
									i_DY1,
									this->GetWidth(),
									this->GetHeight(),
									0,
									0,
									i_SrcImage.GetWidth(),
									i_SrcImage.GetHeight(),
									source_surface);
}

//------------------------------------------------------------------------
//	This DrawImage blits a rectangular section of the source image to a
//	location on this image.  The rectangle of the source image
//	is defined by (i_SX1, i_SY1) - (i_SX2, i_SY2).  The point
//	(i_SX1, i_SY1) will be located at (i_DX1, i_DY1).
//------------------------------------------------------------------------
void g2dImageDX11::DrawImage(	int i_DX1,
								int i_DY1,
								int i_SX1,
								int i_SY1,
								int i_SX2,
								int i_SY2,
								const g2dImage& i_SrcImage)
{
	g2dD3D11TexturePtr dest_surface = this->GetSurface();
	DBG_ASSERT(dest_surface, "Image has no surface");

	const g2dImageDX11 *src_image = dynamic_cast<const g2dImageDX11*>(&i_SrcImage);
	DBG_ASSERT(src_image, "Image needs to be D3D Image");
	g2dD3D11TexturePtr source_surface = src_image->GetSurface();
	DBG_ASSERT(source_surface, "Image has no surface");

	g2dImageDrawUtilDX11::DrawImage(	dest_surface,
									i_DX1,
									i_DY1,
									this->GetWidth(),
									this->GetHeight(),
									i_SX1,
									i_SY1,
									i_SX2,
									i_SY2,
									source_surface);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int g2dImageDX11::GetStride() const
{
	return m_Stride;
}

//--------------------------------------------------------------------
//	GetSurface returns the DirectDraw surface pointer.
//--------------------------------------------------------------------
g2dD3D11TexturePtr g2dImageDX11::GetSurface() const
{
	return m_pSurface;
}

//----------------------------------------------------------------------------
// this call allocates a big block of mem!!!!!
// caller must delete [] o_pBuffer when done using it!!!!
//----------------------------------------------------------------------------
void g2dImageDX11::GetPixels(envType::UInt8** o_pBuffer,
							int* o_bufSize,
							int* o_width,
							int* o_height,
							int* o_bpp,
							int i_sampling /*= 1*/,
							bool i_bGreyscale /*= false*/,
							RECT* i_pRect /*= NULL*/)
{
	g2dDX11SurfaceUtil::GetSurfacePixels(m_pSurface, o_pBuffer, 
		o_bufSize, o_width, o_height, o_bpp, i_sampling, i_bGreyscale, i_pRect);
}
void g2dImageDX11::GetPixelsTGA(envType::UInt8 **o_pBuffer,
	int *o_bufSize,
	int *o_width,
	int *o_height,
	int *o_bpp,
	int	i_Sampling /*= 1*/,
	const std::string& i_CompressionCode /*= std::string("None")*/,
	const RECT* i_pRect /*= NULL*/)
{
	g2dDX11SurfaceUtil::GetSurfacePixelsTGA(m_pSurface, o_pBuffer, 
		o_bufSize, o_width, o_height, o_bpp, i_Sampling, i_CompressionCode, i_pRect);
}

//----------------------------------------------------------------------------
// this call allocates a big block of mem!!!!!
// caller must delete [] o_pBuffer when done using it!!!!
//----------------------------------------------------------------------------
void g2dImageDX11::GetPixels(g2dPixelR16G16B16A16** o_pBuffer,
							int* o_width,
							int* o_height)
{
	g2dDX11SurfaceUtil::GetSurfacePixels(m_pSurface, o_pBuffer, o_width, o_height);
}
//----------------------------------------------------------------------------
// this call allocates a big block of mem!!!!!
// caller must delete [] o_pBuffer when done using it!!!!
//----------------------------------------------------------------------------
void g2dImageDX11::GetPixels(g2dPixelR8G8B8A8** o_pBuffer,
							int* o_width,
							int* o_height)
{
	g2dDX11SurfaceUtil::GetSurfacePixels(m_pSurface, o_pBuffer, o_width, o_height);
}
//----------------------------------------------------------------------------
// this call allocates a big block of mem!!!!!
// caller must delete [] o_pBuffer when done using it!!!!
//----------------------------------------------------------------------------
void g2dImageDX11::GetPixels(g2dPixelRGBA32F** o_pBuffer,
	int* o_width,
	int* o_height)
{
	g2dDX11SurfaceUtil::GetSurfacePixels(m_pSurface, o_pBuffer, o_width, o_height);
}

// fill in buffer as provided
void g2dImageDX11::GetPixels(g2dPixelRGBA32F* o_pBuffer,
	int i_Width,
	int i_Height)
{
	DBG_ASSERT(i_Width <= m_Width, "GetPixels: bad image dimensions");
	DBG_ASSERT(i_Height <= m_Height, "GetPixels: bad image dimensions");
	g2dDX11SurfaceUtil::GetSurfacePixels(m_pSurface, o_pBuffer, i_Width, i_Height);
}

//----------------------------------------------------------------------------
// Get the color of the pixel at an individual position on the image
//----------------------------------------------------------------------------
void g2dImageDX11::GetPixelColor(int i_X,
								int i_Y,
								envType::UInt8 &o_Red,
								envType::UInt8 &o_Green,
								envType::UInt8 &o_Blue,
								envType::UInt8 &o_Alpha)
{
	g2dDX11SurfaceUtil::GetPixelColor(m_pSurface, i_X, i_Y, o_Red, o_Green, o_Blue, o_Alpha);
}

//----------------------------------------------------------------------------
// Get the color of the pixel at an individual position on the image
//----------------------------------------------------------------------------
void g2dImageDX11::GetPixelColor(int i_X,
								int i_Y,
								envType::Float32 &o_Red,
								envType::Float32 &o_Green,
								envType::Float32 &o_Blue,
								envType::Float32 &o_Alpha)
{
	g2dDX11SurfaceUtil::GetPixelColor(m_pSurface, i_X, i_Y, o_Red, o_Green, o_Blue, o_Alpha);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fill_bmp_header(BITMAPINFOHEADER &i_Header, int i_Width, int i_Height,
					  int i_DestBitsPerPixel, int i_DestImageSize)
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
void get_bitmap_info(	BITMAPINFO &i_Info,
						g2dD3D11TexturePtr i_pSurface,
						bool	i_bGreyscale /*= false*/,
						int		i_Sampling /*= 1*/,
						RECT*	i_pRect /*= NULL*/ )
{
	ZeroMemory(&i_Info,sizeof(i_Info));

	BITMAPINFOHEADER &bih = i_Info.bmiHeader;

	D3D11_TEXTURE2D_DESC SDesc;
	i_pSurface->GetDesc( &SDesc );

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
		//int PaletteSize = 256;
		int DestImageSize	= Width * Height;

		fill_bmp_header(bih, Width, Height,
					DestBitsPerPixel,DestImageSize);
	}
	else
	{
		//--------------------------------------------------------------//
		// 24-bit format
		//
		int DestBitsPerPixel = 24;
		//int PaletteSize = 0;
		int DestImageSize	= 3 * Width * Height;

		fill_bmp_header(bih, Width, Height,
					DestBitsPerPixel,DestImageSize);
	}
}

//--------------------------------------------------------------------------------
//	CaptureHBMP writes screen data into HBITMAP
//--------------------------------------------------------------------------------
HBITMAP g2dImageDX11::GetHBMP(const int i_Sampling )
{
	bool bGreyScale = false;
	RECT* pRect = NULL;

	HBITMAP hbm = NULL;

	// Start bitmap info struct
	BITMAPINFO bi;
	get_bitmap_info(bi, m_pSurface, bGreyScale, i_Sampling, pRect);

	int wid=0, ht=0, bpp=0, bufSize=0;
	envType::UInt8* pBuffer = NULL;
	g2dDX11SurfaceUtil::GetSurfacePixels(m_pSurface,
		&pBuffer, &bufSize, &wid, &ht, &bpp, 
		i_Sampling, bGreyScale, pRect);

	if (pBuffer)
	{
		void* bits = NULL;
		HDC hdc = ::CreateCompatibleDC(NULL);
		hbm = ::CreateDIBSection(hdc,( BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, NULL, NULL);
		if (hbm == 0)
		{
			DWORD error = ::GetLastError();
			
			DBG_ASSERT( hbm != NULL, "Failed to create Bitmap from surface (error code = " << error << " [" << ((error == ERROR_INVALID_PARAMETER) ? "invalid parameter":".") << "])" );
		}

		//l_FrameSize = BufferWidth / 1000000.0f;	// convert bytes to megabytes
		//DBG_LOG2( "    size(%d)  framesize(%6.3f)", bi.bmiHeader.biSize, l_FrameSize );

		::SetDIBits(hdc, hbm, 0, ht, pBuffer, &bi, DIB_RGB_COLORS);

		delete [] pBuffer;
		::DeleteDC(hdc);
	}
	return hbm;
}

//----------------------------------------------------------------------------
// SwapChannels() - assumes 4 bytes per pixel and non-floats
//----------------------------------------------------------------------------
void g2dImageDX11::SwapChannels( int i_ChannelOne, int i_ChannelTwo )
{
	int numBytesPerPixel = 4;

	// Access bits in pImg
	void* imgBits = this->Lock();
	BYTE* pImgBits = (BYTE*)imgBits;

	//Loop through all the pixels
	int i, j;
	for (i = 0 ; i < m_Height ; i++) 
	{
		for (j = 0 ; j < m_Width ; j++)
		{
			int idx = (j*numBytesPerPixel) + (i*this->GetStride());
			pImgBits[idx+i_ChannelOne] ^= pImgBits[idx+i_ChannelTwo];
			pImgBits[idx+i_ChannelTwo] ^= pImgBits[idx+i_ChannelOne];
			pImgBits[idx+i_ChannelOne] ^= pImgBits[idx+i_ChannelTwo];
		}
	}

	// Unlock surfaces
	this->Release();
}
