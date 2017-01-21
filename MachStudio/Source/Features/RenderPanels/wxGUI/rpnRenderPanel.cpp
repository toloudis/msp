/*****************************************************************************
**	rpnRenderPanel.cpp
**
**		Window using wxWidgets for doing MachStudio rendering
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/wxGUI/rpnRenderPanel.hpp"

#include "Features/FilmGates/fgtFrameMgr.hpp"
#include "Features/RenderPanels/rpnOperations.hpp"
#include "Features/RenderPanels/rpnPanelViewer.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmTimeCodeMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Cameras/Data/cmraData.hpp"
#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"

#include "Core/Ma/maFunctions.hpp"
#include "Graphics/G3d/g3dPickInfo.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/ctxm/ctxmContextMenu.hpp"
//#include "Tool/gpx/gpxCamera.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dRenderView.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#include "ToolUIWx/twx/twxContextMenu.hpp"
#include "ToolUIWx/twx/twxMessaging.hpp"

#include <boost/bind.hpp>
#include <sstream>


#ifdef USE_WXWIDGETS
//============================================================================
//============================================================================
namespace
{
	// Highlight text color if render panel has focus?
	const bool c_bHighlightTextColor = true; 
	const maFloatRGBA c_HighlightTextColor(1,1,0,1); // yellow highlight

	//--------------------------------------------------------------------
	// IDs for the menu commands
	//--------------------------------------------------------------------
	enum
	{
		MENU_CLEARSELECTION = wxID_HIGHEST,
		MENU_SELECTCAMERA,
		MENU_EDITORPERSP,
		MENU_EDITORTOP,
		MENU_EDITORFRONT,
		MENU_EDITORSIDE,
		MENU_NEWLIGHTSET,
		MENU_SCRIPTED = wxID_HIGHEST + 200,
		MENU_DCUT = MENU_SCRIPTED + 100,
		MENU_LSETADD = MENU_DCUT + 100,
		MENU_LSETREMOVE = MENU_LSETADD + 100
	};

	// What are the maximum number of items we can put into a context menu?
	// Limited by the id separation above.
	const int c_MaxNumSubMenuItems = 99;

	//--------------------------------------------------------------------
	// Callbacks from context menu, set the render pass type
	//--------------------------------------------------------------------
	void Set_Render_Pass(int i_PassIndex)
	{
		rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP.SetValue(i_PassIndex);
	}	
}

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(rpnRenderPanel, wxControl)
	EVT_PAINT(rpnRenderPanel::OnPaint)
	EVT_LEFT_DOWN(rpnRenderPanel::OnMouseDown)
	EVT_MIDDLE_DOWN(rpnRenderPanel::OnMouseDown)
	EVT_RIGHT_DOWN(rpnRenderPanel::OnMouseDown)
	EVT_MOTION(rpnRenderPanel::OnMouseMove)
	EVT_ENTER_WINDOW(rpnRenderPanel::OnEnter)
	EVT_LEAVE_WINDOW(rpnRenderPanel::OnLeave)
    EVT_SIZE(rpnRenderPanel::OnSize)
	EVT_KEY_UP(rpnRenderPanel::OnKeyUp)
	EVT_SET_FOCUS(rpnRenderPanel::OnFocus)
	EVT_KILL_FOCUS(rpnRenderPanel::OnLoseFocus)
    //EVT_MENU(MENU_CLEARSELECTION, rpnRenderPanel::OnClearSelection)
    //EVT_MENU(MENU_SELECTCAMERA, rpnRenderPanel::OnSelectCamera)
    //EVT_MENU(MENU_EDITORPERSP, rpnRenderPanel::OnEditorPersp)
    //EVT_MENU(MENU_EDITORTOP, rpnRenderPanel::OnEditorTop)
    //EVT_MENU(MENU_EDITORFRONT, rpnRenderPanel::OnEditorFront)
    //EVT_MENU(MENU_EDITORSIDE, rpnRenderPanel::OnEditorSide)
    //EVT_MENU(MENU_NEWLIGHTSET, rpnRenderPanel::OnNewLightSet)
END_EVENT_TABLE()


//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
rpnRenderPanel::rpnRenderPanel(wxWindow* parent)
: wxControl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
	m_pRenderView(NULL),
	m_MaxWidth(-1), 
	m_MaxHeight(-1),
	m_PassName("Beauty"),
	m_CameraName(""),
	m_bHasFocus(false),
	m_bCanChangeRenderPass(false)
{
	//this->SetBackgroundColour( *wxBLACK );
	// Since we are drawing using DirectX, we can set the custom flag
	// to prevent flickering:
	this->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

//--------------------------------------------------------------------
// Set render view to control
//--------------------------------------------------------------------
void rpnRenderPanel::SetRenderView(tma3dRenderView* i_pRenderView)
{
	m_pRenderView = i_pRenderView;
	if (m_pRenderView != NULL)
	{
		m_pPanelViewer = dynamic_cast<rpnPanelViewer*>(m_pRenderView->GetViewer());
		this->do_resize();
	}
	else
		m_pPanelViewer = NULL;
}

//------------------------------------------------------------------------
// Set whether this render panel should display the context menu items
// related to changing the render pass.
//------------------------------------------------------------------------
void rpnRenderPanel::SetCanChangeRenderPass(bool i_bEnable)
{
	m_bCanChangeRenderPass = i_bEnable;
}

//----------------------------------------------------------------------------
// Enable rendering in this pane.
//----------------------------------------------------------------------------
void rpnRenderPanel::EnableViewer(bool i_bEnabled)
{
	if (this->m_pRenderView != NULL)
	{
		this->m_pRenderView->EnableViewer( i_bEnabled );
	}
}

//----------------------------------------------------------------------------
// Turn visibility of text label on or off
//----------------------------------------------------------------------------
void rpnRenderPanel::SetTextVisible(bool i_bShow)
{
	if (m_pPanelViewer != NULL)
	{
		m_pPanelViewer->SetTextVisible( i_bShow );
	}
}

//----------------------------------------------------------------------------
// Set pass label.
//----------------------------------------------------------------------------
void rpnRenderPanel::SetPass(const std::string& i_Label)
{
	m_PassName = i_Label;
	build_text_string();
}

//----------------------------------------------------------------------------
// Set camera and label. If this view is active, the camera manipulator
//	will also be set.
//----------------------------------------------------------------------------
void rpnRenderPanel::SetCamera(gpxCamera *i_pCameraProxy, const std::string& i_Label)
{
	if (m_pRenderView != NULL)
	{
		this->m_pRenderView->SetCameraProxy( i_pCameraProxy );

		if (this->m_pPanelViewer)
		{
			m_CameraName = i_Label;
			build_text_string();
			this->m_pPanelViewer->SetDirectorsCut( NULL );
			icnIconLayer::SetCameraForIconLayer(m_pPanelViewer->GetIconLayerIndex(), &(i_pCameraProxy->GetCamera()));
		}

		//if (this->Focused)
		if (IsActiveView())
		{
			// Set our camera into cam3dMgr in order to activate the 
			//	camera manipulator
			this->m_pRenderView->ActivateCameraManipulator();

			// Highlight selection in GUI
			rpnOperations::HighlightCamera( &i_pCameraProxy->GetCamera() );

			// Use our camera name in the status bar panel
			guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, i_Label.c_str() );
			std::string tool_tip("Active Camera: ");
			tool_tip += i_Label;
			guiStatusBarMgr::SetToolTip(mnmConstants::e_SBPanel_Camera, tool_tip.c_str() );
		}
	}
}

//----------------------------------------------------------------------------
// Set directors cut and label. If this view is active, the camera
//	manipulator will de disabled.
//----------------------------------------------------------------------------
void rpnRenderPanel::SetDirectorsCut(camsDirectorsCut *i_pDirectorsCut, const std::string& i_Label)
{
	if (m_pRenderView != NULL)
	{
		if (this->m_pPanelViewer)
		{
			m_CameraName = i_Label;
			build_text_string();
			this->m_pPanelViewer->SetDirectorsCut( i_pDirectorsCut );
		}

		//if (this->Focused)
		if (IsActiveView())
		{
			// Deactivate the camera manipulator while viewing the
			// directors cut - can't edit camera because the camera
			// is not selected in order to key the changes.
			cam3dMgr::EnableManip(false);

			// Highlight selection in GUI
			rpnOperations::HighlightDirectorsCut( i_pDirectorsCut );

			// Use our directors cut name in the status bar panel
			guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, i_Label.c_str() );
			std::string tool_tip("Director's Cut: ");
			tool_tip += i_Label;
			guiStatusBarMgr::SetToolTip(mnmConstants::e_SBPanel_Camera, tool_tip.c_str() );
		}
	}
}


//----------------------------------------------------------------------------
// ConfirmCameraManip - make sure that the camera manip for this viewer is 
//	tracking changes in the scripted camera. Should be called every
//	Think() cycle.
//----------------------------------------------------------------------------
void rpnRenderPanel::ConfirmCameraManip()
{
	if (m_pRenderView && IsActiveView() && cam3dMgr::IsManipEnabled())
	{
		this->m_pRenderView->ActivateCameraManipulator();
	}
}

//----------------------------------------------------------------------------
// ConfirmCamera - make sure that the camera for this viewer is one
//	of the editor or scripted cameras. This should be called when 
//	cameras are deleted.
//----------------------------------------------------------------------------
void rpnRenderPanel::ConfirmCamera()
{
	if (m_pRenderView != NULL)
	{
		camCamera* pCamera = m_pRenderView->GetViewer()->GetCamera();

		// Check editor cameras
		if ((pCamera != &cam3dMgr::GetEditorCamera()) &&
			(pCamera != &cam3dMgr::GetTopCamera()) &&
			(pCamera != &cam3dMgr::GetSideCamera()) &&
			(pCamera != &cam3dMgr::GetFrontCamera()))
		{
			// Check scripted cameras
			int index = camsCameraMgr::GetIndexForCamera( pCamera );
			if (index < 0)
			{
				// Camera not found, so it was probably deleted,
				// switch to editor camera.
				this->SetCamera( cam3dMgr::GetEditorCameraProxy(), "Editor" );
			}
		}
	
		// Confirm director's cut still exists also
		if (this->m_pPanelViewer &&
			this->m_pPanelViewer->GetDirectorsCut())
		{
			int index = camsDirectorsCutMgr::GetIndexForDirectorsCut( this->m_pPanelViewer->GetDirectorsCut() );
			if (index < 0)
			{
				// Director's cut not found, so it was probably deleted,
				// switch to editor camera.
				this->SetCamera( cam3dMgr::GetEditorCameraProxy(), "Editor" );
			}
		}
	}
}

//--------------------------------------------------------------------
// Focus_Camera centers camera with respect to the point
//--------------------------------------------------------------------
void rpnRenderPanel::FocusCamera(const maPoint3d& i_Focus, float i_Radius)
{
	if (m_pRenderView != NULL)
	{
		// I'd rather be doing this through the camera manip interface,
		// but only the active panel has a camera manip.
		//
		//camCamera* pCamera = m_pRenderView->GetViewer()->GetCamera();
		// Switched to camera proxy in order to buffer changes 
		gpxCamera* pCamera = m_pRenderView->GetCameraProxy();

		maVector3d view = pCamera->GetPosition() - pCamera->GetTarget();
		pCamera->LookAt(i_Focus + view, i_Focus, pCamera->GetUp());

		if (pCamera->IsOrthographic())
		{
			const float width_modifier = 1.6f;
			pCamera->SetOrthoWidth( width_modifier * i_Radius );
		}
	}
}

//--------------------------------------------------------------------
//	Return true if this render pane is the active view
//--------------------------------------------------------------------
bool rpnRenderPanel::IsActiveView()
{
	return (tma3dRenderView::GetActiveRenderView() == this->m_pRenderView);
}

//--------------------------------------------------------------------
//	Return true if this render pane has keyboard focus
//--------------------------------------------------------------------
bool rpnRenderPanel::GetHasFocus()
{
	return m_bHasFocus;
}

//--------------------------------------------------------------------
//	Return true if this render pane is using a directors cut
//--------------------------------------------------------------------
bool rpnRenderPanel::HasDirectorsCut()
{
	return (this->m_pPanelViewer && 
			this->m_pPanelViewer->HasDirectorsCut() );
}

//--------------------------------------------------------------------
//	Set the focus distance to the given depth
//--------------------------------------------------------------------
void rpnRenderPanel::FocusAtDepth(const float& i_Depth)
{
	DBG_TRACE(" The depth is " << i_Depth);
	if (m_pRenderView != NULL)
	{
		int cam_index = camsCameraMgr::GetIndexForCamera( m_pRenderView->GetViewer()->GetCamera() );
		if(cam_index >= 0)
		{
			cmraScriptObject* script_object = cmraObjectMgr::GetObject(cam_index);
			if(script_object != NULL)
			{
				cmraCameraObject* pCO = script_object->GetPickObject();
				if(pCO != NULL)
				{
					cmraCameraData cdata = pCO->GetData();
					if(cdata.m_bEnableDOF.GetValue() && cdata.m_bEnableALP.GetValue())
					{
						cdata.m_FocalDistance.SetValue(i_Depth );
						pCO->SetData(cdata);
					}
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
// Select the camera object for the camera used in the view.
// Switching out of directors cut mode if necessary.
//----------------------------------------------------------------------------
void rpnRenderPanel::SelectCamera()
{
	if (m_pRenderView != NULL)
	{
		if (&cam3dMgr::GetEditorCamera() == m_pRenderView->GetViewer()->GetCamera())
		{
			// -1 is code for editor camera
			rpnOperations::SelectCamera( -1 );
		}
		else
		{
			int cam_index = camsCameraMgr::GetIndexForCamera( m_pRenderView->GetViewer()->GetCamera() );
			if (cam_index >= 0 && cam_index < camsCameraMgr::GetNumCameras())
			{
				if ( this->HasDirectorsCut() )
				{
					// Switch from directors cut to the camera that 
					// was active in the cut at this time
					nameString name_str;
					camsCameraMgr::GetCameraName(cam_index, name_str);

					this->SetCamera(camsCameraMgr::GetCameraProxy(cam_index),
												name_str.GetString());
				}
			
				rpnOperations::SelectCamera( cam_index );
			}
		}
	}
}

//----------------------------------------------------------------------------
// SetMaxRenderSize - set maximum size allowed for the render area.
//	Use -1 -1 in order to remove the constraints and allow any resizing.
//----------------------------------------------------------------------------
void rpnRenderPanel::SetMaxRenderSize(int i_Width, int i_Height)
{
	m_MaxWidth = i_Width;
	m_MaxHeight = i_Height;
}

//------------------------------------------------------------------------
// Override the wxWidgets SetSize function in order to enforce a
//	maximum size for the render area.
//------------------------------------------------------------------------
//virtual 
void rpnRenderPanel::DoSetSize(int x, int y,
                       int width, int height,
                       int sizeFlags)
{
	if (m_MaxWidth > 0 && width > m_MaxWidth) 
		width = m_MaxWidth;
	if (m_MaxHeight > 0 && height > m_MaxHeight) 
		height = m_MaxHeight;

	wxControl::DoSetSize(x, y, (width), (height), sizeFlags);
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void rpnRenderPanel::do_resize()
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	wxRect rect = this->GetClientRect();
//	DBG_LOG2("Render window size: %d %d", rect.GetWidth(), rect.GetHeight());
	
	if (this->m_pRenderView != NULL)
	{		
		if (rect.GetWidth() > 0 && rect.GetHeight() > 0)
		{
			// Resize the underlying window
			this->m_pRenderView->ResizeWindow((rect.GetWidth()), (rect.GetHeight()), ToDIP(rect.GetWidth()), ToDIP(rect.GetHeight()));

			// Update the ScreenUtil function if we are the active render view
			if (tma3dRenderView::GetActiveRenderView() == this->m_pRenderView)
			{
				// The mouse move events later are going to be relative to our
				// screen position, so we don't need to give those offsets
				// to the screen util.
				tma3dScreenUtil::SetWindowSize( maPoint2d( 0, 0 ),
					maPoint2d( (rect.GetWidth()), (rect.GetHeight()) ) );
				//tma3dScreenUtil::SetWindowSize( maPoint2d( rect.GetLeft(), rect.GetTop() ),
				//	maPoint2d( rect.GetWidth(), rect.GetHeight() ) );

				//	update the frame gate
				fgtFrameMgr::ResizeFrames();

				// Update the time code manager to adjust the text to
				// the new window size and aspect ratio
				mnmTimeCodeMgr::Update();
			}
		}
	}
}

//--------------------------------------------------------------------
// Add menu items related to cameras to the context menu
//--------------------------------------------------------------------
void rpnRenderPanel::add_cameras_to_context_menu(twxContextMenu &io_ContextMenu)
{
	const char* c_MenuName = "Camera";

	//io_ContextMenu.AddMenu(c_MenuName, "");
	io_ContextMenu.AddMenu("View", "");
	io_ContextMenu.AddMenu("View", c_MenuName);

	// Add in scripted cameras and director's cuts
	std::vector<nameString> camera_names;
	camsCameraMgr::GetCameraNames(camera_names);
	const int num_cameras = maFunctions::Lowest((int)camera_names.size(), c_MaxNumSubMenuItems);
	for (int i=0; i<num_cameras; ++i)
	{
		io_ContextMenu.AddMenuItem(c_MenuName, camera_names[i].GetString().c_str(),
			boost::bind(&rpnRenderPanel::cameraScripted_Click, this, i));
	}
	//if (!camera_names.empty())
	//	io_ContextMenu.AppendSeparator(c_MenuName);

	std::vector<nameString> dircut_names;
	camsDirectorsCutMgr::GetDirectorsCutNames(dircut_names);
	const int num_dircuts = maFunctions::Lowest((int)dircut_names.size(), c_MaxNumSubMenuItems);
	for (int i=0; i<num_dircuts; ++i)
	{
		io_ContextMenu.AddMenuItem(c_MenuName, dircut_names[i].GetString().c_str(),
			boost::bind(&rpnRenderPanel::directorsCut_Click, this, i));
	}
	//if (!dircut_names.empty())
	//	io_ContextMenu.AppendSeparator(c_MenuName);

	// Editor cameras at end
	io_ContextMenu.AddMenuItem(c_MenuName, "Editor", 
		boost::bind(&rpnRenderPanel::OnEditorPersp, this));
	io_ContextMenu.AddMenuItem(c_MenuName, "Top", 
		boost::bind(&rpnRenderPanel::OnEditorTop, this));
	io_ContextMenu.AddMenuItem(c_MenuName, "Front", 
		boost::bind(&rpnRenderPanel::OnEditorFront, this));
	io_ContextMenu.AddMenuItem(c_MenuName, "Side", 
		boost::bind(&rpnRenderPanel::OnEditorSide, this));
}

//--------------------------------------------------------------------
// Add menu items related to render passes to the context menu
//--------------------------------------------------------------------
void rpnRenderPanel::add_renderpasses_to_context_menu(twxContextMenu &io_ContextMenu)
{
	// Note: these context menu items are added here because they are only
	// appropriate for clicks in a render panel and not for right clicks in 
	// the SceneManager.
	const char* c_Context_Menu_Name = "Render Passes";
	io_ContextMenu.AddMenu("View", "");
	io_ContextMenu.AddMenu("View", c_Context_Menu_Name);
	// Create menu dynamically by enumerating the property enum tags.
	prtyEnum &rp_prty = rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs).m_RenderPassVP;
	int num_elems = rp_prty.GetNumTags();
	for (int i=0; i<num_elems; ++i)
	{
		io_ContextMenu.AddMenuItem(c_Context_Menu_Name, rp_prty.GetEnumTag(i).c_str(), 
			boost::bind(&Set_Render_Pass, i));
	}

}

//----------------------------------------------------------------------------
//	run a pick operation at the current cursor pos in current render view
//----------------------------------------------------------------------------
void rpnRenderPanel::do_pick_at_cursor(int i_X, int i_Y, g3dPickInfo& o_PickInfo)
{
	// stop any render threads while doing GPU picking
	gpxRenderControl::ConfirmSingleThread();

	// GPU Pick technique
	m_pRenderView->DoPickRender(i_X, i_Y, tmlnTimeLine::GetTimeInSeconds(), o_PickInfo);

	// If the user clicks on the empty viewport, set the depth based on the camera target position.
	if(o_PickInfo.m_Depth == 0.0)
	{
		o_PickInfo.m_Depth = m_pRenderView->GetCameraDepth();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rpnRenderPanel::build_text_string()
{
	if (this->m_pPanelViewer)
	{
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss << m_CameraName.c_str();
		if (m_PassName.size() > 0)
			oss << " (" << m_PassName.c_str() << ")";
		this->m_pPanelViewer->SetTextString( oss.str() );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnPaint(wxPaintEvent& i_Event)
{
	// Notify that a new render is needed
	gpxRenderControl::SetNeedsNewRender();

	i_Event.Skip(); // let default handler take care of the real painting
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnMouseDown(wxMouseEvent& i_Event)
{	
	if (!i_Event.LeftDown()) // Left mouse takes focus anyway when the event is skipped.
		this->SetFocus();
	tma3dCursorMgr::CursorOverViewCallback(true);		
	tma3dCursorMgr::CursorPosChangedCallback( i_Event.GetX(), i_Event.GetY() );

	if (i_Event.RightDown() && !i_Event.AltDown())
	{
		// Construct context menu
		twxContextMenu menu;
		add_cameras_to_context_menu(menu);

		// Get context info about mouse position
		g3dPickInfo pick_info;
		do_pick_at_cursor(i_Event.GetX(), i_Event.GetY(), pick_info);
		envType::UInt32 pick_code = pick_info.m_ObjectID;
		pick3dPickObject *pPickedObject = pick3dMgr::MatchPickCode(pick_code);
		sel3dObject *pChosenObject = sel3dCastUtil::ConvertPickToSelection(pPickedObject);
		float depth = pick_info.m_Depth;
		// Gather menu items from all systems that have interest in the context menu
		ctxmContextMenu::AddToContextMenu(menu, pChosenObject, &pick_info);

		// Add selection items at end of menu
		menu.AddMenuItem("", "Clear Selection", 
			boost::bind(&rpnRenderPanel::clear_selection, this));
		menu.AddMenuItem("View", "Select Camera", 
			boost::bind(&rpnRenderPanel::select_camera, this));
		menu.AddMenuItem("", "Focus",
			boost::bind(&rpnRenderPanel::focus_camera, this, depth));

		// The render pass only changes in the main panel, 
		// so only show onctext menu in that panel.
		if (m_bCanChangeRenderPass)
			add_renderpasses_to_context_menu(menu);

		if (!menu.IsEmpty())
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
		//DBG_LOG2("mouse middle-press %d,%d", x,y);

		// TODO - This is disabled because the configure preference page wasn't implemented in wx
		//prefsQuickMgr::Show( x, y );
	}

	i_Event.Skip();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnMouseMove(wxMouseEvent& i_Event)
{	
	if (this->IsActiveView())
	{
		//DBG_LOG3("OnMouseMove: %s (%d, %d)", m_pPanelViewer->GetTextString().c_str(), i_Event.GetX(), i_Event.GetY());
		tma3dCursorMgr::CursorPosChangedCallback( i_Event.GetX(), i_Event.GetY() );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnEnter(wxMouseEvent& i_Event)
{	
	//DBG_LOG("OnEnter: " << m_pPanelViewer->GetTextString().c_str());
	if (this->IsActiveView())
		tma3dCursorMgr::CursorOverViewCallback(true);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnLeave(wxMouseEvent& i_Event)
{	
	//DBG_LOG("OnLeave: " << m_pPanelViewer->GetTextString().c_str());
	if (this->IsActiveView())
		tma3dCursorMgr::CursorOverViewCallback(false);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnSize(wxSizeEvent& i_Event)
{		
	this->do_resize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnKeyUp(wxKeyEvent& i_Event)
{	
	// Handle hot keys here
	//twxMessaging::ProcessKeyEvent(i_Event);
	i_Event.Skip();

}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnFocus(wxFocusEvent& i_Event)
{	
	if (this->m_pRenderView)
	{
		//DBG_LOG("Focus: " << m_pPanelViewer->GetTextString().c_str());
		tma3dRenderView::SetActiveRenderView(this->m_pRenderView);	

		wxRect rect = this->GetRect();
		if (rect.GetWidth() > 0 && rect.GetHeight() > 0)
		{
			// The mouse move events later are going to be relative to our
			// screen position, so we don't need to give those offsets
			// to the screen util.
			tma3dScreenUtil::SetWindowSize( maPoint2d( 0, 0 ),
				maPoint2d( (rect.GetWidth()), (rect.GetHeight()) ) );
			//tma3dScreenUtil::SetWindowSize( maPoint2d( (float)rect.GetLeft(), (float)rect.GetTop() ),
			//	maPoint2d( (float)rect.GetWidth(), (float)rect.GetHeight() ) );

			//	update the frame gate
			fgtFrameMgr::ResizeFrames();
		}

		if (this->HasDirectorsCut())
		{
			// Deactivate the camera manipulator while viewing the
			// directors cut - can't edit camera because the camera
			// is not selected in order to key the changes.
			cam3dMgr::EnableManip(false);

			// Highlight selection in GUI
			rpnOperations::HighlightDirectorsCut( this->m_pPanelViewer->GetDirectorsCut() );

			// Status bar tool tip
			std::string tool_tip("Director's Cut: ");
			tool_tip += this->m_pPanelViewer->GetTextString();
			guiStatusBarMgr::SetToolTip(mnmConstants::e_SBPanel_Camera, tool_tip.c_str() );
		}
		else
		{
			// Set our camera into cam3dMgr in order to activate the 
			//	camera manipulator
			this->m_pRenderView->ActivateCameraManipulator();

			// Highlight selection in GUI
			rpnOperations::HighlightCamera( this->m_pRenderView->GetViewer()->GetCamera() );

			// Status bar tool tip
			std::string tool_tip("Active Camera: ");
			tool_tip += this->m_pPanelViewer->GetTextString();
			guiStatusBarMgr::SetToolTip(mnmConstants::e_SBPanel_Camera, tool_tip.c_str() );
		}

		if (this->m_pPanelViewer)
		{
			// Use our camera name in the status bar panel
			guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Camera, 
				this->m_pPanelViewer->GetTextString().c_str() );

			if (c_bHighlightTextColor)
			{
				// Set color of the text to yellow when we have the focus
				m_pPanelViewer->SetTextColor(c_HighlightTextColor);
			}

			// Activate the icons in our layer
			icnIconLayer::SetActiveIconLayer(m_pPanelViewer->GetIconLayerIndex());
		}
	}
	m_bHasFocus = true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rpnRenderPanel::OnLoseFocus(wxFocusEvent& i_Event)
{	
	m_bHasFocus = false;
	if (c_bHighlightTextColor && m_pPanelViewer)
	{
		m_pPanelViewer->SetTextColor(maFloatRGBA(1,1,1,1));
	}
    i_Event.Skip();
}

//------------------------------------------------------------------------
// context menu handlers
//------------------------------------------------------------------------
void rpnRenderPanel::clear_selection()
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	sel3dMgr::CreateUndoOperation();
	sel3dMgr::ClearSelection();
}
void rpnRenderPanel::select_camera()
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	this->SelectCamera();
}
void rpnRenderPanel::focus_camera(const float& i_Depth)
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();
	this->FocusAtDepth(i_Depth);
}
void rpnRenderPanel::OnEditorPersp()
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	this->SetCamera( cam3dMgr::GetEditorCameraProxy(), "Editor" );
}
void rpnRenderPanel::OnEditorTop()
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	this->SetCamera( cam3dMgr::GetTopCameraProxy(), "Top" );
}
void rpnRenderPanel::OnEditorFront()
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	this->SetCamera( cam3dMgr::GetFrontCameraProxy(), "Front" );
}
void rpnRenderPanel::OnEditorSide()
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	this->SetCamera( cam3dMgr::GetSideCameraProxy(), "Side" );
}
void rpnRenderPanel::cameraScripted_Click(int i_Index)
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	gpxCamera *pCamera = camsCameraMgr::GetCameraProxy(i_Index);
	nameString name;
	camsCameraMgr::GetCameraName(i_Index, name);
	this->SetCamera( pCamera, name.GetString() );
}
void rpnRenderPanel::directorsCut_Click(int i_Index)
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	camsDirectorsCut *pDirectorsCut = camsDirectorsCutMgr::GetDirectorsCut(i_Index);
	nameString name;
	camsDirectorsCutMgr::GetDirectorsCutName(i_Index, name);
	this->SetDirectorsCut( pDirectorsCut, name.GetString() );
}

#endif // USE_WXWIDGETS