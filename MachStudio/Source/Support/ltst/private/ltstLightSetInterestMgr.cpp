/*****************************************************************************
**	ltstLightSetInterestMgr.cpp
**
**	Keeps track of lights and objects that can be grouped so that certain
**	lights affect only certain objects.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Support/ltst/private/ltstLightSetInterestMgr.hpp"
#include "Support/ltst/ltstLightSetInterest.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include <vector>

namespace ltstLightSetInterestMgr
{

	namespace
	{
		std::vector<ltstLightSetInterest*>	l_InterestList;

		bool l_bDisableNotify = false;


	}	// end of namespace

	//--------------------------------------------------------------------
	//	RegisterLightSetInterest() - add a LightSet interest 
	//--------------------------------------------------------------------
	void RegisterLightSetInterest( ltstLightSetInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Light Set Interest" );
		l_InterestList.push_back( i_pInterest );
	}

	//--------------------------------------------------------------------
	//	UnRegisterLightSetInterest() - remove a LightSet interest 
	//
	//	Note: this will NOT delete the LightSet interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterLightSetInterest( ltstLightSetInterest* i_pInterest )
	{
		envSTLHelpers::RemoveOneValue( l_InterestList, i_pInterest );
	}

	//--------------------------------------------------------------------
	// Enable/Disable notification
	//--------------------------------------------------------------------
	void SetDisableNotify(bool i_bEnabled)
	{
		l_bDisableNotify = i_bEnabled;
	}

	//--------------------------------------------------------------------
	// Call DataChanged() on all interests
	//--------------------------------------------------------------------
	void NotifyInterestsChanged()
	{
		if (l_bDisableNotify) return;
		std::vector<ltstLightSetInterest*>::iterator it, end = l_InterestList.end();
		for (it  = l_InterestList.begin(); it != end; ++it)
		{
			(*it)->DataChanged();
		}
	}

	//--------------------------------------------------------------------
	// Call LightObjectAdded() on all interests
	//--------------------------------------------------------------------
	void NotifyInterestsAdded()
	{
		if (l_bDisableNotify) return;
		std::vector<ltstLightSetInterest*>::iterator it, end = l_InterestList.end();
		for (it  = l_InterestList.begin(); it != end; ++it)
		{
			(*it)->LightObjectAdded();
		}
	}

	//--------------------------------------------------------------------
	// Call LightRenamed() on all interests
	//--------------------------------------------------------------------
	void NotifyInterestsRenamed()
	{
		if (l_bDisableNotify) return;
		std::vector<ltstLightSetInterest*>::iterator it, end = l_InterestList.end();
		for (it  = l_InterestList.begin(); it != end; ++it)
		{
			(*it)->LightRenamed();
		}
	}


}	// end of namespace
