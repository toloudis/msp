/*****************************************************************************
**  shdwPassStubsVk.hpp
**
**      Stand-ins for the shdw passes the HDR renderer calls that later
**	tasks port (see g2dStubsVk.hpp for the rules): each has DX11's
**	constructors and Render calls, draws nothing and logs
**	GVK_NOT_YET: <class> once. Each moves to shdw/ when it is ported:
**
**	  shdwPassFog, shdwPassDOF, shdwPassAOVolumes,
**	  shdwPassSSGI, shdwPassLPVGI				Plan 5b
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_SHDW_PASSSTUBSVK_HPP
#error shdwPassStubsVk.hpp multiply included
#endif
#define GVK_SHDW_PASSSTUBSVK_HPP

#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif

class camCamera;
class g3dLayer;
class g3dScene;

namespace gvk
{
	class matRenderTargetTexture;
	class shdwPassTraversal;

	class shdwPassFog : public g3dRenderPass
	{
	public:
		shdwPassFog(g2dRenderTarget* io_ColorBuffer,
			matRenderTargetTexture* i_DepthBuffer,
			matRenderTargetTexture* i_TempBuffer,
			const g3dScene* i_Scene,
			const camCamera* i_Camera);

		virtual int Render(float i_time);
		void SetSceneInfo(shdwPassTraversal*) {}
	};

	class shdwPassDOF : public g3dRenderPass
	{
	public:
		shdwPassDOF();

		virtual int Render(float i_time);
		void SetSceneInfo(shdwPassTraversal*) {}
	};

	class shdwPassAOVolumes : public g3dRenderPass
	{
	public:
		shdwPassAOVolumes(g2dRenderTarget* i_pFinalTarget, matRenderTargetTexture* i_pAO,
			matRenderTargetTexture* i_pPositions,
			matRenderTargetTexture* i_pNormals,
			matRenderTargetTexture* i_pDepth,
			const camCamera* i_pCamera,
			const ssaoParams& i_Params);

		virtual int Render(float i_time);
		void SetSceneInfo(shdwPassTraversal*) {}
	};

	class shdwPassSSGI : public g3dRenderPass
	{
	public:
		shdwPassSSGI(matRenderTargetTexture* i_NearDepthBuffer,
			matRenderTargetTexture* i_DepthBuffer,
			matRenderTargetTexture* i_DepthBuffer2,
			g2dRenderTarget* i_FullSceneColors,
			matRenderTargetTexture* i_pGISurface,
			matRenderTargetTexture* i_pTmpSurface,
			const ssgiParams& i_SSGIParams,
			matRenderTargetTexture* i_FullSceneColorsTex);

		virtual int Render(float i_time);
		void SetSceneInfo(shdwPassTraversal*) {}
		void SetCamera(const camCamera*) {}
	};

	class shdwPassLPVGI : public g3dRenderPass
	{
	public:
		shdwPassLPVGI(const ssgiParams& i_SSGIParams,
					matRenderTargetTexture* i_Scratch,
					matRenderTargetTexture* i_Depth,
					g2dRenderTarget* i_Target,
					const camCamera* i_pCamera,
					const g3dScene* i_pScene,
					bool i_bIsDeferRender = false);

		virtual int Render(float i_time);
		void SetSceneInfo(shdwPassTraversal*) {}
	};
}
