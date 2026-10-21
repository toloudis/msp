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
#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif
#ifndef GVK_VKDINFO_HPP
#include "GraphicsVulkan/vkd/vkdInfo.hpp"
#endif
#ifndef GVK_VKDVMA_HPP
#include "GraphicsVulkan/vkd/private/vkdVma.hpp"
#endif

#include <algorithm>
#include <cstring>
#include <numeric>

namespace gvk
{
	namespace
	{
		//	A view only where the usage allows one (transfer-only images have none)
		const VkImageUsageFlags c_ViewUsages = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
			VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

		uint32_t mip_size(uint32_t i_Size, uint32_t i_Mip)
		{
			return std::max(1u, i_Size >> i_Mip);
		}

		//	The layout of one subresource, tightly packed: bytes per row of
		//	pixels (or blocks), rows, and bytes per depth slice
		struct Packing
		{
			VkExtent3D	m_Extent;
			size_t		m_RowBytes;
			uint32_t	m_Rows;
			size_t		m_SliceBytes;
			size_t		m_Bytes;
			VkImageAspectFlags m_Aspect;		// the one aspect a copy uses
		};

		Packing packing(const vkdImageDesc& i_Desc, uint32_t i_Mip)
		{
			Packing p;
			p.m_Extent = { mip_size(i_Desc.m_Width, i_Mip), mip_size(i_Desc.m_Height, i_Mip), mip_size(i_Desc.m_Depth, i_Mip) };
			const vkdFormatInfo info = vkdFormatInfoOf(i_Desc.m_Format);
			uint32_t bytes = info.m_BlockBytes;
			p.m_Aspect = VK_IMAGE_ASPECT_COLOR_BIT;
			if (info.m_bDepth)
			{
				//	Copies of the depth aspect: D16 is 2 bytes, D24 and D32 4
				p.m_Aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
				bytes = i_Desc.m_Format == VK_FORMAT_D16_UNORM ? 2 : 4;
			}
			else if (info.m_bStencil)
			{
				p.m_Aspect = VK_IMAGE_ASPECT_STENCIL_BIT;
			}
			p.m_RowBytes = (size_t)(p.m_Extent.width + info.m_BlockWidth - 1) / info.m_BlockWidth * bytes;
			p.m_Rows = (p.m_Extent.height + info.m_BlockHeight - 1) / info.m_BlockHeight;
			p.m_SliceBytes = p.m_RowBytes * p.m_Rows;
			p.m_Bytes = p.m_SliceBytes * p.m_Extent.depth;
			return p;
		}

		//	Buffer offsets of copies must be a multiple of the texel block
		//	size (and of 4 for depth/stencil)
		VkDeviceSize copy_alignment(VkFormat i_Format)
		{
			const VkDeviceSize block = std::max(1u, vkdFormatInfoOf(i_Format).m_BlockBytes);
			return std::lcm(block, (VkDeviceSize)16);
		}
	}

	uint32_t vkdFormatSize(VkFormat i_Format)
	{
		const vkdFormatInfo info = vkdFormatInfoOf(i_Format);
		return info.m_bCompressed ? 0 : info.m_BlockBytes;
	}

