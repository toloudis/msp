/********************************************************************************************\
**  tmlnDriverAttachInfo.hpp
**
**	Data structure for parsing sound drivers
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERATTACHINFO_HPP
#error tmlnDriverAttachInfo.hpp multiply included
#endif
#define TMLN_DRIVERATTACHINFO_HPP

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


class tmlnDriverAttachInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverAttachInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverAttachInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();


	nameString	m_ObjectName;	// name of object (prop) to target
	std::string m_AttachName;	// name of node to attach to
	maPoint3d	m_AttachOffset;	// target offset within that attachment matrix
	maVector3d	m_WorldSpaceOffset;	// world space offset, added after local point is transformed
	bool		m_bUseBbox;		// Use center of bounding box instead of node transform
};


