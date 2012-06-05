/*****************************************************************************
**	cptrRenderOutputDataUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"

#include "Features/Capture/cptrCategoryConfigFileUtil.hpp"
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
//#include "Core/name/nameString.hpp"

//	library
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Graphics/G3d/g3dConstants.hpp"
#include "Graphics/G3d/g3dSingleLightRendering.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"
#include "Core/prty/prtyButtonUIInfo.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyFolderChooserUIInfo.hpp"
#include "Core/prty/prtyListBoxUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Support/pyth/pythProperty.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#include "Support/tmln/tmlnTimeline.hpp"


//============================================================================
//============================================================================
namespace cptrRenderOutputDataUtil
{
	namespace
	{
		const char* l_cRenderKey_MainKey = "RenderOutput";
		const char* l_cRenderKey_CaptureMovie = "CaptureMovie";
		const char* l_cRenderKey_UseMarkers = "UseMarkers";
		const char* l_cRenderKey_MarkerInTime = "MarkerInTime";
		const char* l_cRenderKey_MarkerOutTime= "MarkerOutTime";
		const char* l_cRenderKey_CaptureFPS = "CaptureFPS";
		const char* l_cRenderKey_MotionSamplesPerFrame = "MotionSamplesPerFrame";
		const char* l_cRenderKey_CaptureMax = "CaptureMax";
		const char* l_cRenderKey_Sampling = "Sampling";
		const char* l_cRenderKey_JitteredSampling = "JitteredSampling";
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
		const char* l_cRenderKey_UseRenderDriverAsDir = "UseRenderDriverAsDir";
		const char* l_cRenderKey_UseCameraNameAsDir = "UseCameraNameAsDir";
		const char* l_cRenderKey_UseResolutionAsDir = "UseResolutionAsDir";
		const char* l_cRenderKey_UseSceneFilenameAsDir = "UseSceneFilenameAsDir";
		const char* l_cRenderKey_Width = "Width";
		const char* l_cRenderKey_Height = "Height";
		const char* l_cRenderKey_TitleCard = "TitleCard";
		const char* l_cRenderKey_CounterDigits = "CounterDigits";
		const char* l_cRenderKey_SubdivLevel = "SubdivLevel";
		const char* l_cRenderKey_PostEmailNotify = "PostEmailNotify";
		const char* l_cRenderKey_PostEmail = "PostEmail";
		const char* l_cRenderKey_PostCommandToExecute = "PostCommandToExecute";
		const char* l_cRenderKey_ExecutePostCommand = "ExecutePostCommand";
		const char* l_cRenderKey_AutoGenDir= "AutoGenDir";
		const char* l_cRenderKey_KeepFrameOpen = "KeepFrameOpen";
		const char* l_cRenderKey_ShowCaptureDialog = "ShowCaptureDialog";
		const char* l_cRenderKey_ShowRenderProgressDialog = "ShowRenderProgressDialog";
		const char* l_cRenderKey_MoviePerDriver = "MoviePerDriver";

		bool l_bShadowEditorState = false;
		bool l_bShadowQualityState = false;

		//==========================================================================
		//
		//	Render Output base object
		//
		//	[rjk] should this be in its own file?
		//
		//==========================================================================
		class cptrRenderOutputObject : public prtyObject
		{
			public:
				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				cptrRenderOutputObject();

				//------------------------------------------------------------------------
				//	called right before showing the dialog
				//------------------------------------------------------------------------
				void SetupControls( bool i_bBatchMode );

				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				void UpdateCameraList( cptrRenderOutputData& i_NewData );

				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				void Read( cptrRenderOutputData& o_Data );
				void Read(const fsLocator& i_ConfigFile,
							cptrRenderOutputData& o_Data );

				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				void Write( const cptrRenderOutputData& i_Data );
				void Write( const fsLocator& i_ConfigFile,
							const cptrRenderOutputData& i_Data );

			protected:
				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				//void SetConfigFilename(const char* i_ConfigFile, const char*i_DefaultConfigFile);

				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				void ReadFromConfigFile(const fsLocator& i_ConfigFile,
										cptrRenderOutputData& o_Data );

				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				// 4-30-08 PSM
				//--------------------------------------------------------------------
				void ReadFromXMLConfig(const char *xmlDocument,
										cptrRenderOutputData& o_Data );

				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				void WriteToConfigFile( const fsLocator& i_ConfigFile,
										const cptrRenderOutputData& i_Data );

				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				void SetToDefault(fsLocator& i_CurrentScene, 
											cptrRenderOutputData& o_Data);

			private:
				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				void RegisterProperties();

				//--------------------------------------------------------------------
				// Callbacks for when properties change, updates member data
				//--------------------------------------------------------------------
				void UpdateData(prtyProperty *i_pProperty, bool i_bDirty);

				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				void update_compresscodeUII( const std::string& i_CaptureType );

				//--------------------------------------------------------------------
				// Callbacks for when properties change, updates member data
				//--------------------------------------------------------------------
				void AllCamerasChanged(prtyProperty *i_pProperty, bool i_bDirty);
				void CaptureFormatChanged(prtyProperty *i_pProperty, bool i_bDirty);
				void UseMarkerTimeChanged(prtyProperty *i_pProperty, bool i_bDirty);
				void RenderPosUseChanged(prtyProperty *i_pProperty, bool i_bDirty);
				void RenderPosDeleteClicked(prtyProperty *i_pProperty, bool i_bDirty);
				void ResolutionClicked(prtyProperty *i_pProperty, bool i_bDirty);

			public:
				cptrRenderOutputData m_Data;

			private:
				//std::string m_OutputFilename;
				//std::string m_OutputDefaultFilename;

			private:
				// Need to keep track of these UI Infos because we need to
				// adjust them after the constructor
				prtyListBoxUIInfo *m_pCamerasUIInfo;
				prtyComboBoxUIInfo *m_pCompressCodeUII;
				prtyPropertyUIInfo *m_pStartTimeUII;
				prtyPropertyUIInfo *m_pEndTimeUII;
				prtyPropertyUIInfo *m_pRenderPosSceneUII;
				prtyPropertyUIInfo *m_pRenderPosCameraUII;
				prtyPropertyUIInfo *m_pRenderPosTimeUII;
				prtyFileChooserUIInfo *m_pRenderPosSaveFileUII;
				prtyButtonUIInfo *m_pRenderPosDeleteUII;
				prtyCheckBoxUIInfo *m_pRenderPosUseUII;
				prtyFloatEditUIInfo *m_pRenderWidthUII;
				prtyFloatEditUIInfo *m_pRenderHeightUII;
				prtyComboBoxUIInfo* m_pResolutions;
		};

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		cptrRenderOutputObject::cptrRenderOutputObject()
		{
			fsLocator curscene;
			SetToDefault(curscene, m_Data);

			RegisterProperties();
		}

		//------------------------------------------------------------------------
		//	called right before showing the dialog
		//------------------------------------------------------------------------
		void cptrRenderOutputObject::SetupControls( bool i_bBatchMode )
		{
			update_compresscodeUII( m_Data.m_CaptureFormat.GetValue() );

			//	set controls based on batch mode
			//
			m_pStartTimeUII->SetReadOnly( i_bBatchMode );
			m_pEndTimeUII->SetReadOnly( i_bBatchMode );

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
		void cptrRenderOutputObject::UpdateCameraList( cptrRenderOutputData& i_NewData )
		{
			m_Data.m_CameraList = i_NewData.m_CameraList;
			m_pCamerasUIInfo->m_List.clear();
			//DBG_LOG0("\nUPDATE CAMERA LIST");
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
		void cptrRenderOutputObject::WriteToConfigFile( const fsLocator& i_ConfigFile,
														const cptrRenderOutputData& i_Data )
		{
			//	if the code is in the middle of reading the file, don't allow writing.
			//if (l_bReadingData)
			//	return;

			fsLocator cfgdir = i_ConfigFile;
			std::string cfgpath;
			fsFileUtil::LocatorToANSIFilename(cfgdir,cfgpath);
			DBG_LOG1("Writing config file (%s)", cfgpath.c_str());

			//	quick and dirty XML Writer
			guiXMLTextWriter::Open(cfgpath.c_str());
			guiXMLTextWriter::WriteStartElement(l_cRenderKey_MainKey);

			//	write the actual values
			guiXMLTextWriter::WriteElement(l_cRenderKey_CaptureMovie, i_Data.m_bCaptureMovie.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_MoviePerDriver, i_Data.m_bMoviePerDriver.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_ShowRenderProgressDialog, i_Data.m_bShowRenderProgressDialog.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_UseMarkers, i_Data.m_bUseMarkerTimes.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_MarkerInTime, i_Data.m_fMarkerInTime.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_MarkerOutTime, i_Data.m_fMarkerOutTime.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_CaptureFPS, i_Data.m_fCaptureFPS.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_MotionSamplesPerFrame, i_Data.m_nMotionSamplesPerFrame.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_CaptureMax, i_Data.m_nCaptureMax.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_Sampling, i_Data.m_nCaptureSampling.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_JitteredSampling, i_Data.m_bJitteredSampling.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_CaptureFormat, i_Data.m_CaptureFormat.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_CompressCode, i_Data.m_CompressCode.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_Directory, i_Data.m_OutputDirectory.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_SoundFile, i_Data.m_SoundFile.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_DisplayTimeCode, i_Data.m_bDisplayTimeCode.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_EndTime, i_Data.m_fEndTime.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_LeadIn, i_Data.m_fLeadIn.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_LeadOut, i_Data.m_fLeadOut.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_Prefix, i_Data.m_Prefix.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_StartTime, i_Data.m_fStartTime.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_CaptureAllCameras, i_Data.m_bCaptureAllCameras.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_UseCameraName, i_Data.m_bUseCameraNameInFilename.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_UseSceneFilename, i_Data.m_bUseSceneFilenameInFilename.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_UseCompressionInName, i_Data.m_bUseCompressionInFilename.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_UseRenderDriverAsDir, i_Data.m_bUseRenderDriverAsDirAndFname.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_UseCameraNameAsDir, i_Data.m_bUseCameraNameAsDirectory.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_UseResolutionAsDir, i_Data.m_bUseResolutionAsDirectory.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_UseSceneFilenameAsDir, i_Data.m_bUseSceneFilenameAsDirectory.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_Width, i_Data.m_nWidth.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_Height, i_Data.m_nHeight.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_TitleCard, i_Data.m_bOutputTitleCard.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_CounterDigits, i_Data.m_nCounterDigits.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_SubdivLevel, i_Data.m_nSubdivLevel.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_AutoGenDir, i_Data.m_bAutoGenerateDirectory.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_KeepFrameOpen, i_Data.m_bKeepFrameOpenAfterRender.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_PostEmailNotify, i_Data.m_bSendPostEmailAddress.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_PostEmail, i_Data.m_PostEmailAddress.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_PostCommandToExecute, i_Data.m_PostCommand.GetValue());
			guiXMLTextWriter::WriteElement(l_cRenderKey_ExecutePostCommand, i_Data.m_bExecutePostCommand.GetValue());

			//	finish it up
			guiXMLTextWriter::WriteEndElement();
			guiXMLTextWriter::Close();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void cptrRenderOutputObject::Write( const cptrRenderOutputData& i_Data )
		{
			WriteToConfigFile(i_Data.m_RenderOutputDataFile.GetValue(), i_Data);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void cptrRenderOutputObject::Write( const fsLocator& i_ConfigFile,
											const cptrRenderOutputData& i_Data )
		{
			WriteToConfigFile(i_ConfigFile, i_Data);
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		// 4-30-08 PSM
		//------------------------------------------------------------------------
		void cptrRenderOutputObject::ReadFromXMLConfig(const char *xmlDocument,
														cptrRenderOutputData& o_Data )
		{
#if 0
		try {
            XMLPlatformUtils::Initialize();
        }
        catch (const XMLException& toCatch) {
            char* message = XMLString::transcode(toCatch.getMessage());

			DBG_LOG1("Error during initialization! : %s\n", message);

			XMLString::release(&message);

			return;
        }

        XercesDOMParser* parser = new XercesDOMParser();
        parser->setValidationScheme(XercesDOMParser::Val_Always);    
        parser->setDoNamespaces(true);    // optional

        ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
        parser->setErrorHandler(errHandler);

        char* xmlFile = "x1.xml";

        try {
            parser->parse(xmlDocument);
        }
        catch (const XMLException& toCatch) {
            char* message = XMLString::transcode(toCatch.getMessage());
			DBG_LOG1("XMLException! : %s\n", message);

            return;
        }
        catch (const DOMException& toCatch) {
            char* message = XMLString::transcode(toCatch.msg);
			DBG_LOG1("DOMException! : %s\n", message);

            return;
        }
        catch (...) {
			DBG_LOG1("unexpected exception!\n","");

            return;
        }


		DOMNode                     *doc = parser->getDocument();


#if 0
		XMLPScanToken token;

		if (!parser.parseFirst(xmlFile, token))
		{
			cerr << "scanFirst() failed\n" << endl;
			return 1;
		}

		bool gotMore = true;
		while (gotMore && !handler.getDone()) {
			gotMore = parser.parseNext(token);
		}
#endif


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
					else if (keyname == l_cRenderKey_UseMarkers)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseMarkerTimes.SetValue(value);}
					else if (keyname == l_cRenderKey_MarkerInTime)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fMarkerInTime.SetValue(value);}
					else if (keyname == l_cRenderKey_MarkerOutTime)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fMarkerOutTime.SetValue(value);}
					else if (keyname == l_cRenderKey_CaptureFPS)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fCaptureFPS.SetValue(value);}
					else if (keyname == l_cRenderKey_MotionSamplesPerFrame)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nMotionSamplesPerFrame.SetValue(value);}
					else if (keyname == l_cRenderKey_CaptureMax)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nCaptureMax.SetValue(value);}
					else if (keyname == l_cRenderKey_Sampling)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nCaptureSampling.SetValue(value);}
					else if (keyname == l_cRenderKey_JitteredSampling)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bJitteredSampling.SetValue(value);}
					else if (keyname == l_cRenderKey_CaptureFormat)
						{std::string value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_CaptureFormat.SetValue(value);}
					else if (keyname == l_cRenderKey_CompressCode)
						{std::string value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_CompressCode.SetValue(value);}
					else if (keyname == l_cRenderKey_Directory)
						{fsLocator value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_OutputDirectory.SetValue(value);}
					else if (keyname == l_cRenderKey_SoundFile)
						{fsLocator value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_SoundFile.SetValue(value);}
					else if (keyname == l_cRenderKey_DisplayTimeCode)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bDisplayTimeCode.SetValue(value);}
					else if (keyname == l_cRenderKey_EndTime)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fEndTime.SetValue(value);}
					else if (keyname == l_cRenderKey_LeadIn)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fLeadIn.SetValue(value);}
					else if (keyname == l_cRenderKey_LeadOut)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fLeadOut.SetValue(value);}
					else if (keyname == l_cRenderKey_Prefix)
						{std::string value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_Prefix.SetValue(value);}
					else if (keyname == l_cRenderKey_StartTime)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fStartTime.SetValue(value);}
					else if (keyname == l_cRenderKey_CaptureAllCameras)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bCaptureAllCameras.SetValue(value);}
					else if (keyname == l_cRenderKey_UseCameraName)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseCameraNameInFilename.SetValue(value);}
					else if (keyname == l_cRenderKey_UseSceneFilename)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseSceneFilenameInFilename.SetValue(value);}
					else if (keyname == l_cRenderKey_UseCompressionInName)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseCompressionInFilename.SetValue(value);}
					else if (keyname == l_cRenderKey_UseRenderDriverAsDir)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseRenderDriverAsDirAndFname.SetValue(value);}
					else if (keyname == l_cRenderKey_UseCameraNameAsDir)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseCameraNameAsDirectory.SetValue(value);}
					else if (keyname == l_cRenderKey_UseResolutionAsDir)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseResolutionAsDirectory.SetValue(value);}
					else if (keyname == l_cRenderKey_UseSceneFilenameAsDir)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseSceneFilenameAsDirectory.SetValue(value);}
					else if (keyname == l_cRenderKey_Width)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nWidth.SetValue(value);}
					else if (keyname == l_cRenderKey_Height)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nHeight.SetValue(value);}
					else if (keyname == l_cRenderKey_TitleCard)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bOutputTitleCard.SetValue(value); /*DBG_LOG1("TitleCard set (%s)",(value?"true":"false"));*/}
					else if (keyname == l_cRenderKey_CounterDigits)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nCounterDigits.SetValue(value);}
					else if (keyname == l_cRenderKey_SubdivLevel)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nSubdivLevel.SetValue(value);}
					else if (keyname == l_cRenderKey_AutoGenDir)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bAutoGenerateDirectory.SetValue(value);}
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
				}
			}

			guiXMLTextReader::Close();

	        delete parser;
	        delete errHandler;
