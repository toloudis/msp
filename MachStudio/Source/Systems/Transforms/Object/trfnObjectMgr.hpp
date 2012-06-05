/*****************************************************************************
**	trfnObjectMgr.hpp
**
**	Manages the 3d representation of the lights in the editor system.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TRFN_OBJECTMGR_HPP
#error trfnObjectMgr.hpp multiply included
#endif
#define TRFN_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmObjectMgrTemplate.hpp"
#endif 
#ifndef TRFN_SCRIPTDATA_HPP
#include "Systems/Transforms/Data/trfnScriptData.hpp"
#endif
#ifndef TRFN_SCRIPTOBJECT_HPP
#include "Systems/Transforms/Object/trfnScriptObject.hpp"
#endif 


//============================================================================
//============================================================================
class trfnObjectCreator 
{
public:
	//--------------------------------------------------------------------
	// Create trfnTransformObject from trfnData
	//--------------------------------------------------------------------
	static trfnScriptObject* Create(const trfnScriptData &i_Data);
};

//============================================================================
//============================================================================
class trfnObjectMgr
	: public cmmObjectMgrGeomTemplate<  class trfnScriptObject,
									class trfnTransformObject, 
									class trfnTransformsData, 
									class trfnScriptData,
									class trfnData,
									class trfnObjectCreator>
{
public:

	//--------------------------------------------------------------------
	//  Clear the UIDs of name items belonging to the data object
	//--------------------------------------------------------------------
	static void ClearItemNameUIDs(trfnTransformsData &o_Data);

	//--------------------------------------------------------------------
	// Return name of transform with given index
	//--------------------------------------------------------------------
	static nameString GetName(int i_Index);

};	// end of static class

