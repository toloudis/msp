#include "GraphicsDX11/shdw/shdwFrameBufferMgr.hpp"

#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

namespace
{

// Define reference sizes of textures in pixels that
// are used to determine sampling filter widths.
// This implies that the HDR renderer was developed using sampling filters
// that were pixel-perfect for a window of size REF_X x REF_Y. We want all
// filters to scale to any resolution to avoid sampling different sized 
// regions of the image.
	const float REF_X = (640.0f);
	const float REF_Y = (480.0f);

	// the hdr pixel format we use.
	g2dPFD l_ldrPFD( 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000, 32 );
	g2dPFD l_hdrPFD(g2dPFD::e_RGBA16f, 16*4);

	//-----------------------------------------------------------------------------
	// Name: ClearTexture()
	// Desc: Helper function to clear a texture surface
	//-----------------------------------------------------------------------------
	void ClearTexture( ID3D11RenderTargetView* pTexture )
	{
		float clearColor[4] = {0,0,0,0};
		g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(
			pTexture, clearColor);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwFrameBufferMgr::shdwFrameBufferMgr()
{
	m_pHDRRenderTarget = NULL;
	m_pHDRRenderTargetTex = NULL;
	m_pHDRAATarget = NULL;
	m_pDepthBuffer = NULL;
	m_pDepthBuffer2 = NULL;
	m_pMultiDepthBuffer = NULL;	//depth peeled (nearest in r)
	m_pNormalsBuffer = NULL;
	m_pVelocityBuffer = NULL;
	
	m_pHDRScratchTex0 = NULL;
	m_pHDRScratchTex1 = NULL;
	m_pHDRScratchTex2 = NULL;

	m_pHDRScaledTex = NULL;
	m_pTexBrightPass = NULL;

	m_pTransDepthBuffer1 = NULL;
	m_pTransDepthBuffer2 = NULL;
	m_pTransDepthAux = NULL;			//used as a copy for reverse peeling (only depth used)

	m_OSM.m_pOSM[0] = NULL;
	m_OSM.m_pOSM[1] = NULL;
	m_OSM.m_pOSM[2] = NULL;
	m_OSM.m_pOSM[3] = NULL;

	int i;
	for (i = 0; i < NUM_TONEMAP_TEXTURES; i++)
		m_TexToneMap.m_apTexToneMap[i] = NULL;

	for (i = 0; i < NUM_BLOOM_TEXTURES; i++)
		m_TexBloom.m_apTexBloom[i] = NULL;
	m_pTexBloomSource = NULL;

	for (i = 0; i < NUM_STAR_TEXTURES; i++)
		m_TexStar.m_apTexStar[i] = NULL;
	m_pTexStarSource = NULL;

	m_pHDRBufferBackup = NULL;

	m_pDOFReserveTarget = NULL;
	m_pDOFHBlurTarget = NULL;
	m_pDOFBlurTarget = NULL;

	m_Width = m_Height = -1;
//	int m_nHairShadowMapRes;
//	int m_nHairShadowMapType;
	m_bAA = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwFrameBufferMgr::~shdwFrameBufferMgr()
{
	ReleaseSurfaces();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwFrameBufferMgr::InitMultisample()
{
	// don't init if already initted! 
	if (m_pHDRAATarget == NULL)
	{
		m_pHDRAATarget = new matRenderTargetTexture( false, false );
		m_pHDRAATarget->Make( m_Width, m_Height, l_hdrPFD, true, false, g2dResourceCounterDX11::eRenderTarget, true );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwFrameBufferMgr::CreateSurfaces(g2dRenderTarget* i_pWindow, int i_OverscanSize)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwFrameBufferMgr::CreateSurfaces" );

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);

	// clamp values to 8 so that we are not creating zero-sized textures.
	w = max(w, 1);
	h = max(h, 1);

	// test conditions for recreating surfaces:
	if ((w != m_Width) || 
		(h != m_Height) || 
		(m_pHDRRenderTargetTex == NULL))
	{
		// Crop the scene texture so width and height are evenly divisible by 8.
		// This cropped version of the scene will be used for post processing effects,
		// and keeping everything evenly divisible allows precise control over
		// sampling points within the shaders.
		m_Width = w;
		m_Height = h;
		// clamp to 8 so the target is big enough to prevent errors
		int dwCropWidth = max(w - (w % 8), 1);
		int dwCropHeight = max(h - (h % 8), 1);
		// smaller temp surfaces used for blurring and other intermediate effects:
		// we want them a fixed size so that they have a uniform look across all resolutions.
		// if the window is smaller than the REF resolution, then just scale things down.
		// I consider REF to be the lowest useful resolution.
		int ref_w = (w > REF_X) ? (int)REF_X : dwCropWidth;
		int ref_h = (h > REF_Y) ? (int)REF_Y : dwCropHeight;

		// special knowledge that this render target is a window...
		g2dPFD targetPFD;
		g2dWindow* pWnd = (g2dWindow*)dynamic_cast<g2dWindow*>(i_pWindow);
		if (pWnd != NULL)
			targetPFD = pWnd->GetBackBufferPixelFormat();
		else
			targetPFD = i_pWindow->GetPixelFormat();

		//create the main hdr target as though this is the first init
		m_bAA = g3dPrefs::CurrentPrefs().m_bHDRAA;
		matTextureMgr::ReleaseTexture(m_pHDRRenderTarget);
		m_pHDRRenderTarget = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &l_hdrPFD, false, true, matTextureMgr::e_Framebuffer, false, m_bAA));	//alloc depth with AA

		// create multisampled surfaces.
		CleanupMultisample();
		if (g3dPrefs::CurrentPrefs().m_bHDRAA)
		{
			InitMultisample();
		}

		// Create the HDR scene texture
		matTextureMgr::ReleaseTexture(m_pHDRRenderTargetTex);
		m_pHDRRenderTargetTex = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &l_hdrPFD, false, false, matTextureMgr::e_Framebuffer));

		matTextureMgr::ReleaseTexture(m_pHDRScratchTex0);
		m_pHDRScratchTex0 = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &l_hdrPFD, false, false, matTextureMgr::e_Framebuffer));

		matTextureMgr::ReleaseTexture(m_pHDRScratchTex1);
		m_pHDRScratchTex1 = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &l_hdrPFD, false, false, matTextureMgr::e_Framebuffer));

		matTextureMgr::ReleaseTexture(m_pHDRScratchTex2);
		m_pHDRScratchTex2 = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &l_hdrPFD, false, false, matTextureMgr::e_Framebuffer));

		// Scaled version of the HDR scene texture
		matTextureMgr::ReleaseTexture(m_pHDRScaledTex);
		m_pHDRScaledTex = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(dwCropWidth/4, dwCropHeight/4,
			false, &l_hdrPFD, false, false, matTextureMgr::e_Framebuffer));

		matTextureMgr::ReleaseTexture(m_pDOFReserveTarget);
		m_pDOFReserveTarget = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &targetPFD, false, false, matTextureMgr::e_Framebuffer));
		matTextureMgr::ReleaseTexture(m_pDOFHBlurTarget);
		m_pDOFHBlurTarget = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &targetPFD, false, false, matTextureMgr::e_Framebuffer));
		matTextureMgr::ReleaseTexture(m_pDOFBlurTarget);
		m_pDOFBlurTarget = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &targetPFD, false, false, matTextureMgr::e_Framebuffer));

		matTextureMgr::ReleaseTexture(m_pHDRBufferBackup);
		m_pHDRBufferBackup = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &l_hdrPFD, false, false, matTextureMgr::e_Framebuffer));
