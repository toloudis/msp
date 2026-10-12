/*****************************************************************************
**	cptrRenderBakeDialog.cpp
**
**		see .hpp
**
\****************************************************************************/
#include "Features/Capture/qtGUI/cptrRenderBakeDialog.hpp"

#include "Features/Capture/cptrRenderBakeDataUtil.hpp"
#include "Features/Capture/cptrRenderBakeDialogUtil.hpp"
#include "Features/RenderLayers/qtGUI/rdrLayersObjectsPage.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"

#include "Core/fs/fsFileUtil.hpp"
//#include "Core/name/nameString.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyFolderChooserUIInfo.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"

#include <vector>
#include <string>

//show object list with fragments
#define DO_FRAG

#ifdef USE_QT

#include <QCloseEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>

//============================================================================
//============================================================================
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


	const int l_dialogWidth = 380;
	const int l_dialogHeight = 480;
	BakePrefsObject* l_pBakePrefsObject = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrRenderBakeDialog* cptrRenderBakeDialog::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrRenderBakeDialog::cptrRenderBakeDialog(cptrRenderBakeData& i_Data, QWidget* parent)
: QDialog( parent ),
  m_Data(i_Data),
  m_bakeAborted(true)
{
	this->setWindowTitle( "Bake Textures" );

	QVBoxLayout* sizer_Dialog = new QVBoxLayout( this );
	m_layerTabPages = new QTabWidget( this );
	m_layerTabPages->setUsesScrollButtons( true );
	sizer_Dialog->addWidget( m_layerTabPages, 1 );

	QPushButton* button1 = new QPushButton( "Bake Textures", this );
	button1->setAutoDefault( false );
	sizer_Dialog->addWidget( button1, 0, Qt::AlignHCenter );

	// Register bake prefs layout
	if (!l_pBakePrefsObject)
	{
		l_pBakePrefsObject = new BakePrefsObject(cptrRenderBakeDataUtil::Data());
	}

	this->resize( l_dialogWidth, l_dialogHeight );

	QObject::connect( button1, &QPushButton::clicked, [this]() { button_bake_OnButtonClick(); } );

	Update();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
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
	//	Add the Objects tab page
	rdrLayersObjectsPage::Instance = new rdrLayersObjectsPage(m_layerTabPages);
	if(cptrRenderBakeDataUtil::GetBakeLayer() != NULL)
	{
		rdrLayersObjectsPage::Instance->Update(cptrRenderBakeDataUtil::GetBakeLayerName());
	}
	m_layerTabPages->addTab( rdrLayersObjectsPage::Instance, "Objects" );
	m_layerTabPages->setCurrentWidget( rdrLayersObjectsPage::Instance );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBakeDialog::RegisterRenderPrefs()
{
	//	Add the Render tab page
	QScrollArea* pScroll = new QScrollArea( m_layerTabPages );
	pScroll->setWidgetResizable( true );
	pScroll->setFrameShape( QFrame::NoFrame );
	pScroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	m_scrolledWindow_Prefs = new QWidget();
	pScroll->setWidget( m_scrolledWindow_Prefs );
	m_layerTabPages->addTab( pScroll, "Bake Prefs" );

	if (l_pBakePrefsObject)
	{
		pqtFormControlBuilder::BuildForm(m_scrolledWindow_Prefs, "Bake Prefs", (l_pBakePrefsObject->GetListContainer()), true, false );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBakeDialog::button_bake_OnButtonClick()
{
	cptrRenderBakeDialogUtil::Hide();
	m_bakeAborted = false;
	this->close();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderBakeDialog::closeEvent( QCloseEvent* i_pEvent )
{
	cptrRenderBakeDialogUtil::Hide();
	cptrRenderBakeDialogUtil::SetDialogClosing( true );
	if (m_bakeAborted)
		cptrRenderBakeDialogUtil::SetDialogExitting(true);
	QDialog::closeEvent( i_pEvent );
}

#endif
