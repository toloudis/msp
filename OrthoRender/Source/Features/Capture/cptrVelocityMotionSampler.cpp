/*****************************************************************************
**  cptrVelocityMotionSampler.cpp
**
**      see .h
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrVelocityMotionSampler.hpp"

// library
#include "GraphicsDX9/eff/effShaderUtilWin.hpp"
#include "Core/fs/fsLocator.hpp"
#include "GraphicsDX9/g2d/g2dDX9GlobalWin.hpp"
#include c_g2dD3DX9TEX_H
#include "Core/gf/gfPaths.hpp"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

cptrPerPixelVelocitySampler::cptrPerPixelVelocitySampler(int nSamplesPerFrame, g3dViewer* pViewer, int i_Width, int i_Height)
:	cptrMotionSampler(nSamplesPerFrame),
	m_pViewer(pViewer), m_w(i_Width), m_h(i_Height)
{

	fsLocator fx_dir = gfPaths::GetPath(gfPaths::e_ExePath);
	fx_dir.Push("Shaders");
	fx_dir.Push("PixelMotionBlur.fx");
	m_pEffect = effShaderUtilWin::CompileEffect(fx_dir);

	D3DFORMAT g_VelocityTexFormat = D3DFMT_G16R16F;

	// if the format is not allowed, use different one.
    D3DCAPS9 Caps;
    g2dDX9Global::g_pDevice->GetDeviceCaps( &Caps );
    IDirect3D9* pD3D;
    g2dDX9Global::g_pDevice->GetDirect3D( &pD3D );
    D3DDISPLAYMODE DisplayMode;
    g2dDX9Global::g_pDevice->GetDisplayMode( 0, &DisplayMode );
	if( FAILED( pD3D->CheckDeviceFormat( Caps.AdapterOrdinal, Caps.DeviceType,
                    DisplayMode.Format, D3DUSAGE_RENDERTARGET, 
                    D3DRTYPE_TEXTURE, g_VelocityTexFormat ) ) )
        g_VelocityTexFormat = D3DFMT_A16B16G16R16F;

	m_pFullScreenRenderTarget = NULL;
    HRESULT op_result = D3DXCreateTexture( g2dDX9Global::g_pDevice,
		i_Width, i_Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
		D3DPOOL_DEFAULT, &m_pFullScreenRenderTarget );
	
	// Create two floating-point render targets with at least 2 channels.  These will be used to store 
    // velocity of each pixel (one for the current frame, and one for last frame).
	m_pVelocityTexture1 = NULL;
	op_result = D3DXCreateTexture( g2dDX9Global::g_pDevice, 
		i_Width, i_Height, 1, D3DUSAGE_RENDERTARGET, g_VelocityTexFormat, 
		D3DPOOL_DEFAULT, &m_pVelocityTexture1 );
	m_pVelocityTexture2 = NULL;
	op_result = D3DXCreateTexture( g2dDX9Global::g_pDevice, 
		i_Width, i_Height, 1, D3DUSAGE_RENDERTARGET, g_VelocityTexFormat, 
		D3DPOOL_DEFAULT, &m_pVelocityTexture2 );

	// Store pointers to surfaces so we can call SetRenderTarget() later
    op_result = m_pFullScreenRenderTarget->GetSurfaceLevel(0, &m_pFullScreenRenderTargetSurf);
    op_result = m_pVelocityTexture1->GetSurfaceLevel(0, &m_pVelocitySurface1);
    op_result = m_pVelocityTexture2->GetSurfaceLevel(0, &m_pVelocitySurface2);

    m_pCurFrameVelocityTexture = m_pVelocityTexture1;
    m_pLastFrameVelocityTexture = m_pVelocityTexture2;
    m_pCurFrameVelocitySurf = m_pVelocitySurface1;
    m_pLastFrameVelocitySurf = m_pVelocitySurface2;

	SetupFullscreenQuad();

    // Save a pointer to the orignal render target to restore it later
    LPDIRECT3DSURFACE9 pOriginalRenderTarget;
    op_result = g2dDX9Global::g_pDevice->GetRenderTarget( 0, &pOriginalRenderTarget ) ;
	// Clear each RT
    op_result = g2dDX9Global::g_pDevice->SetRenderTarget( 0, m_pFullScreenRenderTargetSurf ) ;
    op_result = g2dDX9Global::g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0) ;
    op_result = g2dDX9Global::g_pDevice->SetRenderTarget( 0, m_pLastFrameVelocitySurf ) ;
    op_result = g2dDX9Global::g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0) ;
    op_result = g2dDX9Global::g_pDevice->SetRenderTarget( 0, m_pCurFrameVelocitySurf ) ;
    op_result = g2dDX9Global::g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0) ;
    // Restore the orignal RT
    op_result = g2dDX9Global::g_pDevice->SetRenderTarget( 0, pOriginalRenderTarget ) ;
    SAFE_RELEASE( pOriginalRenderTarget );
}
cptrPerPixelVelocitySampler::~cptrPerPixelVelocitySampler()
{
    SAFE_RELEASE(m_pFullScreenRenderTargetSurf);
    SAFE_RELEASE(m_pFullScreenRenderTarget);
    SAFE_RELEASE(m_pVelocitySurface1);
    SAFE_RELEASE(m_pVelocityTexture1);
    SAFE_RELEASE(m_pVelocitySurface2);
    SAFE_RELEASE(m_pVelocityTexture2);
}

void cptrPerPixelVelocitySampler::CaptureMotionSample(float timeline_time)
{
    // Swap the current frame's per-pixel velocity texture with  
    // last frame's per-pixel velocity texture
    g2dD3D9TexturePtr pTempTex = m_pCurFrameVelocityTexture;
    m_pCurFrameVelocityTexture = m_pLastFrameVelocityTexture;
    m_pLastFrameVelocityTexture = pTempTex;

    g2dD3D9SurfacePtr pTempSurf = m_pCurFrameVelocitySurf;
    m_pCurFrameVelocitySurf = m_pLastFrameVelocitySurf;
    m_pLastFrameVelocitySurf = pTempSurf;

//    CRenderTargetSet *pTempRTSet = g_pCurFrameRTSet;
//   g_pCurFrameRTSet = g_pLastFrameRTSet;
//    g_pLastFrameRTSet = pTempRTSet;

}

//-----------------------------------------------------------------------------
// Name: SetupFullscreenQuad()
// Desc: Sets up a full screen quad.  First we render to a fullscreen render 
//       target texture, and then we render that texture using this quad to 
//       apply a pixel shader on every pixel of the scene.
//-----------------------------------------------------------------------------
const DWORD cptrPerPixelVelocitySampler::SCREEN_VERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
void cptrPerPixelVelocitySampler::SetupFullscreenQuad()
{
    D3DSURFACE_DESC desc;

    m_pFullScreenRenderTargetSurf->GetDesc(&desc);

    // Ensure that we're directly mapping texels to pixels by offset by 0.5
    // For more info see the doc page titled "Directly Mapping Texels to Pixels"
    FLOAT fWidth5 = (FLOAT)m_w - 0.5f;
    FLOAT fHeight5 = (FLOAT)m_h - 0.5f;

    FLOAT fTexWidth1 = (FLOAT)m_w / (FLOAT)desc.Width;
    FLOAT fTexHeight1 = (FLOAT)m_h / (FLOAT)desc.Height;

    // Fill in the vertex values
    g_Vertex[0].pos = maVector4d(fWidth5, -0.5f, 0.0f, 1.0f);
    g_Vertex[0].clr = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
    g_Vertex[0].tex1 = maVector2d(fTexWidth1, 0.0f);

    g_Vertex[1].pos = maVector4d(fWidth5, fHeight5, 0.0f, 1.0f);
    g_Vertex[1].clr = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
    g_Vertex[1].tex1 = maVector2d(fTexWidth1, fTexHeight1);

    g_Vertex[2].pos = maVector4d(-0.5f, -0.5f, 0.0f, 1.0f);
    g_Vertex[2].clr = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
    g_Vertex[2].tex1 = maVector2d(0.0f, 0.0f);

    g_Vertex[3].pos = maVector4d(-0.5f, fHeight5, 0.0f, 1.0f);
    g_Vertex[3].clr = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.66666f);
    g_Vertex[3].tex1 = maVector2d(0.0f, fTexHeight1);
}

void cptrPerPixelVelocitySampler::CaptureFrame()
{
}
