/****************************************************************************\
**	shdwPassHair.cpp
**
**		Render pass to draw hair
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSHAIR_HPP
#error shdwPassHair.hpp multiply included
#endif
#define SHDW_PASSHAIR_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

class shdwPassTraversal;

class camCamera;
class g3dLayer;

class shdwPassHair : public g3dRenderPass
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPassHair();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPassHair(
		const camCamera* i_pCamera,
		matRenderTargetTexture* (&i_Targets)[4],
		g2dRenderTarget* i_destination,
		g2dRenderTarget* i_ZBuffer,
		matRenderTargetTexture* i_Depth,
		matRenderTargetTexture* i_depthTarget1,
		matRenderTargetTexture* i_depthTarget2,
		matRenderTargetTexture* i_depthAux,
		matRenderTargetTexture* i_ScratchTarget1,
		matRenderTargetTexture* i_ScratchTarget2
	);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwPassHair();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual int Render(float i_time);
	int RenderDepthPeeled(float i_time );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal*		m_sceneInfo;

	const camCamera*		m_pCamera;

	g2dRenderTarget*        m_ZBuffer;
	matRenderTargetTexture*	m_pTargets[4];
	matRenderTargetTexture*	m_pDepth;

	//for depth peeling
	matRenderTargetTexture* m_depthTarget1;
	matRenderTargetTexture* m_depthTarget2;
	matRenderTargetTexture* m_depthAux;
	matRenderTargetTexture* m_ScratchTarget1;
	matRenderTargetTexture* m_ScratchTarget2;

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int RenderHairNodes( bool i_bPeeled );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int RenderPeeledRColors();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SwapDepthTargets( matRenderTargetTexture*& io_pDst, matRenderTargetTexture*& io_pSrc );	//acquires if ptrs are NULL
};
