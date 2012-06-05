//****************************************************************************
//	PrefsData.hpp
//
//	Preferences Data
//
//	StudioGPU
//	Copyright(c) 2004-7 - All Rights Reserved
//****************************************************************************
#include "Features/Prefs/PrefsData.hpp"
#include "Features/ObjectManip/mnpModeObjectManip.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/Env/envThreadGroup.hpp"
#include "Core/prty/prtyUnits.hpp"
#include "Tool/icn/icnIconScale.hpp"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
PrefsData::PrefsData()
:	m_MRUHistory("Most Recently Used",6),
	m_bPlaybackTimeCode("Show TimeCode",false),
	m_bPlaybackLoopAtEnd("Loop at End",false),
	m_bPlaybackLowRes("Low Res Playback",false),
	m_bScrubbingLowRes("Low Res Scrubbing",true),
	m_bScrubbingFastRender("Fast Render Scrubbing",false),
	m_bAutoSave("Auto-Save",true),
	m_AutoSaveFrequencyInMinutes("Frequency (min)",5),
	m_AutoSaveBackups("Backups per Scene", 5),
	//m_UndoLevels("Undo Levels",10),
	//m_UndoMemory("Undo Max Memory",0.0f),
	m_BMailLocation("BMail Location"),
	m_MoviePlayerLocation("Movie Player"),
	m_ChannelEditor_DefaultTime("Default Scene Time",10),
	m_RenderWindowWidth("Window Width",720),
	m_RenderWindowHeight("Window Height",576),
	m_CameraControls("Camera Controls"),
	m_TimeFormat("Time Format"),
	m_Units("Units", prtyUnits::GetUnitSystem()),
	m_IconScale("Icon Scale", icnIconScale::GetIconScaleLevel()),
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
	m_bSafeFramesVisible("Safe Frames Visible", true),
	m_bActionSafeFrameVisible("Action Safe Frame Visible", true),
	m_bTitleSafeFrameVisible("Title Safe Frame Visible", true),
	m_bShowToolTips("Show Tooltips", true),
	m_bOnAddSwitchToPlacedTab("On Add, Go To Placed Tab", true),
	m_bManipFastRender("Fast Render When Moving Camera", false),
	m_bMultithreadAnimation("Animation Thread", envThreadGroup::GetThreadingEnabled()),
	m_bMultithreadCapture("Capture Thread", true),
	m_bMultithreadViewer("Viewer Thread", true),
	m_bDuplicateObjectsName("Duplicate Objects Keep Name", false),
	//m_CameraPanRate("Camera Pan Rate", 1.0f),
#ifdef WIN64
	m_VertexAnimationBudget("Vertex Anim Budget", 2000), // in MBs
#else
	m_VertexAnimationBudget("Vertex Anim Budget", 200), // in MBs
#endif
//	m_bUseHardwareTessellation("Use Hardware Tessellation", false),
//	m_HardwareTessellationValue("Tessellation Value", 1.0f ),
//	m_bFlatTessellation("Disable PN Smoothing", false),
//	m_SubdivMode("Subdivision Method", 0),
	m_bNeverLoadTextures("Skip All Textures", false),
	m_bAutoDXTCompress("Auto Compress Textures", false),
	m_TextureReduce("Reduce Texture Size", 0),
	m_DepthMapReduce("Reduce Depth Map Size", 0),
	m_bNeverLoadSounds("Never Load Sounds", false),
	m_UserPyLocation("UserStartup Script"),
	m_MaterialBrowserHome("Material Browser Home"),
	m_bAllChannelsWithDrivers("Key All Altered Properties",false),
	m_bLocalSpaceTranslation("Local Space Translation", mnpModeObjectManip::IsLocalSpaceTranslation()),
	m_FrameRate("Frames per Second", 24.0f),
	m_PRmanLocation("Renderer Location (prman.exe)"),
	m_PixieLocation("Pixie Location (rndr.exe)"),
	m_MRayLocation("Renderer Location (ray.exe)"),
	m_MRayViewerLocation("Viewer v1.19 Location (imf_disp.exe)"),
	m_bEnableAutoUpdate("Prompt Before Asset Update", false),
	m_bEnableRevision("Enable Asset Update", false)
{
	std::string moviestr("C:\\Program Files\\Windows Media Player\\wmplayer.exe");
	m_MoviePlayerLocation.SetString(moviestr);

	//bga - Nothing was using or enforcing these maximums. Limits could go into the UIInfo:
	//m_MRUHistory.SetMaximum(10);
	//m_AutoSaveBackups.SetMaximum(20);
	//m_UndoLevels.SetMaximum(127);
	//m_VertexAnimationBudget.SetMaximum(1000); // in MBs, so 1GB is maximum allowed (because of 32 bit limits)
	m_CameraControls.SetEnumTag(0, "Default");
	m_CameraControls.SetEnumTag(1, "Maya");
	m_CameraControls.SetEnumTag(2, "3D Studio MAX");

	m_TimeFormat.SetEnumTag(0,"HH:MM:SS.Fr");
	m_TimeFormat.SetEnumTag(1,"HH:MM:SS:MS");
	m_TimeFormat.SetEnumTag(2,"Frames");
	m_TimeFormat.SetEnumTag(3,"Seconds");
	//e_HHMMSSFR	= 0,
	//e_HHMMSSMS	= 1,
	//e_Frames	= 2,
	//e_Seconds	= 3,

	m_Units.SetEnumTag(0,"Centimeters");
	m_Units.SetEnumTag(1,"Meters");
	m_Units.SetEnumTag(2,"Inches");
	m_Units.SetEnumTag(3,"Feet");
	//enum UnitTypes
	//{
	//	e_Centimeters = 0, // default value
	//	e_Meters,
	//	e_Inches,
	//	e_Feet
	//};

	m_IconScale.SetEnumTag(0,"Small");
	m_IconScale.SetEnumTag(1,"Medium");
	m_IconScale.SetEnumTag(2,"Large");
	//enum IconScaleLevel
	//{
	//	e_Small = 0,
	//	e_Medium,
	//	e_Large
	//};

//	m_SubdivMode.SetEnumTag(0,"Catmull-Clark Subdiv");
//	m_SubdivMode.SetEnumTag(1,"PN Triangle");
//	m_SubdivMode.SetEnumTag(2,"Quad Patches");

	/*m_BakedTextureFormat.SetEnumTag(0, "DDS");
	m_BakedTextureFormat.SetEnumTag(1, "BMP");
	m_BakedTextureFormat.SetEnumTag(2, "JPG");
	m_BakedTextureFormat.SetEnumTag(3, "PNG");
	m_BakedTextureFormat.SetEnumTag(4, "TIF");*/
}

