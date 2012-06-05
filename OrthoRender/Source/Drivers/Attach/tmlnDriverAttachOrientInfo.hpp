/********************************************************************************************\
**  tmlnDriverAttachOrientInfo.hpp
**
**	Data structure for parsing attachment drivers
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERATTACHORIENTINFO_HPP
#error tmlnDriverAttachOrientInfo.hpp multiply included
#endif
#define TMLN_DRIVERATTACHORIENTINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <string>


class tmlnDriverAttachOrientInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverAttachOrientInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverAttachOrientInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();


	nameString	m_ObjectName;	// name of object (prop) to target
	std::string m_AttachName;	// name of node to attach to
	maPoint3d	m_AttachOffset;	// target offset within that attachment matrix
	maRotation  m_AttachOrientation; // rotational offset within attachment matrix
	maVector3d	m_WorldSpaceOffset;	// world space offset, added after local point is transformed
};


