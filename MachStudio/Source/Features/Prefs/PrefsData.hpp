//****************************************************************************
//	PrefsData.hpp
//
//	Preferences Data
//
//	StudioGPU
//	Copyright(c) 2004-7 - All Rights Reserved
//****************************************************************************
#pragma once

#ifdef PREFSDATA_HPP
#error PrefsData.hpp multiply included
#endif
#define PREFSDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#include <map>


//============================================================================
//============================================================================
class PrefsData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	PrefsData();

	//---------------------------------------------------------------------------
	//	MENU data
	//---------------------------------------------------------------------------
	prtyInt32	m_MRUHistory;

	//---------------------------------------------------------------------------
	//	PLAYBACK data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bPlaybackTimeCode;
	prtyBoolean	m_bPlaybackLoopAtEnd;
	prtyBoolean	m_bPlaybackLowRes;

	//---------------------------------------------------------------------------
	//	SCRUBBING data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bScrubbingLowRes;
	prtyBoolean	m_bScrubbingFastRender;

	//---------------------------------------------------------------------------
	//	AUTO-SAVE data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bAutoSave;
	prtyInt32	m_AutoSaveFrequencyInMinutes;	// minutes
	prtyInt8	m_AutoSaveBackups;				// per scene

	//---------------------------------------------------------------------------
	//	Undo data
	//---------------------------------------------------------------------------
	//prtyInt8		m_UndoLevels;
	//prtyFloat		m_UndoMemory;

	//---------------------------------------------------------------------------
	//	CAPTURE data
	//---------------------------------------------------------------------------
	prtyFilePath	m_BMailLocation;			// including executable name
	prtyFilePath	m_MoviePlayerLocation;		// including executable name

	//---------------------------------------------------------------------------
	//	CHANNEL EDITOR data
	//---------------------------------------------------------------------------
	prtyInt32		m_ChannelEditor_DefaultTime;	// seconds
	prtyBoolean		m_ChannelEditor_SnapActive;
	prtyFloat		m_ChannelEditor_SnapAmount;
	prtyEnum		m_DefaultDriverBlend;

	//---------------------------------------------------------------------------
	//	LAYOUT data
	//---------------------------------------------------------------------------
	prtyText		m_CurrentLayout;	// current layout config name

	//---------------------------------------------------------------------------
	//	Project/Scene data
	//---------------------------------------------------------------------------
	prtyText		m_LastProjectName;
	prtyText		m_DefaultProjectName;
	prtyText		m_DefaultProjectDirectory;
	prtyText		m_DefaultSceneName;

	//---------------------------------------------------------------------------
	//	GUI state data
	//---------------------------------------------------------------------------
	prtyInt32		m_RenderWindowWidth;
	prtyInt32		m_RenderWindowHeight;
	prtyEnum		m_CameraControls;
	prtyEnum		m_TimeFormat;
	prtyEnum		m_Units;
	prtyEnum		m_IconScale;
	prtyBoolean		m_bMainTimelineVisible;
	prtyBoolean		m_bMainActionToolbarVisible;
	prtyBoolean		m_bMainModeToolbarVisible;
	prtyBoolean		m_bMainStatusBarVisible;
	prtyBoolean		m_bAxisCompassVisible;
	prtyBoolean		m_bShowToolTips;
	prtyBoolean		m_bOnAddSwitchToPlacedTab;
	prtyBoolean		m_bManipFastRender;
	//prtyFloat		m_CameraPanRate;

	//---------------------------------------------------------------------------
	//	General
	//---------------------------------------------------------------------------
	prtyBoolean		m_bMuteAudio;
	prtyFloat		m_GlobalScale;
	prtyBoolean		m_bDuplicateObjectsName;
	prtyFloat		m_FrameRate;				// Global framerate per second

 	//---------------------------------------------------------------------------
	//	Threads
	//---------------------------------------------------------------------------
	prtyBoolean		m_bMultithreadAnimation;
	prtyBoolean		m_bMultithreadCapture;
	prtyBoolean		m_bMultithreadViewer;

	//---------------------------------------------------------------------------
	// Safe Frames
	//---------------------------------------------------------------------------
	prtyBoolean		m_bActionSafeFrameVisible;
	prtyBoolean		m_bTitleSafeFrameVisible;
	prtyBoolean		m_bSafeFramesVisible;
	std::map<std::string, bool> m_SafeFrameVisible;

	//---------------------------------------------------------------------------
	//	Vertex animation
	//---------------------------------------------------------------------------
	prtyInt32		m_VertexAnimationBudget;

	//---------------------------------------------------------------------------
	//	Tessellation
	//---------------------------------------------------------------------------
//	prtyBoolean		m_bUseHardwareTessellation;
//	prtyFloat		m_HardwareTessellationValue;
//	prtyBoolean		m_bFlatTessellation;
//	prtyEnum		m_SubdivMode;

	//---------------------------------------------------------------------------
	//	Loading
	//---------------------------------------------------------------------------
	prtyBoolean	m_bNeverLoadTextures;
	prtyBoolean m_bAutoDXTCompress;
	prtyInt8	m_TextureReduce;
	prtyInt8	m_DepthMapReduce;
	prtyBoolean	m_bNeverLoadSounds;

	//---------------------------------------------------------------------------
	//	User directories
	//---------------------------------------------------------------------------
	prtyFilePath	m_UserPyLocation;		// including UserStartup.py
	prtyFilePath	m_MaterialBrowserHome;	// home directory for material browser

	//---------------------------------------------------------------------------
	//	Menu items
	//---------------------------------------------------------------------------
	prtyBoolean		m_bAutoKey;
	prtyBoolean		m_bPropKeyButtons;
	prtyBoolean		m_bAllChannelsWithDrivers;
	prtyBoolean		m_bLocalSpaceTranslation;

	//---------------------------------------------------------------------------
	// Auto Update
	//---------------------------------------------------------------------------
	prtyBoolean m_bEnableAutoUpdate;
	prtyBoolean m_bEnableRevision;

	//---------------------------------------------------------------------------
	//	RENDERMAN data
	//---------------------------------------------------------------------------
	prtyFilePath	m_PRmanLocation;
	prtyFilePath	m_PixieLocation;
	prtyFilePath	m_MRayLocation;
	prtyFilePath	m_MRayViewerLocation;
};