	std::unique_ptr<vkdImage> vkdImage::Create(vkdDevice& i_Device, const vkdImageDesc& i_Desc, const char* i_Name)
	{
		DBG_ASSERT(vkdFormatInfoOf(i_Desc.m_Format).m_BlockBytes != 0, "vkdImage::Create: unknown format " << i_Desc.m_Format);
		DBG_ASSERT(!i_Desc.m_bCube || i_Desc.m_Layers % 6 == 0, "vkdImage::Create: a cube needs 6 layers per cube");
		std::unique_ptr<vkdImage> image(new vkdImage(i_Device));
		image->m_Desc = i_Desc;

		VkImageCreateInfo info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
		if (i_Desc.m_bCube)
		{
			info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		}
		DBG_ASSERT(!i_Desc.m_bLayeredTarget || (i_Desc.m_Type == VK_IMAGE_TYPE_3D && (i_Desc.m_Usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)),
				   "vkdImage::Create: m_bLayeredTarget is for 3D colour attachments");
		if (i_Desc.m_bLayeredTarget)
		{
			//	Rendering into slices, as DX11's render target views of volume textures
			info.flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
		}
		info.imageType = i_Desc.m_Type;
		info.format = i_Desc.m_Format;
		info.extent = { i_Desc.m_Width, i_Desc.m_Height, i_Desc.m_Depth };
		info.mipLevels = i_Desc.m_Mips;
		info.arrayLayers = i_Desc.m_Layers;
		info.samples = i_Desc.m_Samples;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = i_Desc.m_Usage;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VmaAllocationCreateInfo allocation = {};
		allocation.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		VK_CHECK(vmaCreateImage(i_Device.Allocator(), &info, &allocation, &image->m_Image, &image->m_Allocation, NULL));
		i_Device.SetName(VK_OBJECT_TYPE_IMAGE, reinterpret_cast<uint64_t>(image->m_Image), i_Name);

		VkImageViewCreateInfo& view = image->m_ViewInfo;
		view.image = image->m_Image;
		if (i_Desc.m_Type == VK_IMAGE_TYPE_3D)
		{
			view.viewType = VK_IMAGE_VIEW_TYPE_3D;
		}
		else if (i_Desc.m_bCube)
		{
			view.viewType = i_Desc.m_Layers == 6 ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
		}
		else
		{
			view.viewType = i_Desc.m_Layers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
		}
		view.format = i_Desc.m_Format;
		view.subresourceRange = vkdRange(i_Desc.m_Aspect);
		view.subresourceRange.levelCount = i_Desc.m_Mips;
		view.subresourceRange.layerCount = i_Desc.m_Layers;
		if (i_Desc.m_Usage & c_ViewUsages)
		{
			VK_CHECK(vkCreateImageView(i_Device.Device(), &view, NULL, &image->m_View));
			i_Device.SetName(VK_OBJECT_TYPE_IMAGE_VIEW, reinterpret_cast<uint64_t>(image->m_View), i_Name);
		}

		std::lock_guard<std::recursive_mutex> lock(i_Device.StreamLock());
		vkdImageToGeneral(i_Device.Commands(), image->m_Image, i_Desc.m_Aspect);
		return image;
	}

	std::unique_ptr<vkdImage> vkdImage::Wrap(vkdDevice& i_Device, VkImage i_Image, VkImageView i_View,
											 const vkdImageDesc& i_Desc)
	{
		std::unique_ptr<vkdImage> image(new vkdImage(i_Device));
		image->m_Desc = i_Desc;
		image->m_Image = i_Image;
		image->m_View = i_View;
		image->m_bOwnsView = false;
		VkImageViewCreateInfo& view = image->m_ViewInfo;
		view.image = i_Image;
		view.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view.format = i_Desc.m_Format;
		view.subresourceRange = vkdRange(i_Desc.m_Aspect);
		return image;
	}

	std::unique_ptr<vkdImage> vkdImage::Create2D(vkdDevice& i_Device, uint32_t i_Width, uint32_t i_Height,
												 VkFormat i_Format, VkImageUsageFlags i_Usage, const char* i_Name,
												 VkImageAspectFlags i_Aspect)
	{
		vkdImageDesc desc;
		desc.m_Format = i_Format;
		desc.m_Width = i_Width;
		desc.m_Height = i_Height;
		desc.m_Usage = i_Usage;
		desc.m_Aspect = i_Aspect;
		return Create(i_Device, desc, i_Name);
	}

	std::unique_ptr<vkdImage> vkdImage::make_view(const VkImageSubresourceRange& i_Range, VkImageViewType i_Type,
												  const char* i_Name)
	{
		std::unique_ptr<vkdImage> view(new vkdImage(m_Device));
		vkdImageDesc& desc = view->m_Desc;
		desc = m_Desc;
		desc.m_Width = mip_size(m_Desc.m_Width, i_Range.baseMipLevel);
		desc.m_Height = mip_size(m_Desc.m_Height, i_Range.baseMipLevel);
		desc.m_Depth = mip_size(m_Desc.m_Depth, i_Range.baseMipLevel);
		desc.m_Mips = i_Range.levelCount;
		desc.m_Layers = i_Range.layerCount;
		desc.m_bCube = i_Type == VK_IMAGE_VIEW_TYPE_CUBE || i_Type == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
		desc.m_Aspect = i_Range.aspectMask;
		view->m_Image = m_Image;
		view->m_ViewInfo = m_ViewInfo;
		view->m_ViewInfo.viewType = i_Type;
		view->m_ViewInfo.subresourceRange = i_Range;
		if (m_Desc.m_Usage & c_ViewUsages)
		{
			VK_CHECK(vkCreateImageView(m_Device.Device(), &view->m_ViewInfo, NULL, &view->m_View));
			m_Device.SetName(VK_OBJECT_TYPE_IMAGE_VIEW, reinterpret_cast<uint64_t>(view->m_View), i_Name);
		}
		return view;
	}

