/*****************************************************************************
**	captRenderOutputObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
*****************************************************************************/
#include "Support/capt/captRenderOutputObject.hpp"

#include "Features/Capture/cptrRenderUtil.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Support/capt/captRenderOutputTagsUtil.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmQuickTimeUtil.hpp"		// for the USE_QUICKTIME define
#include "Support/pyth/pythProperty.hpp"
#include "Support/tmln/tmlnTimeEditUIInfo.hpp"
#include "Support/tmln/tmlnTimeline.hpp"

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
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"

#include <sstream>


//============================================================================
//============================================================================
namespace
{
	const char* l_cRenderKey_MainKey = "RenderOutput";
	const char* l_cRenderKey_CaptureMovie = "CaptureMovie";
	const char* l_cRenderKey_UseCaptureDrivers = "UseCaptureDrivers";
	const char* l_cRenderKey_RenderFrameRange = "RenderFrameRange";
	//const char* l_cRenderKey_UseMarkers = "UseMarkers";
	//const char* l_cRenderKey_MarkerInTime = "MarkerInTime";
	//const char* l_cRenderKey_MarkerOutTime= "MarkerOutTime";
	const char* l_cRenderKey_CaptureFPS = "CaptureFPS";
	const char* l_cRenderKey_MotionSamplesPerFrame = "MotionSamplesPerFrame";
	const char* l_cRenderKey_CaptureMax = "CaptureMax";
	const char* l_cRenderKey_Sampling = "Sampling";
	const char* l_cRenderKey_JitteredSampling = "JitteredSampling";
	const char* l_cRenderKey_FilterWidth = "FilterWidth";
	const char* l_cRenderKey_FilterFunc = "FilterFunc";
	const char* l_cRenderKey_PixelAspect = "PixelAspect";
	const char* l_cRenderKey_CaptureFormat = "CaptureFormat";
	const char* l_cRenderKey_CompressCode = "CompressCode";
	const char* l_cRenderKey_Directory = "Directory";
	const char* l_cRenderKey_SoundFile = "SoundFile";
	const char* l_cRenderKey_DisplayTimeCode = "DisplayTimeCode";
	const char* l_cRenderKey_EndTime = "EndTime";
	const char* l_cRenderKey_LeadIn = "LeadIn";
	const char* l_cRenderKey_LeadOut = "LeadOut";
	const char* l_cRenderKey_Prefix = "Prefix";
	const char* l_cRenderKey_StartTime = "StartTime";
	const char* l_cRenderKey_CaptureAllCameras = "CaptureAllCameras";
	const char* l_cRenderKey_UseCameraName = "UseCameraName";
	const char* l_cRenderKey_UseSceneFilename = "UseSceneFilename";
	const char* l_cRenderKey_UseCompressionInName = "UseCompressionInName";
	const char* l_cRenderKey_UseRenderPassInName = "UseRenderPassInName";
	const char* l_cRenderKey_OutputFileTags = "OutputFileTags";
	const char* l_cRenderKey_OutputFileCustomize = "OutputFileCustomize";
	const char* l_cRenderKey_UseRenderDriverAsDir = "UseRenderDriverAsDir";
	const char* l_cRenderKey_UseCameraNameAsDir = "UseCameraNameAsDir";
	const char* l_cRenderKey_UseResolutionAsDir = "UseResolutionAsDir";
	const char* l_cRenderKey_UseSceneFilenameAsDir = "UseSceneFilenameAsDir";
	const char* l_cRenderKey_OutputDirectoryTags = "OutputDirectoryTags";
	const char* l_cRenderKey_OutputDirectoryCustomize = "OutputDirectoryCustomize";
	const char* l_cRenderKey_Width = "Width";
	const char* l_cRenderKey_Height = "Height";
	const char* l_cRenderKey_TitleCard = "TitleCard";
	const char* l_cRenderKey_CounterDigits = "CounterDigits";
	//const char* l_cRenderKey_SubdivLevel = "SubdivLevel";
	const char* l_cRenderKey_Smoothing = "Smoothing";
	const char* l_cRenderKey_PostEmailNotify = "PostEmailNotify";
	const char* l_cRenderKey_PostEmail = "PostEmail";
	const char* l_cRenderKey_PostCommandToExecute = "PostCommandToExecute";
	const char* l_cRenderKey_ExecutePostCommand = "ExecutePostCommand";
	const char* l_cRenderKey_AutoGenDir= "AutoGenDir";
	const char* l_cRenderKey_KeepFrameOpen = "KeepFrameOpen";
	const char* l_cRenderKey_ShowCaptureDialog = "ShowCaptureDialog";
	const char* l_cRenderKey_ShowRenderProgressDialog = "ShowRenderProgressDialog";
	const char* l_cRenderKey_MoviePerDriver = "MoviePerDriver";
	const char* l_cRenderKey_UseLayerVisibility = "UseLayerVisibility";
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
captRenderOutputObject::captRenderOutputObject( bool i_bIsLayerOutput, bool i_bIsMasterLayer )
{
	fsLocator curscene;
	SetToDefault(curscene, m_Data);
	m_bIsLayerOutput = i_bIsLayerOutput;
	m_bIsMasterLayer = i_bIsMasterLayer;

	RegisterProperties();
	captRenderOutputTagsUtil::BuildTags( m_Data );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
captRenderOutputObject::captRenderOutputObject( captRenderOutputData& i_Data, bool i_bIsLayerOutput, bool i_bIsMasterLayer )
{
	m_Data = i_Data;
	m_bIsLayerOutput = i_bIsLayerOutput;
	m_bIsMasterLayer = i_bIsMasterLayer;

	RegisterProperties();
	captRenderOutputTagsUtil::BuildTags( m_Data );
}

//------------------------------------------------------------------------
//	called right before showing the dialog
//------------------------------------------------------------------------
void captRenderOutputObject::SetupControls( bool i_bBatchMode )
{
	if( !m_bIsLayerOutput )
	{
		update_compresscodeUII( m_Data.m_CaptureFormat.GetValue() );
		m_pCompressCodeUII->AddProperty(&(m_Data.m_CompressCode));
		//	set controls based on batch mode
		//
		m_pStartTimeUII->SetReadOnly( i_bBatchMode );
		m_pEndTimeUII->SetReadOnly( i_bBatchMode );
	}

	//bool bEnableRangeControls = (!m_Data.m_bUseCaptureDrivers.GetValue());
	bool bEnableRangeControls = (m_Data.m_RenderFrameRange.GetValue() == captRenderOutputData::eSpecifyRange);
	m_pStartTimeUII->SetReadOnly(!bEnableRangeControls);
	m_pEndTimeUII->SetReadOnly(!bEnableRangeControls);

	if(cptrRenderUtil::GetPSflag())
	{
		m_bUseSceneFilenameAsDirectoryUI->SetReadOnly(true);
		m_bUseCameraNameAsDirectoryUI->SetReadOnly(true);
		m_bUseLayerNameAsDirectoryUI->SetReadOnly(true);
		m_bUseRenderPassAsDirectoryUI->SetReadOnly(true);
		m_bUseResolutionAsDirectoryUI->SetReadOnly(true);
		m_OutputDirectoryRootUI->SetReadOnly(true);
	}
	else
	{
		m_bUseSceneFilenameAsDirectoryUI->SetReadOnly(false);
		m_bUseCameraNameAsDirectoryUI->SetReadOnly(false);
		m_bUseLayerNameAsDirectoryUI->SetReadOnly(false);
		m_bUseRenderPassAsDirectoryUI->SetReadOnly(false);
		m_bUseResolutionAsDirectoryUI->SetReadOnly(false);
		m_OutputDirectoryRootUI->SetReadOnly(false);
	}

	if (m_Data.m_bOutputDirectoryCustomize == true)
		m_OutputDirectoryTagsUI->SetReadOnly( false );
	else
		m_OutputDirectoryTagsUI->SetReadOnly( true );

	if (m_Data.m_bOutputFileCustomize == true)
		m_OutputFilenameTagsUI->SetReadOnly( false );
	else
		m_OutputFilenameTagsUI->SetReadOnly( true );

	//	Do this at the beginning so the field gets filled in.
	captRenderOutputTagsUtil::BuildTags( m_Data );

	//	these should also be done related to batch mode
	//
	//checkBox_outputdir_automatic->Checked = true;
	//checkBox_usescenefilename->Checked = true;
	//checkBox_usescenefilename->Enabled = false;
	//checkBox_usescameraname->Checked = true;
	//checkBox_usescameraname->Enabled = false;
	//checkBox_CaptureAllCams->Enabled = false;
	//checkBox_CaptureAllCams->Checked = true;
	//checkedListBox_cameras->Enabled = false;
	//button_resetend->Enabled = !i_bBatchMode;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void captRenderOutputObject::UpdateCameraList( captRenderOutputData& i_NewData )
{
	m_Data.m_CameraList = i_NewData.m_CameraList;
	m_pCamerasUIInfo->m_List.clear();
	//DBG_LOG("\nUPDATE CAMERA LIST");
	for (int i=0; i < i_NewData.m_CameraList.size(); ++i)
	{
		//int b1, b2 = false;
		//b1 = i_NewData.m_CameraList[i].m_bCapture.GetValue();
		////b2 = i_NewData.m_Cameras.GetValueFlag(i);
		//DBG_LOG3("%02d %s vs %s", i, b1?"true":"false", b2?"true":"false");

		m_pCamerasUIInfo->AddItem( i_NewData.m_CameraList[i].m_CameraName.GetValue(), i_NewData.m_CameraList[i].m_bCapture.GetValue() );
	}

	if (i_NewData.m_bCaptureAllCameras.GetValue())
		m_pCamerasUIInfo->SetReadOnly(true);
	else
		m_pCamerasUIInfo->SetReadOnly(false);

	//m_pCamerasUIInfo->UpdateControl();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void captRenderOutputObject::WriteToConfigFile( const fsLocator& i_ConfigFile,
												const captRenderOutputData& i_Data )
{
	//	if the code is in the middle of reading the file, don't allow writing.
	//if (l_bReadingData)
	//	return;

	DBG_TRACE("Writing config file " << i_ConfigFile );

	fsLocator cfgdir = i_ConfigFile;
	std::string cfgpath;
	fsFileUtil::LocatorToANSIFilename(cfgdir,cfgpath);

	//	quick and dirty XML Writer
	guiXMLTextWriter::Open(cfgpath.c_str());
	guiXMLTextWriter::WriteStartElement(l_cRenderKey_MainKey);

	//	write the actual values
	guiXMLTextWriter::WriteElement(l_cRenderKey_CaptureMovie, i_Data.m_bCaptureMovie.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_MoviePerDriver, i_Data.m_bMoviePerDriver.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_ShowRenderProgressDialog, i_Data.m_bShowRenderProgressDialog.GetValue());
	//guiXMLTextWriter::WriteElement(l_cRenderKey_UseCaptureDrivers, i_Data.m_bUseCaptureDrivers.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_RenderFrameRange, i_Data.m_RenderFrameRange.GetValue());
	//guiXMLTextWriter::WriteElement(l_cRenderKey_UseMarkers, i_Data.m_bUseMarkerTimes.GetValue());
	//guiXMLTextWriter::WriteElement(l_cRenderKey_MarkerInTime, i_Data.m_fMarkerInTime.GetValue());
	//guiXMLTextWriter::WriteElement(l_cRenderKey_MarkerOutTime, i_Data.m_fMarkerOutTime.GetValue());
	//guiXMLTextWriter::WriteElement(l_cRenderKey_CaptureFPS, i_Data.m_fCaptureFPS.GetValue());
	//guiXMLTextWriter::WriteElement(l_cRenderKey_MotionSamplesPerFrame, i_Data.m_nMotionSamplesPerFrame.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_CaptureMax, i_Data.m_nCaptureMax.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_Sampling, i_Data.m_nCaptureSampling.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_JitteredSampling, i_Data.m_bJitteredSampling.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_FilterWidth, i_Data.m_FilterWidth.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_FilterFunc, i_Data.m_FilterFunc.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_PixelAspect, i_Data.m_PixelAspect.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_CaptureFormat, i_Data.m_CaptureFormat.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_CompressCode, i_Data.m_CompressCode.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_Directory, i_Data.m_OutputDirectoryRoot.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_SoundFile, i_Data.m_SoundFile.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_DisplayTimeCode, i_Data.m_bDisplayTimeCode.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_EndTime, i_Data.m_fEndTime.GetValue().AsSeconds());
	guiXMLTextWriter::WriteElement(l_cRenderKey_LeadIn, i_Data.m_fLeadIn.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_LeadOut, i_Data.m_fLeadOut.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_Prefix, i_Data.m_Prefix.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_StartTime, i_Data.m_fStartTime.GetValue().AsSeconds());
	guiXMLTextWriter::WriteElement(l_cRenderKey_CaptureAllCameras, i_Data.m_bCaptureAllCameras.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_UseCameraName, i_Data.m_bUseCameraNameInFilename.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_UseSceneFilename, i_Data.m_bUseSceneFilenameInFilename.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_UseCompressionInName, i_Data.m_bUseCompressionInFilename.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_OutputFileTags, i_Data.m_OutputFileTags.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_OutputFileCustomize, i_Data.m_bOutputFileCustomize.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_UseRenderPassInName, i_Data.m_bUseRenderPassInFilename.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_UseRenderDriverAsDir, i_Data.m_bUseRenderDriversAsUniqueCameras.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_UseCameraNameAsDir, i_Data.m_bUseCameraNameAsDirectory.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_UseResolutionAsDir, i_Data.m_bUseResolutionAsDirectory.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_UseSceneFilenameAsDir, i_Data.m_bUseSceneFilenameAsDirectory.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_OutputDirectoryTags, i_Data.m_OutputDirectoryTags.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_OutputDirectoryCustomize, i_Data.m_bOutputDirectoryCustomize.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_Width, i_Data.m_nWidth.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_Height, i_Data.m_nHeight.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_TitleCard, i_Data.m_bOutputTitleCard.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_CounterDigits, i_Data.m_nCounterDigits.GetValue());
	//guiXMLTextWriter::WriteElement(l_cRenderKey_SubdivLevel, i_Data.m_nSubdivLevel.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_Smoothing, i_Data.m_bSmoothing.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_KeepFrameOpen, i_Data.m_bKeepFrameOpenAfterRender.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_PostEmailNotify, i_Data.m_bSendPostEmailAddress.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_PostEmail, i_Data.m_PostEmailAddress.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_PostCommandToExecute, i_Data.m_PostCommand.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_ExecutePostCommand, i_Data.m_bExecutePostCommand.GetValue());
	guiXMLTextWriter::WriteElement(l_cRenderKey_UseLayerVisibility, i_Data.m_bUseLayerVisibility.GetValue());

	//	finish it up
	guiXMLTextWriter::WriteEndElement();
	guiXMLTextWriter::Close();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::Write( const captRenderOutputData& i_Data )
{
	WriteToConfigFile(i_Data.m_RenderOutputDataFile.GetValue(), i_Data);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::Write( const fsLocator& i_ConfigFile,
									const captRenderOutputData& i_Data )
{
	WriteToConfigFile(i_ConfigFile, i_Data);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void captRenderOutputObject::ConvertQuickTime()
{
	/*
		This function will be a temporary solution for an issue where a scene that
		was saved in a 32bit build may have MOV capture formats.  If this scene is 
		loaded into the 64bit build, the output format is blank because MOVs aren't 
		supported.  Currently we have converted all MOVs to AVIs in this case.
	*/

	#ifndef USE_QUICKTIME
	bool bRawCodec = (m_Data.m_CompressCode.GetValue() == "raw ");

	if(m_Data.m_CaptureFormat.GetValue() == "MOV")
		m_Data.m_CaptureFormat.SetValue( "AVI" );

	if(!bRawCodec)
		m_Data.m_CompressCode.SetValue("RGB");
		
	#endif
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void captRenderOutputObject::ReadFromConfigFile(const fsLocator& i_ConfigFile,
												captRenderOutputData& o_Data )
{
	std::string cfgpath;

	//	if config file exists, copy the path
	if (fsFileUtil::FileExists( i_ConfigFile ))
	{
		DBG_TRACE("Render Output Config = " << i_ConfigFile);
		fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);
	}
	else
	{
		//	user cfg file not found, read in a default file
		//
		fsLocator cfgdir;
		cfgdir.Push( o_Data.m_RenderOutputDataDefaultFile.GetValue() );
		if (fsFileUtil::FileExists(cfgdir))
		{
			DBG_TRACE("Render Output Config = " << cfgdir);
			fsFileUtil::LocatorToANSIFilename(cfgdir,cfgpath);
		}
		else
		{
			// this shouldn't happen.  hand-made default values
			fsLocator scene;
			SetToDefault(scene, o_Data);
			return;
		}
	}

	//DBG_LOG1("Reading config file (%s)", cfgpath.c_str());

	if (!guiXMLTextReader::Open(cfgpath.c_str()))
	{
		// TODO - should it get here?  bad data file?

		//	config file error, use defaults
		fsLocator scene;
		SetToDefault(scene, o_Data);
		return;
	}

	//	read in the preferences
	//
	guiXMLTextReader::gui_Node_Type node_type;
	std::string keyname, strvalue;
	while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
	{
		//DBG_LOG3("RO file - key [%s]  type [%d]  value [%s]", keyname.c_str(), node_type, strvalue.c_str());

		if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
		{
			if (keyname == l_cRenderKey_CaptureMovie)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bCaptureMovie.SetValue(value);}
			else if (keyname == l_cRenderKey_MoviePerDriver)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bMoviePerDriver.SetValue(value);}
			else if (keyname == l_cRenderKey_ShowRenderProgressDialog)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bShowRenderProgressDialog.SetValue(value);}
			//else if (keyname == l_cRenderKey_UseMarkers)
			//	{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseMarkerTimes.SetValue(value);}
			else if (keyname == l_cRenderKey_UseCaptureDrivers)	// Not used anymore, map to RenderFrameRange values
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_RenderFrameRange.SetValue(value ? captRenderOutputData::eCaptureDrivers : captRenderOutputData::eSpecifyRange);}
			else if (keyname == l_cRenderKey_RenderFrameRange)
				{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_RenderFrameRange.SetValue(value);}
			//else if (keyname == l_cRenderKey_MarkerInTime)
			//	{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fMarkerInTime.SetValue(value);}
			//else if (keyname == l_cRenderKey_MarkerOutTime)
			//	{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fMarkerOutTime.SetValue(value);}
			else if (keyname == l_cRenderKey_CaptureFPS)
				{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fCaptureFPS.SetValue(value);}
			//else if (keyname == l_cRenderKey_MotionSamplesPerFrame)
			//	{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nMotionSamplesPerFrame.SetValue(value);}
			else if (keyname == l_cRenderKey_CaptureMax)
				{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nCaptureMax.SetValue(value);}
			else if (keyname == l_cRenderKey_Sampling)
				{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nCaptureSampling.SetValue(value);}
			else if (keyname == l_cRenderKey_JitteredSampling)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bJitteredSampling.SetValue(value);}
			else if (keyname == l_cRenderKey_FilterWidth)
				{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_FilterWidth.SetValue(value);}
			else if (keyname == l_cRenderKey_FilterFunc)
				{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_FilterFunc.SetValue(value);}
			else if (keyname == l_cRenderKey_PixelAspect)
				{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_PixelAspect.SetValue(value);}
			else if (keyname == l_cRenderKey_CaptureFormat)
				{std::string value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_CaptureFormat.SetValue(value);}
			else if (keyname == l_cRenderKey_CompressCode)
				{std::string value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_CompressCode.SetValue(value);}
			else if (keyname == l_cRenderKey_Directory)
				{fsLocator value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_OutputDirectoryRoot.SetValue(value);}
			else if (keyname == l_cRenderKey_SoundFile)
				{fsLocator value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_SoundFile.SetValue(value);}
			else if (keyname == l_cRenderKey_DisplayTimeCode)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bDisplayTimeCode.SetValue(value);}
			else if (keyname == l_cRenderKey_EndTime)
			{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fEndTime.SetValue(maTime::FromSeconds(value));}
			else if (keyname == l_cRenderKey_LeadIn)
				{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fLeadIn.SetValue(value);}
			else if (keyname == l_cRenderKey_LeadOut)
				{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fLeadOut.SetValue(value);}
			else if (keyname == l_cRenderKey_Prefix)
				{std::string value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_Prefix.SetValue(value);}
			else if (keyname == l_cRenderKey_StartTime)
			{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fStartTime.SetValue(maTime::FromSeconds(value));}
			else if (keyname == l_cRenderKey_CaptureAllCameras)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bCaptureAllCameras.SetValue(value);}
			else if (keyname == l_cRenderKey_UseCameraName)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseCameraNameInFilename.SetValue(value);}
			else if (keyname == l_cRenderKey_UseSceneFilename)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseSceneFilenameInFilename.SetValue(value);}
			else if (keyname == l_cRenderKey_UseCompressionInName)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseCompressionInFilename.SetValue(value);}
			else if (keyname == l_cRenderKey_UseRenderPassInName)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseRenderPassInFilename.SetValue(value);}
			else if (keyname == l_cRenderKey_OutputDirectoryTags)
				{itString value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_OutputDirectoryTags.SetValue(value);}
			else if (keyname == l_cRenderKey_OutputDirectoryCustomize)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bOutputDirectoryCustomize.SetValue(value);}
			else if (keyname == l_cRenderKey_UseRenderDriverAsDir)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseRenderDriversAsUniqueCameras.SetValue(value);}
			else if (keyname == l_cRenderKey_UseCameraNameAsDir)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseCameraNameAsDirectory.SetValue(value);}
			else if (keyname == l_cRenderKey_UseResolutionAsDir)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseResolutionAsDirectory.SetValue(value);}
			else if (keyname == l_cRenderKey_UseSceneFilenameAsDir)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseSceneFilenameAsDirectory.SetValue(value);}
			else if (keyname == l_cRenderKey_OutputFileTags)
				{itString value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_OutputFileTags.SetValue(value);}
			else if (keyname == l_cRenderKey_OutputFileCustomize)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bOutputFileCustomize.SetValue(value);}
			else if (keyname == l_cRenderKey_Width)
				{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nWidth.SetValue(value);}
			else if (keyname == l_cRenderKey_Height)
				{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nHeight.SetValue(value);}
			else if (keyname == l_cRenderKey_TitleCard)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bOutputTitleCard.SetValue(value); /*DBG_LOG1("TitleCard set (%s)",(value?"true":"false"));*/}
			else if (keyname == l_cRenderKey_CounterDigits)
				{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nCounterDigits.SetValue(value);}
			//else if (keyname == l_cRenderKey_SubdivLevel)
			//	{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nSubdivLevel.SetValue(value);}
			else if (keyname == l_cRenderKey_Smoothing)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bSmoothing.SetValue(value);}
			else if (keyname == l_cRenderKey_AutoGenDir)
				{bool value; guiXMLTextReader::Convert(strvalue, value); /*o_Data.m_bAutoGenerateDirectory.SetValue(value);*/}
			else if (keyname == l_cRenderKey_KeepFrameOpen)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bKeepFrameOpenAfterRender.SetValue(value);}
			else if (keyname == l_cRenderKey_PostEmailNotify)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bSendPostEmailAddress.SetValue(value);}
			else if (keyname == l_cRenderKey_PostEmail)
				{std::string value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_PostEmailAddress.SetValue(value);}
			else if (keyname == l_cRenderKey_PostCommandToExecute)
				{std::string value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_PostCommand.SetValue(value);}
			else if (keyname == l_cRenderKey_ExecutePostCommand)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bExecutePostCommand.SetValue(value);}
			//else if (keyname == l_cRenderKey_ShowCaptureDialog)
			//	{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bShowCaptureSettingsDialog.SetValue(value);}
			else if (keyname == l_cRenderKey_UseLayerVisibility)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseLayerVisibility.SetValue(value);}
		}
	}

