/*****************************************************************************
**  modeMode.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Support/mode/modeMode.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	const int c_NoState = -1;
}


//----------------------------------------------------------------------------
//	constants
//----------------------------------------------------------------------------
const int modeMode::c_InvalidIndex = -1;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
modeMode::modeMode()
:	m_nNewState(c_NoState),
	m_bPushState(false),
	m_bPopState(false),
	m_ID( -1 )
{
	m_szMenuItemName[0] = 0; // terminate string
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// virtual
modeMode::~modeMode()
{
}

//----------------------------------------------------------------------------
//	The mode should do it's per frame "work" in the Think function.
//----------------------------------------------------------------------------
void modeMode::Think()
{
	//bga - appMode::Think() increments the sim time, but we want the
	// simulation time to stay synced with the timeline time, so I am going to 
	// comment this out.
//	appMode::Think();

	// handle state switching
	//
	if ( m_nNewState != c_NoState)
	{
		int size = m_StateStack.size();

		// if we are not pushing a state
		// cleanup all previous states
		//
		if ( !m_bPushState )
		{
			int i = 0;
			for ( i = 0; i < size; ++i)
			{
				CleanState(m_StateStack[i]);
			}
			m_StateStack.clear();
		}
		else if ( size > 0 )
		{
			CleanState(m_StateStack[size - 1]);
		}

		m_StateStack.push_back(m_nNewState);

		SetupState(m_nNewState);

		m_nNewState = c_NoState;
		m_bPushState = false;
	}
	else if ( m_bPopState )
	{
		// if there is only one thing on the stack then we don't pop
		if (m_StateStack.size() > 1)
		{
			CleanState(GetState());
			m_StateStack.pop_back();
			SetupState(m_StateStack[m_StateStack.size() - 1]);
		}
		m_bPopState = false;
	}
}


//----------------------------------------------------------------------------
//	SetID()
//----------------------------------------------------------------------------
void modeMode::SetID( const modeModeID i_ID )
{
	m_ID = i_ID;
}


//----------------------------------------------------------------------------
//	GetID()
//----------------------------------------------------------------------------
const modeModeID& modeMode::GetID() const
{
	return m_ID;
}

//----------------------------------------------------------------------------========
//	CleanState gets called when switching to a new state
//	derived classes can overload to clean up the current state
//----------------------------------------------------------------------------========
void modeMode::CleanState(int i_nState)
{
}

//----------------------------------------------------------------------------========
//	SetupState gets called when switching to a new state
//	derived classes can overload to setup up the next state
//----------------------------------------------------------------------------========
void modeMode::SetupState(int i_nState)
{
}

//----------------------------------------------------------------------------
//	IsLevelModified returns true if the current level is changed but
//	not saved, defined as always returning false at this level
//	builder modes only should override this
//----------------------------------------------------------------------------
bool modeMode::IsLevelModified()
{
	return false;
}

//----------------------------------------------------------------------------
//	IsModal() - signals whether the mode should be pushed onto the mode stack
//	ON TOP of the current mode, or replace the current mode (via Pop)
//
//	true - the mode will be pushed on top of the current mode.
//	false- the mode will replace the current mode.
//----------------------------------------------------------------------------
//virtual
bool modeMode::IsModal()
{
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void modeMode::SetMenuItemName( const char * i_Name )
{
	strcpy( m_szMenuItemName, i_Name );
}
