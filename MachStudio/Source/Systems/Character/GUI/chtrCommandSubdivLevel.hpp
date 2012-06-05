/*****************************************************************************
**  chtrCommandSubdivLevel.hpp
**
**      command to toggle the icons of a system
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef CHTR_COMMANDSUBDIVLEVEL_HPP
#error chtrCommandSubdivLevel.hpp multiply included
#endif
#define CHTR_COMMANDSUBDIVLEVEL_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class chtrCommandSubdivLevel : public cmaCommand
{
	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		chtrCommandSubdivLevel(int i_Level);

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~chtrCommandSubdivLevel();

		//--------------------------------------------------------------------
		// Return subdivision level
		//--------------------------------------------------------------------
		inline int GetLevel() const;

private:
	int m_Level;
};

//--------------------------------------------------------------------
// Return subdivision level
//--------------------------------------------------------------------
inline int chtrCommandSubdivLevel::GetLevel() const
{
	return m_Level;
}

