//*****************************************************************************
/*!	\file importexport_menu.cpp
 	\brief Defines the callbacks that implement the Import Export demo menu.
 */
//*****************************************************************************


#include <xsi_context.h>
#include <xsi_ref.h>
#include <xsi_menu.h>
#include <xsi_customproperty.h>
#include <xsi_application.h>
#include <xsi_value.h>
#include "exportmesh_command.h"

extern XSI::CustomProperty GetImportExportProp();

//*****************************************************************************
/*!	Add the Import Export Demo menu item to the Demo Tool menu. This menu item
	triggers a callback that opens the Import Export Demo UI.
	\param in_ctxt The context that encapsulates the menu to initialize.
 */
//*****************************************************************************

XSIPLUGINCALLBACK XSI::CStatus MSPExportTool_Init( XSI::CRef& in_ctxt )
{
	XSI::Context ctxt( in_ctxt );

	XSI::Menu oMenu;
	oMenu = ctxt.GetSource();		
	
	XSI::MenuItem oNewItem;
	oMenu.AddCallbackItem( L"MSP Export...", L"OnMSPExportMenuClicked", oNewItem );
	
	return XSI::CStatus::OK;
}

//*****************************************************************************
/*!	Callback executed when a user clicks the Import Export Demo menu item. The 
	callback displays the Import Export Demo property page.
	\param in_ctxt The context that encapsulates the selected menu item.
 */
//*****************************************************************************

XSIPLUGINCALLBACK XSI::CStatus OnMSPExportMenuClicked( XSI::CRef& in_ref)
{	
	XSI::CustomProperty prop = GetImportExportProp();

	XSI::CValue retVal ;
	
	XSI::CValueArray args(5) ;	
	args[0] = prop; 
	args[2] = L"MSP file export";
	args[3] = (LONG)XSI::siModal;//(LONG)XSI::siLock;
		
	XSI::Application app;
	XSI::CStatus stat = app.ExecuteCommand( L"InspectObj", args, retVal );		

	if (stat == XSI::CStatus::OK)
	{
		XSI::CValueArray args(9);
		args[0] = prop.GetParameterValue(L"ExportIntent");
		args[1] = prop.GetParameterValue( L"Subd_type" );
		args[2] = prop.GetParameterValue( L"Subd_level" );
		args[3] = prop.GetParameterValue( GXBExportDoc::Texture_ExportFilepath::m_scriptName );
		args[4] = prop.GetParameterValue( GXBExportDoc::MergeBasedOnMtls::m_scriptName );
		args[5] = prop.GetParameterValue( GXBExportDoc::MaxNumTrianglesInMergedMesh::m_scriptName );
		args[6] = prop.GetParameterValue( GXBExportDoc::ExportGeomForVertexAnim::m_scriptName );
		args[7] = prop.GetParameterValue( GXBExportDoc::AnimExportStartFrame::m_scriptName );
		args[8] = prop.GetParameterValue( GXBExportDoc::AnimExportEndFrame::m_scriptName );
		XSI::CValue retVal;
		XSI::Application app;
		app.ExecuteCommand( L"MSPExportMesh", args, retVal ) ;
	}

	return XSI::CStatus::OK;	
}
