/********************************************************************************************\
**  cmraDriverDataAttachNodeInfo.hpp
**
**		Data structure for cmra AttachNode info.
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERDATAATTACHNODEINFO_HPP
#error cmraDriverDataAttachNodeInfo.hpp multiply included
#endif
#define CMRA_DRIVERDATAATTACHNODEINFO_HPP

#ifndef CH_PARSABLE_HPP
#include "Core/Ch/chParsable.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class cmraDriverDataAttachNodeInfo : public chParsable
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverDataAttachNodeInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverDataAttachNodeInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual cmraDriverDataAttachNodeInfo* Clone();

	nameString	m_ObjectName;	// name of object (prop) to target
	std::string m_AttachName;	// name of node to attach to
	maPoint3d	m_TargetOffset;	// target offset within that attachment matrix
};


