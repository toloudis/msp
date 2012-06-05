/*****************************************************************************
**	ltstLightSetMgr.cpp
**
**	Keeps track of lights and objects that can be grouped so that certain
**	lights affect only certain objects.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/ltst/ltstLightSet.hpp"
#include "Support/ltst/ltstLightSetInterest.hpp"
#include "Support/ltst/private/ltstLightSetInterestMgr.hpp"
#include "Support/ltst/private/ltstLightSetLight.hpp"
#include "Support/ltst/private/ltstLightSetNode.hpp"
#include "Support/ltst/private/ltstLightSetObject.hpp"
#include "Support/ltst/ltstLightSetsData.hpp"
#include "Support/ltst/private/ltstLightSetUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Tool/api3d/api3dObjectSingle.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/pick3d/pick3dPickObject.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

#include "Support/ltst/GUI/ltstReAttachUtil.hpp"

#include "Support/ltst/GUI/ltstReAttachForm.h"

//using namespace ltstReAttachUtil;
using namespace ltstLightSetUtil;


namespace ltstLightSetMgr
{

	namespace
	{
		g3dRenderState *m_pRootRenderState = NULL;
		const maFloatRGBA c_Black(0,0,0,0);

	// Note: It may have been easier to use std::map here, but I wasn't sure if
	// we could guarantee that the names were unique.
		std::vector<ltstLightSetLight*> l_Lights;
		std::vector<ltstLightSetObject*> l_Objects;
		std::vector<ltstLightSet*> l_LightSets;

		void remove_light_from_set(ltstLightSet* i_pLightSet, ltstLightSetLight* i_pLight)
		{
			i_pLightSet->RemoveLightFromSet(i_pLight);

			// Add light to root state when unassigned
			m_pRootRenderState->m_Lights.push_back(i_pLight->m_pLight);
		}

		void disconnect_light_set(ltstLightSet* i_pLightSet)
		{
			i_pLightSet->Disconnect( m_pRootRenderState );
		}

		//============================================================================
		//	get_if gets the pointer in the collection for which the predicate
		//	is true
		//============================================================================
		template<class S, class Pred>
		S* get_if(std::vector<S*>& i_Container, Pred p)
		{
			std::vector<S*>::iterator it = std::find_if(i_Container.begin(), i_Container.end(), p);
			if  (it != i_Container.end())
			{
				return (*it);
			}
			return NULL;
		}
	}	// end of namespace

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize()
	{
		m_pRootRenderState = new g3dRenderState();
		g3dSceneNode *root_node = api3dScene::GetRoot( api3dScene::WorldLayerIndex() );
		root_node->SetRenderState(m_pRootRenderState);

		// We could alternatively create an "unassigned" light set with
		// an empty name. Then all lights that are not in a set would belong
		// to that ltstLightSet.  So far, I don't see that to be an advantage.
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize()
	{
		// Clean up
		envSTLHelpers::DeleteContainer(l_Lights);
		envSTLHelpers::DeleteContainer(l_Objects);
		envSTLHelpers::DeleteContainer(l_LightSets);

		delete m_pRootRenderState;
		g3dSceneNode *root_node = api3dScene::GetRoot( api3dScene::WorldLayerIndex() );
		root_node->SetRenderState(NULL);
	}

	//--------------------------------------------------------------------
	//  Add named light to list of things that can be grouped into
	//	lights sets
	//--------------------------------------------------------------------
	void  AddLight(nameObject* i_pNameObj, 
				  g3dLight* i_pLight)
	{
		l_Lights.push_back(new ltstLightSetLight(i_pNameObj, i_pLight));

		// This light's m_pContainingLightSet is NULL by default,
		// signifying that it is "unassigned" for purposes of the
		// GetLightsInSet() function.

		// Add light to root state when unassigned
		m_pRootRenderState->m_Lights.push_back(i_pLight);

		ltstLightSetInterestMgr::NotifyInterestsAdded();
	}

	//--------------------------------------------------------------------
	//	Remove light from manager (removing from light set)
	//--------------------------------------------------------------------
	void RemoveLight(nameObject* i_pNameObj, 
					 g3dLight* i_pLight)
	{
		// Note: a name search is probably not enough here, needs to use light pointer
		// if we don't know if names are unique?

		// Find named light
		ltstLightSetLight* pLight = get_if(l_Lights, nameobj_search<ltstLightSetLight>(i_pNameObj->GetName()));
		if  (pLight)
		{
			//  Remove from light set
			if (pLight->m_pContainingLightSet)
			{
				remove_light_from_set(pLight->m_pContainingLightSet, pLight);
			}
			
			// Light is in root render state when unassigned
			envSTLHelpers::RemoveOneValue(m_pRootRenderState->m_Lights, i_pLight);

			// Delete light structure, remove from list
			envSTLHelpers::DeleteOneValue(l_Lights, pLight);
		}

		ltstLightSetInterestMgr::NotifyInterestsAdded();

	}

	//--------------------------------------------------------------------
	//  Add named object to list of things that can be lit by light sets
	//--------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
				   api3dObjectSingle* i_pObject)
	{
		l_Objects.push_back(new ltstLightSetObject(i_pNameObj,i_pObject));

		ltstLightSetInterestMgr::NotifyInterestsAdded();

	}

	//--------------------------------------------------------------------
	//	Remove object from manager (removing from all light sets)
	//--------------------------------------------------------------------
	void RemoveObject(nameObject* i_pNameObj, 
				      api3dObjectSingle* i_pObject)
	{
		// A name search is not enough here, needs to use object pointer

		// Find named object
		ltstLightSetObject* pObject = get_if(l_Objects, nameobjptr_search<ltstLightSetObject>(i_pNameObj));
		if  (pObject)
		{
			// Remove object from all light sets. Easier to do it this way
			// than to undo each node within the object using the
			// back pointers.
			std::vector<ltstLightSet*>::iterator it, end = l_LightSets.end();
			for (it = l_LightSets.begin(); it != end; ++it)
			{
				ltstLightSet* pSet = (*it);

				// safe to call this even when the object was not added
				pSet->RemoveWholeObjectFromSet(pObject);
			}
			
			envSTLHelpers::DeleteOneValue(l_Objects, pObject);
		}

		ltstLightSetInterestMgr::NotifyInterestsAdded();

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
		// Find named object
		ltstLightSetObject* pObject1 = get_if(l_Objects, nameobjptr_search<ltstLightSetObject>(i_pOldNameObj));
		ltstLightSetObject* pObject2 = get_if(l_Objects, nameobjptr_search<ltstLightSetObject>(i_pNewNameObj));
		if  (pObject1 && pObject2)
		{
			// Synchronize the root nodes
			int num_sets = pObject1->RootNode()->m_ContainingLightSets.size();
			for (int s=0; s<num_sets; s++)
			{
				ltstLightSet* pSet = pObject1->RootNode()->m_ContainingLightSets[s];
				
				// Add new object to same light sets
				pSet->AddWholeObjectToSet( pObject2 );
			}
			
			// Synchronize the fragment nodes
			const int num_nodes = pObject1->GetNumNodes();
			if (pObject2->GetNumNodes() == num_nodes)
			{
				for (int i=0; i<num_nodes; i++)
				{
					num_sets = pObject1->Node(i)->m_ContainingLightSets.size();
					for (int s=0; s<num_sets; s++)
					{
						ltstLightSet* pSet = pObject1->Node(i)->m_ContainingLightSets[s];
						
						// Add new object to same light sets
						pSet->AddNodeToSet( pObject2, i );
					}
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//	Notify the light manager that the name of this light has changed
	//--------------------------------------------------------------------
	void LightRenamed(nameObject* i_pNameObj)
	{
		ltstLightSetInterestMgr::NotifyInterestsRenamed();
	}

	//--------------------------------------------------------------------
	//	Select the light with the given name in the 3D scene
	//--------------------------------------------------------------------
	void SelectLight(const nameString& i_Name)
	{
		// Find named light
		ltstLightSetLight* pLight = get_if(l_Lights, nameobj_search<ltstLightSetLight>(i_Name));
		if  (pLight)
		{
			// See if we can cast the object to an object that can be selected
			// with the sel3dMgr
			pick3dPickObject *pPickObject = dynamic_cast<pick3dPickObject*>(pLight->m_pNameObj);
			if (pPickObject)
			{
				sel3dMgr::CreateUndoOperation();
				sel3dMgr::Select(pPickObject);
			}
		}
	}

	//--------------------------------------------------------------------
	// Returns true if non-empty and unique name for a light set.
	//--------------------------------------------------------------------
	bool IsValidLightSetName(const std::string &i_Name)
	{
		if (i_Name.empty()) return false;

		std::vector<ltstLightSet*>::iterator it, end = l_LightSets.end();
		for (it = l_LightSets.begin(); it != end; ++it)
		{
			if ((*it)->m_Name.GetString() == i_Name)
				return false;
		}
		return true;
	}

	//--------------------------------------------------------------------
	//	Create a named light set
	//--------------------------------------------------------------------
	ltstLightSet*  CreateLightSet(const nameString& i_Name)
	{
		DBG_ASSERT0(!i_Name.IsEmpty(), "A light set needs a valid name");

		// Check for unique name, print warning
		std::vector<ltstLightSet*>::iterator set_it = 
			std::find_if(l_LightSets.begin(), l_LightSets.end(), name_search<ltstLightSet>(i_Name));
		if (set_it != l_LightSets.end())
		{
			DBG_WARNING1("Light set names should be unique: %s", i_Name.GetString().c_str());
		}

		ltstLightSet *pSet = new ltstLightSet(i_Name);
		l_LightSets.push_back(pSet);
		nameMgr::RegisterName( pSet->m_Name );

		ltstLightSetInterestMgr::NotifyInterestsAdded();

		return pSet;
	}

	//--------------------------------------------------------------------
	//	Destroy named light set, all lights in this set become 
	//	"unassigned"
	//--------------------------------------------------------------------
	void  DeleteLightSet(const nameString& i_Name)
	{
		// Find named light set
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_Name));
		if (pSet)
		{
			disconnect_light_set(pSet);

			envSTLHelpers::DeleteOneValue(l_LightSets, pSet);
		}

		ltstLightSetInterestMgr::NotifyInterestsAdded();
	}

	//--------------------------------------------------------------------
	//	Destroy named light set, using std::string
	//--------------------------------------------------------------------
	void  DeleteLightSet(const std::string& i_Name)
	{
		std::vector<ltstLightSet*>::iterator it, end = l_LightSets.end();
		for (it = l_LightSets.begin(); it != end; ++it)
		{
			ltstLightSet *pSet = (*it);
			if (pSet->m_Name.GetString() == i_Name)
			{
				DeleteLightSet(pSet->m_Name);
				break;
			}
		}
	}

	//--------------------------------------------------------------------
	//	Rename a named light set
	//--------------------------------------------------------------------
	void  RenameLightSet(const nameString& i_OldSetName, 
						 const std::string &i_NewName)
	{
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_OldSetName));
		if (pSet)
		{
			pSet->m_Name.SetString(i_NewName);
		}
		ltstLightSetInterestMgr::NotifyInterestsChanged();
	}

	//--------------------------------------------------------------------
	//	Remove all lights and objects from given light set
	//--------------------------------------------------------------------
	void  ClearLightSet(const nameString& i_Name)
	{
		// Find named light set
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_Name));
		if (pSet)
		{
			disconnect_light_set(pSet);
		}

		ltstLightSetInterestMgr::NotifyInterestsAdded();
	}

	//--------------------------------------------------------------------
	// Remove all light sets (preparing for a new scene)
	//--------------------------------------------------------------------
	void ClearAllLightSets()
	{
		// Disconnect all light sets before destroying
		std::vector<ltstLightSet*>::iterator it, end = l_LightSets.end();
		for (it = l_LightSets.begin(); it != end; ++it)
		{
			ltstLightSet *pSet = (*it);

			disconnect_light_set(pSet);
		}

		envSTLHelpers::DeleteContainer(l_LightSets);

		ltstLightSetInterestMgr::NotifyInterestsChanged();
	}

	//--------------------------------------------------------------------
	// Set ambient light for the whole scene.
	//--------------------------------------------------------------------
	void SetSceneAmbientLight(const maFloatRGBA &i_AmbientLight)
	{
		m_pRootRenderState->m_AmbientLight = i_AmbientLight;
	}
	const maFloatRGBA& GetSceneAmbientLight()
	{
		return m_pRootRenderState->m_AmbientLight;
	}

	//--------------------------------------------------------------------
	// Set ambient light for the given light set. This is added to
	// the scene's ambient.
	//--------------------------------------------------------------------
	void SetLightSetAmbientLight(const nameString& i_SetName, 
								 const maFloatRGBA &i_AmbientLight)
	{
		// Find named light set
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_SetName));
		if (pSet)
		{
			pSet->SetAmbientLight( i_AmbientLight );
		}
	}
	const maFloatRGBA& GetLightSetAmbientLight(const nameString& i_SetName)
	{
		// Find named light set
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_SetName));
		if (pSet)
			return pSet->m_AmbientLight;
		
		// light set not found, return const, static black color
		return c_Black;
	}

	//--------------------------------------------------------------------
	//	Add named light to named light set.
	//--------------------------------------------------------------------
	void  AddLightToSet(const nameString& i_SetName, 
						const nameString& i_LightName)
	{
		// Find named light
		ltstLightSetLight* pLight = get_if(l_Lights, nameobj_search<ltstLightSetLight>(i_LightName));
		// Find named light set
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_SetName));
		if  (pLight)
		{
			// Remove light from old light set
			if (pLight->m_pContainingLightSet)
			{
				remove_light_from_set(pLight->m_pContainingLightSet, pLight);
			}


			if (pSet)
			{
				// Add light to new light set
				pSet->AddLightToSet(pLight);
				
				// Light is in root render state when unassigned,
				// when assigned, it is removed (objects will add it when needed)
				envSTLHelpers::RemoveOneValue(m_pRootRenderState->m_Lights, pLight->m_pLight);
			}
		}
		//If the light is not found we need to ask the user to give us a replacement
		else
		{
			if (pLight == NULL)
			{   
				nameObject* newLightLink;
				DBG_WARNING1("Light named %s was not found", i_LightName.GetString().c_str());
				//#ifdef _MANAGED
				newLightLink = ltstReAttachUtil::GetObjByName(i_LightName.GetString(), false);
				//#endif
				if( newLightLink != NULL )
				{
					nameString newLightName = newLightLink->GetName();
					// Find named light
					ltstLightSetLight* newLight = get_if(l_Lights, nameobj_search<ltstLightSetLight>(newLightName));
					
					if  (newLight && pSet)
					{
						pSet->AddLightToSet(newLight);
						envSTLHelpers::RemoveOneValue(m_pRootRenderState->m_Lights, newLight->m_pLight);
					}
				}
			}

		}

		ltstLightSetInterestMgr::NotifyInterestsChanged();
	}

	//--------------------------------------------------------------------
	//	Remove named light from named light set.
	//--------------------------------------------------------------------
	void  RemoveLightFromSet(const nameString& i_SetName, 
							 const nameString& i_LightName)
	{
		// Find named light set
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_SetName));
		// Find named light 
		ltstLightSetLight* pLight = get_if(l_Lights, nameobj_search<ltstLightSetLight>(i_LightName));
		if  (pSet && pLight)
		{
			// Remove light from light set
			pSet->RemoveLightFromSet(pLight);

			// Add light to root state when unassigned
			m_pRootRenderState->m_Lights.push_back(pLight->m_pLight);
		}

		ltstLightSetInterestMgr::NotifyInterestsChanged();
	}

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddObjectToLightSet(const nameString& i_SetName, 
							  const nameString& i_ObjectName)
	{
		// Find named object
		ltstLightSetObject* pObject = get_if(l_Objects, nameobj_search<ltstLightSetObject>(i_ObjectName));
		// Find named light set
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_SetName));
		if  (pObject && pSet)
		{
			// Add object to light set
			pSet->AddWholeObjectToSet(pObject);
		}
		//If the object is not found we need to ask the user to give us a replacement
		else
		{
			if (pObject == NULL)
			{   
				nameObject* newObjectLink;
				DBG_WARNING1("Object named %s was not found", i_ObjectName.GetString().c_str());
				//#ifdef _MANAGED
				newObjectLink = ltstReAttachUtil::GetObjByName(i_ObjectName.GetString(), true);
				//#endif
				if( newObjectLink != NULL )
				{
					nameString newObjectName = newObjectLink->GetName();
					// Find named object
					ltstLightSetObject* newObject = get_if(l_Objects, nameobj_search<ltstLightSetObject>(newObjectName));
					
					if  (newObject && pSet)
						pSet->AddWholeObjectToSet(newObject);
			
				}
				
			}
			
		}

		ltstLightSetInterestMgr::NotifyInterestsChanged();
	}

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveObjectFromLightSet(const nameString& i_SetName, 
								   const nameString& i_ObjectName)
	{
		// Find named object
		ltstLightSetObject* pObject = get_if(l_Objects, nameobj_search<ltstLightSetObject>(i_ObjectName));
		// Find named light set
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_SetName));
		if  (pObject && pSet)
		{
			// Remove object from light set
			pSet->RemoveWholeObjectFromSet(pObject);
		}
		ltstLightSetInterestMgr::NotifyInterestsChanged();
	}

	//--------------------------------------------------------------------
	// Return number of nodes within object
	//--------------------------------------------------------------------
	int GetNumFragmentNodeNames(const nameString& i_ObjectName)
	{
		// Find named object
		ltstLightSetObject* pObject = get_if(l_Objects, nameobj_search<ltstLightSetObject>(i_ObjectName));
		if  (pObject)
		{
			return pObject->GetNumNodes();
		}
		return 0;
	}

	//--------------------------------------------------------------------
	// Get fragment nodes for given object
	//--------------------------------------------------------------------
	void  GetFragmentNodeNames(const nameString& i_ObjectName, 
							   std::vector<std::string>& o_NodeNames)
	{
		// Find named object
		ltstLightSetObject* pObject = get_if(l_Objects, nameobj_search<ltstLightSetObject>(i_ObjectName));
		if  (pObject)
		{
			const int num_nodes = pObject->GetNumNodes();
			o_NodeNames.resize(num_nodes);
			for (int i=0; i<num_nodes; ++i)
			{
				o_NodeNames[i] = pObject->GetNodeName(i);
			}
		}
	}

	//--------------------------------------------------------------------
	//	Add node to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddNodeToLightSet(const nameString& i_SetName, 
							const nameString& i_ObjectName,
							int i_NodeIndex)
	{
		// Find named object
		ltstLightSetObject* pObject = get_if(l_Objects, nameobj_search<ltstLightSetObject>(i_ObjectName));
		// Find named light set
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_SetName));
		if  (pObject && pSet)
		{
			// Add object's node to light set
			pSet->AddNodeToSet(pObject, i_NodeIndex);
		}
		ltstLightSetInterestMgr::NotifyInterestsChanged();
	}

	//--------------------------------------------------------------------
	//	Remove node from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveNodeFromLightSet(const nameString& i_SetName, 
								 const nameString& i_ObjectName,
								 int i_NodeIndex)
	{
		// Find named object
		ltstLightSetObject* pObject = get_if(l_Objects, nameobj_search<ltstLightSetObject>(i_ObjectName));
		// Find named light set
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_SetName));
		if  (pObject && pSet)
		{
			// Remove object's node from light set
			pSet->RemoveNodeFromSet(pObject, i_NodeIndex);
		}
		ltstLightSetInterestMgr::NotifyInterestsChanged();

	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int GetNumLightSets()
	{
		return l_LightSets.size();
	}

	//--------------------------------------------------------------------
	//  Get names of light sets
	//--------------------------------------------------------------------
	void GetLightSetNames(std::vector<nameString> &o_Names)
	{
		std::vector<ltstLightSet*>::iterator it, end = l_LightSets.end();
		for (it = l_LightSets.begin(); it != end; ++it)
		{
			o_Names.push_back((*it)->m_Name);		
		}
	}

	//--------------------------------------------------------------------
	//  Get names of lights in given light set. Use empty
	//	i_SetName ("") to ask about "unassigned" lights.
	//--------------------------------------------------------------------
	void GetLightsInSet(const nameString& i_SetName, 
						std::vector<nameString> &o_LightNames)
	{
		// Handle "unassigned" light set
		if (i_SetName.IsEmpty())
		{
			std::vector<ltstLightSetLight*>::iterator it, end = l_Lights.end();
			for (it = l_Lights.begin(); it != end; ++it)
			{
				if ((*it)->m_pContainingLightSet == NULL)
					o_LightNames.push_back((*it)->m_pNameObj->GetName());
			}
		}
		else
		{
			// Find named light set
			ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_SetName));
			if  (pSet)
			{
				o_LightNames.resize(pSet->Lights().size());
				int i=0;
				std::vector<ltstLightSetLight*>::const_iterator it, end = pSet->Lights().end();
				for (it = pSet->Lights().begin(); it != end; ++it)
				{
					o_LightNames[i++] = (*it)->m_pNameObj->GetName();
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//  Get names of objects lit by light set.
	//--------------------------------------------------------------------
	void GetObjectsInSet(const nameString& i_SetName, 
						 std::vector<nameString> &o_ObjectNames)
	{
		// Find named light set
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_SetName));
		if  (pSet)
		{
			pSet->GetObjectsInSet(o_ObjectNames);
		}

		// Note: no handling of "unassigned" set with objects	
	}

	
	//----------------------------------------------------------------------------
	// Get indices of fragments in this object that are individually lit by this
	// light set. If the whole object is lit, then the list will
	// be returned empty.
	//----------------------------------------------------------------------------
	void GetLitFragmentIndices(const nameString& i_SetName, 
							   const nameString& i_ObjectName,
							   std::vector<int>& o_LitFragmentIndices)
	{
		// Find named object
		ltstLightSetObject* pObject = get_if(l_Objects, nameobj_search<ltstLightSetObject>(i_ObjectName));
		// Find named light set
		ltstLightSet* pSet = get_if(l_LightSets, name_search<ltstLightSet>(i_SetName));
		if  (pObject && pSet)
		{
			pSet->GetLitFragmentIndices(pObject, o_LitFragmentIndices);
		}
	}

	//--------------------------------------------------------------------
	//  Get list of all names of objects registered in the manager
	//--------------------------------------------------------------------
	void GetAllObjects(std::vector<nameString> &o_ObjectNames)
	{
		std::vector<ltstLightSetObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			o_ObjectNames.push_back((*it)->m_pNameObj->GetName());
		}
	}

	//--------------------------------------------------------------------
	//  Get list of all names of lights registered in the manager
	//--------------------------------------------------------------------
	void GetAllLights(std::vector<nameString> &o_LightNames)
	{
		std::vector<ltstLightSetLight*>::iterator it, end = l_Lights.end();
		for (it = l_Lights.begin(); it != end; ++it)
		{
			o_LightNames.push_back((*it)->m_pNameObj->GetName());
		}
	}

	
	//--------------------------------------------------------------------
	//	Gets name of light set containing the given light.
	//	Returns true if light is contained in a set and then
	//		sets o_SetName to hold the name of the set.
	//--------------------------------------------------------------------
	bool  GetSetNameFromLight(const nameString& i_LightName, 
							  nameString& o_SetName)
	{
		// Find named light
		ltstLightSetLight* pLight = get_if(l_Lights, nameobj_search<ltstLightSetLight>(i_LightName));
		if  (pLight)
		{
			if (pLight->m_pContainingLightSet != NULL)
			{
				o_SetName = pLight->m_pContainingLightSet->m_Name;
				return true;
			}
		}
		return false;
	}

	//--------------------------------------------------------------------
	//  Access to whole data as one structure 
	//--------------------------------------------------------------------
	ltstLightSetsData GetData()
	{
		ltstLightSetsData sets_data;
		sets_data.m_GlobalAmbient = GetSceneAmbientLight();
		sets_data.m_LightSets.resize(l_LightSets.size());

		int si = 0;
		std::vector<ltstLightSet*>::iterator it, end = l_LightSets.end();
		for (it = l_LightSets.begin(); it != end; ++it)
		{
			ltstLightSet* pSet = (*it);

			ltstLightSetData &data = sets_data.m_LightSets[si++];
			pSet->GetData(data);
		}

		return sets_data;
	}
	void SetData(const ltstLightSetsData &i_Data)
	{
		ltstLightSetInterestMgr::SetDisableNotify(true);
		ClearAllLightSets();

		SetSceneAmbientLight(i_Data.m_GlobalAmbient);

		std::vector<ltstLightSetData>::const_iterator it, end = i_Data.m_LightSets.end();
		for (it = i_Data.m_LightSets.begin(); it != end; ++it)
		{
			const ltstLightSetData &data = (*it);

			CreateLightSet(data.m_Name);
			SetLightSetAmbientLight(data.m_Name, data.m_AmbientLight);

			const int num_lights = data.m_Lights.size();
			for (int i=0; i<num_lights; ++i)
			{
				AddLightToSet(data.m_Name, data.m_Lights[i]);
			}

			const int num_objects = data.m_Objects.size();
			for (int i=0; i<num_objects; ++i)
			{
				const ltstLightSetObjectData &obj_data = data.m_Objects[i];
				if (obj_data.m_LitFragmentIndices.empty())
				{
					// Add whole object
					AddObjectToLightSet(data.m_Name, obj_data.m_Name);
				}
				else
				{
					// Add just the lit nodes
					const int num_nodes = GetNumFragmentNodeNames(obj_data.m_Name);
					bool bWarnedOutOfRange = false;
					for (int l=0; l<obj_data.m_LitFragmentIndices.size(); ++l)
					{
						if (obj_data.m_LitFragmentIndices[l] < num_nodes)
							AddNodeToLightSet(data.m_Name, obj_data.m_Name, obj_data.m_LitFragmentIndices[l]);
						else if (!bWarnedOutOfRange)
						{
							// Warn user that the fragment indices are out of range on this model
							std::string message = "Number of surfaces has changed in model, light set lighting may not match anymore.\n";
							message += std::string("Object: ") + obj_data.m_Name.GetString();
							message += std::string(" LightSet: ") + data.m_Name.GetString();
							guiMessageBox::Show(message.c_str(), "Light Set Surface Index out of range");
							bWarnedOutOfRange = true;
						}

					}
				}
			}
		}
		ltstLightSetInterestMgr::SetDisableNotify(false);
		ltstLightSetInterestMgr::NotifyInterestsChanged();
	}

	//--------------------------------------------------------------------
	//  Append this data to given light sets, merging into existing
	//	sets of the same name
	//--------------------------------------------------------------------
	void MergeData(const ltstLightSetsData &i_Data)
	{
		ltstLightSetInterestMgr::SetDisableNotify(true);

		std::vector<ltstLightSetData>::const_iterator it, end = i_Data.m_LightSets.end();
		for (it = i_Data.m_LightSets.begin(); it != end; ++it)
		{
			const ltstLightSetData &data = (*it);

			// Only create light set if name does not already exist.
			std::vector<ltstLightSet*>::iterator set_it = 
				std::find_if(l_LightSets.begin(), l_LightSets.end(), name_search<ltstLightSet>(data.m_Name));
			if  (set_it == l_LightSets.end())
			{
				CreateLightSet(data.m_Name);
				SetLightSetAmbientLight(data.m_Name, data.m_AmbientLight);
			}

			const int num_lights = data.m_Lights.size();
			for (int i=0; i<num_lights; ++i)
			{
				AddLightToSet(data.m_Name, data.m_Lights[i]);
			}

			const int num_objects = data.m_Objects.size();
			for (int i=0; i<num_objects; ++i)
			{
				const ltstLightSetObjectData &obj_data = data.m_Objects[i];
				if (obj_data.m_LitFragmentIndices.empty())
				{
					// Add whole object
					AddObjectToLightSet(data.m_Name, obj_data.m_Name);
				}
				else
				{
					// Add just the lit nodes
					for (int l=0; l<obj_data.m_LitFragmentIndices.size(); ++l)
					{
						AddNodeToLightSet(data.m_Name, obj_data.m_Name, obj_data.m_LitFragmentIndices[l]);
					}
				}
			}
		}
		ltstLightSetInterestMgr::SetDisableNotify(false);
		ltstLightSetInterestMgr::NotifyInterestsChanged();
	}

	//--------------------------------------------------------------------
	//	RegisterLightSetInterest() - add a LightSet interest 
	//--------------------------------------------------------------------
	void RegisterLightSetInterest( ltstLightSetInterest* i_pInterest )
	{
		ltstLightSetInterestMgr::RegisterLightSetInterest( i_pInterest );
	}

	//--------------------------------------------------------------------
	//	UnRegisterLightSetInterest() - remove a LightSet interest 
	//
	//	Note: this will NOT delete the LightSet interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterLightSetInterest( ltstLightSetInterest* i_pInterest )
	{
		ltstLightSetInterestMgr::UnRegisterLightSetInterest( i_pInterest );
	}

}	// end of namespace
