/********************************************************************************************\
**  cmraDriverFramedArcInfo.hpp
**
**		Data structure for cmra FramedArc
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERFRAMEDARCINFO_HPP
#error cmraDriverFramedArcInfo.hpp multiply included
#endif
#define CMRA_DRIVERFRAMEDARCINFO_HPP

#ifndef CMRA_DRIVERFRAMEDBASEINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverFramedBaseInfo.hpp"
#endif
#ifndef CMRA_DRIVERDATAPOSITIONINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataPositionInfo.hpp"
#endif
#ifndef CMRA_DRIVERDATASUBJECTINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataSubjectInfo.hpp"
#endif
#ifndef CMRA_DRIVERDATAVIEWTYPEINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataViewTypeInfo.hpp"
#endif


class cmraDriverFramedArcInfo : public cmraDriverFramedBaseInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverFramedArcInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverFramedArcInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	float	m_fRevolutions;
	bool	m_bClockwise;
};


