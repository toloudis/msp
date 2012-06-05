/****************************************************************************\
**	rpnRenderingPrefs.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/rpnRenderingPrefs.hpp"

#include "Graphics/g3d/g3dRenderingHints.hpp"

namespace rpnRenderingPrefs
{
	namespace
	{
		bool l_bAllLowResolution = false;
	}

	//------------------------------------------------------------------------
	//	Set all flags to their default state
	//------------------------------------------------------------------------
	void RestoreDefaults()
	{
		l_bAllLowResolution = false;
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
		l_bAllLowResolution = i_Val;
	}

	//------------------------------------------------------------------------
	// Setup rendering hints so that animation can optimize
	//------------------------------------------------------------------------
	void SetupRenderingHints()
	{
		g3dRenderingHints::SetAllLowResolution(l_bAllLowResolution);
	}

}	// end of namespace

