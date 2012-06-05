/*****************************************************************************
**	g3dSceneRendererCreate.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
g3dSceneRendererCreateImpl* g3dSceneRendererCreate::sm_pImplementation = NULL;


//--------------------------------------------------------------------
//	Creates renderer based on type id
//--------------------------------------------------------------------
g3dSceneRenderer* g3dSceneRendererCreate::CreateRenderer(g3dSceneRendererTypes::RendererType i_Type)
{
	switch (i_Type)
	{
		case g3dSceneRendererTypes::e_Default:
			return CreateDefaultRenderer();
		case g3dSceneRendererTypes::e_HDR:
			return CreateHDRRenderer();
		case g3dSceneRendererTypes::e_CubeMap:
			return CreateCubeMapRenderer(NULL, NULL, false);
		case g3dSceneRendererTypes::e_PlanarReflection:
			return CreatePlanarReflectionRenderer(NULL, NULL);
		case g3dSceneRendererTypes::e_DepthMap:
			return CreateDepthMapRenderer();
		case g3dSceneRendererTypes::e_AmbientOcclusion:
			return CreateAmbientOcclusionRenderer();
		case g3dSceneRendererTypes::e_Depth:
			return CreateDepthRenderer();
		case g3dSceneRendererTypes::e_ShadowMask:
			return CreateShadowMaskRenderer();
		case g3dSceneRendererTypes::e_Normals:
			return CreateNormalRenderer();
		case g3dSceneRendererTypes::e_VelocityMap:
			return CreateVelocityMapRenderer();
		case g3dSceneRendererTypes::e_Materials:
			return CreateMaterialsRenderer();
		case g3dSceneRendererTypes::e_IlluminationOnly:
			return CreateIlluminationRenderer();
		case g3dSceneRendererTypes::e_ReflectionOnly:
			return CreateReflectionOnlyRenderer();
		case g3dSceneRendererTypes::e_GlobalIllumination:
			return CreateGlobalIlluminationRenderer();
		case g3dSceneRendererTypes::e_Glow:
			return CreateGlowRenderer();
		default:
			return CreateDefaultRenderer();
	}
}

//--------------------------------------------------------------------
//	Creates renderer that implements the best of the graphics 
//	features, this should be used by applications for their
//  main window rendering.
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreateDefaultRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateDefaultRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer that implements HDR lighting features - tone 
//  mapping and bloom 
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreateHDRRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateHDRRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer with the simplest of implementations, useful
//  for simple renderings that emphasize speed
//	over features.
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreateSimpleRenderer( )
{	
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateSimpleRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer with the simplest of implementations for 
//	rendering texture
//--------------------------------------------------------------------
g3dSceneRenderer* g3dSceneRendererCreate::CreateTextureRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateTextureRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer which does not do lighting or color, just
//	for getting the depth map info.
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreateDepthMapRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateDepthMapRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer which do rsm rendering for GI
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreateRSMRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateRSMRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer that stores opacity.
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreateOpacityMapRenderer( g3dProjectedLight* pProjLight )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateOpacityMapRenderer( pProjLight );
}

//--------------------------------------------------------------------
//	Creates renderer that draws a full 360 view of the scene onto a 
//  cube map texture.
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame)
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateCubeMapRenderer(i_Obj, i_Material, i_RenderPerFrame);
}

//--------------------------------------------------------------------
//	Creates renderer that draws the scene reflected about a plane. 
//	The plane is determined automatically from a specified mesh in the scene.
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material)
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreatePlanarReflectionRenderer(i_Obj, i_Material);
}

//--------------------------------------------------------------------
//	Creates renderer that draws the scene using ambient occlusion
//	textures only. No lighting. Fragments without AO will be white.
//--------------------------------------------------------------------
//static
g3dSceneRenderer* g3dSceneRendererCreate::CreateAmbientOcclusionRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateAmbientOcclusionRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer that draws the scene using global illumination
//	textures only. No lighting. Fragments without GI will be black.
//--------------------------------------------------------------------
//static
g3dSceneRenderer* g3dSceneRendererCreate::CreateGlobalIlluminationRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateGlobalIlluminationRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer which does not do lighting or color, just
//	draws depths as grayscale.
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreateDepthRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateDepthRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer which draws UV coordinates in a 2d view.
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreateUVRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateUVRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer for the purpose of picking the object at a pixel
//--------------------------------------------------------------------
//static 
g3dPickRenderer* g3dSceneRendererCreate::CreatePickRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreatePickRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer for drawing shadows only
//--------------------------------------------------------------------
//static
g3dSceneRenderer* g3dSceneRendererCreate::CreateShadowMaskRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateShadowMaskRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer for drawing lighting only
//--------------------------------------------------------------------
//static
g3dSceneRenderer* g3dSceneRendererCreate::CreateIlluminationRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateIlluminationRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer for viewing normals.
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreateNormalRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateNormalRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer for drawing velocity maps
//--------------------------------------------------------------------
//static
g3dSceneRenderer* g3dSceneRendererCreate::CreateVelocityMapRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateVelocityMapRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer for ray tracing
//--------------------------------------------------------------------
//static
g3dSceneRenderer* g3dSceneRendererCreate::CreateMaterialsRenderer( )
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateMaterialsRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer for reflection mapping only
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreateReflectionOnlyRenderer()
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateReflectionOnlyRenderer();
}

//--------------------------------------------------------------------
//	Creates renderer for glow pass only
//--------------------------------------------------------------------
//static 
g3dSceneRenderer* g3dSceneRendererCreate::CreateGlowRenderer()
{
	DBG_ASSERT(g3dSceneRendererCreate::sm_pImplementation, "g3dSceneRendererCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateGlowRenderer();
}

//--------------------------------------------------------------------
// Set new implementation method, returns pointer to last one
// that was being used.  Both can be NULL.
// Ownership for the pointer remains with the caller.
//--------------------------------------------------------------------
//static
g3dSceneRendererCreateImpl* g3dSceneRendererCreate::SetImplementation(g3dSceneRendererCreateImpl* i_Creator)
{
	g3dSceneRendererCreateImpl* old_impl = sm_pImplementation;
	sm_pImplementation = i_Creator;
	return old_impl;
}
