/*****************************************************************************
**	lyerLayerMgr.cpp
**
**	Keeps track of groups of objects that can then have active and
**	draw style state altered as a group.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/lyer/lyerLayerMgr.hpp"

#include "Support/lyer/lyerLayerData.hpp"
#include "Support/lyer/lyerLayerInterest.hpp"
#include "Support/lyer/lyerObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Tool/api3d/api3dScene.hpp"


//============================================================================
//============================================================================
namespace lyerLayerMgr
{
	//============================================================================
	//============================================================================
	namespace
	{
		std::vector<lyerLayerInterest*>	l_InterestList;
		bool l_bDisableNotify = false;

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		void notify_interests_changed()
		{
			if (l_bDisableNotify) return;
			std::vector<lyerLayerInterest*>::iterator it, end = l_InterestList.end();
			for (it  = l_InterestList.begin(); it != end; ++it)
			{
				(*it)->DataChanged();
			}
		}
		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		void notify_interests_added()
		{
			if (l_bDisableNotify) return;
			std::vector<lyerLayerInterest*>::iterator it, end = l_InterestList.end();
			for (it  = l_InterestList.begin(); it != end; ++it)
			{
				(*it)->ObjectAdded();
			}
		}
		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		void notify_interests_renamed()
		{
			if (l_bDisableNotify) return;
			std::vector<lyerLayerInterest*>::iterator it, end = l_InterestList.end();
			for (it  = l_InterestList.begin(); it != end; ++it)
			{
				(*it)->ObjectRenamed();
			}
		}

		//--------------------------------------------------------------------------
		// forward declaration
		//--------------------------------------------------------------------------
		struct sLayer;

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		struct sObject
		{
			nameObject* m_pNameObj;
			lyerObject* m_pObject;
			sLayer* m_pContainingLayer;

			sObject(nameObject* i_pNameObj, lyerObject* i_pObject)
				: m_pNameObj(i_pNameObj), m_pObject(i_pObject), m_pContainingLayer(NULL) {}
		};

		//--------------------------------------------------------------------------
	// Note: It may have been easier to use std::map here, but I wasn't sure if
	// we could guarantee that the names were unique.
		//--------------------------------------------------------------------------
		std::vector<sObject*> l_Objects;

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		struct sLayer
		{
			nameString m_Name;
			std::vector<sObject*> m_Objects;
			//lyerLayerStyle m_Style;
			bool m_bVisible;
			bool m_bPickable;
			bool m_bWireframe;
			bool m_bLowRes;

			sLayer(const nameString& i_Name)
				: m_Name(i_Name), m_bVisible(true), m_bPickable(true),
					m_bWireframe(false), m_bLowRes(false)
			{}
		};

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		std::vector<sLayer*> l_Layers;

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		template<class S> 
		class name_search
		{
		public:
			name_search(const nameString& i_Name) : m_Name(i_Name) {};
			bool operator () ( S* i_Set )
			{
				return (m_Name == i_Set->m_Name);
			}
			nameString m_Name;
		};

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		template<class S> 
		class nameobj_search
		{
		public:
			nameobj_search(const nameString& i_Name) : m_Name(i_Name) {};
			bool operator () ( S* i_Set )
			{
				return (m_Name == i_Set->m_pNameObj->GetName());
			}
			nameString m_Name;
		};

		template<class S> 
		class nameobjptr_search
		{
		public:
			nameobjptr_search(nameObject* i_pNameObj) : m_pNameObj(i_pNameObj) {};
			bool operator () ( S* i_Set )
			{
				return (m_pNameObj == i_Set->m_pNameObj);
			}
			nameObject* m_pNameObj;
		};

		//--------------------------------------------------------------------------
		// Reset active, draw style states all to default values
		//--------------------------------------------------------------------------
		void restore_defaults(lyerObject* i_pObject)
		{
			i_pObject->SetLayerVisible(true);
			i_pObject->SetLayerWireframe(false);
			i_pObject->SetLayerPickable(true);
		}

		//--------------------------------------------------------------------------
		// Set values from light set into object
		//--------------------------------------------------------------------------
		/*void set_values(sLayer* i_pLayer, lyerObject* i_pObject)
		{
			switch(i_pLayer->m_Style)
			{
			case e_Normal:
				i_pObject->SetLayerVisible(true);
				i_pObject->SetLayerWireframe(false);
				i_pObject->SetLayerPickable(true);
				break;
			case e_Wireframe:
				i_pObject->SetLayerVisible(true);
				i_pObject->SetLayerWireframe(true);
				i_pObject->SetLayerPickable(false);
				break;
			case e_Invisible:
				i_pObject->SetLayerVisible(false);
				i_pObject->SetLayerWireframe(false);
				i_pObject->SetLayerPickable(false);
				break;
			}
		}*/
		//--------------------------------------------------------------------------
		void set_values(sLayer* i_pLayer, lyerObject* i_pObject)
		{
			i_pObject->SetLayerVisible(i_pLayer->m_bVisible);
			i_pObject->SetLayerPickable(i_pLayer->m_bPickable);
			i_pObject->SetLayerWireframe(i_pLayer->m_bWireframe);
			i_pObject->SetLayerLowRes(i_pLayer->m_bLowRes);
		}

		//--------------------------------------------------------------------------
		void set_values(sLayer* i_pLayer)
		{
			// set style in all objects
			std::vector<sObject*>::iterator obj_it, end = i_pLayer->m_Objects.end();
			for (obj_it = i_pLayer->m_Objects.begin(); obj_it != end; ++obj_it)
			{
				set_values(i_pLayer, (*obj_it)->m_pObject);
			}
		}

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		void disconnect_layer(sLayer* i_pLayer)
		{
			// Remove all objects from this layer
			std::vector<sObject*>::iterator lt_it, lt_end = i_pLayer->m_Objects.end();
			for (lt_it = i_pLayer->m_Objects.begin(); lt_it != lt_end; ++lt_it)
			{
				// restore defaults
				restore_defaults((*lt_it)->m_pObject);
			
				(*lt_it)->m_pContainingLayer = NULL;
			}
		}
		
		//--------------------------------------------------------------------------
		// Find layer by name
		//--------------------------------------------------------------------------
		sLayer* find_named_layer(const nameString& i_LayerName)
		{
			std::vector<sLayer*>::iterator layer_it = 
				std::find_if(l_Layers.begin(), l_Layers.end(), name_search<sLayer>(i_LayerName));
			if  (layer_it != l_Layers.end())
			{
				return (*layer_it);
			}
			return NULL;
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize()
	{
		// We could alternatively create an "unassigned" layer with
		// an empty name. Then all objects that are not in a set would belong
		// to that sLayer.  So far, I don't see that to be an advantage.
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize()
	{
		// Clean up
		envSTLHelpers::DeleteContainer(l_Objects);
		envSTLHelpers::DeleteContainer(l_Layers);
	}

	//--------------------------------------------------------------------
	//  Add named object to list of things that can be grouped
	//--------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
				   lyerObject* i_pObject)
	{
		l_Objects.push_back(new sObject(i_pNameObj,i_pObject));

		notify_interests_added();

	}

	//--------------------------------------------------------------------
	//	Remove object from manager (removing from all layers)
	//--------------------------------------------------------------------
	void RemoveObject(nameObject* i_pNameObj, 
				      lyerObject* i_pObject)
	{
		// A name search is not enough here, needs to use object pointer

		// Find named object
		std::vector<sObject*>::iterator obj_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobjptr_search<sObject>(i_pNameObj));
		if  (obj_it != l_Objects.end())
		{
			sObject* pObject = (*obj_it);

			//  Remove from containing layer
			if (pObject->m_pContainingLayer)
			{
				envSTLHelpers::RemoveOneValue(pObject->m_pContainingLayer->m_Objects, pObject);
			}

			// Delete object structure, remove from list
			delete pObject;
			l_Objects.erase(obj_it);
		}

		notify_interests_added();

	}

	//--------------------------------------------------------------------
	//	Change assignments from OldNameObj to NewNameObj (both of which
	//	should be registered with the manager at the point of calling 
	//	this function). This is used when reloading or replacing 
	//	geometry in a system.
	//--------------------------------------------------------------------
	void ReplaceObject(nameObject* i_pOldNameObj, 
					   nameObject* i_pNewNameObj)
	{
		// Find named objects (using pointer, since they both have same nameString)
		std::vector<sObject*>::iterator obj1_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobjptr_search<sObject>(i_pOldNameObj));
		std::vector<sObject*>::iterator obj2_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobjptr_search<sObject>(i_pNewNameObj));

		if (obj1_it != l_Objects.end() && obj2_it != l_Objects.end())
		{
			sLayer* pLayer = (*obj1_it)->m_pContainingLayer;
			if (pLayer)
			{
				sObject *pObject = (*obj2_it);

				// Remove from old layer
				if (pObject->m_pContainingLayer)
				{
					envSTLHelpers::RemoveOneValue(pObject->m_pContainingLayer->m_Objects, pObject);
				}

				// Add object to new layer
				pLayer->m_Objects.push_back(pObject);
				pObject->m_pContainingLayer = pLayer;

				// setup up state values for this object
				set_values(pLayer, pObject->m_pObject);
			}
		}
	}


	//--------------------------------------------------------------------
	//	Notify the layer manager that the name of this object has changed
	//--------------------------------------------------------------------
	void ObjectRenamed(nameObject* i_pNameObj)
	{
		notify_interests_renamed();
	}


	//--------------------------------------------------------------------
	// Returns true if non-empty and unique name for a layer.
	//--------------------------------------------------------------------
	bool IsValidLayerName(const std::string &i_Name)
	{
		if (i_Name.empty()) return false;

		std::vector<sLayer*>::iterator it, end = l_Layers.end();
		for (it = l_Layers.begin(); it != end; ++it)
		{
			if ((*it)->m_Name.GetString() == i_Name)
				return false;
		}
		return true;
	}

	//--------------------------------------------------------------------
	//	Create a named layer
	//--------------------------------------------------------------------
	lyerLayerHandle  CreateLayer(const nameString& i_Name)
	{
		DBG_ASSERT(!i_Name.IsEmpty(), "A layer needs a valid name");

		// Check for unique name, GUI should use IsValidLayerName
		std::vector<sLayer*>::iterator layer_it = 
			std::find_if(l_Layers.begin(), l_Layers.end(), name_search<sLayer>(i_Name));
		if (layer_it != l_Layers.end())
		{
			DBG_WARNING("Layer names should be unique: " << i_Name.GetString().c_str());
		}

		sLayer *pLayer = new sLayer(i_Name);
		l_Layers.push_back(pLayer);
		nameMgr::RegisterName( pLayer->m_Name );

		notify_interests_added();
		return reinterpret_cast<lyerLayerHandle>(pLayer);
	}

	//--------------------------------------------------------------------
	//	Destroy named layer, all objects in this set become 
	//	"unassigned"
	//--------------------------------------------------------------------
	void  DeleteLayer(const nameString& i_Name)
	{
		// Find named layer
		std::vector<sLayer*>::iterator layer_it = 
			std::find_if(l_Layers.begin(), l_Layers.end(), name_search<sLayer>(i_Name));
		if  (layer_it != l_Layers.end())
		{
			sLayer *pLayer = (*layer_it);

			disconnect_layer(pLayer);
			delete pLayer;

			l_Layers.erase(layer_it);
		}

		notify_interests_added();
	}

	//--------------------------------------------------------------------
	//	Destroy named layer, using std::string
	//--------------------------------------------------------------------
	void  DeleteLayer(const std::string& i_Name)
	{
		std::vector<sLayer*>::iterator it, end = l_Layers.end();
		for (it = l_Layers.begin(); it != end; ++it)
		{
			sLayer *pLayer = (*it);
			if (pLayer->m_Name.GetString() == i_Name)
			{
				DeleteLayer(pLayer->m_Name);
				break;
			}
		}
	}

	//--------------------------------------------------------------------
	//	Rename a layer
	//--------------------------------------------------------------------
	void  RenameLayer(const nameString& i_OldName, const nameString& i_NewName)
	{
		std::vector<sLayer*>::iterator it, end = l_Layers.end();
		for (it = l_Layers.begin(); it != end; ++it)
		{
			sLayer *pLayer = (*it);
			if (pLayer->m_Name == i_OldName)
			{
				pLayer->m_Name = i_NewName;
				break;
			}
		}

		notify_interests_added();
	}

	//--------------------------------------------------------------------
	// Remove all layers (preparing for a new scene)
	//--------------------------------------------------------------------
	void ClearAllLayers()
	{
		// Disconnect all layers before destroying
		std::vector<sLayer*>::iterator it, end = l_Layers.end();
		for (it = l_Layers.begin(); it != end; ++it)
		{
			sLayer *pLayer = (*it);

			disconnect_layer(pLayer);
		}

		envSTLHelpers::DeleteContainer(l_Layers);

		notify_interests_changed();
	}
	//--------------------------------------------------------------------
	//	Remove all objects from the given layer
	//--------------------------------------------------------------------
	void  ClearLayer(const nameString& i_LayerName)
	{
		// Find named layer
		std::vector<sLayer*>::iterator layer_it = 
			std::find_if(l_Layers.begin(), l_Layers.end(), name_search<sLayer>(i_LayerName));
		if  (layer_it != l_Layers.end())
		{
			sLayer *pLayer = (*layer_it);

			disconnect_layer(pLayer);
		}
		notify_interests_changed();
	}

	//--------------------------------------------------------------------
	//	Add object to the given layer
	//--------------------------------------------------------------------
	void  AddObjectToLayer(const nameString& i_LayerName, 
							  const nameString& i_ObjectName)
	{
		// Find named object
		std::vector<sObject*>::iterator obj_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobj_search<sObject>(i_ObjectName));
		// Find named layer
		std::vector<sLayer*>::iterator layer_it = 
			std::find_if(l_Layers.begin(), l_Layers.end(), name_search<sLayer>(i_LayerName));
		if  (layer_it != l_Layers.end() && obj_it != l_Objects.end())
		{
			sLayer *pLayer = (*layer_it);
			sObject *pObject = (*obj_it);

			//DBG_LOG3("layer: object %s remove from %s add to %s", pObject->m_pNameObj->GetName().GetString().c_str(),
			//														pLayer->m_Name.GetString().c_str(), 
			//														i_LayerName.GetString().c_str() );

			// Remove from old layer
			if (pObject->m_pContainingLayer)
			{
				envSTLHelpers::RemoveOneValue(pObject->m_pContainingLayer->m_Objects, pObject);
			}

			// Add object to new layer
			//DBG_LOG2("Adding object %s to layer %s", pObject->m_pNameObj->GetName().GetString().c_str(), pLayer->m_Name.GetString().c_str());
			pLayer->m_Objects.push_back(pObject);
			pObject->m_pContainingLayer = pLayer;

			// setup up state values for this object
			set_values(pLayer, pObject->m_pObject);
		}
		else
		{
			if (layer_it == l_Layers.end())
			{
				DBG_WARNING("Could not find layer named: " << i_LayerName.GetString().c_str());
			}
			if (obj_it == l_Objects.end())
			{
				DBG_WARNING("Could not find object named: " << i_ObjectName.GetString().c_str());
			}
		}
		notify_interests_changed();
	}

	//--------------------------------------------------------------------
	//	Remove object from the given layer
	//--------------------------------------------------------------------
	void  RemoveObjectFromLayer(const nameString& i_LayerName, 
								   const nameString& i_ObjectName)
	{
		// Find named object
		std::vector<sObject*>::iterator obj_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobj_search<sObject>(i_ObjectName));
		// Find named layer
		std::vector<sLayer*>::iterator layer_it = 
			std::find_if(l_Layers.begin(), l_Layers.end(), name_search<sLayer>(i_LayerName));
		if  (layer_it != l_Layers.end() && obj_it != l_Objects.end())
		{
			sLayer *pLayer = (*layer_it);

			// Remove object from layer
			envSTLHelpers::RemoveOneValue(pLayer->m_Objects, (*obj_it));
			(*obj_it)->m_pContainingLayer = NULL;

			// restore defaults
			restore_defaults((*obj_it)->m_pObject);
		}
		notify_interests_changed();
	}

	//--------------------------------------------------------------------
	// Layer style gives one enumeration that controls visible,
	//	wireframe, pickable settings.
	//--------------------------------------------------------------------
	//void SetLayerStyle(const nameString& i_LayerName, 
	//				   lyerLayerStyle i_Style)
	//{
	//	// Find named layer
	//	sLayer *pLayer = find_named_layer(i_LayerName);
	//	if (pLayer)
	//	{
	//		pLayer->m_Style = i_Style;

	//		// set style in all objects in layer
	//		set_values(pLayer);
	//	}

	//}
	//lyerLayerStyle GetLayerStyle(const nameString& i_LayerName)
	//{
	//	// Find named layer
	//	sLayer *pLayer = find_named_layer(i_LayerName);
	//	if (pLayer)
	//	{
	//		return pLayer->m_Style;
	//	}
	//	return e_Normal;
	//}

	//--------------------------------------------------------------------
	// Separate flags for attributes of layer
	//--------------------------------------------------------------------
	void SetLayerVisible(const nameString& i_LayerName, bool i_bVisible)
	{
		// Find named layer
		sLayer *pLayer = find_named_layer(i_LayerName);
		if (pLayer)
		{
			pLayer->m_bVisible = i_bVisible;
			set_values(pLayer);
			notify_interests_changed();
		}
	}
	bool GetLayerVisible(const nameString& i_LayerName)
	{
		// Find named layer
		sLayer *pLayer = find_named_layer(i_LayerName);
		if (pLayer)
			return pLayer->m_bVisible;
		return true;
	}

	void SetLayerPickable(const nameString& i_LayerName, bool i_bPickable)
	{
		// Find named layer
		sLayer *pLayer = find_named_layer(i_LayerName);
		if (pLayer)
		{
			pLayer->m_bPickable = i_bPickable;
			set_values(pLayer);
			notify_interests_changed();
		}
	}
	bool GetLayerPickable(const nameString& i_LayerName)
	{
		// Find named layer
		sLayer *pLayer = find_named_layer(i_LayerName);
		if (pLayer)
			return pLayer->m_bPickable;
		return true;
	}

	void SetLayerWireframe(const nameString& i_LayerName, bool i_bWireframe)
	{
		// Find named layer
		sLayer *pLayer = find_named_layer(i_LayerName);
		if (pLayer)
		{
			pLayer->m_bWireframe = i_bWireframe;
			set_values(pLayer);
			notify_interests_changed();
		}
	}
	bool GetLayerWireframe(const nameString& i_LayerName)
	{
		// Find named layer
		sLayer *pLayer = find_named_layer(i_LayerName);
		if (pLayer)
			return pLayer->m_bWireframe;
		return true;
	}

	void SetLayerLowRes(const nameString& i_LayerName, bool i_bLowRes)
	{
		// Find named layer
		sLayer *pLayer = find_named_layer(i_LayerName);
		if (pLayer)
		{
			pLayer->m_bLowRes = i_bLowRes;
			set_values(pLayer);
			notify_interests_changed();
		}
	}
	bool GetLayerLowRes(const nameString& i_LayerName)
	{
		// Find named layer
		sLayer *pLayer = find_named_layer(i_LayerName);
		if (pLayer)
			return pLayer->m_bLowRes;
		return true;
	}

	//--------------------------------------------------------------------
	// One step function to set all layers visible and to normal state
	//--------------------------------------------------------------------
	void AllLayersVisible()
	{
		std::vector<sLayer*>::iterator it, end = l_Layers.end();
		for (it = l_Layers.begin(); it != end; ++it)
		{
			sLayer *pLayer = (*it);

			// restore all layers to normal visibility and draw style
			pLayer->m_bVisible = true;

			// set style in all objects
			set_values(pLayer);
		}
	}

	//--------------------------------------------------------------------
	//  Convert from handle to name
	//--------------------------------------------------------------------
	const nameString& GetLayerName(lyerLayerHandle i_Handle)
	{
		sLayer *pLayer = reinterpret_cast<sLayer*>(i_Handle);
		return pLayer->m_Name;

	}
	void SetLayerName(lyerLayerHandle i_Handle, const nameString& i_Name)
	{
		sLayer *pLayer = reinterpret_cast<sLayer*>(i_Handle);
		pLayer->m_Name = i_Name;
	}

	//--------------------------------------------------------------------
	// Return number of layers
	//--------------------------------------------------------------------
	int GetNumLayers()
	{
		return l_Layers.size();
	}

	//--------------------------------------------------------------------
	//  Get names of layers
	//--------------------------------------------------------------------
	void GetLayerNames(std::vector<nameString> &o_Names)
	{
		std::vector<sLayer*>::iterator it, end = l_Layers.end();
		for (it = l_Layers.begin(); it != end; ++it)
		{
			o_Names.push_back((*it)->m_Name);		
		}
	}
	//--------------------------------------------------------------------
	//  Get names of objects in a layer. Use empty
	//	i_LayerName ("") to ask about "unassigned" objects.
	//--------------------------------------------------------------------
	void GetObjectsInLayer(const nameString& i_LayerName, 
						 std::vector<nameString> &o_ObjectNames)
	{
		// Handle "unassigned" layer
		if (i_LayerName.IsEmpty())
		{
			std::vector<sObject*>::iterator it, end = l_Objects.end();
			for (it = l_Objects.begin(); it != end; ++it)
			{
				// If no containing layer, this object is unassigned
				if ((*it)->m_pContainingLayer == NULL)
					o_ObjectNames.push_back((*it)->m_pNameObj->GetName());
			}
		}
		else
		{
			// Find named layer
			std::vector<sLayer*>::iterator layer_it = 
				std::find_if(l_Layers.begin(), l_Layers.end(), name_search<sLayer>(i_LayerName));
			if  (layer_it != l_Layers.end())
			{
				sLayer *pLayer = (*layer_it);

				o_ObjectNames.resize(pLayer->m_Objects.size());
				int i=0;
				std::vector<sObject*>::iterator it, end = pLayer->m_Objects.end();
				for (it = pLayer->m_Objects.begin(); it != end; ++it)
				{
					o_ObjectNames[i++] = (*it)->m_pNameObj->GetName();
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//  Get list of all names of objects registered in the manager
	//--------------------------------------------------------------------
	void GetAllObjects(std::vector<nameString> &o_ObjectNames)
	{
		std::vector<sObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			o_ObjectNames.push_back((*it)->m_pNameObj->GetName());
		}
	}
	
	//--------------------------------------------------------------------
	//	Gets name of layer containing the given object.
	//	Returns true if object is contained in a layer and then
	//		sets o_LayerName to hold the name of the layer.
	//--------------------------------------------------------------------
	bool  GetLayerNameFromObject(const nameString& i_ObjectName, 
							     nameString& o_LayerName)
	{
		// Find named object
		std::vector<sObject*>::iterator obj_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobj_search<sObject>(i_ObjectName));
		if  (obj_it != l_Objects.end())
		{
			sObject* pObject = (*obj_it);
			if (pObject->m_pContainingLayer != NULL)
			{
				o_LayerName = pObject->m_pContainingLayer->m_Name;
				return true;
			}
		}
		return false;
	}

	//--------------------------------------------------------------------
	//  Access to whole data as one structure 
	//--------------------------------------------------------------------
	lyerLayersData GetData()
	{
		lyerLayersData layers_data;
		layers_data.m_Layers.resize(l_Layers.size());

		int si = 0;
		std::vector<sLayer*>::iterator it, end = l_Layers.end();
		for (it = l_Layers.begin(); it != end; ++it)
		{
			sLayer* pLayer = (*it);

			lyerLayerData &data = layers_data.m_Layers[si++];
			data.m_Name = pLayer->m_Name;

			// Do state values
			//data.m_Style = pLayer->m_Style;	
			data.m_bVisible = pLayer->m_bVisible;
			data.m_bPickable = pLayer->m_bPickable;
			data.m_bWireframe = pLayer->m_bWireframe;
			data.m_bLowRes = pLayer->m_bLowRes;
			
			//DBG_LOG3("GetData: layer: %s uid: %d, num objects %d", data.m_Name.GetString().c_str(), data.m_Name.GetUID(), pLayer->m_Objects.size());

			int i=0;
			data.m_Objects.resize(pLayer->m_Objects.size());
			std::vector<sObject*>::iterator obj_it, obj_end = pLayer->m_Objects.end();
			for (obj_it = pLayer->m_Objects.begin(); obj_it != obj_end; ++obj_it)
			{
				nameString name = (*obj_it)->m_pNameObj->GetName();
				//DBG_LOG2("  Layer object: %s uid %d", name.GetString().c_str(), name.GetUID());
				data.m_Objects[i++] = name;
			}
		}

		return layers_data;
	}
	void SetData(const lyerLayersData &i_Data)
	{
		l_bDisableNotify = true;
		ClearAllLayers();

		std::vector<lyerLayerData>::const_iterator it, end = i_Data.m_Layers.end();
		for (it = i_Data.m_Layers.begin(); it != end; ++it)
		{
			const lyerLayerData &data = (*it);

			//DBG_LOG3("SetData: creating layer: %s uid: %d, num objects %d", data.m_Name.GetString().c_str(), data.m_Name.GetUID(), data.m_Objects.size());
			CreateLayer(data.m_Name);

			//SetLayerStyle(data.m_Name, data.m_Style);	
			SetLayerVisible(data.m_Name, data.m_bVisible);
			SetLayerPickable(data.m_Name, data.m_bPickable);
			SetLayerWireframe(data.m_Name, data.m_bWireframe);
			SetLayerLowRes(data.m_Name, data.m_bLowRes);

			const int num_objects = data.m_Objects.size();
			for (int i=0; i<num_objects; ++i)
			{
				AddObjectToLayer(data.m_Name, data.m_Objects[i]);
			}
		}

		l_bDisableNotify = false;
		notify_interests_changed();
	}

	//--------------------------------------------------------------------
	//  Adds given data to exisiting data in manager
	//
	//	If i_bRenameDupes is true, the duplicate entry will be renamed
	//	and merged.  If it is false, the duplicate will NOT be merged.
	//--------------------------------------------------------------------
	void MergeData(const lyerLayersData &i_Data, bool i_bRenameDupes)
	{
		l_bDisableNotify = true;

		std::vector<lyerLayerData>::const_iterator it, end = i_Data.m_Layers.end();
		for (it = i_Data.m_Layers.begin(); it != end; ++it)
		{
			const lyerLayerData &data = (*it);
			//DBG_LOG3("MergeData: adding layer: %s uid: %d, num objects %d", data.m_Name.GetString().c_str(), data.m_Name.GetUID(), data.m_Objects.size());

			// Only create layer name if it does not already exist.
			std::vector<sLayer*>::iterator layer_it = 
				std::find_if(l_Layers.begin(), l_Layers.end(), name_search<sLayer>(data.m_Name));
			if ((layer_it == l_Layers.end()) || i_bRenameDupes)
			{
				if (i_bRenameDupes)
				{
					//DBG_LOG("Creating new layer, " << data.m_Name.GetString().c_str() );
					CreateLayer(data.m_Name);
				}
				else
				{
					//DBG_LOG("Creating new layer, " << data.m_Name.GetString().c_str() );
					CreateLayer(data.m_Name);
				}
			}

			//SetLayerStyle(data.m_Name, data.m_Style);
			SetLayerVisible(data.m_Name, data.m_bVisible);
			SetLayerPickable(data.m_Name, data.m_bPickable);
			SetLayerWireframe(data.m_Name, data.m_bWireframe);
			SetLayerLowRes(data.m_Name, data.m_bLowRes);

			const int num_objects = data.m_Objects.size();
			for (int i=0; i<num_objects; ++i)
			{
				AddObjectToLayer(data.m_Name, data.m_Objects[i]);
			}
		}

		l_bDisableNotify = false;
		notify_interests_changed();
	}

	//--------------------------------------------------------------------
	//	RegisterLayerInterest() - add a Layer interest 
	//--------------------------------------------------------------------
	void RegisterLayerInterest( lyerLayerInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Layer Interest" );
		l_InterestList.push_back( i_pInterest );
	}

	//--------------------------------------------------------------------
	//	UnRegisterLayerInterest() - remove a Layer interest 
	//
	//	Note: this will NOT delete the Layer interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterLayerInterest( lyerLayerInterest* i_pInterest )
	{
		envSTLHelpers::RemoveOneValue( l_InterestList, i_pInterest );
	}

}	// end of namespace
