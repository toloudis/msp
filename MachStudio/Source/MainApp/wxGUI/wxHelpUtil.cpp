/****************************************************************************\
**	wxHelpUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/wxGUI/wxHelpUtil.hpp"
#include "MainApp/mainConstants.hpp"

#include "Support/mnm/mnmConstants.hpp"

#include "Core/it/itString.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#ifdef USE_WXWIDGETS
#include <wx/richtext/richtextctrl.h>
#endif

#include <list>
#include <sstream>
#include <iomanip>


//============================================================================
//============================================================================
namespace
{
	//========================================================================
	//========================================================================
	struct HotKeyCommandInfo
	{
		std::string m_Category;
		std::string m_CommandName;
		std::string m_HotKeyString;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		HotKeyCommandInfo(const std::string &i_Category,
							const std::string &i_CommandName,
							const std::string &i_HotKeyString)
		:	m_Category(i_Category), 
			m_CommandName(i_CommandName), 
			m_HotKeyString(i_HotKeyString) {}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		bool operator<(const HotKeyCommandInfo& i_Info) const
		{
			if (m_Category == i_Info.m_Category)
				return (m_CommandName < i_Info.m_CommandName);
			else
				return (m_Category < i_Info.m_Category);
		}
	};

}	// end of namespace


//--------------------------------------------------------------------
// Show dialog with some help information about codes and hot keys.
//--------------------------------------------------------------------
void wxHelpUtil::ShowHelpInfo()
{
	// Set the default text of the control.
	std::ostringstream helpInfo;
	helpInfo <<  "Status Bar Render Flags" << std::endl << std::endl;
	helpInfo <<  "H/L - Render Type" << std::endl;
	helpInfo <<  "S - Shadows" << std::endl;
	helpInfo <<  "M - Matte" << std::endl;
	helpInfo <<  "D - Depth of Field" << std::endl;
	helpInfo <<  "G - Glow" << std::endl;
	helpInfo <<  "A - Ambient Pass" << std::endl;
	helpInfo <<  "L - Lit Pass" << std::endl;
	helpInfo <<  "T - Transparent" << std::endl;
	helpInfo <<  "R - Reflection" << std::endl;
	helpInfo <<  std::endl;
	helpInfo <<  "Camera Movement" << std::endl << std::endl;
	helpInfo <<  "Left, Right Arrows - Camera Yaw" << std::endl;
	helpInfo <<  "Up, Down Arrows - Camera Pitch" << std::endl;
	helpInfo <<  "CTRL + arrows - slows down arrow movements" << std::endl;
	helpInfo <<  "ALT + mouse button 0 - move camera position" << std::endl;
	helpInfo <<  "ALT + mouse button 1 - move camera pos+tar (strafe)" << std::endl;
	helpInfo <<  "ALT + mouse wheel - zoom in/out" << std::endl;
	helpInfo <<  "CTRL + mouse button 0 - move camera target" << std::endl;
	helpInfo <<  "CTRL + mouse button 1 - n" << std::endl;
	helpInfo <<  "CTRL + ALT + mouse button 0 - nothing" << std::endl;
	helpInfo <<  "CTRL + ALT + mouse button 1 - nothing" << std::endl;
	helpInfo <<  "CTRL + ALT + mouse button 0 + 1 - zoom in/out" << std::endl;
	helpInfo <<  std::endl;
	helpInfo <<  "Selecting Objects" << std::endl;
	helpInfo <<  std::endl;
	helpInfo <<  "mouse button 0 - select object" << std::endl;
	helpInfo <<  "LEFT SHIFT + mouse button 0 - accumulate select objects" << std::endl;
	helpInfo <<  "mouse button 1 - deselect object" << std::endl;
	helpInfo <<  std::endl;

	//	Add the custom hot keys
	//
	const std::vector<cmaCommand*>& cmds = cmaCommandMgr::GetCommandList();
	const int num_commands = cmds.size();
	std::list<HotKeyCommandInfo> cmd_infos;
	for (int j = 0; j < num_commands; ++j)
	{
		if (cmds[j]->GetHotKeyString().length() > 0)
		{
			cmd_infos.push_back(HotKeyCommandInfo(cmds[j]->GetCategory(), cmds[j]->GetTag(), cmds[j]->GetHotKeyString()));
		}
	}
	// Sort commands by category and then by name
	cmd_infos.sort();

	std::string lastcat = "";
	std::list<HotKeyCommandInfo>::const_iterator it;
	for (it = cmd_infos.begin(); it != cmd_infos.end(); ++it)
	{
		if (lastcat != it->m_Category)
		{
			helpInfo <<  std::endl << it->m_Category <<  std::endl;
			lastcat = it->m_Category;
		}
		helpInfo <<  /*std::setw(16) << */ it->m_HotKeyString << " - " << it->m_CommandName <<  std::endl;
	}

#ifdef USE_WXWIDGETS
	wxDialog helpInfoDialog(twxSystem::g_pMainForm, wxID_ANY, L"Help Info", 
		wxDefaultPosition, twxSystem::g_pMainForm->FromDIP(wxSize(300,400)));

	wxBoxSizer* bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxString help_str(helpInfo.str().c_str(), wxConvUTF8);
	wxRichTextCtrl *richText1 = new wxRichTextCtrl( &helpInfoDialog, wxID_ANY, 
		help_str, wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
	richText1->SetEditable(false);
	//richText1->SetFont(*wxSWISS_FONT);
	bSizer1->Add( richText1, 1, wxEXPAND | wxALL, 5 );
	
	helpInfoDialog.SetSizer( bSizer1 );
	helpInfoDialog.Layout();

	helpInfoDialog.ShowModal();
#endif
}

//--------------------------------------------------------------------
// Show dialog with version information
//--------------------------------------------------------------------
void wxHelpUtil::ShowHelpAbout()
{
	// dialog title
	itString Title( "About " );
	Title += mnmConstants::cw_PRODUCT;

	// dialog message
	itString helpMessage = itString(mnmConstants::c_PRODUCT_FOR_DISPLAY);
	helpMessage += L" ";
#ifdef WIN64
	helpMessage += itString(mnmConstants::cw_64BIT);
#else
	#ifdef WIN32
	helpMessage += itString(mnmConstants::cw_32BIT);
	#endif
#endif
	helpMessage += L" (";
	helpMessage += itString(mainConstants::mc_ExecutableVersion);
	helpMessage += L")";
	helpMessage += L"\r\n";
	helpMessage += L"\r\n";
	helpMessage += mnmConstants::cw_COPYRIGHT;
	helpMessage += L"\r\n";
	helpMessage += mnmConstants::cw_RIGHTS;
	helpMessage += L"\r\n";

	guiMessageBox::Show( helpMessage, Title, guiMessageBox::e_OKOnly);
}

