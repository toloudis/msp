/*****************************************************************************
**	fcmdModeMgr.hpp
**
**		Singleton class to manage the various modes 
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCMD_MODEMGR_HPP
#error fcmdModeMgr.hpp multiply included
#endif
#define FCMD_MODEMGR_HPP
#pragma once
#ifndef FCMD_MODETEMPLATE_HPP
#include "FCSupport/fcmd/fcmdModeTemplate.hpp"
#endif

#ifndef FCMD_MODECAST_HPP
#include "FCSupport/fcmd/fcmdModeCast.hpp"
#endif
#ifndef FCMD_MODELOCATION_HPP
#include "FCSupport/fcmd/fcmdModeLocation.hpp"
#endif
#ifndef FCMD_MODEACTION_HPP
#include "FCSupport/fcmd/fcmdModeAction.hpp"
#endif
#ifndef FCMD_MODEMAKE_HPP
#include "FCSupport/fcmd/fcmdModeMake.hpp"
#endif
#ifndef FCMD_MODETHEATER_HPP
#include "FCSupport/fcmd/fcmdModeTheater.hpp"
#endif
#ifndef FCMD_MODENEWMOVIE_HPP
#include "FCSupport/fcmd/fcmdModeNewMovie.hpp"
#endif

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef MA_TIME_HPP
#include "Core/Ma/maTime.hpp"
#endif 

#ifndef QTFC3D_H
#include "FCSupport/fcui/qtGUI/qtfc3d.h"
#endif

#include <vector>

//============================================================================
//============================================================================
class fcuiTimelineItemData;

//============================================================================
//============================================================================
class fcmdModeMgr
{
public:
	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	static enum Mode
	{
		e_Cast,
		e_Location,
		e_Action,
		e_Make,
		e_Theater,
		e_NewMovie
	};

	///-----------------------------------------------------------------------
	/// Each event comes from a certain level of operation within the mode
	///-----------------------------------------------------------------------
	static enum ModeLevel
	{
		e_Mode = 0,
		e_Subject,
		e_Category,
		e_Element,
		e_CustomIcons,
		e_HomeScreen
	};

	///-----------------------------------------------------------------------
	/// Each event comes from a certain level of operation within the mode
	///-----------------------------------------------------------------------
	static enum AnimStates
	{
		e_Idle = 0,
		e_HairChange,
		e_EyeChange,
		e_NumStates
	};

	///-----------------------------------------------------------------------
	/// constructors
	///-----------------------------------------------------------------------
	fcmdModeMgr();

	///-----------------------------------------------------------------------
	/// destructors
	///-----------------------------------------------------------------------
	~fcmdModeMgr();

	///-----------------------------------------------------------------------
	/// Initialize the event animations when in cast mode
	///-----------------------------------------------------------------------
	void InitCastAnims();

	///---------------------------------------------------------------------------
	///---------------------------------------------------------------------------
	maTime SetSubjectAnims(const fsLocator& i_SubjectDirectory);

	///---------------------------------------------------------------------------
	///---------------------------------------------------------------------------
	const maTime& GetCastAnimStart();
	const maTime& GetCastAnimEnd();

	///-----------------------------------------------------------------------
	/// Return the directory of a mode
	///-----------------------------------------------------------------------
	static const fsLocator GetModeLocator(const Mode& i_ModeID);
	
	///-----------------------------------------------------------------------
	/// Link the mode to the given QT button
	///-----------------------------------------------------------------------
	fcmdModeTemplate* CreateNewMode(const Mode& i_ModeID);

	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	void SetCurrentMode();

	///-----------------------------------------------------------------------
	/// Add an event to the current mode to process
	///-----------------------------------------------------------------------
	void AddEventToCurrentMode();

	///-----------------------------------------------------------------------
	/// Process an event based on the initiators mode level and their data
	///-----------------------------------------------------------------------
	void ProcessModeEvent(const ModeLevel& i_ModeLevel);
	void ProcessModeEvent(const ModeLevel& i_ModeLevel, const fsLocator& i_Locator, const int& i_ItemID);

	///-----------------------------------------------------------------------
	/// Do any mode operations and process events for the current mode
	///-----------------------------------------------------------------------
	void Think();

	///-----------------------------------------------------------------------
	/// Return the current instance of the mode mgr singleton
	///-----------------------------------------------------------------------
	static fcmdModeMgr* Instance;

	///-----------------------------------------------------------------------
	/// Activate the given mode
	///-----------------------------------------------------------------------
	void ActivateMode(const Mode& i_ModeID);

	//------------------------------------------------------------------------
	/// Process the panel when a new movie request is made
	//------------------------------------------------------------------------
	void ProcessNewMovieRequest();

	//------------------------------------------------------------------------
	/// Load the new movie
	//------------------------------------------------------------------------
	void LoadNewMovie(const fsLocator& i_MovieDirectory);

	///---------------------------------------------------------------------------
	/// Switch to the proper mode camera when an item is clicked
	///---------------------------------------------------------------------------
	void SwitchModeCamera(const fsLocator& i_Directory);

	///-----------------------------------------------------------------------
	/// When a mode operation is initiated, we should handle the appropriate
	/// events here.
	///-----------------------------------------------------------------------
	void ProcessModeOperation(const fsLocator& i_ModeDirectory);

	///-----------------------------------------------------------------------
	/// When a subject operation is initiated, we should handle the appropriate
	/// events here.
	///-----------------------------------------------------------------------
	void ProcessSubjectOperation(const fsLocator& i_SubjectDirectory);

	///-----------------------------------------------------------------------
	/// When a category operation is initiated, we should handle the appropriate
	/// events here.
	///-----------------------------------------------------------------------
	void ProcessCategoryOperation(const fsLocator& i_CategoryDirectory);

	///-----------------------------------------------------------------------
	/// When an element operation is initiated, we should handle the appropriate
	/// events here.
	///-----------------------------------------------------------------------
	void ProcessElementOperation(const fsLocator& i_ElementDirectory);

	///-----------------------------------------------------------------------
	/// When an custom/none icons are clicked, we should handle the appropriate
	/// events here.
	///-----------------------------------------------------------------------
	void ProcessCustomIconsOperation(const fsLocator& i_ElementDirectory);

	///-----------------------------------------------------------------------
	/// Select the element in the given directory
	///-----------------------------------------------------------------------
	void SelectElement(const fsLocator& i_ElementDirectory);

	///---------------------------------------------------------------------------
	/// When an element is checked, we need to execute it's operation
	///---------------------------------------------------------------------------
	void ProcessCheckedElement(const fsLocator& i_ElementDirectory);

	///-----------------------------------------------------------------------
	/// Return the checked item from the current mode
	///-----------------------------------------------------------------------
	const fsLocator GetCheckedElement();

	///---------------------------------------------------------------------------
	///---------------------------------------------------------------------------
	void SetTimelineLocked( bool i_bLocked );

	///---------------------------------------------------------------------------
	///---------------------------------------------------------------------------
	bool GetTimelineLocked();

	///-----------------------------------------------------------------------
	/// When a timeline object is clicked, we need to execute it's operation
	///-----------------------------------------------------------------------
	void ProcessTimelineItem(const fcuiTimelineItemData& i_ItemData);
	
	///-----------------------------------------------------------------------
	/// Shift a timeline object's drivers and data in our main timeline by the
	/// duration amount
	///-----------------------------------------------------------------------
	void ShiftTimelineItem(fcuiTimelineItemData& io_ItemData, const maTime& i_Duration, bool i_bManualShift);

	///-----------------------------------------------------------------------
	/// Remove a timeline object's drivers
	///-----------------------------------------------------------------------
	void RemoveTimelineItem(const fcuiTimelineItemData& i_ItemData);

	///-----------------------------------------------------------------------
	/// Edit a timeline object's drivers or properties
	///-----------------------------------------------------------------------
	void EditTimelineItem(fcuiTimelineItemData& io_ItemData);

	///-----------------------------------------------------------------------
	/// Set the main subject of the scene
	///-----------------------------------------------------------------------
	void SetMainSubject(const itString& i_CurrentSubject);

	///-----------------------------------------------------------------------
	/// Set the main subject of the scene
	///-----------------------------------------------------------------------
	void SetMainSubjectDirectory(const fsLocator& i_CurrentSubjectDirectory);

	///-----------------------------------------------------------------------
	/// Set the main subject directory of the scene
	///-----------------------------------------------------------------------
	const fsLocator& GetMainSubjectDirectory();

	///-----------------------------------------------------------------------
	/// Set the movie pack directory
	///-----------------------------------------------------------------------
	void SetMoviePackDirectory(const itString& i_CurrentMoviePack);

	///-----------------------------------------------------------------------
	/// Set the timeline subject of the scene
	///-----------------------------------------------------------------------
	void SetTimelineSubject(const itString& i_CurrentSubject);

	///-----------------------------------------------------------------------
	/// Set the timeline subject of the scene
	///-----------------------------------------------------------------------
	const itString& GetTimelineSubject();

	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	bool GetCanDrag();

	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	void SetCanDrag(bool i_bCanDrag);

	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	bool GetCanCheck();

	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	void SetCanCheck(bool i_bCanCheck);

private:
	bool m_bCanDrag;
	bool m_bCanCheck;
	bool m_bTimelineLocked;
	fcmdModeTemplate* m_pCurrentMode;
	fcmdModeTemplate* m_pNextMode;
	itString m_MainSubject;
	fsLocator m_MainSubjectDirectory;
	fsLocator m_CurrentCategoryDir;
	itString m_TimelineSubject;
	maTime m_CastAnimStart;
	maTime m_CastAnimEnd;
	std::vector<fcuiTimelineItemData> m_CastAnimList;
};

