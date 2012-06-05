/*****************************************************************************
**	cmaActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CMA_ACTUALOPERATIONS_HPP
#error cmaActualOperations.hpp multiply included
#endif
#define CMA_ACTUALOPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class cmaData;
class cmaScriptData;
class nameString;
class maRotation;


//============================================================================
//============================================================================
class cmaActualOperations
{
public:
	//------------------------------------------------------------------------
	//  Add new hot key
	//------------------------------------------------------------------------
	static void AddHotKey(const std::string& i_CommandName, const std::string& i_KeyCombo);

	//------------------------------------------------------------------------
	//  Update a hot key
	//------------------------------------------------------------------------
	static void UpdateHotKey(const std::string& i_CommandName, const std::string& i_KeyCombo);

};	// end of static class
