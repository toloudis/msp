/****************************************************************************\
**	splnPickInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/spln/splnPickInterest.hpp"

#include "Support/spln/splnCurveSelect.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
splnPickInterest::splnPickInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
splnPickInterest::~splnPickInterest()
{
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* splnPickInterest::MatchPickCode(envType::UInt32 i_PickCode) const
{
	return splnCurveSelect::MatchPickCode(i_PickCode);
}

