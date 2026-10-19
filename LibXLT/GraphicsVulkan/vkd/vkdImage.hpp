/*****************************************************************************
**  vkdImage.hpp
**
**      vkdImage is an image of any texture shape (2D, cube, 3D, 2D array;
**	mips; any format vkdFormat knows, block-compressed included) with one
**	view of the whole of it, allocated with Vulkan Memory Allocator. With
**	VK_KHR_unified_image_layouts it lives in VK_IMAGE_LAYOUT_GENERAL from
**	creation on (Create records that one transition). Destruction is
**	deferred until the GPU is done with it.
**
**	Subresource() and DepthView() give views of part of the image (one mip
**	of one face or slice; the depth aspect) as vkdImages of their own, to
**	bind as render targets or textures where DX11 code makes a view of
**	part of a texture. They share the parent's VkImage and are owned by it.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDIMAGE_HPP
#error vkdImage.hpp multiply included
#endif
#define GVK_VKDIMAGE_HPP

#include <volk.h>

#include <cstdint>
#include <map>
#include <memory>
#include <vector>

typedef struct VmaAllocation_T* VmaAllocation;

namespace gvk
{
	class vkdBindings;
	class vkdContext;
	class vkdDevice;

	struct vkdImageDesc
	{
		VkImageType				m_Type = VK_IMAGE_TYPE_2D;
		VkFormat				m_Format = VK_FORMAT_UNDEFINED;
		uint32_t				m_Width = 0;
		uint32_t				m_Height = 0;
		uint32_t				m_Depth = 1;
		uint32_t				m_Mips = 1;
		uint32_t				m_Layers = 1;			// cube faces count as layers (6 per cube)
		bool					m_bCube = false;
		VkSampleCountFlagBits	m_Samples = VK_SAMPLE_COUNT_1_BIT;
		VkImageUsageFlags		m_Usage = 0;
		VkImageAspectFlags		m_Aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	};

	class vkdImage
	{
		public:
			static std::unique_ptr<vkdImage> Create(vkdDevice& i_Device, const vkdImageDesc& i_Desc, const char* i_Name);
			static std::unique_ptr<vkdImage> Create2D(vkdDevice& i_Device, uint32_t i_Width, uint32_t i_Height,
													  VkFormat i_Format, VkImageUsageFlags i_Usage, const char* i_Name,
													  VkImageAspectFlags i_Aspect = VK_IMAGE_ASPECT_COLOR_BIT);
			~vkdImage();

			//	For a subresource or depth view, the description of what the
			//	view covers (one mip and layer, its size; the depth aspect)
			const vkdImageDesc& Desc() const	{ return m_Desc; }

			VkImage		Image() const		{ return m_Image; }
			VkImageView	View() const		{ return m_View; }
			VkFormat	Format() const		{ return m_Desc.m_Format; }
			VkImageUsageFlags Usage() const	{ return m_Desc.m_Usage; }
			VkImageAspectFlags Aspect() const { return m_Desc.m_Aspect; }
			VkSampleCountFlagBits Samples() const { return m_Desc.m_Samples; }
			uint32_t	Width() const		{ return m_Desc.m_Width; }
			uint32_t	Height() const		{ return m_Desc.m_Height; }

			//	The view description (descriptor heaps build image descriptors
			//	from it); valid even when the usage needs no VkImageView.
			const VkImageViewCreateInfo& ViewInfo() const { return m_ViewInfo; }
			//	The part of Image() the view covers
			const VkImageSubresourceRange& Range() const { return m_ViewInfo.subresourceRange; }

			//	A view of one mip of one layer (cube face, array slice; for a
			//	3D image, the whole depth of that mip), made on first use and
			//	owned by this image. NULL for an out-of-range mip or layer.
			vkdImage* Subresource(uint32_t i_Mip, uint32_t i_Layer);
			//	A view of the depth aspect only, for sampling a depth-stencil
			//	image; made on first use and owned by this image
			vkdImage* DepthView();

			//	The image's index in i_Bindings' resource heap, as a sampled
			//	(t register) or storage (u register) image: registered on first
			//	use, removed when the image is destroyed.
			uint32_t SampledIndex(vkdBindings& i_Bindings);
			uint32_t StorageIndex(vkdBindings& i_Bindings);

			//	Copies one subresource (all depth slices of a 3D mip) from the
			//	CPU, through the ring buffer, as the next command of i_Context.
			//	i_RowPitch is bytes per row of pixels (of blocks, for compressed
			//	formats), i_SlicePitch bytes per depth slice. Needs TRANSFER_DST.
			void Upload(vkdContext& i_Context, uint32_t i_Mip, uint32_t i_Layer, const void* i_pData,
						size_t i_RowPitch, size_t i_SlicePitch);

			//	Fills mips 1.. from mip 0 with linear blits, level by level, on
			//	every layer, as D3D11's GenerateMips. Needs TRANSFER_SRC and
			//	TRANSFER_DST, and a format that can be blitted with linear filtering.
			void GenerateMips(vkdContext& i_Context);

			//	ReadBack copies one subresource to the CPU (rows tightly packed,
			//	as blocks for compressed formats; all depth slices of a 3D mip),
			//	submitting the open commands and waiting for them. Needs
			//	TRANSFER_SRC. A depth-stencil image reads its depth.
			std::vector<uint8_t> ReadBack(uint32_t i_Mip = 0, uint32_t i_Layer = 0);

		private:
			vkdImage(vkdDevice& i_Device) : m_Device(i_Device) {}
			vkdImage(const vkdImage&) = delete;
			vkdImage& operator = (const vkdImage&) = delete;

			//	A view of part of this image, owned by it
			std::unique_ptr<vkdImage> make_view(const VkImageSubresourceRange& i_Range, VkImageViewType i_Type,
												const char* i_Name);

			vkdDevice&		m_Device;
			vkdImageDesc	m_Desc;
			VkImage			m_Image = VK_NULL_HANDLE;
			VkImageView		m_View = VK_NULL_HANDLE;
			VkImageViewCreateInfo m_ViewInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
			VmaAllocation	m_Allocation = nullptr;		// NULL for views: the parent owns the image
			vkdBindings*	m_pBindings = nullptr;
			uint32_t		m_SampledIndex = UINT32_MAX;
			uint32_t		m_StorageIndex = UINT32_MAX;
			std::map<uint64_t, std::unique_ptr<vkdImage>> m_Subresources;	// by mip << 32 | layer
			std::unique_ptr<vkdImage> m_pDepthView;
	};

	//	vkdFormatSize - bytes per pixel of an uncompressed format; 0 for
	//	block-compressed and unknown formats
	uint32_t vkdFormatSize(VkFormat i_Format);
}
