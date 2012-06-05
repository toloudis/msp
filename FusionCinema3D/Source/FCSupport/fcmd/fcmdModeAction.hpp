/*****************************************************************************
**	fcmdModeAction.hpp
**
**		Mode class for the the Action mode
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCMD_MODEACTION_HPP
#error fcmdModeAction.hpp multiply included
#endif
#define FCMD_MODEACTION_HPP

#ifndef FCMD_MODETEMPLATE_HPP
#include "FCSupport/fcmd/fcmdModeTemplate.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class fcuiTimelineItemData;
class maTime;

//============================================================================
//============================================================================
class fcmdModeAction : public fcmdModeTemplate
{
public:
	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	fcmdModeAction();
	fcmdModeAction(const fsLocator& i_Directory);

	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	~fcmdModeAction();

	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	void Activate();

	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	void DeActivate();

	///-----------------------------------------------------------------------
	/// Do any mode thinking
	///-----------------------------------------------------------------------
	void Think();

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when an element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	void ProcessSubject(const fsLocator& i_SubjectDirectory);
	
	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when an element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	void ProcessCategory(const fsLocator& i_CategoryDirectory);

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when an element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	void ProcessElement(const fsLocator& i_ElementDirectory);

	///---------------------------------------------------------------------------
	/// Perform the mode specific operations when an element needs to 
	/// execute an action.
	///---------------------------------------------------------------------------
	void ProcessCustomIcon(const fsLocator& i_ElementDirectory);

	///---------------------------------------------------------------------------
	/// When an element is checked, we need to execute it's operation
	///---------------------------------------------------------------------------
	void ProcessCheckedItem(const fsLocator& i_ElementDirectory);

	///---------------------------------------------------------------------------
	/// When the panel is repopulated, we need to return the checkbox data if necessary
	///---------------------------------------------------------------------------
	const fsLocator GetCheckedLocator();

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when a timeline item needs to 
	/// execute an action.
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

private:
	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when a scene element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	void ProcessScene(const fsLocator& i_ElementDirectory);

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when an audio element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	void ProcessAudio(const fsLocator& i_ElementDirectory);

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when a lighting element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	void ProcessLights(const fsLocator& i_ElementDirectory);

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when a title card element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	void ProcessTitleCard(const fsLocator& i_ElementDirectory);

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when a post fx element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	void ProcessPostFX(const fsLocator& i_ElementDirectory);

	///-----------------------------------------------------------------------
	/// Perfrom the appropriate operations for a timeline object belonging
	/// to the scene category
	///-----------------------------------------------------------------------
	void ProcessSceneTimelineItem(const fcuiTimelineItemData& i_ItemData);

	///-----------------------------------------------------------------------
	/// Perfrom the appropriate operations for a timeline object belonging
	/// to the scene category
	///-----------------------------------------------------------------------
	void ProcessTitlecardTimelineItem(const fcuiTimelineItemData& i_ItemData);

	///-----------------------------------------------------------------------
	/// shift the timeline item belonging to the scene category
	///-----------------------------------------------------------------------
	void ShiftSceneTimelineItem(fcuiTimelineItemData& io_ItemData, const maTime& i_Duration);

	///-----------------------------------------------------------------------
	/// shift the timeline item belonging to the titlecard category
	///-----------------------------------------------------------------------
	void ShiftTitleCardTimelineItem(fcuiTimelineItemData& io_ItemData, const maTime& i_Duration, bool i_bManualShift);

	///-----------------------------------------------------------------------
	/// remove the timeline item belonging to the scene category
	///-----------------------------------------------------------------------
	void RemoveSceneTimelineItem(const fcuiTimelineItemData& i_ItemData);

	///-----------------------------------------------------------------------
	/// remove the timeline item belonging to the scene category
	///-----------------------------------------------------------------------
	void RemoveTitleCardTimelineItem(const fcuiTimelineItemData& i_ItemData);

	///-----------------------------------------------------------------------
	/// Edit the timeline item belonging to the scene category
	///-----------------------------------------------------------------------
	void EditSceneTimelineItem(fcuiTimelineItemData& io_ItemData);

	///-----------------------------------------------------------------------
	/// Edit the timeline item belonging to the titlecard category
	///-----------------------------------------------------------------------
	void EditTitleCardTimelineItem(fcuiTimelineItemData& io_ItemData);

	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	float SetObjectAnims(const itString& i_AnimationName, const fsLocator& i_CurrentDirectory, const maTime& i_StartTime);

	///-----------------------------------------------------------------------
	/// ProcessFiles
	///-----------------------------------------------------------------------
	void ProcessFiles(const fsLocator& i_ElementDirectory, const std::vector<fsLocator>& i_Files);

	///-----------------------------------------------------------------------
	/// ProcessFile
	/// Returns true if the file was processed.
	///-----------------------------------------------------------------------
	bool ProcessFile(const fsLocator& i_ElementDirectory, const fsLocator& i_File);

private:
	fsLocator m_ModeDirectory;
	itString m_ActionState;
	itString m_CurrentSubject;
	itString m_CurrentCategory;
	itString m_CurrentElement;

};