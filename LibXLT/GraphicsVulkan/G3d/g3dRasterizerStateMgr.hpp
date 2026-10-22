/****************************************************************************\
**  g3dRasterizerStateMgr.hpp
**
**	The g3dRasterizerStateMgr sets changes for RasterizerState.
**
**	GraphicsVulkan port of GraphicsDX11's g3dRasterizerStateMgr: the same
**	calls, with Vulkan enums for the D3D11 ones. A RasterizerState is a
**	description (vkdRasterizerState) for vkdContext.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G3DRASTERIZERSTATEMGR_HPP
#error g3dRasterizerStateMgr.hpp multiply included
#endif
#define GVK_G3DRASTERIZERSTATEMGR_HPP

#ifndef GVK_VKDSTATE_HPP
#include "GraphicsVulkan/vkd/vkdState.hpp"
#endif

namespace gvk
{
	namespace g3dRasterizerStateMgr
	{
		class RasterizerState
		{
		public:
			RasterizerState() {}
			//	i_MultisampleMask is ignored, as D3D11's rasterizer state has none
			RasterizerState( VkPolygonMode i_FillMode, VkCullModeFlags i_CullMode, int i_DepthBias, float i_SlopeScaledDepthBias,
							 bool i_ScissorEnable, bool i_MultisampleEnable, uint32_t i_MultisampleMask,
							 bool i_AntialiasedLineEnable, bool i_FrontCounterClockwise = true );

			bool operator == (const RasterizerState& i_Item) const;

			vkdRasterizerState m_Desc;
		};

		void Init();
		void CleanUp();

		void SetDefaultRasterizerState();
		void SetRasterizerState( const RasterizerState* i_pState );
		//	As in DX11, i_FrontCounterClockwise is not used: the states this
		//	picks from all have counter-clockwise front faces
		void SetRasterizerState( VkCullModeFlags i_Cull, VkPolygonMode i_Fill, bool i_FrontCounterClockwise = true );
		void GetCurrentRasterizerState( RasterizerState& o_State );
	}
}
