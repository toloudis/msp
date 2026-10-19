/****************************************************************************\
**  g2dDepthStencilBufferVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g2dDepthStencilBufferDX11: a
**	D24_UNORM_S8_UINT depth/stencil image (DX11 makes R24G8_TYPELESS and
**	ignores the format it is given, and so does this) for a window or
**	render target, whose depth shaders can read (GetDepthShaderResource,
**	DX11's R24_UNORM_X8 view).
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G2DDEPTHSTENCILBUFFERVK_HPP
#error g2dDepthStencilBufferVk.hpp multiply included
#endif
#define GVK_G2DDEPTHSTENCILBUFFERVK_HPP

#ifndef G2D_DEPTHSTENCILBUFFER_HPP
#include "Graphics/G2d/g2dDepthStencilBuffer.hpp"
#endif
#ifndef GVK_G2DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g2dStubsVk.hpp"
#endif

#include <volk.h>

#include <cstdint>
#include <memory>

namespace gvk
{
	class vkdDevice;
	class vkdImage;

	class g2dDepthStencilBufferVk : public g2dDepthStencilBuffer
	{
	public:
		//	i_pDevice: the device to make the image on (NULL: vkdGlobal's);
		//	windows pass theirs
		explicit g2dDepthStencilBufferVk(vkdDevice* i_pDevice = NULL);
		virtual ~g2dDepthStencilBufferVk();

		//--------------------------------------------------------------------
		//	Make makes the surface into one with the given dimensions and
		//	pixel format.
		//--------------------------------------------------------------------
		void Make(int i_Width, int i_Height, uint32_t i_DxgiFormat, VkSampleCountFlagBits i_Multisample,
			g2dResourceCounterVk::eResourceCategory i_Category = g2dResourceCounterVk::eRenderTarget);

		// Access to the primary texture and it's views
		vkdImage* GetDepthView() const { return m_pDepthTexture.get(); }
		vkdImage* GetDepthShaderResource() const;

		//will be 1 for non MSAA targets otherwise > 1
		int GetMultisampleCount() const { return (int)m_Multisample; }

	private:
		g2dResourceCounterVk::eResourceCategory m_Category;
		short m_Width, m_Height;
		uint32_t m_Format;
		VkSampleCountFlagBits m_Multisample;
		vkdDevice* m_pDevice;
		std::unique_ptr<vkdImage> m_pDepthTexture;
	};
}
