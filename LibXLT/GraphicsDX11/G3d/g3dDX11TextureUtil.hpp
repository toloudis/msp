/*****************************************************************************
**  g3dDX11TextureUtil.hpp
**
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_DX11TEXTUREUTIL_HPP
#error g3dDX11TextureUtil.hpp multiply included
#endif
#define G3D_DX11TEXTUREUTIL_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matTexture;

// Texture coordinate rectangle
struct CoordRect
{
	float fLeftU, fTopV;
	float fRightU, fBottomV;
};
class maVector2d;
class maVector4d;

namespace g3dDX11TextureUtil
{
	//------------------------------------------------------------------------
	// GetD3DTexture - return d3d texture for Terawatt texture
	//------------------------------------------------------------------------
	ID3D11ShaderResourceView* GetD3DTexture(const matTexture* i_pTexture);

	//-----------------------------------------------------------------------------
	// GetTextureRect - Get the dimensions of the texture
	//-----------------------------------------------------------------------------
	HRESULT GetTextureRect( g2dD3D11TexturePtr pTexture, RECT* pRect );

	//-----------------------------------------------------------------------------
	// Name: GetTextureCoords()
	// Desc: Get the texture coordinates to use when rendering into the destination
	//       texture, given the source and destination rectangles
	//-----------------------------------------------------------------------------
	HRESULT GetTextureCoords( matTexture* pTexSrc, RECT* pRectSrc, 
							 matTexture* pTexDest, RECT* pRectDest, CoordRect* pCoords );

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
	HRESULT GetSampleOffsets_GaussBlur5x5(float i_filterWidthX,
										  float i_filterWidthY, 
										  maVector2d* avTexCoordOffset,
										  maVector4d* avSampleWeight,
										  FLOAT fMultiplier = 1);

}
