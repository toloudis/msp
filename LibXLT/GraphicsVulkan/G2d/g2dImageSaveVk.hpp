/****************************************************************************\
**  g2dImageSaveVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g2dImageSaveDX11: saves an
**	image, or a window's back buffer, to a file whose type its extension
**	names. EXR, HDR and TGA (images only) go through Graphics' writers,
**	as on DX11; BMP, JPG, PNG and TIFF through matImageEncode, which
**	writes what DX11's WIC path writes. DDS, PPM, DIB and PFM throw
**	g2dUnknownImageFileTypeX, as on DX11.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G2DIMAGESAVEVK_HPP
#error g2dImageSaveVk.hpp multiply included
#endif
#define GVK_G2DIMAGESAVEVK_HPP

#ifndef G2D_IMAGESAVE_HPP
#include "Graphics/G2d/g2dImageSave.hpp"
#endif

namespace gvk
{
	class g2dImageSaveVk : public g2dImageSaveImpl
	{
		public:
			//------------------------------------------------------------------------
			//	Save will attempt to figure out the type based on the filename
			//	passed in.
			//
			//	If the file format is not known the function will throw
			//	g2dUnknownImageFileTypeX.
			//------------------------------------------------------------------------
			virtual void Save(const fsLocator& i_FileName, g2dWindow* i_pWin);
			virtual void Save(const fsLocator& i_FileName, const g2dImage* i_pImage);
	};
}
