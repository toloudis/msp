/*****************************************************************************
**  cmmSplineCommands.hpp
**
**      Spline control point related commands
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SPLINECOMMANDS_HPP
#error cmmSplineCommands.hpp multiply included
#endif
#define CMM_SPLINECOMMANDS_HPP


//============================================================================
//============================================================================
namespace cmmSplineCommands
{
	//--------------------------------------------------------------------
	// SetupMenu -
	//--------------------------------------------------------------------
	void SetupMenu();
	
	//--------------------------------------------------------------------
	// Show or hide the toolbar for spline editing control
	//--------------------------------------------------------------------
	void ShowSplineToolbar(bool i_bShow);
};
