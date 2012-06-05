/****************************************************************************\
**	pick3dMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/pick3d/pick3dMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoPickRay.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	std::vector<pick3dPickInterest*>	m_PickInterestList;
}


//----------------------------------------------------------------------------
//	RegisterPickInterest()
//----------------------------------------------------------------------------
void pick3dMgr::RegisterPickInterest( pick3dPickInterest* i_pInterest )
{
	DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Pick Interest" );

	m_PickInterestList.push_back( i_pInterest );
}

//--------------------------------------------------------------------
//	UnRegisterPickInterest() - remove a pick interest from the system.
//
//	Note: this will NOT delete the pick interest.  It is up to the
//	registerer.
//--------------------------------------------------------------------
void pick3dMgr::UnRegisterPickInterest( pick3dPickInterest* i_pInterest )
{
	envSTLHelpers::RemoveOneValue( m_PickInterestList, i_pInterest );
}

//----------------------------------------------------------------------------
//	Clear() - clear the list
//----------------------------------------------------------------------------
void pick3dMgr::Clear()
{
	m_PickInterestList.clear();
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
pick3dPickObject* pick3dMgr::MatchPickCode(envType::UInt32 i_PickCode)
{
	std::vector<pick3dPickInterest*>::iterator it;
	std::vector<pick3dPickInterest*>::iterator end = m_PickInterestList.end();
	
	it  = m_PickInterestList.begin();
	while ( it != end )
	{
		pick3dPickObject* pPicked = (*it)->MatchPickCode( i_PickCode );
		if (pPicked)
			return pPicked;

		it++;
	}

	return NULL;
}