//		matTextureMgr::ReleaseTexture(m_pMatteTarget);
//		m_pMatteTarget = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
//			false, &targetPFD, false, false, matTextureMgr::e_Framebuffer));

		// Create the bright-pass filter texture. 
		// Texture has a black border of single texel thickness to fake border 
		// addressing using clamp addressing
		matTextureMgr::ReleaseTexture(m_pTexBrightPass);
		m_pTexBrightPass = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(ref_w / 4 + 2, ref_h / 4 + 2,
			false, &targetPFD, false, false, matTextureMgr::e_Framebuffer));
	
		// Create a texture to be used as the source for the star effect
		// Texture has a black border of single texel thickness to fake border 
		// addressing using clamp addressing
		matTextureMgr::ReleaseTexture(m_pTexStarSource);
		m_pTexStarSource = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(ref_w / 4 + 2, ref_h / 4 + 2,
			false, &targetPFD, false, false, matTextureMgr::e_Framebuffer));
    
		// Create a texture to be used as the source for the bloom effect
		// Texture has a black border of single texel thickness to fake border 
		// addressing using clamp addressing
		matTextureMgr::ReleaseTexture(m_pTexBloomSource);
		m_pTexBloomSource = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(ref_w / 8 + 2, ref_h / 8 + 2,
			false, &targetPFD, false, false, matTextureMgr::e_Framebuffer));

		// Target for the VelocityMap
		matTextureMgr::ReleaseTexture(m_pVelocityBuffer);
		g2dPFD velocityPFD(g2dPFD::e_GR32f, 64);
		m_pVelocityBuffer = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &velocityPFD, false, false, matTextureMgr::e_Framebuffer));

		g2dPFD depthPFD(g2dPFD::e_Float32, 32);

		matTextureMgr::ReleaseTexture(m_pTransDepthBuffer1);
		m_pTransDepthBuffer1 = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &depthPFD, false, true, matTextureMgr::e_Framebuffer, true));

		matTextureMgr::ReleaseTexture(m_pTransDepthBuffer2);
		m_pTransDepthBuffer2 = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &depthPFD, false, true, matTextureMgr::e_Framebuffer, true));

		matTextureMgr::ReleaseTexture(m_pTransDepthAux);
		m_pTransDepthAux = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &depthPFD, false, true, matTextureMgr::e_Framebuffer, true));

		// For each scale stage, create a texture to hold the intermediate results
		// of the luminance calculation
		g2dPFD luminancePFD(g2dPFD::e_Float32, 32);//D3DFMT_R32F
		int i;

		for(i = 0; i < NUM_TONEMAP_TEXTURES; i++)
		{
			if (m_TexToneMap.m_apTexToneMap[i] == NULL)
			{
				int iSampleLen = 1 << (2*i);
				matTextureMgr::ReleaseTexture(m_TexToneMap.m_apTexToneMap[i]);
				m_TexToneMap.m_apTexToneMap[i] = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(iSampleLen, iSampleLen,
					false, &luminancePFD, false, false, matTextureMgr::e_Framebuffer));
			}
		}
		// Create the star effect textures
		for( i=0; i < NUM_STAR_TEXTURES; i++ )
		{
			matTextureMgr::ReleaseTexture(m_TexStar.m_apTexStar[i]);
			m_TexStar.m_apTexStar[i] = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(ref_w /4, ref_h / 4,
				false, &l_hdrPFD, false, false, matTextureMgr::e_Framebuffer));
		}

		// Create the temporary blooming effect textures
		// Texture has a black border of single texel thickness to fake border 
		// addressing using clamp addressing
		for( i=1; i < NUM_BLOOM_TEXTURES; i++ )
		{
			matTextureMgr::ReleaseTexture(m_TexBloom.m_apTexBloom[i]);
			m_TexBloom.m_apTexBloom[i] = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(ref_w / 8 + 2, ref_h / 8 + 2,
				false, &targetPFD, false, false, matTextureMgr::e_Framebuffer));
		}

		// Create the final blooming effect texture
		matTextureMgr::ReleaseTexture(m_TexBloom.m_apTexBloom[0]);
		m_TexBloom.m_apTexBloom[0] = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(ref_w / 8, ref_h / 8,
			false, &targetPFD, false, false, matTextureMgr::e_Framebuffer));

		// Textures with borders must be cleared since scissor rect testing will
		// be used to avoid rendering on top of the border
		ClearTexture( m_pTexBloomSource->GetColorBuffer() );
		ClearTexture( m_pTexBrightPass->GetColorBuffer() );
		ClearTexture( m_pTexStarSource->GetColorBuffer() );

		for( i=0; i < NUM_BLOOM_TEXTURES; i++ )
		{
			ClearTexture( m_TexBloom.m_apTexBloom[i]->GetColorBuffer() );
		}

	}

	int dw = 0;
	int dh = 0;
	if( m_pDepthBuffer )
	{
		m_pDepthBuffer->GetDimensions(dw,dh);
	}
	if ((dw != m_Width+(i_OverscanSize<<1)) || 
		(dh != m_Height+(i_OverscanSize<<1)))
	{
		g2dPFD depthPFD(g2dPFD::e_Float32, 32);

		matTextureMgr::ReleaseTexture(m_pDepthBuffer);
		m_pDepthBuffer = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width+(i_OverscanSize<<1), m_Height+(i_OverscanSize<<1),
			false, &depthPFD, false, true, matTextureMgr::e_Framebuffer));

		matTextureMgr::ReleaseTexture(m_pDepthBuffer2);
		m_pDepthBuffer2 = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width+(i_OverscanSize<<1), m_Height+(i_OverscanSize<<1),
			false, &depthPFD, false, true, matTextureMgr::e_Framebuffer));

		g2dPFD AlldepthPFD(g2dPFD::e_RGBA32f, 32);
		matTextureMgr::ReleaseTexture(m_pMultiDepthBuffer);
		m_pMultiDepthBuffer = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width+(i_OverscanSize<<1), m_Height+(i_OverscanSize<<1),
			false, &AlldepthPFD, false, true, matTextureMgr::e_Framebuffer));
	}
