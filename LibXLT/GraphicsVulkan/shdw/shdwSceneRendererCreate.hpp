/****************************************************************************\
**	shdwSceneRendererCreate.hpp
**
**	shdwSceneRendererCreate creates scene renderer objects that use the
**	stencil shadow technique.
**
**	GraphicsVulkan port of GraphicsDX11's shdwSceneRendererCreate, which
**	shdwPackage installs. The default and HDR renderers are
**	shdwHDRRendererVk; every other type gets its port, as on DX11.
**
**	Extra Large Technology
**	Copyright(C) 2005. - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_SCENERENDERERCREATE_HPP
#error shdwSceneRendererCreate.hpp multiply included
#endif
#define GVK_SHDW_SCENERENDERERCREATE_HPP

#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class g3dProjectedLight;
class matMaterial;
class scObject;

namespace gvk
{

class shdwSceneRendererCreate : public g3dSceneRendererCreateImpl
{
public:
		shdwSceneRendererCreate();
		~shdwSceneRendererCreate();

		virtual g3dSceneRenderer* CreateDefaultRenderer( );
		virtual g3dSceneRenderer* CreateHDRRenderer( );
		virtual g3dSceneRenderer* CreateSimpleRenderer( );
		virtual g3dSceneRenderer* CreateTextureRenderer( );
		virtual g3dSceneRenderer* CreateDepthMapRenderer( );
		virtual g3dSceneRenderer* CreateRSMRenderer( );
		virtual g3dSceneRenderer* CreateOpacityMapRenderer( g3dProjectedLight* pProjLight );
		virtual g3dSceneRenderer* CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame);
		virtual g3dSceneRenderer* CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material);
		virtual g3dSceneRenderer* CreateAmbientOcclusionRenderer( );
		virtual g3dSceneRenderer* CreateGlobalIlluminationRenderer( );
		virtual g3dSceneRenderer* CreateDepthRenderer( );
		virtual g3dSceneRenderer* CreateUVRenderer( );
		virtual g3dPickRenderer* CreatePickRenderer( );
		virtual g3dSceneRenderer* CreateShadowMaskRenderer( );
		virtual g3dSceneRenderer* CreateIlluminationRenderer( );
		virtual g3dSceneRenderer* CreateNormalRenderer( );
		virtual g3dSceneRenderer* CreateVelocityMapRenderer( );
		virtual g3dSceneRenderer* CreateMaterialsRenderer( );
		virtual g3dSceneRenderer* CreateReflectionOnlyRenderer();
		virtual g3dSceneRenderer* CreateGlowRenderer();
};

}
