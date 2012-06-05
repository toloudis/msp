/*****************************************************************************
**  g2dImageCreateDX11.cpp
**
**      g2dImageCreateDX11 contains the windows implementation of the
**	g2dImageCreate.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g2d/private/g2dImageCreateDX11.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX11/g2d/g2dImageDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dSurfaceLoaderDX11.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/gf/gfFileTranslationMgr.hpp"

#include "png.h"

namespace
{

enum FileType
{
	e_BMP,
	e_PNG
};

FileType pick_format(const fsLocator& i_Locator)
{
	//	decide what kind of file we have
	//
	gfFileBin test_file_stream(i_Locator, fsFileStream::e_ReadOnly);

	//	The BMP header begins with 0x42 0x4d 'BM'
	unsigned char header[8];
	test_file_stream.Read(2, header);

	if( (header[0] == 0x42) && (header[1] == 0x4d) )
		return e_BMP;
	else
	{
		test_file_stream.Read(6, header+2);

		if( (header[0] == 137)	&&
			(header[1] == 80)	&&
			(header[2] == 78)	&&
			(header[3] == 71)	&&
			(header[4] == 13)	&&
			(header[5] == 10)	&&
			(header[6] == 26)	&&
			(header[7] == 10)	)
		{
			return e_PNG;
		}
		else
			throw g2dUnknownImageFileTypeX();
	}
}

void load_png(const fsLocator& i_Locator, g2dImageDX11& o_Image, g2dImage::CreateLoc i_CreateLoc )
{
	g2dSurfaceLoader::LoadPNG( &o_Image, i_Locator, i_CreateLoc );
}

void load_bmp(const fsLocator& i_Locator, g2dImageDX11& o_Image, g2dImage::CreateLoc i_CreateLoc )
{
	g2dSurfaceLoader::LoadBMP( &o_Image, i_Locator, i_CreateLoc );
}

}

//--------------------------------------------------------------------
//	Make makes the surface into one with the given dimensions and
//	pixel format.  If the pixel format is not supported it will
//	throw a g2dUnsupportedPixelFormatX, and leave the old surface
//	intact.
//--------------------------------------------------------------------
g2dImage* g2dImageCreateDX11::Make( int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_CreateLoc )
{
	return new g2dImageDX11(i_Width, i_Height, i_PFD, i_CreateLoc );
}

//------------------------------------------------------------------------
//	This Load will attempt to use whatever platform-specific facilities
//	are available to load the given file from disk.  If the file does
//	not seem to represent a useable image type, it will throw a
//	g2dUnknownImageFileTypeX.  If the file doesn't seem to exist, it
//	will throw a fsFileDoesntExistX (see the fs package).
//------------------------------------------------------------------------
g2dImage* g2dImageCreateDX11::Load( const fsLocator& i_Locator, g2dImage::CreateLoc i_CreateLoc )
{
	if( !gfFileTranslationMgr::FileExists(i_Locator) )
		throw fsFileDoesntExistX(i_Locator);

	FileType file_type = pick_format(i_Locator);

	g2dImageDX11 *image = new g2dImageDX11;
	switch( file_type )
	{
		case e_BMP:
			load_bmp(i_Locator, *image, i_CreateLoc );
		break;

		case e_PNG:
			load_png(i_Locator, *image, i_CreateLoc );
		break;

		// put in default case here?
	}

	image->SetLocator(i_Locator);
	return image;
}

