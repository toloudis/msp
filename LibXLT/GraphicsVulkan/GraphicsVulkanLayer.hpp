/*****************************************************************************
**  GraphicsVulkanLayer.hpp
**
**      GraphicsVulkanLayer initializes the GraphicsVulkan library: the
**	Vulkan implementation of the Graphics abstractions (the counterpart of
**	GraphicsDX11Layer, with the same calls). Init can fail (no suitable
**	Vulkan device, missing validation layer in Debug, ...); GetInitError
**	then says why.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_GRAPHICSVULKANLAYER_HPP
#error GraphicsVulkanLayer.hpp multiply included
#endif
#define GVK_GRAPHICSVULKANLAYER_HPP

#ifndef GVK_VKDVALIDATIONSTATS_HPP
#include "GraphicsVulkan/vkd/vkdValidationStats.hpp"
#endif

#include <array>
#include <cstdint>
#include <optional>
#include <string>

class g2dSystem;
class g3dSystem;
struct g3dGfxOptions;

class GraphicsVulkanLayer
{
	public:
		//------------------------------------------------------------------------
		//	Init - creates the Vulkan device and the 2D/3D systems. Returns
		//	false (and GetInitError says why) if Vulkan can't be used.
		//------------------------------------------------------------------------
		static bool Init(void* i_hWnd, const g3dGfxOptions& i_Options);

		//------------------------------------------------------------------------
		//	What a layer stacked on this one (GraphicsOptiX) needs of the
		//	device: a GPU it already uses, by UUID (m_DeviceName names it in
		//	messages), and memory and semaphores it can share
		//	(vkdDeviceOptions::m_bExternalMemory)
		//------------------------------------------------------------------------
		struct Interop
		{
			std::optional<std::array<uint8_t, 16>>	m_DeviceUUID;
			std::string								m_DeviceName;
			bool									m_bExternalMemory = false;
		};
		static bool Init(void* i_hWnd, const g3dGfxOptions& i_Options, const Interop& i_Interop);

		//------------------------------------------------------------------------
		//	CleanUp
		//------------------------------------------------------------------------
		static void CleanUp() throw();

		//------------------------------------------------------------------------
		//	InitGraphics - should be called after device is created
		//------------------------------------------------------------------------
		static void InitGraphics();

		//------------------------------------------------------------------------
		//	CleanUpGraphics - should be called before device is destroyed
		//------------------------------------------------------------------------
		static void CleanUpGraphics();

		//------------------------------------------------------------------------
		//	Accessors for the major graphics system objects
		//------------------------------------------------------------------------
		static g2dSystem* GetSystem2D();
		static g3dSystem* GetSystem3D();

		//------------------------------------------------------------------------
		//	GetName - the backend's name for people ("Vulkan");
		//	GetCodeName - its name in command lines, logs and files ("vulkan")
		//------------------------------------------------------------------------
		static const char* GetName();
		static const char* GetCodeName();

		//------------------------------------------------------------------------
		//	GetInitError - why Init failed (empty if it didn't)
		//------------------------------------------------------------------------
		static const std::string& GetInitError();

		//------------------------------------------------------------------------
		//	GetValidationStats - Vulkan validation messages so far (counted
		//	across Init..CleanUp, including device destruction)
		//------------------------------------------------------------------------
		static gvk::vkdValidationStats GetValidationStats();

		//------------------------------------------------------------------------
		//	IsValidating - whether the validation layer ran (Debug builds, or
		//	-gfxValidation); the counts mean nothing otherwise. Kept after
		//	CleanUp, like the counts.
		//------------------------------------------------------------------------
		static bool IsValidating();
};
