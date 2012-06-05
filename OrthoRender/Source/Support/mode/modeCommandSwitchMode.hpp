/*****************************************************************************
**  modeCommandSwitchMode.hpp
**
**      command to switch the current mode with another one.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef MODE_COMMANDSWITCHMODE_HPP
#error modeCommandSwitchMode.hpp multiply included
#endif
#define MODE_COMMANDSWITCHMODE_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class modeCommandSwitchMode : public cmaCommand
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static const std::string GetConstTagName();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		modeCommandSwitchMode( int i_ModeID );

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~modeCommandSwitchMode();

		//--------------------------------------------------------------------
		//	GetModeID()
		//--------------------------------------------------------------------
		inline int GetModeID();

	private:
		int		m_ModeID;
};

//--------------------------------------------------------------------
//	GetModeID()
//--------------------------------------------------------------------
inline int modeCommandSwitchMode::GetModeID()
{
	return m_ModeID;
}

