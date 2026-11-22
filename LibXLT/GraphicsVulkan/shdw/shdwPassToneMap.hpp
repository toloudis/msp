/****************************************************************************\
**	shdwPassToneMap.hpp
**
**	Tone mapping, bloom and star for the HDR renderer.
**
**	GraphicsVulkan port of GraphicsDX11's shdwPassToneMap (which has no
**	header comment or include guard).
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_SHDW_PASSTONEMAP_HPP
#error shdwPassToneMap.hpp multiply included
#endif
#define GVK_SHDW_PASSTONEMAP_HPP

class camCamera;
class g2dRenderTarget;
class g3dScene;

namespace gvk
{

class matRenderTargetTexture;

class shdwPassToneMap
{
public:
	static void InitStates();

	static void CleanupStates();

	shdwPassToneMap();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ToneMap(g2dRenderTarget* io_pDest,
		matRenderTargetTexture* i_pHDRColors,
		matRenderTargetTexture* i_pBloom, 
		matRenderTargetTexture* i_pStar,
		matRenderTargetTexture* i_pLuminance);

	void Setup(const g3dScene& i_Scene, const camCamera& i_Camera);
	bool Scene_To_SceneScaled(matRenderTargetTexture* io_ScaledTex, matRenderTargetTexture* i_SrcTex);
	bool SceneScaled_To_BrightPass(matRenderTargetTexture* i_pSrcTex, matRenderTargetTexture* io_pDstTex);
	bool BrightPass_To_StarSource(matRenderTargetTexture* i_pSrcTex, matRenderTargetTexture* io_pDstTex);
	bool StarSource_To_BloomSource(matRenderTargetTexture* i_pSrcTex, matRenderTargetTexture* io_pDstTex);
	bool RenderStar(matRenderTargetTexture* i_pSrcTex, 
		matRenderTargetTexture** io_pStarTex);
	bool RenderBloom(matRenderTargetTexture* i_pBloomSource,
		matRenderTargetTexture** io_pBloomTex);
	//HRESULT ComputeAvgLuminance();

private:
	int m_eGlareType;

};

}
