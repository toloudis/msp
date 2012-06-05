/****************************************************************************\
**  g3dBlendStateMgr.cpp
**
**	The g3dBlendStateMgr sets changes for blendstate
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"

#include "GraphicsDX11/G3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "Core/Dbg/dbgMsg.hpp"

#ifdef _DEBUG	//debug only testing flags

//This flag compares D3D internal states with the cached state
//If they are out of sync it indicates a blend state was called
// outside of the manager.
//Operates on a Set call.
//#define BLEND_CHECK_CACHE

//This flag checks if a set is identical to what is already cached
// asserts if a duplicate state was set.
//#define BLEND_CHECK_REDUNDANT_STATES

//This flag removes redundant set state calls by using the cached state
// Note: Make sure that the cache is valid with the CHECK above
//#define BLEND_REMOVE_REDUNDANT_STATES

#endif//_DEBUG

//------------------------------------------------------------------------
//	anonymous namespace for local data/functions
//------------------------------------------------------------------------
namespace
{
	g3dBlendStateMgr::BlendState* l_pDefaultState = NULL;		//default state at init

	ID3D11BlendState* l_pCurrentState = NULL;	//cached current blend state
	maFloatRGBA l_BlendFactors;

	//------------------------------------------------------------------------
	//	GetBlendState - Get current blend state
	//------------------------------------------------------------------------
	void GetBlendState( g3dBlendStateMgr::BlendState& o_blendState )
	{
		FLOAT blendFactor[4];
		UINT sampleMask;
		g2dDX11Global::g_pDeviceContext->OMGetBlendState( &o_blendState.m_pBlendState, blendFactor, &sampleMask );
	}
}

g3dBlendStateMgr::BlendState::BlendState( BOOL i_AlphaToCoverage, BOOL i_BlendEnabled, D3D11_BLEND i_SrcBlend, D3D11_BLEND i_DestBlend, D3D11_BLEND_OP i_BlendOp,
		   D3D11_BLEND i_SrcBlendAlpha, D3D11_BLEND i_DestBlendAlpha, D3D11_BLEND_OP i_BlendOpAlpha, UINT8 i_ColorWrittenEnabled )
{
	D3D11_BLEND_DESC omDesc;

	// additive blend on rendertarget 0
	ZeroMemory( &omDesc, sizeof( D3D11_BLEND_DESC ) );
	omDesc.AlphaToCoverageEnable = i_AlphaToCoverage;
	omDesc.IndependentBlendEnable = FALSE;			//We don't use other targets
	omDesc.RenderTarget[0].BlendEnable		= i_BlendEnabled;
	omDesc.RenderTarget[0].BlendOp			= i_BlendOp;
	omDesc.RenderTarget[0].SrcBlend			= i_SrcBlend;
	omDesc.RenderTarget[0].DestBlend		= i_DestBlend;
	omDesc.RenderTarget[0].BlendOpAlpha		= i_BlendOpAlpha;
	omDesc.RenderTarget[0].SrcBlendAlpha	= i_SrcBlendAlpha;
	omDesc.RenderTarget[0].DestBlendAlpha	= i_DestBlendAlpha;
	omDesc.RenderTarget[0].RenderTargetWriteMask = i_ColorWrittenEnabled;

	DBG_ASSERT( g2dDX11Global::g_pDevice, "Cannot create blend state before D3D is initialized!" );
	g2dDX11Global::g_pDevice->CreateBlendState( &omDesc, &m_pBlendState );
};

g3dBlendStateMgr::BlendState::~BlendState()
{
	SAFE_RELEASE( m_pBlendState );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g3dBlendStateMgr::Init()
{
	// open the target file and clean it up
	/*FILE* fp = fopen(l_filename, "w");
	fclose(fp);*/

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(0,0,255,255), L"g3dBlendStateMgr::Init" );
	l_pDefaultState = new BlendState( FALSE, FALSE,
		D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL );
	l_pCurrentState = l_pDefaultState->m_pBlendState;
	D3DPERF_EndEvent();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g3dBlendStateMgr::CleanUp()
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(0,0,255,255), L"g3dBlendStateMgr::CleanUp" );
	float blendFactors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	g2dDX11Global::g_pDeviceContext->OMSetBlendState( NULL, blendFactors, 0xFFFFFFFF );
	SAFE_DELETE( l_pDefaultState );
	D3DPERF_EndEvent();
}

//------------------------------------------------------------------------
//	SetBlendState - Sets the blend state
//------------------------------------------------------------------------
void g3dBlendStateMgr::SetBlendState( const BlendState* i_pBlendState, const maFloatRGBA& i_BlendFactors )
{
	DBG_ASSERT( i_pBlendState, "Invalid blend state." );
	DBG_ASSERT( l_pCurrentState, "Must call Init() before setting state." );
	DBG_ASSERT( i_pBlendState->m_pBlendState, "Blend state hasn't been created yet" );

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(0,0,255,255), L"g3dBlendStateMgr::SetBlendState" );

#ifdef BLEND_CHECK_CACHE
	BlendState CurrentState;
	GetBlendState( CurrentState );
	DBG_ASSERT( (l_pCurrentState == CurrentState.m_pBlendState), "Blend State Cache out of sync!");
//	if( l_CurrentState == CurrentState ) DBG_WARNING( "Blend State Cache out of sync!" );
#endif

#ifdef BLEND_CHECK_REDUNDANT_STATES
	DBG_ASSERT( (l_pCurrentState != i_blendState->m_pBlendState), "Redundant Blend State found!");
	//	if( l_CurrentState == i_blendState ) DBG_WARNING( "Redundant Blend State found!" );
#endif

#ifdef BLEND_REMOVE_REDUNDANT_STATES
	if( l_pCurrentState != i_pBlendState->m_pBlendState &&
		!(l_BlendFactors == i_BlendFactors) )
	{
		g2dDX11Global::g_pDeviceContext->OMSetBlendState( i_pBlendState->m_pBlendState, i_BlendFactors.GetPtr(), 0xFFFFFFFF );
	}
#else
	g2dDX11Global::g_pDeviceContext->OMSetBlendState( i_pBlendState->m_pBlendState, i_BlendFactors.GetPtr(), 0xFFFFFFFF );
#endif

	//set current state;
	l_pCurrentState = i_pBlendState->m_pBlendState;
	l_BlendFactors = i_BlendFactors;

	D3DPERF_EndEvent();
}

//------------------------------------------------------------------------
//	SetDefaultBlendState - Reset the blend state to the default
//------------------------------------------------------------------------
void g3dBlendStateMgr::SetDefaultBlendState()
{
	DBG_ASSERT( l_pDefaultState, "Must call Init() before resetting default state" );
	SetBlendState( l_pDefaultState, maFloatRGBA() );
}

//------------------------------------------------------------------------
//	GetCurrentBlendState - Get current blend state
//------------------------------------------------------------------------
void g3dBlendStateMgr::GetCurrentBlendState( BlendState& o_blendState )
{
	o_blendState.m_pBlendState = l_pCurrentState;
	o_blendState.m_pBlendState->AddRef();	//Get are ref counted
}
