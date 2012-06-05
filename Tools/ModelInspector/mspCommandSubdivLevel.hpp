/*****************************************************************************
**  mspCommandSubdivLevel.hpp
**
**      Command to set subdivision level for model
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MSP_COMMANDSUBDIVLEVEL_HPP
#error mspCommandSubdivLevel.hpp multiply included
#endif
#define MSP_COMMANDSUBDIVLEVEL_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class mspCommandSubdivLevel : public cmaCommand
{
	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		mspCommandSubdivLevel(const std::string &i_Tag,
							  int i_Level);

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~mspCommandSubdivLevel();

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
inline int mspCommandSubdivLevel::GetLevel() const
{
	return m_Level;
}

