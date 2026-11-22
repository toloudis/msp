/****************************************************************************\
**	g3dStateMgr.cpp
**
**	GraphicsVulkan port of GraphicsDX11's g3dStateMgr.cpp: the same default
**	states (DX11 creates state objects; here they are descriptions set on
**	vkdContext).
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsVulkan/G3d/g3dStateMgr.hpp"
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif

namespace gvk
{
	namespace
	{
		vkdRasterizerState		l_DefaultRasterizerState;
		vkdRasterizerState		l_DefaultRSWire;
		vkdDepthStencilState	l_DefaultDepthStencilState;
		vkdBlendState			l_DefaultBlendState;

		g3dStateMgrVk::eFill l_CurrentFill = g3dStateMgrVk::FILL_SOLID;
		g3dStateMgrVk::eCull l_CurrentCull = g3dStateMgrVk::CULL_NONE;
	}

	namespace g3dStateMgrVk
	{
		void Init()
		{
			l_DefaultRasterizerState = vkdRasterizerState();
			l_DefaultRasterizerState.m_Cull = VK_CULL_MODE_BACK_BIT;
			l_DefaultRasterizerState.m_Fill = VK_POLYGON_MODE_FILL;
			l_DefaultRasterizerState.m_bFrontCCW = true;
			l_DefaultRasterizerState.m_bDepthClip = true;
			l_DefaultRSWire = l_DefaultRasterizerState;
			l_DefaultRSWire.m_Fill = VK_POLYGON_MODE_LINE;

			l_DefaultDepthStencilState = vkdDepthStencilState();
			l_DefaultDepthStencilState.m_bDepth = true;
			l_DefaultDepthStencilState.m_bDepthWrite = true;
			l_DefaultDepthStencilState.m_DepthFunc = VK_COMPARE_OP_LESS;
			l_DefaultDepthStencilState.m_bStencil = false;
			//	DX11 zeroes the descriptor: stencil masks 0 and stencil ops 0
			//	(not valid D3D values, unused with the stencil test off)
			l_DefaultDepthStencilState.m_ReadMask = 0;
			l_DefaultDepthStencilState.m_WriteMask = 0;

			l_DefaultBlendState = vkdBlendState();
			vkdBlendTarget& target = l_DefaultBlendState.m_Targets[0];
			target.m_bEnable = true;
			target.m_Op = VK_BLEND_OP_ADD;
			target.m_Src = VK_BLEND_FACTOR_ONE;
			target.m_Dst = VK_BLEND_FACTOR_ZERO;
			target.m_OpAlpha = VK_BLEND_OP_ADD;
			target.m_SrcAlpha = VK_BLEND_FACTOR_ONE;
			target.m_DstAlpha = VK_BLEND_FACTOR_ZERO;
			target.m_WriteMask = 0xF;

			SetDefault();
		}

		void CleanUp()
		{
			vkdContext& context = vkdGlobal::Context();
			context.SetRasterizerState( vkdRasterizerState() );
			context.SetDepthStencilState( vkdDepthStencilState(), 0 );
			const float blendFactors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			context.SetBlendState( vkdBlendState(), blendFactors );
		}

		void SetCull(eCull i_Cull)
		{
			l_CurrentCull = i_Cull;
		}

		void SetFill(eFill i_Fill)
		{
			l_CurrentFill = i_Fill;
		}

		void SetDefault()
		{
			RSSetDefault();
			BSSetDefault();
			DSSetDefault();
		}

		void RSSetDefault()
		{
			vkdGlobal::Context().SetRasterizerState( l_DefaultRasterizerState );
		}

		void RSSetWire()
		{
			vkdGlobal::Context().SetRasterizerState( l_DefaultRSWire );
		}

		void BSSetDefault()
		{
			const float blendFactors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			vkdGlobal::Context().SetBlendState( l_DefaultBlendState, blendFactors );
		}

		void DSSetDefault()
		{
			vkdGlobal::Context().SetDepthStencilState( l_DefaultDepthStencilState, 0 );
		}

		void SetDeviceStates()
		{
		}
	}
}
