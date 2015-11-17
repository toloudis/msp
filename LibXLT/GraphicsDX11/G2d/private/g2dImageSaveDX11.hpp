/*****************************************************************************
**  g2dImageSaveDX11.hpp
**
**      g2dImageSaveDX11 contains the windows implementation of the
**	g2dImageSave.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_IMAGESAVED3D11_HPP
#error g2dImageSaveDX11.hpp multiply included
#endif
#define G2D_IMAGESAVED3D11_HPP

#ifndef G2D_IMAGESAVE_HPP
#include "Graphics/g2d/g2dImageSave.hpp"
#endif

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class fsLocator;
class itString;

//============================================================================
//============================================================================
class g2dImageSaveDX11 : public g2dImageSaveImpl
{
public:

	//------------------------------------------------------------------------
	//	Save will attempt to figure out the type based on the filename 
	//	passed in.
	//
	//	If the file format is not known the function will throw 
	//	g2dUnknownImageFileTypeX.
	//------------------------------------------------------------------------
	void Save(const fsLocator& i_FileName, g2dWindow* i_pWin);
	void Save(const fsLocator& i_FileName, const g2dImage* i_pImage);
	void Save(const itString& i_FileName, const g2dImage* i_pImage);

	enum FileType
	{
		e_BMP,
		e_JPG,
		e_TGA,
		e_PNG,
		e_DDS,
		e_PPM,
		e_DIB,
		e_HDR,
		e_PFM,
		e_TIFF,
		e_EXR
	};

private:
	void Save(const itString& i_Filename, const g2dD3D11TexturePtr i_pSurface, FileType i_Format);
};
