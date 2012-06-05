/****************************************************************************\
**  g2dSurfaceLoaderDX11.hpp
**
**      g2dSurfaceLoaderDX11.hpp contains some helper functions for loading
**	bitmap data to surfaces in windows.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_SURFACELOADERD3D11_HPP
#error g2dSurfaceLoaderDX11.hpp multiply included
#endif
#define G2D_SURFACELOADERD3D11_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif
#ifndef G2D_IMAGE_HPP
#include "Graphics/g2d/g2dImage.hpp"
#endif

#include "Tiff/tiffio.h"
#include <vector>


//============================================================================
//	forward references
//============================================================================
class g2dImageDX11;
class fsLocator;


//============================================================================
//============================================================================
namespace g2dSurfaceLoader
{
	//------------------------------------------------------------------------
	//	The version that doesn't take a g2dPFD loads to the screen
	//	pixel format.  The version that takes one PFD loads to that format.
	//	The version that takes two loads to one or the other depending on
	//	if there is alpha in the file.
	//------------------------------------------------------------------------
	void LoadPNG(g2dImageDX11* io_Image, const fsLocator& i_Locator, const g2dPFD& i_AlphaPFD, const g2dPFD& i_NonAlphaPFD, g2dImage::CreateLoc i_CreateLoc = g2dImage::e_ScratchMemory );
	void LoadPNG(g2dImageDX11* io_Image, const fsLocator& i_Locator, const g2dPFD& i_PFD, g2dImage::CreateLoc i_CreateLoc = g2dImage::e_ScratchMemory );
	void LoadPNG(g2dImageDX11* io_Image, const fsLocator& i_Locator, g2dImage::CreateLoc i_CreateLoc = g2dImage::e_ScratchMemory );

	//------------------------------------------------------------------------
	//	The version that doesn't take a g2dPFD loads to the screen
	//	pixel format.
	//------------------------------------------------------------------------
	void LoadBMP(g2dImageDX11* io_Image, const fsLocator& i_Locator, const g2dPFD& i_PFD, g2dImage::CreateLoc i_CreateLoc = g2dImage::e_ScratchMemory );
	void LoadBMP(g2dImageDX11* io_Image, const fsLocator& i_Locator, g2dImage::CreateLoc i_CreateLoc = g2dImage::e_ScratchMemory );

	//------------------------------------------------------------------------
	//	The version that doesn't take a g2dPFD loads to the screen
	//	pixel format.  The version that takes one PFD loads to that format.
	//	The version that takes two loads to one or the other depending on
	//	if there is alpha in the file.
	//------------------------------------------------------------------------
	void LoadTIFF(g2dImageDX11* io_Image, const fsLocator& i_Locator, const g2dPFD& i_AlphaPFD, const g2dPFD& i_NonAlphaPFD, g2dImage::CreateLoc i_CreateLoc = g2dImage::e_ScratchMemory );
	void LoadTIFF(g2dImageDX11* io_Image, const fsLocator& i_Locator, const g2dPFD& i_PFD, g2dImage::CreateLoc i_CreateLoc = g2dImage::e_ScratchMemory );
	void LoadTIFF(g2dImageDX11* io_Image, const fsLocator& i_Locator, g2dImage::CreateLoc i_CreateLoc = g2dImage::e_ScratchMemory );

	//------------------------------------------------------------------------
	//	The rows should be deleted when you are done with them.  o_HasAlpha
	//	will be true if the PNG file had an alpha channel.  This affects the
	//	format of the pixels in the row data - with alpha RGBA, without
	//	RGB.
	//------------------------------------------------------------------------
	void LoadPNGData(	const fsLocator& i_Locator,
						std::vector<envType::UInt8*>& o_Rows,
						int& o_Width,
						bool& o_HasAlpha);

	//------------------------------------------------------------------------
	//	The rows should be deleted when you are done with them.  The format
	//	for this function's row data is always RGB.
	//------------------------------------------------------------------------
	void LoadBMPData(	const fsLocator& i_Locator,
						std::vector<envType::UInt8*>& o_Rows,
						int& o_Width,
						bool& o_HasAlpha);

	//------------------------------------------------------------------------
	//	The rows should be deleted when you are done with them.  The format
	//	for this function's row data is RGBA.
	//------------------------------------------------------------------------
	void LoadTIFFData(	const fsLocator& i_Locator,
						std::vector<envType::UInt8*>& o_Rows,
						int& o_Width,
						bool& o_HasAlpha,
						TIFF* i_Image);

	//------------------------------------------------------------------------
	//	The rows should be deleted when you are done with them.  The format
	//	for this function's row data is RGBA.
	//------------------------------------------------------------------------
	void LoadTIFFData(	const fsLocator& i_Locator,
						std::vector<envType::UInt16*>& o_Rows,
						int& o_Width,
						bool& o_HasAlpha,
						TIFF* i_Image);
	
	//------------------------------------------------------------------------
	//	The rows should be deleted when you are done with them.  The format
	//	for this function's row data is RGBA.  This call does floating point tiffs
	//------------------------------------------------------------------------
	void LoadTIFFData(	const fsLocator& i_Locator,
						std::vector<envType::Float32*>& o_Rows,
						int& o_Width,
						bool& o_HasAlpha,
						TIFF* i_Image);

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
						bool i_SourceHasAlpha);

	//------------------------------------------------------------------------
	//	This function expects the source image data to be in the same format
	//	as that returned by the LoadPNGData and LoadBMPData functions
	//	(RGB bytes or RGBA bytes).  The source rows are float type.
	//------------------------------------------------------------------------
	void CopyImageData(	void* i_DestBits,
						int i_DestStride,
						int i_DestWidth,
						const g2dPFD& i_DestPFD,
						const std::vector<envType::UInt16*>& i_SourceRows,
						bool i_SourceHasAlpha);

	//------------------------------------------------------------------------
	//	This function expects the source image data to be in the same format
	//	as that returned by the LoadPNGData and LoadBMPData functions
	//	(RGB bytes or RGBA bytes).  The source rows are float type.
	//------------------------------------------------------------------------
	void CopyImageData(	void* i_DestBits,
						int i_DestStride,
						int i_DestWidth,
						const g2dPFD& i_DestPFD,
						const std::vector<envType::Float32*>& i_SourceRows,
						bool i_SourceHasAlpha);
}
