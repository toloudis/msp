/*****************************************************************************
**	ptltObjectMgr.hpp
**
**	Manages the 3d representation of the lights in the editor system.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PTLT_OBJECTMGR_HPP
#error ptltObjectMgr.hpp multiply included
#endif
#define PTLT_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmObjectMgrTemplate.hpp"
#endif

#ifndef PTLT_SCRIPTDATA_HPP
#include "Systems/PtLt/Data/ptltScriptData.hpp"
#endif
#ifndef PTLT_SCRIPTOBJECT_HPP
#include "Systems/PtLt/Object/ptltScriptObject.hpp"
#endif


//============================================================================
//============================================================================
class ptltObjectCreator 
{
public:
	//--------------------------------------------------------------------
	// Create ptltScriptObject from ptltScriptData
	//--------------------------------------------------------------------
	static ptltScriptObject* Create(const ptltScriptData &i_Data);
};


//============================================================================
//============================================================================
class ptltObjectMgr
	: public cmmObjectMgrTemplate<  class ptltScriptObject, 
									class ptltPointLightObject, 
									class ptltPointLightsData, 
									class ptltScriptData, 
									class ptltData,
									class ptltObjectCreator>
{
public:

};	// end of static class

