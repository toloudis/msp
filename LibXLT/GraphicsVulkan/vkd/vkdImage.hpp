/*****************************************************************************
**  vkdImage.hpp
**
**      vkdImage is a 2D image with one view, allocated with Vulkan Memory
**	Allocator. With VK_KHR_unified_image_layouts it lives in
**	VK_IMAGE_LAYOUT_GENERAL from creation on (Create2D records that one
**	transition). Destruction is deferred until the GPU is done with it.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDIMAGE_HPP
#error vkdImage.hpp multiply included
#endif
#define GVK_VKDIMAGE_HPP

#include <volk.h>

#include <cstdint>
#include <memory>
#include <vector>

typedef struct VmaAllocation_T* VmaAllocation;

namespace gvk
{
	class vkdDevice;

	class vkdImage
	{
		public:
			static std::unique_ptr<vkdImage> Create2D(vkdDevice& i_Device, uint32_t i_Width, uint32_t i_Height,
													  VkFormat i_Format, VkImageUsageFlags i_Usage, const char* i_Name,
													  VkImageAspectFlags i_Aspect = VK_IMAGE_ASPECT_COLOR_BIT);
			~vkdImage();

			VkImage		Image() const		{ return m_Image; }
			VkImageView	View() const		{ return m_View; }
			VkFormat	Format() const		{ return m_Format; }
			uint32_t	Width() const		{ return m_Width; }
			uint32_t	Height() const		{ return m_Height; }

			//	ReadBack copies the image to the CPU (rows tightly packed),
			//	submitting the open commands and waiting for them. Needs
			//	VK_IMAGE_USAGE_TRANSFER_SRC_BIT and a colour format.
			std::vector<uint8_t> ReadBack();

		private:
			vkdImage(vkdDevice& i_Device) : m_Device(i_Device) {}

			vkdDevice&		m_Device;
			VkImage			m_Image = VK_NULL_HANDLE;
			VkImageView		m_View = VK_NULL_HANDLE;
			VmaAllocation	m_Allocation = nullptr;
			VkFormat		m_Format = VK_FORMAT_UNDEFINED;
			VkImageAspectFlags m_Aspect = 0;
			uint32_t		m_Width = 0;
			uint32_t		m_Height = 0;
	};

	//	vkdFormatSize - bytes per pixel of the uncompressed formats ReadBack supports
	uint32_t vkdFormatSize(VkFormat i_Format);
}
