/*****************************************************************************
**  vkdDevice.hpp
**
**      vkdDevice owns the Vulkan instance, the chosen physical device, the
**	logical device and its queue, Vulkan Memory Allocator, and a command
**	stream with DX11 immediate-context semantics: Commands() returns the
**	open command buffer (opening one if needed) and Submit() closes and
**	submits it. Up to c_FramesInFlight command buffers are in flight,
**	tracked by one timeline semaphore; DeferDestroy() runs clean-up once
**	the GPU has finished with what was submitted so far.
**
**	Validation follows the GraphicsVulkan policy: always on in Debug (the
**	layer is required there), on in Release with m_bValidation; errors
**	DBG_ASSERT, warnings are logged and counted (see vkdValidation.hpp).
**
**	Threads: recording and Submit() from more than one thread must hold
**	StreamLock() for the whole record-and-submit; every queue operation
**	(submit, present) holds QueueLock().
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDDEVICE_HPP
#error vkdDevice.hpp multiply included
#endif
#define GVK_VKDDEVICE_HPP

#ifndef GVK_VKDADAPTER_HPP
#include "GraphicsVulkan/vkd/vkdAdapter.hpp"
#endif
#ifndef GVK_VKDVALIDATION_HPP
#include "GraphicsVulkan/vkd/vkdValidation.hpp"
#endif

#include <volk.h>

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

typedef struct VmaAllocator_T* VmaAllocator;

namespace gvk
{
	struct vkdDeviceOptions
	{
		bool		m_bValidation = false;		// forced on in Debug builds
		bool		m_bGpuAssisted = false;		// GPU-assisted validation (tests and sign-off runs)
		std::string	m_AdapterOverride;			// MSP_GFX_VK_ADAPTER: device name substring; empty = automatic
		vkdValidation* m_pValidation = nullptr;	// counts validation messages; must outlive the device,
												// so messages during destruction are counted too.
												// NULL: the device keeps its own.
	};

	class vkdDevice
	{
		public:
			static const uint32_t c_FramesInFlight = 3;

			//	Create returns NULL and o_Error says why if Vulkan can't be used
			//	(no loader, no suitable device, validation layer missing in Debug, ...).
			static vkdDevice* Create(const vkdDeviceOptions& i_Options, std::string& o_Error);

			//	Waits for the GPU, runs all deferred destruction, then destroys
			//	everything in reverse order of creation.
			~vkdDevice();

			VkInstance				Instance() const;
			VkPhysicalDevice		Physical() const;
			VkDevice				Device() const;
			VkQueue					Queue() const;
			uint32_t				QueueFamily() const;
			VmaAllocator			Allocator() const;
			const VkPhysicalDeviceProperties& Properties() const;
			const vkdAdapterInfo&	Adapter() const;

			//------------------------------------------------------------------
			//	Command stream
			//------------------------------------------------------------------
			//	The open command buffer, opened on first use.
			VkCommandBuffer Commands();

			//	Closes and submits the open command buffer (an empty one if none
			//	is open, so waits and signals still happen), signalling the
			//	timeline. Returns the timeline value of this submission.
			uint64_t Submit(const std::vector<VkSemaphoreSubmitInfo>& i_Wait = {},
							const std::vector<VkSemaphoreSubmitInfo>& i_Signal = {});

			void		WaitFor(uint64_t i_TimelineValue);
			void		WaitIdle();
			uint64_t	CompletedValue() const;

			//	The timeline value the open command buffer will get when it is
			//	submitted: anything used in it is free once this completes.
			uint64_t	PendingValue() const;

			//	Called (under the stream lock) whenever Commands() is about to
			//	hand out the open command buffer, Submit() included, so the code
			//	that records state into it (vkdContext) can end what it has open
			//	first. One hook; empty removes it.
			void SetRecordingHook(std::function<void()> i_Hook);

			//	Runs i_Destroy once the GPU has finished all work submitted so
			//	far, including the open command buffer.
			void DeferDestroy(std::function<void()> i_Destroy);

			std::recursive_mutex&	StreamLock();
			std::mutex&				QueueLock();

			//------------------------------------------------------------------
			//	Debugging
			//------------------------------------------------------------------
			void SetName(VkObjectType i_Type, uint64_t i_Handle, const char* i_Name);
			void BeginLabel(VkCommandBuffer i_Commands, const char* i_Name);
			void EndLabel(VkCommandBuffer i_Commands);

			vkdValidationStats ValidationStats() const;
			//	Whether the validation layer is on (Debug builds, or asked for)
			bool IsValidating() const;
			void ResetValidationStats();

		private:
			vkdDevice();
			vkdDevice(const vkdDevice&) = delete;
			vkdDevice& operator = (const vkdDevice&) = delete;

			struct Impl;
			std::unique_ptr<Impl> m_pImpl;
	};
}
