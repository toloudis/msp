/****************************************************************************\
**  g2dSurfaceLoaderDX11.hpp
**
**  g2dSurfaceLoaderDX11.hpp contains some helper functions for loading
**	textures in windows.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g2d/private/g2dSurfaceLoaderDX11.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dImageDX11.hpp"

#include "png/lpng162/png.h"


//==============================================================================
//	library pragmas
//==============================================================================
//#pragma comment(lib,"libtiff.lib")
//#pragma comment(lib,"libpng.lib")
//#pragma comment(lib,"zdll.lib")
#ifdef WIN64
//#pragma comment(lib,"jpeg.lib")
#endif


//==============================================================================
//==============================================================================
namespace g2dSurfaceLoader
{
namespace
{
//------------------------------------------------------------------------
//	PNG helpers
//	these are some callbacks used by the png code
//----------------------------------------------------------------------------
void read_data_for_png(png_structp png_ptr, png_bytep data, unsigned int length)
{
	//	The png struct stores a user-defined pointer, which in our case
	//	is just the address of our gfFileBin
	png_voidp read_io_ptr = png_get_io_ptr(png_ptr);
	gfFileBin* file_stream = reinterpret_cast<gfFileBin*>(read_io_ptr);
	file_stream->Read(length, data);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void error_fn_for_png(png_structp png_ptr, png_const_charp error_msg)
{
	DBG_ASSERT(false, "pnglib error: " << std::string(error_msg));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void warning_fn_for_png(png_structp png_ptr, png_const_charp warning_msg)
{
	DBG_LOG("pnglib warning: " << warning_msg);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void load_png_data(	const fsLocator& i_Locator,
					std::vector<envType::UInt8*>& o_Rows,
					bool& o_HasAlpha,
					int& o_Width,
					int& o_Height)
{

	// create png stuff
	//
	png_structp png_ptr = NULL;

	png_ptr = png_create_read_struct(	PNG_LIBPNG_VER_STRING,
										NULL,
										error_fn_for_png,
										warning_fn_for_png);
	if( png_ptr == NULL )
	{
		DBG_ASSERT(png_ptr, "Couldn't allocate png_struct");
		throw g2dGeneralX();
	}

	png_infop info_ptr = NULL;
	info_ptr = png_create_info_struct(png_ptr);

	if( info_ptr == NULL )
	{
		DBG_ASSERT(info_ptr, "Couldn't allocate info struct");
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		throw g2dGeneralX();
	}

	//	go ahead and make the file stream we'll use to read the data
	//
	gfFileBin read_stream(i_Locator, fsFileStream::e_ReadOnly);

	//	Instead of calling png_init_io call png_set_read_fn
	//	This allows us to override the read function so we can read
	//	either straight from a png file or from a pak file
	//
	png_set_read_fn(png_ptr, &read_stream, (png_rw_ptr)read_data_for_png);

	png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */

	png_uint_32 width;
	png_uint_32	height;
	int bit_depth;
	int color_type;
	int interlace_type;
	int compression_type;
	int filter_type;

	//	Get data about the png file
	//
	png_get_IHDR(	png_ptr,
					info_ptr,
					&width,
					&height,
					&bit_depth,
					&color_type,
					&interlace_type,
					&compression_type,
					&filter_type);

	o_Width = width;
	o_Height = height;

	//	set some filters
	//

	//	expand palette images (no palettes in Terawatt!  Hooray!)
	if (	(color_type == PNG_COLOR_TYPE_PALETTE) &&
			(bit_depth <= 8) )
		png_set_expand(png_ptr);

	//	expand grayscale images
    if (	(color_type == PNG_COLOR_TYPE_GRAY) &&
			(bit_depth < 8) )
		png_set_expand(png_ptr);

	// reduce to 8-bit/channel if necessary
	if ( bit_depth == 16 )
        png_set_strip_16(png_ptr);

	//	does it have an alpha channel?
	if ( color_type & PNG_COLOR_MASK_ALPHA )
		o_HasAlpha = true;
	else
		o_HasAlpha = false;

	//	now we should have a format of 8 bit/channel RGB or RGBA
	//	call the update function to update the png info
	png_read_update_info(png_ptr, info_ptr);

	//	we should be able to allocate our row pointers now
	o_Rows.resize(height);
	int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	int i;
	for( i = 0 ; i < height ; i++ )
		o_Rows[i] = new envType::UInt8[row_bytes];

	//	The actual image read
	//
	png_read_image(png_ptr, &(o_Rows[0]));

	//	clean up
	//
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void LoadPNG(g2dImageDX11* io_Image, 
			 const fsLocator& i_Locator, 
			 const g2dPFD& i_AlphaPFD, 
			 const g2dPFD& i_NonAlphaPFD, 
			 g2dImage::CreateLoc i_CreateLoc )
{
	fsResourceTracker::MarkBegin(i_Locator);

	std::vector<envType::UInt8*> rows;
	int width;
	bool alpha;

	LoadPNGData(i_Locator, rows, width, alpha);

	const g2dPFD* image_pfd;
	if( alpha )
		image_pfd = &i_AlphaPFD;
	else
		image_pfd = &i_NonAlphaPFD;

	io_Image->Make( width, rows.size(), *image_pfd, i_CreateLoc );

	void* image_data = io_Image->Lock();
	int stride = io_Image->GetStride();

	CopyImageData(	image_data,
					stride,
					width,
					*image_pfd,
					rows,
					alpha);

	io_Image->Release();

	int i;
	int num_rows = rows.size();
	for( i = 0 ; i < num_rows ; i++ )
		delete [] rows[i];

	fsResourceTracker::MarkEnd(i_Locator);
}

