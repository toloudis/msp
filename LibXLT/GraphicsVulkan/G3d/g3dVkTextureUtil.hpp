/****************************************************************************\
**  g3dVkTextureUtil.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g3dDX11TextureUtil: the image of
**	a Terawatt texture (GetD3DTexture's counterpart), and the texture
**	rectangles and blur offsets of the HDR renderer's tone mapping.
**	TexRect stands in for Windows' RECT, with SetRect and InflateRect.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G3DVKTEXTUREUTIL_HPP
#error g3dVkTextureUtil.hpp multiply included
#endif
#define GVK_G3DVKTEXTUREUTIL_HPP

struct effOpacity;
class matTexture;
class maVector2d;
class maVector4d;

namespace gvk
{
	class vkdImage;

	// Texture coordinate rectangle
	struct CoordRect
	{
		float fLeftU, fTopV;
		float fRightU, fBottomV;
	};

	namespace g3dVkTextureUtil
	{
		//------------------------------------------------------------------------
		// GetImage - the image shaders read for a Terawatt texture (the first
		// page of a UVA texture); NULL if it has none
		//------------------------------------------------------------------------
		vkdImage* GetImage(const matTexture* i_pTexture);

		//------------------------------------------------------------------------
		// GetOpacityImage - the image a pass binds as i_Opacity's map: NULL
		// without one; read through its alpha view for a PBR base color map
		//------------------------------------------------------------------------
		vkdImage* GetOpacityImage(const effOpacity& i_Opacity);

		//------------------------------------------------------------------------
		// TexRect - Windows' RECT (right and bottom exclusive), with its
		// SetRect and InflateRect
		//------------------------------------------------------------------------
		struct TexRect
		{
			int left, top, right, bottom;
		};
		void SetRect(TexRect* o_pRect, int i_Left, int i_Top, int i_Right, int i_Bottom);
		void InflateRect(TexRect* io_pRect, int i_Dx, int i_Dy);

		//-----------------------------------------------------------------------------
		// Name: GetTextureCoords()
		// Desc: Get the texture coordinates to use when rendering into the destination
		//       texture, given the source and destination rectangles
		//-----------------------------------------------------------------------------
		bool GetTextureCoords( matTexture* pTexSrc, TexRect* pRectSrc,
							   matTexture* pTexDest, TexRect* pRectDest, CoordRect* pCoords );

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
		bool GetSampleOffsets_GaussBlur5x5(float i_filterWidthX,
										   float i_filterWidthY,
										   maVector2d* avTexCoordOffset,
										   maVector4d* avSampleWeight,
										   float fMultiplier = 1);
	}
}
