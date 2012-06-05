/*****************************************************************************
**  BakeCommand.hpp
**
**     Base class for commands that have timeline ranges
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef BAKECOMMAND_HPP
#error BakeCommand.hpp multiply included
#endif
#define BAKECOMMAND_HPP

#include <maya/MPxCommand.h>

//============================================================================
//============================================================================
class BakeCommand : public MPxCommand
{
protected:
	//========================================================================
	// Constructor 
	//========================================================================
	BakeCommand();

	//========================================================================
	// Parses an individual argument by index for the frame start and
	//	end flags. If it finds one, it will increment io_ArgIndex
	//	in order to parser the next argument as a frame value.
	//========================================================================
	bool ParseArg(const MArgList& i_Args, size_t &io_ArgIndex);

	//========================================================================
	// Gathers list of blend shapes for this mesh
	//========================================================================
	void GetTimelineRange(double &o_Min, double &o_Max);

	double	m_MinTime, m_MaxTime;
	bool	m_bMinTimeSet, m_bMaxTimeSet;
};

