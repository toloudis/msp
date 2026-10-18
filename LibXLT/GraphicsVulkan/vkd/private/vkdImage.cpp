/*****************************************************************************
**  vkdImage.cpp
**
**      See vkdImage.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef GVK_VKDINFO_HPP
#include "GraphicsVulkan/vkd/vkdInfo.hpp"
#endif
#ifndef GVK_VKDVMA_HPP
#include "GraphicsVulkan/vkd/private/vkdVma.hpp"
#endif

#include <cstring>

namespace gvk
{
	uint32_t vkdFormatSize(VkFormat i_Format)
	{
		switch (i_Format)
		{
			case VK_FORMAT_R8_UNORM:				return 1;
			case VK_FORMAT_R8G8B8A8_UNORM:
			case VK_FORMAT_B8G8R8A8_UNORM:
			case VK_FORMAT_R8G8B8A8_SRGB:
			case VK_FORMAT_B8G8R8A8_SRGB:
			case VK_FORMAT_R32_SFLOAT:
			case VK_FORMAT_R32_UINT:				return 4;
			case VK_FORMAT_R16G16B16A16_SFLOAT:		return 8;
			case VK_FORMAT_R32G32B32A32_SFLOAT:		return 16;
			default:								return 0;
		}
	}

	std::unique_ptr<vkdImage> vkdImage::Create2D(vkdDevice& i_Device, uint32_t i_Width, uint32_t i_Height,
												 VkFormat i_Format, VkImageUsageFlags i_Usage, const char* i_Name,
												 VkImageAspectFlags i_Aspect)
	{
		std::unique_ptr<vkdImage> image(new vkdImage(i_Device));
		image->m_Format = i_Format;
		image->m_Aspect = i_Aspect;
		image->m_Width = i_Width;
		image->m_Height = i_Height;

		VkImageCreateInfo info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
		info.imageType = VK_IMAGE_TYPE_2D;
		info.format = i_Format;
		info.extent = {i_Width, i_Height, 1};
		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = i_Usage;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VmaAllocationCreateInfo allocation = {};
		allocation.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		VK_CHECK(vmaCreateImage(i_Device.Allocator(), &info, &allocation, &image->m_Image, &image->m_Allocation, NULL));
		i_Device.SetName(VK_OBJECT_TYPE_IMAGE, reinterpret_cast<uint64_t>(image->m_Image), i_Name);

		//	A view only where the usage allows one (transfer-only images have none)
		const VkImageUsageFlags c_ViewUsages = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
			VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		if (i_Usage & c_ViewUsages)
		{
			VkImageViewCreateInfo view = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
			view.image = image->m_Image;
			view.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view.format = i_Format;
			view.subresourceRange = vkdRange(i_Aspect);
			VK_CHECK(vkCreateImageView(i_Device.Device(), &view, NULL, &image->m_View));
			i_Device.SetName(VK_OBJECT_TYPE_IMAGE_VIEW, reinterpret_cast<uint64_t>(image->m_View), i_Name);
		}

		std::lock_guard<std::recursive_mutex> lock(i_Device.StreamLock());
		vkdImageToGeneral(i_Device.Commands(), image->m_Image, i_Aspect);
		return image;
	}

	vkdImage::~vkdImage()
	{
		VkDevice device = m_Device.Device();
		VmaAllocator allocator = m_Device.Allocator();
		VkImage image = m_Image;
		VkImageView view = m_View;
		VmaAllocation allocation = m_Allocation;
		m_Device.DeferDestroy([device, allocator, image, view, allocation]()
		{
			if (view)
			{
				vkDestroyImageView(device, view, NULL);
			}
			vmaDestroyImage(allocator, image, allocation);
		});
	}

	std::vector<uint8_t> vkdImage::ReadBack()
	{
		const uint32_t pixel = vkdFormatSize(m_Format);
		DBG_ASSERT(pixel != 0, "vkdImage::ReadBack: unsupported format " << m_Format);
		const VkDeviceSize size = static_cast<VkDeviceSize>(m_Width) * m_Height * pixel;

		VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
		buffer_info.size = size;
		buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		VmaAllocationCreateInfo allocation_info = {};
		allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		allocation_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation allocation = NULL;
		VmaAllocationInfo mapped = {};
		VK_CHECK(vmaCreateBuffer(m_Device.Allocator(), &buffer_info, &allocation_info, &buffer, &allocation, &mapped));

		uint64_t value = 0;
		{
			std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
			VkCommandBuffer commands = m_Device.Commands();
			vkdFullBarrier(commands);
			VkBufferImageCopy region = {};
			region.imageSubresource = {m_Aspect, 0, 0, 1};
			region.imageExtent = {m_Width, m_Height, 1};
			vkCmdCopyImageToBuffer(commands, m_Image, VK_IMAGE_LAYOUT_GENERAL, buffer, 1, &region);
			VkMemoryBarrier2 to_host = {VK_STRUCTURE_TYPE_MEMORY_BARRIER_2};
			to_host.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
			to_host.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
			to_host.dstStageMask = VK_PIPELINE_STAGE_2_HOST_BIT;
			to_host.dstAccessMask = VK_ACCESS_2_HOST_READ_BIT;
			VkDependencyInfo dependency = {VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
			dependency.memoryBarrierCount = 1;
			dependency.pMemoryBarriers = &to_host;
			vkCmdPipelineBarrier2(commands, &dependency);
			value = m_Device.Submit();
		}
		m_Device.WaitFor(value);

		VK_CHECK(vmaInvalidateAllocation(m_Device.Allocator(), allocation, 0, VK_WHOLE_SIZE));
		std::vector<uint8_t> pixels(static_cast<size_t>(size));
		memcpy(pixels.data(), mapped.pMappedData, pixels.size());
		vmaDestroyBuffer(m_Device.Allocator(), buffer, allocation);
		return pixels;
	}
}
