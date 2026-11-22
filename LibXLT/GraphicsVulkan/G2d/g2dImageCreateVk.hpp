/*****************************************************************************
**  g2dImageCreateVk.hpp
**
**      GraphicsDX11's g2dImageCreateDX11: makes g2dImageVk images, and
**	loads BMP and PNG files into them as DX11's g2dSurfaceLoader does.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G2DIMAGECREATEVK_HPP
#error g2dImageCreateVk.hpp multiply included
#endif
#define GVK_G2DIMAGECREATEVK_HPP

#ifndef G2D_IMAGECREATE_HPP
#include "Graphics/G2d/g2dImageCreate.hpp"
#endif

class fsLocator;

namespace gvk
{
	class g2dImageCreateVk : public g2dImageCreateImpl
	{
		public:
			virtual g2dImage* Make(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_Loc);
			virtual g2dImage* Load(const fsLocator& i_Locator, g2dImage::CreateLoc i_Loc);
	};
}
