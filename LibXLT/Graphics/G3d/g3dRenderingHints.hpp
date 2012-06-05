/****************************************************************************\
**	g3dRenderingHints.hpp
**
**		The g3dRenderingHints communicate patterns in multiple panel
**	rendering that can be used to speed animation.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_RENDERINGHINTS_HPP
#error g3dRenderingHints.hpp multiply included
#endif
#define G3D_RENDERINGHINTS_HPP


//============================================================================
//============================================================================
namespace g3dRenderingHints
{
	//------------------------------------------------------------------------
	//	Set all flags to their default state
	//------------------------------------------------------------------------
	void RestoreDefaults();

	//------------------------------------------------------------------------
	// Set to true if all renderings will be in low-resolution, 
	//	allows models to skip high-resolution animation.
	//------------------------------------------------------------------------
	bool GetAllLowResolution();
	void SetAllLowResolution(bool i_Val);

	//------------------------------------------------------------------------
	// Set to true if all renderings will be in fast mode (multipass off, 
	//	no reflections, etc)
	//------------------------------------------------------------------------
	bool GetAllFastRender();
	void SetAllFastRender(bool i_Val);
}