	vkdImage* vkdImage::Subresource(uint32_t i_Mip, uint32_t i_Layer)
	{
		DBG_ASSERT(m_Allocation, "vkdImage::Subresource: called on a view");
		if (i_Mip >= m_Desc.m_Mips || i_Layer >= m_Desc.m_Layers)
		{
			return nullptr;
		}
		std::unique_ptr<vkdImage>& view = m_Subresources[(uint64_t)i_Mip << 32 | i_Layer];
		if (!view)
		{
			VkImageSubresourceRange range = { m_Desc.m_Aspect, i_Mip, 1, i_Layer, 1 };
			view = make_view(range, m_Desc.m_Type == VK_IMAGE_TYPE_3D ? VK_IMAGE_VIEW_TYPE_3D : VK_IMAGE_VIEW_TYPE_2D,
							 "vkdImage subresource");
		}
		return view.get();
	}

	vkdImage* vkdImage::DepthView()
	{
		DBG_ASSERT(m_Allocation, "vkdImage::DepthView: called on a view");
		DBG_ASSERT(vkdFormatInfoOf(m_Desc.m_Format).m_bDepth, "vkdImage::DepthView: not a depth format");
		if (!m_pDepthView)
		{
			VkImageSubresourceRange range = m_ViewInfo.subresourceRange;
			range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			m_pDepthView = make_view(range, m_ViewInfo.viewType, "vkdImage depth view");
		}
		return m_pDepthView.get();
	}

	vkdImage* vkdImage::LayeredView()
	{
		DBG_ASSERT(m_Allocation, "vkdImage::LayeredView: called on a view");
		DBG_ASSERT(m_Desc.m_bLayeredTarget, "vkdImage::LayeredView: the image wasn't made with m_bLayeredTarget");
		if (!m_pLayeredView && m_Desc.m_bLayeredTarget)
		{
			//	the slices of a 2D_ARRAY_COMPATIBLE 3D image are its layers in
			//	a 2D-array view
			VkImageSubresourceRange range = { m_Desc.m_Aspect, 0, 1, 0, m_Desc.m_Depth };
			m_pLayeredView = make_view(range, VK_IMAGE_VIEW_TYPE_2D_ARRAY, "vkdImage layered view");
		}
		return m_pLayeredView.get();
	}

