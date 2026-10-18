/*****************************************************************************
**  g3dSystemVk.cpp
**
**      See g3dSystemVk.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_G3DSYSTEMVK_HPP
#include "GraphicsVulkan/G3d/g3dSystemVk.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif

namespace gvk
{
	g3dSystemVk::g3dSystemVk(vkdDevice& i_Device)
		: m_Device(i_Device)
		, m_Name(i_Device.Adapter().m_Name)
	{
	}

	g3dSystemVk::~g3dSystemVk()
	{
	}

	const char* g3dSystemVk::GetVideoAdapterName()
	{
		return m_Name.c_str();
	}

	//	As DX11 (DXGI_ADAPTER_DESC::DedicatedVideoMemory): bytes.
	float g3dSystemVk::GetVideoMemory()
	{
		return static_cast<float>(m_Device.Adapter().m_DeviceLocalBytes);
	}
}
