/*****************************************************************************
**	billObjectMgr.hpp
**
**	Manages the 3d representation of the Billboard in the editor system.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef BILL_OBJECTMGR_HPP
#error billObjectMgr.hpp multiply included
#endif
#define BILL_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmObjectMgrTemplate.hpp"
#endif

#ifndef BILL_SCRIPTDATA_HPP
#include "Systems/Billboard/Data/billScriptData.hpp"
#endif
#ifndef BILL_SCRIPTOBJECT_HPP
#include "Systems/Billboard/Object/billScriptObject.hpp"
#endif
#ifndef BILL_BILLBOARDOBJECT_HPP
#include "Systems/Billboard/Object/billBillboardObject.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class billScriptObject;
class billBillboardObject;
class geoPickRay;
class pick3dPickList;
class camCamera;


//============================================================================
//============================================================================
class billObjectCreator 
{
public:
	//--------------------------------------------------------------------
	// Create billScriptObject from billScriptData
	//--------------------------------------------------------------------
	static billScriptObject* Create(const billScriptData &i_Data);
};


//============================================================================
//============================================================================
class billObjectMgr
	: public cmmObjectMgrGeomTemplate<  class billScriptObject, 
										class billBillboardObject, 
										class billListData, 
										class billScriptData, 
										class billData,
										class billObjectCreator>
{
public:
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	static void Init();

	//--------------------------------------------------------------------
	//  CleanUp
	//--------------------------------------------------------------------
	static void CleanUp();

	//--------------------------------------------------------------------
	//  Clear the UIDs of name items belonging to the data object
	//--------------------------------------------------------------------
	static void ClearItemNameUIDs(billListData &o_Data);

	//--------------------------------------------------------------------
	//  Update current camera list
	//--------------------------------------------------------------------
	static void UpdateCameraList(std::vector<shared_ptr<camCamera>>& i_CameraList,
								std::vector<std::string>& i_NameList);

	//--------------------------------------------------------------------
	//  Delete camera index
	//--------------------------------------------------------------------
	static void DeleteCameraIndex(int i_Index);
};

