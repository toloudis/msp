/*****************************************************************************
**  cuExternalImageCache.hpp
**
**      cuExternalImageCache gives CUDA texture objects for Vulkan images
**	(GraphicsVulkan's textures), so device programs sample them in place:
**
**	- An exportable image (vkdImage::IsExportable: dedicated, exportable
**	  memory) of a format CUDA arrays have (GetFormat) is imported once:
**	  its memory (cuImportExternalMemory), then the image in it as a
**	  mipmapped array (cuExternalMemoryGetMappedMipmappedArray) of the
**	  image's size, format and mips.
**	- Any other 2D image (a format CUDA lacks: packed 16-bit, 10-10-10-2,
**	  11-11-10, 9-9-9-5, three-channel float; or an image that isn't
**	  exportable) gets an exportable copy of the same size and mips, in
**	  RGBA8 (sRGB kept) or RGBA16F (float formats), filled by Vulkan blits
**	  and refreshed whenever the source's generation changes; the copy is
**	  imported instead. A format that can't be blitted to those (integer
**	  formats, depth) isn't sampled (logged once).
**
**	Each image has a texture object per sampler mode (Mode), made on first
**	use. Formats with fewer than four channels, and BGRA, are read through
**	a swizzle the device programs apply (optxTextureTypes.h), so a sample
**	reads as Vulkan's (and D3D's) sampler gives it: R -> (r, 0, 0, 1), RG
**	-> (r, g, 0, 1), A8 -> (0, 0, 0, a), three channels -> (r, g, b, 1).
**	sRGB formats decode to linear as Vulkan's sampler does.
**
**	An image's entry goes when the image is destroyed (vkdDevice's
**	image-destroy hooks, on any thread); its CUDA objects are destroyed at
**	the next Collect, once the OptiX stream is idle, since a queued launch
**	may still sample them.
**
**	Map and Collect are for the render thread, with the OptiX context
**	current; Map records Vulkan commands (copies) in i_Context.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_CUEXTERNALIMAGECACHE_HPP
#error cuExternalImageCache.hpp multiply included
#endif
#define GOX_CUEXTERNALIMAGECACHE_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif

#include <volk.h>

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace gvk
{
	class vkdContext;
	class vkdDevice;
	class vkdImage;
}

namespace gox
{
	class cuExternalImageCache
	{
		public:
			//	How a texture object samples. Material textures are trilinear:
			//	the raster's sampler is anisotropic, but a ray cone gives one
			//	isotropic level (tex2DLod), so anisotropy would have no effect.
			enum Mode
			{
				e_Material,			// wrap, trilinear (a material's textures)
				e_Projector,		// black border, bilinear, mip 0 (a projected light's texture, as the raster's projSampler)
				e_ModeCount
			};

			//	The device programs' swizzle of a sample (optxTextureTypes.h)
			enum Swizzle
			{
				e_SwizzleRGBA = 0,
				e_SwizzleBGRA = 1,
				e_SwizzleR001 = 2,
				e_SwizzleRG01 = 3,
				e_Swizzle000A = 4,
				e_SwizzleRGB1 = 5
			};

			//	A Vulkan format as a CUDA array format; false if CUDA has none
			struct Format
			{
				CUarray_format	m_Format;
				unsigned		m_Channels;
				Swizzle			m_Swizzle;
				bool			m_bSrgb;
			};
			static bool GetFormat(VkFormat i_Format, Format& o_Format);

			//	An image as CUDA sees it
			struct Image
			{
				CUmipmappedArray	m_Array = NULL;
				CUtexObject			m_Textures[e_ModeCount] = {};
				unsigned			m_Width = 0;
				unsigned			m_Height = 0;
				unsigned			m_Levels = 0;
				Swizzle				m_Swizzle = e_SwizzleRGBA;
				bool				m_bSrgb = false;
				bool				m_bCopy = false;		// sampled through a copy
				uint64_t			m_WriteValue = 0;		// Vulkan's timeline value to wait for before sampling
			};

			//	For tests and the profiler
			struct Stats
			{
				int		m_Imports = 0;		// images (or copies) imported
				int		m_Copies = 0;		// copies made or refreshed
				int		m_Textures = 0;		// texture objects made
			};

			cuExternalImageCache(gvk::vkdDevice& i_Device, gvk::vkdContext& i_Context);
			~cuExternalImageCache();
			cuExternalImageCache(const cuExternalImageCache&) = delete;
			cuExternalImageCache& operator=(const cuExternalImageCache&) = delete;

			//--------------------------------------------------------------------
			//	Map - the CUDA view of a 2D image (imported, or through a copy
			//	brought up to date), or NULL (logged once) if it can't have one.
			//	The pointer stays valid until the image is destroyed and Collect
			//	runs.
			//--------------------------------------------------------------------
			const Image* Map(gvk::vkdImage& i_Image);

			//--------------------------------------------------------------------
			//	Texture - the image's texture object for a sampler mode (made on
			//	first use); 0 if it can't be sampled
			//--------------------------------------------------------------------
			CUtexObject Texture(gvk::vkdImage& i_Image, Mode i_Mode);

			//--------------------------------------------------------------------
			//	Collect - destroys what destroyed images left behind (waits for
			//	the OptiX stream first if there is any)
			//--------------------------------------------------------------------
			void Collect();

			//--------------------------------------------------------------------
			//	For tests
			//--------------------------------------------------------------------
			const Stats& GetStats() const { return m_Stats; }
			size_t GetEntryCount() const;

		private:
			struct Entry;

			bool import(Entry& io_Entry, gvk::vkdImage& i_Image, const Format& i_Format);
			bool refresh_copy(Entry& io_Entry, gvk::vkdImage& i_Source);
			void forget(const gvk::vkdImage* i_pImage);
			static void destroy(Entry& io_Entry);

			gvk::vkdDevice&		m_Device;
			gvk::vkdContext&	m_Context;
			int					m_HookId;
			mutable std::mutex	m_Lock;		// m_Entries and m_Dead (the hook runs on any thread)
			std::unordered_map<const gvk::vkdImage*, std::unique_ptr<Entry>>	m_Entries;
			std::vector<std::unique_ptr<Entry>>									m_Dead;
			Stats				m_Stats;
	};
}
