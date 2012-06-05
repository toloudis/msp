/*****************************************************************************
**	dirltObjectMgr.hpp
**
**	Manages the 3d representation of the lights in the editor system.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef DIRLT_OBJECTMGR_HPP
#error dirltObjectMgr.hpp multiply included
#endif
#define DIRLT_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "cmmObjectMgrTemplate.hpp"
#endif

#ifndef DIRLT_SCRIPTDATA_HPP
#include "dirltScriptData.hpp"
#endif
#ifndef DIRLT_SCRIPTOBJECT_HPP
#include "dirltScriptObject.hpp"
#endif

//============================================================================
//============================================================================
class dirltObjectCreator 
{
public:
	//--------------------------------------------------------------------
	// Create dirltScriptObject from dirltScriptData
	//--------------------------------------------------------------------
	static dirltScriptObject* Create(const dirltScriptData &i_Data);
};


//============================================================================
//============================================================================
class dirltObjectMgr
	: public cmmObjectMgrTemplate<  class dirltScriptObject, 
									class dirltDirLightObject, 
									class dirltDirLightsData, 
									class dirltScriptData, 
									class dirltData,
									class dirltObjectCreator>
{
public:
	//--------------------------------------------------------------------
	//  Add dir light to world
	//--------------------------------------------------------------------
	static int  AddDirLight( g3dDirectionalLight* i_pLight );

	//--------------------------------------------------------------------
	//	ShowIcons - show or hide icons that are not part of real scene.
	//--------------------------------------------------------------------
	static void ShowIcons( bool i_bVisible );

};	// end of static class

