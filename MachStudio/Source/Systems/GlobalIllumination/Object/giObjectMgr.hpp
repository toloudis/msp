/*****************************************************************************
**	giObjectMgr.hpp
**
**	Manages the 3d representation of the storyboard in the editor system.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GI_OBJECTMGR_HPP
#error giObjectMgr.hpp multiply included
#endif
#define GI_OBJECTMGR_HPP

#ifndef CMM_SINGLEOBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmSingleObjectMgrTemplate.hpp"
#endif

#ifndef GI_SCRIPTDATA_HPP
#include "Systems/GlobalIllumination/Data/giScriptData.hpp"
#endif
#ifndef GI_SCRIPTOBJECT_HPP
#include "Systems/GlobalIllumination/Object/giScriptObject.hpp"
#endif
#ifndef GI_GIOBJECT_HPP
#include "Systems/GlobalIllumination/Object/giGIObject.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class giScriptObject;
class giFogObject;
class geoPickRay;
class pick3dPickList;


//============================================================================
//============================================================================
class giObjectMgr
	: public cmmSingleObjectMgrTemplateNoIcon<  class giScriptObject, 
												class giGIObject, 
												class giScriptData, 
												class giGIData>
{
public:
	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const giScriptData &i_Data);

	//--------------------------------------------------------------------
	//  Clear the UIDs of name items belonging to the data object
	//--------------------------------------------------------------------
	static void ClearItemNameUIDs(giScriptData &o_Data);

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	static void DeleteObject();

};	// end of static class