#ifdef DEMO_VERSION
	//	force these values
	o_Data.m_nWidth.SetValue( 852 );
	o_Data.m_nHeight.SetValue( 480 );
	o_Data.m_CaptureFormat.SetValue( "JPG" );				// frame or movie format
	o_Data.m_CompressCode.SetValue( "None" );
#endif

	guiXMLTextReader::Close();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void captRenderOutputObject::Read( captRenderOutputData& o_Data )
{
	//SetConfigFilename( o_Data.m_RenderOutputDataFile.GetValue(), o_Data.m_RenderOutputDataFile.GetValue() );
	ReadFromConfigFile( o_Data.m_RenderOutputDataFile.GetValue(), o_Data);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void captRenderOutputObject::Read(const fsLocator& i_ConfigFile,
									captRenderOutputData& o_Data )
{
	//SetConfigFilename( i_ConfigFile, i_ConfigFile );
	ReadFromConfigFile(i_ConfigFile, o_Data);
}


//------------------------------------------------------------------------
//	set the capture data to default settings
//------------------------------------------------------------------------
void captRenderOutputObject::SetToDefault(fsLocator& i_CurrentScene,
										  captRenderOutputData& o_Data)
{
	o_Data.m_bChunkData.SetValue(false);
	o_Data.m_bCaptureMovie.SetValue( true );
	o_Data.m_bBatchSkipDialog.SetValue(false);
	o_Data.m_bCannotOpen.SetValue(false);

#ifdef DEMO_VERSION
	o_Data.m_CaptureFormat.SetValue( "JPG" );				// frame or movie format
	o_Data.m_CompressCode.SetValue( "None" );
#else
#ifdef USE_QUICKTIME
	o_Data.m_CaptureFormat.SetValue( "MOV" );				// frame or movie format
	o_Data.m_CompressCode.SetValue( "jpeg" );
#else
	o_Data.m_CaptureFormat.SetValue( "AVI" );				// frame or movie format
	o_Data.m_CompressCode.SetValue( "RAW" );
#endif
#endif

	// This needs to match the window size set in WinMain.cpp
	// in non-managed code until the XML parsing is working:
#ifdef DEMO_VERSION
	o_Data.m_nWidth.SetValue( 852 );
	o_Data.m_nHeight.SetValue( 480 );
#else
	o_Data.m_nWidth.SetValue( 1280 );
	o_Data.m_nHeight.SetValue( 720 );
#endif
	o_Data.m_fCaptureFPS.SetValue( g3dConstants::c_fDefaultFrameRate );
	o_Data.m_nMotionSamplesPerFrame.SetValue( 1 );
	o_Data.m_nCaptureMax.SetValue( tmlnTimeLine::GetMaximum().AsFrame(tmlnTimeLine::GetFPS()) );
	o_Data.m_nCaptureSampling.SetValue( 1 );
	o_Data.m_bJitteredSampling.SetValue( true );
	o_Data.m_PixelAspect.SetValue(0);
	o_Data.m_FilterWidth.SetValue( 1.0f );
	o_Data.m_FilterFunc.SetValue( 0 );
	fsLocator empty;
	o_Data.m_SoundFile.SetValue(empty);
	o_Data.m_bDisplayTimeCode.SetValue( false );
	o_Data.m_fEndTime.SetValue( captRenderOutputData::c_InitEndTime );
	o_Data.m_fLeadIn.SetValue( 0.0f );
	o_Data.m_fLeadOut.SetValue( 0.0f );
	o_Data.m_fStartTime.SetValue( maTime::c_ZeroTime );
	o_Data.m_bCaptureAllCameras.SetValue( false );
	o_Data.m_bUseCameraNameInFilename.SetValue( true );
	o_Data.m_bUseLayerNameInFilename.SetValue( true );
	o_Data.m_bUseSceneFilenameInFilename.SetValue( true );
	o_Data.m_bUseCompressionInFilename.SetValue( true );
	o_Data.m_bUseRenderPassInFilename.SetValue( true );
	o_Data.m_bUseRenderDriversAsUniqueCameras.SetValue( true );
	o_Data.m_bUseCameraNameAsDirectory.SetValue( true );
	o_Data.m_bUseLayerNameAsDirectory.SetValue( true );
	o_Data.m_bUseRenderPassAsDirectory.SetValue( false );
	o_Data.m_bUseResolutionAsDirectory.SetValue( false );
	o_Data.m_bUseSceneFilenameAsDirectory.SetValue( true );
	o_Data.m_bOutputTitleCard.SetValue( false );
	o_Data.m_bMoviePerDriver.SetValue( true );
	o_Data.m_Prefix.SetValue( "scene" );
	o_Data.m_nCounterDigits.SetValue( 4 );
	o_Data.m_nCurrentFrame.SetValue( 0 );
	//o_Data.m_nSubdivLevel.SetValue( 3 );
	o_Data.m_bSmoothing.SetValue( true );
	o_Data.m_bShowRenderProgressDialog.SetValue( true );
	o_Data.m_RenderPosTime.SetValue(captRenderOutputData::c_InitRenderPosTime);
	o_Data.m_OutputFiles.ClearList();
	o_Data.m_bKeepFrameOpenAfterRender.SetValue(false);
	o_Data.m_bUseLayerVisibility.SetValue( false );
	//o_Data.m_bUseCaptureDrivers.SetValue( true );
	o_Data.m_RenderFrameRange.SetValue( captRenderOutputData::eCaptureDrivers );

	if(!cptrRenderUtil::GetPSflag())
		o_Data.m_OutputDirectoryRoot.SetValue( gfPaths::GetPath(mnmPaths::e_SaveFootage) );
	else 
		o_Data.m_OutputDirectoryRoot.SetValue( gfPaths::GetPath(mnmPaths::e_PhotoshopExport) );
	o_Data.m_OutputDirectory.SetValue( o_Data.m_OutputDirectoryRoot.GetValue() );
}



//--------------------------------------------------------------------
//--------------------------------------------------------------------
//void captRenderOutputObject::SetConfigFilename(const char* i_ConfigFile, const char* i_DefaultConfigFile)
//{
//	m_OutputFilename = i_ConfigFile;
//	m_OutputDefaultFilename = i_DefaultConfigFile;
//}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void captRenderOutputObject::RegisterProperties()
{
	prtyCheckBoxUIInfo* pCHUII;
	prtyPropertyUIInfo* pPUII;
	prtyTextBoxUIInfo* pTBII;
	prtyFloatEditUIInfo* pFEUII;
	prtyComboBoxUIInfo* pCBUII;
	prtyNumericUpDownUIInfo* pNUDUII;
	prtyListBoxUIInfo* pLBUII;
	prtyFileChooserUIInfo* pFCUII;
	prtyFolderChooserUIInfo* pFOUII;
	prtyButtonUIInfo* pBUII;
	std::ostringstream prop_ss(std::ostringstream::out);
	//	audio
	if( m_bIsMasterLayer )
	{
	//	pPUII = new prtyFileChooserUIInfo(&(m_Data.m_SoundFile), "Audio", "Sound file to attach to a movie only");
	//	AddProperty( pPUII );
	}

	if( !m_bIsLayerOutput )
	{
		//video output
		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_CaptureFormat), "Video Output", "Output movie format");

#ifdef DEMO_VERSION
		pCBUII->AddItem(std::string("JPG")); 
#else
// compiler flag to turn off quicktime
#ifdef USE_QUICKTIME
		pCBUII->AddItem(std::string("MOV"));
#endif
		pCBUII->AddItem(std::string("AVI"));
		pCBUII->AddItem(std::string("BMP"));
		pCBUII->AddItem(std::string("JPG")); 
		pCBUII->AddItem(std::string("TGA"));
		pCBUII->AddItem(std::string("PNG"));
		pCBUII->AddItem(std::string("DDS"));
#if (SGPU_APP != MS_CORE)
		pCBUII->AddItem(std::string("PPM"));
		pCBUII->AddItem(std::string("DIB"));
		pCBUII->AddItem(std::string("HDR"));
		pCBUII->AddItem(std::string("PFM"));
		pCBUII->AddItem(std::string("TIF"));
#ifdef USE_OPENEXR
		pCBUII->AddItem(std::string("EXR"));
#endif
#endif
#endif

		AddProperty( pCBUII );
		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_CompressCode), "Video Output", "Compression code for movie");
		m_pCompressCodeUII = pCBUII;
		AddProperty( pCBUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMoviePerDriver), "Video Output", "Create a movie for each capture driver");
		AddProperty( pPUII );

		//camera
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bCaptureAllCameras), "Cameras", "Capture all cameras");
		AddProperty( pPUII );
		pLBUII = new prtyListBoxUIInfo(&(m_Data.m_Cameras), "Cameras", "Camera List");
		pLBUII->m_bChecked = true;
		pLBUII->m_bOnlyOneSelected = false;
		m_pCamerasUIInfo = pLBUII;	//	store this for later update
		AddProperty( pLBUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseRenderDriversAsUniqueCameras), "Cameras", "Treat Capture Drivers as Unique Cameras");
		AddProperty( pPUII );

		//	Dimensions
		fsLocator cfgdir;
		//char tempstr[256];
		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_Resolution), "Dimensions", "Render output resolution");
		m_pResolutions = pCBUII;

