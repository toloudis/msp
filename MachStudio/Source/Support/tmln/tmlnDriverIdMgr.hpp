/*****************************************************************************
**	tmlnDriverIdMgr.hpp
**
**	This manager maintains a mapping from driver id to driver so that
**	we can refer to a driver uniquely. This is used to support the python
**	API (python will use the id to refer to a driver).
**
**	This id is meant to be persistent for the length of a scene, but will
**	not be used to maintain references during save and load. It will, however,
**	maintain an id for a driver in the situation when the script object
**	re-generates it drivers through tmlnCreator::SetDriverInfo().
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERIDMGR_HPP
#error tmlnDriverIdMgr.hpp multiply included
#endif
#define TMLN_DRIVERIDMGR_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

//============================================================================
//	tmlnDriverId
//============================================================================
typedef envType::UInt64 tmlnDriverId;

//============================================================================
//	forward references
//============================================================================
class tmlnDriver;


//============================================================================
//============================================================================
namespace tmlnDriverIdMgr
{
	//------------------------------------------------------------------------
	// Submit a driver and id. If the id is 0, then a new id will be 
	//	generated and returned in the parameter.
	//------------------------------------------------------------------------
	void SubmitDriver(tmlnDriver *i_pDriver, tmlnDriverId &io_Id);

	//------------------------------------------------------------------------
	// Remove driver from the manager
	//------------------------------------------------------------------------
	void RemoveDriver(tmlnDriver *i_pDriver, tmlnDriverId i_Id);

	//------------------------------------------------------------------------
	// Return driver for given id
	//------------------------------------------------------------------------
	tmlnDriver*	GetDriverById(tmlnDriverId i_Id);
};
