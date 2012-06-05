/*****************************************************************************
**	chnlTimeTickMgr.hpp
**
**	Maintains ticks in time to which drivers can be snapped
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TIMETICKMGR_HPP
#error chnlTimeTickMgr.hpp multiply included
#endif
#define CHNL_TIMETICKMGR_HPP

#include <vector>
#include <set>


//============================================================================
//============================================================================
class tmlnScriptObject;


//============================================================================
//============================================================================
namespace chnlTimeTickMgr
{
	//------------------------------------------------------------------------
	//	Gather time ticks from the begin and end times of the drivers
	//	in the given list of objects.
	//------------------------------------------------------------------------
	void GatherTimeTicks(const std::vector<tmlnScriptObject*>& i_Objects);
	void GatherTimeTicks(tmlnScriptObject* i_pObject);

	//------------------------------------------------------------------------
	// Clear list of objects used to detect time ticks.
	//------------------------------------------------------------------------
	void ClearTimeTicks();

	//------------------------------------------------------------------------
	//	Update position of time ticks based on the same list given to us
	//	in earlier call to GatherTimeTicks.
	//------------------------------------------------------------------------
	void UpdateTimeTicks();

	//------------------------------------------------------------------------
	//	Accessor to set of time ticks
	//------------------------------------------------------------------------
	const std::set<float>&	GetTimeTicks();

	//------------------------------------------------------------------------
	// SnapTimeToTicks - see if given time is within snap threshold
	//	of a tick. If so, return true and the time for the tick.
	//------------------------------------------------------------------------
	bool SnapTimeToTicks(float i_Time, float& o_TickTime);
	
	//------------------------------------------------------------------------
	// Get nearest tick to given time in either direction.
	// Returns -1 if not found.
	//------------------------------------------------------------------------
	float GetNextTickTime( float i_Time );
	float GetPrevTickTime( float i_Time );

}	// end of namespace
