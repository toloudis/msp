/****************************************************************************\
**	g3dRenderingHints.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dRenderingHints.hpp"


//============================================================================
//============================================================================
namespace g3dRenderingHints
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	namespace
	{
		bool l_bAllLowResolution = false;
		bool l_bAllFastRender = false;
	}

	//------------------------------------------------------------------------
	//	Set all flags to their default state
	//------------------------------------------------------------------------
	void RestoreDefaults()
	{
		l_bAllLowResolution = false;
		l_bAllFastRender = false;
	}

	//------------------------------------------------------------------------
	// Set to true if all renderings will be in low-resolution, 
	//	allows models to skip high-resolution animation.
	//------------------------------------------------------------------------
	bool GetAllLowResolution()
	{
		return l_bAllLowResolution;
	}
	void SetAllLowResolution(bool i_Val)
	{
		l_bAllLowResolution = i_Val;
	}

	//------------------------------------------------------------------------
	// Set to true if all renderings will be in fast mode (multipass off, 
	//	no reflections, etc)
	//------------------------------------------------------------------------
	bool GetAllFastRender()
	{
		return l_bAllFastRender;
	}
	void SetAllFastRender(bool i_Val)
	{
		l_bAllFastRender = i_Val;
	}
}	// end of namespace

