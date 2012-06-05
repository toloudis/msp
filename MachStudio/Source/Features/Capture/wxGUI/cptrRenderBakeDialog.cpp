/*****************************************************************************
**	cptrRenderBakeDialog.cpp
**
**		see .hpp
**
\****************************************************************************/
#include "Features/Capture/wxGUI/cptrRenderBakeDialog.hpp"

#include "Features/Capture/cptrRenderBakeDataUtil.hpp"
#include "Features/Capture/cptrRenderBakeDialogUtil.hpp"
#include "Features/RenderLayers/wxGUI/rdrLayersObjectsPage.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/name/nameString.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyFolderChooserUIInfo.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "ToolUIWx/Pwx/pwxFormControlBuilder.hpp"

#include <vector>
#include <string>

//show object list with fragments
#define DO_FRAG

#ifdef USE_WXWIDGETS

namespace
{
	//--------------------------------------------------------------------
	// enumeration for the icons in the placed tree control
	//--------------------------------------------------------------------
	enum PlacedImageState
	{
		e_Unchecked = 0,
		e_Checked = 1,
		e_MixChecked = 2,
		e_NumPlacedImageStates
	};

	//--------------------------------------------------------------------
	// Bake preference object
	//--------------------------------------------------------------------
	class BakePrefsObject : public prtyObject
	{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		BakePrefsObject(cptrRenderBakeData& i_Data);
		~BakePrefsObject(){};

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void RegisterProperties();

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void Apply();
	protected:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Init();	

	private:
		void AddCallbacks();
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		virtual void UpdateBakeType(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateBakeOutputProperties(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateBakeProperties(prtyProperty *i_pProperty, bool i_bDirty);

		cptrRenderBakeData& m_Data;
		prtyPropertyUIInfo* pPUII_AA;
		prtyPropertyUIInfo* pPUII_Environment;
		prtyPropertyUIInfo* pPUII_Lit;
		prtyPropertyUIInfo* pPUII_Shadows;
	};

	BakePrefsObject::BakePrefsObject(cptrRenderBakeData& i_Data)
		: m_Data(i_Data)
	{
		Init();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void BakePrefsObject::RegisterProperties()
	{
		prtyPropertyUIInfo* pPUII;
		prtyComboBoxUIInfo* pCBUII;

		/*pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_BakeMode), "Bake Mode", "Select bake type");
		AddProperty( pCBUII );*/

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bIsBakeColor), "Bake Mode", "Bake Color");
		AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bIsBakeNormal), "Bake Mode", "Bake Normal");
		AddProperty( pPUII );

		pPUII_AA = new prtyCheckBoxUIInfo(&(m_Data.m_bHDRAA), "Bake", "Hardware Antialiasing");
		AddProperty( pPUII_AA );

		pPUII_Environment = new prtyCheckBoxUIInfo(&(m_Data.m_bIsEnvironment), "Advanced Render Flags", "Render environment");
		AddProperty( pPUII_Environment );

		pPUII_Lit = new prtyCheckBoxUIInfo(&(m_Data.m_bIsLit), "Advanced Render Flags", "Render lights");
		AddProperty( pPUII_Lit );

