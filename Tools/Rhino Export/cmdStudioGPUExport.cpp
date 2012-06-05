/////////////////////////////////////////////////////////////////////////////
// cmdStudioGPUExport.cpp : command file
//

#include "StdAfx.h"
#include "StudioGPUExportPlugIn.h"
#include "ExportStudioGPU.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN StudioGPUExport command
//

// Do NOT put the definition of class CCommandStudioGPUExport in a header
// file.  There is only ONE instance of a CCommandStudioGPUExport class
// and that instance is the static theStudioGPUExportCommand that appears
// immediately below the class definition.

class CCommandStudioGPUExport : public CRhinoCommand
{
public:
  // The one and only instance of CCommandStudioGPUExport is created below.
  // No copy constructor or operator= is required.  Values of
  // member variables persist for the duration of the application.

  // CCommandStudioGPUExport::CCommandStudioGPUExport()
  // is called exactly once when static theStudioGPUExportCommand is created.
	CCommandStudioGPUExport() {}

  // CCommandStudioGPUExport::~CCommandStudioGPUExport()
  // is called exactly once when static theStudioGPUExportCommand is
  // destroyed.  The destructor should not make any calls to
  // the Rhino SDK.  If your command has persistent settings,
  // then override CRhinoCommand::SaveProfile and CRhinoCommand::LoadProfile.
  ~CCommandStudioGPUExport() {}

  // Returns a unique UUID for this command.
  // If you try to use an id that is already being used, then
  // your command will not work.  Use GUIDGEN.EXE to make unique UUID.
	UUID CommandUUID()
	{
		// {A52C1046-B07B-4870-81FC-D54AE98C2C97}
    static const GUID StudioGPUExportCommand_UUID =
    { 0xA52C1046, 0xB07B, 0x4870, { 0x81, 0xFC, 0xD5, 0x4A, 0xE9, 0x8C, 0x2C, 0x97 } };
    return StudioGPUExportCommand_UUID;
	}

  // Returns the English command name.
	const wchar_t* EnglishCommandName() { return L"StudioGPUExport"; }

  // Returns the localized command name.
	const wchar_t* LocalCommandName() { return L"StudioGPUExport"; }

  // Rhino calls RunCommand to run the command.
	CRhinoCommand::result RunCommand( const CRhinoCommandContext& );
};

// The one and only CCommandStudioGPUExport object.  
// Do NOT create any other instance of a CCommandStudioGPUExport class.
static class CCommandStudioGPUExport theStudioGPUExportCommand;

CRhinoCommand::result CCommandStudioGPUExport::RunCommand( const CRhinoCommandContext& context )
{
  // CCommandStudioGPUExport::RunCommand() is called when the user runs the "StudioGPUExport"
  // command or the "StudioGPUExport" command is run by a history operation.

  // TODO: Add command code here.

  // Rhino command that display a dialog box interface should also support
  // a command-line, or scriptable interface.

	/*
  ON_wString wStr;
  wStr.Format( L"The \"%s\" command is under construction.\n", EnglishCommandName() );
  if( context.IsInteractive() )
    RhinoMessageBox( wStr, PlugIn()->PlugInName(), MB_OK );
  else
	  RhinoApp().Print( wStr );
  */

  // Prompt for a bitmap filename
	CRhinoGetFileDialog gf;
	gf.SetScriptMode( context.IsInteractive() ? FALSE : TRUE );
	BOOL rc = gf.DisplayFileDialog( 
		  CRhinoGetFileDialog::export_dialog, //save_dialog,//
		  0, 
		  CWnd::FromHandle( RhinoApp().MainWnd() )
		  );
	if( !rc )
		return cancel;


	;


  // TODO: Return one of the following values:
  //   CRhinoCommand::success:  The command worked.
  //   CRhinoCommand::failure:  The command failed because of invalid input, inability
  //                            to compute the desired result, or some other reason
  //                            computation reason.
  //   CRhinoCommand::cancel:   The user interactively canceled the command 
  //                            (by pressing ESCAPE, clicking a CANCEL button, etc.)
  //                            in a Get operation, dialog, time consuming computation, etc.

	bool isCancelled;
	bool ret = ExportStudioGPU(context.m_doc, gf.FileName(), true, context.IsInteractive(), PlugIn()->PlugInName(), isCancelled);

	return ret ? CRhinoCommand::success : (isCancelled?CRhinoCommand::cancel : CRhinoCommand::failure);
}

//
// END StudioGPUExport command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
