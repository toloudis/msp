/*****************************************************************************
**	scrCreator.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Graphics/scr/scrCreator.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Graphics/scr/scrImage.hpp"
#include "Graphics/scr/scrText.hpp"


//--------------------------------------------------------------------
//	Make makes texture based on the locator.  if no width and height
//	are given then the size of the texture is used.
//--------------------------------------------------------------------
scrImage* scrCreator::MakeImage( const fsLocator& i_Locator, 
								g2dWindow* i_pWindow, 
								int i_Width , int i_Height )
{
	scrImage* pImage = new scrImage();
	pImage->SetWindow( i_pWindow );

	if ( (i_Width != 0) && (i_Height != 0) )
	{
		pImage->SetSize( i_Width, i_Height );
	}

	pImage->SetImage( i_Locator );

	return pImage;
}

//--------------------------------------------------------------------
//	Make makes texture based on the locator.  if no width and height
//	are given then the size of the texture is used.
//--------------------------------------------------------------------
scrText* scrCreator::MakeText(	const fsLocator& i_Locator, 
								g2dWindow* i_pWindow, 
								int i_Width, int i_Height )
{
	scrText* pText = new scrText();
	pText->SetWindow( i_pWindow );

	//if ( (i_Width != 0) && (i_Height != 0) )
	//{
	//	pText->SetSize( i_Width, i_Height );
	//}

	pText->SetFontMapImage( i_Locator, i_Width, i_Height );

	return pText;
}
