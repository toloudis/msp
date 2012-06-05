/****************************************************************************\
**	rpnRenderingPrefs.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/rpnRenderingPrefs.hpp"

#include "Graphics/g3d/g3dRenderingHints.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"

namespace rpnRenderingPrefs
{
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
	// Set to true to force all render panels to use low resolution
	//------------------------------------------------------------------------
	bool GetAllLowResolution()
	{
		return l_bAllLowResolution;
	}
	void SetAllLowResolution(bool i_Val)
	{
		// If we have switched a rendering pref, then we need to mark dirty for a new render
		if (l_bAllLowResolution != i_Val)
			gpxRenderControl::SetNeedsNewRender();

		l_bAllLowResolution = i_Val;
	}

	//------------------------------------------------------------------------
	// Set to true to force all render panels to use faster rendering settings
	//------------------------------------------------------------------------
	bool GetAllFastRender()
	{
		return l_bAllFastRender;
	}
	void SetAllFastRender(bool i_Val)
	{
		// If we have switched a rendering pref, then we need to mark dirty for a new render
		if (l_bAllFastRender != i_Val)
			gpxRenderControl::SetNeedsNewRender();

		l_bAllFastRender = i_Val;
	}

	//------------------------------------------------------------------------
	// Setup rendering hints so that animation can optimize
	//------------------------------------------------------------------------
	void SetupRenderingHints()
	{
		g3dRenderingHints::SetAllLowResolution(l_bAllLowResolution);
		g3dRenderingHints::SetAllFastRender(l_bAllFastRender);
	}

}	// end of namespace

