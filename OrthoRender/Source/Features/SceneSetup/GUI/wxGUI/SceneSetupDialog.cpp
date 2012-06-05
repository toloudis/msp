/*****************************************************************************
**	SceneSetupDialog.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/SceneSetup/GUI/wxGUI/SceneSetupDialog.hpp"
#include "Features/SceneSetup/Data/SceneSetupData.hpp"

#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"

#include "Support/mnm/mnmPaths.hpp"

#include "Core/Fs/fsFileEnum.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"

#include <boost/algorithm/string.hpp>

#ifdef USE_WXWIDGETS

namespace
{
	enum TabPages
	{
		e_ProjectsPage = 0,
		e_ScenesPage = 1,
		e_SummaryPage = 2
	};

	//------------------------------------------------------------------------
	// Mark project data as changed so that it can be written back to the
	// project data file.
	//------------------------------------------------------------------------
	void mark_project_data_dirty()
	{
		//	set the data as "dirty"
		ProjectSetupMgr::Data().m_bDirty = true;
	}

}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
SceneSetupDialog::SceneSetupDialog( wxWindow* parent,
								    SceneSetupData& i_Data )
: SceneSetupDialogBase( parent ),
	m_SceneSetupData(i_Data)
{
	//	configure the initial state of the data
	//
	m_SceneSetupData.m_Data.m_bFinished = false;

	PrefsData prefsdata = PrefsMgr::Data();
	m_SceneSetupData.m_Data.m_ProjectName = prefsdata.m_LastProjectName.GetValue();

	// Full the project list box and set initial selection
	this->build_project_list();

	m_notebook1->ChangeSelection(e_ProjectsPage);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
SceneSetupDialog::~SceneSetupDialog()
{
	
}

//------------------------------------------------------------------------
// Fill the project list box
//------------------------------------------------------------------------
void SceneSetupDialog::build_project_list()
{
	m_listBox_Projects->Clear();

	//	set the built-in options
	//
	m_listBox_Projects->Append( "New Project" );

	//	grab the saved ones
	fsFileEnum::fsFileList filelist;
	fsFileEnum::EnumerateFiles( gfPaths::GetPath(mnmPaths::e_SaveProjectFiles), filelist, itString(".mpj") );

	// build the list
	int filenum = filelist.size();
	for ( int i = 0 ; i < filenum ; i++ )
	{
		std::string project_name = itStringUtil::GetStdString(filelist[i].GetLastName());
		// strip off the extension
		boost::replace_last(project_name, ".mpj", "");
		m_listBox_Projects->Append( project_name );

		//DBG_LOG3( "build project list %d of %d (%s)", i, filenum, itStringUtil::GetStdString(name).c_str() );
	}

	if (!m_SceneSetupData.m_Data.m_ProjectName.empty())
	{
		if (!m_listBox_Projects->SetStringSelection( m_SceneSetupData.m_Data.m_ProjectName ))
		{
			// couldn't find project name, so select "New Project" instead
			m_listBox_Projects->SetSelection(0);
		}
	}
	else
	{
		m_listBox_Projects->SetSelection(0);
	}
	update_project_index();
}

//------------------------------------------------------------------------
// Fill the text boxes in the projects tabs with the project data
//------------------------------------------------------------------------
void SceneSetupDialog::set_project_controls()
{
	ProjectSetupData& PSData = ProjectSetupMgr::Data();
	m_textCtrl_ProjectName->SetValue( PSData.m_ProjectName );
	m_textCtrl_ProjectDescription->SetValue( PSData.m_ProjectDesc );
	m_dirPicker_ProjectDir->SetPath( PSData.m_ProjectDirectory );

	// Project Name text can only be edited when it is a New Project
	m_textCtrl_ProjectName->Enable(m_listBox_Projects->GetSelection() == 0);
}

//------------------------------------------------------------------------
// Get values from the selected project into our local scene data
//------------------------------------------------------------------------
void SceneSetupDialog::set_scene_setup_data()
 {
	//	Fill-in the Scene Data
	//
	ProjectSetupData& PSData = ProjectSetupMgr::Data();
	m_SceneSetupData.m_Data.m_ProjectName		= PSData.m_ProjectName;
	m_SceneSetupData.m_Data.m_ProjectDirectory	= PSData.m_ProjectDirectory;
	m_SceneSetupData.m_Data.m_SceneName			= PSData.m_CurrentSceneName;
 }

//------------------------------------------------------------------------
// Handle change in selection from projects list box
//------------------------------------------------------------------------
void SceneSetupDialog::update_project_index()
{
	if ( m_listBox_Projects->GetSelection() > 0 )
	{
		//	Read the Project File
		//
		ProjectSetupMgr::ReadProject( itString( m_listBox_Projects->GetStringSelection() ) );
		mark_project_data_dirty();
	}
	else
	{
		//	"new project" was selected so clear out the scene list + data
		//
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

//------------------------------------------------------------------------
// Fill the list box for scenes with the scenes for the selected
//	project
//------------------------------------------------------------------------
void SceneSetupDialog::build_scene_list()
{
	m_listBox_Scenes->Clear();

	//	set the built-in options
	m_listBox_Scenes->Append( "New Scene" );	// index 0	

	//	add the scenes to the list box
	ProjectSetupData& PSData = ProjectSetupMgr::Data();
	for( int i = 0 ; i < PSData.m_Scenes.size() ; i++ )
	{
		m_listBox_Scenes->Append( PSData.m_Scenes[i].m_SceneName );
	}

	// Set selection
	m_listBox_Scenes->SetSelection( 0 );
	if ( !m_SceneSetupData.m_Data.m_SceneName.empty() )
	{
		m_listBox_Scenes->SetStringSelection( m_SceneSetupData.m_Data.m_SceneName );
	}
}

//------------------------------------------------------------------------
// Tab control is about to switch to the scene tab page,
//	set up the controls for it.
//------------------------------------------------------------------------
void SceneSetupDialog::tabPage_scene_setup()
{
	build_scene_list();

	update_scene_index();
}

//------------------------------------------------------------------------
// Fill the text boxes in the scenes tab with the project data
//------------------------------------------------------------------------
void SceneSetupDialog::set_scene_controls()
{
	 int sel_index = m_listBox_Scenes->GetSelection() - 1;
	 if ( sel_index >= 0 )
	 {
		ProjectSetupData& PSData = ProjectSetupMgr::Data();

		m_textCtrl_SceneName->SetValue( PSData.m_Scenes[sel_index].m_SceneName );
		m_textCtrl_SceneDescription->SetValue( PSData.m_Scenes[sel_index].m_SceneDesc );
	 }
}

//------------------------------------------------------------------------
// Handle change in selection from scenes list box
//------------------------------------------------------------------------
void SceneSetupDialog::update_scene_index()
{
	//	if scenes combo has "new scene" selected then add it to the project
	//
	if ( m_listBox_Scenes->GetSelection() != 0 )
	{
		m_SceneSetupData.m_Data.m_SceneName = m_listBox_Scenes->GetStringSelection();
		ProjectSetupData& PSData = ProjectSetupMgr::Data();
		PSData.m_CurrentSceneName = m_SceneSetupData.m_Data.m_SceneName;
	}
	m_textCtrl_SceneName->Enable( m_listBox_Scenes->GetSelection() == 0 );

	set_scene_controls();
	mark_project_data_dirty();		//	set the data as "dirty"
}

//------------------------------------------------------------------------
// Sync up project data with the values from the dialog
//------------------------------------------------------------------------
void SceneSetupDialog::set_project_data()
{
	ProjectSetupData& PSData = ProjectSetupMgr::Data();

	//	project-specific data
	m_SceneSetupData.m_Data.m_ProjectDirectory = m_dirPicker_ProjectDir->GetPath();

	if ( m_listBox_Projects->GetSelection() == 0 )
	{
		m_SceneSetupData.m_Data.m_ProjectName = m_textCtrl_ProjectName->GetValue();
	
		//	if projects combo has "new project" selected then add it to the project
		PSData.m_ProjectName = m_SceneSetupData.m_Data.m_ProjectName;
		PSData.m_ProjectDesc = m_textCtrl_ProjectDescription->GetValue();
		PSData.m_ProjectDirectory = m_SceneSetupData.m_Data.m_ProjectDirectory;
		
		mark_project_data_dirty();
	}
	else
	{
		m_SceneSetupData.m_Data.m_ProjectName = m_listBox_Projects->GetStringSelection();

		// if not new, check if any of the data has changed
		//
		std::string info = m_textCtrl_ProjectDescription->GetValue();
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

//------------------------------------------------------------------------
// Sync up scene data with the values from the dialog
//------------------------------------------------------------------------
void SceneSetupDialog::set_scene_data()
{
	ProjectSetupData& PSData = ProjectSetupMgr::Data();

	//	scene-specific data
	if ( m_listBox_Scenes->GetSelection() == 0 )
	{
		m_SceneSetupData.m_Data.m_SceneName = m_textCtrl_SceneName->GetValue();

		//	if scene list box has "new scene" selected then add it to the project
		//
		ProjectSetupMgr::AddSceneToProject( m_SceneSetupData.m_Data.m_SceneName,
											m_textCtrl_SceneDescription->GetValue().c_str());
	}
	else
	{
		m_SceneSetupData.m_Data.m_SceneName = m_listBox_Scenes->GetStringSelection();
		PSData.m_CurrentSceneName = m_SceneSetupData.m_Data.m_SceneName;
	}
}

//------------------------------------------------------------------------
// Sync up our data with the values from the dialog
//------------------------------------------------------------------------
void SceneSetupDialog::set_data()
{
	set_project_data();
	set_scene_data();

	// Write out the prefs
	//
	if ( m_listBox_Projects->GetSelection() != 0 )
	{
		PrefsData& prefsdata = PrefsMgr::Data();
		prefsdata.m_LastProjectName = m_listBox_Projects->GetStringSelection();
	
		// Check box controls if this scene is the default project and scene
		// to load on start up
		if (m_checkBox_SetDefault->GetValue())
		{
			prefsdata.m_DefaultProjectName = m_listBox_Projects->GetStringSelection();
			prefsdata.m_DefaultProjectDirectory = m_dirPicker_ProjectDir->GetPath();
			prefsdata.m_DefaultSceneName = m_listBox_Scenes->GetStringSelection();
		}

		PrefsMgr::WritePrefs();
	}
}

//------------------------------------------------------------------------
// Tab control is about to switch to the summary tab page,
//	set up the controls for it.
//------------------------------------------------------------------------
void SceneSetupDialog::tabPage_summary_setup()
{
	// Sync the data from the project and scene tabs into the 
	// scene setup and project data structs
	set_data();

	//	scene-specific data
	ProjectSetupData& PSData = ProjectSetupMgr::Data();
	m_staticText_SummaryProject->SetLabel(wxString("Project: ") + m_SceneSetupData.m_Data.m_ProjectName);
	m_staticText_SummaryDirectory->SetLabel(wxString("Directory: ") + m_SceneSetupData.m_Data.m_ProjectDirectory);
	m_staticText_SummaryScene->SetLabel(wxString("Scene: ") + PSData.m_CurrentSceneName);
}


//------------------------------------------------------------------------
// Dialog is closing, finalize the choices made by the user.
//------------------------------------------------------------------------
void SceneSetupDialog::finalize_data()
{
	set_data();

	m_SceneSetupData.m_Data.m_bFinished = true;

	//
	//	Write the Project File
	//
	ProjectSetupData& PSData = ProjectSetupMgr::Data();
	if (PSData.m_bDirty)
	{
		ProjectSetupMgr::WriteProject( itString( PSData.m_ProjectName.c_str() ) );
		PSData.m_bDirty = false;
	}

	//	prepare the initial scene directory based on project settings
	//
	itString scene_name(m_SceneSetupData.m_Data.m_SceneName.c_str());
	fsLocator project_dir;
	fsFileUtil::ANSIFilenameToLocator( m_SceneSetupData.m_Data.m_ProjectDirectory, project_dir );
	mnmPaths::SetupPaths( project_dir, scene_name );
	guiSingleDocHandler::SetInitialDirectory( gfPaths::GetPath( mnmPaths::e_SaveShots ) );
}

//--------------------------------------------------------------------
// event handling
//--------------------------------------------------------------------
void SceneSetupDialog::OnNotebookPageChanged( wxNotebookEvent& i_Event) 
{ 
	if ( i_Event.GetSelection() == e_ScenesPage )
	{
		// setup scene controls when moving to that tab page
		tabPage_scene_setup();
	}
	else if ( i_Event.GetSelection() == e_SummaryPage )
	{
		// setup summary controls when moving to that tab page
		tabPage_summary_setup();
	}

	i_Event.Skip();
}

void SceneSetupDialog::listBox_Projects_SelIndexChanged( wxCommandEvent& i_Event) 
{ 
	update_project_index();
}
void SceneSetupDialog::listBox_Scenes_SelIndexChanged( wxCommandEvent& i_Event) 
{ 
	update_scene_index();
}
void SceneSetupDialog::button_RemoveScene_Click( wxCommandEvent& i_Event) 
{ 
	int sel_index = m_listBox_Scenes->GetSelection() - 1;
	if ( sel_index >= 0 )
	{
		ProjectSetupMgr::RemoveSceneFromProject(sel_index);

		//	rebuild the scene list
		build_scene_list();
		set_scene_controls();
	}
}
void SceneSetupDialog::button_ImportSceneList_Click( wxCommandEvent& i_Event) 
{ 
	ProjectSetupMgr::ImportSceneList();

	//	rebuild the scene list
	build_scene_list();
	set_scene_controls();
}
void SceneSetupDialog::button_CreateDirs_Click( wxCommandEvent& i_Event) 
{ 
	ProjectSetupMgr::CreateDirectories();
}
void SceneSetupDialog::button_ProjectNext_Click( wxCommandEvent& i_Event) 
{ 
	tabPage_scene_setup();
	m_notebook1->ChangeSelection(e_ScenesPage);
}
void SceneSetupDialog::button_ScenePrev_Click( wxCommandEvent& i_Event) 
{ 
	m_notebook1->ChangeSelection(e_ProjectsPage);
}
void SceneSetupDialog::button_SceneNext_Click( wxCommandEvent& i_Event) 
{ 
	// setup summary controls when moving to that tab page
	tabPage_summary_setup();
	m_notebook1->ChangeSelection(e_SummaryPage);
}
void SceneSetupDialog::button_SummaryPrev_Click( wxCommandEvent& i_Event) 
{ 
	// setup scene controls when moving to that tab page
	tabPage_scene_setup();

	m_notebook1->ChangeSelection(e_ScenesPage);
}
void SceneSetupDialog::button_SummaryFinish_Click( wxCommandEvent& i_Event) 
{ 
	//	grab the information from the GUI and put it in the data
	//
	finalize_data();

	//	if this is a NEW project then ask the user to create the directories
	//
	if ( m_listBox_Projects->GetSelection() == 0 )
	{
		int retval = guiMessageBox::Show( "This is a new project do you want to create the directories for this project?","New Project Creation", guiMessageBox::e_YesNo );
		if ( retval == guiMessageBox::e_Yes )
		{
			fsLocator dir;
			fsFileUtil::ANSIFilenameToLocator(m_SceneSetupData.m_Data.m_ProjectDirectory, dir );
			mnmPaths::CreateProjectDirectories( dir );
		}
	}

	this->EndModal( wxOK );
}

	

#endif // USE_WXWIDGETS