		pPUII_Shadows = new prtyCheckBoxUIInfo(&(m_Data.m_bIsShadows), "Advanced Render Flags", "Render lit pass");
		AddProperty( pPUII_Shadows );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bIsAOVolume), "Advanced Render Flags", "Render Volume AO");
		AddProperty( pPUII );
		pPUII->SetReadOnly(true);

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bIsLPVGI), "Advanced Render Flags", "Render LPV GI");
		AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_OutputFormat), "Output", "Output format");
		AddProperty( pCBUII );

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_OutputResolution), "Output", "Output format");
		AddProperty( pCBUII );

		pPUII = new prtyFolderChooserUIInfo(&(m_Data.m_OutputDir), "Output", "Directory to output textures");
		AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bIsSaveAndReplace), "Advanced Render Flags", "Replace material and save");
		AddProperty( pPUII );
	}

	void BakePrefsObject::Apply()
	{
		cptrRenderBakeDataUtil::UpdateData(m_Data);
	}

	void BakePrefsObject::Init()
	{
		RegisterProperties();
		AddCallbacks();
	}

	void BakePrefsObject::AddCallbacks()
	{
		m_Data.m_BakeMode.AddCallback(new prtyCallbackWrapper<BakePrefsObject>(this, &BakePrefsObject::UpdateBakeType));
		m_Data.m_bIsEnvironment.AddCallback(new prtyCallbackWrapper<BakePrefsObject>(this, &BakePrefsObject::UpdateBakeProperties));
		m_Data.m_bIsLit.AddCallback(new prtyCallbackWrapper<BakePrefsObject>(this, &BakePrefsObject::UpdateBakeProperties));
		m_Data.m_bIsShadows.AddCallback(new prtyCallbackWrapper<BakePrefsObject>(this, &BakePrefsObject::UpdateBakeProperties));
		m_Data.m_bHDRAA.AddCallback(new prtyCallbackWrapper<BakePrefsObject>(this, &BakePrefsObject::UpdateBakeProperties));
		m_Data.m_OutputFormat.AddCallback(new prtyCallbackWrapper<BakePrefsObject>(this, &BakePrefsObject::UpdateBakeOutputProperties));
		m_Data.m_OutputResolution.AddCallback(new prtyCallbackWrapper<BakePrefsObject>(this, &BakePrefsObject::UpdateBakeOutputProperties));
		m_Data.m_OutputDir.AddCallback(new prtyCallbackWrapper<BakePrefsObject>(this, &BakePrefsObject::UpdateBakeOutputProperties));
	}

	void BakePrefsObject::UpdateBakeType(prtyProperty *i_pProperty, bool i_bDirty)
	{
		/*switch (m_Data.m_BakeMode.GetValue())
		{
		case cptrRenderBakeData::bk_Color:
			pPUII_Environment->SetReadOnly(false);
			pPUII_Lit->SetReadOnly(false);
			pPUII_Shadows->SetReadOnly(false);
			break;
		case cptrRenderBakeData::bk_Normals:
			pPUII_Environment->SetReadOnly(true);
			pPUII_Lit->SetReadOnly(true);
			pPUII_Shadows->SetReadOnly(true);
			break;
		};*/
	}

	void BakePrefsObject::UpdateBakeOutputProperties(prtyProperty *i_pProperty, bool i_bDirty)
	{
	}

	void BakePrefsObject::UpdateBakeProperties(prtyProperty *i_pProperty, bool i_bDirty)
	{
	}

	//--------------------------------------------------------------------
	// Tree item data contains index for fragment node within object.
	// Needed because some fragment names are not unique when
	// using mesh instancing.
	//--------------------------------------------------------------------
	class NodeIndexTreeItemData : public wxTreeItemData
	{
	public:
		NodeIndexTreeItemData(int i_Index)
			: m_Index(i_Index) {}
		int m_Index;
	};

	const int l_dialogWidth = 380;
	const int l_dialogHeight = 480;
	BakePrefsObject* l_pBakePrefsObject = NULL;
}

