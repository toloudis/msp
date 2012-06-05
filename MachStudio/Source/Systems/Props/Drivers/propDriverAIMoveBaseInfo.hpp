/********************************************************************************************\
**  propDriverAIMoveBaseInfo.hpp
**
**	Data structure for parsing AI movement drivers
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef PROP_DRIVERAIMOVEBASEINFO_HPP
#error propDriverAIMoveBaseInfo.hpp multiply included
#endif
#define PROP_DRIVERAIMOVEBASEINFO_HPP

#ifndef IT_STRING_HPP
#include "itString.hpp"
#endif
#ifndef TMLN_DRIVERSPLINEINFO_HPP
#include "tmlnDriverSplineInfo.hpp"
#endif
#ifndef PROP_DRIVERANIMATIONFULLINFO_HPP
#include "propDriverAnimationFullInfo.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif

#include <vector>


class propAIMoveBaseInfo
{
public:
	int			m_Stub;
};

class propDriverAIMoveBaseInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	propDriverAIMoveBaseInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~propDriverAIMoveBaseInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

public:
	//===========================================================================
	//	data
	//===========================================================================
	propAIMoveBaseInfo				m_AIMoveBaseInfo;
	propDriverAnimationFullInfo*	m_pAnimFullInfo;
	tmlnDriverSplineInfo*			m_pSplineInfo;
};

