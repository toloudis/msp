/****************************************************************************\
**  g3dRasterizerStateMgr.cpp
**
**	The g3dRasterizerStateMgr sets changes for RasterizerState
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dRasterizerStateMgr.hpp"

#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "Core/Dbg/dbgMsg.hpp"

#ifdef _DEBUG	//debug only testing flags

//This flag compares D3D internal states with the cached state
//If they are out of sync it indicates a Rasterizer state was called
// outside of the manager.
//Operates on a Set call.
//#define RASTERIZER_CHECK_CACHE

//This flag checks if a set is identical to what is already cached
// asserts if a duplicate state was set.
//#define RASTERIZER_CHECK_REDUNDANT_STATES

//This flag removes redundant set state calls by using the cached state
// Note: Make sure that the cache is valid with the CHECK above
//#define RASTERIZER_REMOVE_REDUNDANT_STATES

#endif//_DEBUG

#define MAX_RASTERIZER_STATES 12

//------------------------------------------------------------------------
//	anonymous namespace for local data/functions
//------------------------------------------------------------------------
namespace
{
	g3dRasterizerStateMgr::RasterizerState* l_pDefaultState = NULL;	//default state at init
	g3dRasterizerStateMgr::RasterizerState* l_States[MAX_RASTERIZER_STATES] = {NULL};

	ID3D11RasterizerState* l_pCurrentState = NULL;	//cached current blend state

	//------------------------------------------------------------------------
	//	GetRasterizerState - Get current Rasterizer state
	//------------------------------------------------------------------------
	void GetRasterizerState( g3dRasterizerStateMgr::RasterizerState& o_State )
	{
		g2dDX11Global::g_pDeviceContext->RSGetState( &o_State.m_pRasterizerState );
	}
}

//------constructor------------
g3dRasterizerStateMgr::RasterizerState::RasterizerState(
				D3D11_FILL_MODE i_FillMode,
				D3D11_CULL_MODE i_CullMode,
				int i_DepthBias,
				float i_SlopeScaledDepthBias,
				BOOL i_ScissorEnable,
				BOOL i_MultisampleEnable,
				DWORD i_MultisampleMask,
				BOOL i_AntialiasedLineEnable,
				BOOL i_FrontCounterClockwise)
{
	D3D11_RASTERIZER_DESC rsDesc;

	rsDesc.FillMode = i_FillMode;
	rsDesc.CullMode = i_CullMode;
	rsDesc.DepthBias = i_DepthBias;
	rsDesc.SlopeScaledDepthBias = i_SlopeScaledDepthBias;
	rsDesc.ScissorEnable = i_ScissorEnable;
	rsDesc.MultisampleEnable = i_MultisampleEnable;
	rsDesc.AntialiasedLineEnable = i_AntialiasedLineEnable;
	/////////
	// ALERT!!!
	// this flag might have to change depending on the fullscreen quad or mesh rendering.
	// to be settled later: sdk samples show TRUE.
	// for studiogpu GraphicsDX11 meshes, it seems we want FALSE.
//	rsDesc.FrontCounterClockwise = TRUE;
	rsDesc.FrontCounterClockwise = i_FrontCounterClockwise;
	/////////
	rsDesc.DepthClipEnable = TRUE;

	DBG_ASSERT( g2dDX11Global::g_pDevice, "Cannot create rasterizer state before D3D is initialized!" );
	g2dDX11Global::g_pDevice->CreateRasterizerState( &rsDesc, &m_pRasterizerState );
};

g3dRasterizerStateMgr::RasterizerState::~RasterizerState()
{
	SAFE_RELEASE( m_pRasterizerState );
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g3dRasterizerStateMgr::Init()
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(0,0,255,255), L"g3dRasterizerStateMgr::Init" );

	l_States[0] = new RasterizerState( D3D11_FILL_SOLID, D3D11_CULL_NONE, 0, 0.0f, FALSE, TRUE, 0xffffffff, FALSE );
	l_States[1] = new RasterizerState( D3D11_FILL_WIREFRAME, D3D11_CULL_NONE, 0, 0.0f, FALSE, TRUE, 0xffffffff, FALSE );
	l_States[2] = new RasterizerState( D3D11_FILL_SOLID, D3D11_CULL_FRONT, 0, 0.0f, FALSE, TRUE, 0xffffffff, FALSE );
	l_States[3] = new RasterizerState( D3D11_FILL_WIREFRAME, D3D11_CULL_FRONT, 0, 0.0f, FALSE, TRUE, 0xffffffff, FALSE );
	l_States[4] = new RasterizerState( D3D11_FILL_SOLID, D3D11_CULL_BACK, 0, 0.0f, FALSE, TRUE, 0xffffffff, FALSE );
	l_States[5] = new RasterizerState( D3D11_FILL_WIREFRAME, D3D11_CULL_BACK, 0, 0.0f, FALSE, TRUE, 0xffffffff, FALSE );
	l_States[6] = new RasterizerState( D3D11_FILL_SOLID, D3D11_CULL_NONE, 0, 0.0f, FALSE, TRUE, 0xffffffff, FALSE, FALSE );
	l_States[7] = new RasterizerState( D3D11_FILL_WIREFRAME, D3D11_CULL_NONE, 0, 0.0f, FALSE, TRUE, 0xffffffff, FALSE, FALSE );
	l_States[8] = new RasterizerState( D3D11_FILL_SOLID, D3D11_CULL_FRONT, 0, 0.0f, FALSE, TRUE, 0xffffffff, FALSE, FALSE );
	l_States[9] = new RasterizerState( D3D11_FILL_WIREFRAME, D3D11_CULL_FRONT, 0, 0.0f, FALSE, TRUE, 0xffffffff, FALSE, FALSE );
	l_States[10] = new RasterizerState( D3D11_FILL_SOLID, D3D11_CULL_BACK, 0, 0.0f, FALSE, TRUE, 0xffffffff, FALSE, FALSE );
	l_States[11] = new RasterizerState( D3D11_FILL_WIREFRAME, D3D11_CULL_BACK, 0, 0.0f, FALSE, TRUE, 0xffffffff, FALSE, FALSE );

	l_pDefaultState = new RasterizerState( D3D11_FILL_SOLID, D3D11_CULL_BACK, 0, 0.0f, FALSE, FALSE, 0xFFFFFFFF, FALSE );

	l_pCurrentState = l_pDefaultState->m_pRasterizerState;
	D3DPERF_EndEvent();

}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g3dRasterizerStateMgr::CleanUp()
{
	g2dDX11Global::g_pDeviceContext->RSSetState( NULL );

	for( int i = 0; i < MAX_RASTERIZER_STATES; i++ )
	{
		SAFE_DELETE( l_States[i] );
	}

	SAFE_DELETE( l_pDefaultState );
}

//------------------------------------------------------------------------
//	SetRasterizerState - Sets the Rasterizer state
//------------------------------------------------------------------------
void g3dRasterizerStateMgr::SetRasterizerState( const RasterizerState* i_pState )
{
	DBG_ASSERT( i_pState, "Invalid Rasterizer state." );
	DBG_ASSERT( l_pCurrentState, "Must call Init() before setting state." );
	DBG_ASSERT( i_pState->m_pRasterizerState, "Rasterizer state hasn't been created yet" );

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(0,0,255,255), L"g3dRasterizerStateMgr::SetRasterizerState" );

#ifdef RASTERIZER_CHECK_CACHE
	RasterizerState CurrentState;
	GetRasterizerState( CurrentState );
	DBG_ASSERT( (l_pCurrentState == CurrentState.m_pRasterizerState), "Rasterizer State Cache out of sync!");
	//	if( l_pCurrentState == CurrentState ) DBG_WARNING( "Rasterizer State Cache out of sync!" );
#endif

#ifdef RASTERIZER_CHECK_REDUNDANT_STATES
	DBG_ASSERT( (l_pCurrentState != i_pState->m_pRasterizerState), "Redundant Rasterizer State found!");
	//	if( l_pCurrentState == i_State ) DBG_WARNING( "Redundant Rasterizer State found!" );
#endif

#ifdef RASTERIZER_REMOVE_REDUNDANT_STATES
	if( l_pCurrentState != i_pState->m_pRasterizerState )
	{
		g2dDX11Global::g_pDeviceContext->RSSetState( i_pState->m_pRasterizerState );
	}
#else
	g2dDX11Global::g_pDeviceContext->RSSetState( i_pState->m_pRasterizerState );
#endif

	//set current state;
	l_pCurrentState = i_pState->m_pRasterizerState;

	D3DPERF_EndEvent();
}

//------------------------------------------------------------------------
//	SetRasterizerState - Override that uses just the Cull and Fill modes
// to select a preset state.  All other states are set to defaults.
//------------------------------------------------------------------------
void g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_MODE i_Cull, D3D11_FILL_MODE i_Fill, BOOL i_FrontCounterClockwise )
{
	int Solid = (D3D11_FILL_SOLID == i_Fill) ? 0 : 1;
//	int Fill = (D3D11_CULL_NONE == i_Cull) ? 0 : ((D3D11_CULL_FRONT == i_Cull) ? 1 : 2);
	int Mode = (((D3D11_CULL_NONE == i_Cull) ? 0 : ((D3D11_CULL_FRONT == i_Cull) ? 1 : 2)) << 1) + Solid;
//	int Mode = Solid + (Fill << 1) + ((int)!i_FrontCounterClockwise * 6);
	SetRasterizerState( l_States[ Mode ] );
}

void g3dRasterizerStateMgr::SetDefaultRasterizerState()
{
	DBG_ASSERT( l_pDefaultState, "Must call Init() before resetting default state" );
	SetRasterizerState( l_pDefaultState );
}

void g3dRasterizerStateMgr::GetCurrentRasterizerState( RasterizerState& o_State )
{
	o_State.m_pRasterizerState = l_pCurrentState;
	o_State.m_pRasterizerState->AddRef();	//Get are ref counted
}