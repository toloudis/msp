/*****************************************************************************
**  g2dImageSave.hpp
**
**      Saves an image, or a window's back buffer, to a file whose type its
**	extension names, through g2dImageSaveUtil (the same on every backend;
**	a window is captured to an image with g2dScreenCaptureUtil first).
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_IMAGESAVE_HPP
#error g2dImageSave.hpp multiply included
#endif
#define G2D_IMAGESAVE_HPP

#ifndef G2D_IMAGE_HPP
#include "Graphics/g2d/g2dImage.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif

//============================================================================
//	forward references
//============================================================================
class fsLocator;
class g2dImage;
class g2dWindow;


//============================================================================
//============================================================================
namespace g2dImageSave
{
	//------------------------------------------------------------------------
	//	Save will attempt to figure out the type based on the filename
	//	passed in.
	//
	//	If the file format is not known the function will throw
	//	g2dUnknownImageFileTypeX; if the file can't be written,
	//	g2dImageSaveX.
	//------------------------------------------------------------------------
	void Save(const fsLocator& i_FileName, g2dWindow* i_pWin);
	void Save(const fsLocator& i_FileName, const g2dImage* i_pImage);
}
