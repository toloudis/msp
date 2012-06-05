/*****************************************************************************
**	evmtEnvironmentMgr.cpp
**
**	Keeps track of objects to be grouped with common environment settings
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/evmt/evmtEnvironmentMgr.hpp"

#include "Support/evmt/evmtEnvironment.hpp"
#include "Support/evmt/evmtEnvironmentInterest.hpp"
#include "Support/evmt/GUI/evmtReAttachUtil.hpp"
#include "Support/evmt/private/evmtEnvironmentObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameObject.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	std::vector<evmtEnvironmentInterest*>	l_InterestList;

	bool l_bDisableNotify = false;
	void notify_interests_changed()
	{
		if (l_bDisableNotify) return;
		std::vector<evmtEnvironmentInterest*>::iterator it, end = l_InterestList.end();
		for (it  = l_InterestList.begin(); it != end; ++it)
		{
			(*it)->DataChanged();
		}
	}
	void notify_interests_added()
	{
		if (l_bDisableNotify) return;
		std::vector<evmtEnvironmentInterest*>::iterator it, end = l_InterestList.end();
		for (it  = l_InterestList.begin(); it != end; ++it)
		{
			(*it)->ObjectAdded();
		}
	}
	void notify_interests_removed(const nameString& i_Name)
	{
		if (l_bDisableNotify) return;
		std::vector<evmtEnvironmentInterest*>::iterator it, end = l_InterestList.end();
		for (it  = l_InterestList.begin(); it != end; ++it)
		{
			(*it)->ObjectRemoved(i_Name);
		}
	}
	void notify_interests_renamed()
	{
		if (l_bDisableNotify) return;
		std::vector<evmtEnvironmentInterest*>::iterator it, end = l_InterestList.end();
		for (it  = l_InterestList.begin(); it != end; ++it)
		{
			(*it)->ObjectRenamed();
		}
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

// Note: It may have been easier to use std::map here, but I wasn't sure if
// we could guarantee that the names were unique.
	std::vector<evmtObject*> l_Objects;
	std::vector<evmtEnvironment*> l_Environments;
	evmtEnvironment* l_DefaultEnvironment;
	evmtEnvironment* l_SwlEnvironment;
	
	void disconnect_environment(evmtEnvironment* i_pEnvironment)
	{
		i_pEnvironment->Disconnect();
	}

	// Software lighting variable
	bool l_bSwlEnable = false;
}	// end of namespace

namespace evmtEnvironmentMgr
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize()
	{
//		l_DefaultEnvironment = new evmtEnvironment();
		l_DefaultEnvironment = CreateEnvironment();
		l_SwlEnvironment = CreateEnvironment(true);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize()
	{
		// Clean up
		envSTLHelpers::DeleteContainer(l_Objects);
		envSTLHelpers::DeleteContainer(l_Environments);
		l_DefaultEnvironment = NULL;
		l_SwlEnvironment = NULL;
//		delete l_DefaultEnvironment;
	}

	//--------------------------------------------------------------------
	//  Get the default environment.
	//--------------------------------------------------------------------
	evmtEnvironment* GetDefaultEnvironment()
	{
		return l_DefaultEnvironment;
	}
	std::string GetDefaultEnvironmentName()
	{
		return "Default";
	}

	//--------------------------------------------------------------------
	//  Get the swl environment.
	//--------------------------------------------------------------------
	evmtEnvironment* GetSwlEnvironment()
	{
		return l_SwlEnvironment;
	}
	std::string GetSwlEnvironmentName()
	{
		return "mental ray IBL";
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  AddObjectToEnvironment(evmtEnvironment* i_Env, 
								evmtObject* i_Object,
								bool i_bRemoveOriginEnv = true)
	{
		// remove from existing env't
		if (i_Object->m_ContainingEnvironment != NULL && i_bRemoveOriginEnv)
		{
			evmtEnvironment* pCurSet = i_Object->m_ContainingEnvironment;
			// Remove object from environment
			envSTLHelpers::RemoveOneValue(pCurSet->m_Objects, i_Object);
		}

		// Add object to environment
		i_Env->Add(i_Object);
	}

	void AddAllObjectToEnvironment(const nameString& i_EnvironmentName,
								   bool i_bRemoveOriginEnv)
	{
		evmtEnvironment* pSet = get_if(l_Environments, name_search<evmtEnvironment>(i_EnvironmentName));
		if (!pSet)
		{
			if ( i_EnvironmentName == l_DefaultEnvironment->m_Name )
				pSet = l_DefaultEnvironment;
			else if ( i_EnvironmentName == l_SwlEnvironment->m_Name )
				pSet = l_SwlEnvironment;
			else
				return;
		}

		for (int i = 0; i < l_Objects.size(); i++)
		{
			// remove from existing env't
			if (l_Objects[i]->m_ContainingEnvironment != NULL && i_bRemoveOriginEnv)
			{
				evmtEnvironment* pCurSet = l_Objects[i]->m_ContainingEnvironment;
				// Remove object from environment
				envSTLHelpers::RemoveOneValue(pCurSet->m_Objects, l_Objects[i]);
			}

			// Add object to environment
			pSet->Add(l_Objects[i]);
		}
	}

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given environment
	//--------------------------------------------------------------------
	void  AddObjectToEnvironment(const nameString& i_EnvironmentName,
								const nameString& i_ObjectName)
	{
		// Find named object
		evmtObject* pObject = get_if(l_Objects, nameobj_search<evmtObject>(i_ObjectName));
		// Find named environment
		evmtEnvironment* pSet = get_if(l_Environments, name_search<evmtEnvironment>(i_EnvironmentName));
		if  (pObject && pSet)
		{
			AddObjectToEnvironment(pSet, pObject);
		}
		//If the object is not found we need to ask the user to give us a replacement
		else
		{
			if (pObject == NULL)
			{   
				nameObject* newObjectLink;
				DBG_WARNING("Object named " <<  i_ObjectName.GetString().c_str() << " was not found");
 				newObjectLink = evmtReAttachUtil::GetObjByName(i_ObjectName.GetString());
				if( newObjectLink != NULL )
				{
					nameString newObjectName = newObjectLink->GetName();
					// Find named object
					evmtObject* newObject = get_if(l_Objects, nameobj_search<evmtObject>(newObjectName));
					
					if  (newObject && pSet)
						AddObjectToEnvironment(pSet, newObject);
				}
			}
		}

		notify_interests_changed();


	}

	//--------------------------------------------------------------------
	//  Add named object to list of things that can be lit by environments
	//--------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
				   api3dObjectSingle* i_pObject)
	{
		evmtObject* obj = new evmtObject(i_pNameObj, i_pObject);
		l_Objects.push_back(obj);

		AddObjectToEnvironment(GetDefaultEnvironment(), obj);
		AddObjectToEnvironment(GetSwlEnvironment(), obj, false);

		notify_interests_added();
	}

	//--------------------------------------------------------------------
	//	Remove object from manager (removing from all environments)
	//--------------------------------------------------------------------
	void RemoveObject(nameObject* i_pNameObj, 
				      api3dObjectSingle* i_pObject)
	{
		// A name search is not enough here, needs to use object pointer

		// Find named object
		evmtObject* pObject = get_if(l_Objects, nameobjptr_search<evmtObject>(i_pNameObj));
		if  (pObject)
		{
			//  Remove from containing environments
			if (pObject->m_ContainingEnvironment != NULL)
                pObject->m_ContainingEnvironment->Remove(pObject);
			GetSwlEnvironment()->Remove(pObject);

			envSTLHelpers::DeleteOneValue(l_Objects, pObject);
		}

		notify_interests_removed(i_pNameObj->GetName());
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
		evmtObject* pObject1 = get_if(l_Objects, nameobjptr_search<evmtObject>(i_pOldNameObj));
		evmtObject* pObject2 = get_if(l_Objects, nameobjptr_search<evmtObject>(i_pNewNameObj));
		if  (pObject1 && pObject2)
		{
			evmtEnvironment* pSet = pObject1->m_ContainingEnvironment;

			// if this containing set is the default environment, then 
			// our new object already is in that set. So, we don't need to 
			// add it again...
			if (pObject2->m_ContainingEnvironment != pSet)
			{
				// remove from existing env't
				if (pObject2->m_ContainingEnvironment != NULL)
				{
					evmtEnvironment* pCurSet = pObject2->m_ContainingEnvironment;
					pCurSet->Remove(pObject2);
				}

				// Add new object to environment
				if (pSet != NULL)
				{
					pSet->Add(pObject2);
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//	Notify the environment manager that the name of this object has changed
	//--------------------------------------------------------------------
	void ObjectRenamed(nameObject* i_pNameObj)
	{
		notify_interests_renamed();
	}

	//--------------------------------------------------------------------
	//	Test if name is environment or object that can be 
	//	added to an environment
	//--------------------------------------------------------------------
	bool  IsEnvironment(const nameString& i_Name)
	{
		// Find named environment
		evmtEnvironment* pSet = get_if(l_Environments, name_search<evmtEnvironment>(i_Name));
		return (pSet != NULL);
		
	}
	bool  IsObject(const nameString& i_Name)
	{
		// Find named object
		evmtObject* pObject = get_if(l_Objects, nameobj_search<evmtObject>(i_Name));
		return (pObject != NULL);
	}

	//--------------------------------------------------------------------
	// Returns true if non-empty and unique name for a environment.
	//--------------------------------------------------------------------
	bool IsValidEnvironmentName(const std::string &i_Name)
	{
		if (i_Name.empty()) return false;

		std::vector<evmtEnvironment*>::iterator it, end = l_Environments.end();
		for (it = l_Environments.begin(); it != end; ++it)
		{
			if ((*it)->m_Name.GetString() == i_Name)
				return false;
		}
		return true;
	}

	//--------------------------------------------------------------------
	//	Create a named environment
	//--------------------------------------------------------------------
	evmtEnvironment* CreateEnvironment(bool i_bIsSwl)
	{
		//DBG_LOG("CreateEnvironment: before, num envs = " << l_Environments.size());
		evmtEnvironment *pEnv = new evmtEnvironment(i_bIsSwl);
		l_Environments.push_back(pEnv);
//		notify_interests_added();
		//DBG_LOG("CreateEnvironment: after, num envs = " << l_Environments.size());
		return pEnv;
	}

	void  DeleteEnvironment(evmtEnvironment* i_Env)
	{
		// move all objects into default
		evmtEnvironment* def = GetDefaultEnvironment();
		evmtEnvironment* swldef = GetSwlEnvironment();
		if (i_Env != def && i_Env != swldef)
		{
			// make a discardable copy of the list so we don't erase elements of the list we are iterating.
			std::vector<evmtObject*> objList = i_Env->m_Objects;

			std::vector<evmtObject*>::iterator it, end = objList.end();
			for (it = objList.begin(); it != end; ++it)
			{
				AddObjectToEnvironment(def, (*it));
			}
		}

		// Make sure the env is in the list, and delete
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find(l_Environments.begin(), l_Environments.end(), i_Env);
		if  (set_it != l_Environments.end())
		{
			evmtEnvironment *pSet = (*set_it);
			DBG_ASSERT(pSet == i_Env, "DeleteEnvironment: bad env match with list.");
			l_Environments.erase(set_it);
			delete pSet;
		}
	}

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given environment
	//--------------------------------------------------------------------
	void  removeObjectFromEnvironment(evmtEnvironment* i_pEnvironment, evmtObject* i_pObject)
	{
		evmtEnvironment* pContainingEnvironment = i_pObject->m_ContainingEnvironment;
		DBG_ASSERT((pContainingEnvironment == i_pEnvironment), "object is in wrong environment");

		// Remove object from environment
		i_pEnvironment->Remove(i_pObject);

		// automatically move object into default!!
//		evmtEnvironment* def = GetDefaultEnvironment();
//		if (i_pEnvironment != def)
//		{
//			AddObjectToEnvironment(def, i_pObject);
//		}
	}


	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given environment
	//--------------------------------------------------------------------
	void  RemoveObjectFromEnvironment(const nameString& i_SetName,
								   const nameString& i_ObjectName)
	{
		// Find named object
		evmtObject* pObject = get_if(l_Objects, nameobj_search<evmtObject>(i_ObjectName));
		// Find named environment
		evmtEnvironment* pSet = get_if(l_Environments, name_search<evmtEnvironment>(i_SetName));
		if  (pObject && pSet)
		{
			removeObjectFromEnvironment(pSet, pObject);
		}
		notify_interests_changed();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int GetNumEnvironments()
	{
		return l_Environments.size();
	}

	//--------------------------------------------------------------------
	//  Get names of environments
	//--------------------------------------------------------------------
	void GetEnvironmentNames(std::vector<nameString> &o_Names)
	{
		std::vector<evmtEnvironment*>::iterator it, end = l_Environments.end();
		for (it = l_Environments.begin(); it != end; ++it)
		{
			o_Names.push_back((*it)->m_Name);		
		}
	}

	//--------------------------------------------------------------------
	//  Get names of objects lit by environment. Use empty
	//	i_SetName ("") to ask about "unassigned" objects.
	//--------------------------------------------------------------------
	void GetObjectsInSet(const nameString& i_SetName, 
						 std::vector<nameString> &o_ObjectNames)
	{
		// Find named environment
		evmtEnvironment* pSet = get_if(l_Environments, name_search<evmtEnvironment>(i_SetName));
		if  (pSet != NULL)
		{
			pSet->GetData(o_ObjectNames);
		}
	}

	//--------------------------------------------------------------------
	//  Get list of all names of objects registered in the manager
	//--------------------------------------------------------------------
	void GetAllObjects(std::vector<nameString> &o_ObjectNames)
	{
		std::vector<evmtObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			o_ObjectNames.push_back((*it)->m_pNameObj->GetName());
		}
	}
	
	//--------------------------------------------------------------------
	//	Gets name of environment containing the given object.
	//	Returns true if object is contained in a environment and then
	//		sets o_EnvironmentName to hold the name of the environment.
	//--------------------------------------------------------------------
	bool  GetEnvironmentNameFromObject(const nameString& i_ObjectName, 
							     nameString& o_EnvironmentName)
	{
		// Find named object
		std::vector<evmtObject*>::iterator obj_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobj_search<evmtObject>(i_ObjectName));
		if  (obj_it != l_Objects.end())
		{
			evmtObject* pObject = (*obj_it);
			if (pObject->m_ContainingEnvironment != NULL)
			{
				o_EnvironmentName = pObject->m_ContainingEnvironment->m_Name;
				return true;
			}
		}
		return false;
	}

	//--------------------------------------------------------------------
	//	ReloadTextures - reload the environment textures for diffuse and 
	//	specular maps
	//--------------------------------------------------------------------
	void ReloadTextures(const nameString& i_Name)
	{
		// Find named environment
		evmtEnvironment* pSet = get_if(l_Environments, name_search<evmtEnvironment>(i_Name));
		if (pSet)
		{
			// stop any render threads for texture manager changes
			gpxRenderControl::ConfirmSingleThread();

			matTextureMgr::ReloadTexture( pSet->GetDiffuseMapName() );
			matTextureMgr::ReloadTexture( pSet->GetSpecularMapName() );
		}
	}

	//--------------------------------------------------------------------
	//	RegisterEnvironmentInterest() - add a Environment interest 
	//--------------------------------------------------------------------
	void RegisterEnvironmentInterest( evmtEnvironmentInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Environment Interest" );
		l_InterestList.push_back( i_pInterest );
	}

	//--------------------------------------------------------------------
	//	UnRegisterEnvironmentInterest() - remove a Environment interest 
	//
	//	Note: this will NOT delete the Environment interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterEnvironmentInterest( evmtEnvironmentInterest* i_pInterest )
	{
		envSTLHelpers::RemoveOneValue( l_InterestList, i_pInterest );
	}


	//--------------------------------------------------------------------
	//	Remove all objects from given environment
	//--------------------------------------------------------------------
	void  ClearEnvironment(const nameString& i_Name)
	{
		// assumption: this is only called before re-filling the env with new data.
		// special behavior: never clear the default environment. (?)

		// Find named light set
		evmtEnvironment* pSet = get_if(l_Environments, name_search<evmtEnvironment>(i_Name));
		if (pSet /*&& (i_Name != l_DefaultEnvironmentName)*/)
		{
			disconnect_environment(pSet);
			pSet->m_Objects.clear();
//			std::vector<evmtObject*>::iterator obj_it, obj_end = pSet->m_Objects.end();
//			for (obj_it = pSet->m_Objects.begin(); obj_it != obj_end; ++obj_it)
//			{
//				removeObjectFromEnvironment(pSet, (*obj_it));
//			}
		}

		notify_interests_added();
	}

	//--------------------------------------------------------------------
	//	Get current software lighting flag
	//--------------------------------------------------------------------
	bool GetSoftwareLighting()
	{
		return l_bSwlEnable;
	}

	//--------------------------------------------------------------------
	//	Set current software lighting flag
	//--------------------------------------------------------------------
	void SetSoftwareLighting(bool i_bSwlEnable)
	{
		l_bSwlEnable = i_bSwlEnable;
		/*for (int i = 0; i < l_Objects.size(); i++)
		{
			l_Objects[i]->UpdateSwlData(l_bSwlEnable);
		}*/
	}

	//--------------------------------------------------------------------
	//	Reset all environments attribute
	//--------------------------------------------------------------------
	void UpdateAllEnvironments()
	{
		for (int i = 0; i < l_Environments.size(); i++)
		{
			l_Environments[i]->Update();
		}
	}
}	// end of namespace
