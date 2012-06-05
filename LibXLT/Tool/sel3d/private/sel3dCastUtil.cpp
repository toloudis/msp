/****************************************************************************\
**	sel3dCastUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/sel3d/sel3dCastUtil.hpp"


//============================================================================
//============================================================================
namespace sel3dCastUtil
{
	//--------------------------------------------------------------------
	// Convert the results of a GPU pick to a selectable object. This
	// may return NULL if the picked object is not of a selectable type.
	//--------------------------------------------------------------------
	sel3dObject* ConvertPickToSelection(pick3dPickObject *i_pPickObject)
	{
		if (!i_pPickObject) return NULL;
		return dynamic_cast<sel3dObject*>(i_pPickObject);
	}
}
