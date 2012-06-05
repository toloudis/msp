/*****************************************************************************
**  g3dSceneRendererDX11Create.cpp
**
**	g3dSceneRendererDX11Create creates scene renderer objects
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dSceneRendererDX11Create.hpp"

#include "GraphicsDX11/g3d/g3dAmbientOcclusionRendererDX11.hpp"
#include "GraphicsDX11/g3d/g3dCubeMapRendererDX11.hpp"
#include "GraphicsDX11/g3d/g3dDepthMapRendererDX11.hpp"
#include "GraphicsDX11/g3d/g3dPickBufferRendererDX11.hpp"
#include "GraphicsDX11/g3d/g3dRSMRendererDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneRendererDX11.hpp"
#include "GraphicsDX11/g3d/g3dTextureRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwAOPreviewRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwGIPreviewRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwReflectionsOnlyRendererDX11.hpp"
#include "GraphicsDX11/shdw/shdwOpacityMapRendererDX11.hpp"

namespace
{
}

//--------------------------------------------------------------------
//	Creates renderer that implements the best of the graphics 
//	features, this should be used by applications for their
//  main window rendering.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* g3dSceneRendererDX11Create::CreateDefaultRenderer( )
{
	return new g3dSceneRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that implements HDR lighting features - tone 
//  mapping and bloom 
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* CreateHDRRenderer( )
{
	return new g3dSceneRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer with the simplest of implementations, useful
//  for simple renderings that emphasize speed
//	over features.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* g3dSceneRendererDX11Create::CreateSimpleRenderer( )
{
	return new g3dSceneRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer with the simplest of implementations for 
//	rendering texture
//--------------------------------------------------------------------
g3dSceneRenderer* g3dSceneRendererDX11Create::CreateTextureRenderer( )
{
	return new g3dTextureRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer which does not do lighting or color, just
//	for getting the depth map info.
//--------------------------------------------------------------------
//virtual
g3dSceneRenderer* g3dSceneRendererDX11Create::CreateDepthMapRenderer( )
{
	return new g3dDepthMapRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer which do rsm rendering for GI
//--------------------------------------------------------------------
//virtual
g3dSceneRenderer* g3dSceneRendererDX11Create::CreateRSMRenderer( )
{
	return new g3dRSMRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that stores opacity.
//--------------------------------------------------------------------
g3dSceneRenderer* g3dSceneRendererDX11Create::CreateOpacityMapRenderer( g3dProjectedLight* pProjLight )
{
	return new shdwOpacityMapRendererDX11( pProjLight );
}

//--------------------------------------------------------------------
//	Creates renderer that draws a full 360 view of the scene onto a 
//  cube map texture.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* g3dSceneRendererDX11Create::CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame)
{
	return new g3dCubeMapRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that draws the scene reflected about a plane. 
//	The plane is determined automatically from a specified mesh in the scene.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* g3dSceneRendererDX11Create::CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material)
{
	// TODO: generic d3d planar reflection renderer
	return new g3dSceneRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that draws the scene using ambient occlusion
//	textures only. No lighting. Fragments without AO will be white.
//--------------------------------------------------------------------
//virtual 

g3dSceneRenderer* g3dSceneRendererDX11Create::CreateAmbientOcclusionRenderer( )
{
	return new shdwAOPreviewRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer that draws the scene using global illumination
//	textures only. No lighting. Fragments without GI will be black.
//--------------------------------------------------------------------
//virtual 

g3dSceneRenderer* g3dSceneRendererDX11Create::CreateGlobalIlluminationRenderer( )
{
	return new shdwGIPreviewRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer which does not do lighting or color, just
//	draws depths as grayscale.
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* g3dSceneRendererDX11Create::CreateDepthRenderer( )
{
	return new g3dSceneRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for the purpose of picking the object at a pixel
//--------------------------------------------------------------------
//virtual 
g3dPickRenderer* g3dSceneRendererDX11Create::CreatePickRenderer( )
{
	return new g3dPickBufferRendererDX11();
}

//--------------------------------------------------------------------
//	Creates renderer for reflection mapping only
//--------------------------------------------------------------------
//virtual 
g3dSceneRenderer* g3dSceneRendererDX11Create::CreateReflectionOnlyRenderer()
{
	return new shdwReflectionsOnlyRendererDX11();
}