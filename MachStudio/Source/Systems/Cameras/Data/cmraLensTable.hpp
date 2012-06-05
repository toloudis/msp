/*****************************************************************************
**	cmraLensTable.hpp
**
**	Table of lens types. Matches a string for a lens name to the camera
**	properties for this lens.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_LENSTABLE_HPP
#error cmraLensTable.hpp multiply included
#endif
#define CMRA_LENSTABLE_HPP

#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif 


//============================================================================
//============================================================================
namespace cmraLensTable
{
	//--------------------------------------------------------------------
	// Add the types of film gates possible to this enumeration.
	//--------------------------------------------------------------------
	void SetUpFilmGateEnum(prtyEnum &o_FilmGate);
	
	//--------------------------------------------------------------------
	// Return true if this enumeration value is the "User" type.
	// If it is the user type, then do not use the table for the
	// camera properties.
	//--------------------------------------------------------------------
	bool IsUserType(int i_FilmGateEnumValue);
	int GetUserType();

	//--------------------------------------------------------------------
	// Returns horizontal aperture in inches for the flim gate given
	//--------------------------------------------------------------------
	float GetHorizontalAperture(int i_FilmGateEnumValue);
	int GetEnumerationForAperture(float i_HorizontalAperture);
}
