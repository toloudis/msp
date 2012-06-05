/*****************************************************************************
**	chtrObjectMgr.hpp
**
**	Manages the 3d representation of the Characters in the editor system.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_OBJECTMGR_HPP
#error chtrObjectMgr.hpp multiply included
#endif
#define CHTR_OBJECTMGR_HPP

#ifndef CMM_OBJECTMGRTEMPLATE_HPP
#include "Systems/Common/Templates/cmmObjectMgrTemplate.hpp"
#endif

#ifndef CHTR_SCRIPTDATA_HPP
#include "Systems/Character/Data/chtrScriptData.hpp"
#endif
#ifndef CHTR_SCRIPTOBJECT_HPP
#include "Systems/Character/Object/chtrScriptObject.hpp"
#endif


//============================================================================
//============================================================================
class chtrObjectCreator
{
public:
	//--------------------------------------------------------------------
	// Create chtrScriptObject from chtrScriptData
	//--------------------------------------------------------------------
	static chtrScriptObject* Create(const chtrScriptData &i_Data);
};

//============================================================================
//============================================================================
class chtrObjectMgr
	: public cmmObjectMgrTemplateBase<  class chtrScriptObject, 
										class chtrObject, 
										class chtrCharactersData, 
										class chtrScriptData, 
										class chtrData,
										class chtrObjectCreator>
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
	//  Reload geometry of character with given index
	//--------------------------------------------------------------------
	static void  ReloadCharacter(int i_Index);
	
	//--------------------------------------------------------------------
	//  Change geometry of character with given index
	//--------------------------------------------------------------------
	static void  ChangeFilename(int i_Index, const fsLocator& i_Filename);

	//--------------------------------------------------------------------
	// Set subdivision level being used.
	//--------------------------------------------------------------------
	static void SetSubdivLevel(int i_SubdivLevel);

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const chtrCharactersData &i_Data);

	//--------------------------------------------------------------------
	//  Clear the UIDs of name items belonging to the data object
	//--------------------------------------------------------------------
	static void ClearItemNameUIDs(chtrCharactersData &o_Data);

	//--------------------------------------------------------------------
	// Make sure that all geometry is visible for rendering
	//--------------------------------------------------------------------
	static void ConfirmGeometryVisible();

	//--------------------------------------------------------------------
	// Create baked texture material
	//--------------------------------------------------------------------
	static void CreateBakedMaterials(const fsLocator& i_Path, const std::string& i_Extension);

	//--------------------------------------------------------------------
	// ReplaceBakedMaterials: replace fragments that has baked material
	// and with useBakedFlag on
	//--------------------------------------------------------------------
	static void ReplaceBakedMaterials();

	//--------------------------------------------------------------------
	// Set all UseBakedTexture flag to input value
	//--------------------------------------------------------------------
	static void SetBakedFlag(bool i_bVal);

	//--------------------------------------------------------------------
	// Get internal bake flag setting
	//--------------------------------------------------------------------
	static void GetBakedFlagFromFrags();

};	// end of static class

