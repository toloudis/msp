/*****************************************************************************
**	dbgConsoleDialog.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Debug/wxGUI/dbgConsoleDialog.hpp"

#include "Features/Debug/dbgConsoleDialogUtil.hpp"
#include "Features/Debug/dbgConsoleStream.hpp"

#ifdef USE_WXWIDGETS
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#endif

#include "Core/Dbg/dbgMsg.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	dbgConsoleBuf* l_buf = NULL;
	dbgStringStream* l_dbgs = NULL;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AddDebugConsoleStream()
	{
		l_buf = new dbgConsoleBuf();
		l_dbgs = new dbgStringStream(l_buf);
		
		dbgMsg::addStream("debugConsole", l_dbgs);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const wxColour determineColor(std::string& i_pString)
	{
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

	wxColour textCol = determineColor(i_pString);
	m_richText_Console->BeginTextColour( textCol );
	m_richText_Console->AppendText( wxString(i_pString.c_str()) );
	m_richText_Console->EndSuppressUndo();
	m_richText_Console->Thaw();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dbgConsoleDialog::dbgConsoleDialog( wxWindow* parent, const std::string& i_Title )
:	dbgConsoleDialogBase(parent)
{
	// Add this panel to the AUI manager
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(i_Title).Caption(i_Title).Show().Layer(1).Left());
	AddDebugConsoleStream();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dbgConsoleDialog::~dbgConsoleDialog()
{
	dbgMsg::removeStream("debugConsole");
	if (dbgConsoleDialog::Instance == this)
		dbgConsoleDialog::Instance = NULL;
	
	delete l_buf;
	delete l_dbgs;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void dbgConsoleDialog::dbgConsoleDialog_OnActivate( wxActivateEvent& event )
{ 
	event.Skip(); 
}


#endif