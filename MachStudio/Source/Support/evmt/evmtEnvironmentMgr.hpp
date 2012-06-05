/*****************************************************************************
**	evmtEnvironmentMgr.hpp
**
**	Keeps track of objects that can be grouped based on environment maps
**
**	By definition, an object can only belong to one environment. 
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EVMT_ENVIRONMENTMGR_HPP
#error evmtEnvironmentMgr.hpp multiply included
#endif
#define EVMT_ENVIRONMENTMGR_HPP

//#ifndef NAME_STRING_HPP
//#include "Core/name/nameString.hpp"
//#endif
//#ifndef FS_LOCATOR_HPP
//#include "Core/Fs/fsLocator.hpp"
//#endif 
//#ifndef MA_FLOATRGBA_HPP
//#include "Core/ma/maFloatRGBA.hpp"
//#endif

#include <string>
#include <vector>

//============================================================================
//	Forward References
//============================================================================
class api3dObjectSingle;
class evmtEnvironment;
class evmtEnvironmentInterest;
//class fsysFileList;
//class g3dLight;
class maFloatRGBA;
//class matTexture;
class nameObject;
class nameString;

//============================================================================
//============================================================================
namespace evmtEnvironmentMgr
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize();

//--------------------------------------------------------------------
// Systems should call these functions in order to submit
// and organize objects from different systems.
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	//  Get the default environment.
	//--------------------------------------------------------------------
	evmtEnvironment* GetDefaultEnvironment();
	std::string GetDefaultEnvironmentName();
	
	//--------------------------------------------------------------------
	//  Get the swl environment.
	//--------------------------------------------------------------------
	evmtEnvironment* GetSwlEnvironment();
	std::string GetSwlEnvironmentName();
	
	//--------------------------------------------------------------------
	//  Add named object to list of things that can be lit by environments
	//--------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
				    api3dObjectSingle* i_pObject);

	//--------------------------------------------------------------------
	//	Remove object from manager (removing from all environments)
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
	//	Select the object with the given name in the 3D scene
	//--------------------------------------------------------------------
//	void SelectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//	Notify the environment manager that the name of this object has changed
	//--------------------------------------------------------------------
	void ObjectRenamed(nameObject* i_pNameObj);

//--------------------------------------------------------------------
// The user interface can then manipulate the groupings with the
// following functions by using the names of the lights and objects
//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//	Test if name is environment or object that can be 
	//	added to an environment
	//--------------------------------------------------------------------
	bool  IsEnvironment(const nameString& i_Name);
	bool  IsObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	// Returns true if non-empty and unique name for a environment.
	//--------------------------------------------------------------------
	bool IsValidEnvironmentName(const std::string &i_Name);

	//--------------------------------------------------------------------
	//	Create an environment
	//--------------------------------------------------------------------
	evmtEnvironment* CreateEnvironment(bool i_bIsSwl = false);

	//--------------------------------------------------------------------
	//	Destroy named environment, all objects in this set become 
	//	"unassigned"
	//--------------------------------------------------------------------
	void  DeleteEnvironment(evmtEnvironment* i_Env);

	//--------------------------------------------------------------------
	// Set ambient light for the given environment. This is added to
	// the scene's ambient.
	//--------------------------------------------------------------------
	void SetEnvironmentAmbientLight(const nameString& i_SetName, 
								 const maFloatRGBA &i_AmbientLight);
	const maFloatRGBA& GetEnvironmentAmbientLight(const nameString& i_SetName);

	void AddAllObjectToEnvironment(const nameString& i_EnvironmentName,
								   bool i_bRemoveOriginEnv = true);

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given environment
	//--------------------------------------------------------------------
	void  AddObjectToEnvironment(const nameString& i_EnvironmentName,
							  const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given environment
	//--------------------------------------------------------------------
	void  RemoveObjectFromEnvironment(const nameString& i_EnvironmentName,
								   const nameString& i_ObjectName);

//--------------------------------------------------------------------
// These functions get the current state of the environment groupings
// in order to be displayed to the user.
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int GetNumEnvironments();

	//--------------------------------------------------------------------
	//  Get names of environments
	//--------------------------------------------------------------------
	void GetEnvironmentNames(std::vector<nameString> &o_Names);

	//--------------------------------------------------------------------
	//  Get names of objects lit by environment. Use empty
	//	i_SetName ("") to ask about "unassigned" objects.
	//--------------------------------------------------------------------
	void GetObjectsInSet(const nameString& i_SetName, 
						 std::vector<nameString> &o_ObjectNames);

	//--------------------------------------------------------------------
	//  Get list of all names of objects registered in the manager
	//--------------------------------------------------------------------
	void GetAllObjects(std::vector<nameString> &o_ObjectNames);

	//--------------------------------------------------------------------
	//	Gets name of environment containing the given object.
	//	Returns true if object is contained in a environment and then
	//		sets o_EnvironmentName to hold the name of the environment.
	//--------------------------------------------------------------------
	bool  GetEnvironmentNameFromObject(const nameString& i_ObjectName, 
							     nameString& o_EnvironmentName);

	//--------------------------------------------------------------------
	//	ReloadTextures - reload the environment textures for diffuse and 
	//	specular maps
	//--------------------------------------------------------------------
	void ReloadTextures(const nameString& i_Name);

//--------------------------------------------------------------------
//	data changed interest functions
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	//	RegisterEnvironmentInterest() - add a Environment interest 
	//--------------------------------------------------------------------
	void RegisterEnvironmentInterest( evmtEnvironmentInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterEnvironmentInterest() - remove a Environment interest 
	//
	//	Note: this will NOT delete the Environment interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterEnvironmentInterest( evmtEnvironmentInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	Remove all objects from given environment
	//--------------------------------------------------------------------
	void  ClearEnvironment(const nameString& i_Name);

	//--------------------------------------------------------------------
	//	Get current software lighting flag
	//--------------------------------------------------------------------
	bool GetSoftwareLighting();

	//--------------------------------------------------------------------
	//	Set current software lighting flag
	//--------------------------------------------------------------------
	void SetSoftwareLighting(bool i_bSwlEnable);

	//--------------------------------------------------------------------
	//	Reset all environments attribute
	//--------------------------------------------------------------------
	void UpdateAllEnvironments();
}	// end of namespace

