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
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef GVK_MATSTUBSVK_HPP
#include "GraphicsVulkan/stub/matStubsVk.hpp"
#endif
#ifndef GVK_G2DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g2dStubsVk.hpp"
#endif

namespace gvk
{
	g3dSystemVk::g3dSystemVk(vkdDevice& i_Device)
		: m_Device(i_Device)
		, m_Name(i_Device.Adapter().m_Name)
		, m_pLightMgr(new g3dLightMgrVk)
		, m_pTextureMgr(new matTextureMgrVk)
		, m_pShaderMgr(new matShaderMgrVk)
		, m_pScreenCapture(new g2dScreenCaptureUtilVk)
	{
		g3dLightMgr::SetImplementation(m_pLightMgr);
		matTextureMgr::SetImplementation(m_pTextureMgr);
		matShaderMgr::SetImplementation(m_pShaderMgr);
		matShaderMgr::Initialize();		// registers the built-in shaders, as g3dSystemDX11 does
		g2dScreenCaptureUtil::SetImplementation(m_pScreenCapture);
	}

	g3dSystemVk::~g3dSystemVk()
	{
		g2dScreenCaptureUtil::SetImplementation(NULL);
		matShaderMgr::DeInitialize();
		matShaderMgr::SetImplementation(NULL);
		matTextureMgr::SetImplementation(NULL);
		g3dLightMgr::SetImplementation(NULL);
		delete m_pScreenCapture;
		delete m_pShaderMgr;
		delete m_pTextureMgr;
		delete m_pLightMgr;
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
