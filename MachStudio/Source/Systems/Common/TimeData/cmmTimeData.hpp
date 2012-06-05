/********************************************************************************************\
**  cmmTimeData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef CMM_TIMEDATA_HPP
#error cmmTimeData.hpp multiply included
#endif
#define CMM_TIMEDATA_HPP

#ifndef MA_TIME_HPP
#include "Core/Ma/maTime.hpp"
#endif 


//============================================================================
//============================================================================
class cmmTimeData
{
public:
	cmmTimeData();

	maTime m_MinimumTime;
	maTime m_MaximumTime;
};

