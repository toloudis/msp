/*****************************************************************************
**	SceneSetupDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/SceneSetup/GUI/qtGUI/SceneSetupDialog.hpp"

#include "Features/SceneSetup/Data/SceneSetupData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"

#include "Core/Fs/fsFileEnum.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "ToolUIQt/tqc/tqcDirPicker.hpp"

#ifdef USE_QT

#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>

//============================================================================
//============================================================================
namespace
{
	enum TabPages
	{
		e_ProjectsPage = 0,
		e_ScenesPage = 1,
		e_SummaryPage = 2
	};

	//--------------------------------------------------------------------
	// Mark project data as changed so that it can be written back to the
	// project data file.
	//--------------------------------------------------------------------
	void mark_project_data_dirty()
	{
		//	set the data as "dirty"
		ProjectSetupMgr::Data().m_bDirty = true;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void replace_last(std::wstring& str, const std::wstring& from, const std::wstring& to)
	{
		std::size_t pos = str.rfind(from);
		if (pos == std::string::npos)
			return;
		str.replace(pos, from.length(), to);
	}

	//--------------------------------------------------------------------
	// Select the item with the given text, returns false if not found
	//--------------------------------------------------------------------
	bool set_string_selection(QListWidget* i_pList, const QString& i_Text)
	{
		QList<QListWidgetItem*> items = i_pList->findItems(i_Text, Qt::MatchExactly);
		if (items.isEmpty())
			return false;
		i_pList->setCurrentItem(items.first());
		return true;
	}

	std::string get_string_selection(QListWidget* i_pList)
	{
		QListWidgetItem* pItem = i_pList->currentItem();
		return (pItem) ? tqtStdString(pItem->text()) : std::string();
	}

	//--------------------------------------------------------------------
	// Previous / Next buttons centered at the bottom of a page
	//--------------------------------------------------------------------
	QHBoxLayout* create_button_row(QWidget* i_pParent, QPushButton*& o_pPrev, QPushButton*& o_pNext, const char* i_pNextText = "Next")
	{
		QHBoxLayout* pRow = new QHBoxLayout();
		pRow->addStretch( 1 );
		o_pPrev = new QPushButton( "Previous", i_pParent );
		o_pPrev->setAutoDefault( false );
		pRow->addWidget( o_pPrev );
		o_pNext = new QPushButton( i_pNextText, i_pParent );
		o_pNext->setAutoDefault( false );
		pRow->addWidget( o_pNext );
		pRow->addStretch( 1 );
		return pRow;
	}
}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
SceneSetupDialog::SceneSetupDialog( QWidget* parent,
								    SceneSetupData& i_Data )
: QDialog( parent ),
	m_SceneSetupData(i_Data),
	m_bChangingPage(false)
{
	this->setWindowTitle( "Scene Setup" );

	QVBoxLayout* bSizer1 = new QVBoxLayout( this );
	m_notebook1 = new QTabWidget( this );
	bSizer1->addWidget( m_notebook1, 1 );

	// Project page
	QWidget* pProjectPage = new QWidget( m_notebook1 );
	QVBoxLayout* bSizer61 = new QVBoxLayout( pProjectPage );
	bSizer61->addWidget( new QLabel( "What Project is this for?", pProjectPage ) );
	m_listBox_Projects = new QListWidget( pProjectPage );
	bSizer61->addWidget( m_listBox_Projects, 1 );

	QGroupBox* pProjectData = new QGroupBox( "Project Data", pProjectPage );
	QVBoxLayout* sbSizer2 = new QVBoxLayout( pProjectData );
	sbSizer2->addWidget( new QLabel( "Project Name (this will be the filename too)", pProjectData ) );
	m_textCtrl_ProjectName = new QLineEdit( pProjectData );
	sbSizer2->addWidget( m_textCtrl_ProjectName );
	sbSizer2->addWidget( new QLabel( "Project Description", pProjectData ) );
	m_textCtrl_ProjectDescription = new QLineEdit( pProjectData );
	sbSizer2->addWidget( m_textCtrl_ProjectDescription );
	sbSizer2->addWidget( new QLabel( "Project Directory", pProjectData ) );
	m_dirPicker_ProjectDir = new tqcDirPicker( pProjectData );
	sbSizer2->addWidget( m_dirPicker_ProjectDir );
	bSizer61->addWidget( pProjectData );

	QPushButton* pProjectPrev = NULL;
	QPushButton* pProjectNext = NULL;
	bSizer61->addLayout( create_button_row( pProjectPage, pProjectPrev, pProjectNext ) );
	pProjectPrev->setEnabled( false );
	m_notebook1->addTab( pProjectPage, "Project" );

	// Scene page
	QWidget* pScenePage = new QWidget( m_notebook1 );
	QVBoxLayout* bSizer8 = new QVBoxLayout( pScenePage );
	bSizer8->addWidget( new QLabel( "Pick the scene", pScenePage ) );
	QHBoxLayout* bSizer9 = new QHBoxLayout();
	m_listBox_Scenes = new QListWidget( pScenePage );
	bSizer9->addWidget( m_listBox_Scenes, 1 );
	QVBoxLayout* bSizer10 = new QVBoxLayout();
	QPushButton* pRemoveScene = new QPushButton( "Remove Scene", pScenePage );
	pRemoveScene->setAutoDefault( false );
	bSizer10->addWidget( pRemoveScene );
	QPushButton* pImportSceneList = new QPushButton( "Import Scene\nList", pScenePage );
	pImportSceneList->setAutoDefault( false );
	bSizer10->addWidget( pImportSceneList );
	bSizer10->addStretch( 1 );
	bSizer9->addLayout( bSizer10 );
	bSizer8->addLayout( bSizer9, 1 );

	QGroupBox* pSceneData = new QGroupBox( "Scene Data", pScenePage );
	QVBoxLayout* sbSizer3 = new QVBoxLayout( pSceneData );
	sbSizer3->addWidget( new QLabel( "Scene Name", pSceneData ) );
	m_textCtrl_SceneName = new QLineEdit( pSceneData );
	sbSizer3->addWidget( m_textCtrl_SceneName );
	sbSizer3->addWidget( new QLabel( "Scene Description", pSceneData ) );
	m_textCtrl_SceneDescription = new QLineEdit( pSceneData );
	sbSizer3->addWidget( m_textCtrl_SceneDescription );
	bSizer8->addWidget( pSceneData );

	QPushButton* pScenePrev = NULL;
	QPushButton* pSceneNext = NULL;
	bSizer8->addLayout( create_button_row( pScenePage, pScenePrev, pSceneNext ) );
	m_notebook1->addTab( pScenePage, "Scene Manager" );

	// Summary page
	QWidget* pSummaryPage = new QWidget( m_notebook1 );
	QVBoxLayout* bSizer3 = new QVBoxLayout( pSummaryPage );
	QGroupBox* pSummary = new QGroupBox( "Project-Scene-Summary", pSummaryPage );
	QVBoxLayout* bSizer7 = new QVBoxLayout( pSummary );
	m_staticText_SummaryProject = new QLabel( "Project:", pSummary );
	bSizer7->addWidget( m_staticText_SummaryProject );
	m_staticText_SummaryDirectory = new QLabel( "Directory:", pSummary );
	bSizer7->addWidget( m_staticText_SummaryDirectory );
	m_staticText_SummaryScene = new QLabel( "Scene:", pSummary );
	bSizer7->addWidget( m_staticText_SummaryScene );
	bSizer7->addStretch( 1 );
	bSizer3->addWidget( pSummary, 1 );

	m_checkBox_SetDefault = new QCheckBox( "Set this scene as default", pSummaryPage );
	bSizer3->addWidget( m_checkBox_SetDefault );

	QPushButton* pCreateDirs = new QPushButton( "Create Directories", pSummaryPage );
	pCreateDirs->setAutoDefault( false );
	bSizer3->addWidget( pCreateDirs, 0, Qt::AlignHCenter );

	QPushButton* pSummaryPrev = NULL;
	QPushButton* pSummaryFinish = NULL;
	bSizer3->addLayout( create_button_row( pSummaryPage, pSummaryPrev, pSummaryFinish, "Finish" ) );
	m_notebook1->addTab( pSummaryPage, "Summary" );

	// Connect events
	QObject::connect( m_notebook1, &QTabWidget::currentChanged, [this](int i_Page) { OnNotebookPageChanged(i_Page); } );
	QObject::connect( m_listBox_Projects, &QListWidget::currentRowChanged, [this](int) { update_project_index(); } );
	QObject::connect( m_listBox_Scenes, &QListWidget::currentRowChanged, [this](int) { update_scene_index(); } );
	QObject::connect( pRemoveScene, &QPushButton::clicked, [this]() { button_RemoveScene_Click(); } );
	QObject::connect( pImportSceneList, &QPushButton::clicked, [this]() { button_ImportSceneList_Click(); } );
	QObject::connect( pCreateDirs, &QPushButton::clicked, []() { ProjectSetupMgr::CreateDirectories(); } );
	QObject::connect( pProjectNext, &QPushButton::clicked, [this]()
	{
		tabPage_scene_setup();
		change_page(e_ScenesPage);
	} );
	QObject::connect( pScenePrev, &QPushButton::clicked, [this]() { change_page(e_ProjectsPage); } );
	QObject::connect( pSceneNext, &QPushButton::clicked, [this]()
	{
		// setup summary controls when moving to that tab page
		tabPage_summary_setup();
		change_page(e_SummaryPage);
	} );
	QObject::connect( pSummaryPrev, &QPushButton::clicked, [this]()
	{
		// setup scene controls when moving to that tab page
		tabPage_scene_setup();
		change_page(e_ScenesPage);
	} );
	QObject::connect( pSummaryFinish, &QPushButton::clicked, [this]() { button_SummaryFinish_Click(); } );

	//	configure the initial state of the data
	m_SceneSetupData.m_Data.m_bFinished = false;
	PrefsData prefsdata = PrefsMgr::Data();
	m_SceneSetupData.m_Data.m_ProjectName = prefsdata.m_LastProjectName.GetValue();

	// Fill the project list box and set initial selection
	this->build_project_list();
	change_page(e_ProjectsPage);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
SceneSetupDialog::~SceneSetupDialog()
{
}

//--------------------------------------------------------------------
// Change the tab page without the page change handling
//--------------------------------------------------------------------
void SceneSetupDialog::change_page( int i_Page )
{
	m_bChangingPage = true;
	m_notebook1->setCurrentIndex(i_Page);
	m_bChangingPage = false;
}

//--------------------------------------------------------------------
// Fill the project list box
//--------------------------------------------------------------------
void SceneSetupDialog::build_project_list()
{
	QSignalBlocker blocker(m_listBox_Projects);
	m_listBox_Projects->clear();

	//	set the built-in options
	m_listBox_Projects->addItem( "New Project" );

	//	grab the saved ones
	fsFileEnum::fsFileList filelist;
	fsFileEnum::EnumerateFiles( gfPaths::GetPath(mnmPaths::e_SaveProjectFiles), filelist, itString(".mpj") );

	// build the list
	int filenum = filelist.size();
	for ( int i = 0 ; i < filenum ; i++ )
	{
		std::wstring project_name(filelist[i].GetLastName().GetString());
		// strip off the extension
		replace_last(project_name, L".mpj", L"");
		m_listBox_Projects->addItem( QString::fromStdWString(project_name) );
	}

	if (m_SceneSetupData.m_Data.m_ProjectName.empty() ||
		!set_string_selection(m_listBox_Projects, tqtQString(m_SceneSetupData.m_Data.m_ProjectName)))
	{
		// couldn't find project name, so select "New Project" instead
		m_listBox_Projects->setCurrentRow(0);
	}

	update_project_index();
}

//--------------------------------------------------------------------
// Fill the text boxes in the projects tabs with the project data
//--------------------------------------------------------------------
void SceneSetupDialog::set_project_controls()
{
	ProjectSetupData& PSData = ProjectSetupMgr::Data();
	m_textCtrl_ProjectName->setText( tqtQString(PSData.m_ProjectName) );
	m_textCtrl_ProjectDescription->setText( tqtQString(PSData.m_ProjectDesc) );
	m_dirPicker_ProjectDir->SetPath( tqtQString(PSData.m_ProjectDirectory) );

	// Project Name text can only be edited when it is a New Project
	m_textCtrl_ProjectName->setEnabled(m_listBox_Projects->currentRow() == 0);
}

//--------------------------------------------------------------------
// Get values from the selected project into our local scene data
//--------------------------------------------------------------------
void SceneSetupDialog::set_scene_setup_data()
{
	//	Fill-in the Scene Data
	ProjectSetupData& PSData = ProjectSetupMgr::Data();
	m_SceneSetupData.m_Data.m_ProjectName		= PSData.m_ProjectName;
	m_SceneSetupData.m_Data.m_ProjectDirectory	= PSData.m_ProjectDirectory;
	m_SceneSetupData.m_Data.m_SceneName			= PSData.m_CurrentSceneName;
}

//--------------------------------------------------------------------
// Handle change in selection from projects list box
//--------------------------------------------------------------------
void SceneSetupDialog::update_project_index()
{
	if ( m_listBox_Projects->currentRow() > 0 )
	{
		//	Read the Project File
		ProjectSetupMgr::ReadProject( itString( m_listBox_Projects->currentItem()->text().toStdWString().c_str() ) );
		mark_project_data_dirty();
	}
	else
	{
		//	"new project" was selected so clear out the scene list + data
		ProjectSetupData& PSData = ProjectSetupMgr::Data();
		PSData.m_Scenes.clear();
		PSData.m_ProjectName = "New";
		PSData.m_CurrentSceneName = "";
		PSData.m_ProjectDesc = "";
		PSData.m_ProjectDirectory = "c:\\Projects";
	}

	set_scene_setup_data();
	set_project_controls();
}

//--------------------------------------------------------------------
// Fill the list box for scenes with the scenes for the selected
//	project
//--------------------------------------------------------------------
void SceneSetupDialog::build_scene_list()
{
	QSignalBlocker blocker(m_listBox_Scenes);
	m_listBox_Scenes->clear();

	//	set the built-in options
	m_listBox_Scenes->addItem( "New Scene" );	// index 0

	//	add the scenes to the list box
	ProjectSetupData& PSData = ProjectSetupMgr::Data();
	for( int i = 0 ; i < PSData.m_Scenes.size() ; i++ )
	{
		m_listBox_Scenes->addItem( tqtQString(PSData.m_Scenes[i].m_SceneName) );
	}

	// Set selection
	m_listBox_Scenes->setCurrentRow( 0 );
	if ( !m_SceneSetupData.m_Data.m_SceneName.empty() )
	{
		set_string_selection( m_listBox_Scenes, tqtQString(m_SceneSetupData.m_Data.m_SceneName) );
	}
}

//--------------------------------------------------------------------
// Tab control is about to switch to the scene tab page,
//	set up the controls for it.
//--------------------------------------------------------------------
void SceneSetupDialog::tabPage_scene_setup()
{
	build_scene_list();
	update_scene_index();
}

//--------------------------------------------------------------------
// Fill the text boxes in the scenes tab with the project data
//--------------------------------------------------------------------
void SceneSetupDialog::set_scene_controls()
{
	int sel_index = m_listBox_Scenes->currentRow() - 1;
	if ( sel_index >= 0 )
	{
		ProjectSetupData& PSData = ProjectSetupMgr::Data();
		m_textCtrl_SceneName->setText( tqtQString(PSData.m_Scenes[sel_index].m_SceneName) );
		m_textCtrl_SceneDescription->setText( tqtQString(PSData.m_Scenes[sel_index].m_SceneDesc) );
	}
}

//--------------------------------------------------------------------
// Handle change in selection from scenes list box
//--------------------------------------------------------------------
void SceneSetupDialog::update_scene_index()
{
	//	if scenes combo has "new scene" selected then add it to the project
	if ( m_listBox_Scenes->currentRow() != 0 )
	{
		m_SceneSetupData.m_Data.m_SceneName = get_string_selection(m_listBox_Scenes);
		ProjectSetupData& PSData = ProjectSetupMgr::Data();
		PSData.m_CurrentSceneName = m_SceneSetupData.m_Data.m_SceneName;
	}

	m_textCtrl_SceneName->setEnabled( m_listBox_Scenes->currentRow() == 0 );

	set_scene_controls();
	mark_project_data_dirty();		//	set the data as "dirty"
}

//--------------------------------------------------------------------
// Sync up project data with the values from the dialog
//--------------------------------------------------------------------
void SceneSetupDialog::set_project_data()
{
	ProjectSetupData& PSData = ProjectSetupMgr::Data();

	//	project-specific data
	m_SceneSetupData.m_Data.m_ProjectDirectory = tqtStdString(m_dirPicker_ProjectDir->GetPath());
	if ( m_listBox_Projects->currentRow() == 0 )
	{
		m_SceneSetupData.m_Data.m_ProjectName = tqtStdString(m_textCtrl_ProjectName->text());
		//	if projects combo has "new project" selected then add it to the project
		PSData.m_ProjectName = m_SceneSetupData.m_Data.m_ProjectName;
		PSData.m_ProjectDesc = tqtStdString(m_textCtrl_ProjectDescription->text());
		PSData.m_ProjectDirectory = m_SceneSetupData.m_Data.m_ProjectDirectory;
		mark_project_data_dirty();
	}
	else
	{
		m_SceneSetupData.m_Data.m_ProjectName = get_string_selection(m_listBox_Projects);

		// if not new, check if any of the data has changed
		std::string info = tqtStdString(m_textCtrl_ProjectDescription->text());
		if (info != PSData.m_ProjectDesc)
		{
			PSData.m_ProjectDesc = info;
			mark_project_data_dirty();
		}
		if (m_SceneSetupData.m_Data.m_ProjectDirectory != PSData.m_ProjectDirectory)
		{
			PSData.m_ProjectDirectory = m_SceneSetupData.m_Data.m_ProjectDirectory;
			mark_project_data_dirty();
		}
	}
}

//--------------------------------------------------------------------
// Sync up scene data with the values from the dialog
//--------------------------------------------------------------------
void SceneSetupDialog::set_scene_data()
{
	ProjectSetupData& PSData = ProjectSetupMgr::Data();

	//	scene-specific data
	if ( m_listBox_Scenes->currentRow() == 0 )
	{
		m_SceneSetupData.m_Data.m_SceneName = tqtStdString(m_textCtrl_SceneName->text());
		//	if scene list box has "new scene" selected then add it to the project
		ProjectSetupMgr::AddSceneToProject( m_SceneSetupData.m_Data.m_SceneName,
			tqtStdString(m_textCtrl_SceneDescription->text()));
	}
	else
	{
		m_SceneSetupData.m_Data.m_SceneName = get_string_selection(m_listBox_Scenes);
		PSData.m_CurrentSceneName = m_SceneSetupData.m_Data.m_SceneName;
	}
}

//--------------------------------------------------------------------
// Sync up our data with the values from the dialog
//--------------------------------------------------------------------
void SceneSetupDialog::set_data()
{
	set_project_data();
	set_scene_data();

	// Write out the prefs
	if ( m_listBox_Projects->currentRow() != 0 )
	{
		PrefsData& prefsdata = PrefsMgr::Data();
		prefsdata.m_LastProjectName = get_string_selection(m_listBox_Projects);

		// Check box controls if this scene is the default project and scene
		// to load on start up
		if (m_checkBox_SetDefault->isChecked())
		{
			prefsdata.m_DefaultProjectName = get_string_selection(m_listBox_Projects);
			prefsdata.m_DefaultProjectDirectory = tqtStdString(m_dirPicker_ProjectDir->GetPath());
			prefsdata.m_DefaultSceneName = get_string_selection(m_listBox_Scenes);
		}

		PrefsMgr::WritePrefs();
	}
}

//--------------------------------------------------------------------
// Tab control is about to switch to the summary tab page,
//	set up the controls for it.
//--------------------------------------------------------------------
void SceneSetupDialog::tabPage_summary_setup()
{
	// Sync the data from the project and scene tabs into the
	// scene setup and project data structs
	set_data();

	//	scene-specific data
	ProjectSetupData& PSData = ProjectSetupMgr::Data();
	m_staticText_SummaryProject->setText( "Project: " + tqtQString(m_SceneSetupData.m_Data.m_ProjectName) );
	m_staticText_SummaryDirectory->setText( "Directory: " + tqtQString(m_SceneSetupData.m_Data.m_ProjectDirectory) );
	m_staticText_SummaryScene->setText( "Scene: " + tqtQString(PSData.m_CurrentSceneName) );
}

//--------------------------------------------------------------------
// Dialog is closing, finalize the choices made by the user.
//--------------------------------------------------------------------
void SceneSetupDialog::finalize_data()
{
	set_data();
	m_SceneSetupData.m_Data.m_bFinished = true;

	//	Write the Project File
	ProjectSetupData& PSData = ProjectSetupMgr::Data();
	if (PSData.m_bDirty)
	{
		ProjectSetupMgr::WriteProject( itString( PSData.m_ProjectName.c_str() ) );
		PSData.m_bDirty = false;
	}

	//	prepare the initial scene directory based on project settings
	itString scene_name(m_SceneSetupData.m_Data.m_SceneName.c_str());
	fsLocator project_dir;
	fsFileUtil::ANSIFilenameToLocator( m_SceneSetupData.m_Data.m_ProjectDirectory, project_dir );
	mnmPaths::SetupPaths( project_dir, scene_name );
	guiSingleDocHandler::SetInitialDirectory( gfPaths::GetPath( mnmPaths::e_SaveShots ) );
}

//--------------------------------------------------------------------
// event handling
//--------------------------------------------------------------------
void SceneSetupDialog::OnNotebookPageChanged( int i_Page )
{
	if (m_bChangingPage)
		return;

	if ( i_Page == e_ScenesPage )
	{
		// setup scene controls when moving to that tab page
		tabPage_scene_setup();
	}
	else if ( i_Page == e_SummaryPage )
	{
		// setup summary controls when moving to that tab page
		tabPage_summary_setup();
	}
}

void SceneSetupDialog::button_RemoveScene_Click()
{
	int sel_index = m_listBox_Scenes->currentRow() - 1;
	if ( sel_index >= 0 )
	{
		ProjectSetupMgr::RemoveSceneFromProject(sel_index);

		//	rebuild the scene list
		build_scene_list();
		set_scene_controls();
	}
}

void SceneSetupDialog::button_ImportSceneList_Click()
{
	ProjectSetupMgr::ImportSceneList();

	//	rebuild the scene list
	build_scene_list();
	set_scene_controls();
}

void SceneSetupDialog::button_SummaryFinish_Click()
{
	//	grab the information from the GUI and put it in the data
	finalize_data();

	//	if this is a NEW project then ask the user to create the directories
	if ( m_listBox_Projects->currentRow() == 0 )
	{
		int retval = guiMessageBox::Show( "This is a new project do you want to create the directories for this project?","New Project Creation", guiMessageBox::e_YesNo );
		if ( retval == guiMessageBox::e_Yes )
		{
			fsLocator dir;
			fsFileUtil::ANSIFilenameToLocator(m_SceneSetupData.m_Data.m_ProjectDirectory, dir );
			mnmPaths::CreateProjectDirectories( dir );
		}
	}

	this->accept();
}

#endif // USE_QT
