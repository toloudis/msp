/****************************************************************************\
**	chnlTimeIcon.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlTimeIcon.hpp"

#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/Ma/maTime.hpp"

#include <sstream>
#include <iomanip>

#ifdef USE_WXWIDGETS

namespace
{
	const int c_EdgeOffset = 16;

	int compute_location(float i_Time, float i_MinTime, float i_TimeScale)
	{
		return (int)((i_Time-i_MinTime) * i_TimeScale + c_EdgeOffset);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chnlTimeIcon::chnlTimeIcon(float i_Time, 
						   float i_MinTime,
						   float i_TimeScale)
:	m_Time(i_Time),
	m_Position(compute_location(i_Time, i_MinTime, i_TimeScale))
{		
}

//--------------------------------------------------------------------
// Alter position of note based on scaling of time values
//--------------------------------------------------------------------
void chnlTimeIcon::AlterTimeScale(float i_MinTime, float i_TimeScale)
{
	m_Position = compute_location(m_Time, i_MinTime, i_TimeScale);
}


//--------------------------------------------------------------------
// comparison function to sort note icons
//--------------------------------------------------------------------
bool chnlTimeIcon::operator<(const chnlTimeIcon& i_Icon) const
{
	return (this->m_Time < i_Icon.m_Time);
}

//--------------------------------------------------------------------
// Return string to display when mouse hovers over icon
//--------------------------------------------------------------------
std::string chnlTimeIcon::GetHoverDescription() const
{
	// Base class converts the time to a string format
	//int min, sec, frames;
	//min	= (int)(((int)(m_Time)) / 60.0f);
	//sec	= (int)(((int)m_Time) - (float)min * 60.0f);
 //   const float FPS = 24.0f;    // TODO 24fps is hardcoded.
 //   frames = (int)(((float)(m_Time - ((int)m_Time))) * FPS);

	//std::ostringstream str;
	//str << std::setw(2) << std::setfill('0') << min << ":";
	//str << std::setw(2) << std::setfill('0') << sec << "." << frames;
	//return str.str();

	std::string time_string;
	tmlnTimeUtil::GetTimeString(maTime::FromSeconds(m_Time), time_string);
	return time_string;

}

#endif // USE_WXWIDGETS


