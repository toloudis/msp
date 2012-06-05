/*****************************************************************************
**	prjltObjectMgr.hpp
**
**		Manages the 3d representation of the lights in the editor system.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRJLT_OBJECTMGR_HPP
#error prjltObjectMgr.hpp multiply included
#endif
#define PRJLT_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmObjectMgrTemplate.hpp"
#endif

#ifndef PRJLT_SCRIPTDATA_HPP
#include "Systems/PrjLt/Data/prjltScriptData.hpp"
#endif
#ifndef PRJLT_SCRIPTOBJECT_HPP
#include "Systems/PrjLt/Object/prjltScriptObject.hpp"
#endif
#ifndef PRJLT_PROJECTEDLIGHTOBJECT_HPP
#include "Systems/PrjLt/Object/prjltProjectedLightObject.hpp"
#endif


//============================================================================
//============================================================================
class prjltObjectCreator 
{
public:
	//--------------------------------------------------------------------
	// Create prjltScriptObject from prjltScriptData
	//--------------------------------------------------------------------
	static prjltScriptObject* Create(const prjltScriptData &i_Data);
};


//============================================================================
//============================================================================
class prjltObjectMgr
	: public cmmObjectMgrTemplate<  class prjltScriptObject, 
									class prjltProjectedLightObject, 
									class prjltProjectLightsData, 
									class prjltScriptData, 
									class prjltData,
									class prjltObjectCreator>
{
public:

	//--------------------------------------------------------------------
	//  Clear the UIDs of name items belonging to the data object
	//--------------------------------------------------------------------
	static void ClearItemNameUIDs(prjltProjectLightsData &o_Data);

};	// end of static class

