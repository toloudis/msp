/*****************************************************************************
**	lsetObjectMgr.hpp
**
**	Manages the 3d representation of the lights in the editor system.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef LSET_OBJECTMGR_HPP
#error lsetObjectMgr.hpp multiply included
#endif
#define LSET_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmObjectMgrTemplate.hpp"
#endif

#ifndef LSET_SCRIPTDATA_HPP
#include "Systems/LightSets/Data/lsetScriptData.hpp"
#endif
#ifndef LSET_SCRIPTOBJECT_HPP
#include "Systems/LightSets/Object/lsetScriptObject.hpp"
#endif

//============================================================================
//============================================================================
class lsetGlobalObject;

//============================================================================
//============================================================================
class lsetObjectCreator 
{
public:
	//--------------------------------------------------------------------
	// Create lsetScriptObject from lsetScriptData
	//--------------------------------------------------------------------
	static lsetScriptObject* Create(const lsetScriptData &i_Data);
};


//============================================================================
//============================================================================
class lsetObjectMgr
	: public cmmObjectMgrTemplateNoIcon<  class lsetScriptObject, 
									class lsetLightSetObject, 
									class lsetLightSetsData, 
									class lsetScriptData, 
									class lsetData,
									class lsetObjectCreator>
{
public:
	//--------------------------------------------------------------------
	// Select the prtyObject that has the global ambient light color
	//--------------------------------------------------------------------
	static void SelectGlobalObject(bool i_bAppend);
	static void DeselectGlobalObject();

	//--------------------------------------------------------------------
	// Return pointer to global prtyObject
	//--------------------------------------------------------------------
	static lsetGlobalObject* GetGlobalObject();

	//--------------------------------------------------------------------
	// Return string to use for global prtyObject
	//--------------------------------------------------------------------
	static std::string GetGlobalObjectName();

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const lsetLightSetsData &i_Data);


};	// end of static class

