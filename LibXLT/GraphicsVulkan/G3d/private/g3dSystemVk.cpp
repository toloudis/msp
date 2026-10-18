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
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3DSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dStateMgr.hpp"
#endif
#ifndef GVK_G2DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g2dStubsVk.hpp"
#endif

namespace gvk
{
	namespace
	{
		g3dBlendStateMgr::BlendState*				st_pInitial = NULL;
		g3dDepthStencilStateMgr::DepthStencilState*	ds_pTest_Write_LessE_NS = NULL;

		//	As g3dSystemDX11's init_d3d_states, without the renderers' own
		//	states (each comes with its renderer's port)
		void init_states()
		{
			//initialize State managers first
			g3dBlendStateMgr::Init();
			g3dDepthStencilStateMgr::Init();
			g3dRasterizerStateMgr::Init();

			g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

			st_pInitial = new g3dBlendStateMgr::BlendState( false, false,
				VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD,
				0xF );
			ds_pTest_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState(
				true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_KEEP, OP_KEEP );

			//init g3d state systems
			g3dStateMgrVk::Init();

			//	we want to set D3D to cull CW faces
			//	(because of our coordinate system change to right-handed), even though
			//	our users think that we are culling CCW faces
			//	(DX11 takes the fill mode from g3dDrawStyleUtilDX11, ported with the renderers)
			g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_FRONT_BIT, VK_POLYGON_MODE_FILL );

			//	Set Z comparison
			g3dDepthStencilStateMgr::SetDepthStencilState( ds_pTest_Write_LessE_NS );

			//	set blend states
			g3dBlendStateMgr::SetBlendState( st_pInitial );
		}

		void cleanup_states()
		{
			g3dStateMgrVk::CleanUp();

			delete ds_pTest_Write_LessE_NS;
			delete st_pInitial;
			ds_pTest_Write_LessE_NS = NULL;
			st_pInitial = NULL;

			//cleaup State Managers last
			g3dRasterizerStateMgr::CleanUp();
			g3dDepthStencilStateMgr::CleanUp();
			g3dBlendStateMgr::CleanUp();
		}
	}

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
		init_states();
		matShaderMgr::Initialize();		// registers the built-in shaders, as g3dSystemDX11 does
		g2dScreenCaptureUtil::SetImplementation(m_pScreenCapture);
	}

	g3dSystemVk::~g3dSystemVk()
	{
		g2dScreenCaptureUtil::SetImplementation(NULL);
		matShaderMgr::DeInitialize();
		cleanup_states();
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
