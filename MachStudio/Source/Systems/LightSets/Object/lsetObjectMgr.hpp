/*****************************************************************************
**	lsetObjectMgr.hpp
**
**	Manages the 3d representation of the lights in the editor system.
**
**	StudioGPU
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
	: public cmmObjectMgrTemplateNoIcon<class lsetScriptObject, 
										class lsetLightSetObject, 
										class lsetLightSetsData, 
										class lsetScriptData, 
										class lsetData,
										class lsetObjectCreator>
{
public:
	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const lsetLightSetsData &i_Data);

	//--------------------------------------------------------------------
	// Return name of light set with given index
	//--------------------------------------------------------------------
	static nameString GetName(int i_Index);

	//--------------------------------------------------------------------
	//  Clear the UIDs of name items belonging to the data object
	//--------------------------------------------------------------------
	static void ClearItemNameUIDs(lsetLightSetsData &o_Data);
};	// end of static class