	uint32_t vkdImage::SampledIndex(vkdBindings& i_Bindings)
	{
		DBG_ASSERT(!m_pBindings || m_pBindings == &i_Bindings, "vkdImage: registered with another vkdBindings");
		if (m_SampledIndex == UINT32_MAX)
		{
			m_pBindings = &i_Bindings;
			m_SampledIndex = i_Bindings.AddImage(m_ViewInfo, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
		}
		return m_SampledIndex;
	}

	uint32_t vkdImage::StorageIndex(vkdBindings& i_Bindings)
	{
		DBG_ASSERT(!m_pBindings || m_pBindings == &i_Bindings, "vkdImage: registered with another vkdBindings");
		if (m_StorageIndex == UINT32_MAX)
		{
			m_pBindings = &i_Bindings;
			m_StorageIndex = i_Bindings.AddImage(m_ViewInfo, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		}
		return m_StorageIndex;
	}

	vkdImage::~vkdImage()
	{
		//	Not bound as a target any more (vkdContext)
		m_Device.ImageDestroyed(this);
		//	Views of this image go first, so their destruction is queued before the image's
		m_Subresources.clear();
		m_pDepthView.reset();
		m_pLayeredView.reset();
		if (m_pBindings)
		{
			for (uint32_t index : {m_SampledIndex, m_StorageIndex})
			{
				if (index != UINT32_MAX)
				{
					m_pBindings->RemoveResource(index);
				}
			}
		}
		VkDevice device = m_Device.Device();
		VmaAllocator allocator = m_Device.Allocator();
		VkImage image = m_Allocation ? m_Image : VK_NULL_HANDLE;
		VkImageView view = m_bOwnsView ? m_View : VK_NULL_HANDLE;
		VmaAllocation allocation = m_Allocation;
		m_Device.DeferDestroy([device, allocator, image, view, allocation]()
		{
			if (view)
			{
				vkDestroyImageView(device, view, NULL);
			}
			if (image)
			{
				vmaDestroyImage(allocator, image, allocation);
			}
		});
	}

	void vkdImage::Upload(vkdContext& i_Context, uint32_t i_Mip, uint32_t i_Layer, const void* i_pData,
						  size_t i_RowPitch, size_t i_SlicePitch)
	{
		DBG_ASSERT(m_Desc.m_Usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT, "vkdImage::Upload: needs TRANSFER_DST usage");
		DBG_ASSERT(i_Mip < m_Desc.m_Mips && i_Layer < m_Desc.m_Layers, "vkdImage::Upload: no mip " << i_Mip << " layer " << i_Layer);
		const Packing p = packing(m_Desc, i_Mip);
		DBG_ASSERT(i_RowPitch >= p.m_RowBytes && i_SlicePitch >= p.m_RowBytes * p.m_Rows, "vkdImage::Upload: pitches too small");

		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		//	Repacked tightly into the ring
		vkdRingBuffer::Allocation staging = i_Context.Allocate(p.m_Bytes, copy_alignment(m_Desc.m_Format));
		const uint8_t* src = static_cast<const uint8_t*>(i_pData);
		uint8_t* dst = static_cast<uint8_t*>(staging.m_pCpu);
		for (uint32_t z = 0; z < p.m_Extent.depth; z++)
		{
			for (uint32_t row = 0; row < p.m_Rows; row++)
			{
				memcpy(dst + z * p.m_SliceBytes + row * p.m_RowBytes, src + z * i_SlicePitch + row * i_RowPitch, p.m_RowBytes);
			}
		}
		VkBufferImageCopy region = {};
		region.bufferOffset = staging.m_Offset;
		region.imageSubresource = { p.m_Aspect, i_Mip, i_Layer, 1 };
		region.imageExtent = p.m_Extent;
		vkCmdCopyBufferToImage(i_Context.Commands(), staging.m_Buffer, m_Image, VK_IMAGE_LAYOUT_GENERAL, 1, &region);
	}

	void vkdImage::GenerateMips(vkdContext& i_Context)
	{
		DBG_ASSERT((m_Desc.m_Usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) && (m_Desc.m_Usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT),
				   "vkdImage::GenerateMips: needs TRANSFER_SRC and TRANSFER_DST usage");
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		VkCommandBuffer commands = i_Context.Commands();
		for (uint32_t mip = 1; mip < m_Desc.m_Mips; mip++)
		{
			if (mip > 1)
			{
				vkdFullBarrier(commands);		// the level just written is the next source
			}
			VkImageBlit blit = {};
			blit.srcSubresource = { m_Desc.m_Aspect, mip - 1, 0, m_Desc.m_Layers };
			blit.srcOffsets[1] = { (int32_t)mip_size(m_Desc.m_Width, mip - 1), (int32_t)mip_size(m_Desc.m_Height, mip - 1),
								   (int32_t)mip_size(m_Desc.m_Depth, mip - 1) };
			blit.dstSubresource = { m_Desc.m_Aspect, mip, 0, m_Desc.m_Layers };
			blit.dstOffsets[1] = { (int32_t)mip_size(m_Desc.m_Width, mip), (int32_t)mip_size(m_Desc.m_Height, mip),
								   (int32_t)mip_size(m_Desc.m_Depth, mip) };
			vkCmdBlitImage(commands, m_Image, VK_IMAGE_LAYOUT_GENERAL, m_Image, VK_IMAGE_LAYOUT_GENERAL, 1, &blit, VK_FILTER_LINEAR);
		}
	}

	std::vector<uint8_t> vkdImage::ReadBack(uint32_t i_Mip, uint32_t i_Layer)
	{
		DBG_ASSERT(m_Desc.m_Usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT, "vkdImage::ReadBack: needs TRANSFER_SRC usage");
		DBG_ASSERT(i_Mip < m_Desc.m_Mips && i_Layer < m_Desc.m_Layers, "vkdImage::ReadBack: no mip " << i_Mip << " layer " << i_Layer);
		const Packing p = packing(m_Desc, i_Mip);
		const VkDeviceSize size = p.m_Bytes;

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
			region.imageSubresource = { p.m_Aspect, i_Mip, i_Layer, 1 };
			region.imageExtent = p.m_Extent;
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
