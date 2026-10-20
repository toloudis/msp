/*****************************************************************************
**  shdwPassStubsVk.hpp
**
**      Stand-ins for the shdw passes the HDR renderer calls that later
**	tasks port (see g2dStubsVk.hpp for the rules): each has DX11's
**	constructors and Render calls, draws nothing and logs
**	GVK_NOT_YET: <class> once. Each moves to shdw/ when it is ported:
**
**	  shdwPassLPVGI	Plan 5b
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
