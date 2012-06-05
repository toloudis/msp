/*****************************************************************************
**  xtraCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/xtra/GUI/xtraCommands.hpp"

//#include "Support/xtra/GUI/xtraDialogUtil.hpp"
#include "Support/xtra/GUI/xtraOperations.hpp"
#include "Support/xtra/xtraScriptObject.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"


//============================================================================
//============================================================================
namespace xtraCommands
{
	namespace
	{
		//const char* c_Toolbar_Surfaces_Name = "Custom Properties";

		int l_MenuIdNewBoolean = -1;
		int l_MenuIdNewFloat = -1;
		int l_MenuIdNewColor = -1;
		int l_MenuIdNewString = -1;
		int l_MenuIdNewPosition = -1;
		int l_MenuIdNewOrientation = -1;
		int l_MenuIdNewTexture = -1;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateBoolean()
		{
			if ( xtraScriptObject *pXtraObj = sel3dCastUtil::CastSelectedObject<xtraScriptObject>() )
			{
				xtraOperations::CreateCustomBoolean(pXtraObj);
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateFloat()
		{
			if ( xtraScriptObject *pXtraObj = sel3dCastUtil::CastSelectedObject<xtraScriptObject>() )
			{
				xtraOperations::CreateCustomFloat(pXtraObj);
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateColor()
		{
			if ( xtraScriptObject *pXtraObj = sel3dCastUtil::CastSelectedObject<xtraScriptObject>() )
			{
				xtraOperations::CreateCustomColor(pXtraObj);
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateString()
		{
			if ( xtraScriptObject *pXtraObj = sel3dCastUtil::CastSelectedObject<xtraScriptObject>() )
			{
				xtraOperations::CreateCustomString(pXtraObj);
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreatePosition()
		{
			if ( xtraScriptObject *pXtraObj = sel3dCastUtil::CastSelectedObject<xtraScriptObject>() )
			{
				xtraOperations::CreateCustomPosition(pXtraObj);
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateOrientation()
		{
			if ( xtraScriptObject *pXtraObj = sel3dCastUtil::CastSelectedObject<xtraScriptObject>() )
			{
				xtraOperations::CreateCustomOrientation(pXtraObj);
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateTexture()
		{
			if ( xtraScriptObject *pXtraObj = sel3dCastUtil::CastSelectedObject<xtraScriptObject>() )
			{
				xtraOperations::CreateCustomTexture(pXtraObj);
			}
		}

	}

	//--------------------------------------------------------------------
	// SetupMenu --
	//--------------------------------------------------------------------
	void SetupMenu()
	{
		// Create commands for surface operations
		const char* c_MenuName = "Custom Properties";
		guiMenuMgr::AddMenu("Actions", c_MenuName);
			
		cmaCommand* pCmd = NULL;

		//	COMMAND: Add Boolean Property
		pCmd = new cmaCommandSimple("Add Boolean Property", 
									c_MenuName, 
									"Create new boolean property.",
									&Execute_CreateBoolean );
		l_MenuIdNewBoolean = guiMenuMgr::AddMenuItem( c_MenuName, pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdNewBoolean );

		//	COMMAND: Add Number Property
		pCmd = new cmaCommandSimple("Add Number Property", 
									c_MenuName, 
									"Create new ranged number property.",
									&Execute_CreateFloat );
		l_MenuIdNewFloat = guiMenuMgr::AddMenuItem( c_MenuName, pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdNewFloat );

		//	COMMAND: Add Color Property
		pCmd = new cmaCommandSimple("Add Color Property", 
									c_MenuName, 
									"Create new ranged number property.",
									&Execute_CreateColor );
		l_MenuIdNewColor = guiMenuMgr::AddMenuItem( c_MenuName, pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdNewColor );

		//	COMMAND: Add String Property
		pCmd = new cmaCommandSimple("Add String Property", 
									c_MenuName, 
									"Create new ranged number property.",
									&Execute_CreateString );
		l_MenuIdNewString = guiMenuMgr::AddMenuItem( c_MenuName, pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdNewString );

		//	COMMAND: Add Position Property
		pCmd = new cmaCommandSimple("Add Position Property", 
									c_MenuName, 
									"Create new ranged number property.",
									&Execute_CreatePosition );
		l_MenuIdNewPosition = guiMenuMgr::AddMenuItem( c_MenuName, pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdNewPosition );

		//	COMMAND: Add Orientation Property
		pCmd = new cmaCommandSimple("Add Orientation Property", 
									c_MenuName, 
									"Create new ranged number property.",
									&Execute_CreateOrientation );
		l_MenuIdNewOrientation = guiMenuMgr::AddMenuItem( c_MenuName, pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdNewOrientation );

		//	COMMAND: Add Texture Property
		pCmd = new cmaCommandSimple("Add Texture Property", 
									c_MenuName, 
									"Create new ranged number property.",
									&Execute_CreateTexture );
		l_MenuIdNewTexture = guiMenuMgr::AddMenuItem( c_MenuName, pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuIdNewTexture );

	}

	//--------------------------------------------------------------------
	// EnableMenus -- enable menu items based on selection
	//--------------------------------------------------------------------
	void EnableMenus(bool i_bHaveObject)
	{
		guiMenuMgr::MenuObjectsEnable(l_MenuIdNewBoolean, i_bHaveObject);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdNewFloat, i_bHaveObject);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdNewColor, i_bHaveObject);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdNewString, i_bHaveObject);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdNewPosition, i_bHaveObject);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdNewOrientation, i_bHaveObject);
		guiMenuMgr::MenuObjectsEnable(l_MenuIdNewTexture, i_bHaveObject);
	}

}	// end of namespace
