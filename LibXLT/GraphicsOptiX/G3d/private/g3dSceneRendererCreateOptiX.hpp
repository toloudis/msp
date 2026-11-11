/*****************************************************************************
**  g3dSceneRendererCreateOptiX.hpp
**
**      g3dSceneRendererCreateOptiX makes the OptiX backend's scene
**	renderers (g3dSceneRendererCreate), installed by GraphicsOptiXLayer
**	while it exists.
**
**	Stub: every type is a g3dSceneRendererOptiX (the pick renderer a
**	g3dPickRendererOptiX), which renders nothing.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DSCENERENDERERCREATEOPTIX_HPP
#error g3dSceneRendererCreateOptiX.hpp multiply included
#endif
#define GOX_G3DSCENERENDERERCREATEOPTIX_HPP

#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif

namespace gox
{
	class g3dSceneRendererCreateOptiX : public g3dSceneRendererCreateImpl
	{
		public:
			virtual g3dSceneRenderer* CreateDefaultRenderer();
			virtual g3dSceneRenderer* CreateHDRRenderer();
			virtual g3dSceneRenderer* CreateSimpleRenderer();
			virtual g3dSceneRenderer* CreateTextureRenderer();
			virtual g3dSceneRenderer* CreateDepthMapRenderer();
			virtual g3dSceneRenderer* CreateRSMRenderer();
			virtual g3dSceneRenderer* CreateOpacityMapRenderer(g3dProjectedLight* pProjLight);
			virtual g3dSceneRenderer* CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame);
			virtual g3dSceneRenderer* CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material);
			virtual g3dSceneRenderer* CreateAmbientOcclusionRenderer();
			virtual g3dSceneRenderer* CreateGlobalIlluminationRenderer();
			virtual g3dSceneRenderer* CreateDepthRenderer();
			virtual g3dSceneRenderer* CreateUVRenderer();
			virtual g3dPickRenderer* CreatePickRenderer();
			virtual g3dSceneRenderer* CreateShadowMaskRenderer();
			virtual g3dSceneRenderer* CreateIlluminationRenderer();
			virtual g3dSceneRenderer* CreateNormalRenderer();
			virtual g3dSceneRenderer* CreateVelocityMapRenderer();
			virtual g3dSceneRenderer* CreateMaterialsRenderer();
			virtual g3dSceneRenderer* CreateReflectionOnlyRenderer();
			virtual g3dSceneRenderer* CreateGlowRenderer();
	};
}
