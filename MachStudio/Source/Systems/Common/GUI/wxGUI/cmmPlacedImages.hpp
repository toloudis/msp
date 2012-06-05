/****************************************************************************\
**	cmmPlacedImages.hpp
**
**		This class converts the cmmDialogDataList for the placed objects
**	into a tree structure of cmmSceneTreeNodes in order to display
**	the scene manager object grouped by category.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_PLACEDIMAGES_HPP
#error cmmPlacedImages.hpp multiply included
#endif
#define CMM_PLACEDIMAGES_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif 


//============================================================================
//============================================================================
namespace cmmPlacedImages 
{
	//--------------------------------------------------------------------
	// enumeration for the icons in the placed tree control
	//--------------------------------------------------------------------
	enum PlacedImageState
	{
		e_NoCheckbox = 0,
		e_Unchecked = 1,
		e_Checked = 2,
		e_MixChecked = 3,
		e_ControlPart,
		e_FirstPartIndex = e_ControlPart,
		//e_ExpressionPart,
		e_MaterialPart,
		e_SurfacePart,
		e_NumPlacedImageStates
	};

	enum SystemImageIcons
	{
		e_NoImage = -1,
		e_Parent = 0,
		e_Geometry,
		e_Light,
		e_Camera,
		e_NumSystemImageIcons
	};
}

