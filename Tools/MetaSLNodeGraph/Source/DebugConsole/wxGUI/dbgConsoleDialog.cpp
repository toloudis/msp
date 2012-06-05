/*****************************************************************************
**	dbgConsoleDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "dbgConsoleDialog.hpp"

#include "DebugConsole/dbgConsoleDialogUtil.hpp"
#include "DebugConsole/dbgConsoleStream.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const wxColour determineColor(std::string& i_pString, bool &o_bErrorMessage)
	{
		o_bErrorMessage = false;
		wxColour textColor;
		if(i_pString.find("*WARNING*",0) != std::string::npos)
		{
			//return warning color
			textColor = wxColour(130, 0, 0);
			return textColor;
		}
		if(i_pString.find("*ERROR*",0) != std::string::npos)
		{
			//return error color
			o_bErrorMessage = true;
			textColor = wxColour(255, 0, 0);
			return textColor;
		}
		textColor = wxColour(0, 0, 0);
		return textColor;
	}
}

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
dbgConsoleDialog* dbgConsoleDialog::Instance = NULL;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void dbgConsoleDialog::UpdateConsoleString( std::string& i_pString )
{
	m_richText_Console->Freeze();
	m_richText_Console->BeginSuppressUndo();

	bool bErrorMsg = false;
	wxColour textCol = determineColor(i_pString, bErrorMsg);
	m_richText_Console->BeginTextColour( textCol );
	m_richText_Console->AppendText( wxString(i_pString.c_str(), wxConvUTF8) );
	m_richText_Console->EndSuppressUndo();
	m_richText_Console->Thaw();
	m_richText_Console->ShowPosition( m_richText_Console->GetLastPosition() );

	// Also display error messages in the status bar
	if (bErrorMsg)
	{
		// This line parrots the error messages to the status bar
		//guiStatusBarMgr::SetErrorMessage(i_pString.c_str());

		// This method draws the users attention to the debug console
		guiStatusBarMgr::SetErrorMessage("Errors have occurred, view the debug console for details.");
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dbgConsoleDialog::dbgConsoleDialog( wxWindow* parent, const wxString& i_Title, 
									const wxString& i_Caption )
:	m_Buf(NULL), m_Dbgs(NULL), dbgConsoleDialogBase(parent)
{
	// Add this panel to the AUI manager
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(i_Title).Caption(i_Caption).Show().Layer(2).Bottom());
	AddDebugConsoleStream();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dbgConsoleDialog::~dbgConsoleDialog()
{
	dbgMsg::removeStream(std::wstring(L"debugConsole"));
	if (dbgConsoleDialog::Instance == this)
		dbgConsoleDialog::Instance = NULL;
	
	delete m_Buf;
	delete m_Dbgs;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void dbgConsoleDialog::dbgConsoleDialog_OnActivate( wxActivateEvent& event )
{ 
	event.Skip(); 
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void dbgConsoleDialog::AddDebugConsoleStream()
{
	m_Buf = new dbgConsoleBuf(&dbgConsoleDialogUtil::UpdateConsoleDialog);
	m_Dbgs = new dbgStringStream(m_Buf);
	
	dbgMsg::addStream(std::wstring(L"debugConsole"), m_Dbgs);
}

#endif

