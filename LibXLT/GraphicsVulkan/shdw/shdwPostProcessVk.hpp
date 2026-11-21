/****************************************************************************\
**	shdwPostProcessVk.hpp
**
**	The HDR renderers' post chain, taken out of shdwHDRRendererVk so that a
**	renderer that makes its HDR image some other way (GraphicsOptiX's
**	tracer) can finish it the same way. Run takes the HDR scene texture
**	(the frame buffer manager's HDRRenderTargetTex, RGBA16F, alpha the
**	matte's coverage) and the target's depth, and draws into the target:
**	- tone mapping (shdwPassToneMap), with bloom and star;
**	- or, with the matte preference, the matte;
**	- depth of field: Options::m_DofPrep writes each pixel's blurriness
**	  into the HDR scene texture's alpha (shdwHDRRendererVk: shdwPassDOF
**	  over its scene), then the blurred image is blended in;
**	- the post shader (g3dPostProcessing);
**	- with Options::m_bOverlayLayers, the scene's layers after the first;
**	- or one of the HDR debug modes (g3dPrefs::m_HDRDebugMode).
**	It owns the tone mapper; the textures are the frame buffer manager's.
**
**	The helpers (CopyToBackBuf, ...) draw a texture into a target with the
**	HDRLighting effect, as the HDR renderer does.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_SHDW_POSTPROCESSVK_HPP
#error shdwPostProcessVk.hpp multiply included
#endif
#define GVK_SHDW_POSTPROCESSVK_HPP

#include <functional>

class camCamera;
class g2dRenderTarget;
class g3dScene;

namespace gvk
{

class matRenderTargetTexture;
class shdwFrameBufferMgr;
class shdwPassToneMap;

class shdwPostProcessVk
{
public:
	struct Options
	{
		//	The DOF prep draws into the multisampled HDR target and resolves
		//	it into the scene texture (the HDR renderer with HDR AA on)
		bool m_bAATarget = false;
		//	Draw the scene's layers after the first over the result
		bool m_bOverlayLayers = true;
		//	Writes the blurriness into the HDR scene texture's alpha, with
		//	the HDR target current; returns the triangles it drew
		std::function<int(float)> m_DofPrep;
	};

	shdwPostProcessVk();
	~shdwPostProcessVk();
	shdwPostProcessVk(const shdwPostProcessVk&) = delete;
	shdwPostProcessVk& operator=(const shdwPostProcessVk&) = delete;

	static void InitStates();
	static void CleanupStates();

	//--------------------------------------------------------------------
	//	Run - the post chain (see above) into i_pWindow, whose size the
	//	frame buffer manager's surfaces were made for. Returns the
	//	triangles drawn.
	//--------------------------------------------------------------------
	int Run(g2dRenderTarget* i_pWindow, shdwFrameBufferMgr& i_FrameBuffer, const camCamera& i_Camera, const g3dScene& i_Scene,
			float i_fSimTime, const Options& i_Options);

	shdwPassToneMap& ToneMapper() { return *m_pToneMapper; }

	//--------------------------------------------------------------------
	//	Helpers: i_pTex drawn into i_pDest (HDRLighting's SimpleCopyLDR,
	//	or SimpleCopyLumLDR when not isRGB); the current colour target
	//	copied into pTex (SimpleCopy); a texture's luminance as grey; a
	//	texture's alpha
	//--------------------------------------------------------------------
	static void CopyToBackBuf(g2dRenderTarget* i_pDest, matRenderTargetTexture* pTex, bool bDoBlend = false, bool isRGB = true);
	static void CopyBackBufInto(matRenderTargetTexture* pTex);
	// i_pTexFilter: a D3D11 filter name, as fxEffectDesc's (ignored, as on DX11)
	static void LuminanceToGrayscale(g2dRenderTarget* i_pDest, matRenderTargetTexture* pTex,
									 const char* i_pTexFilter = "MIN_MAG_MIP_POINT");
	static void DrawAlpha(matRenderTargetTexture* i_src, g2dRenderTarget* i_dest);

private:
	int RenderDOF(g2dRenderTarget* i_pWindow, shdwFrameBufferMgr& i_FrameBuffer, float i_fSimTime, bool i_debug,
				  const Options& i_Options);

	shdwPassToneMap* m_pToneMapper;
};

}
