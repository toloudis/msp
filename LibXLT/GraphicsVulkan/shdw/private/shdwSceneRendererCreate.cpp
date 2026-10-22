/*****************************************************************************
**  shdwSceneRendererCreate.cpp
**
**	shdwSceneRendererCreate creates scene renderer objects that use the
**	stencil shadow technique.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_SHDW_SCENERENDERERCREATE_HPP
#include "GraphicsVulkan/shdw/shdwSceneRendererCreate.hpp"
#endif
#ifndef GVK_G3D_DEPTHMAPRENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dDepthMapRendererVk.hpp"
#endif
#ifndef GVK_G3D_RSMRENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dRSMRendererVk.hpp"
#endif
#ifndef GVK_SHDW_HDRRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwHDRRendererVk.hpp"
#endif
#ifndef GVK_SHDW_SHADOWLAYERRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwShadowLayerRendererVk.hpp"
#endif
#ifndef GVK_SHDW_PLANARREFLECTIONRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwPlanarReflectionRendererVk.hpp"
#endif
#ifndef GVK_SHDW_CUBEMAPRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwCubeMapRendererVk.hpp"
#endif
#ifndef GVK_SHDW_SHADOWSONLYRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwShadowsOnlyRendererVk.hpp"
#endif
#ifndef GVK_SHDW_AOPREVIEWRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwAOPreviewRendererVk.hpp"
#endif
#ifndef GVK_SHDW_GIPREVIEWRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwGIPreviewRendererVk.hpp"
#endif
#ifndef GVK_SHDW_DEPTHRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwDepthRendererVk.hpp"
#endif
#ifndef GVK_SHDW_NORMALRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwNormalRendererVk.hpp"
#endif
#ifndef GVK_SHDW_MATERIALSRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwMaterialsRendererVk.hpp"
#endif
#ifndef GVK_SHDW_GLOWRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwGlowRendererVk.hpp"
#endif
#ifndef GVK_SHDW_REFLECTIONSONLYRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwReflectionsOnlyRendererVk.hpp"
#endif
#ifndef GVK_SHDW_UVRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwUVRendererVk.hpp"
#endif
#ifndef GVK_SHDW_VELOCITYMAPRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwVelocityMapRendererVk.hpp"
#endif
#ifndef GVK_G3D_TEXTURERENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dTextureRendererVk.hpp"
#endif
#ifndef GVK_G3D_PICKBUFFERRENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dPickBufferRendererVk.hpp"
#endif
#ifndef GVK_SHDW_OPACITYMAPRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwOpacityMapRendererVk.hpp"
#endif

namespace gvk
{

shdwSceneRendererCreate::shdwSceneRendererCreate()
{
}
shdwSceneRendererCreate::~shdwSceneRendererCreate()
{
}

//	Each renderer type answers with its port of DX11's choice.

g3dSceneRenderer* shdwSceneRendererCreate::CreateDefaultRenderer()		{ return new shdwHDRRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateHDRRenderer()			{ return new shdwHDRRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateSimpleRenderer()		{ return new shdwShadowLayerRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateTextureRenderer()		{ return new g3dTextureRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateDepthMapRenderer()		{ return new g3dDepthMapRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateRSMRenderer()			{ return new g3dRSMRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateOpacityMapRenderer(g3dProjectedLight* pProjLight)	{ return new shdwOpacityMapRendererVk(pProjLight); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame)
{
	return new shdwCubeMapRendererVk(i_Obj, i_Material, i_RenderPerFrame);
}
g3dSceneRenderer* shdwSceneRendererCreate::CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material)
{
	return new shdwPlanarReflectionRendererVk(i_Obj, i_Material);
}
g3dSceneRenderer* shdwSceneRendererCreate::CreateAmbientOcclusionRenderer()	{ return new shdwAOPreviewRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateGlobalIlluminationRenderer()	{ return new shdwGIPreviewRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateDepthRenderer()		{ return new shdwDepthRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateUVRenderer()			{ return new shdwUVRendererVk(); }
g3dPickRenderer* shdwSceneRendererCreate::CreatePickRenderer()			{ return new g3dPickBufferRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateShadowMaskRenderer()	{ return new shdwShadowsOnlyRendererVk(true); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateIlluminationRenderer()	{ return new shdwShadowsOnlyRendererVk(false); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateNormalRenderer()		{ return new shdwNormalRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateVelocityMapRenderer()	{ return new shdwVelocityMapRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateMaterialsRenderer()	{ return new shdwMaterialsRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateReflectionOnlyRenderer()	{ return new shdwReflectionsOnlyRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateGlowRenderer()			{ return new shdwGlowRendererVk(); }

}
