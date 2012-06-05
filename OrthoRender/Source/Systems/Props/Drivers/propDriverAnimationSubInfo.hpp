/********************************************************************************************\
**  propDriverAnimationSubInfo.hpp
**
**	Data structure for parsing animation drivers
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef PROP_DRIVERANIMATIONSUBINFO_HPP
#error propDriverAnimationInfo.hpp multiply included
#endif
#define PROP_DRIVERANIMATIONSUBINFO_HPP

#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif
#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif

#include <vector>


class propAnimationSubInfo
{
public:
	propAnimationSubInfo();

	int			m_AnimIndex;
	itString	m_AnimName;
	bool		m_bDriverResizeByAnimLength;
};

class propDriverAnimationSubInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	propDriverAnimationSubInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~propDriverAnimationSubInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	propAnimationSubInfo m_Info;
};

