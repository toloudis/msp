#include "GraphicsDX11/g3d/g3dStateMgr.hpp"

#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"

namespace
{
	ID3D11RasterizerState* l_DefaultRasterizerState = NULL;
	ID3D11RasterizerState* l_DefaultRSWire = NULL;

	ID3D11DepthStencilState*	l_DefaultDepthStencilState = NULL;
	
	ID3D11BlendState*			l_DefaultBlendState = NULL;

	g3dStateMgrDX11::eFill l_CurrentFill = g3dStateMgrDX11::FILL_SOLID;
	g3dStateMgrDX11::eCull l_CurrentCull = g3dStateMgrDX11::CULL_NONE;

	struct rsMode
	{
		D3D11_FILL_MODE fillMode;
		D3D11_CULL_MODE cullMode;
		// bool multiSample;
	};

	// index is l_CurrentFill*NUM_CULL_MODES + l_CurrentCull
	rsMode l_RSModes[] = {
		{D3D11_FILL_SOLID, D3D11_CULL_NONE},
		{D3D11_FILL_SOLID, D3D11_CULL_FRONT},
		{D3D11_FILL_SOLID, D3D11_CULL_BACK},
		{D3D11_FILL_WIREFRAME, D3D11_CULL_NONE},
		{D3D11_FILL_WIREFRAME, D3D11_CULL_FRONT},
		{D3D11_FILL_WIREFRAME, D3D11_CULL_BACK},
	};

	// index is l_Multisample*(NUM_FILL_MODES*NUM_CULL_MODES) + l_CurrentFill*NUM_CULL_MODES + l_CurrentCull
	std::vector<ID3D11RasterizerState*> l_RasterizerStates;


	g3dStateMgrDX11::eDepthEnable l_CurrentDepthEnable = g3dStateMgrDX11::DEPTH_ENABLE;
	g3dStateMgrDX11::eDepthWrite l_CurrentDepthWrite = g3dStateMgrDX11::DEPTH_WRITE_ON;

	struct dsMode
	{
		BOOL depthEnable;
		D3D11_DEPTH_WRITE_MASK depthWriteEnable;
	};

	// index is l_CurrentDepthEnable*NUM_DEPTHWRITE_MODES + l_CurrentDepthWrite
	dsMode l_DSModes[] = {
		{FALSE, D3D11_DEPTH_WRITE_MASK_ZERO},
		{FALSE, D3D11_DEPTH_WRITE_MASK_ALL},
		{TRUE, D3D11_DEPTH_WRITE_MASK_ZERO},
		{TRUE, D3D11_DEPTH_WRITE_MASK_ALL},
	};

	// index is l_CurrentDepthEnable*NUM_DEPTHWRITE_MODES + l_CurrentDepthWrite
	std::vector<ID3D11DepthStencilState*> l_DepthStencilStates;

//DEPTHENABLE:	DEPTHWRITEENABLE:	DEPTHFUNC:	STENCILENABLE:	STENCILREAD:	STENCILWRITE:	STENCILFAIL:	STENCILZFAIL:	STENCILPASS:	STENCILFUNC:	STENCILREF:
//0	0	4	0	4294967295	4294967295	1	1	1	8	0
//0	1	4	0	4294967295	4294967295	1	1	1	8	0
//1	0	4	0	4294967295	4294967295	1	1	1	8	0
//1	1	4	0	4294967295	4294967295	1	1	1	8	0

//0	0	4	0	4294967295	4294967295	1	1	1	8	1
//0	1	4	0	4294967295	4294967295	1	1	1	8	1
//1	0	4	0	4294967295	4294967295	1	1	1	8	1
//1	1	4	0	4294967295	4294967295	1	1	1	8	1

//1	1	4	1	4294967295	4294967295	1	1	3	8	1
//0	1	4	1	4294967295	4294967295	1	1	3	3	1
//0	1	4	0	4294967295	4294967295	1	1	3	3	1



	int l_CurrentBlendState = 0;

