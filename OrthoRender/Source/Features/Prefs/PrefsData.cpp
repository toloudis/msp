/****************************************************************************\
**  PrefsData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2004-7 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/PrefsData.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/Env/envThreadGroup.hpp"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
PrefsData::PrefsData()
:	m_MRUHistory("Most Recently Used",6),
	m_bPlaybackTimeCode("Show TimeCode",false),
	m_bPlaybackLoopAtEnd("Loop at End",false),
	m_bPlaybackLowRes("Low Res Playback",false),
	m_bScrubbingLowRes("Low Res Scrubbing",true),
	m_bAutoSave("Auto-Save",true),
	m_AutoSaveFrequency("Frequency (min)",5),
	m_AutoSaveBackups("Back-Ups per Scene", 5),
	m_UndoLevels("Undo Levels",10),
	m_UndoMemory("Undo Max Memory",0.0f),
	m_BMailLocation("BMail loc"),
	m_MoviePlayerLocation("Movie Player"),
	m_ChannelEditor_DefaultTime("Default Scene Time",10),
	m_RenderWindowWidth("Window Width",720),
	m_RenderWindowHeight("Window Height",576),
	m_TimeFormat("Time Format"),
	m_LastProjectName(),
	m_DefaultProjectName("Default Project"),
	m_DefaultProjectDirectory(),
	m_DefaultSceneName("Default Scene"),
	m_ChannelEditor_SnapActive("Snap Active", true), 
	m_ChannelEditor_SnapAmount("Snap Amount", 0.1f),
	m_DefaultDriverBlend("Default Blend", 0),
	m_bMainTimelineVisible("TimeLine Visible", true),
	m_bMainActionToolbarVisible("Action Toolbar Visible", true),
	m_bMainModeToolbarVisible("Mode Toolbar Visible", true),
	m_bMainStatusBarVisible("Status Bar Visible", true),
	m_bAxisCompassVisible("Axis Compass Visible", true),
	m_bMuteAudio("Mute Audio", false),
	m_GlobalScale("Global Scale", 1.0f),
	m_bFilmGatesVisible("Film Gates Visible", true),
	m_bShowToolTips("Show Tooltips", true),
	m_bOnAddSwitchToPlacedTab("On Add, Go To Placed Tab", true),
	m_bMultithreading("Multithreading", envThreadGroup::GetThreadingEnabled()),
	m_CameraPanRate("Camera Pan Rate", 1.0f)
{
	std::string moviestr("C:\\Program Files\\Windows Media Player\\wmplayer.exe");
	m_MoviePlayerLocation.SetValue(moviestr);

	m_MRUHistory.SetMaximum(10);
	m_AutoSaveBackups.SetMaximum(20);
	m_UndoLevels.SetMaximum(127);

	m_TimeFormat.SetEnumTag(0,"HH:MM:SS.Fr");
	m_TimeFormat.SetEnumTag(1,"HH:MM:SS:MS");
	m_TimeFormat.SetEnumTag(2,"Frames");
	m_TimeFormat.SetEnumTag(3,"Seconds");
	//e_HHMMSSFR	= 0,
	//e_HHMMSSMS	= 1,
	//e_Frames	= 2,
	//e_Seconds	= 3,

}


