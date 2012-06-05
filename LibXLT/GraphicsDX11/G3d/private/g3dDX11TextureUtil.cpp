/*****************************************************************************
**  g3dDX11TextureUtil.cpp
**
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/ma/maVector4d.hpp"
#include "Graphics/mat/matUVATexture.hpp"
#include "GraphicsDX11/mat/matTextureDX11.hpp"

//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if( op_result != D3D_OK )	\
			{	\
				g2dDX11Global::PrintDXError(op_result);	\
				DBG_ASSERT0(false, error_string);	\
			}	\

namespace
{
}


//------------------------------------------------------------------------
// GetD3DTexture - return d3d texture for Terawatt texture
//------------------------------------------------------------------------
ID3D11ShaderResourceView* g3dDX11TextureUtil::GetD3DTexture(const matTexture* i_pTexture)
{
	const matUVATexture* uva = dynamic_cast<const matUVATexture*>(i_pTexture);
	if ( uva != NULL )
	{
		// only supports 1 tex-page for now
		const matTextureDX11* cur_page = dynamic_cast<const matTextureDX11*>(uva->GetPage(0));	
		DBG_ASSERT(cur_page, "UVA texture page is not a valid D3D texture.");

		return cur_page->GetSurface();
	}

	const matTextureDX11* textured3d = dynamic_cast<const matTextureDX11*>(i_pTexture);
	if( textured3d != NULL )
	{
		return textured3d->GetSurface();
	}
	return NULL;

}

//-----------------------------------------------------------------------------
// GetTextureRect - Get the dimensions of the texture
//-----------------------------------------------------------------------------
HRESULT g3dDX11TextureUtil::GetTextureRect( g2dD3D11TexturePtr pTexture, RECT* pRect )
{
    HRESULT hr = S_OK;

    if( pTexture == NULL || pRect == NULL )
        return E_INVALIDARG;

    D3D11_TEXTURE2D_DESC desc;
    pTexture->GetDesc( &desc );

    pRect->left = 0;
    pRect->top = 0;
    pRect->right = desc.Width;
    pRect->bottom = desc.Height;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GetTextureCoords()
// Desc: Get the texture coordinates to use when rendering into the destination
//       texture, given the source and destination rectangles
//-----------------------------------------------------------------------------
HRESULT g3dDX11TextureUtil::GetTextureCoords( matTexture* pTexSrc, RECT* pRectSrc, 
                          matTexture* pTexDest, RECT* pRectDest, CoordRect* pCoords )
{
    HRESULT hr = S_OK;
    float tU, tV;

    // Validate arguments
    if( pTexSrc == NULL || pTexDest == NULL || pCoords == NULL )
        return E_INVALIDARG;

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

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_GaussBlur5x5
// Desc: Get the texture coordinate offsets to be used inside the GaussBlur5x5
//       pixel shader.

// This used to be a 5x5 PIXEL filter. In which case the filterwidth is 4 pixels wide.
// -2  -1 0  1  2
//  |--+--o--+--|  5 samples located along total width which is 4 pixels. 
// 4 pixels wide is equal to 4/textureWidth.  So to get pixel sampling, use that width.
// However, the textures we are sampling are determined by the render resolution chosen.
// We want our results to scale with resolution, meaning we want to sample the same locations.
// Therefore, we need to keep width constant relative to the texture. 
// Assuming the code was tuned for a 5x5 pixel filter on a 640x480 image (4:3 aspect ratio),
// then the filter widths would be 4/640 and 4/480 respectively. Those fractions can be used
// on any image, since they represent a fraction of the image width and height.
//-----------------------------------------------------------------------------
HRESULT g3dDX11TextureUtil::GetSampleOffsets_GaussBlur5x5(float i_filterWidthX,
														 float i_filterWidthY, 
														 maVector2d* avTexCoordOffset,
														 maVector4d* avSampleWeight,
														 FLOAT fMultiplier )
{
	float tu = 0.25f * i_filterWidthX;
	float tv = 0.25f * i_filterWidthY;
//    float tu = 1.0f / (float)dwD3DTexWidth ;
//    float tv = 1.0f / (float)dwD3DTexHeight ;

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

    return S_OK;
}
