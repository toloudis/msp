/****************************************************************************\
**  matTextureVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11's matTextureDX11: the base of the
**	Vulkan texture classes. GetImage is DX11's GetSurface and GetResource
**	together: the vkdImage (a view of all of it) shaders read and copies
**	use.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_MATTEXTUREVK_HPP
#error matTextureVk.hpp multiply included
#endif
#define GVK_MATTEXTUREVK_HPP

#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif

#include <cstdint>

namespace gvk
{
	class vkdImage;

	class matTextureVk : public matTexture
	{
	public:

		//--------------------------------------------------------------------
		//	This constructor makes an "empty" surface.
		//--------------------------------------------------------------------
		matTextureVk();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~matTextureVk();

		//--------------------------------------------------------------------
		//	GetImage returns the image, usable for shaders (NULL if none).
		//--------------------------------------------------------------------
		virtual vkdImage* GetImage() const = 0;

		//--------------------------------------------------------------------
		//	MakeTransparencyMap will build a transparency map for this texture
		//	i_Bias specifies the range (0-i_Bias) that will be considered transparent
		//--------------------------------------------------------------------
		char *MakeTransparencyMap(float i_Bias, const g2dPFD& i_PFD, int i_Width, int i_Height);

		//--------------------------------------------------------------------
		//	Upload copies one subresource from the CPU, as D3D11 initial data
		//	or UpdateSubresource: rows tightly packed, the pitch from the
		//	format's bits per pixel. B8G8R8X8 data gets alpha 255, so the
		//	B8G8R8A8 image vkdFormat uses for it reads as D3D reads X8.
		//--------------------------------------------------------------------
		static void Upload(vkdImage* io_pImage, uint32_t i_DxgiFormat, const void* i_pData, int i_Width, int i_Height,
						   int i_Depth = 1, uint32_t i_Mip = 0, uint32_t i_Layer = 0);
	};
}
