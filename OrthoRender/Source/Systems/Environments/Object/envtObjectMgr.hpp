/*****************************************************************************
**	envtObjectMgr.hpp
**
**	Manages the 3d representation of the lights in the editor system.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_OBJECTMGR_HPP
#error envtObjectMgr.hpp multiply included
#endif
#define ENVT_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmObjectMgrTemplate.hpp"
#endif

#ifndef ENVT_SCRIPTDATA_HPP
#include "Systems/Environments/Data/envtScriptData.hpp"
#endif
#ifndef ENVT_SCRIPTOBJECT_HPP
#include "Systems/Environments/Object/envtScriptObject.hpp"
#endif


//============================================================================
//============================================================================
class envtObjectCreator 
{
public:
	//--------------------------------------------------------------------
	// Create envtScriptObject from envtScriptData
	//--------------------------------------------------------------------
	static envtScriptObject* Create(const envtScriptData &i_Data);
};


//============================================================================
// Since environments have no 3d representation in the scene, 
//	it uses the "NoIcon" template.
//============================================================================
class envtObjectMgr
	: public cmmObjectMgrTemplateNoIcon<  class envtScriptObject, 
											class envtEnvironmentObject, 
											class envtEnvironmentsData, 
											class envtScriptData, 
											class envtData,
											class envtObjectCreator>
{
public:

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const envtEnvironmentsData &i_Data);

};	// end of static class

