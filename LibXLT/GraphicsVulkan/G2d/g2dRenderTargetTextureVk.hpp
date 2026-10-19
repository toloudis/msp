/****************************************************************************\
**  g2dRenderTargetTextureVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g2dRenderTargetTextureDX11: a
**	colour image that is rendered to and sampled (and copied, cleared and
**	read back), multisampled or with a full mip chain if asked.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G2DRENDERTARGETTEXTUREVK_HPP
#error g2dRenderTargetTextureVk.hpp multiply included
#endif
#define GVK_G2DRENDERTARGETTEXTUREVK_HPP

#ifndef GVK_G2DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g2dStubsVk.hpp"
#endif

#include <volk.h>

#include <cstdint>
#include <memory>

namespace gvk
{
	class vkdImage;

	class g2dRenderTargetTextureVk
	{
	public:
		g2dRenderTargetTextureVk();
		virtual ~g2dRenderTargetTextureVk();

		//--------------------------------------------------------------------
		//	Make makes the surface into one with the given dimensions and
		//	pixel format.
		//--------------------------------------------------------------------
		void Make(int i_Width, int i_Height, uint32_t i_DxgiFormat, VkSampleCountFlagBits i_Multisample,
			g2dResourceCounterVk::eResourceCategory i_Category = g2dResourceCounterVk::eRenderTarget,
			bool i_bGenerateMips = false );

		//	The view rendered to (mip 0) and the one shaders read (all mips)
		vkdImage* GetRenderTargetView() const;
		vkdImage* GetImage() const { return m_pImage.get(); }

		int GetWidth() const {return m_Width;}
		int GetHeight() const {return m_Height;}
		int GetMipLevels() const {return m_Mips;}
		uint32_t GetFormat() const {return m_Format;}
		VkSampleCountFlagBits GetMultisample() const {return m_Multisample;}

	private:
		g2dResourceCounterVk::eResourceCategory m_Category;
		short m_Width, m_Height, m_Mips;
		uint32_t m_Format;
		VkSampleCountFlagBits m_Multisample;
		std::unique_ptr<vkdImage> m_pImage;
	};
}
