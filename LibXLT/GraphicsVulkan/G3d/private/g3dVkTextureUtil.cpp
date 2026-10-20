/****************************************************************************\
**  g3dVkTextureUtil.cpp
**
**	See g3dVkTextureUtil.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_MATTEXTUREVK_HPP
#include "GraphicsVulkan/Mat/matTextureVk.hpp"
#endif
#ifndef MAT_UVATEXTURE_HPP
#include "Graphics/Mat/matUVATexture.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef MA_FUNCTIONS_HPP
#include "Core/Ma/maFunctions.hpp"
#endif
#ifndef MA_VECTOR2D_HPP
#include "Core/Ma/maVector2d.hpp"
#endif
#ifndef MA_VECTOR4D_HPP
#include "Core/Ma/maVector4d.hpp"
#endif

namespace gvk
{
	//------------------------------------------------------------------------
	// GetImage - return the image for Terawatt texture
	//------------------------------------------------------------------------
	vkdImage* g3dVkTextureUtil::GetImage(const matTexture* i_pTexture)
	{
		const matUVATexture* uva = dynamic_cast<const matUVATexture*>(i_pTexture);
		if ( uva != NULL )
		{
			// only supports 1 tex-page for now
			const matTextureVk* cur_page = dynamic_cast<const matTextureVk*>(uva->GetPage(0));
			DBG_ASSERT(cur_page, "UVA texture page is not a valid Vulkan texture.");
			return cur_page ? cur_page->GetImage() : NULL;
		}

		const matTextureVk* texture = dynamic_cast<const matTextureVk*>(i_pTexture);
		if( texture != NULL )
		{
			return texture->GetImage();
		}

		return NULL;
	}

	void g3dVkTextureUtil::SetRect(TexRect* o_pRect, int i_Left, int i_Top, int i_Right, int i_Bottom)
	{
		o_pRect->left = i_Left;
		o_pRect->top = i_Top;
		o_pRect->right = i_Right;
		o_pRect->bottom = i_Bottom;
	}

	void g3dVkTextureUtil::InflateRect(TexRect* io_pRect, int i_Dx, int i_Dy)
	{
		io_pRect->left -= i_Dx;
		io_pRect->top -= i_Dy;
		io_pRect->right += i_Dx;
		io_pRect->bottom += i_Dy;
	}

	//-----------------------------------------------------------------------------
	// Name: GetTextureCoords()
	// Desc: Get the texture coordinates to use when rendering into the destination
	//       texture, given the source and destination rectangles
	//-----------------------------------------------------------------------------
	bool g3dVkTextureUtil::GetTextureCoords( matTexture* pTexSrc, TexRect* pRectSrc,
							  matTexture* pTexDest, TexRect* pRectDest, CoordRect* pCoords )
	{
		float tU, tV;

		// Validate arguments
		if( pTexSrc == NULL || pTexDest == NULL || pCoords == NULL )
			return false;

		// Start with a default mapping of the complete source surface to complete
		// destination surface
		pCoords->fLeftU = 0.0f;
		pCoords->fTopV = 0.0f;
		pCoords->fRightU = 1.0f;
		pCoords->fBottomV = 1.0f;

		// If not using the complete source surface, adjust the coordinates
		if( pRectSrc != NULL )
		{
			// These delta values are the distance between source texel centers in
			// texture address space
			tU = 1.0f / pTexSrc->GetWidth();
			tV = 1.0f / pTexSrc->GetHeight();

			pCoords->fLeftU += pRectSrc->left * tU;
			pCoords->fTopV += pRectSrc->top * tV;
			pCoords->fRightU -= (pTexSrc->GetWidth() - pRectSrc->right) * tU;
			pCoords->fBottomV -= (pTexSrc->GetHeight() - pRectSrc->bottom) * tV;
		}

		// If not drawing to the complete destination surface, adjust the coordinates
		if( pRectDest != NULL )
		{
			// These delta values are the distance between destination texel centers in
			// texture address space
			tU = 1.0f / pTexDest->GetWidth();
			tV = 1.0f / pTexDest->GetHeight();

			pCoords->fLeftU -= pRectDest->left * tU;
			pCoords->fTopV -= pRectDest->top * tV;
			pCoords->fRightU += (pTexDest->GetWidth() - pRectDest->right) * tU;
			pCoords->fBottomV += (pTexDest->GetHeight() - pRectDest->bottom) * tV;
		}

		return true;
	}

	//-----------------------------------------------------------------------------
	// Name: GetSampleOffsets_GaussBlur5x5
	// Desc: Get the texture coordinate offsets to be used inside the GaussBlur5x5
	//       pixel shader (see the .hpp)
	//-----------------------------------------------------------------------------
	bool g3dVkTextureUtil::GetSampleOffsets_GaussBlur5x5(float i_filterWidthX,
														 float i_filterWidthY,
														 maVector2d* avTexCoordOffset,
														 maVector4d* avSampleWeight,
														 float fMultiplier )
	{
		float tu = 0.25f * i_filterWidthX;
		float tv = 0.25f * i_filterWidthY;

		static const maVector4d vWhite( 1.0f, 1.0f, 1.0f, 1.0f );

		float totalWeight = 0.0f;
		int index=0;
		for( int x = -2; x <= 2; x++ )
		{
			for( int y = -2; y <= 2; y++ )
			{
				// Get the unscaled Gaussian intensity for this offset
				avTexCoordOffset[index] = maVector2d( x * tu, y * tv );
				avSampleWeight[index] = vWhite * maFunctions::GaussianDistribution( (float)x, (float)y, 1.0f );
				totalWeight += avSampleWeight[index].GetX();

				index++;
			}
		}

		// Divide the current weight by the total weight of all the samples; Gaussian
		// blur kernels add to 1.0f to ensure that the intensity of the image isn't
		// changed when the blur occurs. An optional multiplier variable is used to
		// add or remove image intensity during the blur.
		for( int i=0; i < index; i++ )
		{
			avSampleWeight[i] /= totalWeight;
			avSampleWeight[i] *= fMultiplier;
		}

		return true;
	}
}
