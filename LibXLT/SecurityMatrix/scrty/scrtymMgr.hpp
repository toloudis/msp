/*****************************************************************************
**	scrtymMgr.hpp
**
**		Manage the software security
**
**		This manager provides methods of checking a security dongle.  The
**	methods of checking have been set-up to make it harder to reverse
**	engineer or hack.
**
**
**	Check types
**		- days valid
**		- usage count
**		- minutes used (TBD)
**
**
**	DONGLE MEMORY
**	=============
**	01 - reserved
**	02 - reserved
**	03 - activate date (initially 0) (Julian)
**	04 - usage type (1=days valid, 2=usage count, 3=minutes)
**	05 - access type (0=full, 1=limited,...)
**	06 - run count
**	07 - minutes used
**	08 - days valid
**	09 - uses left (decrement)
**	10 - minutes valid
**	11 - value (always even)
**	12 - value 2 (derived from 11)
**	13 - invalid counter (gets set when dongle becomes invalid and counts down uses)
**	14 - semi-random (if < 500 invalid dongle, if > 500 valid dongle)
**	15 - last access date
**
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SCRTYM_MGR_HPP
#error scrtymMgr.hpp multiply included
#endif
#define SCRTYM_MGR_HPP

#ifndef SCRTY_MGR_HPP
#include "Core/scrty/scrtyMgr.hpp"
#endif


//============================================================================
//============================================================================
class scrtymMgr : public scrtyMgrImpl
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void Init();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void CleanUp();

	//------------------------------------------------------------------------
	//	for testing purposes only
	//------------------------------------------------------------------------
	virtual void TestDongle();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual bool DongleValid();

	//------------------------------------------------------------------------
	//	Call ONCE on launch of program
	//------------------------------------------------------------------------
	virtual void OpenDongle();

	//------------------------------------------------------------------------
	//	Call ONCE on exit of program
	//------------------------------------------------------------------------
	virtual void CloseDongle();
};
