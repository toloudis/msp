#ifdef SHDW_PASSTRANSPARENT_HPP
#error shdwPassTransparent.hpp multiply included
#endif
#define SHDW_PASSTRANSPARENT_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

class shdwPassTraversal;

class camCamera;
class g3dLayer;

class shdwPassTransparent : public g3dRenderPass
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPassTransparent();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwPassTransparent(g3dLayer* i_pLayer,
		const camCamera* i_pCamera,
		matRenderTargetTexture* i_depthTarget1,
		matRenderTargetTexture* i_depthTarget2,
		matRenderTargetTexture* i_depthAux,
		matRenderTargetTexture* i_ScratchTarget,
		matRenderTargetTexture* i_ScratchTarget2,
		g2dRenderTarget* i_destination);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwPassTransparent();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetBuffers(
		matRenderTargetTexture* i_depthTarget1,
		matRenderTargetTexture* i_depthTarget2,
		matRenderTargetTexture* i_depthAux,
		matRenderTargetTexture* i_ScratchTarget,
		matRenderTargetTexture* i_ScratchTarget2,
		g2dRenderTarget* i_destination);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	typedef std::function<int()> ColorPeeledFunction;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual int Render(float i_time);
	int RenderDepthPeeled(float i_time );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int RenderDeferred(float i_time);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetSceneInfo(shdwPassTraversal* i_traversal) {m_sceneInfo = i_traversal;}

	//state management
	static void InitStates();
	static void CleanupStates();

protected:
	shdwPassTraversal*		m_sceneInfo;

	g3dLayer*				m_pLayer;
	const camCamera*		m_pCamera;

	matRenderTargetTexture* m_depthTarget1;
	matRenderTargetTexture* m_depthTarget2;
	matRenderTargetTexture* m_depthAux;
	matRenderTargetTexture* m_SinglePeelTarget;
	matRenderTargetTexture* m_ColorAccumulationTarget;

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int RenderTransparentNodes();
	int RenderPeeledRColors();		//renders object colors at the peeled depth and composites with the accumulation buffer

	//composite overlays the source on top of the dest using the inverse of the src
	void CompositeTextureInverse(matTexture* src, g2dRenderTarget* tgt);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SwapDepthTargets( matRenderTargetTexture*& io_pDst, matRenderTargetTexture*& io_pSrc );	//acquires if ptrs are NULL

};