/*****************************************************************************
**	prefsLayoutMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/wxGUI/prefsLayoutMgr.hpp"

#include "MainApp/wxGUI/wxMainForm.hpp"
#include "Features/RenderPanels/wxGUI/rpnPanelGrid.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"

#include "Core/Dbg/DbgMsg.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#ifdef USE_WXWIDGETS
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxToolbarMgr.hpp"
#endif

#ifdef USE_WXWIDGETS

#include <wx/config.h>
#include <sstream>
#include <map>


//============================================================================
//============================================================================
namespace
{
	const WCHAR * lc_Pane_Keyname			= L"LastPerspective/Panes";
	const WCHAR * lc_MainForm_Size_Keyname	= L"LastPerspective/MainForm/Size";
	const WCHAR * lc_MainForm_Max_Keyname	= L"LastPerspective/MainForm/Maximized";

	const itString lc_LayoutsFileName(L"Layouts.cfg");
	const itString lc_LayoutsFileName_Default(L"Layouts-Default.cfg");
	const char * lc_DefaultLayoutName		= "Default Layout";

	const char * lc_Element_Layout		= "Layout";
	const char * lc_Key_Name			= "Name";
	const char * lc_Key_Perspective		= "Perspective";
	const char * lc_Key_Maximized		= "Maximized";
	const char * lc_Key_SizePos			= "SizePos";
	//const char * lc_Key_StatusBar		= "StatusBar";
	const char * lc_Key_PanelLayout		= "PanelLayout";
	const char * lc_Key_RenderWidth		= "RenderWidth";
	const char * lc_Key_RenderHeight	= "RenderHeight";

	struct LayoutInfo
	{
		std::string m_Perspective;
		bool m_bMaximized;
		int m_Width, m_Height;
		int m_XPos, m_YPos;

		// Some more attributes added...
		//bool m_bStatusBarVisible;
		rpnPanelGrid::LayoutStyle m_PanelLayout;
		int m_RenderWidth, m_RenderHeight;

		LayoutInfo() : m_bMaximized(false), m_Width(0), m_Height(0), m_XPos(0), m_YPos(0),
			//m_bStatusBarVisible(true), 
			m_PanelLayout(rpnPanelGrid::e_SinglePane),
			m_RenderWidth(0), m_RenderHeight(0) {}
	};

	std::map<std::string, LayoutInfo> l_NamedLayouts;
	std::string l_CurrentLayoutName;

	//------------------------------------------------------------------------
	// Use a string for layout XML file in order to handle changes to 
	// the layout enumeration gracefully
	//------------------------------------------------------------------------
	std::string get_panel_string(rpnPanelGrid::LayoutStyle i_Layout)
	{
		switch (i_Layout)
		{
		default:
		case rpnPanelGrid::e_SinglePane:
			return "SinglePane";
		case rpnPanelGrid::e_TwoStacked:
			return "TwoStacked";
		case rpnPanelGrid::e_FourPanels:
			return "FourPanels";
		case rpnPanelGrid::e_TwoSideBySide:
			return "TwoSideBySide";
		}
	}
	rpnPanelGrid::LayoutStyle convert_panel_string(const std::string &i_String)
	{
		if (i_String == "TwoStacked")
			return rpnPanelGrid::e_TwoStacked;
		else if (i_String == "FourPanels")
			return rpnPanelGrid::e_FourPanels;
		else if (i_String == "TwoSideBySide")
			return rpnPanelGrid::e_TwoSideBySide;
		else 
			return rpnPanelGrid::e_SinglePane;
	}

} // end if namespace

//------------------------------------------------------------------------
//	load in the configuration of panes from the last run
//------------------------------------------------------------------------
void prefsLayoutMgr::LoadLastLayout()
{
	wxString product_name(mnmConstants::c_PRODUCT, wxConvUTF8);
	wxString company_name(mnmConstants::c_COMPANY, wxConvUTF8);
	wxConfig reg_config(product_name, company_name);
	if (reg_config.HasEntry(lc_Pane_Keyname))
	{
		// Read last layout from registry
		wxString perspective;
		if ( reg_config.Read(lc_Pane_Keyname, &perspective) ) 
		{
			twxPaneMgr::SetCurrentPerspective(std::string(perspective.utf8_str()));
			twxToolbarMgr::ResizeToolbars(); // confirm toolbars are big enough
		}
		wxString main_form;
		if ( reg_config.Read(lc_MainForm_Size_Keyname, &main_form) ) 
		{
			std::string main_form_buffer(main_form.utf8_str());
			std::istringstream str(main_form_buffer);
			int width = 0, height = 0, x = 0, y = 0; 
			str >> width >> height >> x >> y;

			if (width > 100 && height > 100)
			{
				wxRect size_rect(x,y,width,height);
				twxSystem::g_pMainForm->SetSize(size_rect);
			}
		}
		bool bMaximized = false;
		if ( reg_config.Read(lc_MainForm_Max_Keyname, &bMaximized) ) 
		{
			if (bMaximized != twxSystem::g_pMainForm->IsMaximized())
				twxSystem::g_pMainForm->Maximize(bMaximized);
		}
	}
	else
	{
		// No layout stored in registry, this must be the first run.
		// In this case, look for a default layout in our named layouts
		// loaded from the default configuration file.
		SwitchToNamedLayout(lc_DefaultLayoutName);
	}

	sel3dMgr::Renotify(); // some toolbars should have visibility set from selection, not just layout
}

//------------------------------------------------------------------------
//	save the current configuration of panes to the registry
//------------------------------------------------------------------------
void prefsLayoutMgr::SaveLastLayout()
{
	wxString product_name(mnmConstants::c_PRODUCT, wxConvUTF8);
	wxString company_name(mnmConstants::c_COMPANY, wxConvUTF8);
	wxConfig reg_config(product_name, company_name);

	std::string perspective;
	if (twxPaneMgr::GetCurrentPerspective(perspective))
	{
		reg_config.Write(lc_Pane_Keyname, wxString(perspective.c_str(), wxConvUTF8) );
	}

	if (twxSystem::g_pMainForm)
	{
		bool bMaximized = twxSystem::g_pMainForm->IsMaximized();
		reg_config.Write(lc_MainForm_Max_Keyname, bMaximized);

		// Size is only meaningful if we are not maximized or minimized
		bool bMinimized = twxSystem::g_pMainForm->IsIconized();
		if (!bMaximized && !bMinimized)
		{
			wxSize size = twxSystem::g_pMainForm->GetSize();
			wxPoint pos = twxSystem::g_pMainForm->GetPosition();

			std::ostringstream str;
			str << size.GetWidth() << " " << size.GetHeight() << " " << pos.x << " " << pos.y;
			reg_config.Write(lc_MainForm_Size_Keyname, wxString(str.str().c_str(), wxConvUTF8));
		}
	}
}

//------------------------------------------------------------------------
// Get strings for names of layouts
//------------------------------------------------------------------------
void prefsLayoutMgr::GetLayoutNames(std::vector<std::string>& o_LayoutNames)
{
	o_LayoutNames.clear();
	std::map<std::string, LayoutInfo>::iterator it;
	for (it = l_NamedLayouts.begin(); it != l_NamedLayouts.end(); ++it)
	{
		o_LayoutNames.push_back(it->first);
	}
}

//------------------------------------------------------------------------
// Get name of last layout that was saved or switch to.
// Can be empty string.
//------------------------------------------------------------------------
const std::string& prefsLayoutMgr::GetCurrentLayoutName()
{
	return l_CurrentLayoutName;
}

//------------------------------------------------------------------------
//	save current layout as a named layout to save to config file
//------------------------------------------------------------------------
bool prefsLayoutMgr::SaveNamedLayout(const std::string &i_Name)
{
	LayoutInfo current_layout;
	if (twxPaneMgr::GetCurrentPerspective(current_layout.m_Perspective))
	{
		if (twxSystem::g_pMainForm)
		{
			current_layout.m_bMaximized = twxSystem::g_pMainForm->IsMaximized();

			// Size is only meaningful if we were not maximized
			if (!current_layout.m_bMaximized)
			{
				wxSize size = twxSystem::g_pMainForm->GetSize();
				current_layout.m_Width = size.GetWidth();
				current_layout.m_Height = size.GetHeight();
				wxPoint pos = twxSystem::g_pMainForm->GetPosition();
				current_layout.m_XPos = pos.x;
				current_layout.m_YPos = pos.y;
			}
			
			// Some new properties to save also
			//current_layout.m_bStatusBarVisible = wxMainForm::GetStatusBarVisible();
	
			if (rpnPanelGrid::Instance)
			{
				current_layout.m_PanelLayout = rpnPanelGrid::Instance->GetLayoutStyle();
			}
			
			int render_width = 0, render_height = 0;
			wxMainForm::GetRenderWindowSize(render_width, render_height);
			if (render_height > 0 && render_width > 0)
			{
				current_layout.m_RenderWidth = render_width;
				current_layout.m_RenderHeight = render_height;
			}

			// Create or overwrite new named layout
			l_NamedLayouts[i_Name] = current_layout;
			l_CurrentLayoutName = i_Name;

			WriteLayoutsToConfigFile();

			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------
//	Change Layout to named layout configuration
//------------------------------------------------------------------------
bool prefsLayoutMgr::SwitchToNamedLayout(const std::string &i_Name)
{
	std::map<std::string, LayoutInfo>::iterator it = l_NamedLayouts.find(i_Name);
	if (it != l_NamedLayouts.end())
	{
		LayoutInfo &info = it->second;

		// Set Maximized state
		twxSystem::g_pMainForm->Maximize(info.m_bMaximized);

		// Render resoution should be set before the main window size is set below
		if (info.m_RenderWidth > 0 && info.m_RenderHeight > 0)
		{
			wxMainForm::ResizeRenderWindow(info.m_RenderWidth, info.m_RenderHeight);
		}

		// Set size only if not maximized
		if ( !info.m_bMaximized ) 
		{
			if (info.m_Width > 0 && info.m_Height > 0)
			{
				wxRect size_rect(info.m_XPos,info.m_YPos,info.m_Width,info.m_Height);
				twxSystem::g_pMainForm->SetSize(size_rect);
			}
		}

		// Panel layout
		if (rpnPanelGrid::Instance)
		{
			rpnPanelGrid::Instance->SetLayoutStyle( info.m_PanelLayout );
		}

		// Set perspective last, after window has been resized
		twxPaneMgr::SetCurrentPerspective(info.m_Perspective.c_str());
		twxToolbarMgr::ResizeToolbars(); // confirm toolbars are big enough
		
		sel3dMgr::Renotify(); // some toolbars should have visibility set from selection, not just layout

		// Status Bar is very last because it also causes a PaneMgr Update()
		//wxMainForm::SetStatusBarVisible( info.m_bStatusBarVisible );

		l_CurrentLayoutName = i_Name;

		return true;
	}
	return false;
}

//------------------------------------------------------------------------
//	Change Layout in a cycle through the names
//------------------------------------------------------------------------
void prefsLayoutMgr::SwitchToNextLayout()
{
	std::map<std::string, LayoutInfo>::iterator it = 
		(l_CurrentLayoutName.empty()) ? l_NamedLayouts.begin() : l_NamedLayouts.find(l_CurrentLayoutName);
	if (it != l_NamedLayouts.end())
	{
		// Go to next layout
		++it;
		if (it == l_NamedLayouts.end())
			it = l_NamedLayouts.begin();

		SwitchToNamedLayout(it->first);
	}
}
void prefsLayoutMgr::SwitchToPrevLayout()
{
	std::map<std::string, LayoutInfo>::iterator it = 
		(l_CurrentLayoutName.empty()) ? l_NamedLayouts.begin() : l_NamedLayouts.find(l_CurrentLayoutName);
	if (it != l_NamedLayouts.end())
	{
		// Go to next layout
		if (it == l_NamedLayouts.begin())
			it = l_NamedLayouts.end();
		
		--it;

		SwitchToNamedLayout(it->first);
	}
}

//------------------------------------------------------------------------
//	remove layout with given name
//------------------------------------------------------------------------
void prefsLayoutMgr::DeleteNamedLayout(const std::string &i_Name)
{
	//	if the name is empty or the default name, don't allow it
	//	to be deleted.
	//
	if (   (!prefsLayoutMgr::GetCurrentLayoutName().empty())
		&& (strcmp(lc_DefaultLayoutName,i_Name.c_str()) !=0))
	{
		l_NamedLayouts.erase(i_Name);
		l_CurrentLayoutName = "";
		WriteLayoutsToConfigFile();
	}
}

//------------------------------------------------------------------------
// Write layout information to XML file in Configs directory
//------------------------------------------------------------------------
void prefsLayoutMgr::WriteLayoutsToConfigFile()
{
	fsLocator cfgdir;
	cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
	cfgdir.Push(lc_LayoutsFileName);
	std::string cfgpath;
	fsFileUtil::LocatorToANSIFilename(cfgdir,cfgpath);

	//	quick and dirty XML Writer
	guiXMLTextWriter::Open(cfgpath.c_str());

	guiXMLTextWriter::WriteStartElement("Layouts");

	// Write each layout in its own element
	std::map<std::string, LayoutInfo>::iterator it;
	for (it = l_NamedLayouts.begin(); it != l_NamedLayouts.end(); ++it)
	{
		guiXMLTextWriter::WriteStartElement(lc_Element_Layout);

		//	write the actual layout values
		guiXMLTextWriter::WriteElement(lc_Key_Name, it->first);
		guiXMLTextWriter::WriteElement(lc_Key_Perspective, it->second.m_Perspective);
		guiXMLTextWriter::WriteElement(lc_Key_Maximized, it->second.m_bMaximized);

		if (!it->second.m_bMaximized)
		{
			// Encode size and position into one string value
			std::ostringstream str;
			str << it->second.m_Width << " " << it->second.m_Height << " " << it->second.m_XPos << " " << it->second.m_YPos;
			guiXMLTextWriter::WriteElement(lc_Key_SizePos, str.str());
		}

		//guiXMLTextWriter::WriteElement(lc_Key_StatusBar, it->second.m_bStatusBarVisible);
		guiXMLTextWriter::WriteElement(lc_Key_PanelLayout, get_panel_string(it->second.m_PanelLayout) );
		guiXMLTextWriter::WriteElement(lc_Key_RenderWidth, it->second.m_RenderWidth);
		guiXMLTextWriter::WriteElement(lc_Key_RenderHeight, it->second.m_RenderHeight);

		//	finish up layout chunk
		guiXMLTextWriter::WriteEndElement();
	}
	//	finish up file
	guiXMLTextWriter::WriteEndElement();

	guiXMLTextWriter::Close();
}		


//------------------------------------------------------------------------
// Read layout information from given XML file
//------------------------------------------------------------------------
void prefsLayoutMgr::ReadLayoutsFromConfigFile(const fsLocator& i_ConfigFile)
{
	std::string cfgpath;
	fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

	guiXMLTextReader::Open(cfgpath.c_str());

	//	read in the preferences
	//
	guiXMLTextReader::gui_Node_Type node_type;
	std::string keyname, strvalue;
	while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
	{
		if ((node_type == guiXMLTextReader::e_Element)
			&& (keyname ==lc_Element_Layout))
		{
			// Read in a new layout
			std::string layout_name;
			LayoutInfo info;
			while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
			{
				if ((node_type == guiXMLTextReader::e_EndElement)
					&& (keyname ==lc_Element_Layout))
				{
					// Store layout data into our data structure
					if (!layout_name.empty())
						l_NamedLayouts[layout_name] = info;
					break;
				}
				else if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
				{
					if (keyname == lc_Key_Name)
						{ layout_name = strvalue; }
					else if (keyname == lc_Key_Perspective)
						{ info.m_Perspective = strvalue; }
					else if (keyname == lc_Key_Maximized)
						{ guiXMLTextReader::Convert(strvalue, info.m_bMaximized); }
					else if (keyname == lc_Key_SizePos)
					{ 
						std::istringstream str(strvalue);
						str >> info.m_Width >> info.m_Height >> info.m_XPos >> info.m_YPos;
					}
					//else if (keyname == lc_Key_StatusBar)
					//	{ guiXMLTextReader::Convert(strvalue, info.m_bStatusBarVisible); }
					else if (keyname == lc_Key_PanelLayout)
					{ 
						info.m_PanelLayout = convert_panel_string(strvalue);
					}
					else if (keyname == lc_Key_RenderWidth)
						{ guiXMLTextReader::Convert(strvalue, info.m_RenderWidth); }
					else if (keyname == lc_Key_RenderHeight)
						{ guiXMLTextReader::Convert(strvalue, info.m_RenderHeight); }

				}
			}
		}
	}

	guiXMLTextReader::Close();
}

//------------------------------------------------------------------------
// Read layout information from XML file in Configs directory
//	or from the default layout file installed with the application.
//------------------------------------------------------------------------
void prefsLayoutMgr::ReadLayouts()
{
	std::string cfgpath;
	fsLocator cfgdir;
	cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
	cfgdir.Push(lc_LayoutsFileName);
	DBG_TRACE("Layout config =" << cfgdir);

	if (fsFileUtil::FileExists(cfgdir))
	{
		ReadLayoutsFromConfigFile(cfgdir);
	}
	else
	{
		// no hot key config, so check for default
		cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
		cfgdir.Push( lc_LayoutsFileName_Default );
		DBG_TRACE("Layout default config =" << cfgdir);

		if (fsFileUtil::FileExists(cfgdir))
		{
			ReadLayoutsFromConfigFile(cfgdir);
		}
	}
}

#endif // USE_WXWIDGETS
