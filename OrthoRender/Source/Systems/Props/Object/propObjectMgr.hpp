/*****************************************************************************
**	propObjectMgr.hpp
**
**	Manages the 3d representation of the Props in the editor system.
**
**	Extra Large Technology
**	Copyright(C) 2004-6 - All Rights Reserved
\****************************************************************************/
#ifdef PROP_OBJECTMGR_HPP
#error propObjectMgr.hpp multiply included
#endif
#define PROP_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmObjectMgrTemplate.hpp"
#endif

#ifndef PROP_SCRIPTDATA_HPP
#include "Systems/Props/Data/propScriptData.hpp"
#endif
#ifndef PROP_SCRIPTOBJECT_HPP
#include "Systems/Props/Object/propScriptObject.hpp"
#endif


//============================================================================
//============================================================================
class propObjectCreator 
{
public:
	//--------------------------------------------------------------------
	// Create propScriptObject from propScriptData
	//--------------------------------------------------------------------
	static propScriptObject* Create(const propScriptData &i_Data);
};

//============================================================================
//============================================================================
class propObjectMgr
	: public cmmObjectMgrTemplateBase<  class propScriptObject, 
									class propPropObject, 
									class propPropsData, 
									class propScriptData, 
									class propData,
									class propObjectCreator>
{
public:
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	static void  Init();

	//--------------------------------------------------------------------
	//  CleanUp
	//--------------------------------------------------------------------
	static void  CleanUp();

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const propPropsData &i_Data);

	//--------------------------------------------------------------------
	// Make sure that all geometry is visible for rendering
	//--------------------------------------------------------------------
	static void ConfirmGeometryVisible();

	//--------------------------------------------------------------------
	// Set subdivision level being used.
	//--------------------------------------------------------------------
	static void SetSubdivLevel(int i_SubdivLevel);

};	// end of static class
