/****************************************************************************\
**	shdwSceneRendererCreate.hpp
**
**	shdwSceneRendererCreate creates scene renderer objects that use the
**	stencil shadow technique.
**
**	GraphicsVulkan port of GraphicsDX11's shdwSceneRendererCreate, which
**	shdwPackage installs. A renderer type whose shdw renderer isn't ported
**	yet is answered by g3dSceneRendererCreateVk (the forward renderer,
**	logged once per type with GVK_NOT_YET), as are all of them until Plan
**	5a's tasks switch them over; the pick renderer is its stub.
**
**	Extra Large Technology
**	Copyright(C) 2005. - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_SCENERENDERERCREATE_HPP
#error shdwSceneRendererCreate.hpp multiply included
#endif
#define GVK_SHDW_SCENERENDERERCREATE_HPP

#ifndef GVK_G3D_SCENERENDERERCREATEVK_HPP
#include "GraphicsVulkan/G3d/g3dSceneRendererCreateVk.hpp"
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

private:
		//	The forward renderer standing in for types not ported yet
		g3dSceneRendererCreateVk m_Fallback;
};

}