/*
	if( g3dPrefs::CurrentPrefs().m_HairShadowRes != m_nHairShadowMapRes ||
		g3dPrefs::CurrentPrefs().m_HairShadowType != m_nHairShadowMapType )
	{
		m_nHairShadowMapRes = g3dPrefs::CurrentPrefs().m_HairShadowRes;
		m_nHairShadowMapType = g3dPrefs::CurrentPrefs().m_HairShadowType;

		int res = 1<<(m_nHairShadowMapRes+6);
		for( int i = 0; i < 4; i++)
		{
			matTextureMgr::ReleaseTexture(m_OSM.m_pOSM[i]);
			m_OSM.m_pOSM[i] = NULL;
		}

		for( int i = 0; i < ((m_nHairShadowMapType==1) ? 4 : 1); i++ )
		{
			m_OSM.m_pOSM[i] = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture( res, res,
				false, &l_ldrPFD, false, true, matTextureMgr::e_Framebuffer));
		}
	}
*/

	//recreate main target here if state has changed
	if( m_bAA != g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		m_bAA = g3dPrefs::CurrentPrefs().m_bHDRAA;
		matTextureMgr::ReleaseTexture(m_pHDRRenderTarget);
		m_pHDRRenderTarget = dynamic_cast<matRenderTargetTexture*>(matTextureMgr::CreateRenderTargetTexture(m_Width, m_Height,
			false, &l_hdrPFD, false, true, matTextureMgr::e_Framebuffer, false, m_bAA));	//alloc depth with AA
	}

	// It is possible that the width/height didn't change but the AA state did.
	// If there is AA, and if the width/height change required the AA surface to be recreated, 
	// then this code won't do anything.
	if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		InitMultisample();
	}
	else
	{
		CleanupMultisample();
	}


	D3DPERF_EndEvent();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwFrameBufferMgr::ReleaseSurfaces()
{
	CleanupMultisample();

	matTextureMgr::ReleaseTexture(m_pHDRRenderTarget);
	m_pHDRRenderTarget = NULL;
	matTextureMgr::ReleaseTexture(m_pHDRRenderTargetTex);
	m_pHDRRenderTargetTex = NULL;
	matTextureMgr::ReleaseTexture(m_pDepthBuffer);
	m_pDepthBuffer = NULL;
	matTextureMgr::ReleaseTexture(m_pDepthBuffer2);
	m_pDepthBuffer2 = NULL;
	matTextureMgr::ReleaseTexture(m_pMultiDepthBuffer);
	m_pMultiDepthBuffer = NULL;
	matTextureMgr::ReleaseTexture(m_pHDRScaledTex);
	m_pHDRScaledTex = NULL;
	matTextureMgr::ReleaseTexture(m_pHDRScratchTex0);
	m_pHDRScratchTex0 = NULL;
	matTextureMgr::ReleaseTexture(m_pHDRScratchTex1);
	m_pHDRScratchTex1 = NULL;
	matTextureMgr::ReleaseTexture(m_pHDRScratchTex2);
	m_pHDRScratchTex2 = NULL;
	matTextureMgr::ReleaseTexture(m_pTexBrightPass);
	m_pTexBrightPass = NULL;
	matTextureMgr::ReleaseTexture(m_pDOFReserveTarget);
	m_pDOFReserveTarget = NULL;
	matTextureMgr::ReleaseTexture(m_pDOFHBlurTarget);
	m_pDOFHBlurTarget = NULL;
	matTextureMgr::ReleaseTexture(m_pDOFBlurTarget);
	m_pDOFBlurTarget = NULL;

	matTextureMgr::ReleaseTexture(m_pHDRBufferBackup);
	m_pHDRBufferBackup = NULL;
//	matTextureMgr::ReleaseTexture(m_pMatteTarget);
//	m_pMatteTarget = NULL;

	matTextureMgr::ReleaseTexture(m_pVelocityBuffer);
	m_pVelocityBuffer = NULL;

	matTextureMgr::ReleaseTexture(m_pTransDepthBuffer1);
	m_pTransDepthBuffer1 = NULL;
	matTextureMgr::ReleaseTexture(m_pTransDepthBuffer2);
	m_pTransDepthBuffer2 = NULL;
	matTextureMgr::ReleaseTexture(m_pTransDepthAux);
	m_pTransDepthAux = NULL;

	matTextureMgr::ReleaseTexture(m_OSM.m_pOSM[0]);
	m_OSM.m_pOSM[0] = NULL;
	matTextureMgr::ReleaseTexture(m_OSM.m_pOSM[1]);
	m_OSM.m_pOSM[1] = NULL;
	matTextureMgr::ReleaseTexture(m_OSM.m_pOSM[2]);
	m_OSM.m_pOSM[2] = NULL;
	matTextureMgr::ReleaseTexture(m_OSM.m_pOSM[3]);
	m_OSM.m_pOSM[3] = NULL;

	int i;
	for (i = 0; i < NUM_TONEMAP_TEXTURES; i++)
	{
		matTextureMgr::ReleaseTexture(m_TexToneMap.m_apTexToneMap[i]);
		m_TexToneMap.m_apTexToneMap[i] = NULL;
	}

	for (i = 0; i < NUM_BLOOM_TEXTURES; i++)
	{
		matTextureMgr::ReleaseTexture(m_TexBloom.m_apTexBloom[i]);
		m_TexBloom.m_apTexBloom[i] = NULL;
	}
	matTextureMgr::ReleaseTexture(m_pTexBloomSource);
	m_pTexBloomSource = NULL;

	for (i = 0; i < NUM_STAR_TEXTURES; i++)
	{
		matTextureMgr::ReleaseTexture(m_TexStar.m_apTexStar[i]);
		m_TexStar.m_apTexStar[i] = NULL;
	}
	matTextureMgr::ReleaseTexture(m_pTexStarSource);
	m_pTexStarSource = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwFrameBufferMgr::CleanupMultisample()
{
	// release the multisample surface.
	delete m_pHDRAATarget;
	m_pHDRAATarget = NULL;
}
