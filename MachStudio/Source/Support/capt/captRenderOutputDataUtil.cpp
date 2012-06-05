/*****************************************************************************
**	captRenderOutputDataUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/capt/captRenderOutputDataUtil.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Support/capt/captRenderOutputObject.hpp"
#include "Support/capt/captRenderOutputTagsUtil.hpp"
#include "Support/capt/captStereoUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/pyth/pythProperty.hpp"
#include "Support/tmln/tmlnTimeline.hpp"
#include "Support/rlyr/rlyrPassesObject.hpp"

//	library
#include "Core/env/envString.hpp"
#include "Core/fs/fsCategoryConfigFileUtil.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyButtonUIInfo.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyFolderChooserUIInfo.hpp"
#include "Core/prty/prtyListBoxUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/G3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"


//============================================================================
//============================================================================
namespace captRenderOutputDataUtil
{
	namespace
	{
		//bool l_bShadowEditorState = false;
		g3dSingleLightRendering::ShadowQualityOverride l_ShadowQualityState = g3dSingleLightRendering::SQ_NONE;

		//
		//	render output object
		//
		static captRenderOutputObject* l_DataObject = 0;
		
		//====================================================================
		//====================================================================
		class CaptureNameResolver : public pythProperty::NameResolver
		{
			//------------------------------------------------------------
			// Resolve the string "CapturePrefs" into our property object
			// in order to be accessible from python.
			//------------------------------------------------------------
			virtual prtyObject* ResolveName(std::string &i_PropertyObjectName)
			{
				if (i_PropertyObjectName == std::string("CapturePrefs"))
				{
					return l_DataObject;
				}
				return NULL;
			}
		};

		shared_ptr<CaptureNameResolver> l_NameResolver(new CaptureNameResolver);

		//--------------------------------------------------------------------------------
		//--------------------------------------------------------------------------------
		void debug_OutputDataValues()
		{
			DBG_LOG( "SCDU Data -> Capture Data" );
			DBG_LOG( "-------------------" );
			DBG_LOG( "SCDU: cap FPS  = " << l_DataObject->m_Data.m_fCaptureFPS.GetValue() );
//			DBG_LOG( "SCDU: cap mblur= " << l_DataObject->m_Data.m_nMotionSamplesPerFrame.GetValue() );
			DBG_LOG( "SCDU: cap sampl= " << l_DataObject->m_Data.m_nCaptureSampling.GetValue() );
			DBG_LOG( "SCDU: cap jittr= " << (l_DataObject->m_Data.m_bJitteredSampling.GetValue() ? "true":"false"));
			DBG_LOG( "SCDU: filtr wid= " << l_DataObject->m_Data.m_FilterWidth.GetValue());
			DBG_LOG( "SCDU: pixel aspect= " << l_DataObject->m_Data.m_PixelAspect.GetValue());
			DBG_LOG( "SCDU: filtr fun= " << l_DataObject->m_Data.m_FilterFunc.GetValue());
			DBG_LOG( "SCDU: cap max  = " << l_DataObject->m_Data.m_nCaptureMax.GetValue() );
			DBG_LOG( "SCDU: lead in  = " << l_DataObject->m_Data.m_fLeadIn.GetValue() );
			DBG_LOG( "SCDU: lead out = " << l_DataObject->m_Data.m_fLeadOut.GetValue() );
			DBG_LOG( "SCDU: starttime= " << l_DataObject->m_Data.m_fStartTime.GetValue().AsSeconds() );
			DBG_LOG( "SCDU: end time = " << l_DataObject->m_Data.m_fEndTime.GetValue().AsSeconds() );
			//DBG_LOG( "SCDU: mrkr in  = " << l_DataObject->m_Data.m_fMarkerInTime.GetValue() );
			//DBG_LOG( "SCDU: mrkr out = " << l_DataObject->m_Data.m_fMarkerOutTime.GetValue() );
			DBG_LOG( "SCDU: width    = " << l_DataObject->m_Data.m_nWidth.GetValue() );
			DBG_LOG( "SCDU: height   = " << l_DataObject->m_Data.m_nHeight.GetValue() );
			DBG_LOG( "SCDU: Smoothing= " << ( l_DataObject->m_Data.m_bSmoothing.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: cap movie= " << ( l_DataObject->m_Data.m_bCaptureMovie.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: scenename= " << ( l_DataObject->m_Data.m_bUseSceneFilenameInFilename.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: cam name = " << ( l_DataObject->m_Data.m_bUseCameraNameInFilename.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: rlyr name= " << ( l_DataObject->m_Data.m_bUseLayerNameInFilename.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: use comp = " << ( l_DataObject->m_Data.m_bUseCompressionInFilename.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: use rpass= " << ( l_DataObject->m_Data.m_bUseRenderPassInFilename.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: scene dir= " << ( l_DataObject->m_Data.m_bUseSceneFilenameAsDirectory.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: cam   dir= " << ( l_DataObject->m_Data.m_bUseCameraNameAsDirectory.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: rlyr  dir= " << ( l_DataObject->m_Data.m_bUseLayerNameAsDirectory.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: rpass dir= " << ( l_DataObject->m_Data.m_bUseRenderPassAsDirectory.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: res   dir= " << ( l_DataObject->m_Data.m_bUseResolutionAsDirectory.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: driverdir= " << ( l_DataObject->m_Data.m_bUseRenderDriversAsUniqueCameras.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: all cams = " << ( l_DataObject->m_Data.m_bCaptureAllCameras.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: timecode = " << ( l_DataObject->m_Data.m_bDisplayTimeCode.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: titlecard= " << ( l_DataObject->m_Data.m_bOutputTitleCard.GetValue() ? "true":"false" ) );
			//DBG_LOG( "SCDU: markers  = " << ( l_DataObject->m_Data.m_bUseCaptureDrivers.GetValue() ? "true":"false" ) );
			//DBG_LOG( "SCDU: markers  = " << ( l_DataObject->m_Data.m_bUseMarkerTimes.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: compress = " << l_DataObject->m_Data.m_CompressCode.GetValue().c_str() );
			DBG_LOG( "SCDU: prefix   = " << l_DataObject->m_Data.m_Prefix.GetValue().c_str() );
			DBG_LOG( "SCDU: cap type = " << l_DataObject->m_Data.m_CaptureFormat.GetValue().c_str() );
			DBG_LOG( "SCDU: cntr #s  = " << l_DataObject->m_Data.m_nCounterDigits.GetValue() );
			DBG_LOG( "SCDU: directory= " << l_DataObject->m_Data.m_OutputDirectoryRoot.GetValue() );
			DBG_LOG( "SCDU: soundfile= " << l_DataObject->m_Data.m_SoundFile.GetValue() );
			DBG_LOG( "SCDU: progress = " << ( l_DataObject->m_Data.m_bShowRenderProgressDialog.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: movie per= " << ( l_DataObject->m_Data.m_bMoviePerDriver.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: layervis = " << ( l_DataObject->m_Data.m_bUseLayerVisibility.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: dir tags = " << l_DataObject->m_Data.m_OutputDirectoryTags.GetValue() );
			DBG_LOG( "SCDU: dir cstm = " << (l_DataObject->m_Data.m_bOutputDirectoryCustomize.GetValue() ? "true":"false" ) );
			DBG_LOG( "SCDU: file tags= " << l_DataObject->m_Data.m_OutputFileTags.GetValue() );
			DBG_LOG( "SCDU: file cstm= " << (l_DataObject->m_Data.m_bOutputFileCustomize.GetValue() ? "true":"false" ) );
		}
	}

	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp()
	{
		if (l_DataObject)
		{
			// Remove name resolver 
			pythProperty::RemoveNameResolver( l_NameResolver ); 

			delete l_DataObject;
			l_DataObject = NULL;
		}

	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	captRenderOutputData& Data()
	{
		if (l_DataObject == 0)
		{
			l_DataObject = new captRenderOutputObject();
			//l_DataObject->ConvertQuickTime();
			// Add name resolver to match a string to our prtyObject
			pythProperty::AddNameResolver( l_NameResolver ); 
		}

		return l_DataObject->m_Data;
	}

	//------------------------------------------------------------------------
	// Set format and compress code for the render
	//------------------------------------------------------------------------
	void SetCaptureFormat(const itString& i_CapFormat)
	{
		std::string format = itStringUtil::GetStdString(i_CapFormat);
		l_DataObject->m_Data.m_CaptureFormat.SetValue(format.c_str());
	}
	void SetCompressCode(const itString& i_CompressCode)
	{
		std::string code = itStringUtil::GetStdString(i_CompressCode);
		l_DataObject->m_Data.m_CompressCode.SetValue(code.c_str());
	}

	//------------------------------------------------------------------------
	// Set the given camera as an active camera
	//------------------------------------------------------------------------
	void SetActiveCamera(const itString& i_Camera)
	{
		std::string cam = itStringUtil::GetStdString(i_Camera);
		int num_cameras = l_DataObject->m_Data.m_Cameras.GetNumberOfItems();
		for (int i = 0; i < num_cameras; ++i)
		{
			if ( cam == l_DataObject->m_Data.m_Cameras.GetValueText(i))
			{
				l_DataObject->m_Data.m_Cameras.SetValueFlag(i, true);
			}
		}
	}

	//------------------------------------------------------------------------
	// Set the render resolution width and height
	//------------------------------------------------------------------------
	void SetCaptureWidth(int i_Width)
	{
		l_DataObject->m_Data.m_nWidth.SetValue(i_Width);
	}
	void SetCaptureHeight(int i_Height)
	{
		l_DataObject->m_Data.m_nHeight.SetValue(i_Height);
	}

	//------------------------------------------------------------------------
	// Set the render output file and path
	//------------------------------------------------------------------------
	void SetCaptureOutputFile(itString& i_OutputFile)
	{
		l_DataObject->m_Data.m_OutputFileTags.SetValue( i_OutputFile );
		l_DataObject->m_Data.m_bOutputFileCustomize.SetValue( true );
	}
	void SetCaptureOutputDirectory(itString& i_OutputPath)
	{
		l_DataObject->m_Data.m_OutputDirectoryTags.SetValue( i_OutputPath );
		l_DataObject->m_Data.m_bOutputDirectoryCustomize.SetValue( true );
	}
	void SetCaptureOutputDirectoryRoot(itString& i_OutputPath)
	{
		l_DataObject->m_Data.m_OutputDirectoryRoot.SetValue( i_OutputPath );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetLayerData(captRenderOutputData& i_LayerData)
	{
		if (l_DataObject == 0)
		{
			l_DataObject = new captRenderOutputObject();
			//l_DataObject->ConvertQuickTime();
			// Add name resolver to match a string to our prtyObject
			pythProperty::AddNameResolver( l_NameResolver ); 
		}

		//l_DataObject->m_Data.m_SoundFile.SetValue( i_LayerData.m_SoundFile.GetValue() );
		//l_DataObject->m_Data.m_bCaptureMovie.SetValue( i_LayerData.m_bCaptureMovie.GetValue() );
		//l_DataObject->m_Data.m_CaptureFormat.SetValue( i_LayerData.m_CaptureFormat.GetValue() );
		//l_DataObject->m_Data.m_CompressCode.SetValue( i_LayerData.m_CompressCode.GetValue() );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadData(captRenderOutputData& o_Data)
	{
		//l_DataObject->Read(o_Data);
	}
	void ReadData(const fsLocator& i_ConfigFile, captRenderOutputData& o_Data)
	{
		//l_DataObject->Read(i_ConfigFile, o_Data);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void WriteData(const captRenderOutputData& i_Data)
	{
		//l_DataObject->Write(i_Data);
	}
	void WriteData(const fsLocator& i_ConfigFile, const captRenderOutputData& i_Data)
	{
		//l_DataObject->Write(i_ConfigFile, i_Data);
	}


	//------------------------------------------------------------------------
	//	called right before showing the dialog
	//------------------------------------------------------------------------
	void SetupControls( bool i_bBatchMode )
	{
		if (l_DataObject != 0)
		{
			l_DataObject->SetupControls(i_bBatchMode);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void BuildCameraList(captRenderOutputData& o_Data)
	{
		// cameras
		//
		nameString camname;
		const int num_cameras = camsCameraMgr::GetNumCameras();
		const int num_dcuts = camsDirectorsCutMgr::GetNumDirectorsCuts();
		//if (o_Data.m_CameraList.size() == 0)
		{
			o_Data.m_CameraList.clear();
			o_Data.m_CameraList.resize(num_cameras + num_dcuts);
			for (int i = 0; i < num_cameras; ++i)
			{
				camsCameraMgr::GetCameraName(i, camname);
				o_Data.m_CameraList[i].m_CameraName.SetValue( camname.GetString() );
				o_Data.m_CameraList[i].m_bCapture.SetValue( true );
			}
			for (int i = 0; i < num_dcuts; ++i)
			{
				camsDirectorsCutMgr::GetDirectorsCutName(i, camname);
				o_Data.m_CameraList[i+num_cameras].m_CameraName.SetValue( camname.GetString() );
				o_Data.m_CameraList[i+num_cameras].m_bCapture.SetValue( false );
			}
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateCameraList(captRenderOutputData& i_NewData)
	{
		if (l_DataObject != 0)
		{
			l_DataObject->UpdateCameraList(i_NewData);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateData(captRenderOutputData& i_NewData)
	{
		if (l_DataObject != 0)
			l_DataObject->m_Data = i_NewData;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentScene( itString& i_SceneName )
	{
		l_DataObject->m_Data.m_CurrentScene.SetValue( i_SceneName );

		//DBG_LOG("Setting CurrentScene (" << itStringUtil::GetStdString(l_DataObject->m_Data.m_CurrentScene).c_str() << ")");
	}
	void SetCurrentScene( nameString& i_SceneName )
	{
		l_DataObject->m_Data.m_CurrentScene.SetValue( itString(i_SceneName.GetString().c_str()) );

		//DBG_LOG("Setting CurrentScene (" << itStringUtil::GetStdString(l_DataObject->m_Data.m_CurrentScene).c_str() << ")");
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentCamera( itString& i_CamName )
	{
		l_DataObject->m_Data.m_CurrentCamera.SetValue( i_CamName );
	}
	void SetCurrentCamera( nameString& i_CamName )
	{
		l_DataObject->m_Data.m_CurrentCamera.SetValue( itString(i_CamName.GetString().c_str()) );
	}
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentLayer( itString& i_LayerName )
	{
		l_DataObject->m_Data.m_CurrentLayer.SetValue( i_LayerName );
	}
	void SetCurrentLayer( nameString& i_LayerName )
	{
		l_DataObject->m_Data.m_CurrentLayer.SetValue( itString(i_LayerName.GetString().c_str()) );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentRenderPass( itString& i_RenderPass )
	{
		l_DataObject->m_Data.m_CurrentRenderPass.SetValue( i_RenderPass );
		//	TODO: remove spaces from the class
		l_DataObject->m_Data.m_CurrentRenderPassNoSpaces.SetValue( i_RenderPass );
		//l_DataObject->m_Data.m_CurrentRenderPassNoSpaces.GetValue()
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentSceneFilename( itString& i_SceneFilename )
	{
		l_DataObject->m_Data.m_CurrentSceneFilename.SetValue( i_SceneFilename );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetDirectory( fsLocator& i_Dir )
	{
		l_DataObject->m_Data.m_OutputDirectory.SetValue( i_Dir );

		//DBG_LOG( "SCDU: directory= ", l_DataObject->m_Data.m_OutputDirectory.GetValue() );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentFrame( int i_FrameNumber )
	{
		l_DataObject->m_Data.m_nCurrentFrame.SetValue( i_FrameNumber );
	}

	//------------------------------------------------------------------------
	//	Generate the filename based on the current data.
	//
	//	Note: if the "use scene" or "use camera" flags are set, it is up
	//	to the code to set the current scene and/or current camera before
	//	generating the name.
	//
	//	Note: if the output is frames then the counter should be set as well.
	//
	//	the format will be "scenename_cameraname_counter" or a variation of
	//	this depending on the flags set.
	//------------------------------------------------------------------------
	void GenerateFilename( itString& o_Filename, bool i_bCapturingRenderman /*false*/ )
	{
		captRenderOutputTagsUtil::GenerateOutputFilename( l_DataObject->m_Data, o_Filename, i_bCapturingRenderman );
	}

	void GenerateDirectoryName()
	{
		captRenderOutputTagsUtil::GenerateOutputDirectory( l_DataObject->m_Data );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetPrefix( itString& i_Prefix )
	{
		l_DataObject->m_Data.m_Prefix.SetValue( itStringUtil::GetStdString( i_Prefix ) );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetMaxTime( int i_nMaxTime )
	{
		l_DataObject->m_Data.m_nCaptureMax.SetValue( i_nMaxTime );
	}

	//------------------------------------------------------------------------
	//	Turn On/off the shadows for capture.  This will also store what the
	//	current shadow state is.	Restore will put the shadow back to it's
	//	state before Enable was called.
	//------------------------------------------------------------------------
	void EnableShadows( bool i_bOn )
	{
		//l_bShadowEditorState = g3dSingleLightRendering::GetDoSingleLightRendering();
		//rndrPrefsUtil::SetMultipassRendering( i_bOn );

		l_ShadowQualityState = g3dSingleLightRendering::GetShadowQualityOverride();
		g3dSingleLightRendering::SetShadowQualityOverride( (g3dSingleLightRendering::ShadowQualityOverride)l_DataObject->m_Data.m_ShadowQuality.GetValue() );
	}

	void RestoreShadows()
	{
		//rndrPrefsUtil::SetMultipassRendering( l_bShadowEditorState );
		g3dSingleLightRendering::SetShadowQualityOverride( l_ShadowQualityState );
	}

	//--------------------------------------------------------------------
	//	EnableWireframe - enable/disable wireframe render
	//--------------------------------------------------------------------
	void EnableWireframe( bool i_bRenderWireframe )
	{
		g3dPrefs::CurrentPrefs().m_bRenderWireframe = i_bRenderWireframe;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject()
	{
		return l_DataObject;
	}

	//------------------------------------------------------------------------
	//	output the struct values at anytime
	//------------------------------------------------------------------------
	void Debug()
	{
		debug_OutputDataValues();
	}
	
	//------------------------------------------------------------------------
	// When a new scene is loaded, set all the capture options to default
	//------------------------------------------------------------------------
	void SetToDefault(fsLocator& i_CurrentScene)
	{
		l_DataObject->SetToDefault(i_CurrentScene, l_DataObject->m_Data);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	itString GetRenderPassName(rlyrPassesObject::ePassType i_PassType)
	{
		itString pass_name;
		switch (i_PassType)
		{
		case rlyrPassesObject::e_Beauty:
			pass_name = l_DataObject->m_Data.m_BeautyPassName.GetValue();
			break;
		case rlyrPassesObject::e_Diffuse:
			pass_name = l_DataObject->m_Data.m_DiffusePassName.GetValue();
			break;
		case rlyrPassesObject::e_Specular:
			pass_name = l_DataObject->m_Data.m_SpecularPassName.GetValue();
			break;
		case rlyrPassesObject::e_AOOnly:
			pass_name = l_DataObject->m_Data.m_AOOnlyPassName.GetValue();
			break;
		case rlyrPassesObject::e_Depth:
			pass_name = l_DataObject->m_Data.m_DepthPassName.GetValue();
			break;
		case rlyrPassesObject::e_ShadowMask:
			pass_name = l_DataObject->m_Data.m_ShadowMaskPassName.GetValue();
			break;
		case rlyrPassesObject::e_IlluminationOnly:
			pass_name = l_DataObject->m_Data.m_IlluminationOnlyPassName.GetValue();
			break;
		case rlyrPassesObject::e_Normals:
			pass_name = l_DataObject->m_Data.m_NormalsPassName.GetValue();
			break;
		case rlyrPassesObject::e_DirtyMatte:
			pass_name = l_DataObject->m_Data.m_DirtyMattePassName.GetValue();
			break;
		case rlyrPassesObject::e_Wireframe:
			pass_name = l_DataObject->m_Data.m_WireframePassName.GetValue();
			break;
		case rlyrPassesObject::e_Materials:
			pass_name = l_DataObject->m_Data.m_MaterialsPassName.GetValue();
			break;
		case rlyrPassesObject::e_ReflectionsOnly:
			pass_name = l_DataObject->m_Data.m_ReflectionsOnlyPassName.GetValue();
			break;
		case rlyrPassesObject::e_Velocity:
			pass_name = l_DataObject->m_Data.m_VelocityPassName.GetValue();
			break;
		case rlyrPassesObject::e_Bloom:
			pass_name = l_DataObject->m_Data.m_BloomPassName.GetValue();
			break;
		case rlyrPassesObject::e_Star:
			pass_name = l_DataObject->m_Data.m_StarPassName.GetValue();
			break;
		case rlyrPassesObject::e_CameraDOF:
			pass_name = l_DataObject->m_Data.m_CameraDOFPassName.GetValue();
			break;
		case rlyrPassesObject::e_Preview:
			pass_name = l_DataObject->m_Data.m_PreviewPassName.GetValue();
			break;
		case rlyrPassesObject::e_Emissive:
			pass_name = l_DataObject->m_Data.m_EmissivePassName.GetValue();
			break;
		case rlyrPassesObject::e_SpecEnv:
			pass_name = l_DataObject->m_Data.m_SpecEnvPassName.GetValue();
			break;
		case rlyrPassesObject::e_SpecLit:
			pass_name = l_DataObject->m_Data.m_SpecLitPassName.GetValue();
			break;
		case rlyrPassesObject::e_DiffEnv:
			pass_name = l_DataObject->m_Data.m_DiffEnvPassName.GetValue();
			break;
		case rlyrPassesObject::e_DiffLit:
			pass_name = l_DataObject->m_Data.m_DiffLitPassName.GetValue();
			break;
		case rlyrPassesObject::e_GlobalIllumination:
			pass_name = l_DataObject->m_Data.m_GIPassName.GetValue();
			break;
		case rlyrPassesObject::e_Glow:
			pass_name = l_DataObject->m_Data.m_GlowPassName.GetValue();
			break;
		case rlyrPassesObject::e_MrayFinalGather:
			pass_name = l_DataObject->m_Data.m_MrayFinalGatherPassName.GetValue();
			break;
		case rlyrPassesObject::e_RmanColorBleed:
			pass_name = l_DataObject->m_Data.m_RmanColorBleedPassName.GetValue();
			break;
		}
		return pass_name;
	}
}
