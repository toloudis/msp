/*****************************************************************************
**	aoObjectMgr.hpp
**
**	Manages the 3d representation of the storyboard in the editor system.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef AO_OBJECTMGR_HPP
#error aoObjectMgr.hpp multiply included
#endif
#define AO_OBJECTMGR_HPP

#ifndef CMM_SINGLEOBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmSingleObjectMgrTemplate.hpp"
#endif

#ifndef AO_SCRIPTDATA_HPP
#include "Systems/AmbientOcclusion/Data/aoScriptData.hpp"
#endif
#ifndef AO_SCRIPTOBJECT_HPP
#include "Systems/AmbientOcclusion/Object/aoScriptObject.hpp"
#endif
#ifndef AO_FOGOBJECT_HPP
#include "Systems/AmbientOcclusion/Object/aoAOObject.hpp"
#endif

//============================================================================
//	forward references
//============================================================================
class aoScriptObject;
class aoFogObject;
class geoPickRay;
class pick3dPickList;

//============================================================================
//============================================================================
class aoObjectMgr
	: public cmmSingleObjectMgrTemplateNoIcon<  class aoScriptObject, 
									class aoFogObject, 
									class aoScriptData, 
									class aoFogData>
{
public:
	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const aoScriptData &i_Data);

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	static void  DeleteObject();
									
};	// end of static class

