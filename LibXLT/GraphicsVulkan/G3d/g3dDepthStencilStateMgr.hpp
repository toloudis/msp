/****************************************************************************\
**  g3dDepthStencilStateMgr.hpp
**
**	The g3dDepthStencilStateMgr sets changes for DepthStencilState.
**
**	GraphicsVulkan port of GraphicsDX11's g3dDepthStencilStateMgr: the same
**	calls, with Vulkan enums for the D3D11 ones. A DepthStencilState is a
**	description (vkdDepthStencilState) that vkdContext sets as dynamic state.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#error g3dDepthStencilStateMgr.hpp multiply included
#endif
#define GVK_G3DDEPTHSTENCILSTATEMGR_HPP

#ifndef GVK_VKDSTATE_HPP
#include "GraphicsVulkan/vkd/vkdState.hpp"
#endif

namespace gvk
{
	namespace g3dDepthStencilStateMgr
	{
		struct DepthStencilOp_Desc
		{
			DepthStencilOp_Desc() {}
			DepthStencilOp_Desc( VkStencilOp i_StencilFailOp, VkStencilOp i_StencilDepthFailOp, VkStencilOp i_StencilPassOp,
								 VkCompareOp i_StencilFunc )
			{
				m_Desc.m_Fail = i_StencilFailOp;
				m_Desc.m_DepthFail = i_StencilDepthFailOp;
				m_Desc.m_Pass = i_StencilPassOp;
				m_Desc.m_Func = i_StencilFunc;
			}

			bool operator == (const DepthStencilOp_Desc& i_Item) const
			{
				return m_Desc.m_Fail == i_Item.m_Desc.m_Fail && m_Desc.m_DepthFail == i_Item.m_Desc.m_DepthFail &&
					   m_Desc.m_Pass == i_Item.m_Desc.m_Pass && m_Desc.m_Func == i_Item.m_Desc.m_Func;
			}

			vkdStencilFace m_Desc;
		};

		class DepthStencilState
		{
		public:
			DepthStencilState() {}
			DepthStencilState( bool i_DepthEnable, bool i_DepthWriteMask, VkCompareOp i_DepthFunc, bool i_StencilEnable,
							   uint8_t i_StencilMask, uint8_t i_StencilWriteMask,
							   const DepthStencilOp_Desc& i_FrontFace, const DepthStencilOp_Desc& i_BackFace );

			bool operator == (const DepthStencilState& i_Item) const;

			vkdDepthStencilState m_Desc;
		};

		void Init();
		void CleanUp();

		void SetDefaultDepthStencilState();
		void SetDepthStencilState( const DepthStencilState* i_pState, uint32_t i_StencilRef = 0 );
		void GetCurrentDepthStencilState( DepthStencilState& o_State );
	}
}
