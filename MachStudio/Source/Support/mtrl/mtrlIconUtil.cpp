/*****************************************************************************
**  mtrlIconUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/mtrlIconUtil.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/mode/modeModeTime.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#include "Systems/Cameras/Undo/cmraActualOperations.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Graphics/ent/entModelInstance.hpp"
#include "Graphics/ent/entModelTemplate.hpp"
#include "Graphics/g2d/g2dImageCreate.hpp"
#include "Graphics/g2d/g2dImageSave.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/g2d/g2dScreenUtil.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/GraphicsLayer.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Graphics/mtr/mtrMaterialSaver.hpp"
#include "Graphics/mtr/mtrThumbnailData.hpp"
#include "Graphics/Sc/scObject.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dProjectedLightWrapper.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"


#define _CREATE_ICON

//============================================================================
//============================================================================
namespace mtrlIconUtil
{
	namespace
	{
		int l_IconSize = 128;
		const int c_DepthMapResolution = 512; //2048;

		g3dSceneRenderer* l_Renderer;
		g2dRenderTarget*l_Window;
		g3dTargetRenderer *l_Viewer;
		g3dScene *l_Scene;
		g3dSceneNode *l_Root;
		g3dPointLight* l_PointLight;
		g3dProjectedLight* l_ProjectedLight1;
		api3dProjectedLightWrapper* l_ProjectedLightWrapper1;
		g3dProjectedLight* l_ProjectedLight2;
		api3dProjectedLightWrapper* l_ProjectedLightWrapper2;
		matTexture* l_LightTexture;
		g3dRenderState* l_StateRoot;
		g3dAmbientEnvState* l_AmbientEnvironment;
		g3dSceneNode* l_Sphere;
		g3dFragment* l_Fragment;
		//matMaterial* l_Material;
		camCamera l_Camera;

		fsLocator l_ObjectFile;

		entModelTemplate *l_pModelTemplate = NULL;
		scObject *l_pSphereObject = NULL;
		entModelInstance *l_pModelInstance = NULL;

		//envType::UInt8* l_Buffer;
		//int l_BufSize;
		//mtrlScriptObject* l_mtrlObject = NULL;
		//int l_mtrlIndex;

		//--------------------------------------------------------------------
		// get_texture_path - returns the path of the gxb file.
		//--------------------------------------------------------------------
		fsLocator get_texture_path(fsLocator i_ObjectFile)
		{
			fsLocator texture_loc = i_ObjectFile;
			texture_loc.Pop();
			return texture_loc;
		}

		//--------------------------------------------------------------------
		// setupScene - initiates the necessary components for our own scene
		//--------------------------------------------------------------------
		void setupScene()
		{
			g2dPFD pfd(g2dPFD::e_Color, 32);
			// This window is owned by the system
			matTexture* tempTexture = matTextureMgr::CreateRenderTargetTexture(l_IconSize, l_IconSize, false, &pfd); 
			l_Window = tempTexture->GetRenderTargetAPI();

			// set up renderer and viewer for this window
			//l_Renderer = g3dSceneRendererCreate::CreateSimpleRenderer();
			l_Renderer = g3dSceneRendererCreate::CreateDefaultRenderer();
			l_Viewer = new g3dTargetRenderer(l_Window, l_Renderer);
			l_Viewer->SetBackgroundColor( g2dRGBColor(0x00, 0x00, 0x00) );

			l_Root = new g3dSceneNode();
			l_Scene = new g3dScene(new g3dLayer(l_Root, g3dLayer::e_ZBuffer, g3dLayer::e_World, g3dLayer::e_Multiplicative, true, true)); // scene owns layers

			l_Viewer->SetScene(l_Scene);

			//maPoint3d camPos(0.606f,1.148f,0.807f);
			//maPoint3d camTarg(0.039f,0.752f,-0.020f);
			//maPoint3d camUp(0,1,0);
			//l_Camera.LookAt(100.0f * camPos, 100.0f*camTarg, camUp);
			maPoint3d camPos(58.311f, 115.497f, 84.529f);
			maPoint3d camTarg(1.524f, 75.954f, 1.806f);
			maPoint3d camUp(0,1,0);
			l_Camera.LookAt(camPos, camTarg, camUp);

			//set the camera's HDR settings
			camHDRData camHDR;
			camHDR.m_MiddleGray = 1.0f;
			camHDR.m_WhiteCutoff = 1.0f;
			camHDR.m_SceneLuminance = 1.0f;
			camHDR.m_StarType = 0;
			camHDR.m_BloomScale = 1.0f;
			camHDR.m_StarScale = 0.50f;
			camHDR.m_BrightPassThresh = 5.0f;
			camHDR.m_BrightPassOffset = 10.0f;
			l_Camera.SetHDRParams(camHDR);

			l_Camera.SetAspect(l_IconSize, l_IconSize);
			l_Camera.SetClip(0.3f, 100.0f);
			//l_Camera.SetFOV(65);
			l_Camera.SetFOV(55);
			
			l_Camera.Set();
			l_Viewer->SetCamera(&l_Camera);

			//try to load shaderball gxb file
			l_ObjectFile = gfPaths::GetPath(gfPaths::e_ExePath);
			l_ObjectFile.Push("Data");
			l_ObjectFile.Push("assets");
			l_ObjectFile.Push("ShaderBall.gxb");
		}

		//--------------------------------------------------------------------
		//	createTexturedSphere makes a sphere-like object which has texture
		//	coordinates.
		//  Like g3dPrimitiveFragmentUtil's function except the texture
		//  coords are altered so that you can see a 2D texture oriented
		//	correctly when looking at the sphere along the Z-axis.
		//--------------------------------------------------------------------
		g3dFragment*  createTexturedSphere(	float i_Radius,
											int i_LatDiv,
											int i_LongDiv,
											matMaterial* i_Material)
		{
			DBG_ASSERT(i_Radius > 0, "Sphere radius must be greater than 0");
			DBG_ASSERT(i_LatDiv > 1, "Sphere must have at least two lateral divisions");
			DBG_ASSERT(i_LongDiv > 2, "Sphere must have at least three longitudinal divisions");

			typedef std::vector<maPoint3d> Row;

			//	make the latitude rows
			//
			const float lat_angle_delta = maConstants::c_fPI / float(i_LatDiv);
			const float long_angle_delta = maConstants::c_fPI_Times_2 / float(i_LongDiv);
			const int num_rows = i_LatDiv - 1;
			const int num_columns = (i_LongDiv+1);
			int row_num;

			std::vector<maPoint3d> row_list;
			std::vector<maPoint3d> normals;
			std::vector<maPoint2d> texture_coords;
			int vertex_row_base = 0;

			for( row_num = 0 ; row_num < num_rows ; row_num++ )
			{
				float theta = lat_angle_delta * float(row_num + 1);
				float sin_theta = sin(theta);
				float cos_theta = cos(theta);

				int column_num;

				for( column_num = 0 ; column_num < num_columns ; column_num++ )
				{
					float phi = long_angle_delta * float(column_num);
					float sin_phi = sin(phi);
					float cos_phi = cos(phi);
					float x = cos_phi * sin_theta;
					float z = sin_phi * sin_theta;
					float y = cos_theta;
					maPoint3d cur(x, y, z);
					row_list.push_back(cur * i_Radius);
					normals.push_back(cur);
					//texture_coords.push_back(maPoint2d(theta / maConstants::c_fPI, phi / maConstants::c_fPI_Times_2));
					texture_coords.push_back(maPoint2d(2 - (phi / maConstants::c_fPI), theta / maConstants::c_fPI ));
				}
			}

			//	The top point's vertex number is (num_rows * num_columns)
			//	The bottom point's vertex number is (num_rows * num_columns) + 1
			int top_index = num_rows * num_columns;
			int bot_index = top_index + 1;
			DBG_ASSERT(top_index == row_list.size(), "Miscount");
			row_list.push_back(maPoint3d(0, i_Radius, 0));
			row_list.push_back(maPoint3d(0, -i_Radius, 0));
			normals.push_back(maPoint3d(0, 1.0f, 0));
			normals.push_back(maPoint3d(0, -1.0f, 0));
			texture_coords.push_back(maPoint2d(0, 0));
			//texture_coords.push_back(maPoint2d(1, 0));
			texture_coords.push_back(maPoint2d(0, 1));

			std::vector<unsigned short> indices;

			int num_strips = num_rows - 1;

			// make indices for the center strips (if necessary)
			for( row_num = 0 ; row_num < num_strips ; ++row_num )
			{
				int column_num;

				int row_index_base = row_num * i_LongDiv * 2 * 3;
				int cur_index_base = row_index_base;
				for( column_num = 0 ; column_num < (num_columns-1) ; column_num++ )
				{
					int next = (column_num + 1) % num_columns;
					int v0 = row_num * num_columns + column_num;
					int v1 = (row_num+1) * num_columns + column_num;
					int v2 = (row_num+1) * num_columns + next;
					int v3 = row_num * num_columns + next;
					indices.push_back(v0);
					indices.push_back(v3);
					indices.push_back(v2);
					indices.push_back(v2);
					indices.push_back(v1);
					indices.push_back(v0);
				}
			}

			// make the top cap
			int column_num;

			for( column_num = 0 ; column_num < num_columns-1 ; column_num++ )
			{
				indices.push_back( top_index );
				indices.push_back( (column_num + 1) % num_columns);
				indices.push_back( column_num );
			}

			// make the bottom cap
			int bot_row_base = (num_rows-1) * num_columns;
			for( column_num = 0 ; column_num < num_columns-1 ; column_num++ )
			{
				indices.push_back( bot_index );
				indices.push_back( bot_row_base + column_num );
				indices.push_back( bot_row_base + ((column_num+1) % num_columns) );
			}

			g3dFragment* ret_val = g3dFragmentCreate::CreateFragment(	&(row_list[0]),
															&(normals[0]),
															&(texture_coords[0]),
															row_list.size(),
															&(indices[0]),
															indices.size(),
															i_Material );
			return ret_val;
		}
		
		//--------------------------------------------------------------------
		// loadGXBSphere-- load a sphere from a .GXB file
		//--------------------------------------------------------------------
		void loadGXBSphere( const mdlMaterialInfo& i_MatInfo )
		{
			if(!l_pModelTemplate)
				l_pModelTemplate = entImport::LoadGeometry(l_ObjectFile);
				
			if (l_pModelTemplate)
			{
                // where is the texture? same directory?
                fsResourceFinderDir finder(get_texture_path(l_ObjectFile));

                std::vector< shared_ptr<mdlMaterialInfo> > material_overrides;
                shared_ptr<mdlMaterialInfo> mat_copy( new mdlMaterialInfo( i_MatInfo ) );

				//override the material data of the gxb
				mat_copy->SetMaterialName("Shader");   
                material_overrides.push_back( mat_copy );

                l_pModelInstance  = new entModelInstance();
                l_pSphereObject = entImport::CreateObject( *l_pModelTemplate,
															*l_pModelInstance, 
															finder, material_overrides );

		        // add object to the local scene
                l_Root->AddChild( l_pSphereObject->GetBase() );
			}
		}

		//--------------------------------------------------------------------
		// loadMtrlSphere-- load a sphere to the scene and apply the specified material 
		//--------------------------------------------------------------------
		//void loadMtrlSphere()
		//{
		//	//create the sphere and retrieve the pointer to the material
		//	l_Sphere = new g3dSceneNode();
		//	//l_Fragment = g3dPrimitiveFragmentUtil::CreateTexturedSphere(5.0f,100,100);
		//	l_Material = l_mtrlObject->GetMaterial(l_mtrlIndex);
		//	l_Fragment = createTexturedSphere(5.0f,100,100,l_Material);
		//	l_Fragment->SetReceivesShadow(true);
		//	l_Fragment->SetCastsShadow(true);

		//	l_Sphere->SetFragment(l_Fragment);
		//	l_Sphere->SetMaterial(l_Material);
		//	l_Sphere->SetCastsShadow(true);
		//	
		//	l_Root->AddChild(l_Sphere);
		//}
	
		//--------------------------------------------------------------------
		// createLight -- Create a Light so the sphere is lit
		//--------------------------------------------------------------------
		void createLight()
		{
			l_PointLight = g3dLightMgr::CreatePointLight();
			l_PointLight->SetPosition(100.0f * maPoint3d(0.603f, 0.156f, 0.176f));
			l_PointLight->SetIntensity(maFloatRGBA(32.0f/255.0f, 28.0f/255.0f, 26.0f/255.0f, 1.0f));
			l_PointLight->SetDiffuseEnabled(true);
			l_PointLight->SetSpecularEnabled(true);
			l_PointLight->SetAffectsGlow(true);
			l_PointLight->SetCastsShadow(true);
			l_PointLight->SetFalloff0(1.00f);
			l_PointLight->SetFalloff1(1.00f);
			l_PointLight->SetFalloff2(0.00f);
			l_PointLight->SetFalloff3(0.00f);
			l_PointLight->SetFalloffStart(0.00f);
			l_PointLight->SetRange(100 * 0.01f);
			l_PointLight->SetIntensityFactor(102.3f);
			l_PointLight->Enable();

			//create the projection texture for both projection lights
			fsResourceFinderDir finder(get_texture_path(l_ObjectFile));
			itString tex_name("Shader_Ball_projection.dds");
			l_LightTexture = matTextureMgr::LoadTexture(finder, tex_name);

			l_ProjectedLight1 = g3dLightMgr::CreateProjectedLight();
			l_ProjectedLight1->SetPosition(100.0f * maPoint3d(-0.199f, 0.482f, 1.010f));
			l_ProjectedLight1->SetTarget(100.0f * maPoint3d(-0.086f, 1.224f, -0.286f));
			l_ProjectedLight1->SetIntensity(maFloatRGBA(207.0f/255.0f, 222.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f));
			l_ProjectedLight1->SetIntensityFactor(1.0f);
			l_ProjectedLight1->SetAngle(45.0f);
			l_ProjectedLight1->SetScale(100 * 2.0f);
			l_ProjectedLight1->SetRange(100 * 2.0f);
			l_ProjectedLight1->SetAspect(1.0f);
			l_ProjectedLight1->SetDiffuseEnabled(true);
			l_ProjectedLight1->SetSpecularEnabled(true);
			l_ProjectedLight1->SetAffectsGlow(true);
			l_ProjectedLight1->SetCastsShadow(true);
			l_ProjectedLight1->SetDepthBias(0.005f);
			l_ProjectedLight1->SetShadowQuality(g3dProjectedLight::SQ_HIGH);
			l_ProjectedLight1->SetShadowIntensity(1.0f);
			l_ProjectedLight1->SetShadowColor(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
			l_ProjectedLight1->SetLightSize(0.001f);
			l_ProjectedLight1->SetTexture(l_LightTexture);
			l_ProjectedLight1->Enable();
			// Wrapper handles the shadow map for the projected light
			l_ProjectedLightWrapper1 = new api3dProjectedLightWrapper(*l_ProjectedLight1, c_DepthMapResolution, l_Scene);

			l_ProjectedLight2 = g3dLightMgr::CreateProjectedLight();
			l_ProjectedLight2->SetPosition(100.0f * maPoint3d(0.669f, 1.278f, 0.089f));
			l_ProjectedLight2->SetTarget(100.0f * maPoint3d(-0.086f, 0.511f, -0.374f));
			l_ProjectedLight2->SetIntensity(maFloatRGBA(255.0f/255.0f, 242.0f/255.0f, 220.0f/255.0f, 255.0f/255.0f));
			l_ProjectedLight2->SetIntensityFactor(1.2f);
			l_ProjectedLight2->SetAngle(34.01f);
			l_ProjectedLight2->SetScale(100 * 2.0f);
			l_ProjectedLight2->SetRange(100 * 2.0f);
			l_ProjectedLight2->SetAspect(1.0f);
			l_ProjectedLight2->SetDiffuseEnabled(true);
			l_ProjectedLight2->SetSpecularEnabled(true);
			l_ProjectedLight2->SetAffectsGlow(true);
			l_ProjectedLight2->SetCastsShadow(true);
			l_ProjectedLight2->SetDepthBias(0.005f);
			l_ProjectedLight2->SetShadowQuality(g3dProjectedLight::SQ_HIGH);
			l_ProjectedLight2->SetShadowIntensity(1.0f);
			l_ProjectedLight2->SetShadowColor(maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f));
			l_ProjectedLight2->SetLightSize(0.001f);
			l_ProjectedLight2->SetTexture(l_LightTexture);
			l_ProjectedLight2->Enable();
			// Wrapper handles the shadow map for the projected light
			l_ProjectedLightWrapper2 = new api3dProjectedLightWrapper(*l_ProjectedLight2, c_DepthMapResolution, l_Scene);

			// Organize light into render state
			l_StateRoot = new g3dRenderState();
			l_StateRoot->m_Lights.push_back(l_PointLight);
			l_StateRoot->m_Lights.push_back(l_ProjectedLight1);
			l_StateRoot->m_Lights.push_back(l_ProjectedLight2);

			l_Root->SetRenderState(l_StateRoot);

			l_AmbientEnvironment = new g3dAmbientEnvState();
			l_AmbientEnvironment->m_DiffuseColor = maFloatRGBA(0.5f,0.5f,0.5f,1.0f);
			l_AmbientEnvironment->m_DiffuseFactor = 0.2f;

			l_Root->SetEnvironment(l_AmbientEnvironment);
		}

		//--------------------------------------------------------------------
		// returnMtrlFocus -- After creating a camera, we need to re-focus back 
		//                    to the currently selected material
		//--------------------------------------------------------------------
		//void returnMtrlFocus()
		//{
		//	mtrlOperations::SetSelectedMaterialIndex(l_mtrlObject, l_mtrlIndex);
		//}

		//--------------------------------------------------------------------
		// renderImage - render the scene to the viewer
		//--------------------------------------------------------------------
		void renderImage()
		{	
			// disregard current render prefs and use custom render prefs.
			g3dPrefs::g3dRenderPrefs* oldPrefs = &g3dPrefs::CurrentPrefs();
			g3dPrefs::g3dRenderPrefs myPrefs;
			myPrefs.m_bHDRAA = true; // Need anti-aliasing in such a small icon
			
			bool bShadowsOn = g3dSingleLightRendering::GetDoSingleLightRendering();
			g3dSingleLightRendering::SetDoSingleLightRendering(true);

			g3dPrefs::SetPrefs(&myPrefs);
			
			l_Scene->UpdateWorldData();

			// Render the depth maps first
			l_ProjectedLightWrapper1->OrientCamera();
			l_ProjectedLightWrapper1->RenderDepthMap(1.0f);
			l_ProjectedLightWrapper2->OrientCamera();
			l_ProjectedLightWrapper2->RenderDepthMap(1.0f);

			// Render the thumbnail's scene
			l_Viewer->Render(1.0f);

			// restore old shadows flag
			g3dSingleLightRendering::SetDoSingleLightRendering(bShadowsOn);

			// restore previous prefs.
			g3dPrefs::SetPrefs(oldPrefs);
		}
		
		//--------------------------------------------------------------------
		// getPixelData -- retrieves the pixel data associated to an image data type
		//--------------------------------------------------------------------
		void getImageData(mtrThumbnailData& o_ThumbnailData)
		{
			g2dImage* image = NULL;
			g2dScreenCaptureUtil::CaptureRenderTargetToImage(*l_Window, image);

			if (image != NULL)
			{
				int w=0,h=0,bpp=0;
				envType::UInt8* buffer = NULL;
				int bufSize = 0;

				image->GetPixels(&buffer, &bufSize, &w, &h, &bpp);
				//g2dImageSave::Save(fsLocator(itString("C:\\Projects\\imageTest.bmp")), image);

				// ownership passes to thumbnail
				o_ThumbnailData.SetThumbData(buffer, bufSize, w);
				DBG_ASSERT(l_IconSize == w, "Icon generated at different size than expected.");

				delete image;
			}
		}
	}

	//--------------------------------------------------------------------
	// cleanUp -- cleanup all objects that aren't connected to the main app
	//--------------------------------------------------------------------
	void cleanUp()
	{		
#ifdef _CREATE_ICON	
		delete l_pModelInstance;
		l_pModelInstance = NULL;

		//cleanup buffer now handled by mtrThumbnailData class
		//delete[] l_Buffer;

		// cleanup lights
		if (l_PointLight)
			g3dLightMgr::DestroyLight(l_PointLight);
		l_PointLight = NULL;
		if (l_ProjectedLight1)
			g3dLightMgr::DestroyLight(l_ProjectedLight1);
		l_ProjectedLight1 = NULL;
		if (l_ProjectedLight2)
			g3dLightMgr::DestroyLight(l_ProjectedLight2);
		l_ProjectedLight2 = NULL;

		// delete projected light wrappers and texture
		delete l_ProjectedLightWrapper1;
		l_ProjectedLightWrapper1 = NULL;
		delete l_ProjectedLightWrapper2;
		l_ProjectedLightWrapper2 = NULL;
		if (l_LightTexture)
			matTextureMgr::ReleaseTexture(l_LightTexture);
		l_LightTexture = NULL;

		// destroy render states
		delete l_StateRoot;
		l_StateRoot = NULL;
		delete l_AmbientEnvironment;
		l_AmbientEnvironment = NULL;
		
		// cleanup fragments
		delete l_Fragment;
		l_Fragment = NULL;

		// cleanup window and viewer
		delete l_Viewer;
		l_Viewer = NULL;
		delete l_Renderer;
		l_Renderer = NULL;
		delete l_Window;
		l_Window = NULL;

		if (l_pSphereObject && l_Root)
			l_Root->RemoveChild( l_pSphereObject->GetBase() );
		delete l_pSphereObject;
		l_pSphereObject = NULL;

		// cleanup main scene component
		delete l_Scene;
		l_Scene = NULL;
		delete l_Root;  
		l_Root = NULL;
#endif
	}

	//--------------------------------------------------------------------
	// cleanUpTemplate -- cleanup the model template used to load the gxb sphere
	//--------------------------------------------------------------------
	void cleanUpTemplate()
	{		
#ifdef _CREATE_ICON	
		if(l_pModelTemplate)
			delete l_pModelTemplate;	
#endif
	}

	//--------------------------------------------------------------------
	// createMtrlIcon -- Creates a material Icon from a specified material (mtl) file
	//--------------------------------------------------------------------
	void createMtrlIcon(const mdlMaterialInfo& i_MatInfo, 
						mtrThumbnailData& o_ThumbnailData)
	{
		//l_mtrlObject = i_mtrlObject;
		//l_mtrlIndex = i_mtrlIndex;

#ifdef _CREATE_ICON	
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		setupScene();
		if (fsFileUtil::FileExists(l_ObjectFile))
		{
			loadGXBSphere(i_MatInfo);
			createLight();
			renderImage();
			getImageData(o_ThumbnailData);
			//returnMtrlFocus();
		}
		cleanUp();
#endif
	}

};
