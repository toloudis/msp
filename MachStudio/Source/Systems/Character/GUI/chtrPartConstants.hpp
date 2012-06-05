/*****************************************************************************
**  chtrPartConstants.hpp
**
**      chtrPartConstants defines strings for category names for parts.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_PARTCONSTANTS_HPP
#error chtrPartConstants.hpp multiply included
#endif
#define CHTR_PARTCONSTANTS_HPP


//============================================================================
//============================================================================
namespace chtrPartConstants
{
	//------------------------------------------------------------------------
	// Materials
	//------------------------------------------------------------------------
	static const char *c_MaterialCategoryName = "Materials";
	static const char *c_MaterialOverrideString = "Override Materials";

	//------------------------------------------------------------------------
	// Surfaces
	//------------------------------------------------------------------------
	static const char *c_SurfaceCategoryName = "Surfaces";
	static const char *c_SurfaceOverrideString = "Override Surface Flags";

	//------------------------------------------------------------------------
	// Controls
	//------------------------------------------------------------------------
	static const char *c_ControlCategoryName = "Controls";
	// space at beginning keeps this operation at the top of the list
	static const char *c_NewControlString = " Add Joint Control"; 

	//------------------------------------------------------------------------
	// Expressions
	//------------------------------------------------------------------------
	static const char *c_ExpressionCategoryName = "Expressions";
	// space at beginning keeps this operation at the top of the list
	static const char *c_NewExpressionSingleString = " Add Single Expression";
	static const char *c_NewExpressionDualString = " Add Dual Expression";
	static const char *c_NewExpressionQuadString = " Add Quad Expression";
}
