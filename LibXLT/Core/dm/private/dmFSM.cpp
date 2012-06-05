/****************************************************************************\
**	dmFSM.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/dm/dmFSM.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dmFSM::dmFSM()
:	m_pCurrentState(0),
	m_pNewState(0)
{
	// Initialize States
	m_StateInitial.SetState( this, &dmFSM::BeginStateInitial, &dmFSM::StateInitial, &dmFSM::EndStateInitial );

	// Initialize State Machine
	m_pCurrentState = static_cast<dmStateBase*>(&m_StateInitial);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dmFSM::~dmFSM()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void dmFSM::UpdateState()
{
	// check for new state
	if (m_pNewState)
	{
		if ( m_pNewState != m_pCurrentState )
		{
			// end the last state
			m_pCurrentState->ExecuteEndState();

			m_pCurrentState = m_pNewState;

			m_pNewState = 0;

			// start new state
			m_pCurrentState->ExecuteBeginState();
		}
		else
		{
			//	this state is already the current one, so don't end + restart it
			m_pNewState = 0;
		}
	}

	m_pCurrentState->ExecuteState();
}

