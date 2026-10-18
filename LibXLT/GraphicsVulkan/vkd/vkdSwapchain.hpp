/*****************************************************************************
**  vkdSwapchain.hpp
**
**      vkdSwapchain is a window's surface and swap chain: R8G8B8A8_UNORM
**	(B8G8R8A8_UNORM if the surface lacks it), no vsync (IMMEDIATE, else
**	MAILBOX, else FIFO). Acquire() makes the next image current (in
**	VK_IMAGE_LAYOUT_GENERAL, contents discarded) and Present() submits the
**	device's open commands, waiting for the acquire and signalling a
**	per-image semaphore, then presents. The swap chain is recreated when
**	the window size changes or Vulkan reports it out of date; while the
**	window has no area nothing is acquired or presented.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDSWAPCHAIN_HPP
#error vkdSwapchain.hpp multiply included
#endif
#define GVK_VKDSWAPCHAIN_HPP

#include <volk.h>

#include <cstdint>
#include <string>
#include <vector>

namespace gvk
{
	class vkdDevice;

	class vkdSwapchain
	{
		public:
			//	Returns with IsValid() false (and a logged reason) if no surface
			//	could be made for the window or the queue can't present to it.
			vkdSwapchain(vkdDevice& i_Device, void* i_NativeWindow, const std::string& i_Name);
			~vkdSwapchain();

			bool IsValid() const { return m_Surface != VK_NULL_HANDLE && m_bCanPresent; }

			//	Makes the next image current if none is; false while the window
			//	has no area (or the swap chain couldn't be made).
			bool Acquire();
			bool IsAcquired() const { return m_bAcquired; }

			//	Presents the current image (acquiring one first if needed).
			void Present();

			//	The window was resized: the swap chain is recreated on the next Acquire.
			void Invalidate() { m_bRecreate = true; }

			//	The current image (valid while IsAcquired()).
			VkImage		Image() const;
			VkImageView	View() const;
			VkFormat	Format() const		{ return m_Format; }
			uint32_t	Width() const		{ return m_Extent.width; }
			uint32_t	Height() const		{ return m_Extent.height; }
			VkPresentModeKHR PresentMode() const { return m_PresentMode; }

			//	Testing: the next Present() first copies the image to the CPU.
			void DebugReadBackNextFrame() { m_bReadBack = true; }
			const std::vector<uint8_t>& DebugPixels() const { return m_Pixels; }

		private:
			bool create();
			void destroy_images();

			vkdDevice&					m_Device;
			void*						m_pWindow;
			std::string					m_Name;
			VkSurfaceKHR				m_Surface = VK_NULL_HANDLE;
			bool						m_bCanPresent = false;
			VkSwapchainKHR				m_Swapchain = VK_NULL_HANDLE;
			VkFormat					m_Format = VK_FORMAT_UNDEFINED;
			VkColorSpaceKHR				m_ColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			VkPresentModeKHR			m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
			VkExtent2D					m_Extent = {0, 0};
			std::vector<VkImage>		m_Images;
			std::vector<VkImageView>	m_Views;
			std::vector<VkSemaphore>	m_RenderDone;		// per image
			std::vector<VkSemaphore>	m_AcquireSemaphores;	// rotating
			std::vector<uint64_t>		m_AcquireValues;	// timeline value that freed each
			uint32_t					m_NextAcquire = 0;
			uint32_t					m_AcquireUsed = 0;
			uint32_t					m_Current = 0;
			bool						m_bAcquired = false;
			bool						m_bRecreate = true;
			bool						m_bReadBack = false;
			std::vector<uint8_t>		m_Pixels;
	};
}
