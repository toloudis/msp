/*****************************************************************************
**	PrefsMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/Prefs/PrefsDialogUtil.hpp"

#include "Drivers/Sound/tmlnDriverSound.hpp"
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrRenderMrayLive.hpp"
#include "Features/Capture/cptrRenderRmanLive.hpp"
#include "Features/Keyframing/keyfKeyframeUtil.hpp"
#include "Features/MaterialBrowse/mbrwDialogUtil.hpp"
#include "Features/ObjectManip/mnpModeObjectManip.hpp"

#include "MainApp/mnmApp.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/pyth/pythProperty.hpp"
#include "Support/rman/rmanMgr.hpp"
#include "Support/mray/mrayMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/Env/envThreadGroup.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/private/fsFileNotifyMgr.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyUnits.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Graphics/Eff/effOcclusionData.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/Mat/matTextureMgr.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/vtx/vtxVertexAnimBudget.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Tool/cam3d/cam3dManipUtil.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"
#include "Tool/icn/icnIconScale.hpp"

#include <string>
#include <vector>

#ifdef USE_WXWIDGETS
#include <wx/tooltip.h>
#endif

#undef CreateDirectory


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
		const char* lc_Key_AutoUpdate   = "AutoUpdate";
		const char* lc_Key_EnableRevision="EnableRevision";
		const char* lc_Key_WinWidth		= "RenderWindowWidth";
		const char* lc_Key_WinHeight	= "RenderWindowHeight";
		const char* lc_Key_BMailLoc		= "Bmail";
		const char* lc_Key_MoviePlayerLoc = "MoviePlayer";
		const char* lc_Key_CEDefaultTime= "ChannelEditorDefaultTime";
		const char* lc_Key_CurrentLayout = "Layout";
		const char* lc_Key_CameraControls	= "CameraControls";
		const char* lc_Key_TimeFormat	= "TimeFormat";
		const char* lc_Key_Units		= "Units";
		const char* lc_Key_IconScale	= "IconScaleLevel";
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
		const char * lc_Key_ScrubFastRender		= "ScrubFastRender";
		const char * lc_Key_GlobalScale			= "GlobalScale";
		const char * lc_Key_ShowToolTips		= "ShowToolTips";
		const char * lc_Key_OnAddGoToPlaced		= "OnAddGoToPlaced";
		const char * lc_Key_ManipFastRender		= "ManipFastRender";
		
		// Single lc_Key_Multithreading replaced by 3 specific controls
		// (keep old key for old file formats)
 		const char * lc_Key_Multithreading		= "Multithreading";
		const char * lc_Key_MultithreadAnimation = "MTAnimation";
		const char * lc_Key_MultithreadCapture	 = "MTCapture";
		const char * lc_Key_MultithreadViewer	 = "MTViewer";

		const char * lc_Key_DuplicateObjectsName= "DuplicateObjectsName";
		//const char * lc_Key_CameraPanRate		= "CameraPanRate";
		const char * lc_Key_VertexAnimBudget	= "VertexAnimBudget";
//		const char * lc_Key_HardwareTessellation= "HardwareTessellation";
//		const char * lc_Key_HardwareTessellationValue = "HardwareTessellationValue";
//		const char * lc_Key_FlatTessellation	= "FlatTessellation";
//		const char * lc_Key_SubdivMode			= "SubdivMode";
		//const char* lc_Key_FilmGateVisible = "Visible";
		const char * lc_Key_SkipTextures		= "SkipTextures";
		const char * lc_Key_CompressTextures    = "CompressTextures";
		const char * lc_Key_TextureReduce		= "TextureReduce";
		const char * lc_Key_DepthMapReduce		= "DepthMapReduce";
		const char * lc_Key_SkipSounds			= "SkipSounds";
		const char * lc_Key_UserPyLoc			= "UserPython";
		const char * lc_Key_MaterialBrowserHome	= "MaterialBrowserHome";
		const char * lc_Key_AutoKey				= "AutoKey";
		const char * lc_Key_PropKeyButtons		= "PropKeyButtons";
		const char * lc_Key_AllChannelsWithDrivers	= "AllChannelsWithDrivers";
		const char * lc_Key_LocalSpaceTranslation	= "LocalSpaceTranslation";
		const char * lc_Key_FrameRate			= "FrameRate";
		const char * lc_PrefsFileName			= "Prefs.cfg";
		const char * lc_PrefsFileName_Default	= "Prefs-Default.cfg";
		const char * lc_Key_PRmanLoc			= "PrmanLocation";
		const char * lc_Key_PixieLoc			= "PixieLocation";
		const char * lc_Key_MRayLoc				= "MentalRayLocation";
		const char * lc_Key_MRayViewerLoc		= "MentalRayViewerLocation";
		const char * lc_Key_ActionSafeFrameVisible	= "ActionSafeFrameVisible";
		const char * lc_Key_TitleSafeFrameVisible	= "TitleSafeFrameVisible";
		
		std::vector<prefsDataInterest*>	l_PrefsInterestList;

		bool	l_bDataReadIn = false;

		//====================================================================
		//====================================================================
		class PrefsObject : public prtyObject
		{
			public:
				//------------------------------------------------------------
				//------------------------------------------------------------
				PrefsObject()
				{
					RegisterProperties();		
				}
				~PrefsObject()
				{		
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
				void CameraControlsPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
				void TimeFormatPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
				void UnitsPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
				void IconScalePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
				void ShowToolTipsPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
				void MultithreadingPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
				//void CameraPanRatePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
				void VertexAnimBudgetPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
//				void HardwareTessellationPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
//				void HardwareTessellationValuePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
//				void FlatTessellationPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
//				void SubdivModePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateSkipTextures(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateCompressTextures(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateTextureReduce(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateSkipSounds(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateAutoSave(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateUndo(prtyProperty *i_pProperty, bool i_bDirty);
				void PrefsObject::AllChannelsWithDriversPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
				void UpdateManipFastRender(prtyProperty* i_pProperty, bool i_bDirty);
				void FrameratePropertyChanged(prtyProperty* i_pProperty, bool i_bDirty);
				void RenderManRendererChanged(prtyProperty* i_pProperty, bool i_bDirty);
				void UpdateAutoUpdate(prtyProperty* i_pProperty, bool i_bDirty);
				void EnableRevision(prtyProperty* i_pProperty, bool i_bDirty);
				
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

		//==========================================================================
		//==========================================================================
		class PrefsTextureAdjuster : public matTextureAdjuster
		{
			//------------------------------------------------------------------------
			//	Implementation of matTextureAdjuster virtual function:
			//	GetReduce returns the amount the texture should be reduced in 
			//	each dimension
			//------------------------------------------------------------------------
			virtual void GetReduce(const fsLocator& i_Locator, 
								   int& o_WidthReduce, 
								   int& o_HeightReduce) const
			{
				// Make sure font for 3d window is not reduced
				if (i_Locator.GetLastName() == itString(mnmConstants::c_ViewerFontName))
					o_WidthReduce = o_HeightReduce = 0;
				else
					o_WidthReduce = o_HeightReduce = PrefsMgr::Data().m_TextureReduce.GetValue();
			}
		};
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
				if (keyname == lc_Key_MRU)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MRUHistory.SetValue(value);};
				if (keyname == lc_Key_PTimeCode)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bPlaybackTimeCode.SetValue(value);};
				if (keyname == lc_Key_PLoopAtEnd)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bPlaybackLoopAtEnd.SetValue(value);};
				if (keyname == lc_Key_ASave)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bAutoSave.SetValue(value);};
				if (keyname == lc_Key_ASaveFreq)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_AutoSaveFrequencyInMinutes.SetValue(value);};
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
				if (keyname == lc_Key_CameraControls)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_CameraControls.SetValue(value);};
				if (keyname == lc_Key_TimeFormat)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_TimeFormat.SetValue(value);};
				if (keyname == lc_Key_Units)
					{m_Data.m_Units.SetValue( prtyUnits::GetUnitsByShortString(strvalue));};	
				if (keyname == lc_Key_IconScale)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_IconScale.SetValue(value);};
				//if (keyname == lc_Key_UndoLevels)
				//	{envType::Int8 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_UndoLevels.SetValue(value);};
				//if (keyname == lc_Key_UndoMemory)
				//	{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_UndoMemory.SetValue(value);};
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
				if (keyname == lc_Key_ScrubFastRender)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bScrubbingFastRender.SetValue(value);};
				if (keyname == lc_Key_GlobalScale)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_GlobalScale.SetValue(value);};
				if (keyname == lc_Key_ShowToolTips)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bShowToolTips.SetValue(value);};
				if (keyname == lc_Key_OnAddGoToPlaced)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bOnAddSwitchToPlacedTab.SetValue(value);};
				if (keyname == lc_Key_ManipFastRender)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bManipFastRender.SetValue(value);};

				// lc_Key_Multithreading used to be the only key. Now there are 3, so if
				// the original key is read, map it to the newer m_bMultithreadAnimation variable.
 				if (keyname == lc_Key_Multithreading)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMultithreadAnimation.SetValue(value);};
				if (keyname == lc_Key_MultithreadAnimation)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMultithreadAnimation.SetValue(value);};
				if (keyname == lc_Key_MultithreadCapture)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMultithreadCapture.SetValue(value);};
				if (keyname == lc_Key_MultithreadViewer)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMultithreadViewer.SetValue(value);};


				if (keyname == lc_Key_DuplicateObjectsName)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bDuplicateObjectsName.SetValue(value);};
//				if (keyname == lc_Key_CameraPanRate)
//					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_CameraPanRate.SetValue(value);};
				if (keyname == lc_Key_VertexAnimBudget)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_VertexAnimationBudget.SetValue(value);};
//				if (keyname == lc_Key_HardwareTessellation)
//					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bUseHardwareTessellation.SetValue(value);};
//				if (keyname == lc_Key_HardwareTessellationValue)
//					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_HardwareTessellationValue.SetValue(value);};
//				if (keyname == lc_Key_FlatTessellation)
//					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bFlatTessellation.SetValue(value);};
//				if (keyname == lc_Key_SubdivMode)
//					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_SubdivMode.SetValue(value);};
				if (keyname == lc_Key_SkipTextures)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bNeverLoadTextures.SetValue(value);};
				if (keyname == lc_Key_TextureReduce)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_TextureReduce.SetValue(value);};
				if (keyname == lc_Key_DepthMapReduce)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_DepthMapReduce.SetValue(value);};
				if (keyname == lc_Key_SkipSounds)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bNeverLoadSounds.SetValue(value);};
				//std::map<std::string, bool> m_FilmGateVisible;
				if (keyname == lc_Key_UserPyLoc)
					{fsLocator value; fsFileUtil::ANSIFilenameToLocator(strvalue, value); m_Data.m_UserPyLocation.SetValue(value);};
				if (keyname == lc_Key_MaterialBrowserHome)
					{fsLocator value; fsFileUtil::ANSIFilenameToLocator(strvalue, value); m_Data.m_MaterialBrowserHome.SetValue(value);};
				if (keyname == lc_Key_AutoKey)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bAutoKey.SetValue(value);};
				if (keyname == lc_Key_AutoUpdate)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableAutoUpdate.SetValue(value);};
				if (keyname == lc_Key_EnableRevision)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableRevision.SetValue(value);};
				if (keyname == lc_Key_PropKeyButtons)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bPropKeyButtons.SetValue(value);};
				if (keyname == lc_Key_AllChannelsWithDrivers)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bAllChannelsWithDrivers.SetValue(value);};
				if (keyname == lc_Key_LocalSpaceTranslation)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bLocalSpaceTranslation.SetValue(value);};
				if (keyname == lc_Key_FrameRate)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_FrameRate.SetValue(value);};
				if (keyname == lc_Key_PRmanLoc)
					{fsLocator value; fsFileUtil::ANSIFilenameToLocator(strvalue, value); m_Data.m_PRmanLocation.SetValue(value);};
				if (keyname == lc_Key_PixieLoc)
					{fsLocator value; fsFileUtil::ANSIFilenameToLocator(strvalue, value); m_Data.m_PixieLocation.SetValue(value);};
				if (keyname == lc_Key_MRayLoc)
					{fsLocator value; fsFileUtil::ANSIFilenameToLocator(strvalue, value); m_Data.m_MRayLocation.SetValue(value);};
				if (keyname == lc_Key_MRayViewerLoc)
					{fsLocator value; fsFileUtil::ANSIFilenameToLocator(strvalue, value); m_Data.m_MRayViewerLocation.SetValue(value);};
				if (keyname == lc_Key_ActionSafeFrameVisible)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bActionSafeFrameVisible.SetValue(value);};
				if (keyname == lc_Key_TitleSafeFrameVisible)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bTitleSafeFrameVisible.SetValue(value);};
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
		DBG_TRACE("Prefs.cfg (read)=" << cfgdir);

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

	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PrefsObject::ApplyPrefs()
	{
		tmlnDriver::SetDefaultBlendType( (tmlnDriver::BlendType)m_Data.m_DefaultDriverBlend.GetValue() );
		vtxVertexAnimBudget::SetBudgetLimitInMBs( m_Data.m_VertexAnimationBudget.GetValue() ); // Confirm the defaults are the same
//		smdlSubdivCharacter::SetSubdivMode( (smdlSubdivCharacter::SubdivMode)m_Data.m_SubdivMode.GetValue() );
		smdlSubdivCharacter::SetSubdivMode( smdlSubdivCharacter::e_SubdivCatmullClark );
//		smdlSubdivCharacter::SetSubdivMode( smdlSubdivCharacter::e_SubdivPatch );//(smdlSubdivCharacter::SubdivMode) m_Data.m_SubdivMode.GetValue() );
//		smdlSubdivCharacter::SetSubdivMode( smdlSubdivCharacter::e_SubdivCatmullClark );//(smdlSubdivCharacter::SubdivMode) m_Data.m_SubdivMode.GetValue() );
		keyfKeyframeUtil::SetAutoKey( m_Data.m_bAutoKey.GetValue() );		
		cmmObjectDialogUtil::SetViewPropertyKeyButtons( m_Data.m_bPropKeyButtons.GetValue() );
		mnpModeObjectManip::SetLocalSpaceTranslation( m_Data.m_bLocalSpaceTranslation.GetValue() );
		mbrwDialogUtil::SetInitialDirectory( m_Data.m_MaterialBrowserHome.GetValue() );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//virtual 
	void PrefsObject::RegisterProperties()
	{
		prtyPropertyUIInfo* pPUII;
		prtyNumericUpDownUIInfo* pNUDUII;

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MRUHistory), "Menu", "Set the number of Most Recently Used Files for the Menu");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetIncrement(1.0f);
		AddProperty( pNUDUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableAutoUpdate), "Auto-Update", "Enable Auto-Update feature");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableRevision), "Auto-Update", "Enable Revision");
		AddProperty( pPUII );
		
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bPlaybackTimeCode), "Playback", "Display the TimeCode on the screen during playback");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bPlaybackLoopAtEnd), "Playback", "Loop at the end of the scene during playback");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bPlaybackLowRes), "Playback", "Force low-resolution during playback");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bScrubbingLowRes), "Scrubbing", "Force low-resolution during scrubbing");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bScrubbingFastRender), "Scrubbing", "Force fast rendering during scrubbing");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bAutoSave), "Auto-Save", "Enable Auto-Save feature");
		AddProperty( pPUII );
		
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AutoSaveFrequencyInMinutes), "Auto-Save", "Set the Auto-Save frequency (in minutes)");
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(88400);
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetIncrement(1.0f);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );
		
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_AutoSaveBackups), "Auto-Save", "Set the number of backups to save for each scene or shot for Auto-Save");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(8);
		pNUDUII->SetIncrement(1.0f);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );
		//pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_UndoLevels), "Undo", "Number of levels of Undo to keep track of");
		//AddProperty( pPUII );
		//pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_UndoMemory), "Undo", "Maximum amount of memory Undo should keep (0 means no limit)");
		//AddProperty( pPUII );
		pPUII = new prtyFileChooserUIInfo(&(m_Data.m_BMailLocation), "Rendering", "Location of the BMail executable for sending render reports");
		AddProperty( pPUII );
		pPUII = new prtyFileChooserUIInfo(&(m_Data.m_MoviePlayerLocation), "Rendering", "Location of movie player for quick renders");
		AddProperty( pPUII );
		pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_ChannelEditor_DefaultTime), "Scene", "Default time for a new scene");
		AddProperty( pPUII );
		//pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_GlobalScale), "Scene", "Global scale for scene");
		//AddProperty( pPUII );
		pPUII = new prtyComboBoxUIInfo(&(m_Data.m_CameraControls), "Interface", "Camera Controls to use in the Viewport");
		AddProperty( pPUII );
		pPUII = new prtyComboBoxUIInfo(&(m_Data.m_TimeFormat), "Interface", "Format time gets displayed in");
		AddProperty( pPUII );
		pPUII = new prtyComboBoxUIInfo(&(m_Data.m_Units), "Interface", "Units for display");
		AddProperty( pPUII );
		pPUII = new prtyComboBoxUIInfo(&(m_Data.m_IconScale), "Interface", "IconScale for display");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_ChannelEditor_SnapActive), "Interface", "Enable drivers to snap when aligned with other drivers");
		AddProperty( pPUII );
		//pPUII = new prtyComboBoxUIInfo(&(m_Data.m_DefaultDriverBlend), "Drivers", "Default blend value for drivers");
		//AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bShowToolTips), "Interface", "Show tooltips (descriptions) for controls");
		AddProperty( pPUII );
		//pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bOnAddSwitchToPlacedTab), "Interface", "Switch to Placed Tab after adding an object");
		//AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMultithreadAnimation), "Threads", "Multithreaded subdivision animation");
 		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMultithreadCapture), "Threads", "Multithreaded rendering to file");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMultithreadViewer), "Threads", "Multithreaded viewport rendering");
		AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bDuplicateObjectsName), "Scene", "Duplicate objects keep name and increment ending number");
		AddProperty( pPUII );
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_ChannelEditor_SnapAmount), "Interface", "Distance before a driver snaps");
		pNUDUII->SetDecimalPlaces(3);
		pNUDUII->SetIncrement(0.01f);
		AddProperty( pNUDUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bManipFastRender), "Interface", "Faster render when manipulating camera");
		AddProperty( pPUII );

		//pPUII = new prtyTextBoxUIInfo(&(m_Data.m_DefaultProjectName), "Default Project", "Initial project");
		//pPUII->SetReadOnly(true);
		//AddProperty( pPUII );
		//pPUII = new prtyTextBoxUIInfo(&(m_Data.m_DefaultSceneName), "Default Project", "Initial scene");
		//pPUII->SetReadOnly(true);
		//AddProperty( pPUII );
		//pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_CameraPanRate), "Interface", "Speed of camera panning");
		//pNUDUII->SetDecimalPlaces(2);
		//pNUDUII->SetIncrement(0.5f);
		//AddProperty( pNUDUII );		
		pPUII = new prtyFloatEditUIInfo(&(m_Data.m_VertexAnimationBudget), "Vertex Animation", "Amounts of MBs of memory to use for vertex animation at once");
		AddProperty( pPUII );
//		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bUseHardwareTessellation), "Tessellation", "Use hardware tessellation, if available");
//		AddProperty( pPUII );
//		prtyRangedFloatUIInfo* pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_HardwareTessellationValue), "Tessellation", "Tessellation Value");
//		pRFUII->SetMinimum( 1.0f );
//		pRFUII->SetMaximum( 14.9999f );
//		AddProperty( pRFUII );
//		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bFlatTessellation), "Tessellation", "Disable PN Smoothing");
//		AddProperty( pPUII );
//		pPUII = new prtyComboBoxUIInfo(&(m_Data.m_SubdivMode), "Tessellation", "Subdivision method to use for smoothing");
//		AddProperty( pPUII );
		// Loading
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bNeverLoadTextures), "Loading", "Skip loading all textures. Saves video and system memory");
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bAutoDXTCompress), "Loading", "Auto Compress Textures.");
		AddProperty( pPUII );
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_TextureReduce), "Loading", "Amount to reduce size of textures when loading. Only numbers between 0 and 31 are valid. Each level divides side length by 2 and memory use by 4.");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(8);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_DepthMapReduce), "Loading", "Amount to reduce size of depth maps. Each level reduces a side by 2 and memory use by 4.");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(8);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bNeverLoadSounds), "Loading", "Don't load any sounds. ");
		AddProperty( pPUII );
		pPUII = new prtyFileChooserUIInfo(&(m_Data.m_UserPyLocation), "Python", "Location of the UserStartup.py to load user-created python scripts");
#if( SGPU_APP == MS_CORE )
		pPUII->SetReadOnly(true);
#endif
		AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bAllChannelsWithDrivers), "Keying", "Key All Altered Properties.");
		AddProperty( pPUII );
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_FrameRate), "Rendering", "FrameRate");
		pNUDUII->SetDecimalPlaces(2);
		pNUDUII->SetMinimum(0.01f);		// arbitrary
		pNUDUII->SetIncrement(1.0f);
		AddProperty( pNUDUII );

		pPUII = new prtyFileChooserUIInfo(&(m_Data.m_PRmanLocation), "RenderMan", "Location of PRMan renderer");
		AddProperty( pPUII );

		pPUII = new prtyFileChooserUIInfo(&(m_Data.m_MRayLocation), "mental ray", "Location of Mental Ray renderer");
		AddProperty( pPUII );

		pPUII = new prtyFileChooserUIInfo(&(m_Data.m_MRayViewerLocation), "mental ray", "Location of Mental Ray viewer");
		AddProperty( pPUII );

		//pPUII = new prtyFileChooserUIInfo(&(m_Data.m_PixieLocation), "RenderMan Renderer", "Location of Pixie renderer");
		//AddProperty( pPUII );

		//	Register callbacks for items that need to be updated immediately.
		//
		m_Data.m_GlobalScale.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::GlobalScalePropertyChanged));
		m_Data.m_DefaultDriverBlend.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::DefaultBlendDriverPropertyChanged));
		m_Data.m_CameraControls.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::CameraControlsPropertyChanged));
		m_Data.m_TimeFormat.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::TimeFormatPropertyChanged));
		m_Data.m_Units.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::UnitsPropertyChanged));
		m_Data.m_IconScale.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::IconScalePropertyChanged));
		m_Data.m_bShowToolTips.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::ShowToolTipsPropertyChanged));	

		m_Data.m_bMultithreadAnimation.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::MultithreadingPropertyChanged));	
		m_Data.m_bMultithreadCapture.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::MultithreadingPropertyChanged));	
		m_Data.m_bMultithreadViewer.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::MultithreadingPropertyChanged));	

		//m_Data.m_CameraPanRate.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::CameraPanRatePropertyChanged));	
		m_Data.m_VertexAnimationBudget.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::VertexAnimBudgetPropertyChanged));	
//		m_Data.m_bUseHardwareTessellation.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::HardwareTessellationPropertyChanged));	
//		m_Data.m_HardwareTessellationValue.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::HardwareTessellationValuePropertyChanged));	
//		m_Data.m_bFlatTessellation.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::FlatTessellationPropertyChanged));	
//		m_Data.m_SubdivMode.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::SubdivModePropertyChanged));	
		m_Data.m_bNeverLoadTextures.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::UpdateSkipTextures));
		m_Data.m_bAutoDXTCompress.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::UpdateCompressTextures));
		m_Data.m_TextureReduce.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::UpdateTextureReduce));
		m_Data.m_bNeverLoadSounds.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::UpdateSkipSounds));
		m_Data.m_bAutoSave.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::UpdateAutoSave));
		m_Data.m_AutoSaveFrequencyInMinutes.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::UpdateAutoSave));
		m_Data.m_AutoSaveBackups.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::UpdateAutoSave));
		//m_Data.m_UndoLevels.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::UpdateUndo));
		//m_Data.m_UndoMemory.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::UpdateUndo));
		m_Data.m_bAllChannelsWithDrivers.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::AllChannelsWithDriversPropertyChanged));
		m_Data.m_bManipFastRender.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::UpdateManipFastRender));
		m_Data.m_FrameRate.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::FrameratePropertyChanged));
		m_Data.m_PRmanLocation.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::RenderManRendererChanged));
		m_Data.m_PixieLocation.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::RenderManRendererChanged));
		m_Data.m_MRayLocation.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::RenderManRendererChanged));
		m_Data.m_MRayViewerLocation.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::RenderManRendererChanged));
		m_Data.m_bEnableAutoUpdate.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::UpdateAutoUpdate));
		m_Data.m_bEnableRevision.AddCallback(new prtyCallbackWrapper<PrefsObject>(this, &PrefsObject::EnableRevision));
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PrefsObject::WritePrefs()
	{
		try
		{
			// Update any values that are not tracked in UI
			m_Data.m_MaterialBrowserHome.SetValue( mbrwDialogUtil::GetInitialDirectory() );

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
		DBG_TRACE("Prefs.cfg (write)=" << cfgdir);

		//	quick and dirty XML Writer
		guiXMLTextWriter::Open(cfgpath.c_str());
		guiXMLTextWriter::WriteStartElement("Preferences");

		//	write the actual values
		guiXMLTextWriter::WriteElement(lc_Key_MRU, m_Data.m_MRUHistory.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_PTimeCode, m_Data.m_bPlaybackTimeCode.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_PLoopAtEnd, m_Data.m_bPlaybackLoopAtEnd.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ASave, m_Data.m_bAutoSave.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ASaveFreq, m_Data.m_AutoSaveFrequencyInMinutes.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ASaveBackups, m_Data.m_AutoSaveBackups.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_CEDefaultTime, m_Data.m_ChannelEditor_DefaultTime.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_WinWidth, m_Data.m_RenderWindowWidth.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_WinHeight, m_Data.m_RenderWindowHeight.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_CurrentLayout, m_Data.m_CurrentLayout.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_BMailLoc, m_Data.m_BMailLocation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MoviePlayerLoc, m_Data.m_MoviePlayerLocation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_CameraControls, m_Data.m_CameraControls.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_TimeFormat, m_Data.m_TimeFormat.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_Units, prtyUnits::GetShortString((prtyUnits::UnitTypes)m_Data.m_Units.GetValue()));
		guiXMLTextWriter::WriteElement(lc_Key_IconScale, m_Data.m_IconScale.GetValue());
		//guiXMLTextWriter::WriteElement(lc_Key_UndoLevels, m_Data.m_UndoLevels.GetValue());
		//guiXMLTextWriter::WriteElement(lc_Key_UndoMemory, m_Data.m_UndoMemory.GetValue());
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
		guiXMLTextWriter::WriteElement(lc_Key_ScrubFastRender, m_Data.m_bScrubbingFastRender.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_GlobalScale, m_Data.m_GlobalScale.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ShowToolTips, m_Data.m_bShowToolTips.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_DriverBlendDefault, m_Data.m_DefaultDriverBlend.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_OnAddGoToPlaced, m_Data.m_bOnAddSwitchToPlacedTab.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ManipFastRender, m_Data.m_bManipFastRender.GetValue());

		guiXMLTextWriter::WriteElement(lc_Key_MultithreadAnimation, m_Data.m_bMultithreadAnimation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MultithreadCapture, m_Data.m_bMultithreadCapture.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MultithreadViewer, m_Data.m_bMultithreadViewer.GetValue());

		guiXMLTextWriter::WriteElement(lc_Key_DuplicateObjectsName, m_Data.m_bDuplicateObjectsName.GetValue());
//		guiXMLTextWriter::WriteElement(lc_Key_CameraPanRate, m_Data.m_CameraPanRate.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_VertexAnimBudget, m_Data.m_VertexAnimationBudget.GetValue());
//		guiXMLTextWriter::WriteElement(lc_Key_HardwareTessellation, m_Data.m_bUseHardwareTessellation.GetValue());
//		guiXMLTextWriter::WriteElement(lc_Key_HardwareTessellationValue, m_Data.m_HardwareTessellationValue.GetValue());
//		guiXMLTextWriter::WriteElement(lc_Key_FlatTessellation, m_Data.m_bFlatTessellation.GetValue());
//		guiXMLTextWriter::WriteElement(lc_Key_SubdivMode, m_Data.m_SubdivMode.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_SkipTextures, m_Data.m_bNeverLoadTextures.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_CompressTextures, m_Data.m_bAutoDXTCompress.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_TextureReduce, m_Data.m_TextureReduce.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_DepthMapReduce, m_Data.m_DepthMapReduce.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_SkipSounds, m_Data.m_bNeverLoadSounds.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_UserPyLoc, m_Data.m_UserPyLocation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MaterialBrowserHome, m_Data.m_MaterialBrowserHome.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_AutoKey, m_Data.m_bAutoKey.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_AutoUpdate, m_Data.m_bEnableAutoUpdate.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_EnableRevision, m_Data.m_bEnableRevision.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_PropKeyButtons, m_Data.m_bPropKeyButtons.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_AllChannelsWithDrivers, m_Data.m_bAllChannelsWithDrivers.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_LocalSpaceTranslation, m_Data.m_bLocalSpaceTranslation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_FrameRate, m_Data.m_FrameRate.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_PRmanLoc, m_Data.m_PRmanLocation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_PixieLoc, m_Data.m_PixieLocation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MRayLoc, m_Data.m_MRayLocation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MRayViewerLoc, m_Data.m_MRayViewerLocation.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_ActionSafeFrameVisible, m_Data.m_bActionSafeFrameVisible.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_TitleSafeFrameVisible, m_Data.m_bTitleSafeFrameVisible.GetValue());

		//	finish it up
		guiXMLTextWriter::WriteEndElement();
		guiXMLTextWriter::Close();
	}		
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::GlobalScalePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		//icnIconScale::SetGlobalScale( m_Data.m_GlobalScale.GetValue() );
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
	void PrefsObject::CameraControlsPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		cam3dManipUtil::SetCameraControlMode( m_Data.m_CameraControls.GetValue());
	}

	//------------------------------------------------------------------------
	// Alter time format in tmlnTimeLine
	//------------------------------------------------------------------------
	void PrefsObject::TimeFormatPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		tmlnTimeLine::SetTimeFormat(m_Data.m_TimeFormat.GetValue());
	}

	//------------------------------------------------------------------------
	// Alter units display of properties
	//------------------------------------------------------------------------
	void PrefsObject::UnitsPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		// assuming direct mapping of enumerations
		prtyUnits::SetUnitSystem((prtyUnits::UnitTypes)m_Data.m_Units.GetValue());
	}

	//------------------------------------------------------------------------
	// Alter scale of units through 3 different categories
	//------------------------------------------------------------------------
	void PrefsObject::IconScalePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		icnIconScale::SetIconScaleLevel((icnIconScale::IconScaleLevel)m_Data.m_IconScale.GetValue());
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::ShowToolTipsPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
#ifdef USE_WXWIDGETS
		wxToolTip::Enable( m_Data.m_bShowToolTips.GetValue() );
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::MultithreadingPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		envThreadGroup::SetThreadingEnabled( m_Data.m_bMultithreadAnimation.GetValue() );
		cptrModeRender::SetThreadingEnabled( m_Data.m_bMultithreadCapture.GetValue() );
		mnmApp::SetThreadingEnabled( m_Data.m_bMultithreadViewer.GetValue() );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//void PrefsObject::CameraPanRatePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	//{
	//	cam3dMgr::SetPanRate(m_Data.m_CameraPanRate.GetValue());
	//}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::VertexAnimBudgetPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		vtxVertexAnimBudget::SetBudgetLimitInMBs( m_Data.m_VertexAnimationBudget.GetValue() );
	}
/*
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::HardwareTessellationPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		// This probably should be in g3d somewhere so that multiple
		// classes throughout the library can use the flag.
//		smdlTessellatorBase::SetHardwareTessellate( m_Data.m_bUseHardwareTessellation.GetValue() );
//		api3dSubdiv::SetSubdivLevel( api3dSubdiv::GetSubdivLevel() );		//smdlPatchSurface will update based on HardwareTessellate state.
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::HardwareTessellationValuePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
//		smdlTessellatorBase::SetTessellateValue( m_Data.m_HardwareTessellationValue.GetValue() );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::FlatTessellationPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
		// This probably should be in g3d somewhere so that multiple
		// classes throughout the library can use the flag.
//		smdlTessellatorBase::SetFlatTessellate( m_Data.m_bFlatTessellation.GetValue() );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::SubdivModePropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
//		smdlSubdivCharacter::SetSubdivMode( (smdlSubdivCharacter::SubdivMode)m_Data.m_SubdivMode.GetValue() );
//		smdlSubdivCharacter::SetSubdivMode( smdlSubdivCharacter::e_SubdivPatch );//(smdlSubdivCharacter::SubdivMode) m_Data.m_SubdivMode.GetValue() );
//		smdlSubdivCharacter::SetSubdivMode( smdlSubdivCharacter::e_SubdivCatmullClark );//(smdlSubdivCharacter::SubdivMode) m_Data.m_SubdivMode.GetValue() );
	}
*/
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::UpdateSkipTextures(prtyProperty *i_pProperty, bool i_bDirty)
	{	
		matTextureMgr::SetSkipAllTextures(m_Data.m_bNeverLoadTextures.GetValue());
		//effOcclusionData::SetSkipTextureOverride(m_Data.m_bAlwaysLoadAOTextures.GetValue());
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::UpdateCompressTextures(prtyProperty *i_pProperty, bool i_bDirty)
	{	
		matTextureMgr::SetCompressTextures(m_Data.m_bAutoDXTCompress.GetValue());
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::UpdateTextureReduce(prtyProperty *i_pProperty, bool i_bDirty)
	{	
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::UpdateSkipSounds(prtyProperty *i_pProperty, bool i_bDirty)
	{	
		tmlnDriverSound::SetSkipSounds(m_Data.m_bNeverLoadSounds.GetValue());
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::UpdateAutoSave(prtyProperty *i_pProperty, bool i_bDirty)
	{	
			mnmAutoSaveMgr::Update( m_Data.m_bAutoSave.GetValue(), m_Data.m_AutoSaveFrequencyInMinutes.GetValue(), m_Data.m_AutoSaveBackups.GetValue() );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PrefsObject::UpdateAutoUpdate(prtyProperty *i_pProperty, bool i_bDirty)
	{	
		FileUpdated::SetAutoUpdate(m_Data.m_bEnableAutoUpdate.GetValue());
	}

	void PrefsObject::EnableRevision(prtyProperty *i_pProperty, bool i_bDirty)
	{	
		FileUpdated::SetEnableRevisionFlag(m_Data.m_bEnableRevision.GetValue());
	}

	void PrefsObject::AllChannelsWithDriversPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
	{
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//void PrefsObject::UpdateUndo(prtyProperty *i_pProperty, bool i_bDirty)
	//{	
	//	undoUndoMgr::SetLimits( m_Data.m_UndoMemory.GetValue(), m_Data.m_UndoLevels.GetValue() );
	//}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PrefsObject::UpdateManipFastRender(prtyProperty* i_pProperty, bool i_bDirty)
	{
		cam3dMgr::SetManipFastRender(m_Data.m_bManipFastRender.GetValue());
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PrefsObject::FrameratePropertyChanged(prtyProperty* i_pProperty, bool i_bDirty)
	{
		float fps = m_Data.m_FrameRate.GetValue();

		//	if the frame rate is valid, set it
		//
		if (fps > 0.0f)		// should there be an upper bound?
		{
			g3dConstants::c_fDefaultFrameRate = fps;
			tmlnTimeLine::SetFPS(fps);
			DataUpdated();
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PrefsObject::RenderManRendererChanged(prtyProperty* i_pProperty, bool i_bDirty)
	{
		rmanMgr::SetPrmanLoc( m_Data.m_PRmanLocation.GetValue() );

		mrayMgr::SetMRayLoc( m_Data.m_MRayLocation.GetValue() );
		cptrRenderMrayLive::SetMRayLoc( m_Data.m_MRayLocation.GetValue() );

		mrayMgr::SetMRayViewerLoc( m_Data.m_MRayViewerLocation.GetValue() );
		cptrRenderMrayLive::SetMRayViewerLoc( m_Data.m_MRayViewerLocation.GetValue() );

		//rmanMgr::SetPixieLoc( m_Data.m_PixieLocation.GetValue() );
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

			// Set the texture adjuster to this pointer so that we can control 
			// the reduction of loaded textures
			matTextureMgr::SetTextureAdjuster( new PrefsTextureAdjuster() );
			
			// Leaving the property "allow missing textures" as always on now
			matTextureMgr::SetAllowNullTextures(true);
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
	//------------------------------------------------------------------------
	PrefsData& GetDataSimple()
	{
		return l_pPrefsObject->m_Data;
	}

	//------------------------------------------------------------------------
	//	RegisterInterest() - add an interest
	//------------------------------------------------------------------------
	void RegisterInterest( prefsDataInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "NULL interest" );

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
		DBG_ASSERT( i_pInterest != 0, "NULL interest" );

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
