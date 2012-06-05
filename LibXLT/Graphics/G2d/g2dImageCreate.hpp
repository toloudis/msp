/*****************************************************************************
**  g2dImageCreate.cpp
**
**      g2dImageCreate is the access point for creating and loading images
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_IMAGECREATE_HPP
#error g2dImageCreate.hpp multiply included
#endif
#define G2D_IMAGECREATE_HPP

#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif
#ifndef G2D_IMAGE_HPP
#include "Graphics/g2d/g2dImage.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class fsLocator;
class gfFileBin;
class g2dImage;


//============================================================================
//============================================================================
class g2dImageCreateImpl
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~g2dImageCreateImpl() {};

	//--------------------------------------------------------------------
	//	Make makes the surface into one with the given dimensions and
	//	pixel format.  If the pixel format is not supported it will
	//	throw a g2dUnsupportedPixelFormatX, and leave the old surface
	//	intact.
	//--------------------------------------------------------------------
	virtual g2dImage* Make(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_Loc ) = 0;

	//------------------------------------------------------------------------
	//	This Load will attempt to use whatever platform-specific facilities
	//	are available to load the given file from disk.  If the file does
	//	not seem to represent a useable image type, it will throw a
	//	g2dUnknownImageFileTypeX.  If the file doesn't seem to exist, it
	//	will throw a fsFileDoesntExistX (see the fs package).
	//------------------------------------------------------------------------
	virtual g2dImage* Load(const fsLocator& i_Locator, g2dImage::CreateLoc i_Loc ) = 0;
};


//============================================================================
//============================================================================
namespace g2dImageCreate
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetImplementation(g2dImageCreateImpl *i_pImpl);

	//--------------------------------------------------------------------
	//	Make() makes the surface into one with the given dimensions and
	//	pixel format.  If the pixel format is not supported it will
	//	throw a g2dUnsupportedPixelFormatX, and leave the old surface
	//	intact.
	//--------------------------------------------------------------------
	g2dImage* Make(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_Loc = g2dImage::e_SystemMemory);

	//------------------------------------------------------------------------
	//	This Load will attempt to use whatever platform-specific facilities
	//	are available to load the given file from disk.  If the file does
	//	not seem to represent a useable image type, it will throw a
	//	g2dUnknownImageFileTypeX.  If the file doesn't seem to exist, it
	//	will throw a fsFileDoesntExistX (see the fs package).
	//------------------------------------------------------------------------
	g2dImage* Load(const fsLocator& i_Locator, g2dImage::CreateLoc i_Loc = g2dImage::e_SystemMemory);
}
