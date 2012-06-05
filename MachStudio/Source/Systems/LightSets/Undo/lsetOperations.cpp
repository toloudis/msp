/*****************************************************************************
**	lsetOperations.cpp
**
**	Utility for operations that are undoable in lset system
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Undo/lsetOperations.hpp"

#include "Support/fgmt/fgmtScriptObject.hpp"
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "Support/ltst/ltstIsolateMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/ltst/ltstInfluenceUtil.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Systems/Common/Gui/cmmSystemDialogUtil.hpp"
#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Systems/LightSets/Object/lsetObjectMgr.hpp"
#include "Systems/LightSets/Undo/lsetActualOperations.hpp"
#include "Systems/LightSets/Undo/lsetLightOperation.hpp"
#include "Systems/LightSets/Undo/lsetNodeOperation.hpp"
#include "Systems/LightSets/Undo/lsetObjectOperation.hpp"

#include "Core/Env/envSTLHelpers.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"


//============================================================================
//============================================================================
namespace lsetOperations
{
	namespace
	{
		const char *c_AddOperationDisplayName = "Add LightSet";
		typedef cmmAddOperationTemplate< lsetScriptData, lsetActualOperations> lsetAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete LightSet";
		typedef cmmDeleteOperationTemplate< lsetScriptData, lsetActualOperations> lsetDeleteOperation;

		struct sNodeInfo
		{
			nameString m_ObjectName;
			int m_FragmentIndex;
		};

		//--------------------------------------------------------------------
		// utility function for finding the interesting things in the
		// selection from a light set point of view
		//--------------------------------------------------------------------
		void get_selected_lightset_components(std::vector<nameString> &o_SelectedObjects, 
											  std::vector<sNodeInfo> &o_SelectedNodes, 
											  std::vector<nameString> &o_SelectedLights)
		{
			std::vector<nameString> objectNames, lightNames;
			ltstLightSetMgr::GetAllObjects(objectNames);
			ltstLightSetMgr::GetAllLights(lightNames);

			const std::list<sel3dObject*>& selected_list = sel3dMgr::GetSelectedList();
			std::list<sel3dObject*>::const_iterator sit;
			for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
			{
				if ( nameObject *pNamedObject = sel3dCastUtil::CastPickObject<nameObject>(*sit) )
				{
					nameString item_name = pNamedObject->GetName();
					//DBG_LOG("Found named object: " << item_name.GetString());
					if (envSTLHelpers::Contains(objectNames, item_name))
					{
						if ( fgmtPropertyObject *pPropObj = dynamic_cast<fgmtPropertyObject*>(*sit) )
						{
							std::string surface_name = pPropObj->GetName();
							//DBG_LOG("  is fragment named " << surface_name);

							std::vector<std::string> node_names;
							ltstLightSetMgr::GetFragmentNodeNames(item_name, node_names);
							for (int i=0; i<node_names.size(); ++i)
							{
								if (node_names[i] == surface_name)
								{
									DBG_LOG("Found fragment named " << surface_name << " has index " << i << " in object " << item_name.GetString());
									sNodeInfo node_info = { item_name, i };
									o_SelectedNodes.push_back( node_info );
									break;
								}
							}
						}
						else
						{
							//DBG_LOG("  is an object");
							o_SelectedObjects.push_back( item_name );
						}
					}					
					else if (envSTLHelpers::Contains(lightNames, item_name))
					{
						//DBG_LOG("  is a light");
						o_SelectedLights.push_back( item_name );
					}
				}
			}
		}

	}	// end of namespace


//============================================================================
//============================================================================

	//--------------------------------------------------------------------
	//  Add new light set
	//--------------------------------------------------------------------
	void  AddObject()
	{
		lsetScriptData default_data;
		AddObject(default_data);
	}
	void  AddObject(const lsetScriptData& i_Data)
	{
		int index = lsetActualOperations::AddObject(i_Data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		//char displaytext[128];
		//sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Data.m_BaseData.m_Name.GetString().c_str());
		std::ostringstream oss;
		oss << c_AddOperationDisplayName <<" - "<<i_Data.m_BaseData.m_Name.GetString();
		std::string displaytext(oss.str());
		
		// Need new name given to new object when making udo operation
		undoUndoMgr::AddOperation(new lsetAddOperation(index, lsetObjectMgr::GetScriptData(index), displaytext.c_str()));
	}

	//--------------------------------------------------------------------
	//  Select light set with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		sel3dMgr::CreateUndoOperation();
		lsetObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = lsetObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			lsetObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = lsetObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			lsetObjectMgr::DeselectObject(index);
		}
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		//char displaytext[128];
		//sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, lsetObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		std::ostringstream oss;
		oss << c_DeleteOperationDisplayName <<" - "<<lsetObjectMgr::GetBaseData(i_Index).m_Name.GetString();
		std::string displaytext(oss.str());
		
		undoUndoMgr::AddOperation(new lsetDeleteOperation(i_Index, lsetObjectMgr::GetScriptData(i_Index), displaytext.c_str()));
		lsetActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//	Add named light to named light set.
	//--------------------------------------------------------------------
	void  AddLightToSet(const nameString& i_SetName, 
						const nameString& i_LightName)
	{
		undoUndoMgr::AddOperation(new lsetAddLightOperation(i_LightName));
		lsetActualOperations::AddLightToSet(i_SetName, i_LightName);
	}

	//--------------------------------------------------------------------
	//	Remove named light from named light set.
	//--------------------------------------------------------------------
	void  RemoveLightFromSet(const nameString& i_SetName, 
							 const nameString& i_LightName)
	{
		undoUndoMgr::AddOperation(new lsetRemoveLightOperation(i_LightName));
		lsetActualOperations::RemoveLightFromSet(i_SetName, i_LightName);
	}


	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddObjectToLightSet(const nameString& i_SetName, 
							  const nameString& i_ObjectName)
	{
		undoUndoMgr::AddOperation(new lsetAddObjectOperation(i_SetName, i_ObjectName));
		lsetActualOperations::AddObjectToLightSet(i_SetName, i_ObjectName);
	}

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveObjectFromLightSet(const nameString& i_SetName, 
								   const nameString& i_ObjectName)
	{
		undoUndoMgr::AddOperation(new lsetRemoveObjectOperation(i_SetName, i_ObjectName));
		lsetActualOperations::RemoveObjectFromLightSet(i_SetName, i_ObjectName);
	}

	//--------------------------------------------------------------------
	//	Add node to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddNodeToLightSet(const nameString& i_SetName, 
							const nameString& i_ObjectName,
							int i_NodeIndex)
	{
		undoUndoMgr::AddOperation(new lsetAddNodeOperation(i_SetName, i_ObjectName, i_NodeIndex));
		lsetActualOperations::AddNodeToLightSet(i_SetName, i_ObjectName, i_NodeIndex);
	}

	//--------------------------------------------------------------------
	//	Remove node from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveNodeFromLightSet(const nameString& i_SetName, 
								 const nameString& i_ObjectName,
								 int i_NodeIndex)
	{
		undoUndoMgr::AddOperation(new lsetRemoveNodeOperation(i_SetName, i_ObjectName, i_NodeIndex));
		lsetActualOperations::RemoveNodeFromLightSet(i_SetName, i_ObjectName, i_NodeIndex);
	}

	//--------------------------------------------------------------------
	//  Create light set from selection
	//--------------------------------------------------------------------
	void  CreateLightSetFromSelection()
	{
		if (sel3dMgr::GetSelected() == NULL)
			return;

		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();
			
		std::vector<nameString> selectedObjects, selectedLights;
		std::vector<sNodeInfo> selectedNodes;
		get_selected_lightset_components(selectedObjects, selectedNodes, selectedLights);

		undoUndoMgr::BeginMultipleOperationBlock("Create Light Set from Selection");

		// Clear the selection as part of the multiple undo block
		// so that the old selection can be restored when undoing the operation.
		sel3dMgr::CreateUndoOperation();
		sel3dMgr::ClearSelection();

		AddObject();

		// Get index of just created light set
		int index = lsetObjectMgr::GetNumObjects()-1;
		if (index >= 0)
		{
			nameString light_set = lsetObjectMgr::GetName(index);

			std::vector<nameString>::const_iterator nit;
			for (nit = selectedLights.begin(); nit != selectedLights.end(); ++nit)
			{
				AddLightToSet(light_set, *nit);
			}
			for (nit = selectedObjects.begin(); nit != selectedObjects.end(); ++nit)
			{
				AddObjectToLightSet(light_set, *nit);
			}
			std::vector<sNodeInfo>::const_iterator fit;
			for (fit = selectedNodes.begin(); fit != selectedNodes.end(); ++fit)
			{
				AddNodeToLightSet(light_set, fit->m_ObjectName, fit->m_FragmentIndex);
			}
		}
		
		undoUndoMgr::EndMultipleOperationBlock();

		// Need to update user interface because we added objects to it after selecting the set when creating it.
		sel3dMgr::Renotify();
	}
	
	//--------------------------------------------------------------------
	//  Create light set from selection
	//--------------------------------------------------------------------
	void  AddSelectionToLightSet(int i_LightSetIndex)
	{
		if (sel3dMgr::GetSelected() == NULL)
			return;
			
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		std::vector<nameString> selectedObjects, selectedLights;
		std::vector<sNodeInfo> selectedNodes;
		get_selected_lightset_components(selectedObjects, selectedNodes, selectedLights);

		undoUndoMgr::BeginMultipleOperationBlock();

		nameString light_set = lsetObjectMgr::GetName(i_LightSetIndex);

		std::vector<nameString>::const_iterator nit;
		for (nit = selectedLights.begin(); nit != selectedLights.end(); ++nit)
		{
			if (!ltstLightSetMgr::IsLightInSet(light_set, *nit))
				AddLightToSet(light_set, *nit);
		}
		for (nit = selectedObjects.begin(); nit != selectedObjects.end(); ++nit)
		{
			if (!ltstLightSetMgr::IsWholeObjectInSet(light_set, *nit))
				AddObjectToLightSet(light_set, *nit);
		}
		std::vector<sNodeInfo>::const_iterator fit;
		for (fit = selectedNodes.begin(); fit != selectedNodes.end(); ++fit)
		{
			if (!ltstLightSetMgr::IsNodeInSet(light_set, fit->m_ObjectName, fit->m_FragmentIndex))
				AddNodeToLightSet(light_set, fit->m_ObjectName, fit->m_FragmentIndex);
		}
		
		undoUndoMgr::EndMultipleOperationBlock();

		// Need to update user interface because we added objects to it after selecting the set when creating it.
		sel3dMgr::Renotify();

	}
	
	//--------------------------------------------------------------------
	//  Create light set from selection
	//--------------------------------------------------------------------
	void  RemoveSelectionFromLightSet(int i_LightSetIndex)
	{
		if (sel3dMgr::GetSelected() == NULL)
			return;
			
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		std::vector<nameString> selectedObjects, selectedLights;
		std::vector<sNodeInfo> selectedNodes;
		get_selected_lightset_components(selectedObjects, selectedNodes, selectedLights);

		undoUndoMgr::BeginMultipleOperationBlock();

		nameString light_set = lsetObjectMgr::GetName(i_LightSetIndex);

		std::vector<nameString>::const_iterator nit;
		for (nit = selectedLights.begin(); nit != selectedLights.end(); ++nit)
		{
			if (ltstLightSetMgr::IsLightInSet(light_set, *nit))
				RemoveLightFromSet(light_set, *nit);
		}
		for (nit = selectedObjects.begin(); nit != selectedObjects.end(); ++nit)
		{
			if (ltstLightSetMgr::IsObjectInSet(light_set, *nit))
				RemoveObjectFromLightSet(light_set, *nit);
		}
		std::vector<sNodeInfo>::const_iterator fit;
		for (fit = selectedNodes.begin(); fit != selectedNodes.end(); ++fit)
		{
			if (ltstLightSetMgr::IsNodeInSet(light_set, fit->m_ObjectName, fit->m_FragmentIndex))
				RemoveNodeFromLightSet(light_set, fit->m_ObjectName, fit->m_FragmentIndex);
		}
		
		undoUndoMgr::EndMultipleOperationBlock();

		// Need to update user interface because we added objects to it after selecting the set when creating it.
		sel3dMgr::Renotify();

	}

	//--------------------------------------------------------------------
	//  Remove selected lights from their light sets and add to
	//  unassigned set
	//--------------------------------------------------------------------
	void  RemoveSelectedLightsFromLightSets()
	{
		if (sel3dMgr::GetSelected() == NULL)
			return;
			
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		std::vector<nameString> selectedObjects, selectedLights;
		std::vector<sNodeInfo> selectedNodes;
		get_selected_lightset_components(selectedObjects, selectedNodes, selectedLights);

		undoUndoMgr::BeginMultipleOperationBlock();

		std::vector<nameString>::const_iterator nit;
		for (nit = selectedLights.begin(); nit != selectedLights.end(); ++nit)
		{
			nameString light_set;
			if (ltstLightSetMgr::GetSetNameFromLight(*nit, light_set))
				RemoveLightFromSet(light_set, *nit);
		}

		undoUndoMgr::EndMultipleOperationBlock();

		// Need to update user interface because we added objects to it after selecting the set when creating it.
		sel3dMgr::Renotify();
	}

	//--------------------------------------------------------------------
	// Restore all lights to enabled state
	//--------------------------------------------------------------------
	void ClearIsolatedLights()
	{
		ltstIsolateMgr::ClearIsolation();

		// Update check boxes for lighting
		cmmSystemDialogUtil::UpdateSetRelationships();
	}

	//--------------------------------------------------------------------
	// Fade all lights to black that are not in selection.
	//--------------------------------------------------------------------
	void IsolateSelectedLights()
	{
		if (sel3dMgr::GetSelected() == NULL)
			return;
			
		std::set<nameString> selectedLights;
		
		const std::list<sel3dObject*>& selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator sit;
		for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
		{
			if ( nameObject *pNamedObject = sel3dCastUtil::CastPickObject<nameObject>(*sit) )
			{
				nameString item_name = pNamedObject->GetName();
				if (ltstLightSetMgr::IsLight(item_name))
					selectedLights.insert(item_name);
				else if (ltstLightSetMgr::IsLightSet(item_name))
				{
					std::vector<nameString> light_names;
					ltstLightSetMgr::GetLightsInSet(item_name, light_names);
					for (int i=0; i<light_names.size(); ++i)
						selectedLights.insert(light_names[i]);
				}
			}
		}

		ltstIsolateMgr::IsolateLights( selectedLights );

		// Update check boxes for lighting
		cmmSystemDialogUtil::UpdateSetRelationships();
	}

	//--------------------------------------------------------------------
	// Restore "active in render layer" flags so that there is no more
	// geometry "hidden" isolation.
	//--------------------------------------------------------------------
	void ClearHidden()
	{
		gpxRenderControl::ConfirmSingleThread();
		rlyrRenderLayerMgr::RestoreActiveInRenderLayer();
		gpxRenderControl::SetNeedsNewRender();
	}

	//--------------------------------------------------------------------
	// Set "active in render layer" flags such that only the selected
	// objects are visible.
	//--------------------------------------------------------------------
	void HideGeometryExceptSelection()
	{
		std::vector<nameString> all_objects;
		ltstLightSetMgr::GetAllObjects( all_objects );

		// start with all objects "off"
		std::map<nameString, bool> active_flags;
		std::vector<nameString>::const_iterator oit;
		for (oit = all_objects.begin(); oit != all_objects.end(); ++oit)
			active_flags[*oit] = false;

		// Per-surface settings for fragment selection
		std::map<nameString, std::vector<int> > surface_settings;

		// now set all selected objects to "true"
		const std::list<sel3dObject*>& selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator sit;
		for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
		{
			// Both object and fragment selections will cast to the parent nameObject
			if ( nameObject *pNamedObject = sel3dCastUtil::CastPickObject<nameObject>(*sit) )
			{
				nameString item_name = pNamedObject->GetName();
				if (ltstLightSetMgr::IsObject(item_name))
				{
					// Turn on the object, even if we select individual surface below
					active_flags[item_name] = true;

					// See if this is an individual fragment selection
					if (fgmtPropertyObject *pSurfaceObject = sel3dCastUtil::CastPickObject<fgmtPropertyObject>(*sit) )
					{
						if (fgmtScriptObject *pScriptObject = sel3dCastUtil::CastPickObject<fgmtScriptObject>(*sit))
						{
							int fragment_index = pScriptObject->GetIndexForName(pSurfaceObject->GetName());
							if (fragment_index >= 0)
							{
								// Add fragment index to vector
								std::vector<int>& surface_vector = surface_settings[item_name];
								surface_vector.push_back(fragment_index);
							}
						}
					}
				}
			}		
		}

		HideGeometry(active_flags, surface_settings);
	}
		
	//--------------------------------------------------------------------
	// Hide geometry based on a boolean flag per object name and
	// an optional map of lit surface ids per object name.
	//--------------------------------------------------------------------
	void HideGeometry(const std::map<nameString, bool>& i_ActiveFlags,
					  const std::map<nameString, std::vector<int> >& i_SurfaceSettings)
	{
		// Set whole object settings and then set individual surface flags.
		gpxRenderControl::ConfirmSingleThread();
		rlyrRenderLayerMgr::SetObjectsEditorVisibility(i_ActiveFlags);

		std::map<nameString, std::vector<int> >::const_iterator it;
		for (it = i_SurfaceSettings.begin(); it != i_SurfaceSettings.end(); ++it)
		{
			rlyrRenderLayerMgr::SetFragmentVisibility(it->first, it->second);
		}

		gpxRenderControl::SetNeedsNewRender();
	}

	//--------------------------------------------------------------------
	// Isolate the lights that influence the selected geometry.
	//--------------------------------------------------------------------
	void IsolateInfluences()
	{
		// Get light sets for the selected geometry
		std::set<nameString> light_sets;
		ltstInfluenceUtil::GetLightSetsForSelectedObjects(light_sets);

		// Add in the unassigned lights
		std::set<nameString> influence_lights;
		std::vector<nameString> light_names;
		nameString unassigned_set;
		ltstLightSetMgr::GetLightsInSet(unassigned_set, light_names);
		for (int i=0; i<light_names.size(); ++i)
			influence_lights.insert(light_names[i]);

		// For each light set, get the containing lights
		std::set<nameString>::const_iterator set_it;
		for (set_it = light_sets.begin(); set_it != light_sets.end(); ++set_it)
		{
			ltstLightSetMgr::GetLightsInSet(*set_it, light_names);
			for (int i=0; i<light_names.size(); ++i)
				influence_lights.insert(light_names[i]);
		}

		// Isolate the lights
		ltstIsolateMgr::IsolateLights( influence_lights );

		//bga - Changing the "influences" commands such that only 
		// one thing happens - either light isolation or geometry hiding.
		//
		// Also hide the geometry that isn't selected
		//HideGeometryExceptSelection();

		// Update check boxes for lighting
		cmmSystemDialogUtil::UpdateSetRelationships();
	}

}	// end of namespace
