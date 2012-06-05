//--------------------------------------------------------------------------------------
// File: RayTracingCS11.cpp
//
// Demonstrates how to use Compute Shader to ray trace
//
// Copyright (c) StudioGPU. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsdlg.h"
#include "SDKmisc.h"
#include "skybox11.h"
#include <D3DX11tex.h>
#include <D3DX11.h>
#include <D3DX11core.h>
#include <D3DX11async.h>

// Defines
#define NUM_BLOOM_TEXTURES 2
#define RES_X	320
#define RES_Y	240

// UI
CDXUTDialogResourceManager  g_DialogResourceManager;    // Manager for shared resources of dialogs
CModelViewerCamera          g_Camera;                   // A model viewing camera
CD3DSettingsDlg             g_D3DSettingsDlg;           // Device settings dialog
CDXUTDialog                 g_HUD;                      // Dialog for standard controls
CDXUTDialog                 g_SampleUI;                 // Dialog for sample specific controls
CSkybox11                   g_Skybox;
CDXUTTextHelper*            g_pTxtHelper = NULL;

// Shaders used in CS path
ID3D11ComputeShader*        g_pRayTraceCS = NULL;
ID3D11ComputeShader*        g_pMainCS = NULL;
ID3D11PixelShader*          g_pDumpBufferPS = NULL;

// Blooming effect intermediate buffers used in CS path   
ID3D11UnorderedAccessView*  g_apBufBloomUAV11[NUM_BLOOM_TEXTURES];

ID3D11Texture2D*            g_pTexRender11 = NULL;          // Render target texture for the skybox
ID3D11RenderTargetView*     g_pTexRenderRTV11 = NULL;  
ID3D11ShaderResourceView*   g_pTexRenderRV11 = NULL;    

// CS buffers
ID3D11Buffer*               g_pcbCS = NULL;
ID3D11Buffer*               g_pcbFilterCS = NULL;
ID3D11Buffer*               g_pBufferBlur1 = NULL;

ID3D11UnorderedAccessView*  g_pBlurUAView1 = NULL;

ID3D11ShaderResourceView*   g_pReductionRV1 = NULL;
ID3D11ShaderResourceView*   g_pBlurRV0 = NULL;
ID3D11ShaderResourceView*   g_pRayTraceRV = NULL;
ID3D11ShaderResourceView*   g_apTexBloomRV11[NUM_BLOOM_TEXTURES];

ID3D11SamplerState*         g_pSampleStatePoint = NULL;
ID3D11SamplerState*         g_pSampleStateLinear = NULL;


// Structs / CB
struct CB_PS
{
    float param[4];
};
struct CB_CS
{
    UINT param[4];
};
struct CB_filter
{
    D3DXVECTOR4  avSampleWeights[15];
    union
    {
        struct
        {
            int outputsize[2];
        } o;        
        struct 
        {
            UINT    outputwidth;
            float   finverse;
        } uf;
    };    
    int     inputsize[2];
};

// Stuff used for drawing the "full screen quad"
struct SCREEN_VERTEX
{
    D3DXVECTOR4 pos;
    D3DXVECTOR2 tex;
};
ID3D11Buffer*               g_pScreenQuadVB = NULL;
ID3D11InputLayout*          g_pQuadLayout = NULL;
ID3D11VertexShader*         g_pQuadVS = NULL;
ID3D11PixelShader*          g_pFinalPassPS = NULL;

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                         void* pUserContext );

HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext );
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                      DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D11DestroyDevice( void* pUserContext );
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                 float fElapsedTime, void* pUserContext );

// My forward declarations
HRESULT RayTraceCS11(ID3D11DeviceContext* g_pd3dContext, const DXGI_SURFACE_DESC* pBackBufferDesc , 
				     int NumThreadGroups_X, int NumThreadGroups_Y, int NumThreadGroups_Z );

