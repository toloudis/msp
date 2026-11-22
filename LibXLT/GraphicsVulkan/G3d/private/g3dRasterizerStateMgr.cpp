/****************************************************************************\
**  g3dRasterizerStateMgr.cpp
**
**	The g3dRasterizerStateMgr sets changes for RasterizerState
**	(GraphicsVulkan port).
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif

#define MAX_RASTERIZER_STATES 12

namespace gvk
{
	namespace
	{
		g3dRasterizerStateMgr::RasterizerState* l_pDefaultState = NULL;	//default state at init
		g3dRasterizerStateMgr::RasterizerState* l_States[MAX_RASTERIZER_STATES] = {NULL};
	}

	g3dRasterizerStateMgr::RasterizerState::RasterizerState( VkPolygonMode i_FillMode, VkCullModeFlags i_CullMode, int i_DepthBias,
		float i_SlopeScaledDepthBias, bool i_ScissorEnable, bool i_MultisampleEnable, uint32_t /*i_MultisampleMask*/,
		bool i_AntialiasedLineEnable, bool i_FrontCounterClockwise )
	{
		m_Desc.m_Fill = i_FillMode;
		m_Desc.m_Cull = i_CullMode;
		m_Desc.m_DepthBias = i_DepthBias;
		m_Desc.m_SlopeScaledDepthBias = i_SlopeScaledDepthBias;
		m_Desc.m_bScissor = i_ScissorEnable;
		m_Desc.m_bMultisample = i_MultisampleEnable;
		m_Desc.m_bAntialiasedLine = i_AntialiasedLineEnable;
		m_Desc.m_bFrontCCW = i_FrontCounterClockwise;
		m_Desc.m_bDepthClip = true;
	}

	bool g3dRasterizerStateMgr::RasterizerState::operator == (const RasterizerState& i_Item) const
	{
		const vkdRasterizerState& a = m_Desc;
		const vkdRasterizerState& b = i_Item.m_Desc;
		return a.m_Fill == b.m_Fill && a.m_Cull == b.m_Cull && a.m_bFrontCCW == b.m_bFrontCCW && a.m_DepthBias == b.m_DepthBias &&
			   a.m_DepthBiasClamp == b.m_DepthBiasClamp && a.m_SlopeScaledDepthBias == b.m_SlopeScaledDepthBias &&
			   a.m_bDepthClip == b.m_bDepthClip && a.m_bScissor == b.m_bScissor && a.m_bMultisample == b.m_bMultisample &&
			   a.m_bAntialiasedLine == b.m_bAntialiasedLine;
	}

	void g3dRasterizerStateMgr::Init()
	{
		l_States[0] = new RasterizerState( VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, 0, 0.0f, false, true, 0xffffffff, false );
		l_States[1] = new RasterizerState( VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, 0, 0.0f, false, true, 0xffffffff, false );
		l_States[2] = new RasterizerState( VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT, 0, 0.0f, false, true, 0xffffffff, false );
		l_States[3] = new RasterizerState( VK_POLYGON_MODE_LINE, VK_CULL_MODE_FRONT_BIT, 0, 0.0f, false, true, 0xffffffff, false );
		l_States[4] = new RasterizerState( VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, 0, 0.0f, false, true, 0xffffffff, false );
		l_States[5] = new RasterizerState( VK_POLYGON_MODE_LINE, VK_CULL_MODE_BACK_BIT, 0, 0.0f, false, true, 0xffffffff, false );
		l_States[6] = new RasterizerState( VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, 0, 0.0f, false, true, 0xffffffff, false, false );
		l_States[7] = new RasterizerState( VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, 0, 0.0f, false, true, 0xffffffff, false, false );
		l_States[8] = new RasterizerState( VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT, 0, 0.0f, false, true, 0xffffffff, false, false );
		l_States[9] = new RasterizerState( VK_POLYGON_MODE_LINE, VK_CULL_MODE_FRONT_BIT, 0, 0.0f, false, true, 0xffffffff, false, false );
		l_States[10] = new RasterizerState( VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, 0, 0.0f, false, true, 0xffffffff, false, false );
		l_States[11] = new RasterizerState( VK_POLYGON_MODE_LINE, VK_CULL_MODE_BACK_BIT, 0, 0.0f, false, true, 0xffffffff, false, false );
		l_pDefaultState = new RasterizerState( VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, 0, 0.0f, false, false, 0xFFFFFFFF, false );
	}

	void g3dRasterizerStateMgr::CleanUp()
	{
		vkdGlobal::Context().SetRasterizerState( vkdRasterizerState() );
		for( int i = 0; i < MAX_RASTERIZER_STATES; i++ )
		{
			delete l_States[i];
			l_States[i] = NULL;
		}
		delete l_pDefaultState;
		l_pDefaultState = NULL;
	}

	void g3dRasterizerStateMgr::SetRasterizerState( const RasterizerState* i_pState )
	{
		DBG_ASSERT( i_pState, "Invalid Rasterizer state." );
		DBG_ASSERT( l_pDefaultState, "Must call Init() before setting state." );
		vkdGlobal::Context().SetRasterizerState( i_pState->m_Desc );
	}

	void g3dRasterizerStateMgr::SetRasterizerState( VkCullModeFlags i_Cull, VkPolygonMode i_Fill, bool /*i_FrontCounterClockwise*/ )
	{
		int Solid = (VK_POLYGON_MODE_FILL == i_Fill) ? 0 : 1;
		int Mode = (((VK_CULL_MODE_NONE == i_Cull) ? 0 : ((VK_CULL_MODE_FRONT_BIT == i_Cull) ? 1 : 2)) << 1) + Solid;
		SetRasterizerState( l_States[ Mode ] );
	}

	void g3dRasterizerStateMgr::SetDefaultRasterizerState()
	{
		DBG_ASSERT( l_pDefaultState, "Must call Init() before resetting default state" );
		SetRasterizerState( l_pDefaultState );
	}

	void g3dRasterizerStateMgr::GetCurrentRasterizerState( RasterizerState& o_State )
	{
		o_State.m_Desc = vkdGlobal::Context().GetRasterizerState();
	}
}