void LoadPNG(g2dImageDX11* io_Image, 
			 const fsLocator& i_Locator, 
			 const g2dPFD& i_PFD, 
			 g2dImage::CreateLoc i_CreateLoc)
{
	LoadPNG(io_Image, i_Locator, i_PFD, i_PFD, i_CreateLoc );
}

void LoadPNG(g2dImageDX11* io_Image, 
			 const fsLocator& i_Locator, 
			 g2dImage::CreateLoc i_CreateLoc)
{
	LoadPNG(io_Image, i_Locator, g2dDX11Global::g_ScreenPixelFormat, g2dDX11Global::g_ScreenPixelFormat, i_CreateLoc );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void LoadBMP(g2dImageDX11* io_Image, 
			 const fsLocator& i_Locator, 
			 const g2dPFD& i_PFD, 
			 g2dImage::CreateLoc i_CreateLoc)
{
	fsResourceTracker::MarkBegin(i_Locator);

	std::vector<envType::UInt8*> rows;
	int width;

	bool bHasAlpha;
	LoadBMPData(i_Locator, rows, width, bHasAlpha);

	io_Image->Make(width, rows.size(), i_PFD, i_CreateLoc );
	void* image_data = io_Image->Lock();
	int stride = io_Image->GetStride();

	CopyImageData(	image_data,
					stride,
					width,
					i_PFD,
					rows,
					bHasAlpha);

	io_Image->Release();

	int i;
	int num_rows = rows.size();
	for( i = 0 ; i < num_rows ; i++ )
		delete [] rows[i];

	fsResourceTracker::MarkEnd(i_Locator);
}

void LoadBMP(g2dImageDX11* io_Image, 
			 const fsLocator& i_Locator, 
			 g2dImage::CreateLoc i_CreateLoc)
{
	LoadBMP(io_Image, i_Locator, g2dDX11Global::g_ScreenPixelFormat, i_CreateLoc );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void LoadTIFF(g2dImageDX11* io_Image, 
			 const fsLocator& i_Locator, 
			 const g2dPFD& i_AlphaPFD, 
			 const g2dPFD& i_NonAlphaPFD, 
			 g2dImage::CreateLoc i_CreateLoc )
{
	fsResourceTracker::MarkBegin(i_Locator);

	std::vector<envType::UInt8*> rows;
	std::vector<envType::UInt16*> rows16;
	std::vector<envType::Float32*> rows_fp;
	int width;
	bool alpha;

    std::string imagefile;
	fsFileUtil::LocatorToANSIFilename(i_Locator, imagefile);

	//read in the image file
	TIFF *image = TIFFOpen(imagefile.c_str(), "r");
	
	if(image)
	{
		uint16 format;
		uint16 samples;
		uint16 pixel_bits;

	
		bool fp_tiff = false;
		//check if the image format is floating point
		TIFFGetField(image, TIFFTAG_SAMPLEFORMAT, &format);
		TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &samples);
		TIFFGetField(image, TIFFTAG_BITSPERSAMPLE, &pixel_bits);
		if(format == SAMPLEFORMAT_IEEEFP)
			fp_tiff = true;
		
		//if this is a floating point tiff, we need to use the 32bit float vector
		if(fp_tiff)
		{
			LoadTIFFData(i_Locator, rows_fp, width, alpha, image);
		}
		else
		{
			if (pixel_bits == 16)
			{
				// Special support for 16 bits tiff images
				LoadTIFFData(i_Locator, rows16, width, alpha, image);
			}
			else
			{
				// otherwise always assume it's 8 bit tiff images
				LoadTIFFData(i_Locator, rows, width, alpha, image);
			}
		}

		TIFFClose(image);	
		
		//if this is a floating point tiff, we need to do the calculations differently
		if(fp_tiff)
		{

			g2dPFD fp_pfd;
			if( alpha )
				fp_pfd = i_AlphaPFD;
			else
				fp_pfd = i_NonAlphaPFD;	
			
			//force RGBA 32 bit floats
			fp_pfd.SetBitsPerPixel(128);
			fp_pfd.SetPixelFormat(g2dPFD::e_RGBA32f);
			
			io_Image->Make( width, rows_fp.size(), fp_pfd, i_CreateLoc );
			void* image_data = io_Image->Lock();
			int stride = io_Image->GetStride();

			CopyImageData(	image_data,
							stride,
							width,
							fp_pfd,
							rows_fp,
							alpha);
			io_Image->Release();
			//use libtiff memory deallocation to free the memory
			int i;
			int num_rows = rows_fp.size();
			for( i = 0 ; i < num_rows ; i++ )
				_TIFFfree(rows_fp[i]);
		}
		else
		{

			g2dPFD image_pfd;
			if( alpha )
				image_pfd = i_AlphaPFD;
			else
				image_pfd = i_NonAlphaPFD;	

			if ( pixel_bits == 16)
			{
				image_pfd.SetBitsPerPixel(64);
				image_pfd.SetPixelFormat(g2dPFD::e_RGBA16UInt);
				io_Image->Make( width, rows16.size(), image_pfd, i_CreateLoc );
			}
			else
				io_Image->Make( width, rows.size(), image_pfd, i_CreateLoc );

			void* image_data = io_Image->Lock();
			int stride = io_Image->GetStride();

			if ( pixel_bits == 16)
			{
				CopyImageData(	image_data,
					stride,
					width,
					image_pfd,
					rows16,
					alpha);
			}
			else
			{
				CopyImageData(	image_data,
					stride,
					width,
					image_pfd,
					rows,
					alpha);
			}
				
			io_Image->Release();
			
			//use libtiff memory deallocation to free the memory
			int i;
			int num_rows = rows.size();
			for( i = 0 ; i < num_rows ; i++ )
				_TIFFfree(rows[i]);
			num_rows = rows16.size();
			for( i = 0 ; i < num_rows ; i++ )
				_TIFFfree(rows16[i]);
		}
	}

	fsResourceTracker::MarkEnd(i_Locator);
}
void LoadTIFF(g2dImageDX11* io_Image, 
			 const fsLocator& i_Locator, 
			 const g2dPFD& i_PFD, 
			 g2dImage::CreateLoc i_CreateLoc)
{
	LoadTIFF(io_Image, i_Locator, i_PFD, i_PFD, i_CreateLoc );
}

void LoadTIFF(g2dImageDX11* io_Image, 
			 const fsLocator& i_Locator, 
			 g2dImage::CreateLoc i_CreateLoc)
{
	LoadTIFF(io_Image, i_Locator, g2dDX11Global::g_ScreenPixelFormat, g2dDX11Global::g_ScreenPixelFormat, i_CreateLoc );
}

//------------------------------------------------------------------------
//	The rows should be deleted when you are done with them.  o_HasAlpha
//	will be true if the PNG file had an alpha channel.  This affects the
//	format of the pixels in the row data - with alpha RGBA, without
//	RGB.
//------------------------------------------------------------------------
void LoadPNGData(	const fsLocator& i_Locator,
					std::vector<envType::UInt8*>& o_Rows,
					int& o_Width,
					bool& o_HasAlpha)
{
	int height;

	//	get the png data from the file
	//
	load_png_data(i_Locator, o_Rows, o_HasAlpha, o_Width, height);

}

//------------------------------------------------------------------------
//	The rows should be deleted when you are done with them.  The format
//	for this function's row data is always RGB.
//------------------------------------------------------------------------
void LoadBMPData(	const fsLocator& i_Locator,
					std::vector<envType::UInt8*>& o_Rows,
					int& o_Width, bool& o_HasAlpha)
{
	gfFileBin read_stream(i_Locator, fsFileStream::e_ReadOnly);

	BITMAPFILEHEADER file_header;
	read_stream.Read(sizeof(file_header), &file_header);

    if( file_header.bfType != 0x4d42 )	// 'BM'
		throw g2dUnknownImageFileTypeX();

	BITMAPINFOHEADER info_header;
	read_stream.Read(sizeof(info_header), &info_header);

    // don't deal with CORE headers, whatever they are
    if( info_header.biSize == sizeof(BITMAPCOREHEADER) )
		throw g2dUnknownImageFileTypeX();

	//	let's try loading only 24 bit bmps
	if ( ( info_header.biBitCount != 24 ) &&
		 ( info_header.biBitCount != 32 ))
		throw g2dUnknownImageFileTypeX();

	//	we don't want to deal with compression
	if( info_header.biCompression != BI_RGB )
		throw g2dUnknownImageFileTypeX();

	o_HasAlpha = (info_header.biBitCount == 32);

	o_Width = info_header.biWidth;
	int height = info_header.biHeight;

	bool top_down = false;
	if( height < 0 )
	{
		top_down = true;
		height = -height;
	}

	fsFileStream::FilePosType bitmap_data_pos = file_header.bfOffBits;
	read_stream.SetFilePos(bitmap_data_pos);

	int pixelsize = info_header.biBitCount / 8;
	//	24bpp
	int i;
	o_Rows.resize(height);
	for( i = 0 ; i < height ; ++i )
		o_Rows[i] = new envType::UInt8[o_Width * pixelsize];

	int file_y;
	if( top_down )
	{
		for( file_y = 0 ; file_y < height ; ++file_y )
			read_stream.Read(o_Width * pixelsize, o_Rows[file_y]);
	}
	else
	{
		for( file_y = height - 1 ; file_y >= 0 ; --file_y )
			read_stream.Read(o_Width * pixelsize, o_Rows[file_y]);
	}

	//	we must reverse the R and B values to have the proper output format
	for( file_y = 0 ; file_y < height ; ++file_y )
	{
		int x_pixel;
		envType::UInt8* row_base = o_Rows[file_y];

		for( x_pixel = 0 ; x_pixel < o_Width ; ++x_pixel )
		{
			int byte_number = x_pixel * pixelsize;
			envType::UInt8 temp = row_base[byte_number];
			row_base[byte_number] = row_base[byte_number + 2];
			row_base[byte_number + 2] = temp;
		}
	}
}

//------------------------------------------------------------------------
//	The rows should be deleted when you are done with them.  o_HasAlpha
//	will be true if the TIFF file has 4 sample channels.  This affects the
//	format of the pixels in the row data - with alpha RGBA, without
//	RGB.
//------------------------------------------------------------------------
void LoadTIFFData(	const fsLocator& i_Locator,
					std::vector<envType::UInt8*>& o_Rows,
					int& o_Width,
					bool& o_HasAlpha,
					TIFF *i_Image)
{
	int height;
	int width;
	uint16 samples;
	size_t num_pixels;
	//uint32* raster;

	std::string imagefile;
	fsFileUtil::LocatorToANSIFilename(i_Locator, imagefile);

    TIFFGetField(i_Image, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(i_Image, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(i_Image, TIFFTAG_SAMPLESPERPIXEL, &samples);
	
	o_Width = width;
	if( samples == 4)
		o_HasAlpha = true;
	else
		o_HasAlpha = false;

    num_pixels = width * height;

	uint32 imagelength;
	uint32 row;
	uint16 config;
	uint16 result;
	uint16 compression;

	TIFFGetField(i_Image, TIFFTAG_IMAGELENGTH, &imagelength);
    TIFFGetField(i_Image, TIFFTAG_PLANARCONFIG, &config);
    TIFFGetField(i_Image, TIFFTAG_COMPRESSION, &compression);

	//check for unsupported compression types
	if(compression == COMPRESSION_JPEG || compression == COMPRESSION_OJPEG )
	{
		DBG_ERROR("TIFFs with JPEG compression are not supported.");
		TIFFClose(i_Image);	
		throw g2dUnknownImageFileTypeX();
	}

	if(compression == COMPRESSION_ADOBE_DEFLATE )
	{
		DBG_ERROR("TIFFs with ZIP compression are not supported.");
		TIFFClose(i_Image);	
		throw g2dUnknownImageFileTypeX();
	}

	if(config == PLANARCONFIG_SEPARATE)
	{
		DBG_ERROR("Per Channel Pixel Order TIFFs not supported.");
		TIFFClose(i_Image);	
		throw g2dUnknownImageFileTypeX();
	}

	o_Rows.resize(imagelength);
	for( int i = 0 ; i < imagelength ; ++i )
		o_Rows[i] = (envType::UInt8*)_TIFFmalloc(TIFFScanlineSize(i_Image));
	
	//handle the different configuration types of tiff images
	//rgbargba...
	if (config == PLANARCONFIG_CONTIG) 
	{
        for (row = 0; row < imagelength; row++)
		{
			result = TIFFReadScanline(i_Image, o_Rows[row], row);
			if(result == -1)
				DBG_ERROR("Error reading TIFF file: " << imagefile.c_str() );
		}
	}
	//rr...gg...bb...aa... CURRENTLY UNSUPPORTED - LIBTIFF documented bug using ReadScanline function with 
	// Separate configurations
	else if (config == PLANARCONFIG_SEPARATE) 
	{
		//int scan_size = TIFFScanlineSize(i_Image);
		//envType::UInt8* pixel_r = (envType::UInt8*)_TIFFmalloc(scan_size);
		//envType::UInt8* pixel_g = (envType::UInt8*)_TIFFmalloc(scan_size);
		//envType::UInt8* pixel_b = (envType::UInt8*)_TIFFmalloc(scan_size);
		//envType::UInt8* pixel_a = (envType::UInt8*)_TIFFmalloc(scan_size);
		//
		//envType::UInt8* cur_line = NULL;
		//
		//for (row = 0; row < imagelength; row++)
		//{
		//	//r
		//	result = TIFFReadScanline(i_Image, pixel_r, row, 0);
		//	if(result == -1)
		//		DBG_ERROR("Error reading TIFF file: " << imagefile.c_str() );

		//	//g
		//	result = TIFFReadScanline(i_Image, pixel_g, row, 1);
		//	if(result == -1)
		//		DBG_ERROR("Error reading TIFF file: " << imagefile.c_str() );
		//	
		//	//b
		//	result = TIFFReadScanline(i_Image, pixel_b, row, 2);
		//	if(result == -1)
		//		DBG_ERROR("Error reading TIFF file: " << imagefile.c_str() );
		//	
		//	if(samples == 4)
		//	{
		//		//a
		//		result = TIFFReadScanline(i_Image, pixel_a, row, 3);
		//		if(result == -1)
		//			DBG_ERROR("Error reading TIFF file: " << imagefile.c_str() );
		//	}
		//	cur_line = o_Rows[row];
		//	envType::UInt8 red;
		//	envType::UInt8 green;
		//	envType::UInt8 blue;
		//	envType::UInt8 alpha;
		//	envType::UInt8* new_r = NULL;
		//	envType::UInt8* new_g = NULL;
		//	envType::UInt8* new_b = NULL;
		//	envType::UInt8* new_a = NULL;

		//	for (int i = 0; i < scan_size; i++)
		//	{

		//		red = *pixel_r++;
		//		green = *pixel_g++;
		//		blue = *pixel_b++;
		//		if(samples == 4)
		//			alpha = *pixel_a++;

		//		new_r = cur_line++;
		//		new_g = cur_line++;
		//		new_b = cur_line++;
		//		if(samples == 4)
		//			new_a = cur_line++;

		//		*new_r = red;
		//		*new_g = green;
		//		*new_b = blue;
		//		if(samples == 4)
		//			*new_a = alpha;
		//	}
		//}
		//_TIFFfree(pixel_r);
		//_TIFFfree(pixel_g);
		//_TIFFfree(pixel_b);
		//_TIFFfree(pixel_a);
	}
}

//------------------------------------------------------------------------
//	The rows should be deleted when you are done with them.  o_HasAlpha
//	will be true if the TIFF file has 4 sample channels.  This affects the
//	format of the pixels in the row data - with alpha RGBA, without
//	RGB.
//------------------------------------------------------------------------
void LoadTIFFData(const fsLocator& i_Locator,
						std::vector<envType::UInt16*>& o_Rows,
						int& o_Width,
						bool& o_HasAlpha,
						TIFF *i_Image)
{
	int height;
	int width;
	uint16 samples;
	size_t num_pixels;
	//uint32* raster;

	std::string imagefile;
	fsFileUtil::LocatorToANSIFilename(i_Locator, imagefile);

    TIFFGetField(i_Image, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(i_Image, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(i_Image, TIFFTAG_SAMPLESPERPIXEL, &samples);
	
	o_Width = width;
	if( samples == 4)
		o_HasAlpha = true;
	else
		o_HasAlpha = false;

    num_pixels = width * height;

	uint32 imagelength;
	uint32 row;
	uint16 config;
	uint16 result;
	uint16 pixel_bits;
	uint16 compression;

	TIFFGetField(i_Image, TIFFTAG_BITSPERSAMPLE, &pixel_bits);
	TIFFGetField(i_Image, TIFFTAG_IMAGELENGTH, &imagelength);
    TIFFGetField(i_Image, TIFFTAG_PLANARCONFIG, &config);
    TIFFGetField(i_Image, TIFFTAG_COMPRESSION, &compression);
	
	if(compression == COMPRESSION_JPEG || compression == COMPRESSION_OJPEG )
	{
		DBG_ERROR("TIFFs with JPEG compression are not supported.");
		TIFFClose(i_Image);	
		throw g2dUnknownImageFileTypeX();
	}

	if(compression == COMPRESSION_ADOBE_DEFLATE )
	{
		DBG_ERROR("TIFFs with ZIP compression are not supported.");
		TIFFClose(i_Image);	
		throw g2dUnknownImageFileTypeX();
	}

	if(config == PLANARCONFIG_SEPARATE)
	{
		DBG_ERROR("Per Channel Pixel Order TIFFs not supported.");
		TIFFClose(i_Image);	
		throw g2dUnknownImageFileTypeX();
	}

	o_Rows.resize(imagelength);
	for( int i = 0 ; i < imagelength ; ++i )
		o_Rows[i] = (envType::UInt16*)_TIFFmalloc(TIFFScanlineSize(i_Image));
	
	//handle the different configuration types of tiff images
	//rgbargba...
	if (config == PLANARCONFIG_CONTIG) 
	{
        for (row = 0; row < imagelength; row++)
		{
			result = TIFFReadScanline(i_Image, o_Rows[row], row);
			if(result == -1)
				DBG_ERROR("Error reading TIFF file: " << imagefile.c_str() );
		}
	}
	//rr...gg...bb...aa... Currently not implemented, known bug with TIFFReadScanline and per chanel pixel format
	else if (config == PLANARCONFIG_SEPARATE) 
	{
       
	}
}


//------------------------------------------------------------------------
//	The rows should be deleted when you are done with them.  o_HasAlpha
//	will be true if the TIFF file has 4 sample channels.  This affects the
//	format of the pixels in the row data - with alpha RGBA, without
//	RGB.
//------------------------------------------------------------------------
void LoadTIFFData(const fsLocator& i_Locator,
						std::vector<envType::Float32*>& o_Rows,
						int& o_Width,
						bool& o_HasAlpha,
						TIFF *i_Image)
{
	int height;
	int width;
	uint16 samples;
	size_t num_pixels;
	//uint32* raster;

	std::string imagefile;
	fsFileUtil::LocatorToANSIFilename(i_Locator, imagefile);

    TIFFGetField(i_Image, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(i_Image, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(i_Image, TIFFTAG_SAMPLESPERPIXEL, &samples);
	
	o_Width = width;
	if( samples == 4)
		o_HasAlpha = true;
	else
		o_HasAlpha = false;

    num_pixels = width * height;

	uint32 imagelength;
	uint32 row;
	uint16 config;
	uint16 result;
	uint16 pixel_bits;
	uint16 compression;

	TIFFGetField(i_Image, TIFFTAG_BITSPERSAMPLE, &pixel_bits);
	TIFFGetField(i_Image, TIFFTAG_IMAGELENGTH, &imagelength);
    TIFFGetField(i_Image, TIFFTAG_PLANARCONFIG, &config);
    TIFFGetField(i_Image, TIFFTAG_COMPRESSION, &compression);
	
	if(compression == COMPRESSION_JPEG || compression == COMPRESSION_OJPEG )
	{
		DBG_ERROR("TIFFs with JPEG compression are not supported.");
		TIFFClose(i_Image);	
		throw g2dUnknownImageFileTypeX();
	}

	if(compression == COMPRESSION_ADOBE_DEFLATE )
	{
		DBG_ERROR("TIFFs with ZIP compression are not supported.");
		TIFFClose(i_Image);	
		throw g2dUnknownImageFileTypeX();
	}

	if(config == PLANARCONFIG_SEPARATE)
	{
		DBG_ERROR("Per Channel Pixel Order TIFFs not supported.");
		TIFFClose(i_Image);	
		throw g2dUnknownImageFileTypeX();
	}

	o_Rows.resize(imagelength);
	for( int i = 0 ; i < imagelength ; ++i )
		o_Rows[i] = (envType::Float32*)_TIFFmalloc(TIFFScanlineSize(i_Image));
	
	//handle the different configuration types of tiff images
	//rgbargba...
	if (config == PLANARCONFIG_CONTIG) 
	{
        for (row = 0; row < imagelength; row++)
		{
			result = TIFFReadScanline(i_Image, o_Rows[row], row);
			if(result == -1)
				DBG_ERROR("Error reading TIFF file: " << imagefile.c_str() );
		}
	}
	//rr...gg...bb...aa... Currently not implemented, known bug with TIFFReadScanline and per chanel pixel format
	else if (config == PLANARCONFIG_SEPARATE) 
	{
       
	}
}

//------------------------------------------------------------------------
//	This function expects the source image data to be in the same format
//	as that returned by the LoadPNGData and LoadBMPData functions
//	(RGB bytes or RGBA bytes).
//------------------------------------------------------------------------
void CopyImageData(	void* i_DestBits,
					int i_DestStride,
					int i_DestWidth,
					const g2dPFD& i_DestPFD,
					const std::vector<envType::UInt8*>& i_SourceRows,
					bool i_SourceHasAlpha)
{
	//	reformat
	//
	int cur_width, cur_height;
	int height = i_SourceRows.size();

	envType::UInt8* cur_image_line = reinterpret_cast<envType::UInt8*>(i_DestBits);

	if ( i_SourceHasAlpha )
	{
		// alpha version
		//
		for( cur_height = 0 ; cur_height < height ; cur_height++ )
		{
			envType::UInt8*	cur_src_pixel = i_SourceRows[cur_height];
			envType::UInt8* cur_image_pixel = cur_image_line;

			for( cur_width = 0 ; cur_width < i_DestWidth ; cur_width++ )
			{
				envType::UInt8 red = *cur_src_pixel++;
				envType::UInt8 green = *cur_src_pixel++;
				envType::UInt8 blue = *cur_src_pixel++;
				envType::UInt8 alpha = *cur_src_pixel++;
				envType::UInt32 pixel = i_DestPFD.MakePixel(red, green, blue, alpha);

				if( i_DestPFD.BitsPerPixel() == 16 )
				{
					envType::UInt16* word_pixel = reinterpret_cast<envType::UInt16*>(cur_image_pixel);
					*word_pixel = envType::UInt16(pixel);
					cur_image_pixel += 2;
				}
				else if( i_DestPFD.BitsPerPixel() == 24 )
				{
/*					envType::UInt8* png_pixel_ptr = reinterpret_cast<envType::UInt8*>(&pixel);
					cur_image_pixel[0] = png_pixel_ptr[0];
					cur_image_pixel[1] = png_pixel_ptr[1];
					cur_image_pixel[2] = png_pixel_ptr[2];
					cur_image_pixel += 3;*/
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 32 )
				{
					envType::UInt32* dword_pixel = reinterpret_cast<envType::UInt32*>(cur_image_pixel);
					*dword_pixel = pixel;
					cur_image_pixel += 4;
				}
			}

			cur_image_line += i_DestStride;
		}
	}
	else
	{
		// non-alpha version
		for( cur_height = 0 ; cur_height < height ; cur_height++ )
		{
			envType::UInt8*	cur_src_pixel = i_SourceRows[cur_height];
			envType::UInt8* cur_image_pixel = cur_image_line;

			for( cur_width = 0 ; cur_width < i_DestWidth ; cur_width++ )
			{
				envType::UInt8 red = *cur_src_pixel++;
				envType::UInt8 green = *cur_src_pixel++;
				envType::UInt8 blue = *cur_src_pixel++;
				envType::UInt32 pixel = i_DestPFD.MakePixel(red, green, blue);

				if( i_DestPFD.BitsPerPixel() == 16 )
				{
					envType::UInt16* word_pixel = reinterpret_cast<envType::UInt16*>(cur_image_pixel);
					*word_pixel = envType::UInt16(pixel);
					cur_image_pixel += 2;
				}
				else if( i_DestPFD.BitsPerPixel() == 24 )
				{
/*					envType::UInt8* png_pixel_ptr = reinterpret_cast<envType::UInt8*>(&pixel);
					cur_image_pixel[0] = png_pixel_ptr[0];
					cur_image_pixel[1] = png_pixel_ptr[1];
					cur_image_pixel[2] = png_pixel_ptr[2];
					cur_image_pixel += 3;*/
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 32 )
				{
					envType::UInt32* dword_pixel = reinterpret_cast<envType::UInt32*>(cur_image_pixel);
					*dword_pixel = pixel;
					cur_image_pixel += 4;
				}
			}

			cur_image_line += i_DestStride;
		}
	}
}

//------------------------------------------------------------------------
//	This function expects the source image data to be in the same format
//	as that returned by the LoadPNGData and LoadBMPData functions
//	(RGB bytes or RGBA bytes).
//------------------------------------------------------------------------
void CopyImageData(	void* i_DestBits,
					int i_DestStride,
					int i_DestWidth,
					const g2dPFD& i_DestPFD,
					const std::vector<envType::UInt16*>& i_SourceRows,
					bool i_SourceHasAlpha)
{
	//	reformat
	//
	int cur_width, cur_height;
	int height = i_SourceRows.size();
	int valid_stride = i_DestStride / 2;
	
	//  make sure the stride isn't too big
	// valid stride is 4 times the image size because we only do RGBA
	if( i_DestStride != valid_stride )
		i_DestStride = valid_stride;

	envType::UInt16* cur_image_line = reinterpret_cast<envType::UInt16*>(i_DestBits);

	if ( i_SourceHasAlpha )
	{
		// alpha version
		//
		for( cur_height = 0 ; cur_height < height ; cur_height++ )
		{
			envType::UInt16*	cur_src_pixel = i_SourceRows[cur_height];
			envType::UInt16*	cur_image_pixel = cur_image_line;

			for( cur_width = 0 ; cur_width < i_DestWidth ; cur_width++ )
			{
				envType::UInt16 red = *cur_src_pixel++;
				envType::UInt16 green = *cur_src_pixel++;
				envType::UInt16 blue = *cur_src_pixel++;
				envType::UInt16 alpha = *cur_src_pixel++;
				
				if( i_DestPFD.BitsPerPixel() == 16 )
				{
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 24 )
				{
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 32 )
				{
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 64 )
				{
					envType::UInt16* pixel_r = cur_image_pixel++;
					envType::UInt16* pixel_g = cur_image_pixel++;
					envType::UInt16* pixel_b = cur_image_pixel++;
					envType::UInt16* pixel_a = cur_image_pixel++;

					*pixel_r = red;
					*pixel_g = green;
					*pixel_b = blue;
					*pixel_a = alpha;
				}
			}

			cur_image_line += i_DestStride;
		}
	}
	else
	{
		// non-alpha version
		for( cur_height = 0 ; cur_height < height ; cur_height++ )
		{
			envType::UInt16* cur_src_pixel = i_SourceRows[cur_height];
			envType::UInt16* cur_image_pixel = cur_image_line;

			for( cur_width = 0 ; cur_width < i_DestWidth ; cur_width++ )
			{
				envType::UInt16 red = *cur_src_pixel++;
				envType::UInt16 green = *cur_src_pixel++;
				envType::UInt16 blue = *cur_src_pixel++;
				envType::UInt16 alpha = 1;

				if( i_DestPFD.BitsPerPixel() == 16 )
				{
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 24 )
				{
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 32 )
				{
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 64 )
				{
					envType::UInt16* pixel_r = cur_image_pixel++;
					envType::UInt16* pixel_g = cur_image_pixel++;
					envType::UInt16* pixel_b = cur_image_pixel++;
					envType::UInt16* pixel_a = cur_image_pixel++;

					*pixel_r = red;
					*pixel_g = green;
					*pixel_b = blue;
					*pixel_a = alpha;
				}
			}

			cur_image_line += i_DestStride;
		}
	}
}


//------------------------------------------------------------------------
//	This function expects the source image data to be in the same format
//	as that returned by the LoadPNGData and LoadBMPData functions
//	(RGB bytes or RGBA bytes).
//------------------------------------------------------------------------
void CopyImageData(	void* i_DestBits,
					int i_DestStride,
					int i_DestWidth,
					const g2dPFD& i_DestPFD,
					const std::vector<envType::Float32*>& i_SourceRows,
					bool i_SourceHasAlpha)
{
	//	reformat
	//
	int cur_width, cur_height;
	int height = i_SourceRows.size();
	int valid_stride = i_DestStride / 4;
	
	//  make sure the stride isn't too big
	// valid stride is 4 times the image size because we only do RGBA
	if( i_DestStride != valid_stride )
		i_DestStride = valid_stride;

	envType::Float32* cur_image_line = reinterpret_cast<envType::Float32*>(i_DestBits);

	if ( i_SourceHasAlpha )
	{
		// alpha version
		//
		for( cur_height = 0 ; cur_height < height ; cur_height++ )
		{
			envType::Float32*	cur_src_pixel = i_SourceRows[cur_height];
			envType::Float32* cur_image_pixel = cur_image_line;

			for( cur_width = 0 ; cur_width < i_DestWidth ; cur_width++ )
			{
				envType::Float32 red = *cur_src_pixel++;
				envType::Float32 green = *cur_src_pixel++;
				envType::Float32 blue = *cur_src_pixel++;
				envType::Float32 alpha = *cur_src_pixel++;
				
				if( i_DestPFD.BitsPerPixel() == 16 )
				{
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 24 )
				{
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 32 )
				{
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 128 )
				{
					envType::Float32* pixel_r = cur_image_pixel++;
					envType::Float32* pixel_g = cur_image_pixel++;
					envType::Float32* pixel_b = cur_image_pixel++;
					envType::Float32* pixel_a = cur_image_pixel++;

					*pixel_r = red;
					*pixel_g = green;
					*pixel_b = blue;
					*pixel_a = alpha;
				}
			}

			cur_image_line += i_DestStride;
		}
	}
	else
	{
		// non-alpha version
		for( cur_height = 0 ; cur_height < height ; cur_height++ )
		{
			envType::Float32*	cur_src_pixel = i_SourceRows[cur_height];
			envType::Float32* cur_image_pixel = cur_image_line;

			for( cur_width = 0 ; cur_width < i_DestWidth ; cur_width++ )
			{
				envType::Float32 red = *cur_src_pixel++;
				envType::Float32 green = *cur_src_pixel++;
				envType::Float32 blue = *cur_src_pixel++;
				envType::Float32 alpha = 1.0f;

				if( i_DestPFD.BitsPerPixel() == 16 )
				{
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 24 )
				{
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 32 )
				{
					DBG_ASSERT(false, "Not Implemented");
				}
				else if( i_DestPFD.BitsPerPixel() == 128 )
				{
					envType::Float32* pixel_r = cur_image_pixel++;
					envType::Float32* pixel_g = cur_image_pixel++;
					envType::Float32* pixel_b = cur_image_pixel++;
					envType::Float32* pixel_a = cur_image_pixel++;

					*pixel_r = red;
					*pixel_g = green;
					*pixel_b = blue;
					*pixel_a = alpha;
				}
			}

			cur_image_line += i_DestStride;
		}
	}
}
}