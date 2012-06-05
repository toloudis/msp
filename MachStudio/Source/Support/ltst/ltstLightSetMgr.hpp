/*****************************************************************************
**	ltstLightSetMgr.hpp
**
**	Keeps track of lights and objects that can be grouped so that certain
**	lights affect only certain objects.
**
**	By definition, a light can only belong to one light set. However, an 
**	object can be lit by more than one light set.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef LTST_LIGHTSETMGR_HPP
#error ltstLightSetMgr.hpp multiply included
#endif
#define LTST_LIGHTSETMGR_HPP


#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <vector>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class g3dLight;
class nameObject;
class maFloatRGBA;
class api3dObjectSingle;
class ltstLightSet;
class ltstLightSetsData;
class ltstLightSetObjectData;
class ltstLightSetInterest;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace ltstLightSetMgr
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize();

//--------------------------------------------------------------------
// Systems should call these functions in order to submit
// and organize lights and objects from different systems.
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	//  Add named light to list of things that can be grouped into
	//	lights sets
	//--------------------------------------------------------------------
	void  AddLight(nameObject* i_pNameObj, 
				   g3dLight* i_pLight);

	//--------------------------------------------------------------------
	//	Remove light from manager (removing from light set)
	//--------------------------------------------------------------------
	void RemoveLight(nameObject* i_pNameObj, 
				     g3dLight* i_pLight);

	//--------------------------------------------------------------------
	//  Add named object to list of things that can be lit by light sets
	//--------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
				    api3dObjectSingle* i_pObject);

	//--------------------------------------------------------------------
	//	Remove object from manager (removing from all light sets)
	//--------------------------------------------------------------------
	void RemoveObject(nameObject* i_pNameObj, 
					  api3dObjectSingle* i_pObject);

	//--------------------------------------------------------------------
	//	Change assignments from OldNameObj to NewNameObj (both of which
	//	should be registered with the manager at the point of calling 
	//	this function). This is used when reloading or replacing 
	//	geometry in a system.
	//--------------------------------------------------------------------
	void ReplaceObject(nameObject* i_pOldNameObj, 
					   nameObject* i_pNewNameObj);

	//--------------------------------------------------------------------
	//	Notify the light manager that the name of this light has changed
	//--------------------------------------------------------------------
	void LightRenamed(nameObject* i_pNameObj);

	//--------------------------------------------------------------------
	//	Select the light with the given name in the 3D scene
	//--------------------------------------------------------------------
	void SelectLight(const nameString& i_Name);

	//--------------------------------------------------------------------
	//	Test if name is component that can be added to light sets
	//--------------------------------------------------------------------
	bool  IsLight(const nameString& i_Name);
	bool  IsObject(const nameString& i_Name);

//--------------------------------------------------------------------
// The user interface can then manipulate the groupings with the
// following functions by using the names of the lights and objects
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Returns true if non-empty and unique name for a light set.
	//--------------------------------------------------------------------
	bool IsValidLightSetName(const std::string &i_Name);

	//--------------------------------------------------------------------
	// Test if given name is an existing light set.
	//--------------------------------------------------------------------
	bool IsLightSet(const nameString &i_Name);

	//--------------------------------------------------------------------
	//	Create a named light set
	//--------------------------------------------------------------------
	ltstLightSet* CreateLightSet(const nameString& i_Name);

	//--------------------------------------------------------------------
	//	Destroy named light set, all lights in this set become 
	//	"unassigned"
	//--------------------------------------------------------------------
	void  DeleteLightSet(const nameString& i_Name);

	//--------------------------------------------------------------------
	//	Destroy named light set, using std::string.
	//	Note: Try to use the nameString one instead.
	//--------------------------------------------------------------------
	void  DeleteLightSet(const std::string& i_Name);

	//--------------------------------------------------------------------
	//	Rename a named light set
	//--------------------------------------------------------------------
	void  RenameLightSet(const nameString& i_OldSetName, 
						 const std::string &i_NewName);

	//--------------------------------------------------------------------
	//	Remove all lights and objects from given light set
	//--------------------------------------------------------------------
	void  ClearLightSet(const nameString& i_Name);

	//--------------------------------------------------------------------
	// Remove all light sets (preparing for a new scene)
	//--------------------------------------------------------------------
	void ClearAllLightSets();

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
//	fragment lighting functions
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Return number of nodes within object
	//--------------------------------------------------------------------
	int GetNumFragmentNodeNames(const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	// Get fragment nodes for given object
	//--------------------------------------------------------------------
	void  GetFragmentNodeNames(const nameString& i_ObjectName, 
							   std::vector<std::string>& o_NodeNames);

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
// These functions get the current state of the light set groupings
// in order to be displayed to the user.
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int GetNumLightSets();

	//--------------------------------------------------------------------
	//  Get names of light sets
	//--------------------------------------------------------------------
	void GetLightSetNames(std::vector<nameString> &o_Names);

	//--------------------------------------------------------------------
	//  Get names of lights in given light set. Use empty
	//	i_SetName ("") to ask about "unassigned" lights.
	//--------------------------------------------------------------------
	void GetLightsInSet(const nameString& i_SetName, 
						std::vector<nameString> &o_LightNames);

	//--------------------------------------------------------------------
	//  Get names of objects lit by light set. 
	//--------------------------------------------------------------------
	void GetObjectsInSet(const nameString& i_SetName, 
						 std::vector<nameString> &o_ObjectNames);

	//----------------------------------------------------------------------------
	// Get indices of fragments in this object that are individually lit by this
	// light set. If the whole object is lit, then the list will
	// be returned empty.
	//----------------------------------------------------------------------------
	void GetLitFragmentIndices(const nameString& i_SetName, 
							   const nameString& i_ObjectName,
							   std::vector<int>& o_LitFragmentIndices);

	
	//----------------------------------------------------------------------------
	// GetLightSetInfo about a given object. This is using the
	// ltstLightSetObjectData structure a little differently in that the
	// name field will be the name of the light set.
	//----------------------------------------------------------------------------
	void GetLightSetsForObject( const nameString& i_ObjectName,
							   	std::vector<ltstLightSetObjectData> &o_LightSets );

	//--------------------------------------------------------------------
	//  Get list of all names of objects registered in the manager
	//--------------------------------------------------------------------
	void GetAllObjects(std::vector<nameString> &o_ObjectNames);

	//--------------------------------------------------------------------
	//  Get list of all names of lights registered in the manager
	//--------------------------------------------------------------------
	void GetAllLights(std::vector<nameString> &o_LightNames);

	//--------------------------------------------------------------------
	//	Gets name of light set containing the given light.
	//	Returns true if light is contained in a set and then
	//		sets o_SetName to hold the name of the set.
	//--------------------------------------------------------------------
	bool  GetSetNameFromLight(const nameString& i_LightName, 
							  nameString& o_SetName);

	//--------------------------------------------------------------------
	//	Test if items are in the set
	//--------------------------------------------------------------------
	bool  IsLightInSet(const nameString& i_SetName, 
					   const nameString& i_LightName);
	bool  IsObjectInSet(const nameString& i_SetName, 
							const nameString& i_ObjectName);
	bool  IsWholeObjectInSet(const nameString& i_SetName, 
							const nameString& i_ObjectName);
	bool  IsNodeInSet(const nameString& i_SetName, 
							const nameString& i_ObjectName,
							int i_NodeIndex);


	//--------------------------------------------------------------------
	//  Access to whole data as one structure 
	//--------------------------------------------------------------------
	ltstLightSetsData GetData();
	void SetData(const ltstLightSetsData &i_Data);

	//--------------------------------------------------------------------
	//	Append this data to given light sets, merging into existing
	//	sets of the same name
	//
	//	If i_bRenameDupes is true, the duplicate entry will be renamed
	//	and merged.  If it is false, the duplicate will NOT be merged.
	//--------------------------------------------------------------------
	void MergeData(const ltstLightSetsData &i_Data, bool i_bRenameDupes = false);

//--------------------------------------------------------------------
//	data changed interest functions
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	//	RegisterLightSetInterest() - add a LightSet interest 
	//--------------------------------------------------------------------
	void RegisterLightSetInterest( ltstLightSetInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterLightSetInterest() - remove a LightSet interest 
	//
	//	Note: this will NOT delete the LightSet interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterLightSetInterest( ltstLightSetInterest* i_pInterest );

}	// end of namespace
