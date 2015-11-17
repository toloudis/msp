/****************************************************************************\
**  g2dImageDrawUtilDX11.cpp
**
**      g2dImageDrawUtil.cpp supplies some primitive
**	drawing operations for g2dImages.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/g2d/g2dARGBColor.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/private/g2dImageDrawUtilDX11.hpp"

namespace g2dImageDrawUtilDX11
{

namespace
{

/*inline void maybe_restore_image(const g2dImage& o_Image)
{
	g2dImagePAC& pac = o_Image.GetPAC();
	if( pac.GetSurface()->IsLost() )
	{
		pac.GetSurface()->Restore();

		if( pac.GetLocator().GetNumNames() > 0 )
			g2dImageLoadUtilPAC::Load(pac.GetLocator(), const_cast<g2dImage&>(o_Image));
	}
}
*/
}

//------------------------------------------------------------------------
//	This DrawImage blits a rectangular section of the source image to a
//	location on the destination image.  The rectangle of the source image
//	is defined by (i_SX1, i_SY1) - (i_SX2, i_SY2).  The point
//	(i_SX1, i_SY1) will be located at (i_DX1, i_DY1).
//------------------------------------------------------------------------
void DrawImage(	g2dD3D11TexturePtr o_DestSurface,
				int i_DX1,
				int i_DY1,
				int i_DestWidth,
				int i_DestHeight,
				int i_SX1,
				int i_SY1,
				int i_SX2,
				int i_SY2,
				g2dD3D11TexturePtr i_SrcSurface)
{
	DBG_ASSERT(o_DestSurface, "Image has no surface");
	DBG_ASSERT(i_SrcSurface, "Image has no surface");

//	maybe_restore_image(o_DestSurface);
//	maybe_restore_image(i_SrcSurface);

	int source_width = i_SX2 - i_SX1;
	int source_height = i_SY2 - i_SY1;

	// do some clipping and rejection
	if( i_DX1 < 0 )
	{
		if( i_DX1 <= -source_width )
			return; // off the left edge

		i_SX1 -= i_DX1;
		source_width += i_DX1;
		i_DX1 = 0;
	}

	if ( i_DY1 < 0 )
	{
		if( i_DY1 <= -source_height )
			return; // off the top edge

		i_SY1 -= i_DY1;
		source_height += i_DY1;
		i_DY1 = 0;
	}

	int width_over_limit = i_DX1 + source_width - i_DestWidth;
	if ( width_over_limit > 0 )
	{
		if( i_DX1 >= i_DestWidth )
			return;	// off the right edge

		i_SX2 -= width_over_limit;
		source_width -= width_over_limit;
	}

	int height_over_limit = i_DY1 + source_height - i_DestHeight;
	if ( height_over_limit > 0 )
	{
		if( i_DY1 >= i_DestHeight )
			return;	// off the right edge

		i_SY2 -= height_over_limit;
		source_height -= height_over_limit;
	}

	DBG_ASSERT(source_width > 0, "Invalid image area");
	DBG_ASSERT(source_height > 0, "Invalid image area");

	D3D11_BOX source_rect;
	source_rect.left = i_SX1;
	source_rect.top = i_SY1;
	source_rect.right = i_SX2;
	source_rect.bottom = i_SY2;
	source_rect.front = 0;
	source_rect.back = 1;

	g2dDX11Global::g_pDeviceContext->CopySubresourceRegion(o_DestSurface,
		D3D11CalcSubresource(0, 0, 1),
		i_DX1, i_DY1, 0,
		i_SrcSurface,
		D3D11CalcSubresource(0, 0, 1),
		&source_rect
		);

	//if ( !SUCCEEDED(op_result) )
	//{
	//	g2dDX11Global::PrintDXError(op_result);
	//	DBG_ERROR("Couldn't copy to surface");
	//	DBG_ASSERT(false, "DrawImage failed");
	//}
}


}
