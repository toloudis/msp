/*****************************************************************************
**	mtrlHighlight.hpp
**
**	Utility for highlighting selected materials
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef MTRL_HIGHLIGHT_HPP
#error mtrlHighlight.hpp multiply included
#endif
#define MTRL_HIGHLIGHT_HPP


//============================================================================
//============================================================================
class mtrlScriptObject;
class matMaterial;

//============================================================================
//============================================================================
namespace mtrlHighlight
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
	// Change highlight state on the given material
	//--------------------------------------------------------------------
	void  HighlightMaterialIndex(mtrlScriptObject* i_pObject, int i_Index, bool i_bOnOff);

	//--------------------------------------------------------------------
	//	Toggle hilighting of current material.
	//--------------------------------------------------------------------
	void HighlightMaterial(bool i_On);
	bool IsHighlightMaterial();

	//--------------------------------------------------------------------
	// Start a flash of the material highlighting for a short period 
	// of time. Call this when a material is selected.
	//--------------------------------------------------------------------
	void BeginHighlightFlashAnimation();

	//--------------------------------------------------------------------
	// Check to see if any materials have highlight animations
	//--------------------------------------------------------------------
	bool AreMaterialsHighlighted();

}	// end of namespace
