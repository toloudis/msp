/*****************************************************************************
**	chnlTimeTickMgr.cpp
**
**	Maintains ticks in time to which drivers can be snapped
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlTimeTickMgr.hpp"

#include "Features/Channels/chnlSnapUtil.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"

#include <algorithm>
#include <math.h>



//============================================================================
//============================================================================
namespace chnlTimeTickMgr
{
	namespace
	{
		std::set<maTime>	l_TimeTicks;				// Set of begin and end times of drivers
		std::vector<tmlnScriptObject*> l_Objects;	// Copy of list of objects we used when gathering ticks

		//------------------------------------------------------------------------
		// gather time ticks from l_Objects list
		//------------------------------------------------------------------------
		void gather_time_ticks()
		{
			l_TimeTicks.clear();
			std::vector<tmlnScriptObject*>::const_iterator obj_it;
			for (obj_it = l_Objects.begin(); obj_it != l_Objects.end(); ++obj_it)
			{
				tmlnScriptObject* pObject = (*obj_it);
				if (pObject)
				{
					const int num_drivers = pObject->GetNumDrivers();
					for (int i=0; i<num_drivers; ++i)
					{
						const tmlnDriver &driver = pObject->GetDriver(i);
						l_TimeTicks.insert(driver.GetBeginTime());
						l_TimeTicks.insert(driver.GetEndTime());
					}
				}
			}
		}

	}	// end of namespace

	//------------------------------------------------------------------------
	//	Gather time ticks from the begin and end times of the drivers
	//	in the given list of objects.
	//------------------------------------------------------------------------
	void GatherTimeTicks(const std::vector<tmlnScriptObject*>& i_Objects)
	{
		l_Objects = i_Objects;
		gather_time_ticks();
	}
	void GatherTimeTicks(tmlnScriptObject* i_pObject)
	{
		l_Objects.clear();
		if (i_pObject != NULL)
			l_Objects.push_back(i_pObject);
		gather_time_ticks();
	}

	//------------------------------------------------------------------------
	// Clear list of objects used to detect time ticks.
	//------------------------------------------------------------------------
	void ClearTimeTicks()
	{
		l_Objects.clear();
		l_TimeTicks.clear();
	}

	//------------------------------------------------------------------------
	//	Update position of time ticks based on the same list given to us
	//	in earlier call to GatherTimeTicks
	//------------------------------------------------------------------------
	void UpdateTimeTicks()
	{
		gather_time_ticks();
	}

	//------------------------------------------------------------------------
	//	Accessor to set of time ticks
	//------------------------------------------------------------------------
	const std::set<maTime>&	GetTimeTicks()
	{
		return l_TimeTicks;
	}

	//------------------------------------------------------------------------
	// SnapTimeToTicks - see if given time is within snap threshold
	//	of a tick. If so, return true and the time for the tick.
	//------------------------------------------------------------------------
	bool SnapTimeToTicks(const maTime& i_Time, maTime& o_TickTime)
	{
		bool bFound = false;

		std::set<maTime>::iterator next_it = l_TimeTicks.lower_bound(i_Time);
		if (next_it != l_TimeTicks.end())
		{
			if (chnlSnapUtil::TimesMatch(i_Time, *next_it))
			{
				o_TickTime = *next_it;
				bFound = true;
			}
		}

		if (next_it != l_TimeTicks.begin())
		{
			// Get previous time tick also
			std::set<maTime>::iterator prev_it = next_it;
			--prev_it;

			if (chnlSnapUtil::TimesMatch(i_Time, *prev_it))
			{
				if (!bFound)
				{
					o_TickTime = *prev_it;
					bFound = true;
				}
				else
				{
					// See if prev_it is closer
					maTime diff1 = (*next_it - i_Time).Abs();
					maTime diff2 = (*prev_it - i_Time).Abs();
					if (diff1 > diff2)
						o_TickTime = *prev_it;
				}
			}
		}

		return bFound;
	}

	//------------------------------------------------------------------------
	// Get nearest tick to given time in either direction.
	// Returns -1 if not found.
	//------------------------------------------------------------------------
	maTime GetNextTickTime( const maTime& i_Time )
	{
		std::set<maTime>::iterator next_it = l_TimeTicks.upper_bound(i_Time);
		if (next_it != l_TimeTicks.end())
		{
			return *next_it;
		}
		return maTime::FromSeconds(-1);	//TIME - magic number
	}
	maTime GetPrevTickTime( const maTime& i_Time )
	{
		std::set<maTime>::iterator prev_it = l_TimeTicks.lower_bound(i_Time);
		if (prev_it != l_TimeTicks.begin())
		{
			prev_it--;
			return *prev_it;
		}
		return maTime::FromSeconds(-1);	//TIME - magic number
	}

}	// end of namespace
