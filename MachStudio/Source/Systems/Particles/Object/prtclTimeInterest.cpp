/****************************************************************************\
**	prtclTimeInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Object/prtclTimeInterest.hpp"

#include "Systems/Particles/Undo/prtclOperations.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclTimeInterest::prtclTimeInterest()
: m_fLastTime(0.0f)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
prtclTimeInterest::~prtclTimeInterest()
{
}

//--------------------------------------------------------------------
//	TimeChanged - timeline current time has changed
//--------------------------------------------------------------------
//virtual 
void prtclTimeInterest::TimeChanged( float i_Time )
{
	if (	(i_Time == 0.0f)
		&&	(m_fLastTime != 0.0f) )
	{
		prtclOperations::ResetGenerators();
	}

	m_fLastTime = i_Time;
}

//--------------------------------------------------------------------
//	TimeChanged - timeline current time has changed
//--------------------------------------------------------------------
//virtual 
void prtclTimeInterest::TimeRangeChanged(float i_MinTime, float i_MaxTime)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void prtclTimeInterest::TimeFormatChanged(int i_TimeFormat)
{
}


