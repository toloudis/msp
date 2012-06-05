/*****************************************************************************
**  cptrVelocityMotionSampler.hpp
**
**      The Capture mode
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_VELOCITYMOTIONSAMPLER_HPP
#error cptrVelocityMotionSampler.hpp multiply included
#endif
#define CPTR_VELOCITYMOTIONSAMPLER_HPP

#ifndef CPTR_MOTIONSAMPLER_HPP
#include "Features/Capture/cptrMotionSampler.hpp"
#endif

#ifndef G2D_DX9TYPES_HPP
#include "GraphicsDX9/g2d/g2dDX9Types.hpp"
#endif

#ifndef MA_VECTOR4D_HPP
#include "Core/ma/maVector4d.hpp"
#endif
#ifndef MA_VECTOR2D_HPP
#include "Core/ma/maVector2d.hpp"
#endif

class g3dViewer;
class matRenderTargetTexture;
class matShaderEffect;

//////////////////////////////////
// EXPERIMENTAL! NOT DONE YET!
// DO NOT INSTANTIATE!
// THIS CODE IS NOT FINISHED!
//////////////////////////////////
class cptrPerPixelVelocitySampler : public cptrMotionSampler
{
	cptrPerPixelVelocitySampler(int nSamplesPerFrame, g3dViewer* pViewer,
		int i_Width, int i_Height);
	virtual ~cptrPerPixelVelocitySampler();
    virtual void CaptureMotionSample(float timeline_time);
	virtual void CaptureFrame();

protected:
	// the 3d scene renderer
	g3dViewer *m_pViewer;

	matShaderEffect* m_pEffect;

	g2dD3D9TexturePtr m_pFullScreenRenderTarget;
	g2dD3D9SurfacePtr m_pFullScreenRenderTargetSurf;
	g2dD3D9TexturePtr m_pVelocityTexture1;
	g2dD3D9SurfacePtr m_pVelocitySurface1;
	g2dD3D9TexturePtr m_pVelocityTexture2;
	g2dD3D9SurfacePtr m_pVelocitySurface2;

    g2dD3D9TexturePtr m_pCurFrameVelocityTexture;
    g2dD3D9TexturePtr m_pLastFrameVelocityTexture;
    g2dD3D9SurfacePtr m_pCurFrameVelocitySurf;
    g2dD3D9SurfacePtr m_pLastFrameVelocitySurf;


	struct SCREEN_VERTEX 
	{
		maVector4d pos;
		DWORD       clr;
		maVector2d tex1;
		static const DWORD FVF;
	};
	// full screen quad
	SCREEN_VERTEX           g_Vertex[4];
	void SetupFullscreenQuad();


	int m_w, m_h;
};

