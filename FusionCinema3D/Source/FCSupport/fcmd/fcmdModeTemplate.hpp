/*****************************************************************************
**	fcmdModeTemplate.hpp
**
**		Template class for the various modes within the FusionCinema
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCMD_MODETEMPLATE_HPP
#error fcmdModeTemplate.hpp multiply included
#endif
#define FCMD_MODETEMPLATE_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

//============================================================================
//============================================================================
class fcmdModeTemplate
{
public:
	//------------------------------------------------------------------------
	// Constructors
	//------------------------------------------------------------------------
	fcmdModeTemplate();
	fcmdModeTemplate(const fsLocator& i_Directory);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void Initialize();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void Activate();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void DeActivate();

	///-----------------------------------------------------------------------
	/// Do any mode thinking
	///-----------------------------------------------------------------------
	virtual void Think();

	//------------------------------------------------------------------------
	// Set the root directory for this mode i.e. "..\FusionCinema\Modes\Cast"
	//------------------------------------------------------------------------
	virtual void SetRootDirectory(const fsLocator& i_Directory);

	//------------------------------------------------------------------------
	// Returns the location of the root directory
	//------------------------------------------------------------------------
	virtual const fsLocator& GetRootDirectory() const;

	//------------------------------------------------------------------------
	// Process the events on the mode's queue
	//------------------------------------------------------------------------
	virtual void ProcessEvents();

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when an element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	virtual void ProcessSubject(const fsLocator& i_SubjectDirectory);
	
	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when an element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	virtual void ProcessCategory(const fsLocator& i_CategoryDirectory);

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when an element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	virtual void ProcessElement(const fsLocator& i_ElementDirectory);

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when an element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	virtual void ProcessCustomIcon(const fsLocator& i_ElementDirectory);

	//------------------------------------------------------------------------
	/// Return whether the mode should allow element's to be clicked to 
	/// instantiate their operation i.e. some Action elements must be
	/// dragged to the timeline to activate them, so clicks should be disabled
	//------------------------------------------------------------------------
	virtual void SetAllowElementClick(bool i_bAllowClick);
	virtual bool GetAllowElementClick();

	//------------------------------------------------------------------------
	// Add events from child elements to the mode's queue
	//------------------------------------------------------------------------
	virtual void AddEvent();

	///-----------------------------------------------------------------------
	/// Get the mode id for this instance
	///-----------------------------------------------------------------------
	virtual int GetModeID();

	///-----------------------------------------------------------------------
	/// Set the mode id for this instance
	///-----------------------------------------------------------------------
	virtual void SetModeID(int i_ModeID);

	///-----------------------------------------------------------------------
	/// Determines if the mode should auto populate all panels
	///-----------------------------------------------------------------------
	virtual void SetAutoPopulatePanels(bool i_bAutoPopulate);

	///-----------------------------------------------------------------------
	/// Determines if the mode should auto populate all panels
	///-----------------------------------------------------------------------
	virtual bool GetAutoPopulatePanels();

private:
	fsLocator m_ModeDirectory;
	int m_ModeID;
	bool m_bAllowElementClick;
	bool m_bAutoPopulate;
	//std::vector<evntEventData> m_Events;
};
