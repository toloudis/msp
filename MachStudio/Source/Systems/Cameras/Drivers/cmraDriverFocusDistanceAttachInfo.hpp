/********************************************************************************************\
**  cmraDriverFocusDistanceAttachInfo.hpp
**
**	Data structure for parsing attachment drivers
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERFOCUSDISTANCEATTACHINFO_HPP
#error cmraDriverFocusDistanceAttachInfo.hpp multiply included
#endif
#define CMRA_DRIVERFOCUSDISTANCEATTACHINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class cmraDriverFocusDistanceAttachInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverFocusDistanceAttachInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverFocusDistanceAttachInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

public:
	nameString	m_ObjectName;		// name of object (prop) to target
	std::string m_AttachName;	// name of node to attach to
};