#endif
		}


		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void cptrRenderOutputObject::ReadFromConfigFile(const fsLocator& i_ConfigFile,
														cptrRenderOutputData& o_Data )
		{
			std::string cfgpath;
			fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

			if (!fsFileUtil::FileExists( i_ConfigFile ))
			{
				//DBG_LOG1("config file not found (%s) [this is alright]", cfgpath.c_str());

				//	config file doesn't exist, use defaults
				fsLocator scene;
				SetToDefault(scene, o_Data);
				return;
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
					else if (keyname == l_cRenderKey_UseMarkers)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseMarkerTimes.SetValue(value);}
					else if (keyname == l_cRenderKey_MarkerInTime)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fMarkerInTime.SetValue(value);}
					else if (keyname == l_cRenderKey_MarkerOutTime)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fMarkerOutTime.SetValue(value);}
					else if (keyname == l_cRenderKey_CaptureFPS)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fCaptureFPS.SetValue(value);}
					else if (keyname == l_cRenderKey_MotionSamplesPerFrame)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nMotionSamplesPerFrame.SetValue(value);}
					else if (keyname == l_cRenderKey_CaptureMax)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nCaptureMax.SetValue(value);}
					else if (keyname == l_cRenderKey_Sampling)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nCaptureSampling.SetValue(value);}
					else if (keyname == l_cRenderKey_JitteredSampling)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bJitteredSampling.SetValue(value);}
					else if (keyname == l_cRenderKey_CaptureFormat)
						{std::string value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_CaptureFormat.SetValue(value);}
					else if (keyname == l_cRenderKey_CompressCode)
						{std::string value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_CompressCode.SetValue(value);}
					else if (keyname == l_cRenderKey_Directory)
						{fsLocator value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_OutputDirectory.SetValue(value);}
					else if (keyname == l_cRenderKey_SoundFile)
						{fsLocator value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_SoundFile.SetValue(value);}
					else if (keyname == l_cRenderKey_DisplayTimeCode)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bDisplayTimeCode.SetValue(value);}
					else if (keyname == l_cRenderKey_EndTime)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fEndTime.SetValue(value);}
					else if (keyname == l_cRenderKey_LeadIn)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fLeadIn.SetValue(value);}
					else if (keyname == l_cRenderKey_LeadOut)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fLeadOut.SetValue(value);}
					else if (keyname == l_cRenderKey_Prefix)
						{std::string value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_Prefix.SetValue(value);}
					else if (keyname == l_cRenderKey_StartTime)
						{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_fStartTime.SetValue(value);}
					else if (keyname == l_cRenderKey_CaptureAllCameras)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bCaptureAllCameras.SetValue(value);}
					else if (keyname == l_cRenderKey_UseCameraName)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseCameraNameInFilename.SetValue(value);}
					else if (keyname == l_cRenderKey_UseSceneFilename)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseSceneFilenameInFilename.SetValue(value);}
					else if (keyname == l_cRenderKey_UseCompressionInName)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseCompressionInFilename.SetValue(value);}
					else if (keyname == l_cRenderKey_UseRenderDriverAsDir)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseRenderDriverAsDirAndFname.SetValue(value);}
					else if (keyname == l_cRenderKey_UseCameraNameAsDir)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseCameraNameAsDirectory.SetValue(value);}
					else if (keyname == l_cRenderKey_UseResolutionAsDir)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseResolutionAsDirectory.SetValue(value);}
					else if (keyname == l_cRenderKey_UseSceneFilenameAsDir)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bUseSceneFilenameAsDirectory.SetValue(value);}
					else if (keyname == l_cRenderKey_Width)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nWidth.SetValue(value);}
					else if (keyname == l_cRenderKey_Height)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nHeight.SetValue(value);}
					else if (keyname == l_cRenderKey_TitleCard)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bOutputTitleCard.SetValue(value); /*DBG_LOG1("TitleCard set (%s)",(value?"true":"false"));*/}
					else if (keyname == l_cRenderKey_CounterDigits)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nCounterDigits.SetValue(value);}
					else if (keyname == l_cRenderKey_SubdivLevel)
						{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_nSubdivLevel.SetValue(value);}
					else if (keyname == l_cRenderKey_AutoGenDir)
						{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bAutoGenerateDirectory.SetValue(value);}
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
				}
			}

			guiXMLTextReader::Close();
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void cptrRenderOutputObject::Read( cptrRenderOutputData& o_Data )
		{
			//SetConfigFilename( o_Data.m_RenderOutputDataFile.GetValue(), o_Data.m_RenderOutputDataFile.GetValue() );
			ReadFromConfigFile( o_Data.m_RenderOutputDataFile.GetValue(), o_Data);
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void cptrRenderOutputObject::Read(const fsLocator& i_ConfigFile,
											cptrRenderOutputData& o_Data )
		{
			//SetConfigFilename( i_ConfigFile, i_ConfigFile );
			ReadFromConfigFile(i_ConfigFile, o_Data);
		}


		//------------------------------------------------------------------------
		//	set the capture data to default settings
		//------------------------------------------------------------------------
		void cptrRenderOutputObject::SetToDefault(fsLocator& i_CurrentScene,
												  cptrRenderOutputData& o_Data)
		{
#ifdef _MANAGED
			o_Data.m_bCaptureMovie.SetValue( true );
			o_Data.m_CaptureFormat.SetValue( "MOV" );				// frame or movie format
			o_Data.m_CompressCode.SetValue( "jpeg" );
			o_Data.m_nWidth.SetValue( 640 );
			o_Data.m_nHeight.SetValue( 480 );
#else
			o_Data.m_bCaptureMovie.SetValue( false );
			//o_Data.m_CaptureFormat.SetValue( "AVI" );				// frame or movie format
			//o_Data.m_CompressCode.SetValue( "DIB" );
			o_Data.m_CaptureFormat.SetValue( "JPG" );				// frame or movie format
			o_Data.m_CompressCode.SetValue( "" );
			// This needs to match the window size set in WinMain.cpp
			// in non-managed code until the XML parsing is working:
			o_Data.m_nWidth.SetValue( 256 );
			o_Data.m_nHeight.SetValue( 256 );
#endif
			o_Data.m_fCaptureFPS.SetValue( g3dConstants::c_fDefaultFrameRate );
			o_Data.m_nMotionSamplesPerFrame.SetValue( 1 );
			o_Data.m_nCaptureMax.SetValue( (int)(o_Data.m_fCaptureFPS.GetValue() * tmlnTimeLine::GetMaximum()) );
			o_Data.m_nCaptureSampling.SetValue( 1 );
			o_Data.m_bJitteredSampling.SetValue( false );
			fsLocator empty;
			o_Data.m_OutputDirectory.SetValue(empty);
			o_Data.m_SoundFile.SetValue(empty);
			o_Data.m_bDisplayTimeCode.SetValue( false );
			o_Data.m_fEndTime.SetValue( -1.0f );
			o_Data.m_fLeadIn.SetValue( 0.0f );
			o_Data.m_fLeadOut.SetValue( 0.0f );
			o_Data.m_fStartTime.SetValue( 0.0f );
			o_Data.m_bCaptureAllCameras.SetValue( false );
			o_Data.m_bUseCameraNameInFilename.SetValue( true );
			o_Data.m_bUseSceneFilenameInFilename.SetValue( true );
			o_Data.m_bUseCompressionInFilename.SetValue( true );
			o_Data.m_bUseRenderDriverAsDirAndFname.SetValue( true );
			o_Data.m_bUseCameraNameAsDirectory.SetValue( true );
			o_Data.m_bUseResolutionAsDirectory.SetValue( false );
			o_Data.m_bUseSceneFilenameAsDirectory.SetValue( true );
			o_Data.m_bOutputTitleCard.SetValue( false );
			o_Data.m_bMoviePerDriver.SetValue( true );
			o_Data.m_Prefix.SetValue( "scene" );
			o_Data.m_bAutoGenerateDirectory.SetValue( true );
			o_Data.m_nCounterDigits.SetValue( 4 );
			o_Data.m_nCurrentFrame.SetValue( 0 );
			o_Data.m_nSubdivLevel.SetValue( 1 );
			o_Data.m_bShowRenderProgressDialog.SetValue( true );
			o_Data.m_RenderPosTime.SetValue(-1.0f);
			o_Data.m_OutputFiles.ClearList();
			o_Data.m_bKeepFrameOpenAfterRender.SetValue(false);

			if ( i_CurrentScene.GetNumNames() > 0 )
			{
				// generate and set the directory
				o_Data.m_OutputDirectory.SetValue( i_CurrentScene );
				fsLocator dir = o_Data.m_OutputDirectory.GetValue();
				dir.Pop();
				o_Data.m_OutputDirectory.SetValue(dir);

				int index = o_Data.m_OutputDirectory.GetValue().FindFirst( itString("Shots") );
				DBG_ASSERT0( index != -1, "Shots name not in directory name" );
				dir = o_Data.m_OutputDirectory.GetValue();
				dir.ReplaceName( itString("Footage"), index );
				o_Data.m_OutputDirectory.SetValue(dir);

				//	debug only
				//std::string sdir;
				//fsFileUtil::LocatorToANSIFilename(o_Data.m_OutputDirectory, sdir);
				//DBG_LOG1("output dir %s", sdir.c_str());
			}
			else
			{
				o_Data.m_OutputDirectory.SetValue( gfPaths::GetPath(mnmPaths::e_SaveFootage) );
			}
		}


		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//void cptrRenderOutputObject::SetConfigFilename(const char* i_ConfigFile, const char* i_DefaultConfigFile)
		//{
		//	m_OutputFilename = i_ConfigFile;
		//	m_OutputDefaultFilename = i_DefaultConfigFile;
		//}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//virtual 
		void cptrRenderOutputObject::RegisterProperties()
		{
			prtyCheckBoxUIInfo* pCHUII;
			prtyPropertyUIInfo* pPUII;
			prtyFloatEditUIInfo* pFEUII;
			prtyComboBoxUIInfo* pCBUII;
			prtyNumericUpDownUIInfo* pNUDUII;
			prtyListBoxUIInfo* pLBUII;
			prtyFileChooserUIInfo* pFCUII;
			prtyButtonUIInfo* pBUII;

			//	audio
			pPUII = new prtyFileChooserUIInfo(&(m_Data.m_SoundFile), "Audio", "Sound file to attach to a movie only");
			AddProperty( pPUII );
			//	camera
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bCaptureAllCameras), "Cameras", "Capture all cameras");
			AddProperty( pPUII );
			pLBUII = new prtyListBoxUIInfo(&(m_Data.m_Cameras), "Cameras", "Camera List");
			pLBUII->m_bChecked = true;
			pLBUII->m_bOnlyOneSelected = false;
			m_pCamerasUIInfo = pLBUII;	//	store this for later update
			AddProperty( pLBUII );
			//	Dimensions
			pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_Resolution), "Dimensions", "Render output resolution");
			m_pResolutions = pCBUII;

			//	read in the configuration file for resolutions
			//
			fsLocator cfgdir;
			cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
			cfgdir.Push( "Resolutions.cfg" );
			category_list_type resolutions(2, std::vector<std::string>(0));
			cptrCategoryConfigFileUtil::SetCategoryTag(std::string("Category"));
			cptrCategoryConfigFileUtil::SetElementTag(std::string("Resolution"));
			cptrCategoryConfigFileUtil::ReadConfigFile( cfgdir, resolutions );

			pCBUII->AddItem(std::string("Custom Resolution"));
			char tempstr[64];
			for (int i=0; i < resolutions[0].size(); ++i)
			{
				sprintf(tempstr,"%s [%s]", resolutions[1][i].c_str(), resolutions[0][i].c_str());
				pCBUII->AddItem(std::string(tempstr));
			}
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
			pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_nCaptureSampling), "Dimensions", "1 = No Anti-Aliasing, 2=4x AA, 3=9x AA, 4=16x AA");
			pNUDUII->SetDecimalPlaces(0);
			pNUDUII->SetMinimum(1);
			pNUDUII->SetMaximum(4);
			AddProperty( pNUDUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bJitteredSampling), "Dimensions", "Jitter sampling");
			AddProperty( pPUII );
			//	Filename
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseSceneFilenameInFilename), "Filename", "Use the scene filename in the output filename");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseCameraNameInFilename), "Filename", "Use the camera name in the output filename");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseCompressionInFilename), "Filename", "Use the compression type in the output filename");
			AddProperty( pPUII );
			pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_nCounterDigits), "Filename", "Number of digits to display in the filename counter");
			pNUDUII->SetDecimalPlaces(0);
			pNUDUII->SetMinimum(1);
			pNUDUII->SetMaximum(10);
			AddProperty( pNUDUII );

			//	Frames
			pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_fCaptureFPS), "Frames", "Number of frames per second to capture");
			cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
			cfgdir.Push( "FramesPerSecond.cfg" );
			category_list_type fps(2, std::vector<std::string>(0));
			cptrCategoryConfigFileUtil::SetElementTag(std::string("FPS"));
			cptrCategoryConfigFileUtil::ReadConfigFile( cfgdir, fps );
	
			//add to list, the values for Frames per second		
			for (int i=0; i < fps[0].size(); ++i)
			{
				sprintf(tempstr,"%s", fps[1][i].c_str());
				pCBUII->AddItem(std::string(tempstr));
			}
			AddProperty( pCBUII );

			//	Motion samples per frame
			pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_nMotionSamplesPerFrame), "Frames", "Number of motion samples per frame");
		
			cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
			cfgdir.Push( "MotionSamplesPerFrame.cfg" );
			category_list_type mspf(2, std::vector<std::string>(0));
			
			cptrCategoryConfigFileUtil::SetElementTag(std::string("MSPF"));
			cptrCategoryConfigFileUtil::ReadConfigFile( cfgdir, mspf );

			//add to list, the values for Motion Samples Per Frame	
			for (int i=0; i < mspf[0].size(); ++i)
			{
				sprintf(tempstr,"%s", mspf[1][i].c_str());
				pCBUII->AddItem(std::string(tempstr));
			}
			AddProperty( pCBUII );

			//	Location
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bAutoGenerateDirectory), "Location", "Output directory based on project and scene");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseSceneFilenameAsDirectory), "Location", "Use the scene filename as an output directory");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseCameraNameAsDirectory), "Location", "Use the camera name as an output directory");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseResolutionAsDirectory), "Location", "Use the resolution as an output directory");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseRenderDriverAsDirAndFname), "Location", "Use the render driver as directory and filename");
			AddProperty( pPUII );
			pPUII = new prtyFolderChooserUIInfo(&(m_Data.m_OutputDirectory), "Location", "Directory to output frames or movies");
			AddProperty( pPUII );
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
			pPUII = new prtyFloatEditUIInfo(&(m_Data.m_RenderPosTime), "Resume Point", "Render Saved Point - Time");
			m_pRenderPosTimeUII = pPUII;
			pPUII->SetReadOnly(true);
			AddProperty( pPUII );
			pFCUII = new prtyFileChooserUIInfo(&(m_Data.m_RenderPosSaveFile), "Resume Point", "Render Saved Point - Save File");
			m_pRenderPosSaveFileUII = pFCUII;
			pFCUII->SetReadOnly(true);
			AddProperty( pFCUII );
			pBUII = new prtyButtonUIInfo(&(m_Data.m_bRenderPosDelete), "Resume Point", "Delete Resume Point");
			m_pRenderPosDeleteUII = pBUII;
			pBUII->SetText(std::string("Delete"));
			AddProperty( pBUII );
			//	Time
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseMarkerTimes), "Time", "Use marker times for rendering");
			AddProperty( pPUII );
			pPUII = new prtyFloatEditUIInfo(&(m_Data.m_fStartTime), "Time", "Time to start rendering");
			m_pStartTimeUII = pPUII;
			AddProperty( pPUII );
			pPUII = new prtyFloatEditUIInfo(&(m_Data.m_fEndTime), "Time", "Time to end rendering");
			m_pEndTimeUII = pPUII;
			AddProperty( pPUII );
			//	Video Output
			//pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bCaptureMovie), "Video Output", "Render to a movie");
			//AddProperty( pPUII );
			pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_CaptureFormat), "Video Output", "Output movie format");
			pCBUII->AddItem(std::string("MOV"));
			pCBUII->AddItem(std::string("AVI"));
			pCBUII->AddItem(std::string("BMP"));
			pCBUII->AddItem(std::string("JPG"));
			pCBUII->AddItem(std::string("TGA"));
			pCBUII->AddItem(std::string("PNG"));
			pCBUII->AddItem(std::string("DDS"));
			pCBUII->AddItem(std::string("PPM"));
			pCBUII->AddItem(std::string("DIB"));
			pCBUII->AddItem(std::string("HDR"));
			pCBUII->AddItem(std::string("PFM"));
			AddProperty( pCBUII );
			pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_CompressCode), "Video Output", "Compression code for movie");
			m_pCompressCodeUII = pCBUII;
			AddProperty( pCBUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMoviePerDriver), "Video Output", "Create a movie for each capture driver");
			AddProperty( pPUII );
			//	Visual Output
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bDisplayTimeCode), "Visual Output", "Display the timecode on the rendered frames");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bOutputTitleCard), "Visual Output", "Output a title card before each capture block");
			AddProperty( pPUII );
			pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_nSubdivLevel), "Visual Output", "Subdiv level");
			pNUDUII->SetDecimalPlaces(0);
			pNUDUII->SetMinimum(0);
			pNUDUII->SetMaximum(3);
			AddProperty( pNUDUII );

			//	not displayed properties, but need to be accessed by python
			//
			pPUII = new prtyPropertyUIInfo(&(m_Data.m_OutputFiles), "Hidden", "Output Files");
			AddProperty(pPUII);

			//	now add the callbacks
			m_Data.m_bCaptureAllCameras.AddCallback(new prtyCallbackWrapper<cptrRenderOutputObject>(this, &cptrRenderOutputObject::AllCamerasChanged));
			m_Data.m_CaptureFormat.AddCallback(new prtyCallbackWrapper<cptrRenderOutputObject>(this, &cptrRenderOutputObject::CaptureFormatChanged));
			m_Data.m_bUseMarkerTimes.AddCallback(new prtyCallbackWrapper<cptrRenderOutputObject>(this, &cptrRenderOutputObject::UseMarkerTimeChanged));
			m_Data.m_bRenderPosUse.AddCallback(new prtyCallbackWrapper<cptrRenderOutputObject>(this, &cptrRenderOutputObject::RenderPosUseChanged));
			m_Data.m_bRenderPosDelete.AddCallback(new prtyCallbackWrapper<cptrRenderOutputObject>(this, &cptrRenderOutputObject::RenderPosDeleteClicked));
			m_Data.m_Resolution.AddCallback(new prtyCallbackWrapper<cptrRenderOutputObject>(this, &cptrRenderOutputObject::ResolutionClicked));
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void cptrRenderOutputObject::update_compresscodeUII( const std::string& i_CaptureType )
		{
			if (i_CaptureType == std::string("AVI"))
			{
				m_Data.m_bCaptureMovie.SetValue(true);
				m_pCompressCodeUII->ClearItems();
				m_pCompressCodeUII->AddItem(std::string("DIB"));
				m_pCompressCodeUII->AddItem(std::string("RAW"));
				m_pCompressCodeUII->AddItem(std::string("RGB"));
				m_pCompressCodeUII->AddItem(std::string("MRLE"));
				m_pCompressCodeUII->AddItem(std::string("IV32"));
				m_pCompressCodeUII->AddItem(std::string("CVID"));
				m_pCompressCodeUII->AddItem(std::string("MJPG"));
				m_pCompressCodeUII->AddItem(std::string("VDOM"));
				m_pCompressCodeUII->AddItem(std::string("IV50"));
				m_pCompressCodeUII->AddItem(std::string("MPG4"));
				m_pCompressCodeUII->AddItem(std::string("DIV4"));
				m_pCompressCodeUII->AddItem(std::string(" "));
				m_pCompressCodeUII->UpdateControl();
			}
			else if (i_CaptureType == std::string("MOV"))
			{
				m_Data.m_bCaptureMovie.SetValue(true);
				m_pCompressCodeUII->ClearItems();
				m_pCompressCodeUII->AddItem(std::string("raw "));
				m_pCompressCodeUII->AddItem(std::string("jpeg"));
				m_pCompressCodeUII->AddItem(std::string("cvid"));
				m_pCompressCodeUII->AddItem(std::string("smc "));
				m_pCompressCodeUII->AddItem(std::string("rle "));
				m_pCompressCodeUII->AddItem(std::string("rpza"));
				m_pCompressCodeUII->AddItem(std::string("yuv2"));
				m_pCompressCodeUII->AddItem(std::string("mjpa"));
				m_pCompressCodeUII->AddItem(std::string("mjpb"));
				m_pCompressCodeUII->AddItem(std::string(".SGI"));
				m_pCompressCodeUII->AddItem(std::string("8BPS"));
				m_pCompressCodeUII->AddItem(std::string("PNTG"));
				m_pCompressCodeUII->AddItem(std::string("gif "));
				m_pCompressCodeUII->AddItem(std::string("kpcd"));
				m_pCompressCodeUII->AddItem(std::string("qdgx"));
				m_pCompressCodeUII->AddItem(std::string("avr "));
				m_pCompressCodeUII->AddItem(std::string("dmb1"));
				m_pCompressCodeUII->AddItem(std::string("WRLE"));
				m_pCompressCodeUII->AddItem(std::string("WRAW"));
				m_pCompressCodeUII->AddItem(std::string("path"));
				m_pCompressCodeUII->AddItem(std::string("qdrw"));
				m_pCompressCodeUII->AddItem(std::string("ripl"));
				m_pCompressCodeUII->AddItem(std::string("fire"));
				m_pCompressCodeUII->AddItem(std::string("clou"));
				m_pCompressCodeUII->AddItem(std::string("h261"));
				m_pCompressCodeUII->AddItem(std::string("h263"));
				m_pCompressCodeUII->AddItem(std::string("dvc "));
				m_pCompressCodeUII->AddItem(std::string("dvcp"));
				m_pCompressCodeUII->AddItem(std::string("dvpp"));
				m_pCompressCodeUII->AddItem(std::string("dv5n"));
				m_pCompressCodeUII->AddItem(std::string("dv5p"));
				m_pCompressCodeUII->AddItem(std::string("dv1n"));
				m_pCompressCodeUII->AddItem(std::string("dv1p"));
				m_pCompressCodeUII->AddItem(std::string("dvhp"));
				m_pCompressCodeUII->AddItem(std::string("dvh6"));
				m_pCompressCodeUII->AddItem(std::string("dvh5"));
				m_pCompressCodeUII->AddItem(std::string("base"));
				m_pCompressCodeUII->AddItem(std::string("flic"));
				m_pCompressCodeUII->AddItem(std::string("tga "));
				m_pCompressCodeUII->AddItem(std::string("png "));
				m_pCompressCodeUII->AddItem(std::string("tiff"));
				m_pCompressCodeUII->AddItem(std::string("yuvu"));
				m_pCompressCodeUII->AddItem(std::string("yuvs"));
				m_pCompressCodeUII->AddItem(std::string("cmyk"));
				m_pCompressCodeUII->AddItem(std::string("msvc"));
				m_pCompressCodeUII->AddItem(std::string("SVQ1"));
				m_pCompressCodeUII->AddItem(std::string("SVQ3"));
				m_pCompressCodeUII->AddItem(std::string("IV41"));
				m_pCompressCodeUII->AddItem(std::string("mp4v"));
				m_pCompressCodeUII->AddItem(std::string("b64a"));
				m_pCompressCodeUII->AddItem(std::string("b48r"));
				m_pCompressCodeUII->AddItem(std::string("b32a"));
				m_pCompressCodeUII->AddItem(std::string("b16g"));
				m_pCompressCodeUII->AddItem(std::string("myuv"));
				m_pCompressCodeUII->AddItem(std::string("y420"));
				m_pCompressCodeUII->AddItem(std::string("syv9"));
				m_pCompressCodeUII->AddItem(std::string("2vuy"));
				m_pCompressCodeUII->AddItem(std::string("v308"));
				m_pCompressCodeUII->AddItem(std::string("v408"));
				m_pCompressCodeUII->AddItem(std::string("v216"));
				m_pCompressCodeUII->AddItem(std::string("v210"));
				m_pCompressCodeUII->AddItem(std::string("v410"));
				m_pCompressCodeUII->AddItem(std::string("r408"));
				m_pCompressCodeUII->AddItem(std::string("mjp2"));
				m_pCompressCodeUII->AddItem(std::string("pxlt"));
				m_pCompressCodeUII->AddItem(std::string("avc1"));
				m_pCompressCodeUII->UpdateControl();
			}
			else
			{
				m_Data.m_bCaptureMovie.SetValue(false);
				m_pCompressCodeUII->ClearItems();
				m_pCompressCodeUII->AddItem(std::string("None"));
				m_pCompressCodeUII->UpdateControl();
			}
		}

		//==========================================================================
		// Callbacks for when properties change, updates member data
		//==========================================================================

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void cptrRenderOutputObject::AllCamerasChanged(prtyProperty *i_pProperty, bool i_bDirty)
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
		void cptrRenderOutputObject::CaptureFormatChanged(prtyProperty *i_pProperty, bool i_bDirty)
		{
			prtyText* ptxt = static_cast<prtyText*>(i_pProperty);

			if (ptxt != NULL)
			{
				update_compresscodeUII( ptxt->GetValue() );
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void cptrRenderOutputObject::UseMarkerTimeChanged(prtyProperty *i_pProperty, bool i_bDirty)
		{
			//if (m_Data.m_bUseMarkerTimes.GetValue())
			//{
			//	m_pStartTimeUII->SetReadOnly(true);
			//	m_pStartTimeUII->UpdateControl();
			//	m_pEndTimeUII->SetReadOnly(true);
			//	m_pEndTimeUII->UpdateControl();
			//}
			//else
			//{
			//	m_pStartTimeUII->SetReadOnly(false);
			//	m_pStartTimeUII->UpdateControl();
			//	m_pEndTimeUII->SetReadOnly(false);
			//	m_pEndTimeUII->UpdateControl();
			//}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void cptrRenderOutputObject::RenderPosUseChanged(prtyProperty *i_pProperty, bool i_bDirty)
		{
			//if (m_Data.m_bRenderPosUse.GetValue())
			//{
				//m_pRenderPosSceneUII->SetReadOnly(true);
				//m_pRenderPosSceneUII->UpdateControl();
				//m_pRenderPosCameraUII->SetReadOnly(true);
				//m_pRenderPosCameraUII->UpdateControl();
				//m_pRenderPosTimeUII->SetReadOnly(true);
				//m_pRenderPosTimeUII->UpdateControl();
				//m_pRenderPosSaveFileUII->SetReadOnly(true);
				//m_pRenderPosSaveFileUII->UpdateControl();
				//
				//m_pRenderPosDeleteUII->SetReadOnly(false);
				//m_pRenderPosDeleteUII->UpdateControl();
				
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
		void cptrRenderOutputObject::RenderPosDeleteClicked(prtyProperty *i_pProperty, bool i_bDirty)
		{
			cptrRenderProgressDialogUtil::DeleteRenderPosition();

			//m_pRenderPosDeleteUII->SetReadOnly(true);
			//m_pRenderPosDeleteUII->UpdateControl();

			m_Data.m_RenderPosSaveFile.SetValue("");
			//m_pRenderPosSaveFileUII->SetReadOnly(true);
			//m_pRenderPosSaveFileUII->UpdateControl();

			m_Data.m_bRenderPosUse.SetValue(false);
			//m_pRenderPosUseUII->SetReadOnly(true);
			//m_pRenderPosUseUII->UpdateControl();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void cptrRenderOutputObject::ResolutionClicked(prtyProperty *i_pProperty, bool i_bDirty)
		{
			if ( (m_Data.m_Resolution.GetValue().size() == 0)
				||
				 (m_Data.m_Resolution.GetValue() == std::string("Custom Resolution")))
			{
				//m_Data.m_nWidth.SetValue(0);
				//m_Data.m_nHeight.SetValue(0);

				//m_pRenderWidthUII->SetReadOnly(false);
				//m_pRenderWidthUII->UpdateControl();
				//m_pRenderHeightUII->SetReadOnly(false);
				//m_pRenderHeightUII->UpdateControl();
			}
			else
			{
				int w,h;
				sscanf( m_Data.m_Resolution.GetValue().c_str(),"%dx%d", &w, &h );
				m_Data.m_nWidth.SetValue(w);
				m_Data.m_nHeight.SetValue(h);

				//m_pRenderWidthUII->SetReadOnly(true);
				//m_pRenderWidthUII->UpdateControl();
				//m_pRenderHeightUII->SetReadOnly(true);
				//m_pRenderHeightUII->UpdateControl();
			}
		}

		//
		//	render output object
		//
		static cptrRenderOutputObject* l_pDataObject = 0;

		
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
					return l_pDataObject;
				}
				return NULL;
			}
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		shared_ptr<CaptureNameResolver> l_NameResolver(new CaptureNameResolver);


		//--------------------------------------------------------------------------------
		//--------------------------------------------------------------------------------
		void debug_OutputDataValues()
		{
			DBG_LOG0( "SCDU Data -> Capture Data" );
			DBG_LOG0( "-------------------" );
			DBG_LOG1( "SCDU: cap FPS  = %6.3f", l_pDataObject->m_Data.m_fCaptureFPS.GetValue() );
			DBG_LOG1( "SCDU: cap mblur= %d", l_pDataObject->m_Data.m_nMotionSamplesPerFrame.GetValue() );
			DBG_LOG1( "SCDU: cap sampl= %d", l_pDataObject->m_Data.m_nCaptureSampling.GetValue() );
			DBG_LOG1( "SCDU: cap jittr= %d", l_pDataObject->m_Data.m_bJitteredSampling.GetValue() ? "true":"false");
			DBG_LOG1( "SCDU: cap max  = %d", l_pDataObject->m_Data.m_nCaptureMax.GetValue() );
			DBG_LOG1( "SCDU: lead in  = %6.3f", l_pDataObject->m_Data.m_fLeadIn.GetValue() );
			DBG_LOG1( "SCDU: lead out = %6.3f", l_pDataObject->m_Data.m_fLeadOut.GetValue() );
			DBG_LOG1( "SCDU: starttime= %6.3f", l_pDataObject->m_Data.m_fStartTime.GetValue() );
			DBG_LOG1( "SCDU: end time = %6.3f", l_pDataObject->m_Data.m_fEndTime.GetValue() );
			DBG_LOG1( "SCDU: mrkr in  = %6.3f", l_pDataObject->m_Data.m_fMarkerInTime.GetValue() );
			DBG_LOG1( "SCDU: mrkr out = %6.3f", l_pDataObject->m_Data.m_fMarkerOutTime.GetValue() );
			DBG_LOG1( "SCDU: width    = %d", l_pDataObject->m_Data.m_nWidth.GetValue() );
			DBG_LOG1( "SCDU: height   = %d", l_pDataObject->m_Data.m_nHeight.GetValue() );
			DBG_LOG1( "SCDU: LOD level= %d", l_pDataObject->m_Data.m_nSubdivLevel.GetValue() );
			DBG_LOG1( "SCDU: cap movie= %s", ( l_pDataObject->m_Data.m_bCaptureMovie.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: scenename= %s", ( l_pDataObject->m_Data.m_bUseSceneFilenameInFilename.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: cam name = %s", ( l_pDataObject->m_Data.m_bUseCameraNameInFilename.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: use comp = %s", ( l_pDataObject->m_Data.m_bUseCompressionInFilename.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: scene dir= %s", ( l_pDataObject->m_Data.m_bUseSceneFilenameAsDirectory.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: cam   dir= %s", ( l_pDataObject->m_Data.m_bUseCameraNameAsDirectory.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: res   dir= %s", ( l_pDataObject->m_Data.m_bUseResolutionAsDirectory.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: driverdir= %s", ( l_pDataObject->m_Data.m_bUseRenderDriverAsDirAndFname.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: all cams = %s", ( l_pDataObject->m_Data.m_bCaptureAllCameras.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: timecode = %s", ( l_pDataObject->m_Data.m_bDisplayTimeCode.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: titlecard= %s", ( l_pDataObject->m_Data.m_bOutputTitleCard.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: markers  = %s", ( l_pDataObject->m_Data.m_bUseMarkerTimes.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: compress = %s", l_pDataObject->m_Data.m_CompressCode.GetValue().c_str() );
			DBG_LOG1( "SCDU: prefix   = %s", l_pDataObject->m_Data.m_Prefix.GetValue().c_str() );
			DBG_LOG1( "SCDU: cap type = %s", l_pDataObject->m_Data.m_CaptureFormat.GetValue().c_str() );
			DBG_LOG1( "SCDU: cntr #s  = %d", l_pDataObject->m_Data.m_nCounterDigits.GetValue() );
			std::string dir;
			fsFileUtil::LocatorToANSIFilename( l_pDataObject->m_Data.m_OutputDirectory.GetValue(), dir );
			DBG_LOG1( "SCDU: directory= (%s)", dir.c_str() );
			fsFileUtil::LocatorToANSIFilename( l_pDataObject->m_Data.m_SoundFile.GetValue(), dir );
			DBG_LOG1( "SCDU: soundfile= (%s)", dir.c_str() );
			DBG_LOG1( "SCDU: progress = %s", ( l_pDataObject->m_Data.m_bShowRenderProgressDialog.GetValue() ? "true":"false" ) );
			DBG_LOG1( "SCDU: movie per= %s", ( l_pDataObject->m_Data.m_bMoviePerDriver.GetValue() ? "true":"false" ) );
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CleanUp()
	{
		if (l_pDataObject)
		{
			// Remove name resolver 
			pythProperty::RemoveNameResolver( l_NameResolver ); 

			delete l_pDataObject;
			l_pDataObject = NULL;
		}
	}


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadData(cptrRenderOutputData& o_Data)
	{
		l_pDataObject->Read(o_Data);
	}
	void ReadData(const fsLocator& i_ConfigFile, cptrRenderOutputData& o_Data)
	{
		l_pDataObject->Read(i_ConfigFile, o_Data);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetWidth(int width) {
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
		data.m_nWidth.SetValue(width);
	}

	void SetHeight(int height) {
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
		data.m_nHeight.SetValue(height);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetProgressiveStartWidth(int width) {
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
		data.m_ProgressiveStartWidth.SetValue(width);
	}

	void SetProgressiveStartHeight(int height) {
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
		data.m_ProgressiveStartHeight.SetValue(height);
	}

	void SetProgressiveCount(int count) {
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
		data.m_ProgressiveCount.SetValue(count);
	}

	void SetProgressive(bool progressive) {
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
		data.m_Progressive.SetValue(progressive);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void WriteData(const cptrRenderOutputData& i_Data)
	{
		l_pDataObject->Write(i_Data);
	}
	void WriteData(const fsLocator& i_ConfigFile, const cptrRenderOutputData& i_Data)
	{
		l_pDataObject->Write(i_ConfigFile, i_Data);
	}


	//------------------------------------------------------------------------
	//	called right before showing the dialog
	//------------------------------------------------------------------------
	void SetupControls( bool i_bBatchMode )
	{
		if (l_pDataObject != 0)
		{
			l_pDataObject->SetupControls(i_bBatchMode);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void BuildCameraList(cptrRenderOutputData& o_Data)
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
	void AppendXMLDescription(LightWaitResponse *response) 
	{
		if (response == NULL) 
		{
			return;
		}

		std::string xmlString;
		
		// cameras
		//
		nameString camname;
		const int num_cameras = camsCameraMgr::GetNumCameras();

		xmlString = "<Cameras>";
		
		xmlString.append("<CameraCount>");
		char tempStr[255];
		sprintf(tempStr,"%d",num_cameras);
		xmlString.append(tempStr);
		xmlString.append("</CameraCount>");

		for (int i = 0; i < num_cameras; ++i) {
			camsCameraMgr::GetCameraName(i, camname);
			xmlString.append("<Camera>");
			xmlString.append("<Index>");
			sprintf(tempStr,"%d",i);
			xmlString.append(tempStr);
			xmlString.append("</Index>");
			xmlString.append("<Name>");
			xmlString.append(camname.GetString());
			xmlString.append("</Name>");
			xmlString.append("</Camera>");
		}

		xmlString.append("</Cameras>");

		response->setCameraDescriptor(xmlString);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateCameraList(cptrRenderOutputData& i_NewData)
	{
		if (l_pDataObject != 0)
		{
			l_pDataObject->UpdateCameraList(i_NewData);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateData(cptrRenderOutputData& i_NewData)
	{
		if (l_pDataObject != 0)
			l_pDataObject->m_Data = i_NewData;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cptrRenderOutputData& Data()
	{
		if (l_pDataObject == 0)
		{
			l_pDataObject = new cptrRenderOutputObject();

			// Add name resolver to match a string to our prtyObject
			pythProperty::AddNameResolver( l_NameResolver ); 
		}

		return l_pDataObject->m_Data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentScene( itString& i_SceneName )
	{
		l_pDataObject->m_Data.m_CurrentScene.SetValue( i_SceneName );

		//DBG_LOG1("Setting CurrentScene (%s)", itStringUtil::GetStdString(l_pDataObject->m_Data.m_CurrentScene).c_str());
	}

	void SetCurrentScene( nameString& i_SceneName )
	{
		l_pDataObject->m_Data.m_CurrentScene.SetValue( itString(i_SceneName.GetString().c_str()) );

		//DBG_LOG1("Setting CurrentScene (%s)", itStringUtil::GetStdString(l_pDataObject->m_Data.m_CurrentScene).c_str());
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentCamera( itString& i_CamName )
	{
		l_pDataObject->m_Data.m_CurrentCamera.SetValue( i_CamName );
	}

	void SetCurrentCamera( nameString& i_CamName )
	{
		l_pDataObject->m_Data.m_CurrentCamera.SetValue( itString(i_CamName.GetString().c_str()) );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentSceneFilename( itString& i_SceneFilename )
	{
		l_pDataObject->m_Data.m_CurrentSceneFilename.SetValue( i_SceneFilename );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetSceneAndDirectory( fsLocator& i_DirAndFilename )
	{
		if ( i_DirAndFilename.GetNumNames() > 0 )
		{
			//	set the scene
			itString current_scenename = i_DirAndFilename.GetLastName();
			current_scenename.StripExtension();
			SetCurrentScene( current_scenename );

			// generate and set the directory
			l_pDataObject->m_Data.m_OutputDirectory.SetValue( i_DirAndFilename );
			fsLocator dir = l_pDataObject->m_Data.m_OutputDirectory.GetValue();
			dir.Pop();
			l_pDataObject->m_Data.m_OutputDirectory.SetValue(dir);	// filename

			//	TODO remove this SHOTS restriction
			int index = l_pDataObject->m_Data.m_OutputDirectory.GetValue().FindFirst( itString("Shots") );
			if (index == -1)
			{
				std::string outputname;
				fsFileUtil::LocatorToANSIFilename(i_DirAndFilename, outputname);
				DBG_ASSERT1( index != -1, "Tried to set the scene and directory but a scene path requires the folder Shots to be in the path.  Instead (%s) was passed in.", outputname.c_str() );
				return;
			}
			dir = l_pDataObject->m_Data.m_OutputDirectory.GetValue();
			dir.ReplaceName( itString("Footage"), index );
			l_pDataObject->m_Data.m_OutputDirectory.SetValue(dir);

			//	debug only
			//std::string sdir;
			//fsFileUtil::LocatorToANSIFilename(l_pDataObject->m_Data.m_OutputDirectory, sdir);
			//DBG_LOG1("output dir %s", sdir.c_str());
		}
		else
		{
			l_pDataObject->m_Data.m_OutputDirectory.SetValue( gfPaths::GetPath(mnmPaths::e_SaveFootage) );
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetDirectory( fsLocator& i_Dir )
	{
		l_pDataObject->m_Data.m_OutputDirectory.SetValue( i_Dir );

		//std::string dir;
		//fsFileUtil::LocatorToANSIFilename( l_pDataObject->m_Data.m_Director.SetValue( dir ) );
		//DBG_LOG1( "SCDU: directory= (%s)", dir.c_str() );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentFrame( int i_FrameNumber )
	{
		l_pDataObject->m_Data.m_nCurrentFrame.SetValue( i_FrameNumber );
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
	void GenerateFilename( std::string& o_Filename )
	{
		char temp[256] = "";
		
		// TODO: (?) Allow users to enter text BEFORE the scene/cam/counter
		//temp = l_pDataObject->m_Data.m_Prefix;

		//	Scene
		if ( l_pDataObject->m_Data.m_bUseSceneFilenameInFilename.GetValue() )
		{
			//DBG_LOG1("Generating Filename - scene (%s)", itStringUtil::GetStdString(l_pDataObject->m_Data.m_CurrentScene).c_str());
			DBG_ASSERT0( l_pDataObject->m_Data.m_CurrentScene.GetValue().GetLength() > 0, "scene name is 0" );

			//strcat(temp,"_");
			strcat(temp, itStringUtil::GetStdString(l_pDataObject->m_Data.m_CurrentScene.GetValue()).c_str() );
		}
		//	camera
		if ( l_pDataObject->m_Data.m_bUseCameraNameInFilename.GetValue() )
		{
			//DBG_LOG1("Generating Filename - camera (%s)", itStringUtil::GetStdString(l_pDataObject->m_Data.m_CurrentCamera).c_str());
			DBG_ASSERT0( l_pDataObject->m_Data.m_CurrentCamera.GetValue().GetLength() > 0, "camera name is 0" );

			strcat(temp,"_");
			strcat(temp, itStringUtil::GetStdString(l_pDataObject->m_Data.m_CurrentCamera.GetValue()).c_str() );
		}
		//	compression
		if ( l_pDataObject->m_Data.m_bUseCompressionInFilename.GetValue() && l_pDataObject->m_Data.m_bCaptureMovie.GetValue())
		{
			//DBG_LOG1("Generating Filename - compression (%s)", itStringUtil::GetStdString(l_pDataObject->m_Data.m_Compression).c_str());
			DBG_ASSERT0( l_pDataObject->m_Data.m_CompressCode.GetValue().size() > 0, "compression is 0" );

			strcat(temp,"_");
			//strcat(temp, l_pDataObject->m_Data.m_CompressCode.GetValue().c_str());

			// remove front + back spaces (front won't happen because of "_" before
			std::string::size_type const first = l_pDataObject->m_Data.m_CompressCode.GetValue().find_first_not_of(" \t");
			strcat(temp, l_pDataObject->m_Data.m_CompressCode.GetValue().substr(first, l_pDataObject->m_Data.m_CompressCode.GetValue().find_last_not_of(" \t")-first+1).c_str() );
		}

		//	digits
		if ( !l_pDataObject->m_Data.m_bCaptureMovie.GetValue() )
		{
			char fnm[256];

			strcpy(fnm, temp);
			strcat(fnm,"_%0");
			char num[3];
			sprintf(num,"%d", l_pDataObject->m_Data.m_nCounterDigits.GetValue());
			strcat(fnm,num);
			strcat(fnm,"d");

			//DBG_LOG1( "generate filename (%s)", fnm );

			sprintf(temp, fnm, l_pDataObject->m_Data.m_nCurrentFrame.GetValue() );
		}

		//	mask
		if (l_pDataObject->m_Data.m_bMaskFrame.GetValue())
		{
		//	strcat(temp, "_mask");
		}

		//	return the newly built filename
		o_Filename = temp;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetPrefix( itString& i_Prefix )
	{
		l_pDataObject->m_Data.m_Prefix.SetValue( itStringUtil::GetStdString( i_Prefix ) );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetMaxTime( int i_nMaxTime )
	{
		l_pDataObject->m_Data.m_nCaptureMax.SetValue( i_nMaxTime );
	}

	//------------------------------------------------------------------------
	//	Turn On/off the shadows for capture.  This will also store what the
	//	current shadow state is.	Restore will put the shadow back to it's
	//	state before Enable was called.
	//------------------------------------------------------------------------
	void EnableShadows( bool i_bOn )
	{
		l_bShadowEditorState = g3dSingleLightRendering::GetDoSingleLightRendering();
		g3dSingleLightRendering::SetDoSingleLightRendering( i_bOn );

		l_bShadowQualityState = g3dSingleLightRendering::GetShadowQualityOverride();
		g3dSingleLightRendering::SetShadowQualityOverride( i_bOn );
	}

	void RestoreShadows()
	{
		g3dSingleLightRendering::SetDoSingleLightRendering( l_bShadowEditorState );
		g3dSingleLightRendering::SetShadowQualityOverride( l_bShadowQualityState );
	}

	//--------------------------------------------------------------------
	//	SetOneCameraToRender() - set a single camera to render
	//--------------------------------------------------------------------
	void SetOneCameraToRender(const std::string& i_CamName)
	{
		//	set only the ortho camera
		//
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
		data.m_bCaptureAllCameras.SetValue(false);
		for (int i = 0; i < data.m_CameraList.size(); ++i)
		{
			if (stricmp(data.m_CameraList[i].m_CameraName.GetValue().c_str(), i_CamName.c_str()) == 0)
				data.m_CameraList[i].m_bCapture.SetValue(true);
			else
				data.m_CameraList[i].m_bCapture.SetValue(false);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject()
	{
		return l_pDataObject;
	}

	//------------------------------------------------------------------------
	//	output the struct values at anytime
	//------------------------------------------------------------------------
	void Debug()
	{
		debug_OutputDataValues();
	}
}