	struct bsMode
	{
		BOOL blendEnable;
		D3D11_BLEND SrcBlend;
		D3D11_BLEND DestBlend;
		D3D11_BLEND_OP BlendOp;
		D3D11_BLEND SrcBlendAlpha;
		D3D11_BLEND DestBlendAlpha;
		D3D11_BLEND_OP BlendOpAlpha;
		UINT8 colorWriteMask;
	};
	
	bsMode l_BSModes[] = {
		{FALSE, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD, 
			D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD,
			D3D11_COLOR_WRITE_ENABLE_ALL}
	};

	std::vector<ID3D11BlendState*> l_BlendStates;
	
//ALPHABLENENABLE:	SRCBLEND:	DESTBLEND:	BLENDOP:	SEPARATEALPHABLENDENABLE:	SRCBLENDALPHA:	DESTBLENDALPHA:	BLENDOPALPHA:	COLORWRITEENABLE:
//0	5	2	1	0	2	1	1	0
//0	5	2	1	0	2	6	1	15
//0	5	2	1	0	2	6	1	0
//0	5	2	1	0	2	6	1	8
//0	5	2	1	1	2	6	1	15
//0	5	6	1	0	2	1	1	15
//0	5	6	1	0	2	6	1	15
//0	5	6	1	0	2	6	1	0
//0	5	6	1	0	2	6	1	8
//0	5	6	1	1	2	6	1	15

//1	5	2	1	1	2	6	1	7
//1	5	2	1	0	2	1	1	15
//1	5	2	1	1	2	6	1	15
//1	5	2	1	0	2	6	1	15
//1	5	6	1	0	2	6	1	15
//1	5	6	1	0	2	6	1	0
//1	5	6	1	0	2	1	1	0
//1	5	6	1	0	2	1	1	15
//1	5	6	1	1	2	6	1	15

//1	2	2	1	0	2	6	1	15
//1	2	1	1	0	2	6	1	8
//1	1	3	1	0	2	6	1	7
//1	14	1	1	0	2	6	1	8



};// namespace


