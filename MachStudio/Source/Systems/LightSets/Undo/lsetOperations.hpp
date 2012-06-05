/*****************************************************************************
**	lsetOperations.hpp
**
**	Utility for operations that are undoable in lset system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef LSET_OPERATIONS_HPP
#error lsetOperations.hpp multiply included
#endif
#define LSET_OPERATIONS_HPP

class api3dObjectSingle;
class lsetScriptData;
class lsetScriptObject;
class ltstLightSetData;
class nameObject;
class nameString;
class itString;

#include <map>
#include <vector>

namespace lsetOperations
{
	//--------------------------------------------------------------------
	//  Add new light set to world
	//--------------------------------------------------------------------
	void  AddObject();
	void  AddObject(const lsetScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Select light set with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Delete point light with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//	Add named light to named light set.
	//--------------------------------------------------------------------
	void  AddLightToSet(const nameString& i_SetName, 
						const nameString& i_LightName);

	//--------------------------------------------------------------------
	//	Remove named light from named light set.
	//--------------------------------------------------------------------
	void  RemoveLightFromSet(const nameString& i_SetName, 
							 const nameString& i_LightName);


	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddObjectToLightSet(const nameString& i_SetName, 
							  const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveObjectFromLightSet(const nameString& i_SetName, 
								   const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Add node to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddNodeToLightSet(const nameString& i_SetName, 
							const nameString& i_ObjectName,
							int i_NodeIndex);

	//--------------------------------------------------------------------
	//	Remove node from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveNodeFromLightSet(const nameString& i_SetName, 
								 const nameString& i_ObjectName,
								 int i_NodeIndex);

	//--------------------------------------------------------------------
	//  Create light set from selection
	//--------------------------------------------------------------------
	void  CreateLightSetFromSelection();
	
	//--------------------------------------------------------------------
	//  Create light set from selection
	//--------------------------------------------------------------------
	void  AddSelectionToLightSet(int i_LightSetIndex);
	
	//--------------------------------------------------------------------
	//  Create light set from selection
	//--------------------------------------------------------------------
	void  RemoveSelectionFromLightSet(int i_LightSetIndex);

	//--------------------------------------------------------------------
	//  Remove selected lights from their light sets and add to
	//  unassigned set
	//--------------------------------------------------------------------
	void  RemoveSelectedLightsFromLightSets();
	
	//--------------------------------------------------------------------
	// Restore all lights to enabled state
	//--------------------------------------------------------------------
	void ClearIsolatedLights();

	//--------------------------------------------------------------------
	// Fade all lights to black that are not in selection.
	//--------------------------------------------------------------------
	void IsolateSelectedLights();

	//--------------------------------------------------------------------
	// Restore "active in render layer" flags so that there is no more
	// geometry "hidden" isolation.
	//--------------------------------------------------------------------
	void ClearHidden();

	//--------------------------------------------------------------------
	// Set "active in render layer" flags such that only the selected
	// objects are visible.
	//--------------------------------------------------------------------
	void HideGeometryExceptSelection();

	//--------------------------------------------------------------------
	// Hide geometry based on a boolean flag per object name and
	// an optional map of lit surface ids per object name.
	//--------------------------------------------------------------------
	void HideGeometry(const std::map<nameString, bool>& i_ActiveFlags,
					  const std::map<nameString, std::vector<int> >& i_SurfaceSettings);

	//--------------------------------------------------------------------
	// Isolate the lights that influence the selected geometry.
	//--------------------------------------------------------------------
	void IsolateInfluences();

}	// end of namespace
