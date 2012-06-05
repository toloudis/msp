/****************************************************************************\
**	wxHelpUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/wxGUI/wxHelpUtil.hpp"
#include "MainApp/mainConstants.hpp"

#include "Support/mnm/mnmConstants.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#ifdef USE_WXWIDGETS
#include <wx/richtext/richtextctrl.h>
#endif

#include <list>
#include <sstream>
#include <iomanip>

namespace
{
	struct HotKeyCommandInfo
	{
		std::string m_Category;
		std::string m_CommandName;
		std::string m_HotKeyString;

		HotKeyCommandInfo(const std::string &i_Category,
							const std::string &i_CommandName,
							const std::string &i_HotKeyString)
			:	m_Category(i_Category), 
				m_CommandName(i_CommandName), 
				m_HotKeyString(i_HotKeyString) {}

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
    helpInfo <<  "F - Fur" << std::endl;
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
		helpInfo <<  std::setw(16) << it->m_HotKeyString << " - " << it->m_CommandName <<  std::endl;
	}

#ifdef USE_WXWIDGETS
	wxDialog helpInfoDialog(twxSystem::g_pMainForm, wxID_ANY, "Help Info", 
		wxDefaultPosition, wxSize(300,400));

	wxBoxSizer* bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxRichTextCtrl *richText1 = new wxRichTextCtrl( &helpInfoDialog, wxID_ANY, 
		helpInfo.str(), wxDefaultPosition, wxDefaultSize, 0|wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS );
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
	std::string stdTitle(mnmConstants::c_EXECUTABLE);
	// Strip off the file extension from the string
	int pos = stdTitle.rfind(".exe");
	if (pos != std::string::npos)
		stdTitle.erase(pos);

	// dialog message
	std::ostringstream helpMessage;
	helpMessage << mnmConstants::c_PRODUCT << " "
		<< mainConstants::mc_ExecutableVersion << std::endl << std::endl
		<< mnmConstants::c_COPYRIGHT << std::endl << "All rights reserved" << std::endl;

	guiMessageBox::Show( helpMessage.str().c_str(), stdTitle.c_str(), guiMessageBox::e_OKOnly );
}
