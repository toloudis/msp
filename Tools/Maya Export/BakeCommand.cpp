/*****************************************************************************
**  BakeCommand.cpp
**
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include <MayaUtil.hpp>
#include <BakeCommand.hpp>
#include <AnimFuncs.hpp>

#include <maya/MArgList.h>

//========================================================================
// Constructor
//========================================================================
BakeCommand::BakeCommand()
: m_MinTime(0),
  m_MaxTime(-1),
  m_bMinTimeSet(false),
  m_bMaxTimeSet(false)
{
}

//========================================================================
// Parses an individual argument by index for the frame start and
//	end flags. If it finds one, it will increment io_ArgIndex
//	in order to parser the next argument as a frame value.
//========================================================================
bool BakeCommand::ParseArg(const MArgList& i_Args, size_t &io_ArgIndex)
{
	const MString	frameStartFlag			("-fs");
	const MString	frameEndFlag			("-fe");

	MStatus     	stat;
	MString arg = i_Args.asString( io_ArgIndex, &stat );
	if (!stat)              
		return false;

	if ( arg == frameStartFlag ) 
	{
		m_MinTime = i_Args.asDouble(io_ArgIndex + 1, &stat);
		io_ArgIndex++;
		if (stat)
		{
			m_bMinTimeSet = true;
			return true;
		}
	}
	else if ( arg == frameEndFlag ) 
	{
		m_MaxTime = i_Args.asDouble(io_ArgIndex + 1, &stat);
		io_ArgIndex++;
		if (stat)
		{
			m_bMaxTimeSet = true;
			return true;
		}
	}

	return false;
}

//========================================================================
// Gets range for timeline from command arguments or from 
//	Maya´s timeline slider
//========================================================================
void BakeCommand::GetTimelineRange(double &o_Min, double &o_Max)
{
	double minTime, maxTime;
	AnimFuncs::GetTimeSliderRange(minTime, maxTime);

	o_Min = (m_bMinTimeSet) ? m_MinTime : minTime;
	o_Max = (m_bMaxTimeSet) ? m_MaxTime : maxTime;
}


