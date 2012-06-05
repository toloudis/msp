/*****************************************************************************
**	sbrdObjectMgr.hpp
**
**	Manages the 3d representation of the storyboard in the editor system.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SBRD_OBJECTMGR_HPP
#error sbrdObjectMgr.hpp multiply included
#endif
#define SBRD_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmObjectMgrTemplate.hpp"
#endif

#ifndef SBRD_SCRIPTDATA_HPP
#include "Systems/Storyboards/Data/sbrdScriptData.hpp"
#endif
#ifndef SBRD_SCRIPTOBJECT_HPP
#include "Systems/Storyboards/Object/sbrdScriptObject.hpp"
#endif
#ifndef SBRD_SBRDBOARDOBJECT_HPP
#include "Systems/Storyboards/Object/sbrdBillboardObject.hpp"
#endif



//============================================================================
//	forward references
//============================================================================
class sbrdScriptObject;
class sbrdBillboardObject;
class geoPickRay;
class pick3dPickList;

//============================================================================
//============================================================================
class sbrdObjectCreator 
{
public:
	//--------------------------------------------------------------------
	// Create sbrdScriptObject from sbrdScriptData
	//--------------------------------------------------------------------
	static sbrdScriptObject* Create(const sbrdScriptData &i_Data);
};


//============================================================================
//============================================================================
class sbrdObjectMgr
	: public cmmObjectMgrGeomTemplate<  class sbrdScriptObject, 
										class sbrdBillboardObject, 
										class sbrdListData, 
										class sbrdScriptData, 
										class sbrdObjectData,
										class sbrdObjectCreator>
{
public:
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	static void  Init();

	//--------------------------------------------------------------------
	//  CleanUp
	//--------------------------------------------------------------------
	static void  CleanUp();

	//--------------------------------------------------------------------
	//  clear
	//--------------------------------------------------------------------
	static void Clear();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static sbrdListData& GetListData();

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	static sbrdListData GetData();

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const sbrdListData &i_Data);

	//--------------------------------------------------------------------
	//  Add new object to world
	//--------------------------------------------------------------------
	static int  AddObject(const sbrdScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	static void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static int AddStoryboard(const itString& i_Filename, bool i_bAddTo3DWorld = true);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void DeleteStoryboard(int i_Index);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void SwapStoryboards(int i_Index1, int i_Index2);
};

