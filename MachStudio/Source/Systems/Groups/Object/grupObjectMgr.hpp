/*****************************************************************************
**	grupObjectMgr.hpp
**
**	Manages the 3d representation of the lights in the editor system.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GRUP_OBJECTMGR_HPP
#error grupObjectMgr.hpp multiply included
#endif
#define GRUP_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmSimpleObjectMgrTemplate.hpp"
#endif

#ifndef GRUP_DATA_HPP
#include "Systems/Groups/Data/grupData.hpp"
#endif
#ifndef GRUP_GROUPOBJECT_HPP
#include "Systems/Groups/Object/grupGroupObject.hpp"
#endif


//============================================================================
//============================================================================
class grupObjectCreator 
{
public:
	//--------------------------------------------------------------------
	// Create grupGroupObject from grupData
	//--------------------------------------------------------------------
	static grupGroupObject* Create(const grupData &i_Data);
};


//============================================================================
//============================================================================
class grupObjectMgr
	: public cmmSimpleObjectMgrTemplate<  class grupGroupObject, 
									class grupGroupsData, 
									class grupData,
									class grupObjectCreator>
{
public:
	//--------------------------------------------------------------------
	//  Clear the UIDs of name items belonging to the data object
	//--------------------------------------------------------------------
	static void ClearItemNameUIDs(grupGroupsData &o_Data);

};	// end of static class

