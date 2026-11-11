/*****************************************************************************
**  optxContext.cpp
**
**      See optxContext.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GOX_CULOADER_HPP
#include "GraphicsOptiX/cu/cuLoader.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif

#include <atomic>
#include <cstdlib>
#include <string>
#include <vector>

namespace gox
{
	namespace
	{
		//	The device programs are compiled for compute_75 (EmbedProgramsOptiX.ps1)
		const int c_MinComputeCapability = 75;

		bool				l_bExists = false;
		CUdevice			l_Device = 0;
		CUcontext			l_Cuda = NULL;
		CUstream			l_Stream = NULL;
		OptixDeviceContext	l_Optix = NULL;
		std::string			l_DeviceName;
		std::atomic<int>	l_nErrors(0);
		std::atomic<int>	l_nWarnings(0);

		struct DeviceInfo
		{
			CUdevice	m_Device;
			std::string	m_Name;
			int			m_ComputeCapability;	// 75 for 7.5
		};

		void log_callback(unsigned int i_Level, const char* i_Tag, const char* i_Message, void* /*i_pData*/)
		{
			//	1 fatal, 2 error, 3 warning, 4 print
			if (i_Level <= 2)
			{
				++l_nErrors;
			}
			else if (i_Level == 3)
			{
				++l_nWarnings;
			}
			DBG_LOG("OptiX [" << (i_Tag ? i_Tag : "") << "]: " << (i_Message ? i_Message : ""));
		}

		std::string describe(const std::vector<DeviceInfo>& i_Devices)
		{
			std::string text;
			for (const DeviceInfo& d : i_Devices)
			{
				text += (text.empty() ? "" : ", ") + d.m_Name + " (compute " + std::to_string(d.m_ComputeCapability / 10) +
						"." + std::to_string(d.m_ComputeCapability % 10) + ")";
			}
			return text.empty() ? "none" : text;
		}

		bool list_devices(std::vector<DeviceInfo>& o_Devices, std::string& o_Error)
		{
			o_Devices.clear();
			int count = 0;
			if (CU_CHECK(cu().cuDeviceGetCount(&count)))
			{
				o_Error = "CUDA could not list its devices";
				return false;
			}
			for (int i = 0; i < count; ++i)
			{
				DeviceInfo info;
				char name[256] = {};
				int major = 0, minor = 0;
				if (CU_CHECK(cu().cuDeviceGet(&info.m_Device, i)) ||
					CU_CHECK(cu().cuDeviceGetName(name, int(sizeof(name)), info.m_Device)) ||
					CU_CHECK(cu().cuDeviceGetAttribute(&major, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, info.m_Device)) ||
					CU_CHECK(cu().cuDeviceGetAttribute(&minor, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, info.m_Device)))
				{
					continue;
				}
				info.m_Name = name;
				info.m_ComputeCapability = major * 10 + minor;
				o_Devices.push_back(info);
			}
			return true;
		}

		bool choose_device(const std::vector<DeviceInfo>& i_Devices, DeviceInfo& o_Device, std::string& o_Error)
		{
			std::string wanted;
			if (const char* env = std::getenv("MSP_GFX_OPTIX_DEVICE"))
			{
				wanted = env;
				envString::ToLower(wanted);
			}
			bool name_matched = false;
			for (const DeviceInfo& d : i_Devices)
			{
				std::string name = d.m_Name;
				envString::ToLower(name);
				if (!wanted.empty() && name.find(wanted) == std::string::npos)
				{
					continue;
				}
				name_matched = true;
				if (d.m_ComputeCapability >= c_MinComputeCapability)
				{
					o_Device = d;
					return true;
				}
			}
			if (!wanted.empty() && !name_matched)
			{
				o_Error = "no CUDA device matches MSP_GFX_OPTIX_DEVICE=\"" + std::string(std::getenv("MSP_GFX_OPTIX_DEVICE")) +
						  "\". Devices: " + describe(i_Devices);
			}
			else
			{
				o_Error = "no CUDA device of compute capability 7.5 (Turing) or newer. Devices: " + describe(i_Devices);
			}
			return false;
		}
	}

	bool optxContext::Create(bool i_bValidation, std::string& o_Error)
	{
		DBG_ASSERT(!l_bExists, "optxContext::Create called twice");
		o_Error.clear();

		std::vector<DeviceInfo> devices;
		DeviceInfo device;
		if (!list_devices(devices, o_Error) || !choose_device(devices, device, o_Error))
		{
			return false;
		}

		CUcontext cuda = NULL;
		if (CU_CHECK(cu().cuDevicePrimaryCtxRetain(&cuda, device.m_Device)))
		{
			o_Error = "CUDA could not create a context on " + device.m_Name;
			return false;
		}
		CUstream stream = NULL;
		if (CU_CHECK(cu().cuCtxSetCurrent(cuda)) || CU_CHECK(cu().cuStreamCreate(&stream, CU_STREAM_NON_BLOCKING)))
		{
			cu().cuDevicePrimaryCtxRelease(device.m_Device);
			o_Error = "CUDA could not create a stream on " + device.m_Name;
			return false;
		}

		OptixDeviceContextOptions options = {};
		options.logCallbackFunction = &log_callback;
		options.logCallbackLevel = 4;
#ifdef _DEBUG
		i_bValidation = true;
#endif
		options.validationMode = i_bValidation ? OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_ALL : OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_OFF;
		OptixDeviceContext optix = NULL;
		const OptixResult result = optixDeviceContextCreate(cuda, &options, &optix);
		if (result != OPTIX_SUCCESS)
		{
			cu().cuStreamDestroy(stream);
			cu().cuDevicePrimaryCtxRelease(device.m_Device);
			o_Error = std::string("OptiX could not create a device context on ") + device.m_Name + " (" + optxResultName(result) + ")";
			return false;
		}

		l_Device = device.m_Device;
		l_Cuda = cuda;
		l_Stream = stream;
		l_Optix = optix;
		l_DeviceName = device.m_Name;
		l_nErrors = 0;
		l_nWarnings = 0;
		l_bExists = true;

		int driver_version = 0;
		cu().cuDriverGetVersion(&driver_version);
		size_t memory = 0;
		cu().cuDeviceTotalMem(&memory, device.m_Device);
		DBG_LOG("GraphicsOptiX: " << device.m_Name << " (compute " << device.m_ComputeCapability / 10 << "."
				<< device.m_ComputeCapability % 10 << ", " << (memory >> 20) << " MB), driver CUDA "
				<< driver_version / 1000 << "." << driver_version % 1000 / 10 << ", OptiX validation "
				<< (i_bValidation ? "on" : "off"));
		return true;
	}

	void optxContext::Destroy() throw()
	{
		if (!l_bExists)
		{
			return;
		}
		cu().cuCtxSetCurrent(l_Cuda);
		cu().cuStreamSynchronize(l_Stream);
		optixDeviceContextDestroy(l_Optix);
		cu().cuStreamDestroy(l_Stream);
		cu().cuCtxSetCurrent(NULL);
		cu().cuDevicePrimaryCtxRelease(l_Device);
		l_Optix = NULL;
		l_Stream = NULL;
		l_Cuda = NULL;
		l_Device = 0;
		l_DeviceName.clear();
		l_bExists = false;
	}

	bool optxContext::Exists()
	{
		return l_bExists;
	}

	void optxContext::MakeCurrent()
	{
		DBG_ASSERT(l_bExists, "optxContext::MakeCurrent without a context");
		CU_CHECK(cu().cuCtxSetCurrent(l_Cuda));
	}

	bool optxContext::Synchronize()
	{
		return !CU_CHECK(cu().cuStreamSynchronize(l_Stream));
	}

	OptixDeviceContext optxContext::GetOptix()
	{
		return l_Optix;
	}

	CUcontext optxContext::GetCuda()
	{
		return l_Cuda;
	}

	CUstream optxContext::GetStream()
	{
		return l_Stream;
	}

	CUdevice optxContext::GetDevice()
	{
		return l_Device;
	}

	const std::string& optxContext::GetDeviceName()
	{
		return l_DeviceName;
	}

	int optxContext::GetErrorCount()
	{
		return l_nErrors;
	}

	int optxContext::GetWarningCount()
	{
		return l_nWarnings;
	}
}
