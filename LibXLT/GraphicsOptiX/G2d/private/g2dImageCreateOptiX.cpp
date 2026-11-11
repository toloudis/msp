/*****************************************************************************
**  g2dImageCreateOptiX.cpp
**
**      See g2dImageCreateOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G2DIMAGECREATEOPTIX_HPP
#include "GraphicsOptiX/G2d/private/g2dImageCreateOptiX.hpp"
#endif

namespace gox
{
	g2dImage* g2dImageCreateOptiX::Make(int /*i_Width*/, int /*i_Height*/, const g2dPFD& /*i_PFD*/, g2dImage::CreateLoc /*i_Loc*/)
	{
		return NULL;
	}

	g2dImage* g2dImageCreateOptiX::Load(const fsLocator& /*i_Locator*/, g2dImage::CreateLoc /*i_Loc*/)
	{
		return NULL;
	}
}
