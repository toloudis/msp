/*****************************************************************************
**  g2dImageCreate.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dImageCreate.hpp"

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
namespace g2dImageCreate
{

namespace
{
	g2dImageCreateImpl *l_pImpl = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void SetImplementation(g2dImageCreateImpl *i_pImpl)
{
	l_pImpl = i_pImpl;
}

//--------------------------------------------------------------------
//	Make makes the surface into one with the given dimensions and
//	pixel format.  If the pixel format is not supported it will
//	throw a g2dUnsupportedPixelFormatX, and leave the old surface
//	intact.
//--------------------------------------------------------------------
g2dImage* Make(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_Loc /*= g2dImage::e_SystemMemory*/ )
{
	DBG_ASSERT(l_pImpl, "No image creation implmentation.");
	if (!l_pImpl)
		return NULL;
	return l_pImpl->Make(i_Width, i_Height, i_PFD, i_Loc );
}

//------------------------------------------------------------------------
//	This Load will attempt to use whatever platform-specific facilities
//	are available to load the given file from disk.  If the file does
//	not seem to represent a useable image type, it will throw a
//	g2dUnknownImageFileTypeX.  If the file doesn't seem to exist, it
//	will throw a fsFileDoesntExistX (see the fs package).
//------------------------------------------------------------------------
g2dImage* Load(const fsLocator& i_Locator, g2dImage::CreateLoc i_Loc )
{
	DBG_ASSERT(l_pImpl, "No image creation implmentation.");
	if (!l_pImpl)
		return NULL;
	return l_pImpl->Load(i_Locator, i_Loc );
}


}


