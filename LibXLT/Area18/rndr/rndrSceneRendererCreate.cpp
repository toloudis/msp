/*****************************************************************************
**  rndrSceneRendererCreate.cpp
**
**	rndrSceneRendererCreate creates scene renderer objects that use the
**	stencil shadow technique.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Area18/rndr/rndrSceneRendererCreate.hpp"

#include "Area18/Area18Layer.hpp"
#include "Area18/rndr/rndrFullRenderer.h"
//#include "Area18/rndr/rndrTiledRenderer.hpp"

#define NULL 0

namespace
{
}

rndrSceneRendererCreate::rndrSceneRendererCreate()
{
}
rndrSceneRendererCreate::~rndrSceneRendererCreate()
{
}

//--------------------------------------------------------------------
//	Creates renderer that implements the best of the graphics 
//	features, this should be used by applications for their
//  main window rendering.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateDefaultRenderer( )
{
	return new rndrFullRenderer;
	//return new rndrTiledRenderer(Area18Layer::DX11()->GetDevice(0));
	//return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that implements HDR lighting features - tone 
//  mapping and bloom 
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateHDRRenderer( )
{
	return new rndrFullRenderer;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer with the simplest of implementations, useful
//  for depth map passes and simple renderings that emphasize speed
//	over features.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateSimpleRenderer()
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}


//--------------------------------------------------------------------
//	Creates renderer with the simplest of implementations for 
//	rendering texture
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateTextureRenderer( )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer which does not do lighting or color, just
//	for getting the depth map info.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateDepthMapRenderer( )
{
	return NULL;
	//	return NULL;//new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer which does rsm rendering for GI
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateRSMRenderer( )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that stores opacity.
//--------------------------------------------------------------------
//virtual
g3dSceneRenderer* rndrSceneRendererCreate::CreateOpacityMapRenderer( g3dProjectedLight* pProjLight )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that draws a full 360 view of the scene onto a 
//  cube map texture.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that draws the scene reflected about a plane. 
//	The plane is determined automatically from a specified mesh in the scene.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material)
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that draws the scene using ambient occlusion
//	textures only. No lighting. Fragments without AO will be white.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateAmbientOcclusionRenderer( )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer which does not do lighting or color, just
//	draws depths as grayscale.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateDepthRenderer( )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer which draws UV coordinates in a 2d view.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateUVRenderer( )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for the purpose of picking the object at a pixel
//--------------------------------------------------------------------
//virtual 
g3dPickRenderer* rndrSceneRendererCreate::CreatePickRenderer( )
{
	return NULL;//new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for drawing shadows only
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateShadowMaskRenderer( )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for drawing lighting only
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateIlluminationRenderer( )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for viewing the normals
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateNormalRenderer( )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for drawing velocity maps
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateVelocityMapRenderer( )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for ray tracing
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateMaterialsRenderer( )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that draws the scene using global illumination
//	textures only. No lighting. Fragments without GI will be black.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateGlobalIlluminationRenderer( )
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for reflection mapping only
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateReflectionOnlyRenderer()
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for glow pass only
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* rndrSceneRendererCreate::CreateGlowRenderer()
{
	return NULL;
	//	return new rndrSinglePassRendererDX11();
}

