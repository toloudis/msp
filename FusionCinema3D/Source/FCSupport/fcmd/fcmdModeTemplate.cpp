/*****************************************************************************
**	fcmdModeTemplate.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcmd/fcmdModeTemplate.hpp"

#include "FCSupport/fcmd/fcmdModeMgr.hpp"


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------
fcmdModeTemplate::fcmdModeTemplate()
:	m_ModeDirectory(fsLocator()),
	m_ModeID(-1),
	m_bAllowElementClick(true),
	m_bAutoPopulate(true)
{
}

fcmdModeTemplate::fcmdModeTemplate(const fsLocator& i_Directory)
:	m_ModeDirectory(i_Directory),
	m_ModeID(-1),
	m_bAllowElementClick(true),
	m_bAutoPopulate(true)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcmdModeTemplate::Initialize()
{

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcmdModeTemplate::Activate()
{

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcmdModeTemplate::DeActivate()
{
}

///-----------------------------------------------------------------------
/// Do any mode thinking
///-----------------------------------------------------------------------
void fcmdModeTemplate::Think()
{
}

//----------------------------------------------------------------------------
// Set the root directory for this mode i.e. "..\FusionCinema\Modes\Cast"
//----------------------------------------------------------------------------
void fcmdModeTemplate::SetRootDirectory(const fsLocator& i_Directory)
{
	m_ModeDirectory = i_Directory;
}

//----------------------------------------------------------------------------
// Returns the location of the root directory
//----------------------------------------------------------------------------
const fsLocator& fcmdModeTemplate::GetRootDirectory() const
{
	return m_ModeDirectory;
}

//----------------------------------------------------------------------------
// Process the events on the mode's queue
//----------------------------------------------------------------------------
void fcmdModeTemplate::ProcessEvents()
{
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a subject needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeTemplate::ProcessSubject(const fsLocator& i_SubjectDirectory)
{
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a category needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeTemplate::ProcessCategory(const fsLocator& i_CategoryDirectory)
{
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when an element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeTemplate::ProcessElement(const fsLocator& i_ElementDirectory)
{
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when an element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeTemplate::ProcessCustomIcon(const fsLocator& i_ElementDirectory)
{
}

//------------------------------------------------------------------------
/// Return whether the mode should allow element's to be clicked to 
/// instantiate their operation i.e. some Action elements must be
/// dragged to the timeline to activate them, so clicks should be disabled
//------------------------------------------------------------------------
//virtual
void fcmdModeTemplate::SetAllowElementClick(bool i_bAllowClick)
{
	m_bAllowElementClick = i_bAllowClick;
}
bool fcmdModeTemplate::GetAllowElementClick()
{
	return m_bAllowElementClick;
}

//----------------------------------------------------------------------------
// Add events from child elements to the mode's queue
//----------------------------------------------------------------------------
void fcmdModeTemplate::AddEvent()
{
}

///---------------------------------------------------------------------------
/// Get the mode id for this instance
///---------------------------------------------------------------------------
int fcmdModeTemplate::GetModeID()
{
	return m_ModeID;
}

///---------------------------------------------------------------------------
/// Set the mode id for this instance
///---------------------------------------------------------------------------
void fcmdModeTemplate::SetModeID(int i_ModeID)
{
	m_ModeID = i_ModeID;
}

///-----------------------------------------------------------------------
/// Determines if the mode should auto populate all panels
///-----------------------------------------------------------------------
void fcmdModeTemplate::SetAutoPopulatePanels(bool i_bAutoPopulate)
{
	m_bAutoPopulate = i_bAutoPopulate;
}

///-----------------------------------------------------------------------
/// Determines if the mode should auto populate all panels
///-----------------------------------------------------------------------
bool fcmdModeTemplate::GetAutoPopulatePanels()
{
	return m_bAutoPopulate;
}
