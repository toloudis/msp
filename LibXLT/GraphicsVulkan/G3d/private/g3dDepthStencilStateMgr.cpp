/****************************************************************************\
**  g3dDepthStencilStateMgr.cpp
**
**	The g3dDepthStencilStateMgr sets changes for DepthStencilState
**	(GraphicsVulkan port).
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
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
		const g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );
		g3dDepthStencilStateMgr::DepthStencilState* l_pDefaultState = NULL;	//default state at init
	}

	g3dDepthStencilStateMgr::DepthStencilState::DepthStencilState( bool i_DepthEnable, bool i_DepthWriteMask, VkCompareOp i_DepthFunc,
		bool i_StencilEnable, uint8_t i_StencilMask, uint8_t i_StencilWriteMask,
		const DepthStencilOp_Desc& i_FrontFace, const DepthStencilOp_Desc& i_BackFace )
	{
		m_Desc.m_bDepth = i_DepthEnable;
		m_Desc.m_bDepthWrite = i_DepthWriteMask;
		m_Desc.m_DepthFunc = i_DepthFunc;
		m_Desc.m_bStencil = i_StencilEnable;
		m_Desc.m_ReadMask = i_StencilMask;
		m_Desc.m_WriteMask = i_StencilWriteMask;
		m_Desc.m_Front = i_FrontFace.m_Desc;
		m_Desc.m_Back = i_BackFace.m_Desc;
	}

	bool g3dDepthStencilStateMgr::DepthStencilState::operator == (const DepthStencilState& i_Item) const
	{
		const vkdDepthStencilState& a = m_Desc;
		const vkdDepthStencilState& b = i_Item.m_Desc;
		DepthStencilOp_Desc af, ab, bf, bb;
		af.m_Desc = a.m_Front;
		ab.m_Desc = a.m_Back;
		bf.m_Desc = b.m_Front;
		bb.m_Desc = b.m_Back;
		return a.m_bDepth == b.m_bDepth && a.m_bDepthWrite == b.m_bDepthWrite && a.m_DepthFunc == b.m_DepthFunc &&
			   a.m_bStencil == b.m_bStencil && a.m_ReadMask == b.m_ReadMask && a.m_WriteMask == b.m_WriteMask &&
			   af == bf && ab == bb;
	}

	void g3dDepthStencilStateMgr::Init()
	{
		l_pDefaultState = new DepthStencilState( true, true, VK_COMPARE_OP_LESS, false, 0xFF, 0xFF, OP_KEEP, OP_KEEP );
	}

	void g3dDepthStencilStateMgr::CleanUp()
	{
		vkdGlobal::Context().SetDepthStencilState( vkdDepthStencilState(), 0 );
		delete l_pDefaultState;
		l_pDefaultState = NULL;
	}

	void g3dDepthStencilStateMgr::SetDepthStencilState( const DepthStencilState* i_pState, uint32_t i_StencilRef )
	{
		DBG_ASSERT( i_pState, "Invalid DepthStencil State" );
		DBG_ASSERT( l_pDefaultState, "Must call Init() before setting state." );
		vkdGlobal::Context().SetDepthStencilState( i_pState->m_Desc, i_StencilRef );
	}

	void g3dDepthStencilStateMgr::SetDefaultDepthStencilState()
	{
		DBG_ASSERT( l_pDefaultState, "Must call Init() before resetting default state" );
		SetDepthStencilState( l_pDefaultState );
	}

	void g3dDepthStencilStateMgr::GetCurrentDepthStencilState( DepthStencilState& o_State )
	{
		o_State.m_Desc = vkdGlobal::Context().GetDepthStencilState();
	}
}
