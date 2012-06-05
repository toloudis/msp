/*****************************************************************************
**  cmmSplineCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/Spline/cmmSplineCommands.hpp"
#include "Systems/Common/Spline/cmmSplineOperations.hpp"

//	tools
#include "Core/dbg/dbgLog.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiToolbarMgr.hpp"
#include "ToolUIWx/twx/twxWidgets.hpp"


//============================================================================
//============================================================================
namespace cmmSplineCommands
{
	namespace
	{
		const char* c_SplineToolbarName = "toolBar_Splines";

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//void Set_SplineToolbar(bool i_bChecked)
		//{
		//	guiToolbarMgr::Show(c_SplineToolbarName, i_bChecked);
		//}
		//bool Get_SplineToolbar()
		//{
		//	return guiToolbarMgr::IsVisible(c_SplineToolbarName);
		//}
	}


	//--------------------------------------------------------------------
	// SetupMenu
	//--------------------------------------------------------------------
	void SetupMenu()
	{
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND:  Select previous control point	
		pCmd = new cmaCommandSimple("Select Previous Point", 
			"Splines",
			"Select previous control point in spline",
			cmmSplineOperations::SelectPreviousControlPoint);
		guiMenuMgr::AddMenu("Actions", "Splines");
		menu_id = guiMenuMgr::AddMenuItem( "Splines", pCmd->GetTag().c_str() 
#ifdef USE_WXWIDGETS
			, c_SplineToolbarName, "spline-prevpoint.png"
#endif
			); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );


		//	COMMAND:  Select next control point	
		pCmd = new cmaCommandSimple("Select Next Point", 
			"Splines",
			"Select next control point in spline",
			cmmSplineOperations::SelectNextControlPoint);
		guiMenuMgr::AddMenu("Actions", "Splines");
		menu_id = guiMenuMgr::AddMenuItem( "Splines", pCmd->GetTag().c_str() 
#ifdef USE_WXWIDGETS
			, c_SplineToolbarName, "spline-nextpoint.png"
#endif
			); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );


		//	COMMAND:  Select all control points in spline	
		pCmd = new cmaCommandSimple("Select All Points", 
			"Splines",
			"Select all control points in spline",
			cmmSplineOperations::SelectAllControlPoints);
		guiMenuMgr::AddMenu("Actions", "Splines");
		menu_id = guiMenuMgr::AddMenuItem( "Splines", pCmd->GetTag().c_str() 
#ifdef USE_WXWIDGETS
			, c_SplineToolbarName, "spline-allpoints.png"
#endif
			); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND:  Insert new control point	
		pCmd = new cmaCommandSimple("Insert New Point", 
			"Splines",
			"Insert new control point after selected control points in spline",
			cmmSplineOperations::InsertControlPoint);
		guiMenuMgr::AddMenu("Actions", "Splines");
		menu_id = guiMenuMgr::AddMenuItem( "Splines", pCmd->GetTag().c_str() 
#ifdef USE_WXWIDGETS
			, c_SplineToolbarName, "spline-insertpoint.png"
#endif
			); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND:  Delete control points	
		pCmd = new cmaCommandSimple("Delete Points", 
			"Splines",
			"Deleted selected control points in spline",
			cmmSplineOperations::DeleteControlPoint);
		guiMenuMgr::AddMenu("Actions", "Splines");
		menu_id = guiMenuMgr::AddMenuItem( "Splines", pCmd->GetTag().c_str() 
#ifdef USE_WXWIDGETS
			, c_SplineToolbarName, "spline-deletepoint.png"
#endif
			); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND:  Move editor camera to selected control point
		pCmd = new cmaCommandSimple("Set Edit Cam to Selected Point", 
			"Splines",
			"Move the editor camera's position to the selected control point in spline",
			cmmSplineOperations::SetEditCamToPoint);
		guiMenuMgr::AddMenu("Actions", "Splines");
		menu_id = guiMenuMgr::AddMenuItem( "Splines", pCmd->GetTag().c_str() 
#ifdef USE_WXWIDGETS
			, c_SplineToolbarName, "spline-edittopoint.png"
#endif
			); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND:  Move selected point to editor camera's position
		pCmd = new cmaCommandSimple("Set Selected Point to Edit Cam", 
			"Splines",
			"Move selected control point in spline to the editor camera's position",
			cmmSplineOperations::SetPointToEditCam);
		guiMenuMgr::AddMenu("Actions", "Splines");
		menu_id = guiMenuMgr::AddMenuItem( "Splines", pCmd->GetTag().c_str() 
#ifdef USE_WXWIDGETS
			, c_SplineToolbarName, "spline-pointtoedit.png"
#endif
			); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		// View of spline toolbar based on selection, not toggle menu
		////	COMMAND: Spline toolbar
		//guiMenuMgr::AddMenu( "View", "Toolbars" );
		//pCmd = new cmaCommandToggle("Spline Toolbar", 
		//							"Toolbars", 
		//							"View the spline editing toolbar",
		//							&Set_SplineToolbar, 
		//							&Get_SplineToolbar );
		//menu_id = guiMenuMgr::AddCheckableMenuItem( "Toolbars", pCmd->GetTag().c_str() );
		//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		////cmmSystemDialogUtil::AddSystemCommand( "Main", "View Spline Toolbar", pCmd );

	}

	//--------------------------------------------------------------------
	// Show or hide the toolbar for spline editing control
	//--------------------------------------------------------------------
	void ShowSplineToolbar(bool i_bShow)
	{
#ifdef USE_WXWIDGETS
//WXGUI
/*
		guiToolbarMgr::Show(c_SplineToolbarName, i_bShow);
*/
#endif
	}
}