namespace g3dStateMgrDX11
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void Init()
{
	/*
	D3D11_RASTERIZER_DESC desc;
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_BACK;
    desc.FrontCounterClockwise = TRUE;
    desc.DepthBias = 0;
    desc.DepthBiasClamp = 0;
    desc.SlopeScaledDepthBias = 0;
    desc.DepthClipEnable = TRUE;
    desc.ScissorEnable = FALSE;
    desc.MultisampleEnable = FALSE;
    desc.AntialiasedLineEnable = FALSE;

	ID3D11RasterizerState* pState = NULL;
	for (int i = 0; i < NUM_CULL_MODES*NUM_FILL_MODES; i++)
	{
		desc.CullMode = l_RSModes[i].cullMode;
		desc.FillMode = l_RSModes[i].fillMode;
		g2dDX11Global::g_pDevice->CreateRasterizerState( &desc, &pState );
		l_RasterizerStates.push_back(pState);
	}

	desc.MultisampleEnable = TRUE;
	for (int i = 0; i < NUM_CULL_MODES*NUM_FILL_MODES; i++)
	{
		desc.CullMode = l_RSModes[i].cullMode;
		desc.FillMode = l_RSModes[i].fillMode;
		g2dDX11Global::g_pDevice->CreateRasterizerState( &desc, &pState );
		l_RasterizerStates.push_back(pState);
	}

	D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = FALSE;
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ID3D11DepthStencilState* pDSState = NULL;
	for (int i = 0; i < NUM_DEPTHENABLE_MODES*NUM_DEPTHWRITE_MODES; i++)
	{
		dsDesc.DepthEnable = l_DSModes[i].depthEnable;
		dsDesc.DepthWriteMask = l_DSModes[i].depthWriteEnable;
		g2dDX11Global::g_pDevice->CreateDepthStencilState( &dsDesc, &pDSState );
		l_DepthStencilStates.push_back(pDSState);
	}


	D3D11_BLEND_DESC bsDesc;
	bsDesc.AlphaToCoverageEnable = FALSE;
	bsDesc.IndependentBlendEnable = FALSE;
	bsDesc.RenderTarget[0].BlendEnable = FALSE;
    bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bsDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* pBLState = NULL;
	for (int i = 0; i < sizeof(l_BSModes)/sizeof(bsMode); i++)
	{
		bsDesc.RenderTarget[0].BlendEnable = l_BSModes[i].blendEnable;
		bsDesc.RenderTarget[0].SrcBlend = l_BSModes[i].SrcBlend;
		bsDesc.RenderTarget[0].DestBlend = l_BSModes[i].DestBlend;
		bsDesc.RenderTarget[0].BlendOp = l_BSModes[i].BlendOp;
		bsDesc.RenderTarget[0].SrcBlendAlpha = l_BSModes[i].SrcBlendAlpha;
		bsDesc.RenderTarget[0].DestBlendAlpha = l_BSModes[i].DestBlendAlpha;
		bsDesc.RenderTarget[0].BlendOpAlpha = l_BSModes[i].BlendOpAlpha;
		bsDesc.RenderTarget[0].RenderTargetWriteMask = l_BSModes[i].colorWriteMask;
		g2dDX11Global::g_pDevice->CreateBlendState( &bsDesc, &pBLState );
		l_BlendStates.push_back(pBLState);
	}

*/

	// RS state
	D3D11_RASTERIZER_DESC rsDesc;
	
	ZeroMemory( &rsDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FillMode = D3D11_FILL_SOLID;
/////////
// ALERT!!!
	// this flag might have to change depending on the fullscreen quad or mesh rendering.
	// to be settled later.
	// leaving it like this for compatibility with code grabbed from sdk samples.
	rsDesc.FrontCounterClockwise = TRUE;
/////////
	rsDesc.DepthClipEnable = TRUE;

    g2dDX11Global::g_pDevice->CreateRasterizerState(&rsDesc, &l_DefaultRasterizerState);

	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	g2dDX11Global::g_pDevice->CreateRasterizerState(&rsDesc, &l_DefaultRSWire);

	// DS state
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	ZeroMemory( &dsDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );

	dsDesc.DepthEnable		= TRUE;
	dsDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc		= D3D11_COMPARISON_LESS;

	dsDesc.StencilEnable	= FALSE;

	g2dDX11Global::g_pDevice->CreateDepthStencilState( &dsDesc, &l_DefaultDepthStencilState );
	
	// OM state
	D3D11_BLEND_DESC omDesc;

	// additive blend on rendertarget 0
	ZeroMemory( &omDesc, sizeof( D3D11_BLEND_DESC ) );
	omDesc.RenderTarget[0].BlendEnable		= TRUE;
	omDesc.RenderTarget[0].BlendOp			= D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].SrcBlend			= D3D11_BLEND_ONE;
	omDesc.RenderTarget[0].DestBlend		= D3D11_BLEND_ZERO;
	omDesc.RenderTarget[0].BlendOpAlpha		= D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].SrcBlendAlpha	= D3D11_BLEND_ONE;
	omDesc.RenderTarget[0].DestBlendAlpha	= D3D11_BLEND_ZERO;
	omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g2dDX11Global::g_pDevice->CreateBlendState( &omDesc, &l_DefaultBlendState );

