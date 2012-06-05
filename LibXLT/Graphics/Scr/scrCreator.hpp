/*****************************************************************************
**	scrCreator.cpp
**
**		scrCreator is the access point for creating and loading scr primitives
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef SCR_CREATOR_HPP
#error scrCreator.hpp multiply included
#endif
#define SCR_CREATOR_HPP

#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class fsLocator;
class gfFileBin;
class scrImage;
class scrText;
class g2dWindow;


//============================================================================
//============================================================================
namespace scrCreator
{
	//--------------------------------------------------------------------
	//	Make makes texture based on the locator.  if no width and height
	//	are given then the size of the texture is used.
	//--------------------------------------------------------------------
	scrImage* MakeImage(const fsLocator& i_Locator,
						g2dWindow* i_pWindow,
						int i_Width = 0, int i_Height = 0);

	//--------------------------------------------------------------------
	//	Make makes texture based on the locator.  if no width and height
	//	are given then the size of the texture is used.
	//--------------------------------------------------------------------
	scrText* MakeText(const fsLocator& i_Locator,
						g2dWindow* i_pWindow,
						int i_Width = 0, int i_Height = 0);
}