//--------------------------------------------------------------------------------------
// Helper function to compile an hlsl shader from file, 
// its binary compiled code is returned
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    // find the file
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, szFileName ) );

    // open the file
    HANDLE hFile = CreateFile( str, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN, NULL );
    if( INVALID_HANDLE_VALUE == hFile )
        return E_FAIL;

    // Get the file size
    LARGE_INTEGER FileSize;
    GetFileSizeEx( hFile, &FileSize );

    // create enough space for the file data
    BYTE* pFileData = new BYTE[ FileSize.LowPart ];
    if( !pFileData )
        return E_OUTOFMEMORY;

    // read the data in
    DWORD BytesRead;
    if( !ReadFile( hFile, pFileData, FileSize.LowPart, &BytesRead, NULL ) )
        return E_FAIL; 

    CloseHandle( hFile );

    // Compile the shader
    char pFilePathName[MAX_PATH];        
    WideCharToMultiByte(CP_ACP, 0, str, -1, pFilePathName, MAX_PATH, NULL, NULL);
    ID3DBlob* pErrorBlob;
    hr = D3DCompile( pFileData, FileSize.LowPart, pFilePathName, NULL, NULL, szEntryPoint, szShaderModel, D3D10_SHADER_ENABLE_STRICTNESS, 0, ppBlobOut, &pErrorBlob );

    delete []pFileData;

    if( FAILED(hr) )
    {
        OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        SAFE_RELEASE( pErrorBlob );
        return hr;
    }
    SAFE_RELEASE( pErrorBlob );

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Disable gamma correction on this sample
    DXUTSetIsInGammaCorrectMode( false );

    DXUTSetCallbackMsgProc( MsgProc );
    
    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );

	g_D3DSettingsDlg.Init( &g_DialogResourceManager );
    
    DXUTInit( true, true );                 // Use this line instead to try to create a hardware device

    DXUTSetCursorSettings( true, true );    // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"Ray Tracing CS11" );
    DXUTCreateDevice( D3D_FEATURE_LEVEL_10_0, true, RES_X, RES_Y );
    DXUTMainLoop();                         // Enter into the DXUT render loop

    return DXUTGetExitCode();
}

//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                         void* pUserContext )
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( g_D3DSettingsDlg.IsActive() )
    {
        g_D3DSettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass all windows messages to camera so it can respond to user input
    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// been destroyed, which generally happens as a result of application termination or 
// windowed/full screen toggles. Resources created in the OnD3D11CreateDevice callback 
// should be released here, which generally includes all D3DPOOL_MANAGED resources. 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11DestroyDevice();
    g_D3DSettingsDlg.OnD3D11DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();
    SAFE_DELETE( g_pTxtHelper );

    g_Skybox.OnD3D11DestroyDevice();

    SAFE_RELEASE( g_pFinalPassPS );
    SAFE_RELEASE( g_pRayTraceCS );
    SAFE_RELEASE( g_pMainCS );
    SAFE_RELEASE( g_pDumpBufferPS );

    SAFE_RELEASE( g_pcbCS );
    SAFE_RELEASE( g_pcbFilterCS );

    SAFE_RELEASE( g_pSampleStateLinear );
    SAFE_RELEASE( g_pSampleStatePoint );

    SAFE_RELEASE( g_pScreenQuadVB );
    SAFE_RELEASE( g_pQuadVS );
    SAFE_RELEASE( g_pQuadLayout );
}


void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11ReleasingSwapChain();

    SAFE_RELEASE( g_pTexRender11 );
    SAFE_RELEASE( g_pTexRenderRTV11 );
    SAFE_RELEASE( g_pTexRenderRV11 );

    SAFE_RELEASE( g_pBufferBlur1 );
    SAFE_RELEASE( g_pBlurUAView1 );
    SAFE_RELEASE( g_pReductionRV1 );
    SAFE_RELEASE( g_pBlurRV0 );
    SAFE_RELEASE( g_pRayTraceRV );

    for( int i = 0; i < NUM_BLOOM_TEXTURES; i++ )
    {
        SAFE_RELEASE( g_apTexBloomRV11[i] );
    }

    g_Skybox.OnD3D11ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                      DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // reject any device which doesn't support CS4x
    if ( DeviceInfo->ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x == FALSE )
        return false;

    return true;
}

HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    // Create the render target texture
    // Our skybox will be rendered to this texture for later post-process
    D3D11_TEXTURE2D_DESC Desc;
    ZeroMemory( &Desc, sizeof( D3D11_TEXTURE2D_DESC ) );
    Desc.ArraySize = 1;
    Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    Desc.Usage = D3D11_USAGE_DEFAULT;
    Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    Desc.Width = pBackBufferSurfaceDesc->Width;
    Desc.Height = pBackBufferSurfaceDesc->Height;
    Desc.MipLevels = 1;
    Desc.SampleDesc.Count = 1;
    V_RETURN( pd3dDevice->CreateTexture2D( &Desc, NULL, &g_pTexRender11 ) );

    // Create the render target view
    D3D11_RENDER_TARGET_VIEW_DESC DescRT;
    DescRT.Format = Desc.Format;
    DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    DescRT.Texture2D.MipSlice = 0;
    V_RETURN( pd3dDevice->CreateRenderTargetView( g_pTexRender11, &DescRT, &g_pTexRenderRTV11 ) );

    // Create the resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
    DescRV.Format = Desc.Format;
    DescRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    DescRV.Texture2D.MipLevels = 1;
    DescRV.Texture2D.MostDetailedMip = 0;
    V_RETURN( pd3dDevice->CreateShaderResourceView( g_pTexRender11, &DescRV, &g_pTexRenderRV11 ) );

    // Create the buffers used in full screen blur for CS path
    {
        D3D11_BUFFER_DESC DescBuffer;
        ZeroMemory( &DescBuffer, sizeof(DescBuffer) );
        DescBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        DescBuffer.ByteWidth = sizeof(D3DXVECTOR4) * pBackBufferSurfaceDesc->Width * pBackBufferSurfaceDesc->Height;
        DescBuffer.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        DescBuffer.StructureByteStride = sizeof(D3DXVECTOR4);
        DescBuffer.Usage = D3D11_USAGE_DEFAULT;
        V_RETURN( pd3dDevice->CreateBuffer( &DescBuffer, NULL, &g_pBufferBlur1 ) );

        D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
        ZeroMemory( &DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC) );
        DescUAV.Format = DXGI_FORMAT_UNKNOWN;
        DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        DescUAV.Buffer.FirstElement = 0;
        DescUAV.Buffer.NumElements = DescBuffer.ByteWidth / DescBuffer.StructureByteStride;
        V_RETURN( pd3dDevice->CreateUnorderedAccessView( g_pBufferBlur1, &DescUAV, &g_pBlurUAView1 ) );

        D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
        ZeroMemory( &DescRV, sizeof( DescRV ) );
        DescRV.Format = DXGI_FORMAT_UNKNOWN;
        DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        DescRV.Buffer.FirstElement = DescUAV.Buffer.FirstElement;
        DescRV.Buffer.NumElements = DescUAV.Buffer.NumElements;
        V_RETURN( pd3dDevice->CreateShaderResourceView( g_pBufferBlur1, &DescRV, &g_pRayTraceRV ) );
    }
    return S_OK;
}

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// created, which will happen during application initialization and windowed/full screen 
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these 
// resources need to be reloaded whenever the device is destroyed. Resources created  
// here should be released in the OnD3D11DestroyDevice callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
    HRESULT hr;

    static bool bFirstOnCreateDevice = true;

    // Warn the user that in order to support CS4x, a non-hardware device has been created, continue or quit?
    if ( DXUTGetDeviceSettings().d3d11.DriverType != D3D_DRIVER_TYPE_HARDWARE && bFirstOnCreateDevice )
    {
        if ( MessageBox( 0, L"CS4x capability is missing. "\
                            L"In order to continue, a non-hardware device has been created, "\
                            L"it will be very slow, continue?", L"Warning", MB_ICONEXCLAMATION | MB_YESNO ) != IDYES )
            return E_FAIL;
    }

    bFirstOnCreateDevice = false;

    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
    V_RETURN( g_DialogResourceManager.OnD3D11CreateDevice( pd3dDevice, pd3dImmediateContext ) );
    V_RETURN( g_D3DSettingsDlg.OnD3D11CreateDevice( pd3dDevice ) );
    g_pTxtHelper = new CDXUTTextHelper( pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15 );

    WCHAR strPath[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, L"Light Probes\\uffizi_cross.dds" ) );

    ID3D11Texture2D* pCubeTexture = NULL;
    ID3D11ShaderResourceView* pCubeRV = NULL;
    UINT SupportCaps = 0;
    
    ID3DBlob* pBlob = NULL;

    V_RETURN( CompileShaderFromFile( L"FinalPass.hlsl", "PSFinalPass", "ps_4_0", &pBlob ) );
    V_RETURN( pd3dDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pFinalPassPS ) );
    SAFE_RELEASE( pBlob );

    V_RETURN( CompileShaderFromFile( L"RayTraceCS.hlsl", "Render", "cs_4_0", &pBlob ) );
    V_RETURN( pd3dDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pRayTraceCS ) );
    SAFE_RELEASE( pBlob );

    V_RETURN( CompileShaderFromFile( L"DumpToTexture.hlsl", "PSDump", "ps_4_0", &pBlob ) );
    V_RETURN( pd3dDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pDumpBufferPS ) );
    SAFE_RELEASE( pBlob );

    V_RETURN( CompileShaderFromFile( L"FinalPass.hlsl", "QuadVS", "vs_4_0", &pBlob ) );
    V_RETURN( pd3dDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pQuadVS ) );
    const D3D11_INPUT_ELEMENT_DESC quadlayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    V_RETURN( pd3dDevice->CreateInputLayout( quadlayout, 2, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &g_pQuadLayout ) );
    SAFE_RELEASE( pBlob );

    // Setup constant buffers
    D3D11_BUFFER_DESC Desc;
    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.MiscFlags = 0;    
    Desc.ByteWidth = sizeof( CB_CS );
    V_RETURN( pd3dDevice->CreateBuffer( &Desc, NULL, &g_pcbCS ) );

    Desc.ByteWidth = sizeof( CB_filter );
    V_RETURN( pd3dDevice->CreateBuffer( &Desc, NULL, &g_pcbFilterCS ) );

    // Samplers
    D3D11_SAMPLER_DESC SamplerDesc;
    ZeroMemory( &SamplerDesc, sizeof(SamplerDesc) );
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    V_RETURN( pd3dDevice->CreateSamplerState( &SamplerDesc, &g_pSampleStateLinear ) );

    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    V_RETURN( pd3dDevice->CreateSamplerState( &SamplerDesc, &g_pSampleStatePoint ) );

    // Create a screen quad for render to texture operations
    SCREEN_VERTEX svQuad[4];
    svQuad[0].pos = D3DXVECTOR4( -1.0f, 1.0f, 0.5f, 1.0f );
    svQuad[0].tex = D3DXVECTOR2( 0.0f, 0.0f );
    svQuad[1].pos = D3DXVECTOR4( 1.0f, 1.0f, 0.5f, 1.0f );
    svQuad[1].tex = D3DXVECTOR2( 1.0f, 0.0f );
    svQuad[2].pos = D3DXVECTOR4( -1.0f, -1.0f, 0.5f, 1.0f );
    svQuad[2].tex = D3DXVECTOR2( 0.0f, 1.0f );
    svQuad[3].pos = D3DXVECTOR4( 1.0f, -1.0f, 0.5f, 1.0f );
    svQuad[3].tex = D3DXVECTOR2( 1.0f, 1.0f );

    D3D11_BUFFER_DESC vbdesc =
    {
        4 * sizeof( SCREEN_VERTEX ),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_VERTEX_BUFFER,
        0,
        0
    };
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = svQuad;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;
    V_RETURN( pd3dDevice->CreateBuffer( &vbdesc, &InitData, &g_pScreenQuadVB ) );

    return S_OK;
}


