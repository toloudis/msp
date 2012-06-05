/*****************************************************************************
**	PrefsMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/PrefsMgr.hpp"

#include "Features/Prefs/PrefsDialogUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/pyth/pythProperty.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/Env/envThreadGroup.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Tool/api3d/api3dScale.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"

#include <string>
#include <vector>



//============================================================================
//============================================================================
//using namespace System::Xml;


//============================================================================
//============================================================================
namespace PrefsMgr
{
	namespace
	{
		const char* lc_Reg_Folder		= "Prefs";
		const char* lc_Key_MRU			= "MRUHistory";
		const char* lc_Key_PTimeCode	= "PlaybackTimeCode";
		const char* lc_Key_PLoopAtEnd	= "PlaybackLoopAtEnd";
		const char* lc_Key_ASave		= "AutoSave";
		const char* lc_Key_ASaveFreq	= "AutoSaveFrequency";
		const char* lc_Key_ASaveBackups	= "AutoSaveBackups";
		const char* lc_Key_WinWidth		= "RenderWindowWidth";
		const char* lc_Key_WinHeight	= "RenderWindowHeight";
		const char* lc_Key_BMailLoc		= "Bmail";
		const char* lc_Key_MoviePlayerLoc = "MoviePlayer";
		const char* lc_Key_CEDefaultTime= "ChannelEditorDefaultTime";
		const char* lc_Key_CurrentLayout = "Layout";
		const char* lc_Key_TimeFormat	= "TimeFormat";
		const char* lc_Key_UndoLevels	= "UndoLevels";
		const char* lc_Key_UndoMemory	= "UndoMemory";
		const char* lc_Key_LastProject	= "LastProject";
		const char* lc_Key_DefaultProject	= "DefaultProject";
		const char* lc_Key_DefaultProjectDir	= "DefaultProjectDir";
		const char* lc_Key_DefaultScene	= "DefaultScene";
		const char* lc_Key_SnapActive	= "SnapActive";
		const char* lc_Key_SnapAmount	= "SnapAmount";
		const char* lc_Key_DriverBlendDefault	= "DriverBlendDefault";
		const char* lc_Key_HotKey		= "HotKeys";
		const char * lc_Key_HotKey_MenuItem		= "Menu Item";
		const char * lc_Key_HotKey_MenuParent	= "Menu Parent";
		const char * lc_Key_HotKey_Combo		= "Keys";
		const char * lc_Key_TimeLine_Visible	= "TimeLineVisible";
		const char * lc_Key_ActionToolbar_Visible	= "ActionToolbarVisible";
		const char * lc_Key_ModeToolbar_Visible	= "ModeToolbarVisible";
		const char * lc_Key_StatusBar_Visible	= "StatusBarVisible";
		const char * lc_Key_AxisCompass_Visible	= "AxisCompassVisible";
		const char * lc_Key_MuteAudio			= "MuteAudio";
		const char * lc_Key_PlaybackLowRes		= "PlaybackLowRes";
		const char * lc_Key_ScrubLowRes			= "ScrubLowRes";
		const char * lc_Key_GlobalScale			= "GlobalScale";
		const char * lc_Key_ShowToolTips		= "ShowToolTips";
		const char * lc_Key_OnAddGoToPlaced		= "OnAddGoToPlaced";
		const char * lc_Key_Multithreading		= "Multithreading";
		const char * lc_Key_CameraPanRate		= "CameraPanRate";
		//const char* lc_Key_FilmGateVisible = "Visible";

		const char * lc_Key_Lightwait_MessagingServer		= "MessagingServer" ;
		const char * lc_Key_Lightwait_MessagingOutputQueue	= "MessagingOutputQueue" ;
		const char * lc_Key_Lightwait_MessagingInputQueue	= "MessagingInputQueue" ;
		const char * lc_Key_Lightwait_MessagingPort			= "MessagingPort" ;
		const char * lc_Key_Lightwait_MessagingWireProtocol	= "MessagingWireProtocol" ;
		const char * lc_Key_Lightwait_MessagingProtocol		= "MessagingProtocol" ;
		const char * lc_Key_Lightwait_MessagingLocalOutputLocation		= "MessagingLocalOutputLocation" ;

		const char * lc_PrefsFileName			= "Prefs.cfg";
		const char * lc_PrefsFileName_Default	= "Prefs-Default.cfg";

		std::vector<prefsDataInterest*>	l_PrefsInterestList;

		bool	l_bDataReadIn = false;

		//====================================================================
		//====================================================================
		class PrefsObject : public prtyObject, public api3dScaleInterest
		{
			public:
				//------------------------------------------------------------
				//------------------------------------------------------------
				PrefsObject()
				{
					RegisterProperties();		
					api3dScale::RegisterScaleInterest( this );
				}
				~PrefsObject()
				{		
					api3dScale::UnRegisterScaleInterest( this );
				}

				//------------------------------------------------------------
				//------------------------------------------------------------
				void WritePrefs();
				void ReadPrefs();
				void ApplyPrefs();

			private:
				//------------------------------------------------------------
				//------------------------------------------------------------
				void ReadPrefsFromConfigFile(fsLocator& i_ConfigFile);
				void WritePrefsToConfigFile();

				//------------------------------------------------------------
				//------------------------------------------------------------
				virtual void RegisterProperties();

				//--------------------------------------------------------------------
				//	GlobalScaleChanged - notification from api3dScale
				//		that global scale has changed.
				//--------------------------------------------------------------------
				virtual void GlobalScaleChanged( float i_Scale );

				//--------------------------------------------------------------------
				// Change callback on global scale property
				//--------------------------------------------------------------------
				void GlobalScalePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

				//--------------------------------------------------------------------
				//	DefaultBlendDriverChanged - notification that the default
				//	blend has changed
				//--------------------------------------------------------------------
				virtual void DefaultBlendDriverChanged( envType::Int8 i_NewDefaultBlendType );
				void DefaultBlendDriverPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				void TimeFormatPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				void MultithreadingPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				void CameraPanRatePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

			public:
				PrefsData m_Data;
		};
		static PrefsObject* l_pPrefsObject = 0;

		
		//====================================================================
		//====================================================================
		class PrefsNameResolver : public pythProperty::NameResolver
		{
			//------------------------------------------------------------
			// Resolve the string "Preferences" into our property object
			// in order to be accessible from python.
			//------------------------------------------------------------
			virtual prtyObject* ResolveName(std::string &i_PropertyObjectName)
			{
				if (i_PropertyObjectName == std::string("Preferences"))
				{
					return l_pPrefsObject;
				}
				return NULL;
			}
		};

		shared_ptr<PrefsNameResolver> l_NameResolver(new PrefsNameResolver);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PrefsObject::ReadPrefsFromConfigFile(fsLocator& i_ConfigFile)
	{
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

		guiXMLTextReader::Open(cfgpath.c_str());

		//	read in the preferences
		//
		guiXMLTextReader::gui_Node_Type node_type;
		std::string keyname, strvalue;
		while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
		{
			if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
			{
				// these are used for remote message processing
				if (keyname == lc_Key_Lightwait_MessagingPort)
					{m_Data.m_Lightwait_MessagingPort.SetValue(strvalue);};
				if (keyname == lc_Key_Lightwait_MessagingServer)
					{m_Data.m_Lightwait_MessagingServer.SetValue(strvalue);};
				if (keyname == lc_Key_Lightwait_MessagingOutputQueue)
					{m_Data.m_Lightwait_MessagingOutputQueue.SetValue(strvalue);};
				if (keyname == lc_Key_Lightwait_MessagingInputQueue)
					{m_Data.m_Lightwait_MessagingInputQueue.SetValue(strvalue);};
				if (keyname == lc_Key_Lightwait_MessagingWireProtocol)
					{m_Data.m_Lightwait_MessagingWireProtocol.SetValue(strvalue);};
				if (keyname == lc_Key_Lightwait_MessagingProtocol)
					{m_Data.m_Lightwait_MessagingProtocol.SetValue(strvalue);};
				if (keyname == lc_Key_Lightwait_MessagingLocalOutputLocation)
					{m_Data.m_Lightwait_MessagingLocalOutputLocation.SetValue(strvalue);};
				//				
				if (keyname == lc_Key_MRU)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MRUHistory.SetValue(value);};
				if (keyname == lc_Key_PTimeCode)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bPlaybackTimeCode.SetValue(value);};
				if (keyname == lc_Key_PLoopAtEnd)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bPlaybackLoopAtEnd.SetValue(value);};
				if (keyname == lc_Key_ASave)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bAutoSave.SetValue(value);};
				if (keyname == lc_Key_ASaveFreq)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_AutoSaveFrequency.SetValue(value);};
				if (keyname == lc_Key_ASaveBackups)
					{envType::Int8 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_AutoSaveBackups.SetValue(value);};
				if (keyname == lc_Key_CEDefaultTime)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_ChannelEditor_DefaultTime.SetValue(value);};
				if (keyname == lc_Key_WinWidth)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RenderWindowWidth.SetValue(value);};
				if (keyname == lc_Key_WinHeight)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_RenderWindowHeight.SetValue(value);};
				if (keyname == lc_Key_CurrentLayout)
					{m_Data.m_CurrentLayout.SetValue(strvalue);};
				if (keyname == lc_Key_BMailLoc)
					{fsLocator value; fsFileUtil::ANSIFilenameToLocator(strvalue, value); m_Data.m_BMailLocation.SetValue(value);};
				if (keyname == lc_Key_MoviePlayerLoc)
					{fsLocator value; fsFileUtil::ANSIFilenameToLocator(strvalue, value); m_Data.m_MoviePlayerLocation.SetValue(value);};
				if (keyname == lc_Key_TimeFormat)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_TimeFormat.SetValue(value);};
				if (keyname == lc_Key_UndoLevels)
					{envType::Int8 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_UndoLevels.SetValue(value);};
				if (keyname == lc_Key_UndoMemory)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_UndoMemory.SetValue(value);};
				if (keyname == lc_Key_LastProject)
					{m_Data.m_LastProjectName.SetValue(strvalue);};
				if (keyname == lc_Key_DefaultProject)
					{m_Data.m_DefaultProjectName.SetValue(strvalue);};
				if (keyname == lc_Key_DefaultProjectDir)
					{m_Data.m_DefaultProjectDirectory.SetValue(strvalue);};
				if (keyname == lc_Key_DefaultScene)
					{m_Data.m_DefaultSceneName.SetValue(strvalue);};
				if (keyname == lc_Key_SnapActive)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_ChannelEditor_SnapActive.SetValue(value);};
				if (keyname == lc_Key_SnapAmount)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_ChannelEditor_SnapAmount.SetValue(value);};
				if (keyname == lc_Key_DriverBlendDefault)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_DefaultDriverBlend.SetValue(value);};
				if (keyname == lc_Key_TimeLine_Visible)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMainTimelineVisible.SetValue(value);};
				if (keyname == lc_Key_ActionToolbar_Visible)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMainActionToolbarVisible.SetValue(value);};
				if (keyname == lc_Key_ModeToolbar_Visible)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMainModeToolbarVisible.SetValue(value);};
				if (keyname == lc_Key_StatusBar_Visible)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMainStatusBarVisible.SetValue(value);};
				if (keyname == lc_Key_AxisCompass_Visible)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bAxisCompassVisible.SetValue(value);};
				if (keyname == lc_Key_MuteAudio)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMuteAudio.SetValue(value);};
				if (keyname == lc_Key_PlaybackLowRes)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bPlaybackLowRes.SetValue(value);};
				if (keyname == lc_Key_ScrubLowRes)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bScrubbingLowRes.SetValue(value);};
				if (keyname == lc_Key_GlobalScale)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_GlobalScale.SetValue(value);};
				if (keyname == lc_Key_ShowToolTips)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bShowToolTips.SetValue(value);};
				if (keyname == lc_Key_OnAddGoToPlaced)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bOnAddSwitchToPlacedTab.SetValue(value);};
				if (keyname == lc_Key_Multithreading)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMultithreading.SetValue(value);};
				if (keyname == lc_Key_CameraPanRate)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_CameraPanRate.SetValue(value);};
				
				//std::map<std::string, bool> m_FilmGateVisible;
			}
		}

		guiXMLTextReader::Close();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PrefsObject::ReadPrefs()
	{
		std::string cfgpath;
		fsLocator cfgdir;
		cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
		cfgdir.Push(lc_PrefsFileName);

		if (fsFileUtil::FileExists(cfgdir))
		{
			ReadPrefsFromConfigFile(cfgdir);
		}
		else
		{
			// no hot key config, so check for default
			cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
			cfgdir.Push( lc_PrefsFileName_Default );
			if (fsFileUtil::FileExists(cfgdir))
			{
				ReadPrefsFromConfigFile(cfgdir);
			}
		}

		//	take the data and apply it as necessary
		//
		ApplyPrefs();

#ifdef _MANAGED
		tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String("Default"); //m_Data.m_CurrentLayout.GetValue().c_str());
#endif
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PrefsObject::ApplyPrefs()
	{
		tmlnDriver::SetDefaultBlendType( (tmlnDriver::BlendType)m_Data.m_DefaultDriverBlend.GetValue() );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//virtual 
	void PrefsObject::RegisterProperties()
	{
		prtyPropertyUIInfo* pPUII;

		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MRUHistory), "Menu", "Set the number of Most Recently Used Files for the Menu");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bPlaybackTimeCode), "Playback", "Display the TimeCode on the screen during playback");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bPlaybackLoopAtEnd), "Playback", "Loop at the end of the scene during playback");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bPlaybackLowRes), "Playback", "Force low-resolution during playback");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bScrubbingLowRes), "Scrubbing", "Force low-resolution during scrubbing");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bAutoSave), "Auto-Save", "Enable Auto-Save feature");
		AddProperty( pPUII );
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AutoSaveFrequency), "Auto-Save", "Set the Auto-Save frequency (in seconds)");
		AddProperty( pPUII );
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AutoSaveBackups), "Auto-Save", "Set the number of back-ups to save for each scene or shot for Auto-Save");
		AddProperty( pPUII );
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_UndoLevels), "Undo", "Number of levels of Undo to keep track of");
		AddProperty( pPUII );
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_UndoMemory), "Undo", "Maximum amount of memory Undo should keep (0 means no limit)");
		AddProperty( pPUII );
		pPUII = new prtyFileChooserUIInfo(&(m_Data.m_BMailLocation), "Rendering", "Location of the BMail executable for sending render reports");
		AddProperty( pPUII );
		pPUII = new prtyFileChooserUIInfo(&(m_Data.m_MoviePlayerLocation), "Rendering", "Location of movie player for quick renders");
		AddProperty( pPUII );
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_ChannelEditor_DefaultTime), "Scene", "Default time for a new scene");
		AddProperty( pPUII );
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_GlobalScale), "Scene", "Global scale for scene");
		AddProperty( pPUII );
		pPUII = new prtyComboBoxUIInfo(&(m_Data.m_TimeFormat), "Interface", "Format time gets displayed in");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_ChannelEditor_SnapActive), "Interface", "Enable drivers to snap when aligned with other drivers");
		AddProperty( pPUII );
		pPUII = new prtyComboBoxUIInfo(&(m_Data.m_DefaultDriverBlend), "Drivers", "Default blend value for drivers");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bShowToolTips), "Interface", "Show tooltips (descriptions) for controls");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bOnAddSwitchToPlacedTab), "Interface", "Switch to Placed Tab after adding an object");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMultithreading), "Threads", "Multithreaded subdivision animation");
		AddProperty( pPUII );
		prtyNumericUpDownUIInfo* pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_ChannelEditor_SnapAmount), "Interface", "Distance before a driver snaps");
		pNUDUII->SetDecimalPlaces(3);
		pNUDUII->SetIncrement(0.01f);
		AddProperty( pNUDUII );
		pPUII = new prtyTextBoxUIInfo(&(m_Data.m_DefaultProjectName), "Default Project", "Initial project");
		pPUII->SetReadOnly(true);
		AddProperty( pPUII );
		pPUII = new prtyTextBoxUIInfo(&(m_Data.m_DefaultSceneName), "Default Project", "Initial scene");
		pPUII->SetReadOnly(true);
		AddProperty( pPUII );
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_CameraPanRate), "Interface", "Speed of camera panning");
		pNUDUII->SetDecimalPlaces(2);
		pNUDUII->SetIncrement(0.5f);
		AddProperty( pNUDUII );

		m_Data.m_GlobalScale.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::GlobalScalePropertyChanged));
		m_Data.m_DefaultDriverBlend.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::DefaultBlendDriverPropertyChanged));
		m_Data.m_TimeFormat.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::TimeFormatPropertyChanged));
		m_Data.m_bMultithreading.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::MultithreadingPropertyChanged));	
		m_Data.m_CameraPanRate.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::CameraPanRatePropertyChanged));	
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PrefsObject::WritePrefs()
	{
		try
		{
			WritePrefsToConfigFile();
		}
		catch (fsInvalidLocatorX& i_Ex)
		{
			fsLocator loc = i_Ex.GetLocator();
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(loc, filename);
			std::string msg = "Cannot write to file\n" + filename + "\nYou might not have permissions.";
			guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PrefsObject::WritePrefsToConfigFile()
	{
		fsLocator cfgdir;
		cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);

		// check if folder exists
		if ( !fsFileUtil::DirectoryExists(cfgdir) )
		{
			fsFileUtil::CreateDirectory(cfgdir);
		}
		

		cfgdir.Push(lc_PrefsFileName);
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(cfgdir,cfgpath);
		


		//	quick and dirty XML Writer
		guiXMLTextWriter::Open(cfgpath.c_str());
		guiXMLTextWriter::WriteStartElement("Preferences");

		//	write the actual values
		guiXMLTextWriter::WriteElement(lc_Key_MRU, m_Data.m_MRUHistory.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_PTimeCode, m_Data.m_bPlaybackTimeCode.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_PLoopAtEnd, m_Data.m_bPlaybackLoopAtEnd.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ASave, m_Data.m_bAutoSave.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ASaveFreq, m_Data.m_AutoSaveFrequency.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ASaveBackups, m_Data.m_AutoSaveBackups.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_CEDefaultTime, m_Data.m_ChannelEditor_DefaultTime.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_WinWidth, m_Data.m_RenderWindowWidth.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_WinHeight, m_Data.m_RenderWindowHeight.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_CurrentLayout, m_Data.m_CurrentLayout.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_BMailLoc, m_Data.m_BMailLocation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MoviePlayerLoc, m_Data.m_MoviePlayerLocation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_TimeFormat, m_Data.m_TimeFormat.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_UndoLevels, m_Data.m_UndoLevels.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_UndoMemory, m_Data.m_UndoMemory.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_LastProject, m_Data.m_LastProjectName.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_DefaultProject, m_Data.m_DefaultProjectName.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_DefaultProjectDir, m_Data.m_DefaultProjectDirectory.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_DefaultScene, m_Data.m_DefaultSceneName.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_SnapActive, m_Data.m_ChannelEditor_SnapActive.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_SnapAmount, m_Data.m_ChannelEditor_SnapAmount.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_TimeLine_Visible, m_Data.m_bMainTimelineVisible.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ActionToolbar_Visible, m_Data.m_bMainActionToolbarVisible.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ModeToolbar_Visible, m_Data.m_bMainModeToolbarVisible.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_StatusBar_Visible, m_Data.m_bMainStatusBarVisible.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_AxisCompass_Visible, m_Data.m_bAxisCompassVisible.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_PlaybackLowRes, m_Data.m_bPlaybackLowRes.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ScrubLowRes, m_Data.m_bScrubbingLowRes.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_GlobalScale, m_Data.m_GlobalScale.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ShowToolTips, m_Data.m_bShowToolTips.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_DriverBlendDefault, m_Data.m_DefaultDriverBlend.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_OnAddGoToPlaced, m_Data.m_bOnAddSwitchToPlacedTab.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_Multithreading, m_Data.m_bMultithreading.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_CameraPanRate, m_Data.m_CameraPanRate.GetValue());
		
		// messaging
		guiXMLTextWriter::WriteElement(lc_Key_Lightwait_MessagingPort, m_Data.m_Lightwait_MessagingPort.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_Lightwait_MessagingServer, m_Data.m_Lightwait_MessagingServer.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_Lightwait_MessagingOutputQueue, m_Data.m_Lightwait_MessagingOutputQueue.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_Lightwait_MessagingInputQueue, m_Data.m_Lightwait_MessagingInputQueue.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_Lightwait_MessagingWireProtocol, m_Data.m_Lightwait_MessagingWireProtocol.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_Lightwait_MessagingProtocol, m_Data.m_Lightwait_MessagingProtocol.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_Lightwait_MessagingLocalOutputLocation, m_Data.m_Lightwait_MessagingLocalOutputLocation.GetValue());

		//std::map<std::string, bool> m_FilmGateVisible;

		//	finish it up
		guiXMLTextWriter::WriteEndElement();
		guiXMLTextWriter::Close();
	}		
	
	//--------------------------------------------------------------------
	//	GlobalScaleChanged - notification that global scale has changed.
	//--------------------------------------------------------------------
	//virtual 
	void PrefsObject::GlobalScaleChanged( float i_Scale )
	{
		m_Data.m_GlobalScale.SetValue( i_Scale );
	}

	void PrefsObject::GlobalScalePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		api3dScale::SetGlobalScale( m_Data.m_GlobalScale.GetValue() );
	}
	
	//--------------------------------------------------------------------
	//	DefaultBlendDriverChanged - notification that the default
	//	blend has changed
	//--------------------------------------------------------------------
	//virtual 
	void PrefsObject::DefaultBlendDriverChanged( envType::Int8 i_NewDefaultBlendType )
	{
		m_Data.m_DefaultDriverBlend.SetValue( i_NewDefaultBlendType );
	}
	void PrefsObject::DefaultBlendDriverPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		tmlnDriver::SetDefaultBlendType( (tmlnDriver::BlendType)m_Data.m_DefaultDriverBlend.GetValue() );
	}

	//------------------------------------------------------------------------
	// Alter time format in tmlnTimeLine
	//------------------------------------------------------------------------
	void PrefsObject::TimeFormatPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		tmlnTimeLine::SetTimeFormat(m_Data.m_TimeFormat.GetValue());
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::MultithreadingPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		envThreadGroup::SetThreadingEnabled( m_Data.m_bMultithreading.GetValue() );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::CameraPanRatePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		cam3dMgr::SetPanRate(m_Data.m_CameraPanRate.GetValue());
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadHotKeys()
	{
		cmaCommandMgr::ReadHotKeys();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WriteHotKeys()
	{
		cmaCommandMgr::WriteHotKeys();
	}

	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp()
	{
		if (l_pPrefsObject)
		{
			// Remove name resolver 
			pythProperty::RemoveNameResolver( l_NameResolver ); 

			delete l_pPrefsObject;
			l_pPrefsObject = NULL;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WritePrefs()
	{
		//
		//	main prefs
		//
		l_pPrefsObject->WritePrefs();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadPrefs()
	{
		//
		//	main prefs
		//
		if ( l_pPrefsObject == 0 )
		{
			l_pPrefsObject = new PrefsObject();

			// Add name resolver now that we have a property object
			pythProperty::AddNameResolver( l_NameResolver ); 
		}

		l_pPrefsObject->ReadPrefs();
	}

	//------------------------------------------------------------------------
	//	apply the prefs
	//------------------------------------------------------------------------
	void ApplyPrefs()
	{
		if (l_pPrefsObject != NULL)
			l_pPrefsObject->ApplyPrefs();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	PrefsData& Data()
	{
		if ( !l_bDataReadIn )
		{
			ReadPrefs();
			l_bDataReadIn = true;
		}

		return l_pPrefsObject->m_Data;
	}

	//------------------------------------------------------------------------
	//	RegisterInterest() - add an interest
	//------------------------------------------------------------------------
	void RegisterInterest( prefsDataInterest* i_pInterest )
	{
		DBG_ASSERT0( i_pInterest != 0, "NULL interest" );

		l_PrefsInterestList.push_back( i_pInterest );
	}

	//------------------------------------------------------------------------
	//	UnRegisterInterest() - remove an interest
	//
	//	Note: this will NOT delete the  interest.  It is up to the
	//	registerer.
	//------------------------------------------------------------------------
	void UnRegisterInterest( prefsDataInterest* i_pInterest )
	{
		DBG_ASSERT0( i_pInterest != 0, "NULL interest" );

		envSTLHelpers::RemoveOneValue( l_PrefsInterestList, i_pInterest );
	}

	//------------------------------------------------------------------------
	//	Clear() - clear the interest list
	//------------------------------------------------------------------------
	void ClearInterests()
	{
		l_PrefsInterestList.clear();
	}

	//------------------------------------------------------------------------
	//	This gets called to let the interests know that the data has changed
	//------------------------------------------------------------------------
	void DataUpdated()
	{
		for (int i = 0; i < l_PrefsInterestList.size(); ++i)
		{
			l_PrefsInterestList[i]->PrefsDataUpdated(l_pPrefsObject->m_Data);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject()
	{
		return l_pPrefsObject;
	}
}
