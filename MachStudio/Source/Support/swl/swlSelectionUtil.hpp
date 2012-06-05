/*****************************************************************************
**	swlSelectionUtil.hpp
**
**	Utility for finding the selected script objects.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef SWL_SELECTIONUTIL_HPP
#error swlSelectionUtil.hpp multiply included
#endif
#define SWL_SELECTIONUTIL_HPP

#include <vector>

class sel3dObject;
class swlScriptObject;

namespace swlSelectionUtil
{
	//--------------------------------------------------------------------
	//  Get the script object from the "top" selected object (the most
	//		recent selected object is more than one is selected).
	//  Will return NULL if no selection or if the selection does not
	//		contain a script object.
	//--------------------------------------------------------------------
	swlScriptObject*  GetSelectedScriptObject();

	//--------------------------------------------------------------------
	//  Get script objects that are in the selected list
	//--------------------------------------------------------------------
	void  GetSelectedScriptObjects(std::vector<swlScriptObject*> &o_Objects);

	int GetSelectedScriptObjectsCount();

	//--------------------------------------------------------------------
	//  Get the script object from the given pick object
	//--------------------------------------------------------------------
	swlScriptObject*  GetScriptObject(sel3dObject* i_pObj);

}	// end of namespace
