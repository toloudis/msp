/*****************************************************************************
**	lyerLayerMgr.hpp
**
**	Keeps track of groups of objects that can then have active and
**	draw style state altered as a group.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef LYER_LAYERMGR_HPP
#error lyerLayerMgr.hpp multiply included
#endif
#define LYER_LAYERMGR_HPP


#ifndef LYER_LAYERSTYLE_HPP
#include "Support/lyer/lyerLayerStyle.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class g3dLight;
class nameObject;
class maFloatRGBA;
class lyerObject;
class lyerLayersData;
class lyerLayerInterest;

//============================================================================
// Handle to refer to layer
//============================================================================
typedef void* lyerLayerHandle;

//============================================================================
//============================================================================
namespace lyerLayerMgr
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize();


//
// Systems should call these functions in order to submit
// and organize objects from different systems.
//

	//--------------------------------------------------------------------
	//  Add named object to list of things that can be grouped
	//--------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
				    lyerObject* i_pObject);

	//--------------------------------------------------------------------
	//	Remove object from manager (removing from all layers)
	//--------------------------------------------------------------------
	void RemoveObject(nameObject* i_pNameObj, 
					  lyerObject* i_pObject);

	//--------------------------------------------------------------------
	//	Change assignments from OldNameObj to NewNameObj (both of which
	//	should be registered with the manager at the point of calling 
	//	this function). This is used when reloading or replacing 
	//	geometry in a system.
	//--------------------------------------------------------------------
	void ReplaceObject(nameObject* i_pOldNameObj, 
					   nameObject* i_pNewNameObj);

	//--------------------------------------------------------------------
	//	Notify the layer manager that the name of this object has changed
	//--------------------------------------------------------------------
	void ObjectRenamed(nameObject* i_pNameObj);


//
// The user interface can then manipulate the groupings with the
// following functions by using the names of the layers and objects
//

	//--------------------------------------------------------------------
	// Returns true if non-empty and unique name for a layer.
	//--------------------------------------------------------------------
	bool IsValidLayerName(const std::string &i_Name);

	//--------------------------------------------------------------------
	//	Create a named layer
	//--------------------------------------------------------------------
	lyerLayerHandle  CreateLayer(const nameString& i_Name);

	//--------------------------------------------------------------------
	//	Destroy named layer, all objects in this set become 
	//	"unassigned"
	//--------------------------------------------------------------------
	void  DeleteLayer(const nameString& i_Name);

	//--------------------------------------------------------------------
	//	Destroy named layer, using std::string
	//	Note: Try to use the nameString function.
	//--------------------------------------------------------------------
	void  DeleteLayer(const std::string& i_Name);

	//--------------------------------------------------------------------
	//	Rename a layer
	//--------------------------------------------------------------------
	void  RenameLayer(const nameString& i_OldName, const nameString& i_NewName);

	//--------------------------------------------------------------------
	// Remove all layers (preparing for a new scene)
	//--------------------------------------------------------------------
	void ClearAllLayers();

	//--------------------------------------------------------------------
	//	Remove all objects from the given layer
	//--------------------------------------------------------------------
	void  ClearLayer(const nameString& i_LayerName);

	//--------------------------------------------------------------------
	//	Add object to the given layer
	//--------------------------------------------------------------------
	void  AddObjectToLayer(const nameString& i_LayerName, 
							  const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Remove object from the given layer
	//--------------------------------------------------------------------
	void  RemoveObjectFromLayer(const nameString& i_LayerName, 
								   const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	// Layer style gives one enumeration that controls visible,
	//	wireframe, pickable settings.
	//--------------------------------------------------------------------
	//void SetLayerStyle(const nameString& i_LayerName, 
	//				   lyerLayerStyle i_Style);
	//lyerLayerStyle GetLayerStyle(const nameString& i_LayerName);

	//--------------------------------------------------------------------
	// Separate flags for attributes of layer
	//--------------------------------------------------------------------
	void SetLayerVisible(const nameString& i_LayerName, bool i_bVisible);
	bool GetLayerVisible(const nameString& i_LayerName);
	void SetLayerPickable(const nameString& i_LayerName, bool i_bPickable);
	bool GetLayerPickable(const nameString& i_LayerName);
	void SetLayerWireframe(const nameString& i_LayerName, bool i_bWireframe);
	bool GetLayerWireframe(const nameString& i_LayerName);
	void SetLayerLowRes(const nameString& i_LayerName, bool i_bLowRes);
	bool GetLayerLowRes(const nameString& i_LayerName);


	//--------------------------------------------------------------------
	// One step function to set all layers visible and to normal state
	//--------------------------------------------------------------------
	void AllLayersVisible();

	//--------------------------------------------------------------------
	//  Convert from handle to name
	//--------------------------------------------------------------------
	const nameString& GetLayerName(lyerLayerHandle i_Handle);
	void SetLayerName(lyerLayerHandle i_Handle, const nameString& i_Name);

//--------------------------------------------------------------------
// These functions get the current state of the layer groupings
// in order to be displayed to the user.
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Return number of layers
	//--------------------------------------------------------------------
	int GetNumLayers();

	//--------------------------------------------------------------------
	//  Get names of layers
	//--------------------------------------------------------------------
	void GetLayerNames(std::vector<nameString> &o_Names);

	//--------------------------------------------------------------------
	//  Get names of objects in a layer. Use empty
	//	i_LayerName ("") to ask about "unassigned" objects.
	//--------------------------------------------------------------------
	void GetObjectsInLayer(const nameString& i_LayerName, 
						 std::vector<nameString> &o_ObjectNames);

	//--------------------------------------------------------------------
	//  Get list of all names of objects registered in the manager
	//--------------------------------------------------------------------
	void GetAllObjects(std::vector<nameString> &o_ObjectNames);

	//--------------------------------------------------------------------
	//	Gets name of layer containing the given object.
	//	Returns true if object is contained in a layer and then
	//		sets o_LayerName to hold the name of the layer.
	//--------------------------------------------------------------------
	bool  GetLayerNameFromObject(const nameString& i_ObjectName, 
							     nameString& o_LayerName);

	//--------------------------------------------------------------------
	//  Access to whole data as one structure 
	//--------------------------------------------------------------------
	lyerLayersData GetData();
	void SetData(const lyerLayersData &i_Data);

	//--------------------------------------------------------------------
	//  Adds given data to exisiting data in manager
	//--------------------------------------------------------------------
	void MergeData(const lyerLayersData &i_Data);

	
//--------------------------------------------------------------------
//	data changed interest functions
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	//	RegisterLayerInterest() - add a Layer interest 
	//--------------------------------------------------------------------
	void RegisterLayerInterest( lyerLayerInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterLayerInterest() - remove a Layer interest 
	//
	//	Note: this will NOT delete the Layer interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterLayerInterest( lyerLayerInterest* i_pInterest );

}	// end of namespace
