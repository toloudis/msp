/****************************************************************************\
**	cmpsPickInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/cmps/cmpsPickInterest.hpp"

#include "Support/cmps/cmpsCompassMgr.hpp"

#include "Core/geo/geoPickRay.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsPickInterest::cmpsPickInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
cmpsPickInterest::~cmpsPickInterest()
{
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* cmpsPickInterest::MatchPickCode(envType::UInt32 i_PickCode) const
{
	return cmpsCompassMgr::MatchPickCode(i_PickCode);
}