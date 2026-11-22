/*****************************************************************************
**  cuExternalImageCache.cpp
**
**      See cuExternalImageCache.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_CUEXTERNALIMAGECACHE_HPP
#include "GraphicsOptiX/cu/cuExternalImageCache.hpp"
#endif
#ifndef GOX_CUEXTERNAL_HPP
#include "GraphicsOptiX/cu/cuExternal.hpp"
#endif
#ifndef GOX_CULOADER_HPP
#include "GraphicsOptiX/cu/cuLoader.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDEXTERNAL_HPP
#include "GraphicsVulkan/vkd/vkdExternal.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDINFO_HPP
#include "GraphicsVulkan/vkd/vkdInfo.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <algorithm>
#include <set>

namespace gox
{
	namespace
	{

		//	The format a copy of an image CUDA can't map is made in: float
		//	formats keep their range in RGBA16F; the rest are 8-bit, sRGB kept
		VkFormat copy_format(VkFormat i_Format)
		{
			switch (i_Format)
			{
				case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
				case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
				case VK_FORMAT_R32G32B32_SFLOAT:
				case VK_FORMAT_BC6H_UFLOAT_BLOCK:
				case VK_FORMAT_BC6H_SFLOAT_BLOCK:
				case VK_FORMAT_R16G16B16A16_SFLOAT:
				case VK_FORMAT_R16G16_SFLOAT:
				case VK_FORMAT_R16_SFLOAT:
				case VK_FORMAT_R32G32B32A32_SFLOAT:
				case VK_FORMAT_R32G32_SFLOAT:
				case VK_FORMAT_R32_SFLOAT:
					return VK_FORMAT_R16G16B16A16_SFLOAT;
				default:
					return gvk::vkdFormatInfoOf(i_Format).m_bSrgb ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
			}
		}

		void log_once(VkFormat i_Format, const char* i_What)
		{
			static std::set<int> s_Logged;
			static std::mutex s_Lock;
			std::lock_guard<std::mutex> lock(s_Lock);
			if (s_Logged.insert(int(i_Format)).second)
			{
				DBG_LOG("GraphicsOptiX: an image of format " << int(i_Format) << " " << i_What);
			}
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	struct cuExternalImageCache::Entry
	{
		CUexternalMemory			m_Memory = NULL;
		Image						m_Image;
		std::unique_ptr<gvk::vkdImage>	m_pCopy;			// for a source CUDA can't map
		uint64_t					m_SourceGeneration = 0;	// the copy's
		bool						m_bFailed = false;		// can't be mapped: don't try again
	};

	bool cuExternalImageCache::GetFormat(VkFormat i_Format, Format& o_Format)
	{
		struct Row
		{
			VkFormat		m_Vk;
			CUarray_format	m_Cu;
			unsigned		m_Channels;
			Swizzle			m_Swizzle;
			bool			m_bSrgb;
		};
		static const Row c_Rows[] =
		{
			{ VK_FORMAT_R8G8B8A8_UNORM,			CU_AD_FORMAT_UNSIGNED_INT8,		4, e_SwizzleRGBA, false },
			{ VK_FORMAT_R8G8B8A8_SRGB,			CU_AD_FORMAT_UNSIGNED_INT8,		4, e_SwizzleRGBA, true },
			{ VK_FORMAT_R8G8B8A8_SNORM,			CU_AD_FORMAT_SIGNED_INT8,		4, e_SwizzleRGBA, false },
			{ VK_FORMAT_B8G8R8A8_UNORM,			CU_AD_FORMAT_UNSIGNED_INT8,		4, e_SwizzleBGRA, false },
			{ VK_FORMAT_B8G8R8A8_SRGB,			CU_AD_FORMAT_UNSIGNED_INT8,		4, e_SwizzleBGRA, true },
			{ VK_FORMAT_R8G8_UNORM,				CU_AD_FORMAT_UNSIGNED_INT8,		2, e_SwizzleRG01, false },
			{ VK_FORMAT_R8G8_SNORM,				CU_AD_FORMAT_SIGNED_INT8,		2, e_SwizzleRG01, false },
			{ VK_FORMAT_R8_UNORM,				CU_AD_FORMAT_UNSIGNED_INT8,		1, e_SwizzleR001, false },
			{ VK_FORMAT_R8_SNORM,				CU_AD_FORMAT_SIGNED_INT8,		1, e_SwizzleR001, false },
			{ VK_FORMAT_A8_UNORM,				CU_AD_FORMAT_UNSIGNED_INT8,		1, e_Swizzle000A, false },
			{ VK_FORMAT_R16G16B16A16_UNORM,		CU_AD_FORMAT_UNSIGNED_INT16,	4, e_SwizzleRGBA, false },
			{ VK_FORMAT_R16G16B16A16_SNORM,		CU_AD_FORMAT_SIGNED_INT16,		4, e_SwizzleRGBA, false },
			{ VK_FORMAT_R16G16_UNORM,			CU_AD_FORMAT_UNSIGNED_INT16,	2, e_SwizzleRG01, false },
			{ VK_FORMAT_R16G16_SNORM,			CU_AD_FORMAT_SIGNED_INT16,		2, e_SwizzleRG01, false },
			{ VK_FORMAT_R16_UNORM,				CU_AD_FORMAT_UNSIGNED_INT16,	1, e_SwizzleR001, false },
			{ VK_FORMAT_R16_SNORM,				CU_AD_FORMAT_SIGNED_INT16,		1, e_SwizzleR001, false },
			{ VK_FORMAT_R16G16B16A16_SFLOAT,	CU_AD_FORMAT_HALF,				4, e_SwizzleRGBA, false },
			{ VK_FORMAT_R16G16_SFLOAT,			CU_AD_FORMAT_HALF,				2, e_SwizzleRG01, false },
			{ VK_FORMAT_R16_SFLOAT,				CU_AD_FORMAT_HALF,				1, e_SwizzleR001, false },
			{ VK_FORMAT_R32G32B32A32_SFLOAT,	CU_AD_FORMAT_FLOAT,				4, e_SwizzleRGBA, false },
			{ VK_FORMAT_R32G32_SFLOAT,			CU_AD_FORMAT_FLOAT,				2, e_SwizzleRG01, false },
			{ VK_FORMAT_R32_SFLOAT,				CU_AD_FORMAT_FLOAT,				1, e_SwizzleR001, false },
			{ VK_FORMAT_BC1_RGBA_UNORM_BLOCK,	CU_AD_FORMAT_BC1_UNORM,			4, e_SwizzleRGBA, false },
			{ VK_FORMAT_BC1_RGBA_SRGB_BLOCK,	CU_AD_FORMAT_BC1_UNORM_SRGB,	4, e_SwizzleRGBA, true },
			{ VK_FORMAT_BC2_UNORM_BLOCK,		CU_AD_FORMAT_BC2_UNORM,			4, e_SwizzleRGBA, false },
			{ VK_FORMAT_BC2_SRGB_BLOCK,			CU_AD_FORMAT_BC2_UNORM_SRGB,	4, e_SwizzleRGBA, true },
			{ VK_FORMAT_BC3_UNORM_BLOCK,		CU_AD_FORMAT_BC3_UNORM,			4, e_SwizzleRGBA, false },
			{ VK_FORMAT_BC3_SRGB_BLOCK,			CU_AD_FORMAT_BC3_UNORM_SRGB,	4, e_SwizzleRGBA, true },
			{ VK_FORMAT_BC4_UNORM_BLOCK,		CU_AD_FORMAT_BC4_UNORM,			1, e_SwizzleR001, false },
			{ VK_FORMAT_BC4_SNORM_BLOCK,		CU_AD_FORMAT_BC4_SNORM,			1, e_SwizzleR001, false },
			{ VK_FORMAT_BC5_UNORM_BLOCK,		CU_AD_FORMAT_BC5_UNORM,			2, e_SwizzleRG01, false },
			{ VK_FORMAT_BC5_SNORM_BLOCK,		CU_AD_FORMAT_BC5_SNORM,			2, e_SwizzleRG01, false },
			{ VK_FORMAT_BC6H_UFLOAT_BLOCK,		CU_AD_FORMAT_BC6H_UF16,			3, e_SwizzleRGB1, false },
			{ VK_FORMAT_BC6H_SFLOAT_BLOCK,		CU_AD_FORMAT_BC6H_SF16,			3, e_SwizzleRGB1, false },
			{ VK_FORMAT_BC7_UNORM_BLOCK,		CU_AD_FORMAT_BC7_UNORM,			4, e_SwizzleRGBA, false },
			{ VK_FORMAT_BC7_SRGB_BLOCK,			CU_AD_FORMAT_BC7_UNORM_SRGB,	4, e_SwizzleRGBA, true },
		};
		for (const Row& row : c_Rows)
		{
			if (row.m_Vk == i_Format)
			{
				o_Format.m_Format = row.m_Cu;
				o_Format.m_Channels = row.m_Channels;
				o_Format.m_Swizzle = row.m_Swizzle;
				o_Format.m_bSrgb = row.m_bSrgb;
				return true;
			}
		}
		return false;
	}

	cuExternalImageCache::cuExternalImageCache(gvk::vkdDevice& i_Device, gvk::vkdContext& i_Context)
		: m_Device(i_Device)
		, m_Context(i_Context)
		, m_HookId(0)
	{
		m_HookId = m_Device.AddImageDestroyHook([this](const gvk::vkdImage* i_pImage) { forget(i_pImage); });
	}

	cuExternalImageCache::~cuExternalImageCache()
	{
		m_Device.RemoveImageDestroyHook(m_HookId);
		std::vector<std::unique_ptr<Entry>> entries;
		{
			std::lock_guard<std::mutex> lock(m_Lock);
			for (auto& entry : m_Entries)
			{
				entries.push_back(std::move(entry.second));
			}
			m_Entries.clear();
			for (auto& entry : m_Dead)
			{
				entries.push_back(std::move(entry));
			}
			m_Dead.clear();
		}
		if (!entries.empty())
		{
			optxContext::MakeCurrent();
			optxContext::Synchronize();
			for (auto& entry : entries)
			{
				destroy(*entry);
			}
		}
	}

	size_t cuExternalImageCache::GetEntryCount() const
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		return m_Entries.size();
	}

	void cuExternalImageCache::destroy(Entry& io_Entry)
	{
		for (CUtexObject& texture : io_Entry.m_Image.m_Textures)
		{
			if (texture)
			{
				CU_CHECK(cu().cuTexObjectDestroy(texture));
				texture = 0;
			}
		}
		if (io_Entry.m_Image.m_Array)
		{
			CU_CHECK(cu().cuMipmappedArrayDestroy(io_Entry.m_Image.m_Array));
			io_Entry.m_Image.m_Array = NULL;
		}
		if (io_Entry.m_Memory)
		{
			CU_CHECK(cu().cuDestroyExternalMemory(io_Entry.m_Memory));
			io_Entry.m_Memory = NULL;
		}
		io_Entry.m_pCopy.reset();	// Vulkan defers its destruction
	}

	void cuExternalImageCache::forget(const gvk::vkdImage* i_pImage)
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		auto found = m_Entries.find(i_pImage);
		if (found != m_Entries.end())
		{
			m_Dead.push_back(std::move(found->second));
			m_Entries.erase(found);
		}
	}

	void cuExternalImageCache::Collect()
	{
		std::vector<std::unique_ptr<Entry>> dead;
		{
			std::lock_guard<std::mutex> lock(m_Lock);
			dead.swap(m_Dead);
		}
		if (dead.empty())
		{
			return;
		}
		optxContext::Synchronize();
		std::lock_guard<std::recursive_mutex> stream(m_Device.StreamLock());
		for (auto& entry : dead)
		{
			destroy(*entry);
		}
	}

	//	The image's memory imported, and the image in it as a mipmapped array
	bool cuExternalImageCache::import(Entry& io_Entry, gvk::vkdImage& i_Image, const Format& i_Format)
	{
		const gvk::vkdImageDesc& desc = i_Image.Desc();
		void* handle = gvk::vkdExportMemoryHandle(m_Device.Device(), i_Image.Memory());
		if (!handle)
		{
			DBG_LOG("GraphicsOptiX: Vulkan gave no handle for an image's memory");
			return false;
		}
		CUDA_EXTERNAL_MEMORY_HANDLE_DESC memory_desc = {};
		cuSetPlatformMemoryHandle(memory_desc, handle);
		memory_desc.size = i_Image.MemorySize();
		memory_desc.flags = i_Image.IsDedicatedMemory() ? CUDA_EXTERNAL_MEMORY_DEDICATED : 0;
		const bool imported = !CU_CHECK(cu().cuImportExternalMemory(&io_Entry.m_Memory, &memory_desc));
		gvk::vkdCloseExternalHandle(handle);	// CUDA keeps its own reference
		if (!imported)
		{
			io_Entry.m_Memory = NULL;
			return false;
		}

		CUDA_EXTERNAL_MEMORY_MIPMAPPED_ARRAY_DESC array_desc = {};
		array_desc.offset = 0;
		array_desc.arrayDesc.Width = desc.m_Width;
		array_desc.arrayDesc.Height = desc.m_Height;
		array_desc.arrayDesc.Depth = 0;
		array_desc.arrayDesc.Format = i_Format.m_Format;
		array_desc.arrayDesc.NumChannels = i_Format.m_Channels;
		array_desc.arrayDesc.Flags = (desc.m_Usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) ? CUDA_ARRAY3D_COLOR_ATTACHMENT : 0;
		array_desc.numLevels = desc.m_Mips;
		if (CU_CHECK(cu().cuExternalMemoryGetMappedMipmappedArray(&io_Entry.m_Image.m_Array, io_Entry.m_Memory, &array_desc)))
		{
			io_Entry.m_Image.m_Array = NULL;
			CU_CHECK(cu().cuDestroyExternalMemory(io_Entry.m_Memory));
			io_Entry.m_Memory = NULL;
			return false;
		}
		io_Entry.m_Image.m_Width = desc.m_Width;
		io_Entry.m_Image.m_Height = desc.m_Height;
		io_Entry.m_Image.m_Levels = desc.m_Mips;
		io_Entry.m_Image.m_Swizzle = i_Format.m_Swizzle;
		io_Entry.m_Image.m_bSrgb = i_Format.m_bSrgb;
		++m_Stats.m_Imports;
		return true;
	}

	//	The copy's mips blitted from the source's (same sizes, so no filtering)
	bool cuExternalImageCache::refresh_copy(Entry& io_Entry, gvk::vkdImage& i_Source)
	{
		const gvk::vkdImageDesc& source = i_Source.Desc();
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		VkCommandBuffer commands = m_Context.Commands();
		gvk::vkdFullBarrier(commands);
		for (uint32_t mip = 0; mip < source.m_Mips; ++mip)
		{
			const int32_t w = int32_t(std::max(1u, source.m_Width >> mip));
			const int32_t h = int32_t(std::max(1u, source.m_Height >> mip));
			VkImageBlit blit = {};
			blit.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, mip, 0, 1 };
			blit.srcOffsets[1] = { w, h, 1 };
			blit.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, mip, 0, 1 };
			blit.dstOffsets[1] = { w, h, 1 };
			vkCmdBlitImage(commands, i_Source.Image(), VK_IMAGE_LAYOUT_GENERAL, io_Entry.m_pCopy->Image(),
						   VK_IMAGE_LAYOUT_GENERAL, 1, &blit, VK_FILTER_NEAREST);
		}
		gvk::vkdFullBarrier(commands);
		io_Entry.m_pCopy->MarkWritten();
		io_Entry.m_SourceGeneration = i_Source.Generation();
		++m_Stats.m_Copies;
		return true;
	}

	const cuExternalImageCache::Image* cuExternalImageCache::Map(gvk::vkdImage& i_Image)
	{
		//	The stream lock first: the destroy hook runs under it, then takes m_Lock
		std::lock_guard<std::recursive_mutex> stream(m_Device.StreamLock());
		std::lock_guard<std::mutex> lock(m_Lock);
		std::unique_ptr<Entry>& slot = m_Entries[&i_Image];
		if (!slot)
		{
			slot.reset(new Entry);
			Entry& entry = *slot;
			const gvk::vkdImageDesc& desc = i_Image.Desc();
			const bool plain = desc.m_Type == VK_IMAGE_TYPE_2D && !desc.m_bCube && desc.m_Layers == 1 &&
							   desc.m_Samples == VK_SAMPLE_COUNT_1_BIT && (desc.m_Aspect & VK_IMAGE_ASPECT_COLOR_BIT);
			Format format;
			if (!plain)
			{
				log_once(desc.m_Format, "isn't a plain 2D colour image; the tracer doesn't sample it");
				entry.m_bFailed = true;
			}
			else if (i_Image.IsExportable() && GetFormat(desc.m_Format, format))
			{
				entry.m_bFailed = !import(entry, i_Image, format);
			}
			else
			{
				//	A copy CUDA can map, if Vulkan can blit into it
				const VkFormat target = copy_format(desc.m_Format);
				VkFormatProperties from = {}, to = {};
				vkGetPhysicalDeviceFormatProperties(m_Device.Physical(), desc.m_Format, &from);
				vkGetPhysicalDeviceFormatProperties(m_Device.Physical(), target, &to);
				const bool blittable = (from.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) &&
									   (to.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT) &&
									   (desc.m_Usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
				if (!blittable || !GetFormat(target, format))
				{
					log_once(desc.m_Format, "can't be copied into a format CUDA samples; the tracer doesn't sample it");
					entry.m_bFailed = true;
				}
				else
				{
					gvk::vkdImageDesc copy = {};
					copy.m_Format = target;
					copy.m_Width = desc.m_Width;
					copy.m_Height = desc.m_Height;
					copy.m_Mips = desc.m_Mips;
					copy.m_Usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
					copy.m_bExportable = true;
					entry.m_pCopy = gvk::vkdImage::Create(m_Device, copy, "cuExternalImageCache copy");
					entry.m_bFailed = !entry.m_pCopy->IsExportable() || !import(entry, *entry.m_pCopy, format);
					if (!entry.m_bFailed)
					{
						log_once(desc.m_Format, i_Image.IsExportable() ? "is sampled through a copy (CUDA has no such format)"
																	   : "is sampled through a copy (it isn't exportable)");
						entry.m_Image.m_bCopy = true;
						refresh_copy(entry, i_Image);
					}
				}
			}
		}
		Entry& entry = *slot;
		if (entry.m_bFailed)
		{
			return NULL;
		}
		if (entry.m_pCopy)
		{
			if (entry.m_SourceGeneration != i_Image.Generation())
			{
				refresh_copy(entry, i_Image);
			}
			entry.m_Image.m_WriteValue = entry.m_pCopy->WriteValue();
		}
		else
		{
			entry.m_Image.m_WriteValue = i_Image.WriteValue();
		}
		return &entry.m_Image;
	}

	CUtexObject cuExternalImageCache::Texture(gvk::vkdImage& i_Image, Mode i_Mode)
	{
		//	Held throughout, so the image's entry can't go (the destroy hook runs under it)
		std::lock_guard<std::recursive_mutex> stream(m_Device.StreamLock());
		if (!Map(i_Image))
		{
			return 0;
		}
		std::lock_guard<std::mutex> lock(m_Lock);
		Image& image = m_Entries[&i_Image]->m_Image;
		CUtexObject& texture = image.m_Textures[i_Mode];
		if (texture)
		{
			return texture;
		}
		CUDA_RESOURCE_DESC resource = {};
		resource.resType = CU_RESOURCE_TYPE_MIPMAPPED_ARRAY;
		resource.res.mipmap.hMipmappedArray = image.m_Array;
		CUDA_TEXTURE_DESC sampler = {};
		const CUaddress_mode address = (i_Mode == e_Projector) ? CU_TR_ADDRESS_MODE_BORDER : CU_TR_ADDRESS_MODE_WRAP;
		sampler.addressMode[0] = sampler.addressMode[1] = sampler.addressMode[2] = address;
		if (i_Mode == e_Environment)
		{
			sampler.addressMode[1] = CU_TR_ADDRESS_MODE_CLAMP;	// the poles don't wrap into each other
		}
		sampler.filterMode = CU_TR_FILTER_MODE_LINEAR;
		sampler.flags = CU_TRSF_NORMALIZED_COORDINATES | (image.m_bSrgb ? CU_TRSF_SRGB : 0);
		sampler.maxAnisotropy = 1;
		sampler.mipmapFilterMode = (i_Mode == e_Projector) ? CU_TR_FILTER_MODE_POINT : CU_TR_FILTER_MODE_LINEAR;
		sampler.minMipmapLevelClamp = 0.0f;
		sampler.maxMipmapLevelClamp = (i_Mode == e_Projector) ? 0.0f : float(image.m_Levels - 1);
		//	borderColor stays transparent black
		if (CU_CHECK(cu().cuTexObjectCreate(&texture, &resource, &sampler, NULL)))
		{
			texture = 0;
			return 0;
		}
		++m_Stats.m_Textures;
		return texture;
	}
}
