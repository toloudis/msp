/****************************************************************************\
**	g3dSceneRendererCreateVk.hpp
**
**	The GraphicsVulkan renderer creator (GraphicsDX11's renderers are made
**	by g3dSceneRendererDX11Create and, in MachStudio, shdwPackage's
**	creator, whose default is the HDR renderer).
**
**	Until the shdw renderers are ported (Plans 5 and 6), every renderer
**	type the creator can't build yet, the default and HDR ones included,
**	is answered with the forward renderer (g3dSceneRendererVk), logged once
**	per type with GVK_NOT_YET. The pick renderer is a stub that picks
**	nothing (it isn't a scene renderer).
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
