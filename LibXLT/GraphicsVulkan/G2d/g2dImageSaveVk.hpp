/****************************************************************************\
**  g2dImageSaveVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g2dImageSaveDX11: saves an
**	image, or a window's back buffer, to a file whose type its extension
**	names. EXR, HDR and TGA go through Graphics' writers, as on DX11;
**	PNG (8-bit RGBA images) through stb_image_write, where DX11 used
**	WIC. The other WIC formats (BMP, JPG, DDS, TIFF) aren't ported yet.
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
