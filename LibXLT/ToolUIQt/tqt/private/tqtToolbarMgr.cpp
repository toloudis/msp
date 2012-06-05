/*****************************************************************************
**  tqtToolbarMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtToolbarMgr.hpp"

#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/Fs/fsFileUtil.hpp"

#include <map>

#ifdef QT_FINISH_PORT
#include <wx/artprov.h>
#endif

//============================================================================
//============================================================================
namespace
{
	class MyToolbar : public QToolBar
	{
	public:
#ifdef QT_FINISH_PORT
		MyToolbar(QWidget *i_pParent, QWidgetID i_Id)
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
#endif
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
#ifdef USE_QT
			DBG_ASSERT(tqtSystem::g_pMainForm, "MainForm not yet initialized.");
#endif
			MyToolbar* pToolbar = NULL;
#ifdef QT_FINISH_PORT
			pToolbar = new MyToolbar(tqtSystem::g_pMainForm, wxID_ANY);
#endif
			m_ToolbarMap[i_Name] = pToolbar;
			return pToolbar;
		}
		return it->second;
	}
} // end of namespace


//---------------------------------------------------------------------------
// Create tool strip grouping with given name
//---------------------------------------------------------------------------
void tqtToolbarMgr::AddToolBar(const char* i_ToolbarName)
{
	MyToolbar* pToolbar = get_or_create_toolbar(i_ToolbarName);	
}

//---------------------------------------------------------------------------
// Show the toolbar with the given name
//---------------------------------------------------------------------------
void tqtToolbarMgr::Show(const char* i_ToolbarName, bool i_bVisible)
{
	MyToolbar* pToolbar = get_or_create_toolbar(i_ToolbarName);
	
	// If we have not realized the toolbars, then the pane manager 
	// doesn't know about them yet. So, set a boolean in order to
	// set that initial state later when realizing.
	if (l_bRealized)
		tqtPaneMgr::Show(pToolbar, i_bVisible);
	else
		pToolbar->m_bInitialVisible = i_bVisible;
}

//---------------------------------------------------------------------------
// Returns true if the toolbar is currently visible
//---------------------------------------------------------------------------
bool tqtToolbarMgr::IsVisible(const char* i_ToolbarName) 
{
	if (tqtSystem::g_pMainForm != NULL)
	{
		std::map<std::string, MyToolbar*>::iterator it = m_ToolbarMap.find(i_ToolbarName);
		if (it != m_ToolbarMap.end())
		{
			MyToolbar* pToolbar = it->second;
			if (l_bRealized)
				return tqtPaneMgr::IsVisible(pToolbar);
			else
				return pToolbar->m_bInitialVisible;
		}
	}
	return false;
}

//---------------------------------------------------------------------------
// Refreshes a toolbar, use when toggle states change
//---------------------------------------------------------------------------
void tqtToolbarMgr::Refresh(const char* i_ToolbarName)
{
	if (tqtSystem::g_pMainForm && l_bRealized)
	{
		std::map<std::string, MyToolbar*>::iterator it = m_ToolbarMap.find(i_ToolbarName);
		if (it != m_ToolbarMap.end())
		{
			MyToolbar* pToolbar = it->second;
#ifdef QT_FINISH_PORT
			pToolbar->Refresh();
#endif
		}
	}
}

//---------------------------------------------------------------------------
// Set directory to use to find toolbar icons
//---------------------------------------------------------------------------
void tqtToolbarMgr::SetIconDirectory( std::vector<fsLocator>& i_IconPathList )
{
	l_IconPathList = i_IconPathList;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void tqtToolbarMgr::AddToolBarButton( int i_Id,
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
#ifdef QT_FINISH_PORT
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
#endif

		//stop traversing path list now that have set our image
		break;
	}
}

//---------------------------------------------------------------------------
// Set enabled state of toolbar button
//---------------------------------------------------------------------------
void tqtToolbarMgr::EnableButton( int i_Id, bool i_bEnabled )
{
	if (tqtSystem::g_pMainForm != NULL)
	{
		std::map<std::string, MyToolbar*>::iterator it;
		for (it = m_ToolbarMap.begin(); it != m_ToolbarMap.end(); ++it)
		{
			MyToolbar* pToolbar = it->second;
#ifdef QT_FINISH_PORT
			if (pToolbar->FindTool(i_Id))
			{
				pToolbar->EnableTool(i_Id, i_bEnabled);
			}
#endif
		}
	}
}

//---------------------------------------------------------------------------
// Set toggled state of toolbar button
//---------------------------------------------------------------------------
void tqtToolbarMgr::CheckButton( int i_Id, bool i_bChecked )
{
	if (tqtSystem::g_pMainForm != NULL)
	{
		std::map<std::string, MyToolbar*>::iterator it;
		for (it = m_ToolbarMap.begin(); it != m_ToolbarMap.end(); ++it)
		{
			MyToolbar* pToolbar = it->second;
#ifdef QT_FINISH_PORT
			if (pToolbar->FindTool(i_Id))
			{
				pToolbar->ToggleTool(i_Id, i_bChecked);
			}
#endif
		}
	}
}

//---------------------------------------------------------------------------
// Set Help string to display when mouse is over given tool item
//---------------------------------------------------------------------------
void tqtToolbarMgr::SetToolItemHelpString( int i_ObjectID, const char * i_HelpString )
{
	if (tqtSystem::g_pMainForm != NULL)
	{
		std::map<std::string, MyToolbar*>::iterator it;
		for (it = m_ToolbarMap.begin(); it != m_ToolbarMap.end(); ++it)
		{
			MyToolbar* pToolbar = it->second;
#ifdef QT_FINISH_PORT
			if (pToolbar->FindTool(i_ObjectID))
			{
				pToolbar->SetToolLongHelp(i_ObjectID, wxString(i_HelpString, wxConvUTF8));
			}
#endif
		}
	}
}


//---------------------------------------------------------------------------
// Creates the actual toolbars. Should be called once all toolbar buttons
//	have been added during application startup.
//---------------------------------------------------------------------------
void tqtToolbarMgr::RealizeAllToolbars()
{
	if (!l_bRealized)
	{
		std::map<std::string, MyToolbar*>::iterator it;
		for (it = m_ToolbarMap.begin(); it != m_ToolbarMap.end(); ++it)
		{
#ifdef QT_FINISH_PORT
			it->second->Realize();
			wxString pane_name(it->first.c_str(), wxConvUTF8);
			tqtPaneMgr::AddPane(it->second, wxAuiPaneInfo().
				  Name(pane_name).Caption(pane_name).
				  ToolbarPane().Top(). //Row(1). //MinSize(32,-1).
				  LeftDockable(false).RightDockable(false).
				  Show(it->second->m_bInitialVisible));
#endif
		}
		l_bRealized = true;
	}
}

//---------------------------------------------------------------------------
// Perspectives save the size of toolbars, but when new buttons are added,
// we need to make sure that the toolbars are big enough to show all of
// their buttons.
//---------------------------------------------------------------------------
void tqtToolbarMgr::ResizeToolbars()
{
	if (l_bRealized)
	{
		std::map<std::string, MyToolbar*>::iterator it;
		for (it = m_ToolbarMap.begin(); it != m_ToolbarMap.end(); ++it)
		{
#ifdef QT_FINISH_PORT
			//it->second->Fit(); // Resize the toolbar to fit the buttons
			//wxSize s = it->second->GetMinSize();
			wxSizer *pToolSizer = it->second->GetToolbarSizer();
			if (pToolSizer)
			{
				wxSize s = pToolSizer->GetMinSize();
				// Have to add in some extra width because of the gripper on the left.
				//tqtPaneMgr::ResizePane(it->second, s.GetWidth()+20, s.GetHeight());
				tqtPaneMgr::ResizePane(it->second, s.GetWidth(), s.GetHeight());
			}
#endif
		}
	}
}

#ifdef USE_QT
//---------------------------------------------------------------------------
// Access to toolbar control, can be used as parent for controls that
//	will be added to the toolbar.
//---------------------------------------------------------------------------
QToolBar* tqtToolbarMgr::GetToolbarByName(const char* i_ToolbarName)
{
	return get_or_create_toolbar(i_ToolbarName);
}	

//---------------------------------------------------------------------------
// Add a custom control to the toolbar with the given name
//---------------------------------------------------------------------------
void tqtToolbarMgr::AddControlToToolBar(const char* i_ToolbarName,
										QWidget* i_pControl)
{
	MyToolbar* pToolbar = get_or_create_toolbar(i_ToolbarName);
#ifdef QT_FINISH_PORT
	pToolbar->AddControl(i_pControl);
#endif
}
#endif // USE_QT