cptrRenderBakeDialog* cptrRenderBakeDialog::Instance = NULL;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrRenderBakeDialog::cptrRenderBakeDialog(cptrRenderBakeData& i_Data, wxWindow* parent)
: wxDialog( parent, wxID_ANY, wxT("Bake Textures"), wxDefaultPosition, wxSize(l_dialogWidth, l_dialogHeight), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ),
  m_Data(i_Data),
  m_bakeAborted(true)
{
	wxBoxSizer* sizer_Dialog;
	sizer_Dialog = new wxBoxSizer( wxVERTICAL );

	m_panel_Dialog = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

	wxBoxSizer* fgSizer;
	fgSizer = new wxBoxSizer(wxVERTICAL);

	m_layerTabPages = new wxAuiNotebook( m_panel_Dialog, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxAUI_NB_SCROLL_BUTTONS );
	fgSizer->Add(m_layerTabPages, 1, wxALL|wxEXPAND, 5);

	// Create objects list tab
	/*wxPanel* panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* sizer1;
	sizer1 = new wxBoxSizer( wxVERTICAL );
	m_treeCtrl_Objects = new wxTreeCtrl( panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
											wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	sizer1->Add( m_treeCtrl_Objects, 1, wxALL|wxEXPAND, 5 ); 
	panel1->SetSizer(sizer1);
	m_layerTabPages->AddPage(panel1, wxT("Objects"), true);*/

	// Create render prefs tab
	//m_scrolledWindow_Prefs = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	//m_layerTabPages->AddPage(m_scrolledWindow_Prefs, wxT("Bake Prefs"), false);

	m_panel_Dialog->SetSizer(fgSizer);
	sizer_Dialog->Add( m_panel_Dialog, 1, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* sizer3;
	sizer3 = new wxBoxSizer(wxVERTICAL);

	wxButton* button1 = new wxButton(this, wxID_ANY, wxT("Bake Textures"), wxDefaultPosition, wxDefaultSize);
	sizer3->Add(button1, 0, wxALIGN_CENTER, 5);
	sizer_Dialog->Add(sizer3, 0.2, wxEXPAND | wxALL, 5);

	this->SetSizer( sizer_Dialog );

	// Update object tab
	//UpdateObjectList();
	// Register bake prefs layout
	if (!l_pBakePrefsObject)
	{
		l_pBakePrefsObject = new BakePrefsObject(cptrRenderBakeDataUtil::Data());
	}
	
	this->Layout();
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( cptrRenderBakeDialog::OnClose ) );
	button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cptrRenderBakeDialog::button_bake_OnButtonClick ), NULL, this );

	Update();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrRenderBakeDialog::~cptrRenderBakeDialog()
{
	if (cptrRenderBakeDialog::Instance == this)
		cptrRenderBakeDialog::Instance = NULL;

	if (l_pBakePrefsObject)
	{
		delete l_pBakePrefsObject;
		l_pBakePrefsObject = NULL;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//void rdrLayersDialog::Update(bool i_bPreviewDialog)
void cptrRenderBakeDialog::Update()
{
	UpdateTabbedPage();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBakeDialog::UpdateTabbedPage()
{
	UpdateObjectList();
	RegisterRenderPrefs();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBakeDialog::UpdateObjectList()
{
	m_layerTabPages->Freeze();

	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	//	Add the Objects tab page
	if (rdrLayersObjectsPage::Instance)
			rdrLayersObjectsPage::Instance = NULL;

	rdrLayersObjectsPage::Instance = new rdrLayersObjectsPage(m_layerTabPages);
	if(cptrRenderBakeDataUtil::GetBakeLayer() != NULL)
	{
		rdrLayersObjectsPage::Instance->Update(cptrRenderBakeDataUtil::GetBakeLayerName());
	}

	m_layerTabPages->AddPage( rdrLayersObjectsPage::Instance, wxT("Objects"), true );

	m_layerTabPages->Thaw();
}

//protected methods

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrRenderBakeDialog::RegisterRenderPrefs()
{
	const bool cbSHOW_CATEGORY = true;
	const bool cbAUTO_COLLAPSE = false;
	//	Add the Render tab page
	m_scrolledWindow_Prefs = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_layerTabPages->AddPage(m_scrolledWindow_Prefs, wxT("Bake Prefs"), false);
	
	m_scrolledWindow_Prefs->SetScrollRate( 0, 5 );
	
	wxBoxSizer* sizer_Render = new wxBoxSizer( wxVERTICAL );
	m_scrolledWindow_Prefs->SetSizer( sizer_Render );
	m_scrolledWindow_Prefs->Layout();
	sizer_Render->Fit( m_scrolledWindow_Prefs );

	if (l_pBakePrefsObject)
	{
		pwxFormControlBuilder::BuildForm(m_scrolledWindow_Prefs, "Bake Prefs", (l_pBakePrefsObject->GetListContainer()), cbSHOW_CATEGORY, cbAUTO_COLLAPSE );
	}
}

void cptrRenderBakeDialog::button_bake_OnButtonClick( wxCommandEvent& event )
{
	cptrRenderBakeDialogUtil::Hide();
	m_bakeAborted = false;
	this->Close();
}

void cptrRenderBakeDialog::OnClose( wxCloseEvent& event )
{
	//SetDataValues();

	cptrRenderBakeDialogUtil::Hide();
	cptrRenderBakeDialogUtil::SetDialogClosing( true );
	if (m_bakeAborted)
		cptrRenderBakeDialogUtil::SetDialogExitting(true);
}

#endif
