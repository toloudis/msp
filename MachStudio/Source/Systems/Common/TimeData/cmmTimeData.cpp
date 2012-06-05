/********************************************************************************************\
**  cmmTimeData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "Systems/Common/TimeData/cmmTimeData.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmmTimeData::cmmTimeData()
:	m_MinimumTime(maTime::c_ZeroTime), m_MaximumTime(maTime::FromSeconds(10))
{
}

