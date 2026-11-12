/*****************************************************************************
**  optxContext.hpp
**
**      optxContext is GraphicsOptiX's one GPU: a CUDA device, its primary
**	context, one stream all work goes on, and the OptixDeviceContext.
**	GraphicsOptiXLayer::Init creates it (after loading CUDA and OptiX) and
**	CleanUp destroys it.
**
**	The device is the first CUDA device (CUDA lists the fastest first)
**	that OptiX IR compiled for compute_75 runs on, or, if the environment
**	variable MSP_GFX_OPTIX_DEVICE is set, the first whose name contains
**	it (ignoring case).
**
**	A CUDA context is current per thread: code that calls CUDA on a
**	thread calls MakeCurrent first (cheap).
**
**	OptiX's messages go to the log ("OptiX [tag]: ..."); errors are
**	counted (GetErrorCount), for tests. Validation mode is on in Debug
**	builds and with -gfxValidation.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXCONTEXT_HPP
#error optxContext.hpp multiply included
#endif
#define GOX_OPTXCONTEXT_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif

#include <array>
#include <cstdint>
#include <string>

namespace gox
{
	class optxContext
	{
		public:
			//--------------------------------------------------------------------
			//	Create - cuLoader and optxLoader must have loaded. Returns false
			//	(o_Error says why) if there is no usable device.
			//--------------------------------------------------------------------
			static bool Create(bool i_bValidation, std::string& o_Error);

			//--------------------------------------------------------------------
			//	Destroy - waits for the GPU, then releases everything. Device
			//	memory still allocated is released with the context.
			//--------------------------------------------------------------------
			static void Destroy() throw();

			static bool Exists();

			//--------------------------------------------------------------------
			//	MakeCurrent - makes the CUDA context current on this thread
			//--------------------------------------------------------------------
			static void MakeCurrent();

			//--------------------------------------------------------------------
			//	Synchronize - waits until the stream's work is done
			//--------------------------------------------------------------------
			static bool Synchronize();

			static OptixDeviceContext GetOptix();
			static CUcontext GetCuda();
			static CUstream GetStream();
			static CUdevice GetDevice();

			//--------------------------------------------------------------------
			//	GetDeviceName - "NVIDIA GeForce RTX ..."
			//--------------------------------------------------------------------
			static const std::string& GetDeviceName();

			//--------------------------------------------------------------------
			//	GetDeviceUUID - the device's UUID (cuDeviceGetUuid), which
			//	Vulkan reports for the same GPU (VkPhysicalDeviceIDProperties)
			//--------------------------------------------------------------------
			static const std::array<uint8_t, 16>& GetDeviceUUID();

			//--------------------------------------------------------------------
			//	GetErrorCount - OptiX messages at fatal or error level so far;
			//	GetWarningCount - at warning level
			//--------------------------------------------------------------------
			static int GetErrorCount();
			static int GetWarningCount();
	};
}
