/*****************************************************************************
**	tmlnDriverIdMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnDriverIdMgr.hpp"

#include "Support/tmln/tmlnDriver.hpp"

//#include "Core/dbg/dbgAssert.hpp"

#include <map>


//============================================================================
//============================================================================
namespace tmlnDriverIdMgr 
{
	namespace
	{
		tmlnDriverId l_Counter = 1;
		std::map<tmlnDriverId, tmlnDriver*> l_DriverIdMap;
	}

	//------------------------------------------------------------------------
	// Submit a driver and id. If the id is 0, then a new id will be 
	//	generated and returned in the parameter.
	//------------------------------------------------------------------------
	void SubmitDriver(tmlnDriver *i_pDriver, tmlnDriverId &io_Id)
	{
		if (io_Id == 0)
		{
			// Generate new id from counter
			io_Id = l_Counter++;
		}

#ifdef DEBUG
		std::map<tmlnDriverId, tmlnDriver*>::iterator it = l_DriverIdMap.find(i_Id);
		DBG_ASSERT1(it == l_DriverIdMap.end(), "Driver id (%d) already is registered.", i_Id);
#endif

		l_DriverIdMap[io_Id] = i_pDriver;
	}

	//------------------------------------------------------------------------
	// Remove driver from the manager
	//------------------------------------------------------------------------
	void RemoveDriver(tmlnDriver *i_pDriver, tmlnDriverId i_Id)
	{
		std::map<tmlnDriverId, tmlnDriver*>::iterator it = l_DriverIdMap.find(i_Id);
		if (it != l_DriverIdMap.end())
		{
			if (it->second != i_pDriver)
			{
				tmlnDriver* p2ndDriver = dynamic_cast<tmlnDriver*>(it->second);
				DBG_ASSERT3(it->second == i_pDriver, "Driver %s pointer does not match id's mapping (id == %d vs %d).", i_pDriver->GetName().c_str(), i_Id, p2ndDriver->GetDriverId());
			}

			l_DriverIdMap.erase(it);
		}
	}

	//------------------------------------------------------------------------
	// Return driver for given id
	//------------------------------------------------------------------------
	tmlnDriver*	GetDriverById(tmlnDriverId i_Id)
	{
		std::map<tmlnDriverId, tmlnDriver*>::iterator it = l_DriverIdMap.find(i_Id);
		if (it != l_DriverIdMap.end())
		{
			return it->second;
		}
		return NULL;
	}
	
}