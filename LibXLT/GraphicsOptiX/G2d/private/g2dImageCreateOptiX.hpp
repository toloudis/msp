/*****************************************************************************
**  g2dImageCreateOptiX.hpp
**
**      g2dImageCreateOptiX makes and loads the OptiX backend's CPU images
**	(g2dImageCreate).
**
**	Stub: makes and loads nothing (NULL).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G2DIMAGECREATEOPTIX_HPP
#error g2dImageCreateOptiX.hpp multiply included
#endif
#define GOX_G2DIMAGECREATEOPTIX_HPP

#ifndef G2D_IMAGECREATE_HPP
#include "Graphics/G2d/g2dImageCreate.hpp"
#endif

namespace gox
{
	class g2dImageCreateOptiX : public g2dImageCreateImpl
	{
		public:
			virtual g2dImage* Make(int i_Width, int i_Height, const g2dPFD& i_PFD, g2dImage::CreateLoc i_Loc);
			virtual g2dImage* Load(const fsLocator& i_Locator, g2dImage::CreateLoc i_Loc);
	};
}
