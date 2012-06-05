/*****************************************************************************
**  g2dImageCreateDX11.hpp
**
**      g2dImageCreateDX11 contains the windows implementation of the
**	g2dImageCreate.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_IMAGECREATED3D11_HPP
#error g2dImageCreateDX11.hpp multiply included
#endif
#define G2D_IMAGECREATED3D11_HPP

#ifndef G2D_IMAGECREATE_HPP
#include "Graphics/g2d/g2dImageCreate.hpp"
#endif


class g2dImageCreateDX11 : public g2dImageCreateImpl
{
public:

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.  If the pixel format is not supported it will
	//	throw a g2dUnsupportedPixelFormatX, and leave the old surface
	//	intact.
	//--------------------------------------------------------------------
	g2dImage* Make(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_CreateLoc = g2dImage::e_SystemMemory );

	//------------------------------------------------------------------------
	//	This Load will attempt to use whatever platform-specific facilities
	//	are available to load the given file from disk.  If the file does
	//	not seem to represent a useable image type, it will throw a
	//	g2dUnknownImageFileTypeX.  If the file doesn't seem to exist, it
	//	will throw a fsFileDoesntExistX (see the fs package).
	//------------------------------------------------------------------------
	g2dImage* Load(const fsLocator& i_Locator, g2dImage::CreateLoc i_CreateLoc );
};
