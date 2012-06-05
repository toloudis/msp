/*****************************************************************************
**	brshPaintBrushMgr.hpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/brsh/brshPaintBrushMgr.hpp"

#include "Support/brsh/brshBrushStroke.hpp"
#include "Support/brsh/brshColorDialogUtil.hpp"
#include "Support/brsh/data/brshPropertyObject.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/g2d/g2dImageSave.hpp"
#include "Graphics/g2d/g2dRenderTarget.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/g2d/g2dScreenUtil.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPickInfo.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/Sc/scObject.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"

#include<string>
#include<sstream>


//============================================================================
//============================================================================
namespace brshPaintBrushMgr
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	namespace
	{
		bool l_Enabled = false;
		bool l_bDialogVisible = false;
		bool l_bCanvasDirty = false;
		int l_PaintMode = 0;
		float l_U = 0;
		float l_V = 0;
		int l_MatIndex = -1;

		brshBrushStroke*	l_pCurrentStroke = NULL;
		effParamTexture*	l_pCurrentParam = NULL;
		matTexture*			l_pTempRenderTarget = NULL;
		matTexture*			l_pRectangleTexture = NULL;
		matMaterial*		l_pTexMat = NULL;
		mtrlScriptObject*	l_Object = NULL;
		g3dScene*			l_pScene = NULL;
		g3dSceneNode*		l_pSceneNodeW = NULL;
		g3dSceneNode*		l_pSceneNodeS = NULL;
		int					l_nScreenSpaceIndex = -1;
		g3dSceneRenderer*	l_pRenderer = NULL;
		g3dTargetRenderer*	l_pPaintTargetRenderer = NULL;
		g2dRenderTarget*	l_pPaintRenderTarget = NULL;
		g3dFragment*		l_pTextureFrag = NULL;
		api3dObjectSimple*	l_pTextureObject = NULL;
		fsLocator			l_TextureLocator;
		brshPropertyObject*	l_pBrushObject = NULL;
		bool takeScreen = true;
		maFloatRGBA l_BrushColor;
		camCamera l_Camera;
		int num = 0;

		
		//--------------------------------------------------------------------
		//  Adjust the UV coordinates for screen space mapping
		//--------------------------------------------------------------------
		void ss_adjust( float &io_Coord )
		{
			//screen space is from -1 to 1
			//UV is 0 to 1
			float adjustment = 2 * io_Coord;
			io_Coord = -1 + adjustment;
		}

		//--------------------------------------------------------------------
		// Paint a desired rgb value onto the material
		//--------------------------------------------------------------------
		void paint_color()
		{
#ifdef ENABLE_PAINT
			if(l_pTempRenderTarget == NULL)
				return;

			gpxRenderControl::ConfirmSingleThread();

			l_pPaintRenderTarget = l_pTempRenderTarget->GetRenderTargetAPI();
			if(l_pPaintRenderTarget)
			{
				//set the scenerenderer and targetrenderer to the target renderer
				l_pPaintTargetRenderer->SetTarget(l_pPaintRenderTarget);
				l_pPaintTargetRenderer->SetRenderer(l_pRenderer);			
				l_pPaintTargetRenderer->SetScene(l_pScene);
				l_pPaintTargetRenderer->SetCamera(&l_Camera);

				//create the paint object and position it
				l_pTextureObject = l_pCurrentStroke->GetStrokeObject();
				l_pTextureObject->SetPosition(maPoint3d(l_U, -1*l_V, 0)); 
				
				scObject* tempObject = const_cast<scObject*>(l_pTextureObject->GetObject());
				l_pSceneNodeS->AddChild(tempObject->GetBase());

				//allow our render target to receive the next render calls
				l_pPaintRenderTarget->MakeCurrent();
				l_pScene->UpdateWorldData();
				l_pPaintTargetRenderer->Render(1.0f, false);

				//remove the object so we don't get a duplication assert on the next paint call
				l_pSceneNodeS->RemoveChild(tempObject->GetBase());
			}
#endif
		}

		//--------------------------------------------------------------------
		// paint the desired light intensity onto the material
		//--------------------------------------------------------------------
		void paint_light()
		{
			
		}
		
		
		//--------------------------------------------------------------------
		// save_image -- save the rendertarget to the desired file location
		//--------------------------------------------------------------------
		bool save_image(const fsLocator& i_SaveLocation)
		{
			bool success = false;
			if( l_pPaintRenderTarget == NULL )
				return success;

			g2dImage* image = NULL;
			g2dScreenCaptureUtil::CaptureRenderTargetToImage(*l_pPaintRenderTarget, image);

			if (image != NULL)
			{	
				g2dImageSave::Save(i_SaveLocation, image);
				success = true;
				delete image;
			}
			return success;
		}

		//--------------------------------------------------------------------
		// override the diffuse texture of the material with a render target texture
		//--------------------------------------------------------------------
		void set_render_target(matTexture* i_pOriginalTexture)
		{
			if(i_pOriginalTexture == NULL)
			{
				guiMessageBox::Show("Material must have a texture to paint", "No Texture Data");
				l_pCurrentParam = NULL;
				l_pCurrentStroke->SetStrokeRenderTarget(NULL);
				l_pBrushObject->m_Data.m_bEnabled.SetValue(false);
				return;
			}

			l_pCurrentStroke->SetStrokeRenderTarget(matTextureMgr::CreateRenderTargetTexture(i_pOriginalTexture));
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void clear_render_target()
		{
			if(l_pCurrentStroke)
			{
				l_pCurrentStroke->SetStrokeRenderTarget(NULL);
			}
		}

		//--------------------------------------------------------------------
		// adjust the size of the brush based on the pen pressure
		//--------------------------------------------------------------------
		void set_brush_size(int i_Pressure)
		{
			maVector3d brush_scale = maVector3d(1.0f, 1.0f, 1.0f) * l_pBrushObject->m_Data.m_BrushSize.GetValue();
			//Tablets have pressure sensitivity between 0 and 1024 (non-inclusive)
			if( i_Pressure >= 0 && i_Pressure <= 300 )
				l_pCurrentStroke->SetScale(brush_scale * 1.0f);
			else if( i_Pressure >= 301 && i_Pressure <= 600 )
				l_pCurrentStroke->SetScale(brush_scale * 1.2f);
			else if( i_Pressure >= 601 && i_Pressure <= 700 )
				l_pCurrentStroke->SetScale(brush_scale * 1.4f);
			else if( i_Pressure >= 701 && i_Pressure <= 800 )
				l_pCurrentStroke->SetScale(brush_scale * 1.5f);
			else if( i_Pressure >= 801 && i_Pressure <= 900 )
				l_pCurrentStroke->SetScale(brush_scale * 1.6f);
			else if( i_Pressure >= 901 && i_Pressure <= 1000 )
				l_pCurrentStroke->SetScale(brush_scale * 1.8f);
			else if( i_Pressure >= 1001 && i_Pressure <= 1024 )
				l_pCurrentStroke->SetScale(brush_scale * 2.0f);
			else
				l_pCurrentStroke->SetScale(brush_scale);
		}

		//--------------------------------------------------------------------
		// Set the color of the brush to be used on the next Paint call
		//--------------------------------------------------------------------
		void set_brush_color(const maFloatRGBA &i_Color)
		{
			maFloatRGBA color = i_Color;
			float opacity = l_pBrushObject->m_Data.m_Opacity.GetValue();
			float cur_alpha = color.GetAlpha();
			float new_alpha = (opacity/100.0f) * cur_alpha;
			color.SetAlpha(new_alpha);

			//this will change the color of the current brushstroke
			if(l_pCurrentStroke)				
				l_pCurrentStroke->SetColor(color);
		}

		//--------------------------------------------------------------------
		// adjust the size of the brush based on the pen pressure
		//--------------------------------------------------------------------
		void set_brush_texture(const fsLocator& i_Texture)
		{
			if(l_pCurrentStroke)
				l_pCurrentStroke->SetTexture(i_Texture);
		}

		//--------------------------------------------------------------------
		// set the canvas text box with a dirty bit
		//--------------------------------------------------------------------
		void set_canvas_dirty()
		{
			l_bCanvasDirty = true;
			std::string canvas = l_pBrushObject->m_Data.m_CurrentCanvas.GetValue();
			canvas += std::string("*");
			l_pBrushObject->m_Data.m_CurrentCanvas.SetValue(canvas);
		}

	} //end anonymous namespace

	//------------------------------------------------------------------------
	// Initialize the components of the paint renderer
	//------------------------------------------------------------------------
	void Initialize()
	{
#ifdef ENABLE_PAINT
		gpxRenderControl::ConfirmSingleThread();

		//add Property object
		l_pBrushObject = new brshPropertyObject();

		//create the various overlay scene pointers
		l_pPaintTargetRenderer = new g3dTargetRenderer();
		l_pRenderer = g3dSceneRendererCreate::CreateDefaultRenderer();
		l_pSceneNodeW = new g3dSceneNode();
		l_pScene = new g3dScene(new g3dLayer(l_pSceneNodeW, g3dLayer::e_ZBuffer, g3dLayer::e_World, g3dLayer::e_Multiplicative, true, true)); // scene owns layers
		l_pSceneNodeS = new g3dSceneNode();
		l_pScene->AppendLayer(new g3dLayer( l_pSceneNodeS, g3dLayer::e_ZBuffer,
												g3dLayer::e_Screen, g3dLayer::e_Additive,
												false, false, true, false ));
		l_nScreenSpaceIndex = 1;// the index of the layer i just appended.
				
		//set up the overlay camera
		maPoint3d camPos(0.0f, 0.0f, 10.0f);
		maPoint3d camTarg(0.0f, 0.0f, 0.0f);
		maPoint3d camUp(0,1,0);
		l_Camera.LookAt(camPos, camTarg, camUp);

		l_Camera.SetAspect(128,128);
		l_Camera.SetClip(0.3f, 100.0f);
		l_Camera.SetFOV(65);
		l_Camera.Set();

		//initialize the dialog
		brshColorDialogUtil::Init();
#endif
	}

	//------------------------------------------------------------------------
	// deinitialize the components of the paint renderer, free up all resources
	//------------------------------------------------------------------------
	void DeInitialize()
	{
#ifdef ENABLE_PAINT
		gpxRenderControl::ConfirmSingleThread();

		//remove property object
		if(l_pBrushObject)
			delete l_pBrushObject;

		// cleanup window and viewer
		delete l_pPaintTargetRenderer;
		l_pPaintTargetRenderer = NULL;
		
		matTextureMgr::ReleaseTexture(l_pTempRenderTarget);
		l_pTempRenderTarget = NULL;

		if(l_pRenderer)
			delete l_pRenderer;
		l_pRenderer = NULL;
		
		if(l_pCurrentStroke)
			delete l_pCurrentStroke;
		l_pCurrentStroke = NULL;

		//the original param pointer will cleanup the memory
		l_pCurrentParam = NULL;

		// cleanup main scene component
		delete l_pScene;
		l_pScene = NULL;
		delete l_pSceneNodeW;  
		l_pSceneNodeW = NULL;
		delete l_pSceneNodeS;  
		l_pSceneNodeS = NULL;
#endif
	}

	//------------------------------------------------------------------------
	// Get the manager's brush data
	//------------------------------------------------------------------------
	brshData& Data()
	{
		if(l_pBrushObject == NULL)
		{
			//add Property object
			l_pBrushObject = new brshPropertyObject();
		}

		return l_pBrushObject->m_Data;
	}

	//------------------------------------------------------------------------
	// Enable or disable paint mode
	//------------------------------------------------------------------------
	void EnablePaint(bool i_bEnable, bool i_bClearData)
	{
		l_pBrushObject->m_Data.m_bEnabled.SetValue(i_bEnable);
		//enable or disable tablet recognition
		if(inTablet* pTab = inDeviceMgr::GetTablet())
		{
			pTab->SetInkEnabled( i_bEnable );
		}

		if(i_bClearData)
			ClearPaintData();
	}

	//------------------------------------------------------------------------
	// Get whether or not the paint brush manager is enabled
	//------------------------------------------------------------------------
	bool IsEnabled()
	{
		return l_pBrushObject->m_Data.m_bEnabled.GetValue();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetPaintMode(int i_Mode)
	{
		l_PaintMode = i_Mode;
	}

	//------------------------------------------------------------------------
	//  Set the object and material index that should be drawn
	//------------------------------------------------------------------------
	void SetMaterial(mtrlScriptObject* i_Object, int i_Index)
	{
		l_Object = i_Object;
		l_MatIndex = i_Index;
	
		if(l_MatIndex >=0)
		{
			l_pCurrentStroke = l_Object->GetMaterialUI(l_MatIndex)->GetBrushStroke();
		}
	}

	//------------------------------------------------------------------------
	// Initialize a new brush object with a matTexture
	//------------------------------------------------------------------------
	void SetMatTexture(effParamTexture* i_pParam)
	{
		if( i_pParam == NULL )
		{
			guiMessageBox::Show("Info for the texture does not exist.", "No Texture Info");
			l_pCurrentStroke = NULL;
			l_pBrushObject->m_Data.m_bEnabled.SetValue(false);
			return;
		}

		gpxRenderControl::ConfirmSingleThread();

		//return if we are already drawing to this texture
		if(l_pCurrentParam == i_pParam)
			return;

		if( l_pCurrentStroke )
		{
			clear_render_target();
			delete l_pCurrentStroke;
			l_pCurrentStroke = NULL;
		}
		//painting on a new canvas so, set the dirty flag off
		l_bCanvasDirty = false;

		//check if the texture should be saved before switching
		PromptSave();

		l_pBrushObject->SetSaveEnabled(false);
		l_pCurrentParam = i_pParam;

		//create a new brushstroke
		l_pCurrentStroke = new brshBrushStroke();
		UpdateBrushStroke();
	
		//create the render target using the matTexture info
		matTexture* pTex = i_pParam->GetTexture();

		// check for invalid file formats with the current texture;
		if(i_pParam->Property().GetValue().GetNumNames() > 0)
		{
			itString ext;
			i_pParam->Property().GetValue().GetLastName().GetExtension( ext );
			if( ext == itString("dds") )
			{
				guiMessageBox::Show("The texture file format is not a valid paint canvas", "Invalid Texture Type");
				l_pCurrentParam = NULL;
				delete l_pCurrentStroke;
				l_pCurrentStroke = NULL;
				l_pBrushObject->m_Data.m_bEnabled.SetValue(false);
				return;
			}
		}

		set_render_target(pTex);

		if( l_pCurrentStroke->GetStrokeRenderTarget() )
		{
			l_pCurrentParam->SetTexture(l_pCurrentStroke->GetStrokeRenderTarget());
		}

		// Display the name of the material file we are drawing to
		if( l_pBrushObject != NULL )
		{
			l_pBrushObject->m_Data.m_CurrentCanvas.SetValue( i_pParam->Property().GetString() );
		}
	}
	
	//------------------------------------------------------------------------
	// save the image to the desired location
	//------------------------------------------------------------------------
	void SaveImage(const fsLocator& i_SaveLocation)
	{
		bool reload = false;

		if(fsFileUtil::FileExists(i_SaveLocation))
			reload = true;
		
		if(save_image(i_SaveLocation))
		{
			l_bCanvasDirty = false;
			//turn off save button
			l_pBrushObject->SetSaveEnabled(false);
			if( reload )
				matTextureMgr::ReloadTexture(i_SaveLocation);
			else
				l_pCurrentParam->SetTexture(matTextureMgr::LoadTexture(i_SaveLocation));
			
			prtyTextureFileData val;
			val.m_TextureLocator = i_SaveLocation;
			l_pCurrentParam->Property().SetValueWithoutNotify(val);
			l_pBrushObject->m_Data.m_CurrentCanvas.SetValue( l_pCurrentParam->Property().GetString() );
		}
	}

	//------------------------------------------------------------------------
	// PromptSave - before closing the dialog ask if the user wants to save
	//------------------------------------------------------------------------
	void PromptSave()
	{
		if(l_bCanvasDirty)
		{
			int choice = guiMessageBox::Show("Would you like to save the changes made to the texture?", "Save Painted Texture", guiMessageBox::e_YesNo);
			if(choice == guiMessageBox::e_Yes)
			{
				l_pBrushObject->m_Data.m_SaveButton.TriggerCallbacks();
			}
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ClearPaintData(bool i_bClearStroke)
	{
		gpxRenderControl::ConfirmSingleThread();

		l_pCurrentParam = NULL;

		if(l_pCurrentStroke)
		{
			if(i_bClearStroke)
				ClearStrokeTarget();
			delete l_pCurrentStroke;
		}
	
		l_pCurrentStroke = NULL;
		l_pTempRenderTarget = NULL;	
		if( l_pBrushObject != NULL )
		{
			l_pBrushObject->m_Data.m_CurrentCanvas.SetValue("");
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ClearStrokeTarget()
	{
		clear_render_target();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateBrushStroke()
	{
		set_brush_color(l_pBrushObject->m_Data.m_Color.GetValue());
		set_brush_texture(l_pBrushObject->GetTextureLocator());
	}

	//------------------------------------------------------------------------
	//	Paint to the material based on the current mode
	//------------------------------------------------------------------------
	void Paint(const g3dPickInfo& i_PickInfo, int i_Pressure, bool i_bErase)
	{
		if( !l_pCurrentStroke )
			return;

		l_pTempRenderTarget = l_pCurrentStroke->GetStrokeRenderTarget();
	
		//set the UV coordinates
		l_U = i_PickInfo.m_U;
		l_V = i_PickInfo.m_V;

		//adjust the UV coordinates for Screen Space.
		ss_adjust(l_U);
		ss_adjust(l_V);
		
		//TODO: figure out a way to erase texture paint
		//if(i_bErase)
		//	set_brush_color(maFloatRGBA(0.0f,0.0f,1.0f,1.0f));
		
		set_brush_size(i_Pressure);

		//paint according to the current mode
		switch(l_PaintMode)
		{
		case e_Color:
			paint_color();
			break;
		case e_LightIntensity:
			paint_light();
			break;
		default:
			break;
		}

		if( !l_bCanvasDirty && !l_pBrushObject->GetSaveEnabled() )
		{
			l_pBrushObject->SetSaveEnabled(true);
			set_canvas_dirty();
		}
		else if( l_bCanvasDirty && !l_pBrushObject->GetSaveEnabled() )
		{
			l_pBrushObject->SetSaveEnabled(true);
		}

		l_pTempRenderTarget = NULL;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetBrushObject()
	{
		return l_pBrushObject;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DialogCheck()
	{
		if(!brshColorDialogUtil::IsVisible() && l_bDialogVisible)
		{
			l_bDialogVisible = false;
			bool bEnable = false;
			bool bClearData = false;
			EnablePaint(bEnable, bClearData);
			//prompt save only when closing the dialog
			PromptSave();
		}
		else if(brshColorDialogUtil::IsVisible() && !l_bDialogVisible)
		{
			l_bDialogVisible = true;
		}
	}

}  // end brshPaintBrushMgr namespace