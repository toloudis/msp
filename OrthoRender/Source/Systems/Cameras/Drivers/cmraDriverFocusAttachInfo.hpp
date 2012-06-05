/********************************************************************************************\
**  cmraDriverFocusAttachInfo.hpp
**
**	Data structure for parsing attachment drivers
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERFOCUSATTACHINFO_HPP
#error cmraDriverFocusAttachInfo.hpp multiply included
#endif
#define CMRA_DRIVERFOCUSATTACHINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class cmraDriverFocusAttachInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverFocusAttachInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverFocusAttachInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

public:
	nameString	m_ObjectName;		// name of object (prop) to target
	std::string m_AttachName;	// name of node to attach to

	float m_NearFocusOffset;
	float m_FarFocusOffset;
	float m_NearFalloffDist;
	float m_FarFalloffDist;
};


