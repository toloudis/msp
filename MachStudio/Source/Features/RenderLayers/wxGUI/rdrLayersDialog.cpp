/*****************************************************************************
**	rdrLayersDialog.cpp
**
**		see .hpp
**
\****************************************************************************/
#include "Features/RenderLayers/wxGUI/rdrLayersDialog.hpp"
#include "Features/RenderLayers/wxGUI/rdrLayersObjectsPage.hpp"

#include "Features/RenderLayers/rdrLayersDialogUtil.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/capt/captRenderOutputObject.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/pfx/pfxPostEffectObject.hpp"
#include "Support/rlyr/data/rlyrLayersDocumentChunk.hpp"
#include "Support/rlyr/rlyrPassesObject.hpp"
#include "Support/rlyr/rlyrRenderCam.hpp"
#include "Support/rlyr/rlyrRenderLayer.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "ToolUIWx/Pwx/pwxFormControlBuilder.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"

#include <vector>
#include <string>

//show object list with fragments
#define DO_FRAG

#ifdef USE_WXWIDGETS

namespace
{
	enum CheckBoxImage
	{
		e_Unchecked = 0,
		e_Checked,
		e_Mixchecked,
		e_NumBoxStates
	};

	//--------------------------------------------------------------------
	// rdrTreeItemData class will hold the data of the tree elements
	//--------------------------------------------------------------------
	class rdrTreeItemData : public wxTreeItemData
	{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		rdrTreeItemData(const wxString& i_Name, const wxString& i_ParentName)
			: m_Name(i_Name),
			  m_ParentName(i_ParentName)
		{}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const wxString& GetName()
		{
			return m_Name;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetName(const wxString& i_Name)
		{
			m_Name = i_Name;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const wxString& GetParentName()
		{
			return m_ParentName;
		}


	private:
		wxString m_Name;
		wxString m_ParentName;
	};

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	wxScrolledWindow* l_pTabPage_Render;
	wxScrolledWindow* l_pTabPage_Passes;
	wxScrolledWindow* l_pTabPage_Pfx;
	wxSizer*		  l_pTabPage_Pfx_Sizer;
	wxPanel*		  l_pTabPage_Pfx_Base;
	wxPanel*		  l_pTabPage_Pfx_Shader;
	int cur_page = 0;
	wxString l_RootName = wxT("Render Layers");
	nameString l_MasterName = "Master";
	nameString l_CurLayer;
	bool l_bCtrl;
	bool l_bShift;
	bool l_bItemRemoved;
	wxTreeItemId l_FirstShift;
	std::vector<wxTreeItemId> l_SelectedList;
	//------------------------------------------------------------------------
	// returns the next image index for the toggled item
	//------------------------------------------------------------------------
	int get_next_image(int cur_image)
	{
		return cur_image == e_Unchecked ? e_Checked : e_Unchecked;
	}

	//------------------------------------------------------------------------
	// returns the initial image index for a layer object
	//------------------------------------------------------------------------
	int get_image(bool i_isChecked)
	{
		return i_isChecked ? e_Checked : e_Unchecked;
	}

	//------------------------------------------------------------------------
	// sets the camera object's checkbox image based on the image of its children
	//------------------------------------------------------------------------
	void set_parent_image(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildItem, int i_ChildState)
	{
		wxTreeItemId parent = i_TreeCtrl->GetItemParent(i_ChildItem);
		wxTreeItemIdValue cookie;

		for (wxTreeItemId id = i_TreeCtrl->GetFirstChild(parent, cookie);
			 id.IsOk(); 
			 id = i_TreeCtrl->GetNextChild(id, cookie))
		{
			if(i_TreeCtrl->GetItemImage(id) != i_ChildState)
			{
				i_TreeCtrl->SetItemImage(parent, e_Mixchecked);
				break;
			}
			//if all are the same make the parent also the same image
			i_TreeCtrl->SetItemImage(parent, i_ChildState);
		}
	}

	//------------------------------------------------------------------------
	// sets the checkbox image for the current child item
	//------------------------------------------------------------------------
	void set_child_image(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildItem, int i_ChildState)
	{
		i_TreeCtrl->SetItemImage(i_ChildItem, i_ChildState);
	}

	//------------------------------------------------------------------------
	// return the oldest child in the given tree
	//------------------------------------------------------------------------
	wxTreeItemId& get_oldest_child(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildOne, wxTreeItemId& i_ChildTwo)
	{
		wxTreeItemId parent = i_TreeCtrl->GetItemParent(i_ChildOne);
		wxTreeItemIdValue cookie;

		wxTreeItemId id;
		for (id = i_TreeCtrl->GetFirstChild(parent, cookie);
			 id.IsOk(); 
			 id = i_TreeCtrl->GetNextChild(id, cookie))
		{
			if( id == i_ChildOne )
				return i_ChildOne;
			else if( id == i_ChildTwo )
				return i_ChildTwo;
		}
		//invalidate i_ChildOne to return it as not OK.
		i_ChildOne.Unset();
		return i_ChildOne;
	}

	//------------------------------------------------------------------------
	// sets the label text based on the current selection on the tree
	//------------------------------------------------------------------------
	void set_label_text(wxStaticText* i_StaticLabel, wxTreeCtrl* i_TreeCtrl)
	{
		wxTreeItemId sel = i_TreeCtrl->GetSelection();
		if(!sel.IsOk())
			return;
		
		wxTreeItemData* cur_data = i_TreeCtrl->GetItemData(sel);
		rdrTreeItemData* curItemData = dynamic_cast<rdrTreeItemData*>(cur_data);
		if(curItemData == NULL)
			return;

		//root - "Render Layers"
		if( sel == i_TreeCtrl->GetRootItem() )
			i_StaticLabel->SetLabel(wxT(" "));
		
		//camera
		else if( curItemData->GetParentName() == l_RootName )
			i_StaticLabel->SetLabel(curItemData->GetName());

		//render layer node
		else
			i_StaticLabel->SetLabel(curItemData->GetParentName() + wxT(" - ") + curItemData->GetName());	
	}
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void highlight_child(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildItem, bool i_bHighlight)
	{
		i_TreeCtrl->Freeze();
		if( i_bHighlight )
		{
			i_TreeCtrl->SetItemTextColour(i_ChildItem, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
			i_TreeCtrl->SetItemBackgroundColour(i_ChildItem,  wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );			
		}
		else
		{
			i_TreeCtrl->SetItemTextColour(i_ChildItem, i_TreeCtrl->GetForegroundColour() );
			i_TreeCtrl->SetItemBackgroundColour(i_ChildItem,  i_TreeCtrl->GetBackgroundColour() );			
		}
		i_TreeCtrl->Update();
		i_TreeCtrl->Thaw();
	}

	//------------------------------------------------------------------------
	// add a tree item to our selected list
	//------------------------------------------------------------------------
	void add_selected_child(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildItem)
	{
		std::vector<wxTreeItemId>::iterator it, end = l_SelectedList.end();
		for( it = l_SelectedList.begin(); it != end; ++it )
		{
			if( (*it) == i_ChildItem )
			{
				//DBG_TRACE( "tree item is already highlighted ");
				return;
			}
		}

		// add the tree item to our selected item list
		l_SelectedList.push_back(i_ChildItem);
		highlight_child(i_TreeCtrl, i_ChildItem, true);
	}

	//------------------------------------------------------------------------
	// add a tree item to our selected list, return true if the item was removed
	//------------------------------------------------------------------------
	bool remove_if_selected_child(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildItem)
	{
		std::vector<wxTreeItemId>::iterator it, end = l_SelectedList.end();
		for(it = l_SelectedList.begin(); it != end; ++it )
		{
			if( (*it) == i_ChildItem )
			{
				l_SelectedList.erase(it);
				highlight_child(i_TreeCtrl, i_ChildItem, false);
				return true;
			}		
		}
		return false;
	}

	//------------------------------------------------------------------------
	// add a tree item to our selected list
	//------------------------------------------------------------------------
	bool child_selected(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildItem)
	{
		std::vector<wxTreeItemId>::iterator it, end = l_SelectedList.end();
		for(it = l_SelectedList.begin(); it != end; ++it )
		{
			if( (*it) == i_ChildItem )
				return true;
		}
		return false;
	}
	//------------------------------------------------------------------------
	// remove all items from the selected list
	//------------------------------------------------------------------------
	void remove_all_highlight(wxTreeCtrl* i_TreeCtrl)
	{
		std::vector<wxTreeItemId>::iterator it, end = l_SelectedList.end();
		for(it = l_SelectedList.begin(); it != end; ++it )
		{
			if( (*it).IsOk() )
				highlight_child(i_TreeCtrl, (*it), false);
		}
		l_SelectedList.clear();
	}

	//------------------------------------------------------------------------
	// Highlight all child items in the range given
	//------------------------------------------------------------------------
	void highlight_child_range( wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_StartChild, wxTreeItemId& i_FinishChild )
	{
		//remove_all_highlight(i_TreeCtrl);
		wxTreeItemIdValue cookie;
		for (wxTreeItemId id = i_StartChild;
			 id.IsOk(); 
			 id = i_TreeCtrl->GetNextChild(id, cookie))
		{
			add_selected_child(i_TreeCtrl, id);
			if( id == i_FinishChild )
				return;
		}
	}

	void update_highlights(wxTreeCtrl* i_TreeCtrl)
	{
		std::vector<wxTreeItemId> temp = l_SelectedList;
		remove_all_highlight(i_TreeCtrl);
		l_SelectedList = temp;

		std::vector<wxTreeItemId>::iterator it, end = l_SelectedList.end();
		for(it = l_SelectedList.begin(); it != end; ++it )
		{
			if( (*it).IsOk() )
				highlight_child(i_TreeCtrl, (*it), true);
		}
	}

	void toggle_child_checkbox(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildItem, int i_ToggleImage)
	{

		wxTreeItemData* cur_data = i_TreeCtrl->GetItemData(i_ChildItem);
		rdrTreeItemData* curItemData = dynamic_cast<rdrTreeItemData*>(cur_data);
		if(curItemData == NULL)
			return;

		//toggle on camera changes all layers belonging to it
		if(curItemData->GetParentName() == l_RootName)
		{
			//toggle state in the current layer
			i_TreeCtrl->SetItemImage(i_ChildItem, i_ToggleImage);
			set_parent_image(i_TreeCtrl, i_ChildItem, i_ToggleImage);
			
			bool child_state = (i_ToggleImage == e_Checked ? true : false);
			rlyrRenderLayerMgr::SetLayerActive(nameString(std::string(curItemData->GetName().utf8_str())),
												child_state);
		}
		
	}
}

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
rdrLayersDialog* rdrLayersDialog::Instance = NULL;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
rdrLayersDialog::rdrLayersDialog( wxWindow* parent, const std::string& i_Title, 
									const std::string& i_Caption )
:	rdrLayersDialogBase(parent)
{
	m_bPreviewDialog = false;

	// Create state images to represent checked state
	itString icon_dir;
	fsFileUtil::LocatorToUnicodeString( guiMenuMgr::GetIconDirectory(), icon_dir );
	std::wstring icondir = icon_dir.GetString();
	wxImageList *pCheckImages = new wxImageList(FromDIP(13), FromDIP(13), false, e_NumBoxStates);
    {
        wxLogNull nullLog;
	    pCheckImages->Add(wxBitmap(icondir + L"\\tree-unchecked.png", wxBITMAP_TYPE_PNG));
	    pCheckImages->Add(wxBitmap(icondir + L"\\tree-checked.png", wxBITMAP_TYPE_PNG));
	    pCheckImages->Add(wxBitmap(icondir + L"\\tree-mixchecked.png", wxBITMAP_TYPE_PNG));
    }
	m_layerTree->AssignImageList(pCheckImages); 

	// Add this panel to the AUI manager
	//twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(i_Title).Caption(i_Caption).Show().Layer(1).Left());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
rdrLayersDialog::~rdrLayersDialog()
{
	if (rdrLayersDialog::Instance == this)
		rdrLayersDialog::Instance = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//void rdrLayersDialog::Update(bool i_bPreviewDialog)
void rdrLayersDialog::Update()
{
	//m_bPreviewDialog = i_bPreviewDialog;
	/*m_addButton->Show(m_bPreviewDialog);
	m_duplicateButton->Show(m_bPreviewDialog);
	m_deleteButton->Show(m_bPreviewDialog);*/
#if(SGPU_APP == MS_CORE)
	m_deleteButton->Disable();
	m_duplicateButton->Disable();
	m_addButton->Disable();
	m_renameButton->Disable();
#endif
	m_bPreviewDialog = false;
	UpdateLayerTree();
	UpdateTabPages();

	//select the master layer by default
	wxTreeItemIdValue cookie;
	wxTreeItemId first_child = m_layerTree->GetFirstChild(m_layerTree->GetRootItem(), cookie);
	m_layerTree->SelectItem(first_child);
	m_layerTree->SetFocus();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::Draw()
{
	if (m_layerTabPages)
		m_layerTabPages->Update();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//void rdrLayersDialog::UpdatePfxPage()
//{
//	UpdateTabPages(rlyrRenderLayerMgr::GetLayerRenderPrefs(l_CurLayer),
//		rlyrRenderLayerMgr::GetLayerRenderPasses(l_CurLayer),
//		rlyrRenderLayerMgr::GetLayerRenderPfx(l_CurLayer));
//}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::ResetSelection()
{
	wxTreeItemId cur_sel= m_curSelection;
	m_layerTree->SelectItem(cur_sel);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::UpdateLayerTree()
{
	m_layerTree->Freeze();
	m_layerTree->DeleteAllItems();
	// If root has not been created yet, then we are in process of starting program,
	// ignore these calls to update.
	
	wxTreeItemId root = m_layerTree->GetRootItem();
	if (!root.IsOk())
	{
		root = m_layerTree->AddRoot(l_RootName, e_Checked, -1, 
									new rdrTreeItemData(l_RootName, wxT("")));
	}

	buildLayerTree(root);
	m_layerNameText->SetLabel(wxT("-"));
	m_layerTree->Thaw();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::UpdateTabPages(rprfPrefsObject* i_RenderPrefs,
									 rlyrPassesObject* i_RenderPasses,
									 pfxPostEffectObject* i_RenderPfx)
{
	int currentPage = cur_page;
	
	//update the tabs, but wait to show update until everything is finished drawing
	m_layerTabPages->Freeze();
	
	while(m_layerTabPages->GetPageCount() > 0)
	{	
		m_layerTabPages->DeletePage(0);
	}	
	//m_layerTabPages->DeleteRelatedConstraints();
	//m_layerTabPages->ResetConstraints();
	//m_layerTabPages->Update();
	
	if( !m_bPreviewDialog )
	{
		UpdateObjectList();
		UpdateRenderPrefs(i_RenderPrefs);
		UpdatePassesList(i_RenderPasses);
		UpdatePfx(i_RenderPfx);

		//UpdateOutputFormat(i_CaptureOptions);
		m_layerTabPages->SetSelection(currentPage);
	}
	else
	{
		UpdateRenderPrefs(i_RenderPrefs);
		UpdatePassesList(i_RenderPasses);
		UpdatePfx(i_RenderPfx);
	}

	//m_layerTabPages->ResetConstraints();
	//m_layerTabPages->Update();

	m_layerTabPages->Thaw();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::UpdateObjectList()
{
#ifndef DO_FRAG
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	//	Add the Objects tab page
	wxScrolledWindow* pTabPage_Objects = new wxScrolledWindow( m_layerTabPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	pTabPage_Objects->SetScrollRate( 0, 5 );
	wxCheckListBox* object_list;
	if(l_CurLayer != "")
	{
		wxBoxSizer* sizer_Objects = new wxBoxSizer( wxVERTICAL );
		pTabPage_Objects->SetSizer( sizer_Objects );
		pTabPage_Objects->Layout();

		object_list = new wxCheckListBox(pTabPage_Objects, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SORT|wxLB_EXTENDED);
		sizer_Objects->Add( object_list, 1, wxEXPAND, 5 );
		sizer_Objects->Fit( pTabPage_Objects );
	
		object_list->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( rdrLayersDialog::rdrLayersDialog_ObjectToggle ), NULL, this );
	}
	m_layerTabPages->AddPage( pTabPage_Objects, wxT("Objects"), true );

	std::vector<nameString> all_objects;
	rlyrRenderLayerMgr::GetAllObjects(all_objects);
	//object_list->Clear();
	
	const int num_objects = all_objects.size();
	int list_index;
	for (int i=0; i < num_objects; i++)
	{
		bool isChecked = true;
		if(l_CurLayer != "")
		{	
			list_index = object_list->Append( wxString(all_objects[i].GetString().c_str(), wxConvUTF8) );
			isChecked = rlyrRenderLayerMgr::GetLayerObjectState(l_CurLayer, all_objects[i]);
			object_list->Check(list_index, isChecked );
		}
	}
	m_ObjectList = object_list;
#else

	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	//	Add the Objects tab page
	if (rdrLayersObjectsPage::Instance)
			rdrLayersObjectsPage::Instance = NULL;

	rdrLayersObjectsPage::Instance = new rdrLayersObjectsPage(m_layerTabPages);
	if(l_CurLayer != "")
	{
		rdrLayersObjectsPage::Instance->Update(l_CurLayer);
	}

	m_layerTabPages->AddPage( rdrLayersObjectsPage::Instance, wxT("Objects"), true );
#endif
}

//protected methods

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::UpdateRenderPrefs(rprfPrefsObject* i_RenderPrefs)
{
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	//	Add the Render tab page
	l_pTabPage_Render = new wxScrolledWindow( m_layerTabPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	l_pTabPage_Render->SetScrollRate( 0, 5 );
	wxBoxSizer* sizer_Render = new wxBoxSizer( wxVERTICAL );
	l_pTabPage_Render->SetSizer( sizer_Render );
	l_pTabPage_Render->Layout();
	sizer_Render->Fit( l_pTabPage_Render );
	m_layerTabPages->AddPage( l_pTabPage_Render, wxT("Render Prefs"), true );
	
	if(i_RenderPrefs != NULL)
	{
		//create the entries for the Render tab
		pwxFormControlBuilder::BuildForm(l_pTabPage_Render, "Render Prefs", (i_RenderPrefs->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::UpdatePassesList(rlyrPassesObject* i_RenderPasses)
{
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	//	Add the Render tab page
	l_pTabPage_Passes = new wxScrolledWindow( m_layerTabPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	l_pTabPage_Passes->SetScrollRate( 0, 5 );
	wxBoxSizer* sizer_Render = new wxBoxSizer( wxVERTICAL );
	l_pTabPage_Passes->SetSizer( sizer_Render );
	l_pTabPage_Passes->Layout();
	sizer_Render->Fit( l_pTabPage_Passes );
	m_layerTabPages->AddPage( l_pTabPage_Passes, wxT("Passes"), true );
	
	if(i_RenderPasses != NULL)
	{
		//create the entries for the Render tab
		pwxFormControlBuilder::BuildForm(l_pTabPage_Passes, "Passes", (i_RenderPasses->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::UpdatePfx(pfxPostEffectObject* i_RenderPfx)
{
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	//	Add the Render tab page
	l_pTabPage_Pfx = new wxScrolledWindow( m_layerTabPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	l_pTabPage_Pfx->SetScrollRate( 0, 5 );
	
	l_pTabPage_Pfx_Sizer = new wxBoxSizer( wxVERTICAL );
	l_pTabPage_Pfx_Base = new wxPanel(l_pTabPage_Pfx);
	l_pTabPage_Pfx_Shader = new wxPanel(l_pTabPage_Pfx);

	if(i_RenderPfx != NULL)
	{
		//create the entries for the Render tab
		pwxFormControlBuilder::BuildForm(l_pTabPage_Pfx_Base, "Post Effect Base", (*i_RenderPfx->GetBaseContainer().get()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
		pwxFormControlBuilder::BuildForm(l_pTabPage_Pfx_Shader, "Post Effect Shader", (*i_RenderPfx->GetShaderContainer().get()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
	}

	l_pTabPage_Pfx_Sizer->Add(l_pTabPage_Pfx_Base, wxSizerFlags().Expand());
	l_pTabPage_Pfx_Sizer->Add(l_pTabPage_Pfx_Shader, wxSizerFlags().Expand());
	//l_pTabPage_Pfx_Sizer->Add(l_pTabPage_Pfx_Base);
	//l_pTabPage_Pfx_Sizer->Add(l_pTabPage_Pfx_Shader);

	l_pTabPage_Pfx->SetSizer( l_pTabPage_Pfx_Sizer );
	l_pTabPage_Pfx->Layout();
	l_pTabPage_Pfx_Sizer->Fit( l_pTabPage_Pfx );
	m_layerTabPages->AddPage( l_pTabPage_Pfx, wxT("Post Effect"), true );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::UpdatePfxShader()
{
	pfxPostEffectObject* pPfxObj = rlyrRenderLayerMgr::GetLayerRenderPfx(l_CurLayer);
	if (!pPfxObj)
		return;
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;

	wxPanel* new_window = new wxPanel(l_pTabPage_Pfx);

	if(pPfxObj != NULL)
	{
		//create the entries for the Render tab
		pwxFormControlBuilder::BuildForm(new_window, "Post Effect Shader", (*pPfxObj->GetShaderContainer().get()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
	}

	if (l_pTabPage_Pfx_Shader)
	{
		bool bSuccess = l_pTabPage_Pfx_Sizer->Replace(l_pTabPage_Pfx_Shader, new_window, true);
		delete l_pTabPage_Pfx_Shader;
		l_pTabPage_Pfx_Shader = new_window;
	}
	
	l_pTabPage_Pfx->Layout();
	

	//l_pTabPage_Pfx_Sizer->Add(l_pTabPage_Pfx_Shader, 1, wxALL , 0);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::UpdateOutputFormat(captRenderOutputObject* i_CaptureOptions)
{
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	//	Add the Render tab page
	wxScrolledWindow* pTabPage_Output = new wxScrolledWindow( m_layerTabPages, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	pTabPage_Output->SetScrollRate( 0, 5 );
	wxBoxSizer* sizer_Output = new wxBoxSizer( wxVERTICAL );
	pTabPage_Output->SetSizer( sizer_Output );
	pTabPage_Output->Layout();
	sizer_Output->Fit( pTabPage_Output );
	m_layerTabPages->AddPage( pTabPage_Output, wxT("Output Format"), true );
	
	//	create the entries for the options tab
	if(i_CaptureOptions != NULL)
	{
		prtyObject* pDO = i_CaptureOptions;
		pDO->SortListByCategory();
		if(pDO->GetList().size() > 0)
			pwxFormControlBuilder::BuildForm(pTabPage_Output, "Output Format", (pDO->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::buildLayerTree(wxTreeItemId &io_Root)
{
	std::vector<rlyrRenderLayer*> all_layers;
	all_layers = rlyrRenderLayerMgr::GetRenderLayers();

	std::vector<rlyrRenderLayer*>::iterator it, end = all_layers.end();
	wxTreeItemId cur_layer;
	bool child_checked;
	for(it = all_layers.begin(); it != end; ++it)
	{
		cur_layer = m_layerTree->AppendItem(io_Root,wxString((*it)->GetName().GetString().c_str(), wxConvUTF8), e_Checked, -1, 
								new rdrTreeItemData(wxString((*it)->GetName().GetString().c_str(), wxConvUTF8), l_RootName));

		if((*it)->GetName().GetString() == l_MasterName.GetString())
			m_layerTree->SetItemBold(cur_layer, true);

		//set the checked image based on the current node's active state
		child_checked = rlyrRenderLayerMgr::GetLayerActive((*it)->GetName());
		set_child_image(m_layerTree, cur_layer, get_image(child_checked));

		//buildChildLayers(cur_cam, (*it));
	}
	set_parent_image(m_layerTree, cur_layer, m_layerTree->GetItemImage(cur_layer));
	m_layerTree->Expand(io_Root);
}

//----------------------------------------------------------------------------
// buildChildLayers - iterates through each camera and builds the tree based on 
//					their current set of layers.
//----------------------------------------------------------------------------
void rdrLayersDialog::buildChildLayers(wxTreeItemId &io_ParentItem, rlyrRenderCam* i_ParentCam)
{
	std::vector<nameString> all_layers;
	i_ParentCam->GetLayerNames(all_layers);
	std::vector<nameString>::iterator it, end = all_layers.end();
	wxTreeItemId cur_layer;
	bool child_checked;
	for(it = all_layers.begin(); it != end; ++it)
	{
		cur_layer = m_layerTree->AppendItem(io_ParentItem,wxString((*it).GetString().c_str(), wxConvUTF8), e_Checked, -1, 
								new rdrTreeItemData(wxString((*it).GetString().c_str(), wxConvUTF8), 
													wxString(i_ParentCam->GetName().GetString().c_str(), wxConvUTF8)));

		if((*it).GetString() == l_MasterName.GetString())
			m_layerTree->SetItemBold(cur_layer, true);

		//set the checked image based on the current node's active state
		child_checked = rlyrRenderLayerMgr::GetLayerActive((*it));
		set_child_image(m_layerTree, cur_layer, get_image(child_checked));
	}
	m_layerTree->Expand(io_ParentItem);

	//now set the image of the parent cam based on its children
	//set_parent_image(m_layerTree, cur_layer, m_layerTree->GetItemImage(cur_layer));
}

//----------------------------------------------------------------------------
// deleteSelection - removes an element from the tree
//----------------------------------------------------------------------------
void rdrLayersDialog::deleteSelection()
{
	l_bCtrl = false;
	l_bShift = false;
	wxTreeItemId parent;
	if(l_SelectedList.size() > 0)
	{
		//set the parent
		parent = m_layerTree->GetItemParent(l_SelectedList[0]);
		for(int i = 0; i < l_SelectedList.size(); ++i)
		{		
			wxTreeItemData* cur_data = m_layerTree->GetItemData(l_SelectedList[i]);
			rdrTreeItemData* curItemData = dynamic_cast<rdrTreeItemData*>(cur_data);
			if(curItemData == NULL)
				return;
			
			//only delete if the current selected item is a render layer
			if(curItemData->GetName() != wxString(l_MasterName.GetString().c_str(), wxConvUTF8))
			{
				rlyrRenderLayerMgr::DeleteRenderLayer(nameString(std::string(curItemData->GetName().utf8_str())));
				m_layerTree->Delete(l_SelectedList[i]);
			}
		}
		l_FirstShift.Unset();
		remove_all_highlight(m_layerTree);
		rlyrLayersDocumentChunk::ActiveDataChanged();
	}
	else
	{
		m_curSelection = m_layerTree->GetSelection();
		if((m_curSelection == m_layerTree->GetRootItem()) || (!m_curSelection.IsOk()))
			return;
		parent = m_layerTree->GetItemParent(m_curSelection);
		
		wxTreeItemData* cur_data = m_layerTree->GetItemData(m_curSelection);
		rdrTreeItemData* curItemData = dynamic_cast<rdrTreeItemData*>(cur_data);
		if(curItemData == NULL)
			return;
		
		//only delete if the current selected item is a render layer
		if(curItemData->GetName() != wxString(l_MasterName.GetString().c_str(), wxConvUTF8))
		{
			rlyrRenderLayerMgr::DeleteRenderLayer(nameString(std::string(curItemData->GetName().utf8_str())));
			m_layerTree->Delete(m_curSelection);
			rlyrLayersDocumentChunk::ActiveDataChanged();
		}
		else
		{	
			return; 
		}
	}

	//return focus to root
	//m_layerTree->SelectItem(parent);
	m_layerTree->SetFocus();
	wxTreeItemIdValue cookie;
	wxTreeItemId first_child = m_layerTree->GetFirstChild(parent, cookie);

	//now set the image of the root
	set_parent_image( m_layerTree, first_child, m_layerTree->GetItemImage(first_child) );
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_OnActivate( wxActivateEvent& event )
{ 
	event.Skip(); 
}

//----------------------------------------------------------------------------
// Layer Tree Events
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_TreeCharPressed( wxKeyEvent& event )
{ 
	if (event.GetKeyCode() == WXK_DELETE)
	{
		deleteSelection();
	}
	else
		event.Skip();
}

//----------------------------------------------------------------------------
// update image if pressed and change the render layer 
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_TreeLeftMouseDown( wxMouseEvent& event )
{
	int flags = 0;
	wxTreeItemId pick_item = m_layerTree->HitTest(event.GetPosition(), flags);
	bool child_state;

	l_bCtrl = event.m_controlDown;
	l_bShift = event.m_shiftDown;

	//if ((pick_item != m_layerTree->GetRootItem()) && pick_item.IsOk())
	if (pick_item.IsOk())
	{
		wxTreeItemData* cur_data = m_layerTree->GetItemData(pick_item);
		rdrTreeItemData* curItemData = dynamic_cast<rdrTreeItemData*>(cur_data);
		if(curItemData == NULL)
			return;
		// Check for click on the icon
		if (flags & wxTREE_HITTEST_ONITEMICON)
		{
			int toggle_index;
			//toggle on camera changes all layers belonging to it
			if(curItemData->GetParentName() == l_RootName)
			{
				//toggle state in the current layer
				toggle_index = get_next_image(m_layerTree->GetItemImage(pick_item));
				if(!child_selected(m_layerTree, pick_item))
					remove_all_highlight(m_layerTree);

				if( l_SelectedList.size() == 0)	
				{
					toggle_child_checkbox(m_layerTree, pick_item, toggle_index);
				}
				else
				{
					for(int i = 0; i < l_SelectedList.size(); ++i)
					{
						toggle_child_checkbox(m_layerTree, l_SelectedList[i], toggle_index);
					}
				}
				rlyrLayersDocumentChunk::ActiveDataChanged();
			}
			else if(curItemData->GetName() == l_RootName)
			{
				toggle_index = get_next_image(m_layerTree->GetItemImage(pick_item));
				m_layerTree->SetItemImage(pick_item, toggle_index);
				
				//toggle all children
				wxTreeItemIdValue cookie;
				for (wxTreeItemId id = m_layerTree->GetFirstChild(pick_item, cookie);
					 id.IsOk(); 
					 id = m_layerTree->GetNextChild(pick_item, cookie))
				{
					m_layerTree->SetItemImage(id, toggle_index);
				}

				child_state = (toggle_index == e_Checked ? true : false);
				rlyrRenderLayerMgr::SetAllLayersActive(child_state);
				rlyrLayersDocumentChunk::ActiveDataChanged();
			}
		}

		//if no multiple selection, remove the highlights if any
		if( !l_bCtrl && !l_bShift )
		{
			remove_all_highlight(m_layerTree);
		}	
		else
		{
			if(m_layerTree->GetSelection() !=  m_layerTree->GetRootItem())
			{
				m_layerTree->SelectItem( m_layerTree->GetRootItem() );
			}
			
		}
	}

	event.Skip();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_TreeBeginLabelEdit( wxTreeEvent& event )
{ 
	m_curSelection = m_layerTree->GetSelection();
	if(!m_curSelection.IsOk())
	{
		event.Skip();
		return;
	}
	
	if(m_curSelection == m_layerTree->GetRootItem())
		 event.Veto();

	wxTreeItemData* cur_data = m_layerTree->GetItemData(m_curSelection);
	rdrTreeItemData* curItemData = dynamic_cast<rdrTreeItemData*>(cur_data);
	if(curItemData == NULL)
		return;

	//don't edit the name of the master layer
	if(curItemData->GetName() == wxString(l_MasterName.GetString().c_str(), wxConvUTF8))
	{
		event.Veto();
	}
		
	//allow the edit of every other layer and then save the new data through the layer mgr
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_TreeEndLabelEdit( wxTreeEvent& event )
{ 
	//allow the edit of every layer except master and then save the new data through the layer mgr
	m_curSelection = event.GetItem();
	if(!m_curSelection.IsOk())
		return;

	wxTreeItemData* cur_data = m_layerTree->GetItemData(m_curSelection);
	rdrTreeItemData* curItemData = dynamic_cast<rdrTreeItemData*>(cur_data);
	if(curItemData == NULL)
		return;

	if(rlyrRenderLayerMgr::ChangeLayerName(nameString(std::string(curItemData->GetName().utf8_str())),
										nameString(std::string(event.GetLabel().utf8_str()))))
	{

		curItemData->SetName(event.GetLabel());
		m_layerNameText->SetLabel(curItemData->GetName());
		rlyrLayersDocumentChunk::ActiveDataChanged();
		//m_layerTree->SetItemData(m_curSelection, curItemData);
	}

	else
	{
		event.Veto();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_TreeDeleteItem( wxTreeEvent& event )
{ 
	event.Skip(); 
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_TreeItemActivated( wxTreeEvent& event )
{ 
	event.Skip(); 
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_TreeSelChanged( wxTreeEvent& event )
{ 
	m_curSelection = event.GetItem();

	//once the selection of the render layer has changed, update the corresponding tab pages
	UpdateTabPages(rlyrRenderLayerMgr::GetLayerRenderPrefs(l_CurLayer),
		rlyrRenderLayerMgr::GetLayerRenderPasses(l_CurLayer),
		rlyrRenderLayerMgr::GetLayerRenderPfx(l_CurLayer));
	
	//update the current layers capture controls
	rlyrRenderLayerMgr::SetupLayerCaptureControls(l_CurLayer);
}

//--------------------------------------------------------------------
// start to update all tabs according to the new item to be pressed
//--------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_TreeSelChanging( wxTreeEvent& event )
{
	wxTreeItemId prevSelection = m_curSelection;
	m_curSelection = event.GetItem();

	l_CurLayer.SetString("");
	if(!m_curSelection.IsOk())
	{
		event.Veto();
		return;
	}
	//root - "Render Layers"
	if( m_curSelection == m_layerTree->GetRootItem() )
	{
		if( !l_bCtrl )
			remove_all_highlight(m_layerTree);
		else
			update_highlights(m_layerTree);
		m_layerNameText->SetLabel(wxT("-"));
		event.Skip();
		return;
	}

	//remove the highlights if not doing multiple selection
	if( !l_bCtrl && !l_bShift )
	{
		l_FirstShift = m_curSelection;
		remove_all_highlight(m_layerTree);
		add_selected_child(m_layerTree, m_curSelection);
	}	
	else if( l_bCtrl && !l_bShift )
	{
		l_FirstShift = m_curSelection;
		//if the current selection isn't highlighted, highlight
		//if it is remove it's highlight
		if(remove_if_selected_child(m_layerTree, m_curSelection))
		{
			update_highlights(m_layerTree);
			event.Veto();
			return;
		}
		add_selected_child(m_layerTree, m_curSelection);
		update_highlights(m_layerTree);
		event.Veto();
		return;
	}
	else if( !l_bCtrl && l_bShift )
	{
		if( !l_FirstShift.IsOk() )
		{
			l_FirstShift = m_curSelection;
		}
		else
		{
			//check for oldest child
			wxTreeItemId oldest_item = get_oldest_child(m_layerTree, l_FirstShift, m_curSelection);

			if( !oldest_item.IsOk() )
				return;

			//highlight in range oldest to youngest child
			if( oldest_item == l_FirstShift )
				highlight_child_range(m_layerTree, l_FirstShift, m_curSelection);
			else
				highlight_child_range(m_layerTree, m_curSelection,l_FirstShift);
		}
		event.Veto();
		return;
	}
	else
	{
		l_FirstShift.Unset();
		event.Veto();
		return;
	}
	
	wxTreeItemData* cur_data = m_layerTree->GetItemData(m_curSelection);
	rdrTreeItemData* curItemData = dynamic_cast<rdrTreeItemData*>(cur_data);
	if(curItemData == NULL)
		return;

	//render layer
	if( curItemData->GetParentName() == l_RootName )
	{
		l_CurLayer.SetString( std::string(curItemData->GetName().utf8_str()) );
		m_layerNameText->SetLabel(curItemData->GetName());
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_TreeImageClick( wxTreeEvent& event )
{ 
	event.Skip(); 
}

//Button Click Events

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_AddButtonClick( wxCommandEvent& event )
{ 
	wxTreeItemId new_layer;
	nameString new_layer_name;

	m_curSelection = m_layerTree->GetSelection();
	if( !m_curSelection.IsOk() )
		return;

	wxTreeItemData* cur_data = m_layerTree->GetItemData(m_curSelection);
	rdrTreeItemData* curItemData = dynamic_cast<rdrTreeItemData*>(cur_data);
	if(curItemData == NULL && m_curSelection != m_layerTree->GetRootItem())
		return;

	//add new layer
	new_layer_name = rlyrRenderLayerMgr::AddRenderLayer();
	new_layer = m_layerTree->AppendItem(m_layerTree->GetRootItem(),wxString(new_layer_name.GetString().c_str(), wxConvUTF8), e_Checked, -1, 
							new rdrTreeItemData(wxString(new_layer_name.GetString().c_str(), wxConvUTF8), l_RootName));
	rlyrLayersDocumentChunk::ActiveDataChanged();

	l_bCtrl = false;
	l_bShift = false;
	//focus on the new layer
	m_layerTree->SelectItem(new_layer);
	m_layerTree->SetFocus();
	//now set the image of the parent cam based on its children
	set_parent_image( m_layerTree, new_layer, m_layerTree->GetItemImage(new_layer) );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_DuplicateButtonClick( wxCommandEvent& event )
{ 
	wxTreeItemId dup_layer;
	wxTreeItemId parent_cam;
	nameString dup_layer_name;
	bool child_checked;

	m_curSelection = m_layerTree->GetSelection();
	if((m_curSelection == m_layerTree->GetRootItem()) || (!m_curSelection.IsOk()))
		return;
	
	wxTreeItemData* cur_data = m_layerTree->GetItemData(m_curSelection);
	rdrTreeItemData* curItemData = dynamic_cast<rdrTreeItemData*>(cur_data);
	if(curItemData == NULL)
		return;

	//only duplicate if the current selected item is a render layer
	if(curItemData->GetParentName() == l_RootName)
	{
		parent_cam = m_layerTree->GetItemParent(m_curSelection);
		dup_layer_name = rlyrRenderLayerMgr::CopyRenderLayer(nameString(std::string(curItemData->GetName().utf8_str())));
		dup_layer = m_layerTree->AppendItem(parent_cam, wxString(dup_layer_name.GetString().c_str(), wxConvUTF8), e_Checked, -1, 
								new rdrTreeItemData(wxString(dup_layer_name.GetString().c_str(), wxConvUTF8), l_RootName));
		//set the checked image based on the current node's active state
		child_checked = rlyrRenderLayerMgr::GetLayerActive(nameString(std::string(curItemData->GetName().utf8_str())));
		set_child_image(m_layerTree, dup_layer, get_image(child_checked));
		rlyrLayersDocumentChunk::ActiveDataChanged();
	}
	else
	{	
		return; 
	}

	l_bCtrl = false;
	l_bShift = false;
	//return focus to root
	m_layerTree->SelectItem(dup_layer); 
	m_layerTree->SetFocus();
	//now set the image of the parent cam based on its children
	set_parent_image( m_layerTree, dup_layer, m_layerTree->GetItemImage(dup_layer) );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_DeleteButtonClick( wxCommandEvent& event )
{ 
	deleteSelection();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_RenameButtonClick( wxCommandEvent& event )
{ 
	m_curSelection = m_layerTree->GetSelection();
	if( !m_curSelection.IsOk() )
		return;
	
	m_layerTree->EditLabel( m_curSelection );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_TabPageChanged( wxAuiNotebookEvent& event )
{ 
	cur_page = event.GetSelection();
	event.Skip();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_TabPageChanging( wxAuiNotebookEvent& event )
{ 
	event.Skip();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::rdrLayersDialog_ObjectToggle( wxCommandEvent& event )
{ 
	m_curSelection = m_layerTree->GetSelection();
	if((m_curSelection == m_layerTree->GetRootItem()) || (!m_curSelection.IsOk()))
		return;

	wxTreeItemId parent = m_layerTree->GetItemParent(m_curSelection);

	wxTreeItemData* cur_data = m_layerTree->GetItemData(m_curSelection);
	rdrTreeItemData* curItemData = dynamic_cast<rdrTreeItemData*>(cur_data);
	if(curItemData == NULL)
		return;
	
	//only alter object toggle state if this is a render layer
	if(curItemData->GetParentName() == l_RootName)
	{	
		int id = event.GetSelection();
		nameString parent = nameString(std::string(curItemData->GetParentName().utf8_str()));
		nameString layer = nameString(std::string(curItemData->GetName().utf8_str()));
		bool checked = m_ObjectList->IsChecked( id );
		
		// now do the check operation on any selected objects in the list
		wxArrayInt selections;
		m_ObjectList->GetSelections(selections);
	
		//toggle the check of all the selected objects
		if( selections.size() > 0 && m_ObjectList->IsSelected(id))
		{
			for(int i = 0; i < selections.size(); ++i)
			{
				m_ObjectList->Check(selections[i], checked);
				wxString cur_object = m_ObjectList->GetString(selections[i]);
				rlyrRenderLayerMgr::SetLayerObjectState(layer, nameString(std::string(cur_object.utf8_str())), checked);
			}
		}
		else
		{
			wxString cur_object = m_ObjectList->GetString(id);
			rlyrRenderLayerMgr::SetLayerObjectState(layer, nameString(std::string(cur_object.utf8_str())), checked);
		}

		rlyrLayersDocumentChunk::ActiveDataChanged();
	}
}


#endif
