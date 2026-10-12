/*****************************************************************************
**	prefsLayoutMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Prefs/qtGUI/prefsLayoutMgr.hpp"

#include "MainApp/qtGUI/qtMainForm.hpp"
#include "Features/RenderPanels/qtGUI/rpnPanelGrid.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"

#include "Core/Dbg/DbgMsg.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#ifdef USE_QT
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#include "ToolUIQt/tqt/tqtToolbarMgr.hpp"
#endif

#ifdef USE_QT

#include <QSettings>
#include <sstream>
#include <map>


//============================================================================
//============================================================================
namespace
{
	// The Qt build saves its pane layouts under its own keys and in its own
	// file (the wxWidgets build can't read them). It can read the wxWidgets
	// ones, so it starts from the last wxWidgets layout until it has saved
	// one of its own.
	const char * lc_Pane_Keyname			= "LastPerspectiveQt/Panes";
	const char * lc_MainForm_Size_Keyname	= "LastPerspectiveQt/MainForm/Size";
	const char * lc_MainForm_Max_Keyname	= "LastPerspectiveQt/MainForm/Maximized";
	const char * lc_WxPane_Keyname			= "LastPerspective/Panes";
	const char * lc_WxMainForm_Size_Keyname	= "LastPerspective/MainForm/Size";
	const char * lc_WxMainForm_Max_Keyname	= "LastPerspective/MainForm/Maximized";

	const itString lc_LayoutsFileName(L"Layouts-Qt.cfg");
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

	void set_maximized(bool i_bMaximized)
	{
		if (i_bMaximized)
			tqtSystem::g_pMainForm->showMaximized();
		else if (tqtSystem::g_pMainForm->isMaximized())
			tqtSystem::g_pMainForm->showNormal();
	}

} // end if namespace

//------------------------------------------------------------------------
//	load in the configuration of panes from the last run
//------------------------------------------------------------------------
void prefsLayoutMgr::LoadLastLayout()
{
	QSettings reg_config(QString::fromUtf8(mnmConstants::c_COMPANY), QString::fromUtf8(mnmConstants::c_PRODUCT));
	const char * pane_key = lc_Pane_Keyname;
	const char * size_key = lc_MainForm_Size_Keyname;
	const char * max_key = lc_MainForm_Max_Keyname;
	if (!reg_config.contains(pane_key) && reg_config.contains(lc_WxPane_Keyname))
	{
		pane_key = lc_WxPane_Keyname;
		size_key = lc_WxMainForm_Size_Keyname;
		max_key = lc_WxMainForm_Max_Keyname;
	}

	if (reg_config.contains(pane_key))
	{
		// Read last layout from registry
		QString perspective = reg_config.value(pane_key).toString();
		if ( !perspective.isEmpty() )
		{
			tqtPaneMgr::SetCurrentPerspective(perspective.toStdString());
			tqtToolbarMgr::ResizeToolbars(); // confirm toolbars are big enough
		}

		QString main_form = reg_config.value(size_key).toString();
		if ( !main_form.isEmpty() )
		{
			std::string main_form_buffer(main_form.toStdString());
			std::istringstream str(main_form_buffer);
			int width = 0, height = 0, x = 0, y = 0;
			str >> width >> height >> x >> y;
			if (width > 100 && height > 100)
			{
				tqtSystem::g_pMainForm->setGeometry(x, y, width, height);
			}
		}

		if ( reg_config.contains(max_key) )
		{
			bool bMaximized = reg_config.value(max_key).toBool();
			if (bMaximized != tqtSystem::g_pMainForm->isMaximized())
				set_maximized(bMaximized);
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
	QSettings reg_config(QString::fromUtf8(mnmConstants::c_COMPANY), QString::fromUtf8(mnmConstants::c_PRODUCT));

	std::string perspective;
	if (tqtPaneMgr::GetCurrentPerspective(perspective))
	{
		reg_config.setValue(lc_Pane_Keyname, QString::fromStdString(perspective));
	}

	if (tqtSystem::g_pMainForm)
	{
		bool bMaximized = tqtSystem::g_pMainForm->isMaximized();
		reg_config.setValue(lc_MainForm_Max_Keyname, bMaximized);

		// Size is only meaningful if we are not maximized or minimized
		bool bMinimized = tqtSystem::g_pMainForm->isMinimized();
		if (!bMaximized && !bMinimized)
		{
			const QRect rect = tqtSystem::g_pMainForm->geometry();
			std::ostringstream str;
			str << rect.width() << " " << rect.height() << " " << rect.x() << " " << rect.y();
			reg_config.setValue(lc_MainForm_Size_Keyname, QString::fromStdString(str.str()));
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
	if (tqtPaneMgr::GetCurrentPerspective(current_layout.m_Perspective))
	{
		if (tqtSystem::g_pMainForm)
		{
			current_layout.m_bMaximized = tqtSystem::g_pMainForm->isMaximized();

			// Size is only meaningful if we were not maximized
			if (!current_layout.m_bMaximized)
			{
				const QRect rect = tqtSystem::g_pMainForm->geometry();
				current_layout.m_Width = rect.width();
				current_layout.m_Height = rect.height();
				current_layout.m_XPos = rect.x();
				current_layout.m_YPos = rect.y();
			}
			
			// Some new properties to save also
			//current_layout.m_bStatusBarVisible = qtMainForm::GetStatusBarVisible();
	
			if (rpnPanelGrid::Instance)
			{
				current_layout.m_PanelLayout = rpnPanelGrid::Instance->GetLayoutStyle();
			}
			
			int render_width = 0, render_height = 0;
			qtMainForm::GetRenderWindowSize(render_width, render_height);
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
		set_maximized(info.m_bMaximized);

		// Render resoution should be set before the main window size is set below
		if (info.m_RenderWidth > 0 && info.m_RenderHeight > 0)
		{
			qtMainForm::ResizeRenderWindow(info.m_RenderWidth, info.m_RenderHeight);
		}

		// Set size only if not maximized
		if ( !info.m_bMaximized ) 
		{
			if (info.m_Width > 0 && info.m_Height > 0)
			{
				tqtSystem::g_pMainForm->setGeometry(info.m_XPos, info.m_YPos, info.m_Width, info.m_Height);
			}
		}

		// Panel layout
		if (rpnPanelGrid::Instance)
		{
			rpnPanelGrid::Instance->SetLayoutStyle( info.m_PanelLayout );
		}

		// Set perspective last, after window has been resized
		tqtPaneMgr::SetCurrentPerspective(info.m_Perspective);
		tqtToolbarMgr::ResizeToolbars(); // confirm toolbars are big enough
		
		sel3dMgr::Renotify(); // some toolbars should have visibility set from selection, not just layout

		// Status Bar is very last because it also causes a PaneMgr Update()
		//qtMainForm::SetStatusBarVisible( info.m_bStatusBarVisible );

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

#endif // USE_QT
