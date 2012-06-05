/****************************************************************************\
**	g3dSceneRendererCreate.hpp
**
**	g3dSceneRendererCreate is a factory for creating renderers for the current
**	graphics implementation. 
**
**	StudioGPU
**	Copyright(C) 2005. - All Rights Reserved
\****************************************************************************/
#ifdef G3D_SCENERENDERERCREATE_HPP
#error g3dSceneRendererCreate.hpp multiply included
#endif
#define G3D_SCENERENDERERCREATE_HPP

#ifndef G3D_SCENERENDERERTYPES_HPP
#include "Graphics/g3d/g3dSceneRendererTypes.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================
class g3dPickRenderer;
class g3dSceneRenderer;
class g3dSceneRendererCreateImpl;
class scObject;
class matMaterial;
class g3dProjectedLight;


//============================================================================
//============================================================================
class g3dSceneRendererCreate
{
public:
	//--------------------------------------------------------------------
	//	Creates renderer based on type id
	//--------------------------------------------------------------------
	static g3dSceneRenderer* CreateRenderer(g3dSceneRendererTypes::RendererType i_Type);

	//--------------------------------------------------------------------
	// Static functions for direct access:
	//   g3dSceneRendererCreate::CreateSimpleRenderer
	//   g3dSceneRendererCreate::CreateDefaultRenderer
	//   g3dSceneRendererCreate::CreateDepthMapRenderer
	//--------------------------------------------------------------------

		//--------------------------------------------------------------------
		//	Creates renderer that implements the best of the graphics 
		//	features, this should be used by applications for their
		//  main window rendering.
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateDefaultRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer that implements HDR lighting features - tone 
		//  mapping and bloom 
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateHDRRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer with the simplest of implementations, useful
		//  for simple renderings that emphasize speed
		//	over features.
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateSimpleRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer with the simplest of implementations for 
		//	rendering texture
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateTextureRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer which does not do lighting or color, just
		//	for getting the depth map info.
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateDepthMapRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer which do rsm rendering for GI
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateRSMRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer that stores opacity.
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateOpacityMapRenderer( g3dProjectedLight* pProjLight );

		//--------------------------------------------------------------------
		//	Creates renderer that draws a full 360 view of the scene onto a 
		//  cube map texture.
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateCubeMapRenderer( scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame );

		//--------------------------------------------------------------------
		//	Creates renderer that draws the scene reflected about a plane. 
		//	The plane is determined automatically from a specified mesh in the scene.
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreatePlanarReflectionRenderer( scObject* i_Obj, matMaterial* i_Material );

		//--------------------------------------------------------------------
		//	Creates renderer that draws the scene using ambient occlusion
		//	textures only. No lighting. Fragments without AO will be white.
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateAmbientOcclusionRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer that draws the scene using global illumination
		//	textures only. No lighting. Fragments without GI will be black.
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateGlobalIlluminationRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer which does not do lighting or color, just
		//	draws depths as grayscale.
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateDepthRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer which draws UV coordinates in a 2d view.
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateUVRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer for the purpose of picking the object at a pixel
		//--------------------------------------------------------------------
		static g3dPickRenderer* CreatePickRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer for drawing shadows only
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateShadowMaskRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer for drawing lighting only
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateIlluminationRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer which does not do lighting or color, just
		//	draws normals as scaled and biased rgb colors.
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateNormalRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer for drawing shadows only
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateVelocityMapRenderer();

		//--------------------------------------------------------------------
		//	Creates renderer for ray tracing
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateMaterialsRenderer();

		//--------------------------------------------------------------------
		//	Creates renderer for reflection mapping only
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateReflectionOnlyRenderer();

		//--------------------------------------------------------------------
		//	Creates renderer for glow pass only
		//--------------------------------------------------------------------
		static g3dSceneRenderer* CreateGlowRenderer();

	//--------------------------------------------------------------------
	// Methods for defining implementation
	//--------------------------------------------------------------------

		//--------------------------------------------------------------------
		// Set new implementation method, returns pointer to last one
		// that was being used.  Both can be NULL.
		// Ownership for the pointer remains with the caller.
		//--------------------------------------------------------------------
		static g3dSceneRendererCreateImpl* SetImplementation(g3dSceneRendererCreateImpl* i_pCreator);

private:
	static g3dSceneRendererCreateImpl* sm_pImplementation;
};


//============================================================================
//============================================================================
class g3dSceneRendererCreateImpl
{
public:

	//--------------------------------------------------------------------
	// Virtual functions to be overriden in implementation
	//--------------------------------------------------------------------
		
		//--------------------------------------------------------------------
		//	Creates renderer that implements the best of the graphics 
		//	features, this should be used by applications for their
		//  main window rendering.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateDefaultRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer that implements HDR lighting features - tone 
		//  mapping and bloom 
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateHDRRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer with the simplest of implementations, useful
		//  for simple renderings that emphasize speed
		//	over features.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateSimpleRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer with the simplest of implementations for 
		//	rendering texture
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateTextureRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer which does not do lighting or color, just
		//	for getting the depth map info.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateDepthMapRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer which do rsm rendering for GI
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateRSMRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer that stores opacity.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateOpacityMapRenderer( g3dProjectedLight* pProjLight ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer that draws a full 360 view of the scene onto a 
		//  cube map texture.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer that draws the scene reflected about a plane. 
		//	The plane is determined automatically from a specified mesh in the scene.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer that draws the scene using ambient occlusion
		//	textures only. No lighting. Fragments without AO will be white.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateAmbientOcclusionRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer that draws the scene using global illumination
		//	textures only. No lighting. Fragments without GI will be black.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateGlobalIlluminationRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer which does not do lighting or color, just
		//	draws depths as grayscale.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateDepthRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer which draws UV coordinates in a 2d view.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateUVRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer for the purpose of picking the object at a pixel
		//--------------------------------------------------------------------
		virtual g3dPickRenderer* CreatePickRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer for drawing shadows only
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateShadowMaskRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer for drawing lighting only
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateIlluminationRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer for viewing the normals
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateNormalRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer for drawing velocity maps
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateVelocityMapRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer for ray tracing
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateMaterialsRenderer( ) = 0;

		//--------------------------------------------------------------------
		//	Creates renderer for reflection mapping only
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateReflectionOnlyRenderer() = 0;

		//--------------------------------------------------------------------
		//	Creates renderer for glow pass only
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateGlowRenderer() = 0;
};
