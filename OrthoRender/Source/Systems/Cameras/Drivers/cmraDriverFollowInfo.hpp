/********************************************************************************************\
**  cmraDriverFollowInfo.hpp
**
**	Data structure for camera driver Follow
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERFOLLOWINFO_HPP
#error cmraDriverFollowInfo.hpp multiply included
#endif
#define CMRA_DRIVERFOLLOWINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif


//============================================================================
//============================================================================
class cmraDriverFollowInfo : public tmlnDriverInfo
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmraDriverFollowInfo(chDefs::Name i_ChunkName);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~cmraDriverFollowInfo();

	//------------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//------------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

public:
	maVector3d	m_Direction;
};

