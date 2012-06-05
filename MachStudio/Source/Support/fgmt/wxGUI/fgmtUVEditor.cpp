/*****************************************************************************
**  fgmtUVEditor.hpp
**
**     MainForm when using wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/wxGUI/fgmtUVEditor.hpp"

#include "Support/fgmt/GUI/fgmtDialogUtil.hpp"

// TODO: support a uv compass?
//#include "Support/cmps/cmpsCompassMgr.hpp" 

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "ToolUIWx/twc/twcRenderPanel.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"

#ifdef USE_WXWIDGETS

namespace
{

}	// end of namespace

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(fgmtUVEditor, wxPanel)
//	EVT_CLOSE(fgmtUVEditor::OnClose)
    EVT_SIZE(fgmtUVEditor::OnResize)
	EVT_KEY_UP(fgmtUVEditor::OnKeyUp)
END_EVENT_TABLE()


// ============================================================================
// implementation
// ============================================================================

//--------------------------------------------------------------------
// frame constructor
//--------------------------------------------------------------------
fgmtUVEditor::fgmtUVEditor(wxWindow* parent, const std::string& i_Title)
: wxPanel(parent),
	m_pRenderPanel(NULL),
	m_pScene(NULL),
	m_pRootNode(NULL),
	m_pRenderer(NULL),
	m_pCamera(NULL)
{
	this->SetSize(400,400);

    // set the frame icon
    //SetIcon(wxICON(sample));
    
	// create a menu bar
    //wxMenu *fileMenu = new wxMenu;
    // now append the freshly created menu to the menu bar...
    //wxMenuBar *menuBar = new wxMenuBar();
    //menuBar->Append(fileMenu, _T("&File"));
    //menuBar->Append(helpMenu, _T("&Help"));
    // ... and attach this menu bar to the frame
    //SetMenuBar(menuBar);

    // create a status bar
	//const int c_StatusPaneWidths[] = {
	//	100, 	// Mode
	//	-1, 	// Messages
	//};
	//const int c_NumStatusPanes = 2;
    //wxStatusBar *statusBar = CreateStatusBar(c_NumStatusPanes);
	//statusBar->SetStatusWidths(c_NumStatusPanes, c_StatusPaneWidths);
	//SetStatusBarPane(1); // second pane of variable width contains messages
    //SetStatusText(_T("using wxWidgets!"), 1);

	// Setup panel for the render area, which is placed
	// inside an empty content pane:
	m_pSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pRenderPanel = new twcRenderPanel( this, fgmtDialogUtil::GetSystem() );
	m_pSizer->Add( m_pRenderPanel, 1, wxEXPAND, 0 );
	this->SetSizer(m_pSizer);
	this->Layout();

	//	set the functions to handle menu items being checked and
	//	checking if hot keys are valid here.
	// should be already done? these seem like one time global funcs to call.
//	cmaCommandMgr::SetMenuCheckedFunction(guiMenuMgr::MenuObjectsCheck);
//	cmaCommandMgr::SetValidHotKeyFunction(guiMenuMgr::IsValidShortcut);

	//	Create some menu items so we control the order
//	fileMenu->AppendSeparator();
	
//	guiMenuMgr::AddMenu("Edit","");
//	guiMenuMgr::AddMenu("View","");
//	guiMenuMgr::AddMenu("Tools","");
//	guiMenuMgr::AddMenu("Actions","");
//	guiMenuMgr::AddMenu("Modes","");
//	guiMenuMgr::AddMenu("Objects","");
//	guiMenuMgr::AddMenu("Windows","");
//	guiMenuMgr::AddMenu("Help","");

	// i own the scene, but the renderpanel will use it.
	m_pScene = new g3dScene(new g3dLayer());
	m_pRootNode = new g3dSceneNode(NULL);
	m_pScene->GetLayer(0)->SetRootNode(m_pRootNode);
	m_pRenderPanel->SetScene(m_pScene);

	// i own the camera, but the renderpanel will use it.
	// ortho camera where the SetSubViewport will define the coordinate system.
	m_pCamera = new camCamera();
	m_pCamera->SetOrthographic(true);
	m_pCamera->SetClip(0,1);
	m_pCamera->SetAspect(1);//SetMatchAspectToWindow(true);
	m_pRenderPanel->SetCamera(m_pCamera);

	// i own the renderer, but the renderpanel will use it.
	m_pRenderer = g3dSceneRendererCreate::CreateUVRenderer();
	m_pRenderPanel->SetRenderer(m_pRenderer);

	// Add this panel to the AUI manager
	wxString title(i_Title.c_str(), wxConvUTF8);
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(title).Caption(title).Show().Layer(1).Left());
}

//--------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------
fgmtUVEditor::~fgmtUVEditor()
{
	// assumes singleton owned by fgmtDialogUtil!!!
	fgmtDialogUtil::NotifyUVEditorDestroyed();

	delete m_pRenderer;

	if (m_pRootNode)
		delete m_pRootNode;
	delete m_pScene;

	delete m_pCamera;
}

//--------------------------------------------------------------------
// Access to the render areas
//--------------------------------------------------------------------
twcRenderPanel* fgmtUVEditor::GetRenderWindow()
{
	return m_pRenderPanel;
}

//--------------------------------------------------------------------
// Set the fragment to edit.
//--------------------------------------------------------------------
void fgmtUVEditor::SetNode(g3dSceneNode* i_pNode)
{
	// clean up old root
	m_pScene->GetLayer(0)->SetRootNode(NULL);
	delete m_pRootNode;
	m_pRootNode = NULL;

	// set new root
	m_pScene->GetLayer(0)->SetRootNode(i_pNode);

	// redraw!
	m_pRenderPanel->Refresh(false);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//	You should check whether the application is forcing the deletion 
//	of the window using wxCloseEvent::CanVeto. If this is false, you 
//	must destroy the window using wxWindow::Destroy. If the return 
//	value is true, it is up to you whether you respond by destroying the window.
//	
//	If you don't destroy the window, you should call wxCloseEvent::Veto 
//	to let the calling code know that you did not destroy the window. 
//	This allows the wxWindow::Close function to return true or false 
//	depending on whether the close instruction was honoured or not.
void fgmtUVEditor::OnClose(wxCloseEvent& i_Event)
{
//	this->Destroy();

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void fgmtUVEditor::OnResize(wxSizeEvent& i_Event)
{
	// this is necessary to resize the panel
	this->OnSize(i_Event);

	// keep -1..1 centered in the view.
	if (m_pCamera)
	{
		wxSize s = this->GetClientSize();
		float aspect = (float)s.GetWidth()/(float)s.GetHeight();
		if (aspect >= 1)
			m_pCamera->SetSubViewport(1, -1, -aspect, aspect);//t,b,l,r
		else
			m_pCamera->SetSubViewport(1/aspect, -1/aspect, -1, 1);// t,b,l,r
		m_pRenderPanel->Refresh(false);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void fgmtUVEditor::OnKeyUp(wxKeyEvent& i_Event)
{	
	// Handle hot keys here
	//twxMessaging::ProcessKeyEvent(i_Event);
	i_Event.Skip();

}

#endif // USE_WXWIDGETS
