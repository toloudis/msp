/*****************************************************************************
**  rpnRenderingPrefs.hpp
**
**      controls a "fast" render mode for all render panels
**
**	Note: the current implementation is designed so that the render
**	panels check these flags before rendering. But, a future implementation
**	may need to set and check flags on the render panels from here
**	instead of the other way around. This will have to be rethought when
**	each render panel has its own rendering preferences.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_RENDERINGPREFS_HPP
#error rpnRenderingPrefs.hpp multiply included
#endif
#define RPN_RENDERINGPREFS_HPP


namespace rpnRenderingPrefs
{
	//------------------------------------------------------------------------
	//	Set all flags to their default state
	//------------------------------------------------------------------------
	void RestoreDefaults();

	//------------------------------------------------------------------------
	// Set to true to force all render panels to use low resolution
	//------------------------------------------------------------------------
	bool GetAllLowResolution();
	void SetAllLowResolution(bool i_Val);

	//------------------------------------------------------------------------
	// Set to true to force all render panels to use faster rendering settings
	//------------------------------------------------------------------------
	bool GetAllFastRender();
	void SetAllFastRender(bool i_Val);

	//------------------------------------------------------------------------
	// Setup rendering hints so that animation can optimize
	//------------------------------------------------------------------------
	void SetupRenderingHints();
};
