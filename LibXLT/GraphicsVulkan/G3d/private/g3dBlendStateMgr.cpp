/****************************************************************************\
**  g3dBlendStateMgr.cpp
**
**	The g3dBlendStateMgr sets changes for blendstate (GraphicsVulkan port).
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif

namespace gvk
{
	namespace
	{
		g3dBlendStateMgr::BlendState* l_pDefaultState = NULL;		//default state at init
	}

	g3dBlendStateMgr::BlendState::BlendState( bool i_AlphaToCoverage, bool i_BlendEnabled, VkBlendFactor i_SrcBlend, VkBlendFactor i_DestBlend,
		VkBlendOp i_BlendOp, VkBlendFactor i_SrcBlendAlpha, VkBlendFactor i_DestBlendAlpha, VkBlendOp i_BlendOpAlpha,
		VkColorComponentFlags i_ColorWrittenEnabled )
	{
		m_Desc.m_bAlphaToCoverage = i_AlphaToCoverage;
		m_Desc.m_bIndependent = false;			//We don't use other targets
		vkdBlendTarget& target = m_Desc.m_Targets[0];
		target.m_bEnable = i_BlendEnabled;
		target.m_Op = i_BlendOp;
		target.m_Src = i_SrcBlend;
		target.m_Dst = i_DestBlend;
		target.m_OpAlpha = i_BlendOpAlpha;
		target.m_SrcAlpha = i_SrcBlendAlpha;
		target.m_DstAlpha = i_DestBlendAlpha;
		target.m_WriteMask = i_ColorWrittenEnabled;
	}

	bool g3dBlendStateMgr::BlendState::operator == (const BlendState& i_Item) const
	{
		const vkdBlendTarget& a = m_Desc.m_Targets[0];
		const vkdBlendTarget& b = i_Item.m_Desc.m_Targets[0];
		return m_Desc.m_bAlphaToCoverage == i_Item.m_Desc.m_bAlphaToCoverage && a.m_bEnable == b.m_bEnable &&
			   a.m_Src == b.m_Src && a.m_Dst == b.m_Dst && a.m_Op == b.m_Op && a.m_SrcAlpha == b.m_SrcAlpha &&
			   a.m_DstAlpha == b.m_DstAlpha && a.m_OpAlpha == b.m_OpAlpha && a.m_WriteMask == b.m_WriteMask;
	}

	void g3dBlendStateMgr::Init()
	{
		l_pDefaultState = new BlendState( false, false,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
			0xF );
	}

	void g3dBlendStateMgr::CleanUp()
	{
		const float blendFactors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		vkdGlobal::Context().SetBlendState( vkdBlendState(), blendFactors );
		delete l_pDefaultState;
		l_pDefaultState = NULL;
	}

	void g3dBlendStateMgr::SetBlendState( const BlendState* i_pBlendState, const maFloatRGBA& i_BlendFactors )
	{
		DBG_ASSERT( i_pBlendState, "Invalid blend state." );
		DBG_ASSERT( l_pDefaultState, "Must call Init() before setting state." );
		vkdGlobal::Context().SetBlendState( i_pBlendState->m_Desc, i_BlendFactors.GetPtr() );
	}

	void g3dBlendStateMgr::SetDefaultBlendState()
	{
		DBG_ASSERT( l_pDefaultState, "Must call Init() before resetting default state" );
		SetBlendState( l_pDefaultState, maFloatRGBA() );
	}

	void g3dBlendStateMgr::GetCurrentBlendState( BlendState& o_blendState )
	{
		o_blendState.m_Desc = vkdGlobal::Context().GetBlendState();
	}
}
