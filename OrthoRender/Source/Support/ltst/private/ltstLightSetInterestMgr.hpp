/*****************************************************************************
**	ltstLightSetInterestMgr.hpp
**
**	Supporting namespace for the light set manager for maintaining
**	and notifying light set interests.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef LTST_LIGHTSETINTERESTMGR_HPP
#error ltstLightSetInterestMgr.hpp multiply included
#endif
#define LTST_LIGHTSETINTERESTMGR_HPP

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class ltstLightSetInterest;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace ltstLightSetInterestMgr
{
	//--------------------------------------------------------------------
	//	RegisterLightSetInterest() - add a LightSet interest 
	//--------------------------------------------------------------------
	void RegisterLightSetInterest( ltstLightSetInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterLightSetInterest() - remove a LightSet interest 
	//
	//	Note: this will NOT delete the LightSet interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterLightSetInterest( ltstLightSetInterest* i_pInterest );

	//--------------------------------------------------------------------
	// Enable/Disable notification
	//--------------------------------------------------------------------
	void SetDisableNotify(bool i_bEnabled);

	//--------------------------------------------------------------------
	// Call DataChanged() on all interests
	//--------------------------------------------------------------------
	void NotifyInterestsChanged();

	//--------------------------------------------------------------------
	// Call LightObjectAdded() on all interests
	//--------------------------------------------------------------------
	void NotifyInterestsAdded();

	//--------------------------------------------------------------------
	// Call LightRenamed() on all interests
	//--------------------------------------------------------------------
	void NotifyInterestsRenamed();

}	// end of namespace
