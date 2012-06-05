/****************************************************************************\
**  g3dDepthStencilStateMgr.hpp
**
**	The g3dDepthStencilStateMgr sets changes for DepthStencilState
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_DEPTHSTENCILSTATEMGR_HPP
#error g3dDepthStencilStateMgr.hpp multiply included
#endif
#define G3D_DEPTHSTENCILSTATEMGR_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

namespace g3dDepthStencilStateMgr
{
	struct DepthStencilOp_Desc
	{
		DepthStencilOp_Desc(){};
		DepthStencilOp_Desc(
			D3D11_STENCIL_OP i_StencilFailOp,
			D3D11_STENCIL_OP i_StencilDepthFailOp,
			D3D11_STENCIL_OP i_StencilPassOp,
			D3D11_COMPARISON_FUNC   i_StencilFunc)
		{
			m_Desc.StencilFailOp = i_StencilFailOp;
			m_Desc.StencilDepthFailOp = i_StencilDepthFailOp;
			m_Desc.StencilPassOp = i_StencilPassOp;
			m_Desc.StencilFunc = i_StencilFunc;
		};

		bool operator == (const DepthStencilOp_Desc& i_Item) const
		{
			return((m_Desc.StencilFailOp == i_Item.m_Desc.StencilFailOp) &&
				(m_Desc.StencilDepthFailOp == i_Item.m_Desc.StencilDepthFailOp) &&
				(m_Desc.StencilPassOp == i_Item.m_Desc.StencilPassOp) &&
				(m_Desc.StencilFunc == i_Item.m_Desc.StencilFunc));
		};

		D3D11_DEPTH_STENCILOP_DESC m_Desc;
	};

	class DepthStencilState
	{
	public:

		DepthStencilState() : m_pDepthStencilState(NULL){};
		~DepthStencilState();

		DepthStencilState(
			BOOL i_DepthEnable,
			BOOL i_DepthWriteMask,
			D3D11_COMPARISON_FUNC i_DepthFunc,
			BOOL i_StencilEnable,
//			DWORD i_StencilRef,		//deprecate so we move it to the set call
			UINT8 i_StencilMask,
			UINT8 i_StencilWriteMask,
			const DepthStencilOp_Desc& i_FrontFace,
			const DepthStencilOp_Desc& i_BackFace );

/*		DepthStencilState(const DepthStencilState& i_Item)
		{
			DBG_ERROR( "Not Implemented" );
		};*/

		bool operator == (const DepthStencilState& i_Item) const
		{
			return i_Item.m_pDepthStencilState == m_pDepthStencilState;
		};

		ID3D11DepthStencilState* m_pDepthStencilState;
	};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp();

	//------------------------------------------------------------------------
	//	SetDefaultDepthStencilState - Reset the Depth/Stencil state to the default
	//------------------------------------------------------------------------
	void SetDefaultDepthStencilState();

	//------------------------------------------------------------------------
	//	SetDepthStencilState - Sets the Depth/Stencil state
	//------------------------------------------------------------------------
	void SetDepthStencilState( const DepthStencilState* i_pState, UINT i_StencilRef = 0 );

	//------------------------------------------------------------------------
	//	GetCurrentDepthStencilState - Get current Depth/Stencil state
	//------------------------------------------------------------------------
	void GetCurrentDepthStencilState( g3dDepthStencilStateMgr::DepthStencilState& o_State );
}
	


