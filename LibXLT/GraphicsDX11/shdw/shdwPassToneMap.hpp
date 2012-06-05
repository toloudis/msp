#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

class camCamera;
class g2dRenderTarget;
class g3dScene;
class matRenderTargetTexture;

class shdwPassToneMap
{
public:
	static void InitStates();

	static void CleanupStates();

	shdwPassToneMap();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void shdwPassToneMap::ToneMap(g2dRenderTarget* io_pDest,
		matRenderTargetTexture* i_pHDRColors,
		matRenderTargetTexture* i_pBloom, 
		matRenderTargetTexture* i_pStar,
		matRenderTargetTexture* i_pLuminance);

	void Setup(const g3dScene& i_Scene, const camCamera& i_Camera);
	HRESULT Scene_To_SceneScaled(matRenderTargetTexture* io_ScaledTex, matRenderTargetTexture* i_SrcTex);
	HRESULT SceneScaled_To_BrightPass(matRenderTargetTexture* i_pSrcTex, matRenderTargetTexture* io_pDstTex);
	HRESULT BrightPass_To_StarSource(matRenderTargetTexture* i_pSrcTex, matRenderTargetTexture* io_pDstTex);
	HRESULT StarSource_To_BloomSource(matRenderTargetTexture* i_pSrcTex, matRenderTargetTexture* io_pDstTex);
	HRESULT RenderStar(matRenderTargetTexture* i_pSrcTex, 
		matRenderTargetTexture** io_pStarTex);
	HRESULT RenderBloom(matRenderTargetTexture* i_pBloomSource,
		matRenderTargetTexture** io_pBloomTex);
	//HRESULT ComputeAvgLuminance();

private:
	int m_eGlareType;

};
