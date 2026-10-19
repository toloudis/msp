/*****************************************************************************
**  vkdSwapchain.cpp
**
**      See vkdSwapchain.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDSWAPCHAIN_HPP
#include "GraphicsVulkan/vkd/vkdSwapchain.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef GVK_VKDINFO_HPP
#include "GraphicsVulkan/vkd/vkdInfo.hpp"
#endif
#ifndef GVK_VKDSURFACE_HPP
#include "GraphicsVulkan/vkd/vkdSurface.hpp"
#endif
#ifndef GVK_VKDVMA_HPP
#include "GraphicsVulkan/vkd/private/vkdVma.hpp"
#endif

#include <algorithm>
#include <cstring>

namespace gvk
{
	namespace
	{
		const char* present_mode_name(VkPresentModeKHR i_Mode)
		{
			switch (i_Mode)
			{
				case VK_PRESENT_MODE_IMMEDIATE_KHR:	return "IMMEDIATE";
				case VK_PRESENT_MODE_MAILBOX_KHR:	return "MAILBOX";
				case VK_PRESENT_MODE_FIFO_KHR:		return "FIFO";
				default:							return "other";
			}
		}

		//	One image barrier with explicit scopes.
		void image_barrier(VkCommandBuffer i_Commands, VkImage i_Image,
						   VkPipelineStageFlags2 i_SrcStage, VkAccessFlags2 i_SrcAccess, VkImageLayout i_Old,
						   VkPipelineStageFlags2 i_DstStage, VkAccessFlags2 i_DstAccess, VkImageLayout i_New)
		{
			VkImageMemoryBarrier2 barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
			barrier.srcStageMask = i_SrcStage;
			barrier.srcAccessMask = i_SrcAccess;
			barrier.dstStageMask = i_DstStage;
			barrier.dstAccessMask = i_DstAccess;
			barrier.oldLayout = i_Old;
			barrier.newLayout = i_New;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = i_Image;
			barrier.subresourceRange = vkdRange();
			VkDependencyInfo dependency = {VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
			dependency.imageMemoryBarrierCount = 1;
			dependency.pImageMemoryBarriers = &barrier;
			vkCmdPipelineBarrier2(i_Commands, &dependency);
		}

		VkSemaphore new_semaphore(vkdDevice& i_Device, const std::string& i_Name)
		{
			VkSemaphoreCreateInfo info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
			VkSemaphore semaphore = VK_NULL_HANDLE;
			VK_CHECK(vkCreateSemaphore(i_Device.Device(), &info, NULL, &semaphore));
			i_Device.SetName(VK_OBJECT_TYPE_SEMAPHORE, reinterpret_cast<uint64_t>(semaphore), i_Name.c_str());
			return semaphore;
		}
	}

	vkdSwapchain::vkdSwapchain(vkdDevice& i_Device, void* i_NativeWindow, const std::string& i_Name)
		: m_Device(i_Device)
		, m_pWindow(i_NativeWindow)
		, m_Name(i_Name)
	{
		m_Surface = vkdCreateSurface(i_Device.Instance(), i_NativeWindow);
		if (!m_Surface)
		{
			DBG_LOG("vkdSwapchain " << m_Name << ": no Vulkan surface for the window");
			return;
		}
		VkBool32 supported = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(i_Device.Physical(), i_Device.QueueFamily(), m_Surface, &supported);
		m_bCanPresent = supported == VK_TRUE;
		if (!m_bCanPresent)
		{
			DBG_LOG("vkdSwapchain " << m_Name << ": the queue can't present to this window");
		}
	}

	vkdSwapchain::~vkdSwapchain()
	{
		m_Device.WaitIdle();
		destroy_images();
		for (VkSemaphore semaphore : m_AcquireSemaphores)
		{
			vkDestroySemaphore(m_Device.Device(), semaphore, NULL);
		}
		if (m_Swapchain)
		{
			vkDestroySwapchainKHR(m_Device.Device(), m_Swapchain, NULL);
		}
		if (m_Surface)
		{
			vkDestroySurfaceKHR(m_Device.Instance(), m_Surface, NULL);
		}
	}

	void vkdSwapchain::destroy_images()
	{
		m_Targets.clear();
		for (VkImageView view : m_Views)
		{
			vkDestroyImageView(m_Device.Device(), view, NULL);
		}
		for (VkSemaphore semaphore : m_RenderDone)
		{
			vkDestroySemaphore(m_Device.Device(), semaphore, NULL);
		}
		m_Views.clear();
		m_RenderDone.clear();
		m_Images.clear();
	}

	VkImage vkdSwapchain::Image() const
	{
		return m_bAcquired ? m_Images[m_Current] : VK_NULL_HANDLE;
	}

	VkImageView vkdSwapchain::View() const
	{
		return m_bAcquired ? m_Views[m_Current] : VK_NULL_HANDLE;
	}

	vkdImage* vkdSwapchain::Target() const
	{
		return m_bAcquired ? m_Targets[m_Current].get() : nullptr;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool vkdSwapchain::create()
	{
		VkPhysicalDevice physical = m_Device.Physical();
		VkSurfaceCapabilitiesKHR caps;
		VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, m_Surface, &caps));
		VkExtent2D extent = caps.currentExtent;
		if (extent.width == 0xFFFFFFFF)
		{
			int width = 0, height = 0;
			vkdWindowClientSize(m_pWindow, width, height);
			extent.width = std::clamp(static_cast<uint32_t>(width), caps.minImageExtent.width, caps.maxImageExtent.width);
			extent.height = std::clamp(static_cast<uint32_t>(height), caps.minImageExtent.height, caps.maxImageExtent.height);
		}
		if (extent.width == 0 || extent.height == 0)
		{
			return false;
		}

		uint32_t count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical, m_Surface, &count, NULL);
		std::vector<VkSurfaceFormatKHR> formats(count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical, m_Surface, &count, formats.data());
		VkSurfaceFormatKHR chosen = formats.empty() ? VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR} : formats[0];
		for (VkFormat wanted : {VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM})
		{
			auto found = std::find_if(formats.begin(), formats.end(), [wanted](const VkSurfaceFormatKHR& f)
				{ return f.format == wanted && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; });
			if (found != formats.end())
			{
				chosen = *found;
				break;
			}
		}

		vkGetPhysicalDeviceSurfacePresentModesKHR(physical, m_Surface, &count, NULL);
		std::vector<VkPresentModeKHR> modes(count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical, m_Surface, &count, modes.data());
		VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;
		for (VkPresentModeKHR wanted : {VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_MAILBOX_KHR})
		{
			if (std::find(modes.begin(), modes.end(), wanted) != modes.end())
			{
				mode = wanted;
				break;
			}
		}

		uint32_t image_count = std::max(caps.minImageCount + 1, 2u);
		if (caps.maxImageCount > 0)
		{
			image_count = std::min(image_count, caps.maxImageCount);
		}
		VkCompositeAlphaFlagBitsKHR alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		if (!(caps.supportedCompositeAlpha & alpha))
		{
			alpha = static_cast<VkCompositeAlphaFlagBitsKHR>(caps.supportedCompositeAlpha & ~(caps.supportedCompositeAlpha - 1));
		}

		//	The old swap chain's images may still be in use.
		m_Device.WaitIdle();
		destroy_images();

		VkSwapchainCreateInfoKHR info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
		info.surface = m_Surface;
		info.minImageCount = image_count;
		info.imageFormat = chosen.format;
		info.imageColorSpace = chosen.colorSpace;
		info.imageExtent = extent;
		info.imageArrayLayers = 1;
		info.imageUsage = (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
						   VK_IMAGE_USAGE_TRANSFER_SRC_BIT) & caps.supportedUsageFlags;
		info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.preTransform = caps.currentTransform;
		info.compositeAlpha = alpha;
		info.presentMode = mode;
		info.clipped = VK_TRUE;
		info.oldSwapchain = m_Swapchain;
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		VK_CHECK(vkCreateSwapchainKHR(m_Device.Device(), &info, NULL, &swapchain));
		if (m_Swapchain)
		{
			vkDestroySwapchainKHR(m_Device.Device(), m_Swapchain, NULL);
		}
		m_Swapchain = swapchain;
		m_Device.SetName(VK_OBJECT_TYPE_SWAPCHAIN_KHR, reinterpret_cast<uint64_t>(m_Swapchain), m_Name.c_str());

		const bool first = m_Format == VK_FORMAT_UNDEFINED;
		m_Format = chosen.format;
		m_ColorSpace = chosen.colorSpace;
		m_PresentMode = mode;
		m_Extent = extent;

		vkGetSwapchainImagesKHR(m_Device.Device(), m_Swapchain, &count, NULL);
		m_Images.resize(count);
		vkGetSwapchainImagesKHR(m_Device.Device(), m_Swapchain, &count, m_Images.data());
		for (uint32_t i = 0; i < count; ++i)
		{
			const std::string name = m_Name + " image " + std::to_string(i);
			m_Device.SetName(VK_OBJECT_TYPE_IMAGE, reinterpret_cast<uint64_t>(m_Images[i]), name.c_str());
			VkImageViewCreateInfo view = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
			view.image = m_Images[i];
			view.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view.format = m_Format;
			view.subresourceRange = vkdRange();
			VkImageView image_view = VK_NULL_HANDLE;
			VK_CHECK(vkCreateImageView(m_Device.Device(), &view, NULL, &image_view));
			m_Views.push_back(image_view);
			vkdImageDesc desc;
			desc.m_Format = m_Format;
			desc.m_Width = extent.width;
			desc.m_Height = extent.height;
			desc.m_Usage = info.imageUsage;
			m_Targets.push_back(vkdImage::Wrap(m_Device, m_Images[i], image_view, desc));
			m_RenderDone.push_back(new_semaphore(m_Device, m_Name + " render done " + std::to_string(i)));
		}
		while (m_AcquireSemaphores.size() < count + 1)
		{
			m_AcquireSemaphores.push_back(new_semaphore(m_Device, m_Name + " acquire " + std::to_string(m_AcquireSemaphores.size())));
			m_AcquireValues.push_back(0);
		}
		if (first)
		{
			DBG_LOG("vkdSwapchain " << m_Name << ": " << (m_Format == VK_FORMAT_R8G8B8A8_UNORM ? "R8G8B8A8_UNORM" : "B8G8R8A8_UNORM")
					<< ", " << present_mode_name(m_PresentMode) << ", " << count << " images");
		}
		return true;
	}

	bool vkdSwapchain::Acquire()
	{
		if (m_bAcquired)
		{
			return true;
		}
		if (!IsValid())
		{
			return false;
		}
		int width = 0, height = 0;
		vkdWindowClientSize(m_pWindow, width, height);
		if (width <= 0 || height <= 0)
		{
			m_bRecreate = true;			// size it again once the window has area
			return false;
		}
		if ((m_bRecreate || !m_Swapchain) && !create())
		{
			return false;
		}
		m_bRecreate = false;

		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		for (int attempt = 0; attempt < 2; ++attempt)
		{
			const uint32_t slot = m_NextAcquire;
			m_Device.WaitFor(m_AcquireValues[slot]);
			VkResult result = vkAcquireNextImageKHR(m_Device.Device(), m_Swapchain, UINT64_MAX,
													m_AcquireSemaphores[slot], VK_NULL_HANDLE, &m_Current);
			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				if (!create())
				{
					return false;
				}
				continue;
			}
			if (result == VK_SUBOPTIMAL_KHR)
			{
				m_bRecreate = true;
			}
			else if (VK_CHECK(result) != VK_SUCCESS)
			{
				return false;
			}
			m_AcquireUsed = slot;
			m_NextAcquire = (slot + 1) % static_cast<uint32_t>(m_AcquireSemaphores.size());
			m_bAcquired = true;
			//	After the acquire wait (ALL_COMMANDS in Present's submit): to
			//	GENERAL, discarding the old contents.
			image_barrier(m_Device.Commands(), m_Images[m_Current],
						  VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, VK_ACCESS_2_NONE, VK_IMAGE_LAYOUT_UNDEFINED,
						  VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT,
						  VK_IMAGE_LAYOUT_GENERAL);
			return true;
		}
		return false;
	}

	void vkdSwapchain::Present()
	{
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		if (!Acquire())
		{
			return;
		}
		VkCommandBuffer commands = m_Device.Commands();

		VkBuffer readback = VK_NULL_HANDLE;
		VmaAllocation readback_allocation = NULL;
		VmaAllocationInfo readback_info = {};
		const VkDeviceSize readback_size = static_cast<VkDeviceSize>(m_Extent.width) * m_Extent.height * 4;
		const bool read_back = m_bReadBack;
		if (read_back)
		{
			VkBufferCreateInfo buffer = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
			buffer.size = readback_size;
			buffer.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			VmaAllocationCreateInfo allocation = {};
			allocation.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
			allocation.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
			VK_CHECK(vmaCreateBuffer(m_Device.Allocator(), &buffer, &allocation, &readback, &readback_allocation, &readback_info));
			vkdFullBarrier(commands);
			VkBufferImageCopy region = {};
			region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
			region.imageExtent = {m_Extent.width, m_Extent.height, 1};
			vkCmdCopyImageToBuffer(commands, m_Images[m_Current], VK_IMAGE_LAYOUT_GENERAL, readback, 1, &region);
		}

		image_barrier(commands, m_Images[m_Current],
					  VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, VK_ACCESS_2_MEMORY_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL,
					  VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT, VK_ACCESS_2_NONE, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		VkSemaphoreSubmitInfo wait = {VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO};
		wait.semaphore = m_AcquireSemaphores[m_AcquireUsed];
		wait.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		VkSemaphoreSubmitInfo signal = {VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO};
		signal.semaphore = m_RenderDone[m_Current];
		signal.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		const uint64_t value = m_Device.Submit({wait}, {signal});
		m_AcquireValues[m_AcquireUsed] = value;

		VkPresentInfoKHR present = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
		present.waitSemaphoreCount = 1;
		present.pWaitSemaphores = &m_RenderDone[m_Current];
		present.swapchainCount = 1;
		present.pSwapchains = &m_Swapchain;
		present.pImageIndices = &m_Current;
		VkResult result;
		{
			std::lock_guard<std::mutex> queue_lock(m_Device.QueueLock());
			result = vkQueuePresentKHR(m_Device.Queue(), &present);
		}
		m_bAcquired = false;
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			m_bRecreate = true;
		}
		else
		{
			VK_CHECK(result);
		}

		if (read_back)
		{
			m_Device.WaitFor(value);
			VK_CHECK(vmaInvalidateAllocation(m_Device.Allocator(), readback_allocation, 0, VK_WHOLE_SIZE));
			m_Pixels.resize(static_cast<size_t>(readback_size));
			memcpy(m_Pixels.data(), readback_info.pMappedData, m_Pixels.size());
			vmaDestroyBuffer(m_Device.Allocator(), readback, readback_allocation);
			m_bReadBack = false;
		}
	}
}
