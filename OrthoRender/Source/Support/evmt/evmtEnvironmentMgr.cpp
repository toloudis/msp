/*****************************************************************************
**	evmtEnvironmentMgr.cpp
**
**	Keeps track of objects to be grouped with common environment settings
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/


#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/evmt/evmtEnvironmentInterest.hpp"

#include "Support/fsys/fsysFileList.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceTrackerData.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/api3d/api3dObjectSingle.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/pick3d/pick3dPickObject.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"


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


// Note: It may have been easier to use std::map here, but I wasn't sure if
// we could guarantee that the names were unique.
	std::vector<evmtObject*> l_Objects;
	std::vector<evmtEnvironment*> l_Environments;

	nameString l_DefaultEnvironmentName("Default");

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

	void update_diffusemap(evmtObject* i_pObject)
	{
		matTexture* tex = i_pObject->m_ContainingEnvironment->m_DiffuseMap;
		i_pObject->m_pObject->SetRenderStateDiffuseEnv(tex, 
			i_pObject->m_ContainingEnvironment->m_DiffuseFactor,
			i_pObject->m_ContainingEnvironment->m_DiffuseAngle);
	}

	void update_specularmap(evmtObject* i_pObject)
	{
		matTexture* tex = i_pObject->m_ContainingEnvironment->m_SpecularMap;
		i_pObject->m_pObject->SetRenderStateSpecularEnv(tex, 
			i_pObject->m_ContainingEnvironment->m_SpecularFactor,
			i_pObject->m_ContainingEnvironment->m_SpecularAngle);
	}
	
	void update_env(evmtObject* i_pObject)
	{
		if (i_pObject->m_ContainingEnvironment != NULL)
		{
			matTexture* texd = i_pObject->m_ContainingEnvironment->m_DiffuseMap;
			i_pObject->m_pObject->SetRenderStateDiffuseEnv(texd, 
				i_pObject->m_ContainingEnvironment->m_DiffuseFactor,
				i_pObject->m_ContainingEnvironment->m_DiffuseAngle);
			matTexture* texs = i_pObject->m_ContainingEnvironment->m_SpecularMap;
			i_pObject->m_pObject->SetRenderStateSpecularEnv(texs, 
				i_pObject->m_ContainingEnvironment->m_SpecularFactor,
				i_pObject->m_ContainingEnvironment->m_SpecularAngle);
		}
		else
		{
			i_pObject->m_pObject->SetRenderStateDiffuseEnv(NULL, 0, 0);
			i_pObject->m_pObject->SetRenderStateSpecularEnv(NULL, 0, 0);
		}
	}

	void disconnect_environment(evmtEnvironment* i_pEnvironment)
	{
		// Remove this environment from the objects' containing environment list
		std::vector<evmtObject*>::iterator obj_it, obj_end = i_pEnvironment->m_Objects.end();
		for (obj_it = i_pEnvironment->m_Objects.begin(); obj_it != obj_end; ++obj_it)
		{
			DBG_ASSERT0(((*obj_it)->m_ContainingEnvironment == i_pEnvironment), 
				"object belongs to wrong environment");
			(*obj_it)->m_ContainingEnvironment = NULL;

			// update state for this object
			update_env(*obj_it);
		}
	}

}	// end of namespace


evmtEnvironment::~evmtEnvironment()
{
	if (m_DiffuseMap != NULL)
		matTextureMgr::ReleaseTexture(m_DiffuseMap);
	if (m_SpecularMap != NULL)
		matTextureMgr::ReleaseTexture(m_SpecularMap);
}

void evmtEnvironment::SetName(const nameString& i_Name)
{
	m_Name = i_Name;
}

void evmtEnvironment::SetDiffuseFactor(float i_Factor)
{
	m_DiffuseFactor = i_Factor;

	// Now we have to go through all of the objects in this environment
	// and have them update 
	std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
	for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
	{
		update_diffusemap(*obj_it);
	}
}
void evmtEnvironment::SetSpecularFactor(float i_Factor)
{
	m_SpecularFactor = i_Factor;

	// Now we have to go through all of the objects in this environment
	// and have them update 
	std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
	for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
	{
		update_specularmap(*obj_it);
	}
}
float evmtEnvironment::GetDiffuseFactor()
{
	return m_DiffuseFactor;
}
float evmtEnvironment::GetSpecularFactor()
{
	return m_SpecularFactor;
}

void evmtEnvironment::SetDiffuseAngle(float i_Angle)
{
	m_DiffuseAngle = i_Angle;

	// Now we have to go through all of the objects in this environment
	// and have them update 
	std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
	for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
	{
		update_diffusemap(*obj_it);
	}
}
float evmtEnvironment::GetDiffuseAngle()
{
	return m_DiffuseAngle;
}
void evmtEnvironment::SetSpecularAngle(float i_Angle)
{
	m_SpecularAngle = i_Angle;

	// Now we have to go through all of the objects in this environment
	// and have them update 
	std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
	for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
	{
		update_specularmap(*obj_it);
	}
}
float evmtEnvironment::GetSpecularAngle()
{
	return m_SpecularAngle;
}

void evmtEnvironment::SetDiffuseMap(const itString& i_FileName, fsysFileList& i_FileList)
{
	if (m_DiffuseMapName != i_FileName)
	{
		if (m_DiffuseMap != NULL)
			matTextureMgr::ReleaseTexture(m_DiffuseMap);

		if (i_FileName.GetLength() == 0)
		{
			m_DiffuseMap = NULL;
		}
		else
		{
			//	get the file path
			//
			fsLocator locator;
			i_FileList.GetFilePath(i_FileName, locator);
			locator.Push(i_FileName);

			m_DiffuseMap = matTextureMgr::LoadTexture(locator);
		}
		m_DiffuseMapName = i_FileName;

		// Now we have to go through all of the objects in this environment
		// and have them update 
		std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
		for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
		{
			update_diffusemap(*obj_it);
		}
	}
}
void evmtEnvironment::SetSpecularMap(const itString& i_FileName, fsysFileList& i_FileList)
{
	if (m_SpecularMapName != i_FileName)
	{
		if (m_SpecularMap != NULL)
			matTextureMgr::ReleaseTexture(m_SpecularMap);

		if (i_FileName.GetLength() == 0)
		{
			m_SpecularMap = NULL;
		}
		else
		{
			//	get the file path
			//
			fsLocator locator;
			i_FileList.GetFilePath(i_FileName, locator);
			locator.Push(i_FileName);

			m_SpecularMap = matTextureMgr::LoadTexture(locator);
		}
		m_SpecularMapName = i_FileName;

		// Now we have to go through all of the objects in this environment
		// and have them update 
		std::vector<evmtObject*>::iterator obj_it, end = m_Objects.end();
		for (obj_it = m_Objects.begin(); obj_it != end; ++obj_it)
		{
			update_specularmap(*obj_it);
		}
	}
}
itString evmtEnvironment::GetDiffuseMapName()
{
	return m_DiffuseMapName;
}
itString evmtEnvironment::GetSpecularMapName()
{
	return m_SpecularMapName;
}


namespace evmtEnvironmentMgr
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CreateDefaultEnvironment()
	{
		// Check for unique name, print warning
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(l_DefaultEnvironmentName));
		if (set_it != l_Environments.end())
		{
			DBG_WARNING0("CreateDefaultEnvironment: Default Environment already exists");
			return;
		}

		evmtEnvironment* pEnv = CreateEnvironment();
		pEnv->m_Name = l_DefaultEnvironmentName;
		nameMgr::RegisterName( pEnv->m_Name );
	}

	void Initialize()
	{
		// create the initial (default) environment
//		CreateDefaultEnvironment();

	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize()
	{
		// Clean up
		envSTLHelpers::DeleteContainer(l_Objects);
		envSTLHelpers::DeleteContainer(l_Environments);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  AddObjectToEnvironment(evmtEnvironment* i_Env, 
								evmtObject* i_Object)
	{
		// remove from existing env't
		if (i_Object->m_ContainingEnvironment != NULL)
		{
			evmtEnvironment* pCurSet = i_Object->m_ContainingEnvironment;
			// Remove object from environment
			envSTLHelpers::RemoveOneValue(pCurSet->m_Objects, i_Object);
		}

		// Add object to environment
		i_Env->m_Objects.push_back(i_Object);
		i_Object->m_ContainingEnvironment = i_Env;

		// update state for this object
		update_env(i_Object);
	}

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given environment
	//--------------------------------------------------------------------
	void  AddObjectToEnvironment(evmtEnvironment* i_Environment,
								const nameString& i_ObjectName)
	{
		// Find named object
		std::vector<evmtObject*>::iterator obj_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobj_search<evmtObject>(i_ObjectName));
		if  (obj_it != l_Objects.end())
		{
			AddObjectToEnvironment(i_Environment, *obj_it);
		}
		notify_interests_changed();
	}

	//--------------------------------------------------------------------
	//  Add named object to list of things that can be lit by environments
	//--------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
				   api3dObjectSingle* i_pObject)
	{
		l_Objects.push_back(new evmtObject(i_pNameObj,i_pObject));

		//// Find named environment
		//std::vector<evmtEnvironment*>::iterator set_it = 
		//	std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(l_DefaultEnvironmentName));
		//if (set_it != l_Environments.end())
		//	AddObjectToEnvironment(*set_it, i_pNameObj->GetName());

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
		std::vector<evmtObject*>::iterator obj_it, end = l_Objects.end();
		for (obj_it = l_Objects.begin(); obj_it != end; ++obj_it)
		{
			evmtObject* pObject = (*obj_it);

			if (i_pNameObj == pObject->m_pNameObj)
			{
				//  Remove from containing environments
				if (pObject->m_ContainingEnvironment != NULL)
                    envSTLHelpers::RemoveOneValue(pObject->m_ContainingEnvironment->m_Objects, pObject);

				// Moved this outside the loop to make VS2005 happy:
				// Delete object structure, remove from list
				//delete pObject;
				//l_Objects.erase(obj_it);

				break;
			}
		}

		// If the object was found, delete the object and remove the 
		// iterator from the container
		if (obj_it != end)
		{
			evmtObject* pObject = (*obj_it);
			l_Objects.erase(obj_it);
			delete pObject;
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
		std::vector<evmtObject*>::iterator obj1_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobjptr_search<evmtObject>(i_pOldNameObj));
		std::vector<evmtObject*>::iterator obj2_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobjptr_search<evmtObject>(i_pNewNameObj));

		if (obj1_it != l_Objects.end() && obj2_it != l_Objects.end())
		{
			evmtEnvironment* pSet = (*obj1_it)->m_ContainingEnvironment;

			// if this containing set is the default environment, then 
			// our new object already is in that set. So, we don't need to 
			// add it again...
			if ((*obj2_it)->m_ContainingEnvironment != pSet)
			{
				// remove from existing env't
				if ((*obj2_it)->m_ContainingEnvironment != NULL)
				{
					evmtEnvironment* pCurSet = (*obj2_it)->m_ContainingEnvironment;
					// Remove object from environment
					envSTLHelpers::RemoveOneValue(pCurSet->m_Objects, (*obj2_it));
				}

				// Add new object to environment
				if (pSet != NULL)
					pSet->m_Objects.push_back(*obj2_it);
				(*obj2_it)->m_ContainingEnvironment = pSet;

				// update state for this object
				update_env(*obj2_it);
			}
		}
	}

	//--------------------------------------------------------------------
	//	Select the object with the given name in the 3D scene
	//--------------------------------------------------------------------
/*	void SelectObject(const nameString& i_Name)
	{
		// Find named object
		std::vector<evmtObject*>::iterator ob_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobj_search<evmtObject>(i_Name));
		if  (ob_it != l_Objects.end())
		{
			evmtObject* pObject = (*ob_it);

			// See if we can cast the object to an object that can be selected
			// with the sel3dMgr
			pick3dPickObject *pPickObject = dynamic_cast<pick3dPickObject*>(pObject->m_pNameObj);
			if (pPickObject)
			{
				sel3dMgr::CreateUndoOperation();
				sel3dMgr::Select(pPickObject);
			}
		}
	}
*/
	//--------------------------------------------------------------------
	//	Notify the environment manager that the name of this object has changed
	//--------------------------------------------------------------------
	void ObjectRenamed(nameObject* i_pNameObj)
	{
		notify_interests_renamed();
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
	evmtEnvironment* CreateEnvironment()
	{
		DBG_LOG1("CreateEnvironment: before, num envs = %d", l_Environments.size());
		evmtEnvironment *pEnv = new evmtEnvironment();
		l_Environments.push_back(pEnv);
		notify_interests_added();
		DBG_LOG1("CreateEnvironment: after, num envs = %d", l_Environments.size());
		return pEnv;
	}

	//--------------------------------------------------------------------
	//	Destroy named environment, all objects return to default environment
	//--------------------------------------------------------------------
/*
void  DeleteEnvironment(const nameString& i_Name)
	{
		// move all objects into default
		std::vector<nameString> objNames;
		GetObjectsInSet(i_Name, objNames);
		std::vector<nameString>::iterator it, end = objNames.end();
		for (it = objNames.begin(); it != end; ++it)
		{
			AddObjectToEnvironment(l_DefaultEnvironmentName, (*it));
		}

		// Find named environment and delete
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_Name));
		if  (set_it != l_Environments.end())
		{
			evmtEnvironment *pSet = (*set_it);
			l_Environments.erase(set_it);
			delete pSet;
		}

		notify_interests_added();
	}
*/
	void  DeleteEnvironment(evmtEnvironment* i_Env)
	{
		//// Find default environment by name
		//std::vector<evmtEnvironment*>::iterator set_it = 
		//	std::find_if(l_Environments.begin(), l_Environments.end(), 
		//		name_search<evmtEnvironment>(l_DefaultEnvironmentName));

		//// move all objects into default
		//std::vector<evmtObject*>::iterator it, end = i_Env->m_Objects.end();
		//for (it = i_Env->m_Objects.begin(); it != end; ++it)
		//{
		//	AddObjectToEnvironment((*set_it), (*it));
		//}

		// Make sure the env is in the list, and delete
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find(l_Environments.begin(), l_Environments.end(), i_Env);
		if  (set_it != l_Environments.end())
		{
			evmtEnvironment *pSet = (*set_it);
			DBG_ASSERT0(pSet == i_Env, "DeleteEnvironment: bad env match with list.");
			l_Environments.erase(set_it);
			delete pSet;
		}

		notify_interests_added();
	}

	//--------------------------------------------------------------------
	//	Destroy named environment, using std::string
	//--------------------------------------------------------------------
/*
void  DeleteEnvironment(const std::string& i_Name)
	{
		std::vector<evmtEnvironment*>::iterator it, end = l_Environments.end();
		for (it = l_Environments.begin(); it != end; ++it)
		{
			evmtEnvironment *pSet = (*it);
			if (pSet->m_Name.GetString() == i_Name)
			{
				DeleteEnvironment(pSet->m_Name);
				break;
			}
		}
	}
*/

	//--------------------------------------------------------------------
	// Remove all environments (preparing for a new scene)
	//--------------------------------------------------------------------
	void ClearAllEnvironments()
	{
		// Disconnect all environments before destroying
		std::vector<evmtEnvironment*>::iterator it, end = l_Environments.end();
		for (it = l_Environments.begin(); it != end; ++it)
		{
			evmtEnvironment *pSet = (*it);

			disconnect_environment(pSet);
		}

		envSTLHelpers::DeleteContainer(l_Environments);

		notify_interests_changed();
	}

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given environment
	//--------------------------------------------------------------------
	void  RemoveObjectFromEnvironment(evmtEnvironment* i_Environment,
								   const nameString& i_ObjectName)
	{
		// Find named object
		std::vector<evmtObject*>::iterator obj_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobj_search<evmtObject>(i_ObjectName));
		if  (obj_it != l_Objects.end())
		{
			// Remove object from environment
			envSTLHelpers::RemoveOneValue(i_Environment->m_Objects, (*obj_it));
			evmtEnvironment* pContainingEnvironment = (*obj_it)->m_ContainingEnvironment;
			// TODO - why is this out of sync -- DBG_ASSERT0((pContainingEnvironment == i_Environment), "object is in wrong environment");
			(*obj_it)->m_ContainingEnvironment = NULL;

			// update state for this object
			update_env(*obj_it);
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
		// Handle "unassigned" environment
		if (i_SetName.IsEmpty())
		{
			std::vector<evmtObject*>::iterator it, end = l_Objects.end();
			for (it = l_Objects.begin(); it != end; ++it)
			{
				// If containing environment is empty, this object is unassigned
				if ((*it)->m_ContainingEnvironment == NULL)
					o_ObjectNames.push_back((*it)->m_pNameObj->GetName());
			}
		}
		else
		{
			// Find named environment
			std::vector<evmtEnvironment*>::iterator set_it = 
				std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
			if  (set_it != l_Environments.end())
			{
				evmtEnvironment *pSet = (*set_it);

				o_ObjectNames.resize(pSet->m_Objects.size());
				int i=0;
				std::vector<evmtObject*>::iterator it, end = pSet->m_Objects.end();
				for (it = pSet->m_Objects.begin(); it != end; ++it)
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
	//	RegisterEnvironmentInterest() - add a Environment interest 
	//--------------------------------------------------------------------
	void RegisterEnvironmentInterest( evmtEnvironmentInterest* i_pInterest )
	{
		DBG_ASSERT0( i_pInterest != 0, "Cannot register a NULL Environment Interest" );
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

	void SetDiffuseMap(const nameString& i_SetName, const itString& i_FileName, fsysFileList& i_FileList)
	{
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
		if  (set_it != l_Environments.end())
		{
			evmtEnvironment *pSet = (*set_it);
			pSet->SetDiffuseMap(i_FileName, i_FileList);
		}
	}
	void SetSpecularMap(const nameString& i_SetName, const itString& i_FileName, fsysFileList& i_FileList)
	{
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
		if  (set_it != l_Environments.end())
		{
			evmtEnvironment *pSet = (*set_it);
			pSet->SetSpecularMap(i_FileName, i_FileList);
		}
	}
	itString GetDiffuseMapName(const nameString& i_SetName)
	{
		itString retval;
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
		if  (set_it != l_Environments.end())
		{
			retval = (*set_it)->m_DiffuseMapName;
		}
		return retval;
	}
	itString GetSpecularMapName(const nameString& i_SetName)
	{
		itString retval;
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
		if  (set_it != l_Environments.end())
		{
			retval = (*set_it)->m_SpecularMapName;
		}
		return retval;
	}
	void SetDiffuseFactor(const nameString& i_SetName, float i_Factor)
	{
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
		if  (set_it != l_Environments.end())
		{
			evmtEnvironment *pSet = (*set_it);
			pSet->SetDiffuseFactor(i_Factor);
		}
	}
	void SetSpecularFactor(const nameString& i_SetName, float i_Factor)
	{
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
		if  (set_it != l_Environments.end())
		{
			evmtEnvironment *pSet = (*set_it);
			pSet->SetSpecularFactor(i_Factor);
		}
	}
	void SetDiffuseAngle(const nameString& i_SetName, float i_Angle)
	{
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
		if  (set_it != l_Environments.end())
		{
			evmtEnvironment *pSet = (*set_it);
			pSet->SetDiffuseAngle(i_Angle);
		}
	}
	float GetDiffuseAngle(const nameString& i_SetName)
	{
		float retval = 0;
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
		if  (set_it != l_Environments.end())
		{
			retval = (*set_it)->m_DiffuseAngle;
		}
		return retval;
	}
	void SetSpecularAngle(const nameString& i_SetName, float i_Angle)
	{
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
		if  (set_it != l_Environments.end())
		{
			evmtEnvironment *pSet = (*set_it);
			pSet->SetSpecularAngle(i_Angle);
		}
	}
	float GetSpecularAngle(const nameString& i_SetName)
	{
		float retval = 0;
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
		if  (set_it != l_Environments.end())
		{
			retval = (*set_it)->m_SpecularAngle;
		}
		return retval;
	}

	float GetDiffuseFactor(const nameString& i_SetName)
	{
		float retval = 0;
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
		if  (set_it != l_Environments.end())
		{
			retval = (*set_it)->m_DiffuseFactor;
		}
		return retval;
	}

	float GetSpecularFactor(const nameString& i_SetName)
	{
		float retval = 0;
		std::vector<evmtEnvironment*>::iterator set_it = 
			std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(i_SetName));
		if  (set_it != l_Environments.end())
		{
			retval = (*set_it)->m_SpecularFactor;
		}
		return retval;
	}



}	// end of namespace
