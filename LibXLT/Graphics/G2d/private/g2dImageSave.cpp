/*****************************************************************************
**  g2dImageSave.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dImageSave.hpp"

#ifndef G2D_IMAGESAVEUTIL_HPP
#include "Graphics/g2d/g2dImageSaveUtil.hpp"
#endif
#ifndef G2D_SCREENCAPTUREUTIL_HPP
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#endif

#include <memory>


//============================================================================
//============================================================================
namespace g2dImageSave
{

//------------------------------------------------------------------------
//	Save will attempt to figure out the type based on the filename
//	passed in.
//
//	If the file format is not known the function will throw
//	g2dUnknownImageFileTypeX.
//------------------------------------------------------------------------
void Save(const fsLocator& i_FileName, g2dWindow* i_pWin)
{
	if (!i_pWin)
		return;

	g2dImage* image = NULL;
	g2dScreenCaptureUtil::CaptureWindowToImage(*i_pWin, image);
	std::unique_ptr<g2dImage> owned(image);
	g2dImageSaveUtil::Write(i_FileName, image);
}

void Save(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	g2dImageSaveUtil::Write(i_FileName, i_pImage);
}
}
