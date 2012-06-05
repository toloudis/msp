/****************************************************************************\
**  g3dDepthStencilStateMgr.cpp
**
**	The g3dDepthStencilStateMgr sets changes for DepthStencilState
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dDepthStencilStateMgr.hpp"

#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "Core/Dbg/dbgMsg.hpp"

#ifdef _DEBUG	//debug only testing flags

//This flag compares D3D internal states with the cached state
//If they are out of sync it indicates a Depth/Stencil state was called
// outside of the manager.
//Operates on a Set call.
//#define DEPTHSTENCIL_CHECK_CACHE

//This flag checks if a set is identical to what is already cached
// asserts if a duplicate state was set.
//#define DEPTHSTENCIL_CHECK_REDUNDANT_STATES

//This flag removes redundant set state calls by using the cached state
// Note: Make sure that the cache is valid with the CHECK above
//#define DEPTHSTENCIL_REMOVE_REDUNDANT_STATES

#endif//_DEBUG

//------------------------------------------------------------------------
//	anonymous namespace for local data/functions
//------------------------------------------------------------------------
namespace
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	g3dDepthStencilStateMgr::DepthStencilState* l_pDefaultState = NULL;	//default state at init
	g3dDepthStencilStateMgr::DepthStencilState* l_States[6] = {NULL};

	ID3D11DepthStencilState* l_pCurrentState = NULL;	//cached current blend state

	//------------------------------------------------------------------------
	//	GetDepthStencilState - Get current Depth/Stencil state
	//------------------------------------------------------------------------
	void GetDepthStencilState( g3dDepthStencilStateMgr::DepthStencilState& o_State )
	{
		UINT stencilRef;
		g2dDX11Global::g_pDeviceContext->OMGetDepthStencilState( &o_State.m_pDepthStencilState, &stencilRef );
	}
}

//--------Constructor----------------------
g3dDepthStencilStateMgr::DepthStencilState::DepthStencilState(
				  BOOL i_DepthEnable,
				  BOOL i_DepthWriteMask,
				  D3D11_COMPARISON_FUNC i_DepthFunc,
				  BOOL i_StencilEnable,
//				  DWORD i_StencilRef,
				  UINT8 i_StencilMask,
				  UINT8 i_StencilWriteMask,
				  const DepthStencilOp_Desc& i_FrontFace,
				  const DepthStencilOp_Desc& i_BackFace )
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	dsDesc.DepthEnable = i_DepthEnable;
	dsDesc.DepthWriteMask = i_DepthWriteMask ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = i_DepthFunc;
	dsDesc.StencilEnable = i_StencilEnable;
	dsDesc.StencilReadMask = i_StencilMask;
	dsDesc.StencilWriteMask = i_StencilWriteMask;
	dsDesc.FrontFace = i_FrontFace.m_Desc;
	dsDesc.BackFace = i_BackFace.m_Desc;
	DBG_ASSERT( g2dDX11Global::g_pDevice, "Cannot create depth/stencil state before D3D is initialized!" );
	g2dDX11Global::g_pDevice->CreateDepthStencilState( &dsDesc, &m_pDepthStencilState );
};

g3dDepthStencilStateMgr::DepthStencilState::~DepthStencilState()
{
	SAFE_RELEASE( m_pDepthStencilState );
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g3dDepthStencilStateMgr::Init()
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(0,0,255,255), L"g3dDepthStencilStateMgr::Init" );

	l_pDefaultState = new DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS, FALSE,
		D3D11_DEFAULT_STENCIL_READ_MASK, D3D11_DEFAULT_STENCIL_WRITE_MASK, OP_KEEP, OP_KEEP );
	l_pCurrentState = l_pDefaultState->m_pDepthStencilState;

	D3DPERF_EndEvent();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g3dDepthStencilStateMgr::CleanUp()
{
	g2dDX11Global::g_pDeviceContext->OMSetDepthStencilState( NULL, 0 );

	SAFE_DELETE( l_pDefaultState );
}

//------------------------------------------------------------------------
//	SetDepthStencilState - Sets the Depth/Stencil state
//------------------------------------------------------------------------
void g3dDepthStencilStateMgr::SetDepthStencilState( const DepthStencilState* i_pState, UINT i_StencilRef )
{
	DBG_ASSERT( i_pState, "Invalid DepthStencil State" );
	DBG_ASSERT( l_pCurrentState, "Must call Init() before setting state." );
	DBG_ASSERT( i_pState->m_pDepthStencilState, "Depth/Stencil state hasn't been created yet" );

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(0,0,255,255), L"g3dDepthStencilStateMgr::SetDepthStencilState" );

#ifdef DEPTHSTENCIL_CHECK_CACHE
	DepthStencilState CurrentState;
	GetDepthStencilState( CurrentState );
	DBG_ASSERT( (l_pCurrentState == CurrentState.m_pDepthStencilState), "Depth/Stencil State Cache out of sync!");
	//	if( l_CurrentState == CurrentState ) DBG_WARNING( "Depth/Stencil State Cache out of sync!" );
#endif

#ifdef DEPTHSTENCIL_CHECK_REDUNDANT_STATES
	DBG_ASSERT( (l_pCurrentState != i_State->m_pDepthStencilState), "Redundant Depth/Stencil State found!");
	//	if( l_CurrentState == i_State ) DBG_WARNING( "Redundant Depth/Stencil State found!" );
#endif

#ifdef DEPTHSTENCIL_REMOVE_REDUNDANT_STATES
	if( l_pCurrentState != i_pState->m_pDepthStencilState )
	{
		g2dDX11Global::g_pDeviceContext->OMSetDepthStencilState( i_pState->m_pDepthStencilState, i_StencilRef );
	}
#else
	g2dDX11Global::g_pDeviceContext->OMSetDepthStencilState( i_pState->m_pDepthStencilState, i_StencilRef );
#endif

	l_pCurrentState = i_pState->m_pDepthStencilState;

	D3DPERF_EndEvent();
}

//------------------------------------------------------------------------
//	SetDefaultDepthStencilState - Reset the Depth/Stencil state to the default
//------------------------------------------------------------------------
void g3dDepthStencilStateMgr::SetDefaultDepthStencilState()
{
	DBG_ASSERT( l_pDefaultState, "Must call Init() before resetting default state" );
	SetDepthStencilState( l_pDefaultState );
}

//------------------------------------------------------------------------
//	GetCurrentDepthStencilState - Get current Depth/Stencil state
//------------------------------------------------------------------------
void g3dDepthStencilStateMgr::GetCurrentDepthStencilState( g3dDepthStencilStateMgr::DepthStencilState& o_State )
{
	o_State.m_pDepthStencilState = l_pCurrentState;
	o_State.m_pDepthStencilState->AddRef();	//Get are ref counted
}
