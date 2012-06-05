/*****************************************************************************
**	fogObjectMgr.hpp
**
**	Manages the 3d representation of the storyboard in the editor system.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef FOG_OBJECTMGR_HPP
#error fogObjectMgr.hpp multiply included
#endif
#define FOG_OBJECTMGR_HPP

#ifndef CMM_SINGLEOBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmSingleObjectMgrTemplate.hpp"
#endif

#ifndef FOG_SCRIPTDATA_HPP
#include "Systems/Fog/Data/fogScriptData.hpp"
#endif
#ifndef FOG_SCRIPTOBJECT_HPP
#include "Systems/Fog/Object/fogScriptObject.hpp"
#endif
#ifndef FOG_FOGOBJECT_HPP
#include "Systems/Fog/Object/fogFogObject.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class fogScriptObject;
class fogFogObject;
class geoPickRay;
class pick3dPickList;


//============================================================================
//============================================================================
class fogObjectMgr
	: public cmmSingleObjectMgrTemplateNoIcon<  class fogScriptObject, 
												class fogFogObject, 
												class fogScriptData, 
												class fogFogData>
{
public:
	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const fogScriptData &i_Data);

	//--------------------------------------------------------------------
	//  Clear the UIDs of name items belonging to the data object
	//--------------------------------------------------------------------
	static void ClearItemNameUIDs(fogScriptData &o_Data);

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	static void DeleteObject();
};	// end of static class

