/*****************************************************************************
**  g2dFullscreenQuad.cpp
**
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g2d/g2dFullscreenQuad.hpp"

#include "GraphicsDX11/eff/effShaderUtilWin.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"

//#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"

namespace
{
	// Stuff used for drawing the "full screen quad"
	g2dFullscreenQuad::SCREEN_VERTEX				g_svQuad[4];

	ID3D11Buffer*               g_pScreenQuadVB = NULL;
	ID3D11InputLayout*          g_pQuadLayout = NULL;
	ID3D11VertexShader*         g_pQuadVS = NULL;

}; //namespace

void g2dFullscreenQuad::InitFullscreenQuad()
{
	HRESULT hr;
	
	// the dx input layout, corresponds to the struct in the shader
    const D3D11_INPUT_ELEMENT_DESC quadlayout[] =
    {
        { "SV_POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

	const char* fullscreenQuadVSSource = "\
struct QuadVS_Input\
{\
    float4 Pos : SV_POSITION;\
	float2 Tex : TEXCOORD0;\
};\
\
struct QuadVS_Output\
{\
    float4 Pos : SV_POSITION;\
    float2 Tex : TEXCOORD0;\
};\
\
QuadVS_Output QuadVS( QuadVS_Input Input )\
{\
    QuadVS_Output Output;\
    Output.Pos = Input.Pos;\
	Output.Tex = Input.Tex;\
    return Output;\
}\
";

	// compile shader to bytecode

	ID3DBlob* shaderCode = NULL;
	ID3DBlob* errors = NULL;
	// Compile effect 
	hr = D3DX11CompileFromMemory(fullscreenQuadVSSource, strlen(fullscreenQuadVSSource), "fullscreenQuadVS",
		NULL, NULL, "QuadVS", "vs_5_0", 
#ifdef _DEBUG
		D3D10_SHADER_OPTIMIZATION_LEVEL0 | D3D10_SHADER_DEBUG, 
#else
		D3D10_SHADER_OPTIMIZATION_LEVEL0, 
#endif
		0, 0, &shaderCode, &errors, 0);

    if (FAILED(hr))
    { 
		g2dDX11Global::PrintDXError(hr);
        if(errors) 
        { 
			const char* err_msg = reinterpret_cast<const char*>(errors->GetBufferPointer());
			DBG_ERROR(err_msg);
        } 
		throw g2dScreenInitX();
		return;
    } 
 
	// create vtx shader from bytecode

	hr = ( g2dDX11Global::g_pDevice->CreateVertexShader( shaderCode->GetBufferPointer(),
		shaderCode->GetBufferSize(), NULL, &g_pQuadVS ) );
    if (FAILED(hr))
    { 
		g2dDX11Global::PrintDXError(hr);
        if(errors) 
        { 
			const char* err_msg = reinterpret_cast<const char*>(errors->GetBufferPointer());
			DBG_ERROR(err_msg);
        } 
		throw g2dScreenInitX();
		return;
    } 

	// create input layout from shader bytecode and input layout desc
	hr = ( g2dDX11Global::g_pDevice->CreateInputLayout( quadlayout, 2,
		shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), &g_pQuadLayout ) );

	// done with the blob now
	shaderCode->Release( );
	
	// Create a screen quad for render to texture operations
    g_svQuad[0].pos = maVector4d( -1.0f, 1.0f, 0.5f, 1.0f );
    g_svQuad[0].tex = maVector2d( 0.0f, 0.0f );
    g_svQuad[1].pos = maVector4d( -1.0f, -1.0f, 0.5f, 1.0f );
    g_svQuad[1].tex = maVector2d( 0.0f, 1.0f );
    g_svQuad[2].pos = maVector4d( 1.0f, 1.0f, 0.5f, 1.0f );
    g_svQuad[2].tex = maVector2d( 1.0f, 0.0f );
    g_svQuad[3].pos = maVector4d( 1.0f, -1.0f, 0.5f, 1.0f );
    g_svQuad[3].tex = maVector2d( 1.0f, 1.0f );

	// put in vtx buffer
    D3D11_BUFFER_DESC vbdesc =
    {
        4 * sizeof( SCREEN_VERTEX ),
        D3D11_USAGE_DYNAMIC,
        D3D11_BIND_VERTEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE,
        0
    };
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = g_svQuad;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;
    hr = ( g2dDX11Global::g_pDevice->CreateBuffer( &vbdesc, &InitData, &g_pScreenQuadVB ) );
}

void g2dFullscreenQuad::CleanUpFullscreenQuad()
{
	SAFE_RELEASE( g_pScreenQuadVB );
	SAFE_RELEASE( g_pQuadLayout );
	SAFE_RELEASE( g_pQuadVS );
}
/*
void g2dFullscreenQuad::DrawFullScreenQuad11( ID3D11PixelShader* pPS,
                           UINT Width, UINT Height,
						   float offsetPixelsX, float offsetPixelsY,
						   SCREEN_VERTEX* i_InputVtxData)
{
    // Save the old viewport
	
	D3D11_VIEWPORT vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;
	g2dDX11Global::g_pDeviceContext->RSGetViewports( &nViewPorts, vpOld );

	// Setup the viewport to match the backbuffer
	D3D11_VIEWPORT vp;
	vp.Width = (float)Width;
	vp.Height = (float)Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = offsetPixelsX;
	vp.TopLeftY = offsetPixelsY;
	g2dDX11Global::g_pDeviceContext->RSSetViewports( 1, &vp );

	// Change vertex buffer content
	if (i_InputVtxData)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;            
		HRESULT hr = ( g2dDX11Global::g_pDeviceContext->Map( g_pScreenQuadVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );

		memcpy(MappedResource.pData, i_InputVtxData, 4 * sizeof(SCREEN_VERTEX));
		
		g2dDX11Global::g_pDeviceContext->Unmap( g_pScreenQuadVB, 0 );
	}

    UINT strides = sizeof( SCREEN_VERTEX );
    UINT offsets = 0;
    ID3D11Buffer* pBuffers[1] = { g_pScreenQuadVB };

    g2dDX11Global::g_pDeviceContext->IASetInputLayout( g_pQuadLayout );
    g2dDX11Global::g_pDeviceContext->IASetVertexBuffers( 0, 1, pBuffers, &strides, &offsets );
    g2dDX11Global::g_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

    g2dDX11Global::g_pDeviceContext->VSSetShader( g_pQuadVS, NULL, 0 );
	// Set ps shader if it's not NULL, otherwise inherit whatevery pixel shader using currently
	if (pPS)
		g2dDX11Global::g_pDeviceContext->PSSetShader( pPS, NULL, 0 );

    g2dDX11Global::g_pDeviceContext->Draw( 4, 0 );

	// Restroe vertex buffer content
	// Considering most cases the default vtx buffer is used
	if (i_InputVtxData)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;            
		HRESULT hr = ( g2dDX11Global::g_pDeviceContext->Map( g_pScreenQuadVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );

		memcpy(MappedResource.pData, g_svQuad, 4 * sizeof(SCREEN_VERTEX));
		
		g2dDX11Global::g_pDeviceContext->Unmap( g_pScreenQuadVB, 0 );
	}

    // Restore the Old viewport
	g2dDX11Global::g_pDeviceContext->RSSetViewports( nViewPorts, vpOld );
}
*/
void g2dFullscreenQuad::DrawFullScreenQuad11( UINT Width, UINT Height,
											 float offsetPixelsX, float offsetPixelsY,
											 SCREEN_VERTEX* i_InputVtxData)
{
	// Save the old viewport

	D3D11_VIEWPORT vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
	UINT nViewPorts = 1;
	g2dDX11Global::g_pDeviceContext->RSGetViewports( &nViewPorts, vpOld );

	// Setup the viewport to match the backbuffer
	D3D11_VIEWPORT vp;
	vp.Width = (float)Width;
	vp.Height = (float)Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = offsetPixelsX;
	vp.TopLeftY = offsetPixelsY;
	g2dDX11Global::g_pDeviceContext->RSSetViewports( 1, &vp );

	// Change vertex buffer content
	if (i_InputVtxData)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;            
		HRESULT hr = ( g2dDX11Global::g_pDeviceContext->Map( g_pScreenQuadVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );

		memcpy(MappedResource.pData, i_InputVtxData, 4 * sizeof(SCREEN_VERTEX));

		g2dDX11Global::g_pDeviceContext->Unmap( g_pScreenQuadVB, 0 );
	}

	UINT strides = sizeof( SCREEN_VERTEX );
	UINT offsets = 0;
	ID3D11Buffer* pBuffers[1] = { g_pScreenQuadVB };

	g2dDX11Global::g_pDeviceContext->IASetInputLayout( g_pQuadLayout );
	g2dDX11Global::g_pDeviceContext->IASetVertexBuffers( 0, 1, pBuffers, &strides, &offsets );
	g2dDX11Global::g_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	g2dDX11Global::g_pDeviceContext->Draw( 4, 0 );

	// Restroe vertex buffer content
	// Considering most cases the default vtx buffer is used
	if (i_InputVtxData)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;            
		HRESULT hr = ( g2dDX11Global::g_pDeviceContext->Map( g_pScreenQuadVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );

		memcpy(MappedResource.pData, g_svQuad, 4 * sizeof(SCREEN_VERTEX));

		g2dDX11Global::g_pDeviceContext->Unmap( g_pScreenQuadVB, 0 );
	}

	// Restore the Old viewport
	g2dDX11Global::g_pDeviceContext->RSSetViewports( nViewPorts, vpOld );
}

