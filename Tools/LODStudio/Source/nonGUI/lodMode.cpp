/*****************************************************************************
**  lodMode.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "lodMode.hpp"

//	library
#include "appSimTime.hpp"
//#include "fsLocator.hpp"
#include "g2dRGBColor.hpp"
#include "g3dViewer.hpp"
#include "inDeviceMgr.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	int l_ScreenCapCounter = 1;

	const int c_NoState = -1;
}


//----------------------------------------------------------------------------
//	constants
//----------------------------------------------------------------------------
const int lodMode::c_InvalidIndex = -1;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lodMode::lodMode()
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
lodMode::~lodMode()
{
}

//----------------------------------------------------------------------------
//	The mode should do it's per frame "work" in the Think function.
//----------------------------------------------------------------------------
void lodMode::Think()
{
	appMode::Think();

	inKeyboard* pkeyboard = inDeviceMgr::GetKeyboard();

	if ( pkeyboard )
	{
		// Check for screen capture button here so that
		// you can screen capture any mode or menu
		//
		if ( pkeyboard->IsReleased(inKeys::e_F9) )
		{
		}
		// Wireframe rendering (debugging)
/*		else if( pkeyboard->IsReleased(inKeys::e_F6) )
		{
			DWORD cur_state;
			DWORD next_state;
			g2dDX9Global::g_pDevice->GetRenderState(D3DRS_FILLMODE, &cur_state);

			if( cur_state == D3DFILL_WIREFRAME )
				next_state = D3DFILL_SOLID;
			else
				next_state = D3DFILL_WIREFRAME;

			g2dDX9Global::g_pDevice->SetRenderState(D3DRS_FILLMODE, next_state);
		}*/
	}

	// handle state switching
	//
	if ( m_nNewState != c_NoState)
	{
		int size = m_StateStack.size();

		// if we are not pushing a state
		// cleanup all previous states
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
void lodMode::SetID( const lodModeID i_ID )
{
	m_ID = i_ID;
}


//----------------------------------------------------------------------------
//	GetID()
//----------------------------------------------------------------------------
const lodModeID& lodMode::GetID() const
{
	return m_ID;
}

//----------------------------------------------------------------------------========
//	CleanState gets called when switching to a new state
//	derived classes can overload to clean up the current state
//----------------------------------------------------------------------------========
void lodMode::CleanState(int i_nState)
{
}

//----------------------------------------------------------------------------========
//	SetupState gets called when switching to a new state
//	derived classes can overload to setup up the next state
//----------------------------------------------------------------------------========
void lodMode::SetupState(int i_nState)
{
}

//----------------------------------------------------------------------------
//	IsLevelModified returns true if the current level is changed but
//	not saved, defined as always returning false at this level
//	builder modes only should override this
//----------------------------------------------------------------------------
bool lodMode::IsLevelModified()
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
bool lodMode::IsModal()
{
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void lodMode::SetMenuItemName( const char * i_Name )
{
	strcpy( m_szMenuItemName, i_Name );
}
