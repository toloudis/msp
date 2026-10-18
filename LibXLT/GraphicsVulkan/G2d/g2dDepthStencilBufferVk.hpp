/*****************************************************************************
**  g2dDepthStencilBufferVk.hpp
**
**      g2dDepthStencilBufferVk is a depth/stencil image (D24_UNORM_S8_UINT,
**	as DX11's D24S8) for a window or render target.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G2DDEPTHSTENCILBUFFERVK_HPP
#error g2dDepthStencilBufferVk.hpp multiply included
#endif
#define GVK_G2DDEPTHSTENCILBUFFERVK_HPP

#ifndef G2D_DEPTHSTENCILBUFFER_HPP
#include "Graphics/G2d/g2dDepthStencilBuffer.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif

#include <memory>

namespace gvk
{
	class g2dDepthStencilBufferVk : public g2dDepthStencilBuffer
	{
		public:
			g2dDepthStencilBufferVk(vkdDevice& i_Device, uint32_t i_Width, uint32_t i_Height)
				: m_pImage(vkdImage::Create2D(i_Device, i_Width, i_Height, VK_FORMAT_D24_UNORM_S8_UINT,
											  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
											  "depth-stencil", VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
			{
			}

			vkdImage& Image() { return *m_pImage; }

		private:
			std::unique_ptr<vkdImage> m_pImage;
	};
}
