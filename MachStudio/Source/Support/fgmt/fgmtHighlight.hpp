/*****************************************************************************
**	fgmtHighlight.hpp
**
**	Utility for highlighting selected surfaces
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FGMT_HIGHLIGHT_HPP
#error fgmtHighlight.hpp multiply included
#endif
#define FGMT_HIGHLIGHT_HPP


//============================================================================
//============================================================================
class fgmtScriptObject;
class matMaterial;

//============================================================================
//============================================================================
namespace fgmtHighlight
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize();

	//--------------------------------------------------------------------
	// Access to animated material used in highlighting
	//--------------------------------------------------------------------
	matMaterial* GetHighlightMaterial();

	//--------------------------------------------------------------------
	// Change highlight state on the given fragment
	//--------------------------------------------------------------------
	void  HighlightFragmentIndex(fgmtScriptObject* i_pObject, int i_Index, bool i_bOnOff);

	//--------------------------------------------------------------------
	//	Toggle hilighting of current fragment.
	//--------------------------------------------------------------------
	void HighlightFragment(bool i_On);
	bool IsHighlightFragment();

	//--------------------------------------------------------------------
	// Start a flash of the material highlighting for a short period 
	// of time. Call this when a material is selected.
	//--------------------------------------------------------------------
	void BeginHighlightFlashAnimation();

	//--------------------------------------------------------------------
	// Check to see if any surfaces have highlight animations
	//--------------------------------------------------------------------
	bool AreSurfacesHighlighted();

}	// end of namespace