void DrawFullScreenQuad11( ID3D11DeviceContext* pd3dImmediateContext, 
                           ID3D11PixelShader* pPS,
                           UINT Width, UINT Height )
{
    // Save the old viewport
    D3D11_VIEWPORT vpOld[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
    UINT nViewPorts = 1;
    pd3dImmediateContext->RSGetViewports( &nViewPorts, vpOld );

    // Setup the viewport to match the backbuffer
    D3D11_VIEWPORT vp;
    vp.Width = (float)Width;
    vp.Height = (float)Height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pd3dImmediateContext->RSSetViewports( 1, &vp );

    UINT strides = sizeof( SCREEN_VERTEX );
    UINT offsets = 0;
    ID3D11Buffer* pBuffers[1] = { g_pScreenQuadVB };

    pd3dImmediateContext->IASetInputLayout( g_pQuadLayout );
    pd3dImmediateContext->IASetVertexBuffers( 0, 1, pBuffers, &strides, &offsets );
    pd3dImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

    pd3dImmediateContext->VSSetShader( g_pQuadVS, NULL, 0 );
    pd3dImmediateContext->PSSetShader( pPS, NULL, 0 );
    pd3dImmediateContext->Draw( 4, 0 );

    // Restore the Old viewport
    pd3dImmediateContext->RSSetViewports( nViewPorts, vpOld );
}

HRESULT GetSampleWeights_D3D11( D3DXVECTOR4* avColorWeight,
                                float fDeviation, float fMultiplier )
{
    // Fill the center texel
    float weight = 1.0f;
    avColorWeight[7] = D3DXVECTOR4( weight, weight, weight, 1.0f );

    // Fill the right side
    for( int i = 1; i < 8; i++ )
    {
        weight = fMultiplier;
        avColorWeight[7-i] = D3DXVECTOR4( weight, weight, weight, 1.0f );
    }

    // Copy to the left side
    for( int i = 8; i < 15; i++ )
    {
        avColorWeight[i] = avColorWeight[14 - i];
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Convert buffer result output from CS to a texture, used in CS path
//--------------------------------------------------------------------------------------
HRESULT DumpToTexture( ID3D11DeviceContext* pd3dImmediateContext, DWORD dwWidth, DWORD dwHeight,
                       ID3D11ShaderResourceView* pFromRV, ID3D11RenderTargetView* pToRTV )
{
    HRESULT hr = S_OK;
    
    ID3D11ShaderResourceView* aRViews[ 1 ] = { pFromRV };
    pd3dImmediateContext->PSSetShaderResources( 0, 1, aRViews );

    ID3D11RenderTargetView* aRTViews[ 1 ] = { pToRTV };
    pd3dImmediateContext->OMSetRenderTargets( 1, aRTViews, NULL );          

    D3D11_MAPPED_SUBRESOURCE MappedResource;            
    V( pd3dImmediateContext->Map( g_pcbCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
    UINT* p = (UINT*)MappedResource.pData;
    p[0] = dwWidth;
    p[1] = dwHeight;
    pd3dImmediateContext->Unmap( g_pcbCS, 0 );
    ID3D11Buffer* ppCB[1] = { g_pcbCS };
    pd3dImmediateContext->PSSetConstantBuffers( 0, 1, ppCB );

    DrawFullScreenQuad11( pd3dImmediateContext, g_pDumpBufferPS, dwWidth, dwHeight );

    return hr;
}

//--------------------------------------------------------------------------------------
// Helper function which makes CS invocation more convenient
//--------------------------------------------------------------------------------------
void RunComputeShader( ID3D11DeviceContext* pd3dImmediateContext,
                       ID3D11ComputeShader* pComputeShader,
                       UINT nNumViews, ID3D11ShaderResourceView** pShaderResourceViews, 
                       ID3D11Buffer* pCBCS, void* pCSData, DWORD dwNumDataBytes,
                       ID3D11UnorderedAccessView* pUnorderedAccessView,
                       UINT X, UINT Y, UINT Z )
{
    HRESULT hr = S_OK;
    
    pd3dImmediateContext->CSSetShader( pComputeShader, NULL, 0 );
    pd3dImmediateContext->CSSetShaderResources( 0, nNumViews, pShaderResourceViews );
    pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, &pUnorderedAccessView, (UINT*)&pUnorderedAccessView );
    if ( pCBCS )
    {
        D3D11_MAPPED_SUBRESOURCE MappedResource;
        V( pd3dImmediateContext->Map( pCBCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
        memcpy( MappedResource.pData, pCSData, dwNumDataBytes );
        pd3dImmediateContext->Unmap( pCBCS, 0 );
        ID3D11Buffer* ppCB[1] = { pCBCS };
        pd3dImmediateContext->CSSetConstantBuffers( 0, 1, ppCB );
    }

    pd3dImmediateContext->Dispatch( X, Y, Z );

    ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
    pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, ppUAViewNULL, (UINT*)(&ppUAViewNULL) );

    ID3D11ShaderResourceView* ppSRVNULL[3] = { NULL, NULL, NULL };
    pd3dImmediateContext->CSSetShaderResources( 0, 3, ppSRVNULL );
    ID3D11Buffer* ppBufferNULL[1] = { NULL };
    pd3dImmediateContext->CSSetConstantBuffers( 0, 1, ppBufferNULL );
}

void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                 float fElapsedTime, void* pUserContext )
{
    // Store off original render target, this is the back buffer of the swap chain
    ID3D11RenderTargetView* pOrigRTV = NULL;
    ID3D11DepthStencilView* pOrigDSV = NULL;
    pd3dImmediateContext->OMGetRenderTargets( 1, &pOrigRTV, &pOrigDSV );

    // Clear the render target & depth stencil
    float ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f } ; // red, green, blue, alpha
    pd3dImmediateContext->ClearRenderTargetView( g_pTexRenderRTV11, ClearColor );
    pd3dImmediateContext->ClearDepthStencilView( pOrigDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );    

    HRESULT hr;

    const DXGI_SURFACE_DESC* pBackBufferDesc = DXUTGetDXGIBackBufferSurfaceDesc();

    // g_pTexRender11 is bound as the render target, release it here,
    // as it will be used later as the input texture to the CS
    ID3D11RenderTargetView* ppRTVNULL[1] = { NULL };
    pd3dImmediateContext->OMSetRenderTargets( 1, ppRTVNULL, pOrigDSV );

	RayTraceCS11(pd3dImmediateContext, pBackBufferDesc, RES_X, 1, 1);

    DumpToTexture( pd3dImmediateContext, pBackBufferDesc->Width, pBackBufferDesc->Height, g_pRayTraceRV, g_pTexRenderRTV11 );

    // Restore original render targets
    ID3D11RenderTargetView* aRTViews[ 1 ] = { pOrigRTV };
    pd3dImmediateContext->OMSetRenderTargets( 1, aRTViews, pOrigDSV );        

    // Tone-mapping
    ID3D11ShaderResourceView* aRViews[ 3 ] = { g_pTexRenderRV11, g_pReductionRV1, false ? g_apTexBloomRV11[0] : NULL };
    pd3dImmediateContext->PSSetShaderResources( 0, 3, aRViews );

    D3D11_MAPPED_SUBRESOURCE MappedResource;            
    V( pd3dImmediateContext->Map( g_pcbCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
    CB_PS* pcbCS = ( CB_PS* )MappedResource.pData;
    pcbCS->param[0] = 1.0f / ((int)pow(3.0f, 6-1)*(int)pow(3.0f, 6-1));
    pd3dImmediateContext->Unmap( g_pcbCS, 0 );
    ID3D11Buffer* ppCB[1] = { g_pcbCS };
    pd3dImmediateContext->PSSetConstantBuffers( 0, 1, ppCB );

    ID3D11SamplerState* aSamplers[] = { g_pSampleStatePoint, g_pSampleStateLinear };
    pd3dImmediateContext->PSSetSamplers( 0, 2, aSamplers );

    DrawFullScreenQuad11( pd3dImmediateContext, g_pFinalPassPS, pBackBufferDesc->Width, pBackBufferDesc->Height );

	// End
    ID3D11ShaderResourceView* ppSRVNULL[3] = { NULL, NULL, NULL };
    pd3dImmediateContext->PSSetShaderResources( 0, 3, ppSRVNULL );  
    
    SAFE_RELEASE( pOrigRTV );
    SAFE_RELEASE( pOrigDSV );
}


HRESULT RayTraceCS11(ID3D11DeviceContext* g_pd3dContext, const DXGI_SURFACE_DESC* pBackBufferDesc , 
				    int NumThreadGroups_X, int NumThreadGroups_Y, int NumThreadGroups_Z ) 
{
    HRESULT hr = S_OK;
    
    ID3D11ShaderResourceView* aRViews[ 1 ] = { g_pBlurRV0 };
    CB_filter cbFilter;
    GetSampleWeights_D3D11( cbFilter.avSampleWeights, 3.0f, 1.25f );
    cbFilter.o.outputsize[0] = pBackBufferDesc->Width;
    cbFilter.o.outputsize[1] = pBackBufferDesc->Height;
    cbFilter.inputsize[0] = pBackBufferDesc->Width;
    cbFilter.inputsize[1] = pBackBufferDesc->Height;

    RunComputeShader( g_pd3dContext, 
                      g_pRayTraceCS,
                      1, aRViews,
                      g_pcbFilterCS, &cbFilter, sizeof(cbFilter),
                      g_pBlurUAView1,
                      NumThreadGroups_X, NumThreadGroups_Y, NumThreadGroups_Z );

    return hr;
}
