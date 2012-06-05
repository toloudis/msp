/****************************************************************************\
**	chnlTimeCommon.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlTimeCommon.hpp"

namespace
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlTimeCommon::chnlTimeCommon()
:	m_MinTime(0),
	m_MaxTime(60),
	m_TimeScale(10)
{	
}

//--------------------------------------------------------------------
// SetTotalTime changes length of timeline controls
//--------------------------------------------------------------------
//void chnlTimeCommon::SetTotalTime(float i_Time)
//{
//	m_TotalTime = i_Time;
//	this->update_size();
//}
//float chnlTimeCommon::GetTotalTime() const
//{
//	return m_TotalTime;
//}

//--------------------------------------------------------------------
// SetTimeRange changes length of timeline controls, setting
//	 minimum and maximum time range.
//--------------------------------------------------------------------
void chnlTimeCommon::SetTimeRange(float i_MinTime, float i_MaxTime)
{
	m_MinTime = i_MinTime;
	m_MaxTime = i_MaxTime;
	this->update_size();
}
float chnlTimeCommon::GetMinTime() const
{
	return m_MinTime;
}
float chnlTimeCommon::GetMaxTime() const
{
	return m_MaxTime;
}

//--------------------------------------------------------------------
// SetTimeScale
//--------------------------------------------------------------------
void chnlTimeCommon::SetTimeScale(float i_Scale)
{
	m_TimeScale = i_Scale;
	this->update_size();
}
float chnlTimeCommon::GetTimeScale() const
{
	return m_TimeScale;
}

//----------------------------------------------------------------------------
// Compute width based on time range and scale
//----------------------------------------------------------------------------
int chnlTimeCommon::get_full_width()
{
	return (int)(m_TimeScale * (m_MaxTime-m_MinTime) + 2*c_EdgeOffset);
}

//--------------------------------------------------------------------
// Get the horizontal position of the given time
//--------------------------------------------------------------------
int chnlTimeCommon::get_position_for_time(float i_Time)
{
	int pos = (int)((i_Time-m_MinTime) * m_TimeScale + c_EdgeOffset);
	return pos;
}

//--------------------------------------------------------------------
//	Get the time based on the horizontal position
//--------------------------------------------------------------------
float chnlTimeCommon::get_time_at_position(int i_ScreenXPos)
{
	float time = m_MinTime + (i_ScreenXPos - c_EdgeOffset) / m_TimeScale;
	return time;
}
