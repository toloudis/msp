/*****************************************************************************
**  vkdDevice.cpp
**
**      See vkdDevice.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef GVK_VKDSURFACE_HPP
#include "GraphicsVulkan/vkd/vkdSurface.hpp"
#endif
#ifndef GVK_VKDVMA_HPP
#include "GraphicsVulkan/vkd/private/vkdVma.hpp"
#endif

#include <cstring>
#include <deque>

namespace gvk
{
	namespace
	{
		const char* const c_ValidationLayer = "VK_LAYER_KHRONOS_validation";

#ifdef _DEBUG
		const bool c_bDebugBuild = true;
#else
		const bool c_bDebugBuild = false;
#endif

		bool has_layer(const char* i_Name)
		{
			uint32_t count = 0;
			vkEnumerateInstanceLayerProperties(&count, NULL);
			std::vector<VkLayerProperties> layers(count);
			vkEnumerateInstanceLayerProperties(&count, layers.data());
			for (const VkLayerProperties& layer : layers)
			{
				if (strcmp(layer.layerName, i_Name) == 0)
				{
					return true;
				}
			}
			return false;
		}

		bool has_instance_extension(const char* i_Layer, const char* i_Name)
		{
			uint32_t count = 0;
			vkEnumerateInstanceExtensionProperties(i_Layer, &count, NULL);
			std::vector<VkExtensionProperties> extensions(count);
			vkEnumerateInstanceExtensionProperties(i_Layer, &count, extensions.data());
			for (const VkExtensionProperties& extension : extensions)
			{
				if (strcmp(extension.extensionName, i_Name) == 0)
				{
					return true;
				}
			}
			return false;
		}

		bool listed(const std::vector<std::string>& i_List, const char* i_Name)
		{
			for (const std::string& name : i_List)
			{
				if (name == i_Name)
				{
					return true;
				}
			}
			return false;
		}

		//	The feature structs GraphicsVulkan queries and enables, chained.
		struct FeatureChain
		{
			VkPhysicalDeviceFeatures2						m_Features2 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
			VkPhysicalDeviceVulkan12Features				m_Vulkan12 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
			VkPhysicalDeviceVulkan13Features				m_Vulkan13 = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
			VkPhysicalDeviceDescriptorHeapFeaturesEXT		m_Heap = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_EXT};
			VkPhysicalDeviceUnifiedImageLayoutsFeaturesKHR	m_Unified = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_UNIFIED_IMAGE_LAYOUTS_FEATURES_KHR};

			//	The extension structs are chained only when the device has the extension.
			void Link(bool i_bHeap, bool i_bUnified)
			{
				m_Features2.pNext = &m_Vulkan12;
				m_Vulkan12.pNext = &m_Vulkan13;
				void** next = &m_Vulkan13.pNext;
				if (i_bHeap)
				{
					*next = &m_Heap;
					next = &m_Heap.pNext;
				}
				if (i_bUnified)
				{
					*next = &m_Unified;
					next = &m_Unified.pNext;
				}
				*next = NULL;
			}
		};

		vkdAdapterInfo describe(VkPhysicalDevice i_Physical)
		{
			vkdAdapterInfo info;
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(i_Physical, &properties);
			info.m_Name = properties.deviceName;
			info.m_bDiscrete = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
			info.m_ApiVersion = properties.apiVersion;

			VkPhysicalDeviceMemoryProperties memory;
			vkGetPhysicalDeviceMemoryProperties(i_Physical, &memory);
			for (uint32_t i = 0; i < memory.memoryHeapCount; ++i)
			{
				if (memory.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
				{
					info.m_DeviceLocalBytes += memory.memoryHeaps[i].size;
				}
			}

			uint32_t count = 0;
			vkEnumerateDeviceExtensionProperties(i_Physical, NULL, &count, NULL);
			std::vector<VkExtensionProperties> extensions(count);
			vkEnumerateDeviceExtensionProperties(i_Physical, NULL, &count, extensions.data());
			for (const VkExtensionProperties& extension : extensions)
			{
				info.m_Extensions.push_back(extension.extensionName);
			}

			FeatureChain chain;
			chain.Link(listed(info.m_Extensions, VK_EXT_DESCRIPTOR_HEAP_EXTENSION_NAME),
					   listed(info.m_Extensions, VK_KHR_UNIFIED_IMAGE_LAYOUTS_EXTENSION_NAME));
			if (properties.apiVersion >= VK_API_VERSION_1_3)
			{
				vkGetPhysicalDeviceFeatures2(i_Physical, &chain.m_Features2);
			}
			const struct { const char* m_pName; VkBool32 m_bValue; } features[] =
			{
				{"geometryShader",		chain.m_Features2.features.geometryShader},
				{"tessellationShader",	chain.m_Features2.features.tessellationShader},
				{"dynamicRendering",	chain.m_Vulkan13.dynamicRendering},
				{"synchronization2",	chain.m_Vulkan13.synchronization2},
				{"timelineSemaphore",	chain.m_Vulkan12.timelineSemaphore},
				{"bufferDeviceAddress",	chain.m_Vulkan12.bufferDeviceAddress},
				{"descriptorHeap",		chain.m_Heap.descriptorHeap},
				{"unifiedImageLayouts",	chain.m_Unified.unifiedImageLayouts},
			};
			for (const auto& feature : features)
			{
				if (feature.m_bValue)
				{
					info.m_Features.push_back(feature.m_pName);
				}
			}
			return info;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	struct vkdDevice::Impl
	{
		vkdDeviceOptions			m_Options;
		vkdValidation				m_OwnValidation;
		vkdValidation*				m_pValidation = NULL;

		VkInstance					m_Instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT	m_Messenger = VK_NULL_HANDLE;
		bool						m_bDebugUtils = false;

		VkPhysicalDevice			m_Physical = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties	m_Properties = {};
		vkdAdapterInfo				m_Adapter;

		VkDevice					m_Device = VK_NULL_HANDLE;
		VkQueue						m_Queue = VK_NULL_HANDLE;
		uint32_t					m_QueueFamily = 0;
		VmaAllocator				m_Allocator = NULL;

		VkSemaphore					m_Timeline = VK_NULL_HANDLE;
		uint64_t					m_LastSubmitted = 0;

		struct Slot
		{
			VkCommandPool	m_Pool = VK_NULL_HANDLE;
			VkCommandBuffer	m_Commands = VK_NULL_HANDLE;
			uint64_t		m_Value = 0;
		};
		Slot						m_Slots[c_FramesInFlight];
		uint32_t					m_Slot = 0;
		bool						m_bOpen = false;

		struct Deferred
		{
			uint64_t				m_Value;
			std::function<void()>	m_Destroy;
		};
		std::deque<Deferred>		m_Deferred;

		std::recursive_mutex		m_StreamLock;
		std::mutex					m_QueueLock;
		std::mutex					m_DeferLock;

		bool Init(const vkdDeviceOptions& i_Options, std::string& o_Error);
		void Destroy();
		void RunDeferred(bool i_bAll);
		uint64_t Completed() const;
	};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool vkdDevice::Impl::Init(const vkdDeviceOptions& i_Options, std::string& o_Error)
	{
		m_Options = i_Options;
		m_pValidation = i_Options.m_pValidation ? i_Options.m_pValidation : &m_OwnValidation;

		if (volkInitialize() != VK_SUCCESS)
		{
			o_Error = "The Vulkan loader (vulkan-1) isn't installed; install or update the graphics driver.";
			return false;
		}
		if (volkGetInstanceVersion() < VK_API_VERSION_1_4)
		{
			o_Error = "The Vulkan loader is older than Vulkan 1.4; update the graphics driver or the Vulkan runtime.";
			return false;
		}

		//	Instance: validation per the policy
		const bool want_validation = c_bDebugBuild || i_Options.m_bValidation || i_Options.m_bGpuAssisted;
		if (want_validation && !has_layer(c_ValidationLayer))
		{
			o_Error = std::string(c_ValidationLayer) + " isn't installed, and " +
					  (c_bDebugBuild ? "Debug builds always validate" : "validation was asked for (-gfxValidation)") +
					  "; install the Vulkan SDK.";
			return false;
		}
		std::vector<const char*> layers;
		std::vector<const char*> extensions;
		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		extensions.push_back(vkdPlatformSurfaceExtension());
		m_bDebugUtils = has_instance_extension(NULL, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		if (m_bDebugUtils)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		const bool layer_settings = want_validation && has_instance_extension(c_ValidationLayer, VK_EXT_LAYER_SETTINGS_EXTENSION_NAME);
		if (want_validation)
		{
			layers.push_back(c_ValidationLayer);
			if (layer_settings)
			{
				extensions.push_back(VK_EXT_LAYER_SETTINGS_EXTENSION_NAME);
			}
		}

		VkDebugUtilsMessengerCreateInfoEXT messenger_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
		messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
										 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
										 VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
		messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		messenger_info.pfnUserCallback = &vkdValidation::Callback;
		messenger_info.pUserData = m_pValidation;

		const VkBool32 c_True = VK_TRUE;
		std::vector<VkLayerSettingEXT> settings;
		settings.push_back({c_ValidationLayer, "validate_sync", VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &c_True});
		if (i_Options.m_bGpuAssisted)
		{
			settings.push_back({c_ValidationLayer, "gpuav_enable", VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &c_True});
		}
		VkLayerSettingsCreateInfoEXT settings_info = {VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT};
		settings_info.settingCount = static_cast<uint32_t>(settings.size());
		settings_info.pSettings = settings.data();

		const void* instance_next = NULL;
		if (want_validation && m_bDebugUtils)
		{
			messenger_info.pNext = layer_settings ? &settings_info : NULL;
			instance_next = &messenger_info;
		}
		else if (layer_settings)
		{
			instance_next = &settings_info;
		}

		VkApplicationInfo app = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
		app.pApplicationName = "MachStudio";
		app.pEngineName = "LibXLT GraphicsVulkan";
		app.apiVersion = VK_API_VERSION_1_4;
		VkInstanceCreateInfo instance_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
		instance_info.pNext = instance_next;
		instance_info.pApplicationInfo = &app;
		instance_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
		instance_info.ppEnabledLayerNames = layers.data();
		instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instance_info.ppEnabledExtensionNames = extensions.data();
		VkResult result = vkCreateInstance(&instance_info, NULL, &m_Instance);
		if (result != VK_SUCCESS)
		{
			o_Error = std::string("vkCreateInstance failed: ") + vkdResultName(result);
			return false;
		}
		volkLoadInstance(m_Instance);
		if (want_validation && m_bDebugUtils)
		{
			messenger_info.pNext = NULL;
			VK_CHECK(vkCreateDebugUtilsMessengerEXT(m_Instance, &messenger_info, NULL, &m_Messenger));
		}

		//	Device choice
		uint32_t count = 0;
		vkEnumeratePhysicalDevices(m_Instance, &count, NULL);
		std::vector<VkPhysicalDevice> physicals(count);
		vkEnumeratePhysicalDevices(m_Instance, &count, physicals.data());
		std::vector<vkdAdapterInfo> adapters;
		for (VkPhysicalDevice physical : physicals)
		{
			adapters.push_back(describe(physical));
		}
		int chosen = vkdSelectAdapter(adapters, i_Options.m_AdapterOverride, o_Error);
		if (chosen < 0)
		{
			return false;
		}
		m_Physical = physicals[chosen];
		m_Adapter = adapters[chosen];
		vkGetPhysicalDeviceProperties(m_Physical, &m_Properties);
		DBG_LOG("Vulkan device: " << m_Adapter.m_Name << (m_Adapter.m_bDiscrete ? " (discrete, " : " (")
				<< (m_Adapter.m_DeviceLocalBytes >> 20) << " MB device-local), Vulkan "
				<< VK_API_VERSION_MAJOR(m_Properties.apiVersion) << "." << VK_API_VERSION_MINOR(m_Properties.apiVersion)
				<< "." << VK_API_VERSION_PATCH(m_Properties.apiVersion) << ", driver " << m_Properties.driverVersion);

		//	Queue: one graphics + compute queue
		uint32_t family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_Physical, &family_count, NULL);
		std::vector<VkQueueFamilyProperties> families(family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(m_Physical, &family_count, families.data());
		bool found = false;
		for (uint32_t i = 0; i < family_count && !found; ++i)
		{
			const VkQueueFlags needed = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
			if ((families[i].queueFlags & needed) == needed)
			{
				m_QueueFamily = i;
				found = true;
			}
		}
		if (!found)
		{
			o_Error = m_Adapter.m_Name + " has no graphics and compute queue";
			return false;
		}

		//	Device: exactly the required features and extensions
		FeatureChain chain;
		chain.Link(true, true);
		chain.m_Features2.features.geometryShader = VK_TRUE;
		chain.m_Features2.features.tessellationShader = VK_TRUE;
		chain.m_Vulkan12.timelineSemaphore = VK_TRUE;
		chain.m_Vulkan12.bufferDeviceAddress = VK_TRUE;
		chain.m_Vulkan13.dynamicRendering = VK_TRUE;
		chain.m_Vulkan13.synchronization2 = VK_TRUE;
		chain.m_Heap.descriptorHeap = VK_TRUE;
		chain.m_Unified.unifiedImageLayouts = VK_TRUE;

		std::vector<const char*> device_extensions;
		for (const std::string& name : vkdRequiredExtensions())
		{
			device_extensions.push_back(name.c_str());
		}
		const float priority = 1.0f;
		VkDeviceQueueCreateInfo queue_info = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
		queue_info.queueFamilyIndex = m_QueueFamily;
		queue_info.queueCount = 1;
		queue_info.pQueuePriorities = &priority;
		VkDeviceCreateInfo device_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
		device_info.pNext = &chain.m_Features2;
		device_info.queueCreateInfoCount = 1;
		device_info.pQueueCreateInfos = &queue_info;
		device_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
		device_info.ppEnabledExtensionNames = device_extensions.data();
		result = vkCreateDevice(m_Physical, &device_info, NULL, &m_Device);
		if (result != VK_SUCCESS)
		{
			o_Error = std::string("vkCreateDevice failed on ") + m_Adapter.m_Name + ": " + vkdResultName(result);
			return false;
		}
		volkLoadDevice(m_Device);
		vkGetDeviceQueue(m_Device, m_QueueFamily, 0, &m_Queue);

		//	Memory
		VmaVulkanFunctions functions = {};
		functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
		VmaAllocatorCreateInfo allocator_info = {};
		allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT | VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
		allocator_info.physicalDevice = m_Physical;
		allocator_info.device = m_Device;
		allocator_info.instance = m_Instance;
		allocator_info.vulkanApiVersion = VK_API_VERSION_1_3;
		allocator_info.pVulkanFunctions = &functions;
		result = vmaCreateAllocator(&allocator_info, &m_Allocator);
		if (result != VK_SUCCESS)
		{
			o_Error = std::string("vmaCreateAllocator failed: ") + vkdResultName(result);
			return false;
		}

		//	Command stream
		VkSemaphoreTypeCreateInfo timeline_type = {VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO};
		timeline_type.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
		VkSemaphoreCreateInfo semaphore_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
		semaphore_info.pNext = &timeline_type;
		VK_CHECK(vkCreateSemaphore(m_Device, &semaphore_info, NULL, &m_Timeline));
		for (Slot& slot : m_Slots)
		{
			VkCommandPoolCreateInfo pool_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
			pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			pool_info.queueFamilyIndex = m_QueueFamily;
			VK_CHECK(vkCreateCommandPool(m_Device, &pool_info, NULL, &slot.m_Pool));
			VkCommandBufferAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
			allocate_info.commandPool = slot.m_Pool;
			allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocate_info.commandBufferCount = 1;
			VK_CHECK(vkAllocateCommandBuffers(m_Device, &allocate_info, &slot.m_Commands));
		}
		return true;
	}

	uint64_t vkdDevice::Impl::Completed() const
	{
		uint64_t value = 0;
		if (m_Timeline)
		{
			vkGetSemaphoreCounterValue(m_Device, m_Timeline, &value);
		}
		return value;
	}

	void vkdDevice::Impl::RunDeferred(bool i_bAll)
	{
		std::vector<std::function<void()>> ready;
		{
			std::lock_guard<std::mutex> lock(m_DeferLock);
			const uint64_t completed = i_bAll ? UINT64_MAX : Completed();
			while (!m_Deferred.empty() && m_Deferred.front().m_Value <= completed)
			{
				ready.push_back(std::move(m_Deferred.front().m_Destroy));
				m_Deferred.pop_front();
			}
		}
		for (std::function<void()>& destroy : ready)
		{
			destroy();
		}
	}

	void vkdDevice::Impl::Destroy()
	{
		if (m_Device)
		{
			vkDeviceWaitIdle(m_Device);
			RunDeferred(true);
			for (Slot& slot : m_Slots)
			{
				if (slot.m_Pool)
				{
					vkDestroyCommandPool(m_Device, slot.m_Pool, NULL);
				}
			}
			if (m_Timeline)
			{
				vkDestroySemaphore(m_Device, m_Timeline, NULL);
			}
			if (m_Allocator)
			{
				vmaDestroyAllocator(m_Allocator);
			}
			vkDestroyDevice(m_Device, NULL);
			m_Device = VK_NULL_HANDLE;
		}
		if (m_Messenger)
		{
			vkDestroyDebugUtilsMessengerEXT(m_Instance, m_Messenger, NULL);
			m_Messenger = VK_NULL_HANDLE;
		}
		if (m_Instance)
		{
			vkDestroyInstance(m_Instance, NULL);
			m_Instance = VK_NULL_HANDLE;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	vkdDevice::vkdDevice()
		: m_pImpl(new Impl)
	{
	}

	vkdDevice* vkdDevice::Create(const vkdDeviceOptions& i_Options, std::string& o_Error)
	{
		vkdDevice* device = new vkdDevice;
		if (!device->m_pImpl->Init(i_Options, o_Error))
		{
			device->m_pImpl->Destroy();
			delete device;
			return NULL;
		}
		return device;
	}

	vkdDevice::~vkdDevice()
	{
		{
			std::lock_guard<std::recursive_mutex> lock(m_pImpl->m_StreamLock);
			if (m_pImpl->m_bOpen)
			{
				Submit();
			}
		}
		m_pImpl->Destroy();
	}

	VkInstance vkdDevice::Instance() const					{ return m_pImpl->m_Instance; }
	VkPhysicalDevice vkdDevice::Physical() const			{ return m_pImpl->m_Physical; }
	VkDevice vkdDevice::Device() const						{ return m_pImpl->m_Device; }
	VkQueue vkdDevice::Queue() const						{ return m_pImpl->m_Queue; }
	uint32_t vkdDevice::QueueFamily() const					{ return m_pImpl->m_QueueFamily; }
	VmaAllocator vkdDevice::Allocator() const				{ return m_pImpl->m_Allocator; }
	const VkPhysicalDeviceProperties& vkdDevice::Properties() const { return m_pImpl->m_Properties; }
	const vkdAdapterInfo& vkdDevice::Adapter() const		{ return m_pImpl->m_Adapter; }
	std::recursive_mutex& vkdDevice::StreamLock()			{ return m_pImpl->m_StreamLock; }
	std::mutex& vkdDevice::QueueLock()						{ return m_pImpl->m_QueueLock; }

	//------------------------------------------------------------------------
	//	Command stream
	//------------------------------------------------------------------------
	VkCommandBuffer vkdDevice::Commands()
	{
		std::lock_guard<std::recursive_mutex> lock(m_pImpl->m_StreamLock);
		Impl::Slot& slot = m_pImpl->m_Slots[m_pImpl->m_Slot];
		if (!m_pImpl->m_bOpen)
		{
			WaitFor(slot.m_Value);
			VK_CHECK(vkResetCommandPool(m_pImpl->m_Device, slot.m_Pool, 0));
			VkCommandBufferBeginInfo begin = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
			begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			VK_CHECK(vkBeginCommandBuffer(slot.m_Commands, &begin));
			m_pImpl->m_bOpen = true;
		}
		return slot.m_Commands;
	}

	uint64_t vkdDevice::Submit(const std::vector<VkSemaphoreSubmitInfo>& i_Wait,
							   const std::vector<VkSemaphoreSubmitInfo>& i_Signal)
	{
		std::lock_guard<std::recursive_mutex> lock(m_pImpl->m_StreamLock);
		VkCommandBuffer commands = Commands();
		VK_CHECK(vkEndCommandBuffer(commands));

		const uint64_t value = ++m_pImpl->m_LastSubmitted;
		std::vector<VkSemaphoreSubmitInfo> signals(i_Signal);
		VkSemaphoreSubmitInfo timeline = {VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO};
		timeline.semaphore = m_pImpl->m_Timeline;
		timeline.value = value;
		timeline.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		signals.push_back(timeline);

		VkCommandBufferSubmitInfo command_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO};
		command_info.commandBuffer = commands;
		VkSubmitInfo2 submit = {VK_STRUCTURE_TYPE_SUBMIT_INFO_2};
		submit.waitSemaphoreInfoCount = static_cast<uint32_t>(i_Wait.size());
		submit.pWaitSemaphoreInfos = i_Wait.data();
		submit.commandBufferInfoCount = 1;
		submit.pCommandBufferInfos = &command_info;
		submit.signalSemaphoreInfoCount = static_cast<uint32_t>(signals.size());
		submit.pSignalSemaphoreInfos = signals.data();
		{
			std::lock_guard<std::mutex> queue_lock(m_pImpl->m_QueueLock);
			VK_CHECK(vkQueueSubmit2(m_pImpl->m_Queue, 1, &submit, VK_NULL_HANDLE));
		}

		m_pImpl->m_Slots[m_pImpl->m_Slot].m_Value = value;
		m_pImpl->m_Slot = (m_pImpl->m_Slot + 1) % c_FramesInFlight;
		m_pImpl->m_bOpen = false;
		m_pImpl->RunDeferred(false);
		return value;
	}

	void vkdDevice::WaitFor(uint64_t i_TimelineValue)
	{
		if (i_TimelineValue == 0)
		{
			return;
		}
		VkSemaphoreWaitInfo wait = {VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO};
		wait.semaphoreCount = 1;
		wait.pSemaphores = &m_pImpl->m_Timeline;
		wait.pValues = &i_TimelineValue;
		VK_CHECK(vkWaitSemaphores(m_pImpl->m_Device, &wait, UINT64_MAX));
		m_pImpl->RunDeferred(false);
	}

	void vkdDevice::WaitIdle()
	{
		{
			std::lock_guard<std::mutex> queue_lock(m_pImpl->m_QueueLock);
			VK_CHECK(vkQueueWaitIdle(m_pImpl->m_Queue));
		}
		m_pImpl->RunDeferred(false);
	}

	uint64_t vkdDevice::CompletedValue() const
	{
		return m_pImpl->Completed();
	}

	void vkdDevice::DeferDestroy(std::function<void()> i_Destroy)
	{
		std::lock_guard<std::mutex> lock(m_pImpl->m_DeferLock);
		//	The next submission includes the open command buffer, which may use it.
		Impl::Deferred deferred;
		deferred.m_Value = m_pImpl->m_LastSubmitted + 1;
		deferred.m_Destroy = std::move(i_Destroy);
		m_pImpl->m_Deferred.push_back(std::move(deferred));
	}

	//------------------------------------------------------------------------
	//	Debugging
	//------------------------------------------------------------------------
	void vkdDevice::SetName(VkObjectType i_Type, uint64_t i_Handle, const char* i_Name)
	{
		if (m_pImpl->m_bDebugUtils && i_Handle && i_Name)
		{
			VkDebugUtilsObjectNameInfoEXT name = {VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
			name.objectType = i_Type;
			name.objectHandle = i_Handle;
			name.pObjectName = i_Name;
			vkSetDebugUtilsObjectNameEXT(m_pImpl->m_Device, &name);
		}
	}

	void vkdDevice::BeginLabel(VkCommandBuffer i_Commands, const char* i_Name)
	{
		if (m_pImpl->m_bDebugUtils)
		{
			VkDebugUtilsLabelEXT label = {VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT};
			label.pLabelName = i_Name;
			vkCmdBeginDebugUtilsLabelEXT(i_Commands, &label);
		}
	}

	void vkdDevice::EndLabel(VkCommandBuffer i_Commands)
	{
		if (m_pImpl->m_bDebugUtils)
		{
			vkCmdEndDebugUtilsLabelEXT(i_Commands);
		}
	}

	vkdValidationStats vkdDevice::ValidationStats() const
	{
		return m_pImpl->m_pValidation->Stats();
	}

	void vkdDevice::ResetValidationStats()
	{
		m_pImpl->m_pValidation->Reset();
	}
}
