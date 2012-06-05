/****************************************************************************\
**	twcTestTreeView.cpp
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcTreeView.hpp"

#include "Core/CoreLayer.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsLocator.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Core/It/itStringUtil.hpp"
#include "ToolUIWx/twx/twxContextMenu.hpp"

#include <boost/bind.hpp>

//--------------------------------------------------------------------
// enumeration for the icons in the placed tree control
//--------------------------------------------------------------------
enum PlacedImageState
{
	e_NoCheckbox = 0,
	e_Unchecked = 1,
	e_Checked = 2,
	e_MixChecked = 3,
	e_ControlPart,
	e_FirstPartIndex = e_ControlPart,
	//e_ExpressionPart,
	e_MaterialPart,
	e_SurfacePart,
	e_NumPlacedImageStates
};

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void CheckedTreeView_Checked(twcTreeViewEvent& i_Event);
	void OnRightMouseDown(wxMouseEvent& i_Event);

	void CreateLight();
	void CreateCamera();

protected:
		twcTreeView* m_treeCtrl_Checked;
		twcTreeView* m_treeCtrl_Scene;
private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
	EVT_RIGHT_DOWN(MyFrame::OnRightMouseDown)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

	CoreLayer::Init();

#if wxUSE_LIBPNG
	wxImage::AddHandler( new wxPNGHandler );
#endif

    // create the main application window
    MyFrame *frame = new MyFrame(_T("Test of twc controls"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class MyTreeNode : public twcTreeNode
{
public:
	MyTreeNode(const std::string & i_Name) 
		: m_Name(i_Name), m_bCheckable(true),m_StateIndex(-1)  {}
	MyTreeNode(const std::string & i_Name, int i_StateIndex) 
		: m_Name(i_Name), m_bCheckable(false),m_StateIndex(i_StateIndex)  {}
	virtual wxString GetDisplayString() const { return wxString(m_Name.c_str(), wxConvUTF8); }
	virtual bool IsChecked() const { return false; }
	virtual bool IsCheckable() const { return m_bCheckable; }
	virtual int GetStateIndex() const { return m_StateIndex; }
	virtual bool ShouldSortChildren() const { return true; }

	std::string m_Name;
	bool m_bCheckable;
	int m_StateIndex;
};


// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));

	wxBoxSizer* panelSizer;
	panelSizer = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* checkSizer;
	checkSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* label1 = new wxStaticText(this, wxID_ANY, L"Checked Set Tree");
	checkSizer->Add( label1, 0, wxALL, 5 );

	fsLocator icon_dir;
	icon_dir.Push("Data");
	m_treeCtrl_Checked = new twcTreeView( this );
	m_treeCtrl_Checked->LoadImagesFromDirectory( icon_dir );
	checkSizer->Add( m_treeCtrl_Checked, 1, wxEXPAND | wxALL, 5 );

	// Set up the tree for the display in the tree control...
	shared_ptr<MyTreeNode> root(new MyTreeNode("Top Hat"));
	shared_ptr<MyTreeNode> child1(new MyTreeNode("Hello"));
	shared_ptr<MyTreeNode> child2(new MyTreeNode("Kiddy"));
	shared_ptr<MyTreeNode> nested1(new MyTreeNode("Grandson"));
	child2->AddChild(nested1);
	shared_ptr<MyTreeNode> nested2(new MyTreeNode("Granddaughter"));
	child2->AddChild(nested2);
	shared_ptr<MyTreeNode> child3(new MyTreeNode("Apple"));
	root->AddChild(child1);
	root->AddChild(child2);
	root->AddChild(child3);
	m_treeCtrl_Checked->UpdateTreeData( root );
	
	panelSizer->Add( checkSizer, 1, wxEXPAND | wxALL, 5 );

	wxBoxSizer* sceneSizer;
	sceneSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* label2 = new wxStaticText(this, wxID_ANY, L"Scene Manager Tree");
	sceneSizer->Add( label2, 0, wxALL, 5 );

	itString icon_str;
	fsFileUtil::LocatorToUnicodeString( icon_dir, icon_str );
	std::wstring icondir = icon_str.GetString();
	wxImageList *pCheckImages = new wxImageList(13, 13, false, e_NumPlacedImageStates);
	pCheckImages->Add(wxBitmap(icondir + L"\\tree-unchecked.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap(icondir + L"\\tree-unchecked.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap(icondir + L"\\tree-checked.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap(icondir + L"\\tree-mixchecked.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap(icondir + L"\\tree-controlpart.png", wxBITMAP_TYPE_PNG));
	//pCheckImages->Add(wxBitmap(icondir + L"\\tree-expressionpart.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap(icondir + L"\\tree-materialpart.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap(icondir + L"\\tree-surfacepart.png", wxBITMAP_TYPE_PNG));

	m_treeCtrl_Scene = new twcTreeView( this );
	m_treeCtrl_Scene->AssignStateImageList( pCheckImages );
	//m_treeCtrl_Scene->SetImageList( pCheckImages );
	sceneSizer->Add( m_treeCtrl_Scene, 1, wxEXPAND | wxALL, 5 );
	panelSizer->Add( sceneSizer, 1, wxEXPAND | wxALL, 5 );

	//wxTreeItemId my_root = m_treeCtrl_Scene->AddRoot(L"Root");
	//wxTreeItemId root2 = m_treeCtrl_Scene->AppendItem(my_root, L"Root2", e_SurfacePart);
	//wxTreeItemId node0 = m_treeCtrl_Scene->AppendItem(root2, L"Test State 0", e_SurfacePart);
	//m_treeCtrl_Scene->SetState(node0, 0);
	//wxTreeItemId node1 = m_treeCtrl_Scene->AppendItem(root2, L"Test State 1", e_SurfacePart);
	//m_treeCtrl_Scene->SetState(node1, 1);
	//wxTreeItemId node2 = m_treeCtrl_Scene->AppendItem(root2, L"Test State 2", e_MaterialPart);
	//m_treeCtrl_Scene->SetState(node2, 2);

	// Set up the tree for the display in the tree control...
	std::vector< shared_ptr<twcTreeNode> > roots;
	shared_ptr<MyTreeNode> objects(new MyTreeNode("Objects"));
	shared_ptr<MyTreeNode> geom1(new MyTreeNode("Shape1"));
	shared_ptr<MyTreeNode> surfaces(new MyTreeNode("Surfaces", e_SurfacePart));
	geom1->AddChild(surfaces);
	shared_ptr<MyTreeNode> surface1(new MyTreeNode("Surface1", e_SurfacePart));
	shared_ptr<MyTreeNode> surface2(new MyTreeNode("Surface2", e_SurfacePart));
	surfaces->AddChild(surface1);
	surfaces->AddChild(surface2);
	shared_ptr<MyTreeNode> geom2(new MyTreeNode("Shape2"));
	objects->AddChild(geom1);
	objects->AddChild(geom2);
	shared_ptr<MyTreeNode> lights(new MyTreeNode("Lights"));
	shared_ptr<MyTreeNode> light1(new MyTreeNode("Point Light"));
	shared_ptr<MyTreeNode> light2(new MyTreeNode("Projected Light"));
	lights->AddChild(light1);
	lights->AddChild(light2);

	roots.push_back(objects);
	roots.push_back(lights);
	m_treeCtrl_Scene->UpdateTreeData( roots );

	this->SetSizer( panelSizer );
	this->Layout();

	// Event callbacks
	this->Connect( m_treeCtrl_Scene->GetId(), wxEVT_CHECKED_TREE_VIEW,
					EVT_TWC_TREE_VIEW_FUNC(MyFrame::CheckedTreeView_Checked) );

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

    fileMenu->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& i_Event)
{
	CoreLayer::CleanUp();

    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format(
                    _T("Welcome to %s!\n")
                    _T("\n")
                    _T("This is the minimal wxWidgets sample\n")
                    _T("running under %s."),
                    wxVERSION_STRING,
                    wxGetOsDescription().c_str()
                 ),
                 _T("About wxWidgets minimal sample"),
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MyFrame::CheckedTreeView_Checked(twcTreeViewEvent& i_Event)
{
	DBG_LOG("Check changed, num nodes: " << i_Event.GetNodes().size());
	for (int i=0; i<i_Event.GetNodes().size(); ++i)
	{
		MyTreeNode *pNode = static_cast<MyTreeNode*>(i_Event.GetNodes()[i].get());
		DBG_LOG("Check changed: " << pNode->m_Name);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void MyFrame::OnRightMouseDown(wxMouseEvent& i_Event)
{	
	twxContextMenu menu;
	
	// Selection commands go at end
	//wxMenu *select_menu = new wxMenu();
	//select_menu->Append(MENU_CLEARSELECTION,  _T("Clear Selection"));
	//select_menu->Append(MENU_SELECTCAMERA,  _T("Select Camera"));
	//menu.AppendSubMenu(select_menu, _T("Select")); 

	//wxMenuItem *pItem = NULL;
	//pItem = menu.Append(wxID_ANY,  _T("Clear Selection"));
	//DBG_LOG("Menu index: " << pItem->GetId());
	//pItem = menu.Append(wxID_ANY,  _T("Select Camera"));
	//DBG_LOG("Menu index: " << pItem->GetId());

	menu.AddMenu("Lights", "");
	menu.AddMenuItem("Lights", "Create Light", boost::bind(&MyFrame::CreateLight, this));
	menu.AddMenu("Cameras", "");
	menu.AddMenuItem("Cameras", "Create Camera", boost::bind(&MyFrame::CreateCamera, this));

	PopupMenu(&menu, i_Event.GetX(), i_Event.GetY());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void MyFrame::CreateLight()
{	
	DBG_LOG("CreateLight ");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void MyFrame::CreateCamera()
{	
	DBG_LOG("CreateCamera ");
}

