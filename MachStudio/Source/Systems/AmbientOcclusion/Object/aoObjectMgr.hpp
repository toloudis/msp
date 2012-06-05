/*****************************************************************************
**	aoObjectMgr.hpp
**
**	Manages the 3d representation of the storyboard in the editor system.
**
**	StudioGPU
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
#ifndef AO_AOOBJECT_HPP
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
												class aoAOObject, 
												class aoScriptData, 
												class aoAOData>
{
public:
	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const aoScriptData &i_Data);

	//--------------------------------------------------------------------
	//  Clear the UIDs of name items belonging to the data object
	//--------------------------------------------------------------------
	static void ClearItemNameUIDs(aoScriptData &o_Data);

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	static void DeleteObject();

};	// end of static class

