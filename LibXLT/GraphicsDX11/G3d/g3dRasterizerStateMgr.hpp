/****************************************************************************\
**  g3dRasterizerStateMgr.hpp
**
**	The g3dRasterizerStateMgr sets changes for RasterizerState
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_RASTERIZERSTATEMGR_HPP
#error g3dRasterizerStateMgr.hpp multiply included
#endif
#define G3D_RASTERIZERSTATEMGR_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

namespace g3dRasterizerStateMgr
{
	class RasterizerState
	{
	public:
		RasterizerState() : m_pRasterizerState(NULL){};
		~RasterizerState();

		RasterizerState(
			D3D11_FILL_MODE i_FillMode,
			D3D11_CULL_MODE i_CullMode,
			int i_DepthBias,
			float i_SlopeScaledDepthBias,
			BOOL i_ScissorEnable,
			BOOL i_MultisampleEnable,
			DWORD i_MultisampleMask,
			BOOL i_AntialiasedLineEnable,
			BOOL i_FrontCounterClockwise = TRUE );

/*		RasterizerState(const RasterizerState& i_Item)
		{
			DBG_ERROR( "Not Implemented" );
		};*/

		bool operator == (const RasterizerState& i_Item) const
		{
			return i_Item.m_pRasterizerState == m_pRasterizerState;
		};

		ID3D11RasterizerState* m_pRasterizerState;
	};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp();

	//------------------------------------------------------------------------
	//	SetDefaultRasterizerState - Reset the rasterizer state to the default
	//------------------------------------------------------------------------
	void SetDefaultRasterizerState();

	//------------------------------------------------------------------------
	//	SetRasterizerState - Sets the Rasterizer state
	//------------------------------------------------------------------------
	void SetRasterizerState( const RasterizerState* i_pState );

	//------------------------------------------------------------------------
	//	SetRasterizerState - Override that uses just the Cull and Fill modes
	// to select a preset state.  All other states are set to defaults.
	//------------------------------------------------------------------------
	void SetRasterizerState( D3D11_CULL_MODE i_Cull, D3D11_FILL_MODE i_Fill, BOOL i_FrontCounterClockwise = TRUE );

	//------------------------------------------------------------------------
	//	GetCurrentRasterizerState - Get current Rasterizer state
	//------------------------------------------------------------------------
	void GetCurrentRasterizerState( RasterizerState& o_State);
}
	


