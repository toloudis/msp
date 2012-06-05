/*****************************************************************************
**	cmmSceneOperations.hpp
**
**	Operations on scene objects like selection and deletion
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SCENEOPERATIONS_HPP
#error cmmSceneOperations.hpp multiply included
#endif
#define CMM_SCENEOPERATIONS_HPP

#include <list>
#include <map>
#include <string>

class sel3dObject;
class nameString;

//============================================================================
//============================================================================
namespace cmmSceneOperations
{
	//--------------------------------------------------------------------
	// Make copies of selected objects 
	//--------------------------------------------------------------------
	void DuplicateSelected();

	//--------------------------------------------------------------------
	// Make copy of given named object and return the name of 
	// the new copy.
	//--------------------------------------------------------------------
	nameString DuplicateObject(const nameString& i_ObjectName,
							   std::map<nameString, nameString> &o_DuplicateNameMap);

	//--------------------------------------------------------------------
	// Change internal name attachments for given object based on name map
	//--------------------------------------------------------------------
	void RemapNames(const nameString& i_ObjectName,
					const std::map<nameString, nameString> &i_DuplicateNameMap);

	//--------------------------------------------------------------------
	// Reload selected object 
	//--------------------------------------------------------------------
	void ReloadSelected();

	//--------------------------------------------------------------------
	// Delete selected objects 
	//--------------------------------------------------------------------
	void DeleteSelected();

	//--------------------------------------------------------------------
	// Set visibility for entire system at once 
	//--------------------------------------------------------------------
	void SetSystemVisibility(const std::string &i_SystemName, bool i_bVisible);

	//--------------------------------------------------------------------
	// Select the objects in the list, clearing all previous selections
	//--------------------------------------------------------------------
	void Select(const std::list<sel3dObject*>& i_Objects );
	
	//--------------------------------------------------------------------
	// Append objects in the list to the selection
	//--------------------------------------------------------------------
	void AddToSelection(const std::list<sel3dObject*>& i_Objects );

	//--------------------------------------------------------------------
	// Remove objects in the list from the selection
	//--------------------------------------------------------------------
	void RemoveFromSelection(const std::list<sel3dObject*>& i_Objects );
}	// end of namespace
