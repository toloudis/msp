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
#ifndef GVK_SHDW_HDRRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwHDRRendererVk.hpp"
#endif

namespace gvk
{

shdwSceneRendererCreate::shdwSceneRendererCreate()
{
}
shdwSceneRendererCreate::~shdwSceneRendererCreate()
{
}

//	Each renderer type answers with its shdw renderer once that is ported
//	(DX11's choice is in the comment); until then, with the fallback.

g3dSceneRenderer* shdwSceneRendererCreate::CreateDefaultRenderer()		{ return new shdwHDRRendererVk(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateHDRRenderer()			{ return new shdwHDRRendererVk(); }
//	shdwShadowLayerRendererDX11 (Plan 5a, Task 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateSimpleRenderer()		{ return m_Fallback.CreateSimpleRenderer(); }
//	g3dTextureRendererDX11 (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateTextureRenderer()		{ return m_Fallback.CreateTextureRenderer(); }
g3dSceneRenderer* shdwSceneRendererCreate::CreateDepthMapRenderer()		{ return new g3dDepthMapRendererVk(); }
//	g3dRSMRendererDX11 (Plan 5b)
g3dSceneRenderer* shdwSceneRendererCreate::CreateRSMRenderer()			{ return m_Fallback.CreateRSMRenderer(); }
//	shdwOpacityMapRendererDX11 (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateOpacityMapRenderer(g3dProjectedLight* pProjLight)	{ return m_Fallback.CreateOpacityMapRenderer(pProjLight); }
//	shdwCubeMapRendererDX11 (Plan 5a, Task 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame)
{
	return m_Fallback.CreateCubeMapRenderer(i_Obj, i_Material, i_RenderPerFrame);
}
//	shdwPlanarReflectionRendererDX11 (Plan 5a, Task 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material)
{
	return m_Fallback.CreatePlanarReflectionRenderer(i_Obj, i_Material);
}
//	shdwAOPreviewRendererDX11 (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateAmbientOcclusionRenderer()	{ return m_Fallback.CreateAmbientOcclusionRenderer(); }
//	shdwGIPreviewRendererDX11 (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateGlobalIlluminationRenderer()	{ return m_Fallback.CreateGlobalIlluminationRenderer(); }
//	shdwDepthRendererDX11 (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateDepthRenderer()		{ return m_Fallback.CreateDepthRenderer(); }
//	shdwUVRendererDX11 (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateUVRenderer()			{ return m_Fallback.CreateUVRenderer(); }
//	g3dPickBufferRendererDX11 (Plan 6; the stub picks nothing)
g3dPickRenderer* shdwSceneRendererCreate::CreatePickRenderer()			{ return m_Fallback.CreatePickRenderer(); }
//	shdwShadowsOnlyRendererDX11(true) (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateShadowMaskRenderer()	{ return m_Fallback.CreateShadowMaskRenderer(); }
//	shdwShadowsOnlyRendererDX11(false) (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateIlluminationRenderer()	{ return m_Fallback.CreateIlluminationRenderer(); }
//	shdwNormalRendererDX11 (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateNormalRenderer()		{ return m_Fallback.CreateNormalRenderer(); }
//	shdwVelocityMapRendererDX11 (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateVelocityMapRenderer()	{ return m_Fallback.CreateVelocityMapRenderer(); }
//	shdwMaterialsRendererDX11 (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateMaterialsRenderer()	{ return m_Fallback.CreateMaterialsRenderer(); }
//	shdwReflectionsOnlyRendererDX11 (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateReflectionOnlyRenderer()	{ return m_Fallback.CreateReflectionOnlyRenderer(); }
//	shdwGlowRendererDX11 (Plan 6)
g3dSceneRenderer* shdwSceneRendererCreate::CreateGlowRenderer()			{ return m_Fallback.CreateGlowRenderer(); }

}