#ifdef DEMO_VERSION
		pCBUII->AddItem(std::string("852x480"));
#else
		//	read in the configuration file for resolutions
		//
		cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
		cfgdir.Push( "Resolutions.cfg" );
		category_list_type resolutions(2, std::vector<std::string>(0));
		fsCategoryConfigFileUtil::SetCategoryTag(std::string("Category"));
		fsCategoryConfigFileUtil::SetElementTag(std::string("Resolution"));

		fsCategoryConfigFileUtil::ReadConfigFile( cfgdir, resolutions );

		pCBUII->AddItem(std::string("Custom Resolution"));

		for (int i=0; i < resolutions[0].size(); ++i)
		{
			prop_ss.str("");
			prop_ss << resolutions[1][i].c_str() << " [" << resolutions[0][i].c_str() << "]";
			//sprintf(tempstr,"%s [%s]", resolutions[1][i].c_str(), resolutions[0][i].c_str());
			pCBUII->AddItem(std::string(prop_ss.str()));
		}
#endif

		AddProperty( pCBUII );
		// Combobox for the aspect ratio
		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_PixelAspect), "Dimensions", "Pixel aspect ratio");
		pCBUII->AddItem(std::string("Square"), 0);
		pCBUII->AddItem(std::string("PAL (1.06667)"), 1);
		pCBUII->AddItem(std::string("NTSC (0.9)"), 2);
		AddProperty( pCBUII );

		pFEUII = new prtyFloatEditUIInfo(&(m_Data.m_nWidth), "Dimensions", "Render width");
		m_pRenderWidthUII = pFEUII;
		pFEUII->SetDecimalPlaces(0);
		pFEUII->SetReadOnly(true);
		AddProperty( pFEUII );
		pFEUII = new prtyFloatEditUIInfo(&(m_Data.m_nHeight), "Dimensions", "Render height");
		m_pRenderHeightUII = pFEUII;
		pFEUII->SetDecimalPlaces(0);
		pFEUII->SetReadOnly(true);
		AddProperty( pFEUII );
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_nCaptureSampling), "Sampling", "1 = No Anti-Aliasing, 2=4x AA, 3=9x AA, 4=16x AA");
		m_pCaptureSampling = pNUDUII;
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(8);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bJitteredSampling), "Sampling", "Jitter sampling (not for RenderMan)");
		AddProperty( pPUII );
		pFEUII = new prtyFloatEditUIInfo(&(m_Data.m_FilterWidth), "Sampling", "Filter width (RenderMan and jittered sampling)");
		pFEUII->SetDecimalPlaces(2);
		pFEUII->SetReadOnly(false);
		AddProperty( pFEUII );
		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_FilterFunc), "Sampling", "Filter function (RenderMan and jittered sampling)");
		AddProperty( pCBUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bHardwareAA), "Sampling", "Enable hardware antialiasing for rasterizer (not for RenderMan)");
		AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_ShadowQuality), "Shadows", "Global Shadow Quality");
		pCBUII->AddItem(std::string("No Override"), 0);
		pCBUII->AddItem(std::string("Low"), 1);
		pCBUII->AddItem(std::string("Medium"), 2);
		pCBUII->AddItem(std::string("High"), 3);
		pCBUII->AddItem(std::string("Very High"), 4);
		AddProperty( pCBUII );
		//	Filename
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseSceneFilenameInFilename), "Filename", "Use the scene filename in the output filename");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseCameraNameInFilename), "Filename", "Use the camera name in the output filename");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseLayerNameInFilename), "Filename", "Use the render layer name in the output filename");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseCompressionInFilename), "Filename", "Use the compression type in the output filename");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseRenderPassInFilename), "Filename", "Use the render pass in the output filename");
		AddProperty( pPUII );
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_nCounterDigits), "Filename", "Number of digits to display in the filename counter");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(10);
		AddProperty( pNUDUII );
		pCHUII = new prtyCheckBoxUIInfo(&(m_Data.m_bOutputFileCustomize), "Filename", "Customize the filename with tags");
		//pCHUII->SetReadOnly(true);
		AddProperty( pCHUII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_OutputFileTags), "Filename", "Output Filename built with tags");
		m_OutputFilenameTagsUI = pTBII;
		AddProperty( pTBII );

		//	Frames
		//pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_fCaptureFPS), "Frames", "Number of frames per second to capture");
		//cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
		//cfgdir.Push( "FramesPerSecond.cfg" );
		//category_list_type fps(2, std::vector<std::string>(0));
		//fsCategoryConfigFileUtil::SetElementTag(std::string("FPS"));
		//fsCategoryConfigFileUtil::ReadConfigFile( cfgdir, fps );
		//
		//add to list, the values for Frames per second		
		//for (int i=0; i < fps[0].size(); ++i)
		//{
		//	prop_ss.str("");
		//	prop_ss << fps[1][i];
		//	//sprintf(tempstr,"%s", fps[1][i].c_str());
		//	pCBUII->AddItem(std::string(prop_ss.str()));
		//}
		//AddProperty( pCBUII );

		//Motion samples per frame
		//pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_nMotionSamplesPerFrame), "Frames", "Number of motion samples per frame");
		//
		//cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
		//cfgdir.Push( "MotionSamplesPerFrame.cfg" );
		//category_list_type mspf(2, std::vector<std::string>(0));
		//
		//fsCategoryConfigFileUtil::SetElementTag(std::string("MSPF"));
		//fsCategoryConfigFileUtil::ReadConfigFile( cfgdir, mspf );
		//
		////add to list, the values for Motion Samples Per Frame	
		//for (int i=0; i < mspf[0].size(); ++i)
		//{
		//	prop_ss.str("");
		//	prop_ss << mspf[1][i];
		//	//sprintf(tempstr,"%s", mspf[1][i].c_str());
		//	pCBUII->AddItem(std::string(prop_ss.str()));
		//}
		//
		//AddProperty( pCBUII );

		//	Location
		pFOUII = new prtyFolderChooserUIInfo(&(m_Data.m_OutputDirectoryRoot), "Location", "Directory to output frames or movies");
		m_OutputDirectoryRootUI = pFOUII;
		AddProperty( pFOUII );
		pCHUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseSceneFilenameAsDirectory), "Location", "Use the scene filename as an output directory");
		m_bUseSceneFilenameAsDirectoryUI = pCHUII;
		AddProperty( pCHUII );
		pCHUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseCameraNameAsDirectory), "Location", "Use the camera name as an output directory (only used on non-movie renders)");
		m_bUseCameraNameAsDirectoryUI = pCHUII;
		AddProperty( pCHUII );
		pCHUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseLayerNameAsDirectory), "Location", "Use the render layer name as an output directory (only used on non-movie renders)");
		m_bUseLayerNameAsDirectoryUI = pCHUII;
		AddProperty( pCHUII );
		pCHUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseRenderPassAsDirectory), "Location", "Use the render pass name as an output directory (only used on non-movie renders)");
		m_bUseRenderPassAsDirectoryUI = pCHUII;
		AddProperty( pCHUII );
		pCHUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseResolutionAsDirectory), "Location", "Use the resolution as an output directory (only used on non-movie renders)");
		m_bUseResolutionAsDirectoryUI = pCHUII;
		AddProperty( pCHUII );
		pCHUII = new prtyCheckBoxUIInfo(&(m_Data.m_bOutputDirectoryCustomize), "Location", "Customize the directory with tags");
		//pCHUII->SetReadOnly(true);
		AddProperty( pCHUII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_OutputDirectoryTags), "Location", "Output Directory built with tags");
		m_OutputDirectoryTagsUI = pTBII;
		AddProperty( pTBII );

		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_BeautyPassName), "Pass Tags", "Beauty Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_DiffusePassName), "Pass Tags", "Diffuse Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_DiffEnvPassName), "Pass Tags", "Diffuse Environment Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_DiffLitPassName), "Pass Tags", "Diffuse Lights Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_SpecularPassName), "Pass Tags", "Specular Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_SpecEnvPassName), "Pass Tags", "Specular Environment Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_SpecLitPassName), "Pass Tags", "Specular Lights Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_EmissivePassName), "Pass Tags", "Emissive Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_AOOnlyPassName), "Pass Tags", "AO Only Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_GIPassName), "Pass Tags", "Global Illumination Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_DepthPassName), "Pass Tags", "Depth Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_ShadowMaskPassName), "Pass Tags", "Shadow Mask Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_IlluminationOnlyPassName), "Pass Tags", "Illumination Only Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_NormalsPassName), "Pass Tags", "Normals Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_DirtyMattePassName), "Pass Tags", "Dirty Matte Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_WireframePassName), "Pass Tags", "Wireframe Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_MaterialsPassName), "Pass Tags", "Materials Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_ReflectionsOnlyPassName), "Pass Tags", "Reflections Only Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_VelocityPassName), "Pass Tags", "Velocity Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_BloomPassName), "Pass Tags", "Bloom Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_StarPassName), "Pass Tags", "Star Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_CameraDOFPassName), "Pass Tags", "Camera DOF Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_PreviewPassName), "Pass Tags", "Preview Pass Name");
		AddProperty( pTBII );
		pTBII = new prtyTextBoxUIInfo(&(m_Data.m_GlowPassName), "Pass Tags", "Glow Pass Name");
		AddProperty( pTBII );

		//	Other
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bShowRenderProgressDialog), "Other", "Show the render progress dialog while rendering");
		AddProperty( pPUII );

		//	Padding
		pPUII = new prtyFloatEditUIInfo(&(m_Data.m_fLeadIn), "Padding", "Lead-in padding (hold on first frame)");
		AddProperty( pPUII );
		pPUII = new prtyFloatEditUIInfo(&(m_Data.m_fLeadOut), "Padding", "Lead-out padding (hold on last frame)");
		AddProperty( pPUII );

		//	Post
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bSendPostEmailAddress), "Post", "Send an email with render stats");
		AddProperty( pPUII );
		pPUII = new prtyTextBoxUIInfo(&(m_Data.m_PostEmailAddress), "Post", "Send email on post render");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bExecutePostCommand), "Post", "Execute a command after rendering");
		AddProperty( pPUII );
		pPUII = new prtyTextBoxUIInfo(&(m_Data.m_PostCommand), "Post", "Command to execute after rendering has completed");
		AddProperty( pPUII );

		//	Resume Point
		pCHUII = new prtyCheckBoxUIInfo(&(m_Data.m_bRenderPosUse), "Resume Point", "Use Resume Point");
		m_pRenderPosUseUII = pCHUII;
		AddProperty( pCHUII );
		pPUII = new prtyTextBoxUIInfo(&(m_Data.m_RenderPosScene), "Resume Point", "Render Saved Point - Scene");
		m_pRenderPosSceneUII = pPUII;
		pPUII->SetReadOnly(true);
		AddProperty( pPUII );
		pPUII = new prtyTextBoxUIInfo(&(m_Data.m_RenderPosCamera), "Resume Point", "Render Saved Point - Camera");
		m_pRenderPosCameraUII = pPUII;
		pPUII->SetReadOnly(true);
		AddProperty( pPUII );
		pPUII = new prtyTextBoxUIInfo(&(m_Data.m_RenderPosLayer), "Resume Point", "Render Saved Point - Render Layer");
		m_pRenderPosLayerUII = pPUII;
		pPUII->SetReadOnly(true);
		AddProperty( pPUII );
		pPUII = new tmlnTimeEditUIInfo(&(m_Data.m_RenderPosTime), "Resume Point", "Render Saved Point - Time");
		m_pRenderPosTimeUII = pPUII;
		pPUII->SetReadOnly(true);
		AddProperty( pPUII );
		pFCUII = new prtyFileChooserUIInfo(&(m_Data.m_RenderPosSaveFile), "Resume Point", "Render Saved Point - Save File");
		m_pRenderPosSaveFileUII = pFCUII;
		pFCUII->SetReadOnly(true);
		AddProperty( pFCUII );
		pBUII = new prtyButtonUIInfo(&(m_Data.m_tRenderPosDelete), "Resume Point", "Delete Resume Point");
		m_pRenderPosDeleteUII = pBUII;
		pBUII->SetText(std::string("Delete"));
		AddProperty( pBUII );
		//	Time
		//pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseCaptureDrivers), "Time", "Use capture drivers for rendering");
		//AddProperty( pPUII );
		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_RenderFrameRange), "Time", "Method for defining time range to render");
		AddProperty( pCBUII );
		pPUII = new tmlnTimeEditUIInfo(&(m_Data.m_fStartTime), "Time", "Time to start rendering");
		m_pStartTimeUII = pPUII;
		AddProperty( pPUII );
		pPUII = new tmlnTimeEditUIInfo(&(m_Data.m_fEndTime), "Time", "Time to end rendering");
		m_pEndTimeUII = pPUII;
		AddProperty( pPUII );

		//	Visual Output
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bDisplayTimeCode), "Visual Output", "Display the timecode on the rendered frames");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bOutputTitleCard), "Visual Output", "Output a title card before each capture block");
		AddProperty( pPUII );
		//pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_nSubdivLevel), "Visual Output", "Subdiv level");
		//pNUDUII->SetDecimalPlaces(0);
		//pNUDUII->SetMinimum(0);
		//pNUDUII->SetMaximum(3);
		//AddProperty( pNUDUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bSmoothing), "Visual Output", "Smooth subdivision surfaces");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseLayerVisibility), "Visual Output", "Use layer visibility flags instead of forcing all geometry visible");
		AddProperty( pPUII );

		//	not displayed properties, but need to be accessed by python
		//
		pPUII = new prtyPropertyUIInfo(&(m_Data.m_OutputFiles), "Hidden", "Output Files");
		AddProperty(pPUII);
		
		//	now add the callbacks
		m_Data.m_CaptureFormat.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::CaptureFormatChanged));
		m_Data.m_bCaptureAllCameras.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::AllCamerasChanged));
		//m_Data.m_bUseMarkerTimes.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::UseMarkerTimeChanged));
		//m_Data.m_bUseCaptureDrivers.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::UseCaptureDriversChanged));
		m_Data.m_RenderFrameRange.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::RenderFrameRangeChanged));
		m_Data.m_bRenderPosUse.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::RenderPosUseChanged));
		m_Data.m_tRenderPosDelete.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::RenderPosDeleteClicked));
		m_Data.m_Resolution.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::ResolutionClicked));
		m_Data.m_nWidth.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::ResolutionChanged));
		m_Data.m_nHeight.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::ResolutionChanged));
		m_Data.m_nCaptureSampling.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::SamplingChanged));
		//	filename callbacks
		m_Data.m_bOutputFileCustomize.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::OutputFileCustomizeClicked));
		m_Data.m_bUseSceneFilenameInFilename.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::UseSceneFilenameInFilenameClicked));
		m_Data.m_bUseCameraNameInFilename.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::UseCameraNameInFilenameClicked));
		m_Data.m_bUseLayerNameInFilename.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::UseLayerNameInFilenameClicked));
		m_Data.m_bUseCompressionInFilename.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::UseCompressionInFilenameClicked));
		m_Data.m_bUseRenderPassInFilename.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::UseRenderPassInFilenameClicked));
		m_Data.m_nCounterDigits.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::CounterDigitsClicked));
		//	directory callbacks
		m_Data.m_bOutputDirectoryCustomize.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::OutputDirectoryCustomizeClicked));
		m_Data.m_OutputDirectoryRoot.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::OutputDirectoryRootChanged));
		m_Data.m_bUseSceneFilenameAsDirectory.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::UseSceneFilenameAsDirectoryClicked));
		m_Data.m_bUseCameraNameAsDirectory.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::UseCameraNameAsDirectoryClicked));
		m_Data.m_bUseLayerNameAsDirectory.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::UseLayerNameAsDirectoryClicked));
		m_Data.m_bUseRenderPassAsDirectory.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::UseRenderPassAsDirectoryClicked));
		m_Data.m_bUseResolutionAsDirectory.AddCallback(new prtyCallbackWrapper<captRenderOutputObject>(this, &captRenderOutputObject::UseResolutionAsDirectoryClicked));
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::update_compresscodeUII( const std::string& i_CaptureType )
{
#ifndef DEMO_VERSION
	// TODO - codes commented out generated empty or bad files

	if (i_CaptureType == std::string("AVI"))
	{
		m_Data.m_bCaptureMovie.SetValue(true);
		m_pCompressCodeUII->ClearItems();

	#if (SGPU_APP != MS_CORE)
		m_pCompressCodeUII->AddItem(std::string("CVID"));	// cinepak
		m_pCompressCodeUII->AddItem(std::string("DIB"));
	#endif
		m_pCompressCodeUII->AddItem(std::string("RAW"));
		m_pCompressCodeUII->AddItem(std::string("RGB"));

		//	None of these work
	#if (SGPU_APP != MS_CORE)
		//m_pCompressCodeUII->AddItem(std::string("CRAM"));	// Microsoft Video 1
		//m_pCompressCodeUII->AddItem(std::string("IV31"));	// Intel Indeo 3.1
		//m_pCompressCodeUII->AddItem(std::string("IV32"));
		//m_pCompressCodeUII->AddItem(std::string("IV40"));
		//m_pCompressCodeUII->AddItem(std::string("IV41"));
		//m_pCompressCodeUII->AddItem(std::string("IV50"));
		//m_pCompressCodeUII->AddItem(std::string("IV51"));
		//m_pCompressCodeUII->AddItem(std::string("MRLE"));
		//m_pCompressCodeUII->AddItem(std::string("MJPG"));
		//m_pCompressCodeUII->AddItem(std::string("VDOM"));
		//m_pCompressCodeUII->AddItem(std::string("MPG4"));
		//m_pCompressCodeUII->AddItem(std::string("MP42"));
		//m_pCompressCodeUII->AddItem(std::string("MP43"));
		//m_pCompressCodeUII->AddItem(std::string("X263"));
		//m_pCompressCodeUII->AddItem(std::string("M263"));
		//m_pCompressCodeUII->AddItem(std::string("I263"));
		//m_pCompressCodeUII->AddItem(std::string("X264"));
		//m_pCompressCodeUII->AddItem(std::string("M264"));
		//m_pCompressCodeUII->AddItem(std::string("I264"));
		//m_pCompressCodeUII->AddItem(std::string("DIV4"));
	#endif

		m_pCompressCodeUII->UpdateControl();
	}
	else if (i_CaptureType == std::string("MOV"))
	{
		m_Data.m_bCaptureMovie.SetValue(true);
		m_pCompressCodeUII->ClearItems();
		m_pCompressCodeUII->AddItem(std::string("raw "));
		m_pCompressCodeUII->AddItem(std::string("jpeg"));
	#if (SGPU_APP != MS_CORE)
		//m_pCompressCodeUII->AddItem(std::string("apch"));	// prores
		m_pCompressCodeUII->AddItem(std::string("cvid"));
		m_pCompressCodeUII->AddItem(std::string("smc "));
	#endif
		m_pCompressCodeUII->AddItem(std::string("rle "));
	#if (SGPU_APP != MS_CORE)
		m_pCompressCodeUII->AddItem(std::string("rpza"));
		m_pCompressCodeUII->AddItem(std::string("yuv2"));
		m_pCompressCodeUII->AddItem(std::string("mjpa"));
		m_pCompressCodeUII->AddItem(std::string("mjpb"));
		//m_pCompressCodeUII->AddItem(std::string(".SGI"));
		m_pCompressCodeUII->AddItem(std::string("8BPS"));
		//m_pCompressCodeUII->AddItem(std::string("PNTG"));
		//m_pCompressCodeUII->AddItem(std::string("gif "));
		//m_pCompressCodeUII->AddItem(std::string("kpcd"));
		//m_pCompressCodeUII->AddItem(std::string("qdgx"));
		//m_pCompressCodeUII->AddItem(std::string("avr "));
		//m_pCompressCodeUII->AddItem(std::string("dmb1"));
		m_pCompressCodeUII->AddItem(std::string("WRLE"));
		//m_pCompressCodeUII->AddItem(std::string("WRAW"));
		//m_pCompressCodeUII->AddItem(std::string("path"));
		//m_pCompressCodeUII->AddItem(std::string("qdrw"));
		//m_pCompressCodeUII->AddItem(std::string("ripl"));
		//m_pCompressCodeUII->AddItem(std::string("fire"));
		//m_pCompressCodeUII->AddItem(std::string("clou"));
		m_pCompressCodeUII->AddItem(std::string("h261"));
	#endif
		m_pCompressCodeUII->AddItem(std::string("h263"));
	#if (SGPU_APP != MS_CORE)
		m_pCompressCodeUII->AddItem(std::string("dvc "));
		m_pCompressCodeUII->AddItem(std::string("dvcp"));
		m_pCompressCodeUII->AddItem(std::string("dvpp"));
		//m_pCompressCodeUII->AddItem(std::string("dv5n"));
		//m_pCompressCodeUII->AddItem(std::string("dv5p"));
		//m_pCompressCodeUII->AddItem(std::string("dv1n"));
		//m_pCompressCodeUII->AddItem(std::string("dv1p"));
		//m_pCompressCodeUII->AddItem(std::string("dvhp"));
		//m_pCompressCodeUII->AddItem(std::string("dvh6"));
		//m_pCompressCodeUII->AddItem(std::string("dvh5"));
		//m_pCompressCodeUII->AddItem(std::string("base"));
		//m_pCompressCodeUII->AddItem(std::string("flic"));
		m_pCompressCodeUII->AddItem(std::string("tga "));
		m_pCompressCodeUII->AddItem(std::string("png "));
		m_pCompressCodeUII->AddItem(std::string("tiff"));
		//m_pCompressCodeUII->AddItem(std::string("yuvu"));
		//m_pCompressCodeUII->AddItem(std::string("yuvs"));
		//m_pCompressCodeUII->AddItem(std::string("cmyk"));
		//m_pCompressCodeUII->AddItem(std::string("msvc"));
		m_pCompressCodeUII->AddItem(std::string("SVQ1"));
		m_pCompressCodeUII->AddItem(std::string("SVQ3"));
		//m_pCompressCodeUII->AddItem(std::string("IV41"));
	#endif
		m_pCompressCodeUII->AddItem(std::string("mp4v"));
	#if (SGPU_APP != MS_CORE)
		//m_pCompressCodeUII->AddItem(std::string("b64a"));
		//m_pCompressCodeUII->AddItem(std::string("b48r"));
		//m_pCompressCodeUII->AddItem(std::string("b32a"));
		//m_pCompressCodeUII->AddItem(std::string("b16g"));
		//m_pCompressCodeUII->AddItem(std::string("myuv"));
		//m_pCompressCodeUII->AddItem(std::string("y420"));
		//m_pCompressCodeUII->AddItem(std::string("syv9"));
		//m_pCompressCodeUII->AddItem(std::string("2vuy"));
		//m_pCompressCodeUII->AddItem(std::string("v308"));
		//m_pCompressCodeUII->AddItem(std::string("v408"));
		//m_pCompressCodeUII->AddItem(std::string("v216"));
		//m_pCompressCodeUII->AddItem(std::string("v210"));
		//m_pCompressCodeUII->AddItem(std::string("v410"));
		//m_pCompressCodeUII->AddItem(std::string("r408"));
		m_pCompressCodeUII->AddItem(std::string("mjp2"));
		//m_pCompressCodeUII->AddItem(std::string("pxlt"));
		m_pCompressCodeUII->AddItem(std::string("avc1"));
	#endif
		m_pCompressCodeUII->UpdateControl();
	}
	else
#endif
	{
		m_Data.m_bCaptureMovie.SetValue(false);
		m_pCompressCodeUII->ClearItems();
		m_pCompressCodeUII->AddItem(std::string("None"));
		m_pCompressCodeUII->UpdateControl();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::GetDefaultCompressCode(const std::string& i_CaptureType)
{
	m_Data.m_CompressCode.SetValue("");
	if (i_CaptureType == std::string("AVI"))
		m_Data.m_CompressCode.SetValue("RAW");
	else if (i_CaptureType == std::string("MOV"))
		m_Data.m_CompressCode.SetValue("jpeg");
	else
		m_Data.m_CompressCode.SetValue("None");
}



//==========================================================================
// Callbacks for when properties change, updates member data
//==========================================================================

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::AllCamerasChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (m_Data.m_bCaptureAllCameras.GetValue())
	{
		//	need to disable the cameras listbox
		m_pCamerasUIInfo->SetReadOnly(true);
		m_pCamerasUIInfo->UpdateControl();
	}
	else
	{
		//	need to enable the cameras listbox
		m_pCamerasUIInfo->SetReadOnly(false);
		m_pCamerasUIInfo->UpdateControl();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::CaptureFormatChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	prtyText* ptxt = static_cast<prtyText*>(i_pProperty);

	if (ptxt != NULL)
	{
		update_compresscodeUII( ptxt->GetValue() );
		GetDefaultCompressCode( ptxt->GetValue() );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::UseMarkerTimeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	/*if (m_Data.m_bUseMarkerTimes.GetValue())
	{
		m_pStartTimeUII->SetReadOnly(true);
		m_pStartTimeUII->UpdateControl();
		m_pEndTimeUII->SetReadOnly(true);
		m_pEndTimeUII->UpdateControl();
	}
	else
	{
		m_pStartTimeUII->SetReadOnly(false);
		m_pStartTimeUII->UpdateControl();
		m_pEndTimeUII->SetReadOnly(false);
		m_pEndTimeUII->UpdateControl();
	}*/
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//void captRenderOutputObject::UseCaptureDriversChanged(prtyProperty *i_pProperty, bool i_bDirty)
//{
//	if (m_Data.m_bUseCaptureDrivers.GetValue())
//	{
//		m_pStartTimeUII->SetReadOnly(true);
//		m_pStartTimeUII->UpdateControl();
//		m_pEndTimeUII->SetReadOnly(true);
//		m_pEndTimeUII->UpdateControl();
//	}
//	else
//	{
//		m_pStartTimeUII->SetReadOnly(false);
//		m_pStartTimeUII->UpdateControl();
//		m_pEndTimeUII->SetReadOnly(false);
//		m_pEndTimeUII->UpdateControl();
//	}
//}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::RenderFrameRangeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	bool bEnableRangeControls = (m_Data.m_RenderFrameRange.GetValue() == captRenderOutputData::eSpecifyRange);
	m_pStartTimeUII->SetReadOnly(!bEnableRangeControls);
	m_pStartTimeUII->UpdateControl();
	m_pEndTimeUII->SetReadOnly(!bEnableRangeControls);
	m_pEndTimeUII->UpdateControl();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::RenderPosUseChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	//if (m_Data.m_bRenderPosUse.GetValue())
	//{
		m_pRenderPosSceneUII->SetReadOnly(true);
		m_pRenderPosSceneUII->UpdateControl();
		m_pRenderPosCameraUII->SetReadOnly(true);
		m_pRenderPosCameraUII->UpdateControl();

		m_pRenderPosLayerUII->SetReadOnly(true);
		m_pRenderPosLayerUII->UpdateControl();

		m_pRenderPosTimeUII->SetReadOnly(true);
		m_pRenderPosTimeUII->UpdateControl();
		m_pRenderPosSaveFileUII->SetReadOnly(true);
		m_pRenderPosSaveFileUII->UpdateControl();
		
		m_pRenderPosDeleteUII->SetReadOnly(false);
		m_pRenderPosDeleteUII->UpdateControl();
		
	//}
	//else
	//{
	//	m_pRenderPosSceneUII->SetReadOnly(false);
	//	m_pRenderPosSceneUII->UpdateControl();
	//	m_pRenderPosCameraUII->SetReadOnly(false);
	//	m_pRenderPosCameraUII->UpdateControl();
	//	m_pRenderPosTimeUII->SetReadOnly(false);
	//	m_pRenderPosTimeUII->UpdateControl();
	//	m_pRenderPosSaveFileUII->SetReadOnly(false);
	//	m_pRenderPosSaveFileUII->UpdateControl();
	//}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::RenderPosDeleteClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	//cptrRenderStateUtil::DeleteRenderPosition();

	m_pRenderPosDeleteUII->SetReadOnly(true);
	m_pRenderPosDeleteUII->UpdateControl();

	m_Data.m_RenderPosSaveFile.SetValue(fsLocator());
	//m_pRenderPosSaveFileUII->SetReadOnly(true);
	m_pRenderPosSaveFileUII->UpdateControl();

	m_Data.m_bRenderPosUse.SetValue(false);
	m_pRenderPosUseUII->SetReadOnly(true);
	m_pRenderPosUseUII->UpdateControl();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::ResolutionClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	if ( (m_Data.m_Resolution.GetValue().size() == 0)
		||
		 (m_Data.m_Resolution.GetValue() == std::string("Custom Resolution")))
	{
		//m_Data.m_nWidth.SetValue(0);
		//m_Data.m_nHeight.SetValue(0);

		m_pRenderWidthUII->SetReadOnly(false);
		m_pRenderWidthUII->UpdateControl();
		m_pRenderHeightUII->SetReadOnly(false);
		m_pRenderHeightUII->UpdateControl();
	}
	else
	{
		int w,h;
		sscanf( m_Data.m_Resolution.GetValue().c_str(),"%dx%d", &w, &h );
		m_Data.m_nWidth.SetValueWithoutNotify(w);
		m_Data.m_nHeight.SetValueWithoutNotify(h);

		m_pRenderWidthUII->SetReadOnly(true);
		m_pRenderWidthUII->UpdateControl();
		m_pRenderHeightUII->SetReadOnly(true);
		m_pRenderHeightUII->UpdateControl();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::ResolutionChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	int w, h, res_w, res_h;
	w = m_Data.m_nWidth.GetValue();
	h = m_Data.m_nHeight.GetValue();
	
	//if the width and height are already matched with the resolution string, return
	sscanf( m_Data.m_Resolution.GetValue().c_str(),"%dx%d", &res_w, &res_h );
	if( (res_w == w) && (res_h == h) )
		return;

	//if not, find the match in the resolution list
	for( int i = 0; i < m_pResolutions->m_List.size(); ++i )
	{
		sscanf( m_pResolutions->m_List[i].c_str(), "%dx%d", &res_w, &res_h );
		if( (res_w == w) && (res_h == h) )
		{
			m_Data.m_Resolution.SetValueWithoutNotify( m_pResolutions->m_List[i] );
			m_pResolutions->UpdateControl();
			return;
		}
	}

	//no match, show the Custom Resolution string
	m_Data.m_Resolution.SetValueWithoutNotify( std::string("Custom Resolution") );
	m_pRenderWidthUII->SetReadOnly(false);
	m_pRenderWidthUII->UpdateControl();
	m_pRenderHeightUII->SetReadOnly(false);
	m_pRenderHeightUII->UpdateControl();
	m_pResolutions->UpdateControl();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::SamplingChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	if( m_Data.m_nCaptureSampling.GetValue() <= 0 )
		m_Data.m_nCaptureSampling.SetValueWithoutNotify(1);
	else if( m_Data.m_nCaptureSampling.GetValue() > 8 )
		m_Data.m_nCaptureSampling.SetValueWithoutNotify(8);

	m_pCaptureSampling->UpdateControl();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::OutputFileCustomizeClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (m_Data.m_bOutputFileCustomize == true)
		m_OutputFilenameTagsUI->SetReadOnly( false );
	else
		m_OutputFilenameTagsUI->SetReadOnly( true );
	m_OutputFilenameTagsUI->UpdateControl();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void captRenderOutputObject::UseSceneFilenameInFilenameClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	captRenderOutputTagsUtil::BuildTagsForFile( m_Data );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void captRenderOutputObject::UseCameraNameInFilenameClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	captRenderOutputTagsUtil::BuildTagsForFile( m_Data );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void captRenderOutputObject::UseLayerNameInFilenameClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	captRenderOutputTagsUtil::BuildTagsForFile( m_Data );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void captRenderOutputObject::UseCompressionInFilenameClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	captRenderOutputTagsUtil::BuildTagsForFile( m_Data );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void captRenderOutputObject::UseRenderPassInFilenameClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	captRenderOutputTagsUtil::BuildTagsForFile( m_Data );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void captRenderOutputObject::CounterDigitsClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	captRenderOutputTagsUtil::BuildTagsForFile( m_Data );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::OutputDirectoryCustomizeClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (m_Data.m_bOutputDirectoryCustomize == true)
		m_OutputDirectoryTagsUI->SetReadOnly( false );
	else
		m_OutputDirectoryTagsUI->SetReadOnly( true );
	m_OutputDirectoryTagsUI->UpdateControl();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::OutputDirectoryRootChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	DBG_TRACE("User set output directory to " << m_Data.m_OutputDirectoryRoot.GetValue());

	captRenderOutputTagsUtil::BuildTagsForDirectory( m_Data );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::UseSceneFilenameAsDirectoryClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	captRenderOutputTagsUtil::BuildTagsForDirectory( m_Data );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::UseCameraNameAsDirectoryClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	captRenderOutputTagsUtil::BuildTagsForDirectory( m_Data );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::UseLayerNameAsDirectoryClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	captRenderOutputTagsUtil::BuildTagsForDirectory( m_Data );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::UseRenderPassAsDirectoryClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	captRenderOutputTagsUtil::BuildTagsForDirectory( m_Data );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void captRenderOutputObject::UseResolutionAsDirectoryClicked(prtyProperty *i_pProperty, bool i_bDirty)
{
	captRenderOutputTagsUtil::BuildTagsForDirectory( m_Data );
}

