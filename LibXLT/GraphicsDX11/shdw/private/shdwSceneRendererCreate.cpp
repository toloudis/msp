/*****************************************************************************
**  shdwSceneRendererCreate.cpp
**
**	shdwSceneRendererCreate creates scene renderer objects that use the
**	stencil shadow technique.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/shdw/shdwSceneRendererCreate.hpp"

#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "GraphicsDX11/shdw/shdwAOPreviewRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwCubeMapRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwDepthRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwGlowRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwHDRRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwNormalRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwPlanarReflectionRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwShadowLayerRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwShadowsOnlyRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwRayTraceRenderer.hpp"
#include "GraphicsDX11/shdw/shdwSinglePassRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwTestRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwUVRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwVelocityMapRendererDX11.hpp"

#include "GraphicsDX11/G3d/g3dTextureRendererDX11.hpp"
#include "GraphicsDX11/g3d/g3dDepthMapRendererDX11.hpp"
#include "GraphicsDX11/g3d/g3dPickBufferRendererDX11.hpp"
#include "GraphicsDX11/g3d/g3dRSMRendererDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneRendererDX11.hpp"
#include "GraphicsDX11/g3d/g3dVSMRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwMaterialsRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwGIPreviewRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwReflectionsOnlyRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwOpacityMapRendererDX11.hpp"

namespace
{
}

shdwSceneRendererCreate::shdwSceneRendererCreate()
{
}
shdwSceneRendererCreate::~shdwSceneRendererCreate()
{
}

//--------------------------------------------------------------------
//	Creates renderer that implements the best of the graphics 
//	features, this should be used by applications for their
//  main window rendering.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateDefaultRenderer( )
{
	return new shdwHDRRendererDX11();
//	return new shdwNormalRendererDX11();
//	return new shdwRayTraceRenderer();
//	return new shdwSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that implements HDR lighting features - tone 
//  mapping and bloom 
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateHDRRenderer( )
{
	return new shdwHDRRendererDX11();
//	return new shdwNormalRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer with the simplest of implementations, useful
//  for depth map passes and simple renderings that emphasize speed
//	over features.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateSimpleRenderer()
{
	return new shdwShadowLayerRendererDX11();
}


//--------------------------------------------------------------------
//	Creates renderer with the simplest of implementations for 
//	rendering texture
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateTextureRenderer( )
{
	return new g3dTextureRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer which does not do lighting or color, just
//	for getting the depth map info.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateDepthMapRenderer( )
{
#if defined(USE_VSM_SHADOWS)
	return new g3dVSMRendererDX11();
#else
	return new g3dDepthMapRendererDX11();
#endif
}

//--------------------------------------------------------------------
//	Creates renderer which does rsm rendering for GI
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateRSMRenderer( )
{
	return new g3dRSMRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that stores opacity.
//--------------------------------------------------------------------
//virtual
g3dSceneRenderer* shdwSceneRendererCreate::CreateOpacityMapRenderer( g3dProjectedLight* pProjLight )
{
	return new shdwOpacityMapRendererDX11( pProjLight );
}

//--------------------------------------------------------------------
//	Creates renderer that draws a full 360 view of the scene onto a 
//  cube map texture.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame )
{
	return new shdwCubeMapRendererDX11(i_Obj, i_Material, i_RenderPerFrame);
}

//--------------------------------------------------------------------
//	Creates renderer that draws the scene reflected about a plane. 
//	The plane is determined automatically from a specified mesh in the scene.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material)
{
	return new shdwPlanarReflectionRendererDX11(i_Obj, i_Material);
}

//--------------------------------------------------------------------
//	Creates renderer that draws the scene using ambient occlusion
//	textures only. No lighting. Fragments without AO will be white.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateAmbientOcclusionRenderer( )
{
	return new shdwAOPreviewRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer which does not do lighting or color, just
//	draws depths as grayscale.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateDepthRenderer( )
{
	return new shdwDepthRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer which draws UV coordinates in a 2d view.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateUVRenderer( )
{
	return new shdwUVRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for the purpose of picking the object at a pixel
//--------------------------------------------------------------------
//virtual 
g3dPickRenderer* shdwSceneRendererCreate::CreatePickRenderer( )
{
	return new g3dPickBufferRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for drawing shadows only
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateShadowMaskRenderer( )
{
	return new shdwShadowsOnlyRendererDX11(true);
}

//--------------------------------------------------------------------
//	Creates renderer for drawing lighting only
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateIlluminationRenderer( )
{
	return new shdwShadowsOnlyRendererDX11(false);
}

//--------------------------------------------------------------------
//	Creates renderer for viewing the normals
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateNormalRenderer( )
{
	return new shdwNormalRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for drawing velocity maps
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateVelocityMapRenderer( )
{
	return new shdwVelocityMapRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for ray tracing
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateMaterialsRenderer( )
{
	return new shdwMaterialsRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that draws the scene using global illumination
//	textures only. No lighting. Fragments without GI will be black.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateGlobalIlluminationRenderer( )
{
	return new shdwGIPreviewRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for reflection mapping only
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateReflectionOnlyRenderer()
{
	return new shdwReflectionsOnlyRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for glow pass only
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* shdwSceneRendererCreate::CreateGlowRenderer()
{
	return new shdwGlowRendererDX11();
}

