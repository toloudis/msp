/****************************************************************************\
**	g3dSceneRendererDX11Create.hpp
**
**	g3dSceneRendererDX11Create creates scene renderer objects
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_SCENERENDERERDX11CREATE_HPP
#error g3dSceneRendererDX11Create.hpp multiply included
#endif
#define G3D_SCENERENDERERDX11CREATE_HPP

#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#endif

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class g3dProjectedLight;

class g3dSceneRendererDX11Create : public g3dSceneRendererCreateImpl
{
public:
	
		//--------------------------------------------------------------------
		//	Creates renderer that implements the best of the graphics 
		//	features, this should be used by applications for their
		//  main window rendering.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateDefaultRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer that implements HDR lighting features - tone 
		//  mapping and bloom 
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateHDRRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer with the simplest of implementations, useful
		//  for simple renderings that emphasize speed
		//	over features.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateSimpleRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer with the simplest of implementations for 
		//	rendering texture
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateTextureRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer which does not do lighting or color, just
		//	for getting the depth map info.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateDepthMapRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer which do rsm rendering for GI
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateRSMRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer that stores opacity.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateOpacityMapRenderer( g3dProjectedLight* pProjLight );

		//--------------------------------------------------------------------
		//	Creates renderer that draws a full 360 view of the scene onto a 
		//  cube map texture.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame);

		//--------------------------------------------------------------------
		//	Creates renderer that draws the scene reflected about a plane. 
		//	The plane is determined automatically from a specified mesh in the scene.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material);

		//--------------------------------------------------------------------
		//	Creates renderer that draws the scene using ambient occlusion
		//	textures only. No lighting. Fragments without AO will be white.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateAmbientOcclusionRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer that draws the scene using global illumination
		//	textures only. No lighting. Fragments without GI will be black.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateGlobalIlluminationRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer which does not do lighting or color, just
		//	draws depths as grayscale.
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateDepthRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer which views the normals
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateNormalRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer for the purpose of picking the object at a pixel
		//--------------------------------------------------------------------
		virtual g3dPickRenderer* CreatePickRenderer( );

		//--------------------------------------------------------------------
		//	Creates renderer for reflection mapping only
		//--------------------------------------------------------------------
		virtual g3dSceneRenderer* CreateReflectionOnlyRenderer();
};