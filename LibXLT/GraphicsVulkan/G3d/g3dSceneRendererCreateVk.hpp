/****************************************************************************\
**	g3dSceneRendererCreateVk.hpp
**
**	GraphicsVulkan's counterpart of g3dSceneRendererDX11Create (which
**	GraphicsDX11 never installs: shdwPackage's creator is the one used).
**	Here it is the fallback of shdw's creator (shdwSceneRendererCreate),
**	which uses it only for the opacity map renderer (Plan 7): every type
**	is answered with the forward renderer (g3dSceneRendererVk), logged
**	once per type with GVK_NOT_YET. The pick renderer is
**	g3dPickBufferRendererVk, as DX11's.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G3D_SCENERENDERERCREATEVK_HPP
#error g3dSceneRendererCreateVk.hpp multiply included
#endif
#define GVK_G3D_SCENERENDERERCREATEVK_HPP

#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#endif

class g3dProjectedLight;
class matMaterial;
class scObject;

namespace gvk
{

class g3dSceneRendererCreateVk : public g3dSceneRendererCreateImpl
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
