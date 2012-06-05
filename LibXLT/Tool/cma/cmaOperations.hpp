/*****************************************************************************
**	cmaOperations.hpp
**
**	Utility for doing operations that are undoable in Commands
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CMA_OPERATIONS_HPP
#error cmaOperations.hpp multiply included
#endif
#define CMA_OPERATIONS_HPP

#include <string>


//============================================================================
//============================================================================
namespace cmaOperations
{
	//------------------------------------------------------------------------
	//  Add new hot key
	//------------------------------------------------------------------------
	void  AddHotKey(const std::string& i_CommandName, const std::string& i_KeyCombo);

	//------------------------------------------------------------------------
	//  Update a hot key
	//------------------------------------------------------------------------
	void  UpdateHotKey(const std::string& i_CommandName, const std::string& i_KeyCombo);
}

