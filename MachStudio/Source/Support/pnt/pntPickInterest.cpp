/****************************************************************************\
**	pntPickInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/pnt/pntPickInterest.hpp"

#include "Support/pnt/pntPointSelect.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
pntPickInterest::pntPickInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
pntPickInterest::~pntPickInterest()
{
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* pntPickInterest::MatchPickCode(envType::UInt32 i_PickCode) const
{
	return pntPointSelect::MatchPickCode(i_PickCode);
}
