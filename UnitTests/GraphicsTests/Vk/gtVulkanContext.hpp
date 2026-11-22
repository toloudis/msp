/*****************************************************************************
**  gtVulkanContext.hpp
**
**      gtVulkanContext is gtVulkan plus what GraphicsVulkan's ported code
**      expects to find through vkdGlobal: bindings, a ring buffer, the
**      pipeline cache and a vkdContext. It also makes render targets and
**      textures and reads images back.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include "Vk/gtVulkan.hpp"

#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif

#include <memory>
#include <vector>

//	The images gtVulkanContext and gtVulkanSuite make, and their readback
namespace gtVulkanImages
{
	//	An R8G8B8A8 render target that can also be sampled, cleared and read back
	inline std::unique_ptr<gvk::vkdImage> Target(gvk::vkdDevice& i_Device, uint32_t i_Width, uint32_t i_Height)
	{
		return gvk::vkdImage::Create2D(i_Device, i_Width, i_Height, VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
			VK_IMAGE_USAGE_TRANSFER_DST_BIT, "gtVulkanContext target");
	}

	//	An R8G8B8A8 texture holding i_pRgba (rows tightly packed)
	inline std::unique_ptr<gvk::vkdImage> Texture(gvk::vkdDevice& i_Device, gvk::vkdContext& i_Context,
												  uint32_t i_Width, uint32_t i_Height, const uint8_t* i_pRgba)
	{
		std::unique_ptr<gvk::vkdImage> image = gvk::vkdImage::Create2D(i_Device, i_Width, i_Height, VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, "gtVulkanContext texture");
		gvk::vkdRingBuffer::Allocation staging = i_Context.Upload(i_pRgba, (VkDeviceSize)i_Width * i_Height * 4, 16);
		VkBufferImageCopy region = {};
		region.bufferOffset = staging.m_Offset;
		region.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		region.imageExtent = { i_Width, i_Height, 1 };
		vkCmdCopyBufferToImage(i_Context.Commands(), staging.m_Buffer, image->Image(), VK_IMAGE_LAYOUT_GENERAL, 1, &region);
		return image;
	}

	inline std::vector<uint8_t> Read(gvk::vkdContext& i_Context, gvk::vkdImage* i_pImage)
	{
		i_Context.Flush();
		return i_pImage->ReadBack();
	}
}

class gtVulkanContext : public gtVulkan
{
	protected:
		void SetUp() override
		{
			gtVulkan::SetUp();
			if (!m_pDevice)
			{
				return;
			}
			m_pBindings.reset(new gvk::vkdBindings(Device()));
			m_pRing.reset(new gvk::vkdRingBuffer(Device(), 4 << 20));
			m_pPipelines.reset(new gvk::vkdPipelineCache(Device(), *m_pBindings, ""));
			m_pContext.reset(new gvk::vkdContext(Device(), *m_pRing, *m_pBindings, *m_pPipelines));
			gvk::vkdGlobal::Set(m_pDevice.get(), m_pRing.get(), m_pBindings.get(), m_pPipelines.get(), m_pContext.get());
		}

		//	Subclasses release what they made from the context before this runs
		void TearDown() override
		{
			if (m_pDevice)
			{
				m_Images.clear();
				gvk::vkdGlobal::Set(NULL, NULL, NULL, NULL, NULL);
				m_pContext.reset();
				m_pPipelines.reset();
				m_pRing.reset();
				m_pBindings.reset();
			}
			gtVulkan::TearDown();
		}

		gvk::vkdContext& Context() { return *m_pContext; }

		//	An R8G8B8A8 render target that can also be sampled, cleared and read back
		gvk::vkdImage* Target(uint32_t i_Width, uint32_t i_Height)
		{
			m_Images.push_back(gtVulkanImages::Target(Device(), i_Width, i_Height));
			return m_Images.back().get();
		}

		//	An R8G8B8A8 texture holding i_pRgba (rows tightly packed)
		gvk::vkdImage* Texture(uint32_t i_Width, uint32_t i_Height, const uint8_t* i_pRgba)
		{
			m_Images.push_back(gtVulkanImages::Texture(Device(), *m_pContext, i_Width, i_Height, i_pRgba));
			return m_Images.back().get();
		}

		std::vector<uint8_t> Read(gvk::vkdImage* i_pImage)
		{
			return gtVulkanImages::Read(*m_pContext, i_pImage);
		}

		std::unique_ptr<gvk::vkdBindings>		m_pBindings;
		std::unique_ptr<gvk::vkdRingBuffer>		m_pRing;
		std::unique_ptr<gvk::vkdPipelineCache>	m_pPipelines;
		std::unique_ptr<gvk::vkdContext>		m_pContext;
		std::vector<std::unique_ptr<gvk::vkdImage>> m_Images;
};
