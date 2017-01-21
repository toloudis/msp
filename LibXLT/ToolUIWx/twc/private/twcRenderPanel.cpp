/*****************************************************************************
**  twcRenderPanel.cpp
**
**     Window using wxWidgets for doing MachStudio rendering
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcRenderPanel.hpp"

#include "Core/app/appTime.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dRenderView.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"

#ifdef USE_WXWIDGETS

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(twcRenderPanel, wxControl)
	EVT_ERASE_BACKGROUND(twcRenderPanel::OnEraseBackground)
	EVT_PAINT(twcRenderPanel::OnPaint)
	EVT_LEFT_DOWN(twcRenderPanel::OnMouseDown)
	EVT_MIDDLE_DOWN(twcRenderPanel::OnMouseDown)
	EVT_RIGHT_DOWN(twcRenderPanel::OnMouseDown)
	EVT_MOTION(twcRenderPanel::OnMouseMove)
	EVT_ENTER_WINDOW(twcRenderPanel::OnEnter)
	EVT_LEAVE_WINDOW(twcRenderPanel::OnLeave)
    EVT_SIZE(twcRenderPanel::OnSize)
	EVT_KEY_UP(twcRenderPanel::OnKeyUp)
	EVT_SET_FOCUS(twcRenderPanel::OnFocus)
END_EVENT_TABLE()


//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
twcRenderPanel::twcRenderPanel(wxWindow* parent, g2dSystem* i_pSystem)
	: wxControl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
	m_pSystem(i_pSystem),
	 m_pViewer(NULL)
{
	this->SetBackgroundColour( *wxBLACK );

	m_pWindow = i_pSystem->CreateSubWindow(this->GetHandle());
	m_pViewer = new g3dViewer(m_pWindow, NULL);
	m_pViewer->SetBackgroundColor(g2dRGBColor(0,0,0));
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
twcRenderPanel::~twcRenderPanel()
{
	delete m_pViewer;
	// hopefully g2dSystem is still alive? Is there a better way to access this globally?
	m_pSystem->DestroyWindow(m_pWindow);
}

//----------------------------------------------------------------------------
// The renderer is not owned by the viewer, just pointed to
//----------------------------------------------------------------------------
g3dSceneRenderer* twcRenderPanel::GetRenderer() const
{
	return m_pViewer->GetRenderer();
}
void twcRenderPanel::SetRenderer(g3dSceneRenderer* i_pRenderer)
{
	m_pViewer->SetRenderer(i_pRenderer);
}

//----------------------------------------------------------------------------
// The scene is not owned by the viewer, just pointed to
//----------------------------------------------------------------------------
g3dScene* twcRenderPanel::GetScene() const
{
	return m_pViewer->GetScene();
}
void twcRenderPanel::SetScene(g3dScene* i_pScene)
{
	m_pViewer->SetScene(i_pScene);
}

//----------------------------------------------------------------------------
// The camera is not owned by the viewer, just pointed to
//----------------------------------------------------------------------------
camCamera* twcRenderPanel::GetCamera() const
{
	return m_pViewer->GetCamera();
}
void twcRenderPanel::SetCamera(camCamera* i_pCamera)
{
	m_pViewer->SetCamera(i_pCamera);
}

//----------------------------------------------------------------------------
// Set background color
//----------------------------------------------------------------------------
void twcRenderPanel::SetBackgroundColor(const g2dRGBColor& i_Color)
{
	m_pViewer->SetBackgroundColor(i_Color);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void twcRenderPanel::do_resize()
{
	wxRect rect = this->GetClientRect();
//	DBG_LOG2("Render window size: %d %d", rect.GetWidth(), rect.GetHeight());
	
	if (this->m_pViewer != NULL)
	{		
		if (rect.GetWidth() > 0 && rect.GetHeight() > 0)
		{
			// Resize the underlying window
			m_pViewer->GetWindow()->ResizeWindow(rect.GetWidth(), rect.GetHeight());
			// Setting virtual resolution will keep text the same size 
			// when the window resizes and will avoid stretching when 
			// aspect ratio changes
			m_pViewer->GetWindow()->SetVirtualResolution(ToDIP(rect.GetWidth()), ToDIP(rect.GetHeight()));
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcRenderPanel::OnMouseDown(wxMouseEvent& i_Event)
{	
/*
	if (!i_Event.LeftDown()) // Left mouse takes focus anyway when the event is skipped.
		this->SetFocus();
	tma3dCursorMgr::CursorOverViewCallback(true);		
	tma3dCursorMgr::CursorPosChangedCallback( i_Event.GetX(), i_Event.GetY() );

	if (i_Event.RightDown() && !i_Event.AltDown())
	{
		wxMenu menu;
		
		wxMenu *camera_menu = new wxMenu();

		// Add in scripted cameras and director's cuts
		std::vector<nameString> camera_names;
		camsCameraMgr::GetCameraNames(camera_names);
		for (int i=0; i<camera_names.size(); ++i)
		{
			wxMenuItem* pItem = camera_menu->Append(MENU_SCRIPTED+i, camera_names[i].GetString());
			this->Connect( pItem->GetId(), wxEVT_COMMAND_MENU_SELECTED,
				wxCommandEventHandler(twcRenderPanel::cameraScripted_Click) );
		}
		if (!camera_names.empty())
			camera_menu->AppendSeparator();
		std::vector<nameString> dircut_names;
		camsDirectorsCutMgr::GetDirectorsCutNames(dircut_names);
		for (int i=0; i<dircut_names.size(); ++i)
		{
			wxMenuItem* pItem = camera_menu->Append(MENU_DCUT+i, dircut_names[i].GetString());
			this->Connect( pItem->GetId(), wxEVT_COMMAND_MENU_SELECTED,
				wxCommandEventHandler(twcRenderPanel::directorsCut_Click) );
		}
		if (!dircut_names.empty())
			camera_menu->AppendSeparator();

		// Editor cameras at end
		camera_menu->Append(MENU_EDITORPERSP,  _T("Editor"));
		camera_menu->Append(MENU_EDITORTOP,  _T("Top"));
		camera_menu->Append(MENU_EDITORFRONT,  _T("Front"));
		camera_menu->Append(MENU_EDITORSIDE,  _T("Side"));

		// flattening the menus to make it easier to select
		//wxMenu *scripted_menu = new wxMenu();
		//wxMenu *dcut_menu = new wxMenu();

		//camera_menu->AppendSubMenu(editor_menu, _T("Editor"));
		//camera_menu->AppendSubMenu(scripted_menu, _T("Scripted"));
		//camera_menu->AppendSubMenu(dcut_menu, _T("Director's Cut"));
		menu.AppendSubMenu(camera_menu, _T("Camera"));
		
		//wxMenu *select_menu = new wxMenu();
		//select_menu->Append(MENU_CLEARSELECTION,  _T("Clear Selection"));
		//select_menu->Append(MENU_SELECTCAMERA,  _T("Select Camera"));
		//menu.AppendSubMenu(select_menu, _T("Select")); 
		menu.Append(MENU_CLEARSELECTION,  _T("Clear Selection"));
		menu.Append(MENU_SELECTCAMERA,  _T("Select Camera"));
		PopupMenu(&menu, i_Event.GetX(), i_Event.GetY());
	}
	if (i_Event.MiddleDown() && !i_Event.AltDown())
	{
		//	sum up the local locations to get the screen coordinates
		//
		//int x,y,w,h;
		//GetPosition(&x,&y);
		//GetSize(&w,&h);
		//prefsQuickMgr::Show(x+(w/2),y+(h/2));
		int sx,sy;
		GetScreenPosition(&sx,&sy);
		int x,y;
		x = sx + (i_Event.GetX());
		y = sy + (i_Event.GetY());
		DBG_LOG2("mouse middle-press %d,%d", x,y);
		prefsQuickMgr::Show( x, y );
	}

	i_Event.Skip();
*/
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcRenderPanel::OnMouseMove(wxMouseEvent& i_Event)
{	
/*
if (this->IsActiveView())
	{
		//DBG_LOG3("OnMouseMove: %s (%d, %d)", m_pPanelViewer->GetTextString().c_str(), i_Event.GetX(), i_Event.GetY());
		tma3dCursorMgr::CursorPosChangedCallback( i_Event.GetX(), i_Event.GetY() );
	}
*/
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcRenderPanel::OnEnter(wxMouseEvent& i_Event)
{	
/*
//DBG_LOG("OnEnter: " << m_pPanelViewer->GetTextString().c_str());
	if (this->IsActiveView())
		tma3dCursorMgr::CursorOverViewCallback(true);
*/
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcRenderPanel::OnLeave(wxMouseEvent& i_Event)
{	
/*
//DBG_LOG("OnLeave: " << m_pPanelViewer->GetTextString().c_str());
	if (this->IsActiveView())
		tma3dCursorMgr::CursorOverViewCallback(false);
*/
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcRenderPanel::OnSize(wxSizeEvent& i_Event)
{		
	this->do_resize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcRenderPanel::OnKeyUp(wxKeyEvent& i_Event)
{	
	// Handle hot keys here
	//twxMessaging::ProcessKeyEvent(i_Event);
	i_Event.Skip();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcRenderPanel::OnFocus(wxFocusEvent& i_Event)
{	
	// don't take focus (for now?)
//	i_Event.Skip();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcRenderPanel::OnEraseBackground(wxEraseEvent& i_Event)
{
	// if we have a renderer, do nothing. otherwise use default handling.
	if (m_pViewer->GetRenderer() == NULL)
		i_Event.Skip();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcRenderPanel::OnPaint(wxPaintEvent& i_Event)
{
	wxPaintDC dc(this);
	if (m_pViewer->GetRenderer() != NULL)
	{
		m_pViewer->Render(appTime::GetTime());
		m_pViewer->Present();
	}
}

#endif // USE_WXWIDGETS