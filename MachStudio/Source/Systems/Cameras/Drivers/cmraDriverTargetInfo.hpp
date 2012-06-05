/********************************************************************************************\
**  cmraDriverTargetInfo.hpp
**
**	Data structure for parsing sound drivers
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERTARGETINFO_HPP
#error cmraDriverTargetInfo.hpp multiply included
#endif
#define CMRA_DRIVERTARGETINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <string>


class cmraDriverTargetInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverTargetInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverTargetInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	nameString	m_ObjectName;	// name of object (prop) to target
	std::string m_AttachName;	// name of node to attach to
	maPoint3d	m_TargetOffset;	// target offset within that attachment matrix
};


