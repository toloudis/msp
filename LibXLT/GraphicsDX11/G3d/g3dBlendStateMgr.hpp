/****************************************************************************\
**  g3dBlendStateMgr.hpp
**
**	The g3dBlendStateMgr sets changes for blendstate
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_BLENDSTATEMGR_HPP
#error g3dBlendStateMgr.hpp multiply included
#endif
#define G3D_BLENDSTATEMGR_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif

namespace g3dBlendStateMgr
{
	class BlendState
	{
	public:
		BlendState() : m_pBlendState(NULL){};
		~BlendState();

		BlendState( BOOL i_AlphaToCoverage, BOOL i_BlendEnabled, D3D11_BLEND i_SrcBlend, D3D11_BLEND i_DestBlend, D3D11_BLEND_OP i_BlendOp,
		            D3D11_BLEND i_SrcBlendAlpha, D3D11_BLEND i_DestBlendAlpha, D3D11_BLEND_OP i_BlendOpAlpha, UINT8 i_ColorWrittenEnabled );

		bool operator == (const BlendState& i_Item) const
		{
			return i_Item.m_pBlendState == m_pBlendState;
		};

		ID3D11BlendState* m_pBlendState;
	};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanUp();

	//------------------------------------------------------------------------
	//	SetDefaultBlendState - Reset the blend state to the default
	//------------------------------------------------------------------------
	void SetDefaultBlendState();

	//------------------------------------------------------------------------
	//	SetBlendState - Sets the blend state
	//------------------------------------------------------------------------
	void SetBlendState( const BlendState* i_pBlendState, const maFloatRGBA& i_BlendFactors = maFloatRGBA() );

	//------------------------------------------------------------------------
	//	GetCurrentBlendState - Get current blend state
	//------------------------------------------------------------------------
	void GetCurrentBlendState( BlendState& o_blendState );
}


	


