/*****************************************************************************
**  vkdInfo.hpp
**
**      Small helpers that fill common Vulkan structs with sensible defaults
**	and record common commands, so ported code stays short. They cost
**	nothing extra per call. Grown as the port shows what repeats.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDINFO_HPP
#error vkdInfo.hpp multiply included
#endif
#define GVK_VKDINFO_HPP

#include <volk.h>

namespace gvk
{
	inline VkImageSubresourceRange vkdRange(VkImageAspectFlags i_Aspect = VK_IMAGE_ASPECT_COLOR_BIT)
	{
		VkImageSubresourceRange range = {};
		range.aspectMask = i_Aspect;
		range.levelCount = VK_REMAINING_MIP_LEVELS;
		range.layerCount = VK_REMAINING_ARRAY_LAYERS;
		return range;
	}

	//	vkdFullBarrier - every earlier command's writes are visible to every
	//	later command (DX11's implicit ordering between commands).
	inline void vkdFullBarrier(VkCommandBuffer i_Commands)
	{
		VkMemoryBarrier2 barrier = {VK_STRUCTURE_TYPE_MEMORY_BARRIER_2};
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
		barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
		VkDependencyInfo dependency = {VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
		dependency.memoryBarrierCount = 1;
		dependency.pMemoryBarriers = &barrier;
		vkCmdPipelineBarrier2(i_Commands, &dependency);
	}

	//	vkdImageToGeneral - the one layout transition an image needs with
	//	VK_KHR_unified_image_layouts: from UNDEFINED (contents discarded) to
	//	GENERAL, where it stays.
	inline void vkdImageToGeneral(VkCommandBuffer i_Commands, VkImage i_Image,
								  VkImageAspectFlags i_Aspect = VK_IMAGE_ASPECT_COLOR_BIT)
	{
		VkImageMemoryBarrier2 barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;
		barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = i_Image;
		barrier.subresourceRange = vkdRange(i_Aspect);
		VkDependencyInfo dependency = {VK_STRUCTURE_TYPE_DEPENDENCY_INFO};
		dependency.imageMemoryBarrierCount = 1;
		dependency.pImageMemoryBarriers = &barrier;
		vkCmdPipelineBarrier2(i_Commands, &dependency);
	}
}
