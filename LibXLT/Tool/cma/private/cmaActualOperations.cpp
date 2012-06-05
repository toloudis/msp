/*****************************************************************************
**	cmaActualOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Tool/cma/cmaActualOperations.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"


//------------------------------------------------------------------------
//  Add new hot key
//------------------------------------------------------------------------
void  cmaActualOperations::AddHotKey(const std::string& i_CommandName, const std::string& i_KeyCombo)
{
	cmaCommandMgr::UpdateHotKey(i_CommandName, i_KeyCombo);
}

//------------------------------------------------------------------------
//  Update a hot key
//------------------------------------------------------------------------
void  cmaActualOperations::UpdateHotKey(const std::string& i_CommandName, const std::string& i_KeyCombo)
{
	cmaCommandMgr::UpdateHotKey(i_CommandName, i_KeyCombo);
}
