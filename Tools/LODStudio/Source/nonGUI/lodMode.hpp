/*****************************************************************************
**  lodMode.hpp
**
**      The base mode for this project.
**		appMode contains all the really interesting functionality.
**		appModeMgr handles the switching of modes
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef LOD_MODE_HPP
#error lodMode.hpp multiply included
#endif
#define LOD_MODE_HPP

#ifndef APP_MODE_HPP
#include "appMode.hpp"
#endif
#ifndef DBG_ASSERT_HPP
#include "dbgAssert.hpp"
#endif

#include <vector>


//============================================================================
//	forward references
//============================================================================
class maFloatRGBA;


//============================================================================
//	typedefs
//============================================================================
typedef int lodModeID;


//============================================================================
//============================================================================
class lodMode : public appMode
{
	public:

		static const int c_InvalidIndex;

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		inline const char* GetMenuItemName();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		lodMode();

		//----------------------------------------------------------------------------
		// pure-virtual, children must be created
		//----------------------------------------------------------------------------
		virtual ~lodMode() = 0;

		//
		//	mode functions
		//

		//----------------------------------------------------------------------------
		//	Initialize will be called before the first call of Think after
		//	the object is first created or DeInitialized.  During the
		//	lifetime of a mode, Initialize and DeInitialize may be called
		//	several times.  Children of appMode should remember to call
		//	appMode::Initialize() at the beginning of their Initialize
		//	function.
		//----------------------------------------------------------------------------
		virtual void Initialize() = 0;

		//----------------------------------------------------------------------------
		//	The object should clean up things that are not needed while the
		//	mode is not running in the DeInitialize function.
		//----------------------------------------------------------------------------
		virtual void DeInitialize() = 0;

		//----------------------------------------------------------------------------
		//	The mode should do it's per frame "work" in the Think function.
		// This simply calls the base class and handles screen captures.
		//----------------------------------------------------------------------------
		virtual void Think();

		//----------------------------------------------------------------------------
		//	SetID()
		//----------------------------------------------------------------------------
		void SetID( const lodModeID i_ID );

		//----------------------------------------------------------------------------
		//	GetID()
		//----------------------------------------------------------------------------
		const lodModeID& GetID() const;

		//
		//	state functions
		//

		//----------------------------------------------------------------------------
		//	SetState sets the state for the mode
		//----------------------------------------------------------------------------
		inline void SetState( int i_nState );

		//----------------------------------------------------------------------------
		//	GetState returns the state
		//----------------------------------------------------------------------------
		inline int GetState() const;

		//----------------------------------------------------------------------------
		// PushState puts a new state on the stack
		//----------------------------------------------------------------------------
		inline void PushState( int i_nState );

		//----------------------------------------------------------------------------
		// PopState if there is a state in the stack it restores it.
		// otherwise it keeps the current state.
		//----------------------------------------------------------------------------
		inline void PopState();

		//----------------------------------------------------------------------------
		//	CleanState gets called when switching to a new state
		//	derived classes can overload to clean up the current state
		//----------------------------------------------------------------------------
		virtual void CleanState(int i_nState);

		//----------------------------------------------------------------------------
		//	SetupState gets called when switching to a new state
		//	derived classes can overload to setup up the next state
		//----------------------------------------------------------------------------
		virtual void SetupState(int i_nState);

		//----------------------------------------------------------------------------
		//	IsModal() - signals whether the mode should be pushed onto the mode stack
		//	ON TOP of the current mode, or replace the current mode (via Pop)
		//
		//	true - the mode will be pushed on top of the current mode.
		//	false- the mode will replace the current mode.
		//----------------------------------------------------------------------------
		virtual bool IsModal();

	protected:
		//----------------------------------------------------------------------------
		//	IsLevelModified returns true if the current level is changed but
		//	not saved, defined as always returning false at this level
		//	builder modes only should override this
		//----------------------------------------------------------------------------
		virtual bool IsLevelModified();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void lodMode::SetMenuItemName( const char * i_Name );

	private:
		char		m_szMenuItemName[ 64 ];

		std::vector<int> m_StateStack;
		int			m_nNewState;
		bool		m_bPushState;
		bool		m_bPopState;
		lodModeID	m_ID;
};

//----------------------------------------------------------------------------
//	SetState sets the state for the mode
//----------------------------------------------------------------------------
inline void lodMode::SetState( int i_nState )
{
	DBG_ASSERT0( i_nState >= 0,"Invalid state");
	m_nNewState = i_nState;
}

//----------------------------------------------------------------------------
//	GetState returns the state
//----------------------------------------------------------------------------
inline int lodMode::GetState() const
{
	if ( m_StateStack.size() == 0)
		return c_InvalidIndex;

	return m_StateStack[m_StateStack.size() - 1];
}

//----------------------------------------------------------------------------
// PusState puts a new state on the stack
//----------------------------------------------------------------------------
inline void lodMode::PushState( int i_nState )
{
	// dont push the  same thing twice
	if (GetState() != i_nState)
	{
		DBG_ASSERT0( i_nState >= 0,"Invalid state");
		m_nNewState = i_nState;
		m_bPushState = true;
	}
}

//----------------------------------------------------------------------------
// PopState if there is a state in the stack it restores it.
// otherwise it keeps the current state.
//----------------------------------------------------------------------------
inline void lodMode::PopState()
{
	m_bPopState = true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline const char* lodMode::GetMenuItemName()
{
	return m_szMenuItemName;
}
