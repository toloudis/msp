/*****************************************************************************
**	envtObjectMgr.hpp
**
**	Manages the 3d representation of the lights in the editor system.
**
**	StudioGPU
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
class envtDefaultEnvironment;
class envtSwlEnvironment;


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
	: public cmmObjectMgrTemplateNoIcon<class envtScriptObject, 
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

	//--------------------------------------------------------------------
	//  Clear the UIDs of name items belonging to the data object
	//--------------------------------------------------------------------
	static void ClearItemNameUIDs(envtEnvironmentsData &o_Data);

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	static envtEnvironmentsData GetData();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static void CreateDefaultEnvironment();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static void DestroyDefaultEnvironment();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static envtDefaultEnvironment* GetDefaultEnvironment();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static void envtObjectMgr::CreateSwlEnvironment();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static void envtObjectMgr::DestroySwlEnvironment();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static envtSwlEnvironment* envtObjectMgr::GetSwlEnvironment();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static void envtObjectMgr::SelectDefaultEnvironment(bool i_bAppend = false);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static void envtObjectMgr::DeselectDefaultEnvironment();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static void envtObjectMgr::SelectSwlEnvironment(bool i_bAppend = false);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	static void envtObjectMgr::DeselectSwlEnvironment();
};	// end of static class

