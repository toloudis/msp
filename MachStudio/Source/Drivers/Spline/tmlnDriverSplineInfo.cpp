/*****************************************************************************
**	tmlnDriverSplineInfo.cpp
**
**	Data structure for parsing spline drivers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Drivers/Spline/tmlnDriverSplineInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverSplineInfo::tmlnDriverSplineInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverSplineInfo::~tmlnDriverSplineInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverSplineInfo::Clone()
{
	return new tmlnDriverSplineInfo(*this);
}
