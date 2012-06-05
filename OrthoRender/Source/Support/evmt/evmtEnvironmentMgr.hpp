/*****************************************************************************
**	evmtEnvironmentMgr.hpp
**
**	Keeps track of objects that can be grouped based on environment maps
**
**	By definition, an object can only belong to one environment. 
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EVMT_ENVIRONMENTMGR_HPP
#error evmtEnvironmentMgr.hpp multiply included
#endif
#define EVMT_ENVIRONMENTMGR_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

#include <vector>


//============================================================================
//	Forward References
//============================================================================
class api3dObjectSingle;
class evmtEnvironment;
class evmtEnvironmentInterest;
class fsysFileList;
class g3dLight;
class maFloatRGBA;
class matTexture;
class nameObject;


//============================================================================
// A named 3d object that can be put in an environment
//============================================================================
class evmtObject
{
public:
	nameObject* m_pNameObj;
	api3dObjectSingle* m_pObject;
	evmtEnvironment* m_ContainingEnvironment;

public:
	evmtObject(nameObject* i_pNameObj, api3dObjectSingle* i_pObject)
	:	m_pNameObj(i_pNameObj), 
		m_pObject(i_pObject),
		m_ContainingEnvironment(NULL) 
	{};
};


//============================================================================
// An environment
//============================================================================
class evmtEnvironment
{
public:
	nameString m_Name;
	itString m_DiffuseMapName;
	matTexture* m_DiffuseMap;
	float m_DiffuseFactor;
	float m_DiffuseAngle;
	itString m_SpecularMapName;
	matTexture* m_SpecularMap;
	float m_SpecularFactor;
	float m_SpecularAngle;

	std::vector<evmtObject*> m_Objects;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	evmtEnvironment()
		: m_DiffuseAngle(0), m_SpecularAngle(0),
			m_DiffuseMap(NULL), m_SpecularMap(NULL),
			m_DiffuseFactor(1), m_SpecularFactor(1) {};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~evmtEnvironment();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetName(const nameString& i_Name);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetDiffuseFactor(float i_Factor);
	void SetSpecularFactor(float i_Factor);
	float GetDiffuseFactor();
	float GetSpecularFactor();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetDiffuseAngle(float i_Angle);
	float GetDiffuseAngle();
	void SetSpecularAngle(float i_Angle);
	float GetSpecularAngle();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetDiffuseMap(const itString& i_FileName, fsysFileList& i_FileList);
	void SetSpecularMap(const itString& i_FileName, fsysFileList& i_FileList);
	itString GetDiffuseMapName();
	itString GetSpecularMapName();
};


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
	// Returns true if non-empty and unique name for a environment.
	//--------------------------------------------------------------------
	bool IsValidEnvironmentName(const std::string &i_Name);

	//--------------------------------------------------------------------
	//	Create an environment
	//--------------------------------------------------------------------
	evmtEnvironment* CreateEnvironment();

	//--------------------------------------------------------------------
	//	Destroy named environment, all objects in this set become 
	//	"unassigned"
	//--------------------------------------------------------------------
	void  DeleteEnvironment(evmtEnvironment* i_Env);

	//--------------------------------------------------------------------
	//	Destroy named environment, using std::string.
	//	Note: Try to use the nameString one instead.
	//--------------------------------------------------------------------
//	void  DeleteEnvironment(const std::string& i_Name);

	//--------------------------------------------------------------------
	// Remove all environments (preparing for a new scene)
	//--------------------------------------------------------------------
	void ClearAllEnvironments();

	//--------------------------------------------------------------------
	// Set ambient light for the given environment. This is added to
	// the scene's ambient.
	//--------------------------------------------------------------------
	void SetEnvironmentAmbientLight(const nameString& i_SetName, 
								 const maFloatRGBA &i_AmbientLight);
	const maFloatRGBA& GetEnvironmentAmbientLight(const nameString& i_SetName);

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given environment
	//--------------------------------------------------------------------
	void  AddObjectToEnvironment(evmtEnvironment* i_Environment,
							  const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given environment
	//--------------------------------------------------------------------
	void  RemoveObjectFromEnvironment(evmtEnvironment* i_Environment,
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

}	// end of namespace

