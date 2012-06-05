/*****************************************************************************
**	ltstIsolateMgr.hpp
**
**	Keeps track of lights that can be isolated by nameString. 
**
**	Provides functions for isolating the lighting from sets of lights.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef LTST_ISOLATEMGR_HPP
#error ltstIsolateMgr.hpp multiply included
#endif
#define LTST_ISOLATEMGR_HPP


#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <set>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class nameObject;
class ltstIsolatable;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace ltstIsolateMgr
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
	//  Add named light to list of things that can be isolated
	//--------------------------------------------------------------------
	void  AddLight(nameObject* i_pNameObj, 
				   ltstIsolatable* i_pLight);

	//--------------------------------------------------------------------
	//	Remove light from manager 
	//--------------------------------------------------------------------
	void RemoveLight(nameObject* i_pNameObj, 
				     ltstIsolatable* i_pLight);

	//--------------------------------------------------------------------
	//	Test if name is component that can be isolated
	//--------------------------------------------------------------------
	bool  IsLight(const nameString& i_Name);

//--------------------------------------------------------------------
// The user interface can then manipulate the isolation with the
// following functions by using the names of the lights and objects
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Remove all light sets (preparing for a new scene)
	//--------------------------------------------------------------------
	void ClearIsolation();

	//--------------------------------------------------------------------
	//	Turn off all lights except the ones in this named set.
	//--------------------------------------------------------------------
	void  IsolateLights(const std::set<nameString>& i_IsolatedLights);

	//--------------------------------------------------------------------
	// Add or remove a single light to the isolation set by name
	//--------------------------------------------------------------------
	void  SetIsolated(const nameString& i_LightName, bool i_bIsolated);

	//--------------------------------------------------------------------
	// Return true if there is a current isolated set of lights.
	//--------------------------------------------------------------------
	bool HasIsolatedSet();
	
	//--------------------------------------------------------------------
	// Return true if light is isolated. If there is no isolated set,
	// then this will return true for all lights (because all are enabled)
	//--------------------------------------------------------------------
	bool  IsLightEnabled(const nameString& i_LightName);

}	// end of namespace
