/*****************************************************************************
**  twxToolbarMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twx/twxToolbarMgr.hpp"

#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#include "Core/Fs/fsFileUtil.hpp"

#include <map>


#ifdef USE_WXWIDGETS

#include <wx/artprov.h>


//============================================================================
//============================================================================
namespace
{
	class MyToolbar : public wxAuiToolBar
	//class MyToolbar : public wxToolBar
	{
	public:
		MyToolbar(wxWindow *i_pParent, wxWindowID i_Id)
			: wxAuiToolBar(i_pParent, i_Id), m_bInitialVisible(true) 
		{
			// Since we are drawing the background ourself, we can set the custom flag
			// to prevent flickering:
			this->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
		}
		wxBoxSizer* GetToolbarSizer()
		{
			return m_sizer;
		}

		bool m_bInitialVisible;
	};

	bool l_bRealized = false;
	std::vector<fsLocator> l_IconPathList;
	std::map<std::string, MyToolbar*> m_ToolbarMap;

	MyToolbar* get_or_create_toolbar(const std::string& i_Name)
	{
		std::map<std::string, MyToolbar*>::iterator it = m_ToolbarMap.find(i_Name);
		if (it == m_ToolbarMap.end())
		{
			DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
			MyToolbar* pToolbar = new MyToolbar(twxSystem::g_pMainForm, wxID_ANY);
	//		pToolbar->SetToolBitmapSize(wxSize(16,16));

			//twxPaneMgr::AddPane(pToolbar, wxAuiPaneInfo().
   //               Name(i_Name).Caption(i_Name).
   //               ToolbarPane().Top().Row(1). //.MinSize(32,-1).
   //               LeftDockable(false).RightDockable(false));

			m_ToolbarMap[i_Name] = pToolbar;
			return pToolbar;
		}
		return it->second;
	}

} // end of namespace


//---------------------------------------------------------------------------
// Create tool strip grouping with given name
//---------------------------------------------------------------------------
void twxToolbarMgr::AddToolBar(const char* i_ToolbarName)
{
	MyToolbar* pToolbar = get_or_create_toolbar(i_ToolbarName);	
}

//---------------------------------------------------------------------------
// Show the toolbar with the given name
//---------------------------------------------------------------------------
void twxToolbarMgr::Show(const char* i_ToolbarName, bool i_bVisible)
{
	MyToolbar* pToolbar = get_or_create_toolbar(i_ToolbarName);
	
	// If we have not realized the toolbars, then the pane manager 
	// doesn't know about them yet. So, set a boolean in order to
	// set that initial state later when realizing.
	if (l_bRealized)
		twxPaneMgr::Show(pToolbar, i_bVisible);
	else
		pToolbar->m_bInitialVisible = i_bVisible;
}

//---------------------------------------------------------------------------
// Returns true if the toolbar is currently visible
//---------------------------------------------------------------------------
bool twxToolbarMgr::IsVisible(const char* i_ToolbarName) 
{
	if (twxSystem::g_pMainForm)
	{
		std::map<std::string, MyToolbar*>::iterator it = m_ToolbarMap.find(i_ToolbarName);
		if (it != m_ToolbarMap.end())
		{
			MyToolbar* pToolbar = it->second;
			if (l_bRealized)
				return twxPaneMgr::IsVisible(pToolbar);
			else
				return pToolbar->m_bInitialVisible;
		}
	}
	return false;
}

//---------------------------------------------------------------------------
// Refreshes a toolbar, use when toggle states change
//---------------------------------------------------------------------------
void twxToolbarMgr::Refresh(const char* i_ToolbarName)
{
	if (twxSystem::g_pMainForm && l_bRealized)
	{
		std::map<std::string, MyToolbar*>::iterator it = m_ToolbarMap.find(i_ToolbarName);
		if (it != m_ToolbarMap.end())
		{
			MyToolbar* pToolbar = it->second;
			pToolbar->Refresh();
		}
	}
}

//---------------------------------------------------------------------------
// Set directory to use to find toolbar icons
//---------------------------------------------------------------------------
void twxToolbarMgr::SetIconDirectory( std::vector<fsLocator>& i_IconPathList )
{
	l_IconPathList = i_IconPathList;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void twxToolbarMgr::AddToolBarButton( int i_Id,
									  const char * i_ToolBarName, 
									  const char * i_ToolStripButton_Image, 
									  const char * i_ToolStripButton_Name,
									  bool i_bCheckable )
{
	MyToolbar* pToolbar = get_or_create_toolbar(i_ToolBarName);

	for( int i = 0; i < l_IconPathList.size(); ++i )
	{
		fsLocator icon_loc = l_IconPathList[i];
		icon_loc.Push(i_ToolStripButton_Image);
		if( !fsFileUtil::FileExists(icon_loc) )
			continue;

		itString icon_filename;
		fsFileUtil::LocatorToUnicodeString(icon_loc, icon_filename);
        
		wxLogNull nullLog;
		wxImage icon_image( icon_filename.GetString(), wxBITMAP_TYPE_PNG );

		icon_image.Rescale(24,24);
		wxBitmap icon_bitmap( icon_image );

		wxString button_name(i_ToolStripButton_Name, wxConvUTF8);
		if (i_bCheckable)
			pToolbar->AddTool(i_Id, button_name, icon_bitmap, wxNullBitmap, wxITEM_CHECK, 
				button_name, button_name, NULL);
		else
			pToolbar->AddTool(i_Id, button_name, icon_bitmap, button_name);
		//pToolbar->Realize();

		//stop traversing path list now that have set our image
		break;
	}
}

//---------------------------------------------------------------------------
// Set enabled state of toolbar button
//---------------------------------------------------------------------------
void twxToolbarMgr::EnableButton( int i_Id, bool i_bEnabled )
{
	if (twxSystem::g_pMainForm)
	{
		std::map<std::string, MyToolbar*>::iterator it;
		for (it = m_ToolbarMap.begin(); it != m_ToolbarMap.end(); ++it)
		{
			MyToolbar* pToolbar = it->second;
			if (pToolbar->FindTool(i_Id))
			{
				pToolbar->EnableTool(i_Id, i_bEnabled);
			}
		}
	}
}

//---------------------------------------------------------------------------
// Set toggled state of toolbar button
//---------------------------------------------------------------------------
void twxToolbarMgr::CheckButton( int i_Id, bool i_bChecked )
{
	if (twxSystem::g_pMainForm)
	{
		std::map<std::string, MyToolbar*>::iterator it;
		for (it = m_ToolbarMap.begin(); it != m_ToolbarMap.end(); ++it)
		{
			MyToolbar* pToolbar = it->second;
			if (pToolbar->FindTool(i_Id))
			{
				pToolbar->ToggleTool(i_Id, i_bChecked);
			}
		}
	}
}

//---------------------------------------------------------------------------
// Set Help string to display when mouse is over given tool item
//---------------------------------------------------------------------------
void twxToolbarMgr::SetToolItemHelpString( int i_ObjectID, const char * i_HelpString )
{
	if (twxSystem::g_pMainForm)
	{
		std::map<std::string, MyToolbar*>::iterator it;
		for (it = m_ToolbarMap.begin(); it != m_ToolbarMap.end(); ++it)
		{
			MyToolbar* pToolbar = it->second;
			if (pToolbar->FindTool(i_ObjectID))
			{
				pToolbar->SetToolLongHelp(i_ObjectID, wxString(i_HelpString, wxConvUTF8));
			}
		}
	}
}


//---------------------------------------------------------------------------
// Creates the actual toolbars. Should be called once all toolbar buttons
//	have been added during application startup.
//---------------------------------------------------------------------------
void twxToolbarMgr::RealizeAllToolbars()
{
	if (!l_bRealized)
	{
		std::map<std::string, MyToolbar*>::iterator it;
		for (it = m_ToolbarMap.begin(); it != m_ToolbarMap.end(); ++it)
		{
			it->second->Realize();
			wxString pane_name(it->first.c_str(), wxConvUTF8);
			twxPaneMgr::AddPane(it->second, wxAuiPaneInfo().
				  Name(pane_name).Caption(pane_name).
				  ToolbarPane().Top(). //Row(1). //MinSize(32,-1).
				  LeftDockable(false).RightDockable(false).
				  Show(it->second->m_bInitialVisible));
		}
		l_bRealized = true;
	}
}

//---------------------------------------------------------------------------
// Perspectives save the size of toolbars, but when new buttons are added,
// we need to make sure that the toolbars are big enough to show all of
// their buttons.
//---------------------------------------------------------------------------
void twxToolbarMgr::ResizeToolbars()
{
	if (l_bRealized)
	{
		std::map<std::string, MyToolbar*>::iterator it;
		for (it = m_ToolbarMap.begin(); it != m_ToolbarMap.end(); ++it)
		{
			//it->second->Fit(); // Resize the toolbar to fit the buttons
			//wxSize s = it->second->GetMinSize();
			wxSizer *pToolSizer = it->second->GetToolbarSizer();
			if (pToolSizer)
			{
				wxSize s = pToolSizer->GetMinSize();
				// Have to add in some extra width because of the gripper on the left.
				//twxPaneMgr::ResizePane(it->second, s.GetWidth()+20, s.GetHeight());
				twxPaneMgr::ResizePane(it->second, s.GetWidth(), s.GetHeight());
			}
		}
	}
}

//---------------------------------------------------------------------------
// Access to toolbar control, can be used as parent for controls that
//	will be added to the toolbar.
//---------------------------------------------------------------------------
wxAuiToolBar* twxToolbarMgr::GetToolbarByName(const char* i_ToolbarName)
{
	return get_or_create_toolbar(i_ToolbarName);
}	

//---------------------------------------------------------------------------
// Add a custom control to the toolbar with the given name
//---------------------------------------------------------------------------
void twxToolbarMgr::AddControlToToolBar(const char* i_ToolbarName,
										wxControl* i_pControl)
{
	MyToolbar* pToolbar = get_or_create_toolbar(i_ToolbarName);
	pToolbar->AddControl(i_pControl);
}

#endif // USE_WXWIDGETS
