/********************************************************************************************\
**  PrefsData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
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

	//---------------------------------------------------------------------------
	//	AUTO-SAVE data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bAutoSave;
	prtyInt32	m_AutoSaveFrequency;	// seconds
	prtyInt8	m_AutoSaveBackups;		// per scene

	//---------------------------------------------------------------------------
	//	Undo data
	//---------------------------------------------------------------------------
	prtyInt8		m_UndoLevels;
	prtyFloat		m_UndoMemory;

	//---------------------------------------------------------------------------
	//	CAPTURE data
	//---------------------------------------------------------------------------
	prtyFilePath	m_BMailLocation;		// including executable name
	prtyFilePath	m_MoviePlayerLocation;	// including executable name

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
	prtyEnum		m_TimeFormat;
	prtyBoolean		m_bMainTimelineVisible;
	prtyBoolean		m_bMainActionToolbarVisible;
	prtyBoolean		m_bMainModeToolbarVisible;
	prtyBoolean		m_bMainStatusBarVisible;
	prtyBoolean		m_bAxisCompassVisible;
	prtyBoolean		m_bShowToolTips;
	prtyBoolean		m_bOnAddSwitchToPlacedTab;
	prtyFloat		m_CameraPanRate;

	//---------------------------------------------------------------------------
	//	General
	//---------------------------------------------------------------------------
	prtyBoolean		m_bMuteAudio;
	prtyFloat		m_GlobalScale;
	prtyBoolean		m_bMultithreading;

	//---------------------------------------------------------------------------
	// Film Gates
	//---------------------------------------------------------------------------
	prtyBoolean		m_bFilmGatesVisible;
	std::map<std::string, bool> m_FilmGateVisible;

	//---------------------------------------------------------------------------
	// Messaging
	//---------------------------------------------------------------------------
	prtyText		m_Lightwait_MessagingPort;
	prtyText		m_Lightwait_MessagingServer;
	prtyText		m_Lightwait_MessagingOutputQueue;
	prtyText		m_Lightwait_MessagingInputQueue;
	prtyText		m_Lightwait_MessagingWireProtocol;
	prtyText		m_Lightwait_MessagingProtocol;
	prtyText		m_Lightwait_MessagingLocalOutputLocation;
};

