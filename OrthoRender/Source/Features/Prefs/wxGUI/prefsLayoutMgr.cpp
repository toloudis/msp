/*****************************************************************************
**	prefsLayoutMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/wxGUI/prefsLayoutMgr.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"

#include "Core/Fs/fsFileUtil.hpp"
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
	const char * lc_Pane_Keyname			= "LastPerspective/Panes";
	const char * lc_MainForm_Size_Keyname	= "LastPerspective/MainForm/Size";
	const char * lc_MainForm_Max_Keyname	= "LastPerspective/MainForm/Maximized";

	const char * lc_LayoutsFileName			= "Layouts.cfg";
	const char * lc_LayoutsFileName_Default	= "Layouts-Default.cfg";
	const char * lc_DefaultLayoutName		= "Default Layout";

	const char * lc_Element_Layout		= "Layout";
	const char * lc_Key_Name			= "Name";
	const char * lc_Key_Perspective		= "Perspective";
	const char * lc_Key_Maximized		= "Maximized";
	const char * lc_Key_SizePos			= "SizePos";

	struct LayoutInfo
	{
		std::string m_Perspective;
		bool m_bMaximized;
		int m_Width, m_Height;
		int m_XPos, m_YPos;

		LayoutInfo() : m_bMaximized(false), m_Width(0), m_Height(0), m_XPos(0), m_YPos(0) {}
	};

	std::map<std::string, LayoutInfo> l_NamedLayouts;
	std::string l_CurrentLayoutName;

} // end if namespace

//------------------------------------------------------------------------
//	load in the configuration of panes from the last run
//------------------------------------------------------------------------
void prefsLayoutMgr::LoadLastLayout()
{
	wxConfig reg_config(mnmConstants::c_PRODUCT, mnmConstants::c_COMPANY);
	if (reg_config.HasEntry(lc_Pane_Keyname))
	{
		// Read last layout from registry
		wxString perspective;
		if ( reg_config.Read(lc_Pane_Keyname, &perspective) ) 
		{
			twxPaneMgr::SetCurrentPerspective(perspective.c_str());
			twxToolbarMgr::ResizeToolbars(); // confirm toolbars are big enough
		}
		wxString main_form;
		if ( reg_config.Read(lc_MainForm_Size_Keyname, &main_form) ) 
		{
			std::istringstream str(main_form.c_str());
			int width = 0, height = 0, x = 0, y = 0; 
			str >> width >> height >> x >> y;

			if (width > 0 && height > 0)
			{
				wxRect size_rect(x,y,width,height);
				twxSystem::g_pMainForm->SetSize(size_rect);
			}
		}
		bool bMaximized = false;
		if ( reg_config.Read(lc_MainForm_Max_Keyname, &bMaximized) ) 
		{
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
	wxConfig reg_config(mnmConstants::c_PRODUCT, mnmConstants::c_COMPANY);

	std::string perspective;
	if (twxPaneMgr::GetCurrentPerspective(perspective))
	{
		reg_config.Write(lc_Pane_Keyname, perspective);
	}

	if (twxSystem::g_pMainForm)
	{
		bool bMaximized = twxSystem::g_pMainForm->IsMaximized();
		reg_config.Write(lc_MainForm_Max_Keyname, bMaximized);

		// Size is only meaningful if we were not maximized
		if (!bMaximized)
		{
			wxSize size = twxSystem::g_pMainForm->GetSize();
			wxPoint pos = twxSystem::g_pMainForm->GetPosition();

			std::ostringstream str;
			str << size.GetWidth() << " " << size.GetHeight() << " " << pos.x << " " << pos.y;
			reg_config.Write(lc_MainForm_Size_Keyname, str.str());
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

		// Set size only if not maximized
		if ( !info.m_bMaximized ) 
		{
			if (info.m_Width > 0 && info.m_Height > 0)
			{
				wxRect size_rect(info.m_XPos,info.m_YPos,info.m_Width,info.m_Height);
				twxSystem::g_pMainForm->SetSize(size_rect);
			}
		}

		// Set perspective last, after window has been resized
		twxPaneMgr::SetCurrentPerspective(info.m_Perspective.c_str());
		twxToolbarMgr::ResizeToolbars(); // confirm toolbars are big enough
		
		sel3dMgr::Renotify(); // some toolbars should have visibility set from selection, not just layout

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
	l_NamedLayouts.erase(i_Name);
	l_CurrentLayoutName = "";
	WriteLayoutsToConfigFile();
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

	if (fsFileUtil::FileExists(cfgdir))
	{
		ReadLayoutsFromConfigFile(cfgdir);
	}
	else
	{
		// no hot key config, so check for default
		cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
		cfgdir.Push( lc_LayoutsFileName_Default );
		if (fsFileUtil::FileExists(cfgdir))
		{
			ReadLayoutsFromConfigFile(cfgdir);
		}
	}
}

#endif // USE_WXWIDGETS