//	ZeroMemory( &omDesc, sizeof( D3D11_BLEND_DESC ) );
//	omDesc.RenderTarget[0].BlendEnable		= TRUE;
//	omDesc.RenderTarget[0].BlendOp			= D3D11_BLEND_OP_ADD;
//	omDesc.RenderTarget[0].SrcBlend			= D3D11_BLEND_ONE;
//	omDesc.RenderTarget[0].DestBlend		= D3D11_BLEND_ZERO;
//	omDesc.RenderTarget[0].BlendOpAlpha		= D3D11_BLEND_OP_ADD;
//	omDesc.RenderTarget[0].SrcBlendAlpha	= D3D11_BLEND_ONE;
//	omDesc.RenderTarget[0].DestBlendAlpha	= D3D11_BLEND_ZERO;
//	omDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
//	g2dDX11Global::g_pDevice->CreateBlendState( &omDesc, &l_DefaultBlendState );

	// _) Configure the pipeline state
	g2dDX11Global::g_pDeviceContext->RSSetState( l_DefaultRasterizerState );
	g2dDX11Global::g_pDeviceContext->OMSetDepthStencilState( l_DefaultDepthStencilState, 0 );
	float blendFactors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	g2dDX11Global::g_pDeviceContext->OMSetBlendState( l_DefaultBlendState, blendFactors, 0xFFFFFFFF );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void CleanUp()
{
	g2dDX11Global::g_pDeviceContext->RSSetState( NULL );
	g2dDX11Global::g_pDeviceContext->OMSetDepthStencilState( NULL, 0 );
	float blendFactors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	g2dDX11Global::g_pDeviceContext->OMSetBlendState( NULL, blendFactors, 0xFFFFFFFF );

	int n = l_RasterizerStates.size();
	for (int i = 0; i < n; i++)
	{
		l_RasterizerStates[i]->Release();
	}
	l_RasterizerStates.clear();

	n = l_DepthStencilStates.size();
	for (int i = 0; i < n; i++)
	{
		l_DepthStencilStates[i]->Release();
	}
	l_DepthStencilStates.clear();

	n = l_BlendStates.size();
	for (int i = 0; i < n; i++)
	{
		l_BlendStates[i]->Release();
	}
	l_BlendStates.clear();

	l_DefaultRasterizerState->Release();
	l_DefaultRasterizerState = NULL;
	l_DefaultRSWire->Release();
	l_DefaultRSWire = NULL;

	l_DefaultBlendState->Release();
	l_DefaultBlendState = NULL;
	l_DefaultDepthStencilState->Release();
	l_DefaultDepthStencilState = NULL;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void SetCull(eCull i_Cull)
{
	l_CurrentCull = i_Cull;
}
void SetFill(eFill i_Fill)
{
	l_CurrentFill = i_Fill;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void SetDefault()
{
	RSSetDefault();
	BSSetDefault();
	DSSetDefault();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void RSSetDefault()
{
//	g2dDX11Global::g_pDeviceContext->RSSetState(NULL);
	g2dDX11Global::g_pDeviceContext->RSSetState( l_DefaultRasterizerState );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void RSSetWire()
{
	g2dDX11Global::g_pDeviceContext->RSSetState( l_DefaultRSWire );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void BSSetDefault()
{
//	g2dDX11Global::g_pDeviceContext->OMSetBlendState(NULL, NULL, 0xffffffff);
	float blendFactors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	g2dDX11Global::g_pDeviceContext->OMSetBlendState( l_DefaultBlendState, blendFactors, 0xFFFFFFFF );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void DSSetDefault()
{
//	g2dDX11Global::g_pDeviceContext->OMSetDepthStencilState(NULL, 0);
	g2dDX11Global::g_pDeviceContext->OMSetDepthStencilState( l_DefaultDepthStencilState, 0 );
}

//------------------------------------------------------------------------
// call this immediately before draw to commit the state to the device
//------------------------------------------------------------------------
void SetDeviceStates()
{
	// index is l_Multisample*(NUM_FILL_MODES*NUM_CULL_MODES) + l_CurrentFill*NUM_CULL_MODES + l_CurrentCull
	// ( currently no multisample. )
//	g2dDX11Global::g_pDevice->RSSetState(l_RasterizerStates[0 + l_CurrentFill*NUM_CULL_MODES + l_CurrentCull]);

//	g2dDX11Global::g_pDevice->OMSetBlendState(l_BlendStates[l_CurrentBlendState], NULL, 0);

//	g2dDX11Global::g_pDevice->OMSetDepthStencilState(l_DepthStencilStates[l_CurrentDepthEnable*NUM_DEPTHWRITE_MODES + l_CurrentDepthWrite], 0);
}

}; // namespace g3dStateMgr
