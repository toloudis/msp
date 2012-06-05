/*****************************************************************************
**	tmlnSelectionUtil.hpp
**
**	Utility for finding the selected script objects.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_SELECTIONUTIL_HPP
#error tmlnSelectionUtil.hpp multiply included
#endif
#define TMLN_SELECTIONUTIL_HPP

#include <vector>

class pick3dPickObject;
class tmlnScriptObject;

namespace tmlnSelectionUtil
{
	//--------------------------------------------------------------------
	//  Get the script object from the "top" selected object (the most
	//		recent selected object is more than one is selected).
	//  Will return NULL if no selection or if the selection does not
	//		contain a script object.
	//--------------------------------------------------------------------
	tmlnScriptObject*  GetSelectedScriptObject();

	//--------------------------------------------------------------------
	//  Get script objects that are in the selected list
	//--------------------------------------------------------------------
	void  GetSelectedScriptObjects(std::vector<tmlnScriptObject*> &o_Objects);

	//--------------------------------------------------------------------
	//  Get the script object from the given pick object
	//--------------------------------------------------------------------
	tmlnScriptObject*  GetScriptObject(pick3dPickObject* i_pObj);

}	// end of namespace
