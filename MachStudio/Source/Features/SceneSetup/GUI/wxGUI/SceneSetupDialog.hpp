/*****************************************************************************
**	SceneSetupDialog.hpp
**
**	New Scene Wizard dialog in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef SCENESETUPDIALOG_HPP
#error SceneSetupDialog.hpp multiply included
#endif
#define SCENESETUPDIALOG_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "Features/SceneSetup/GUI/wxGUI/SceneSetupDialogBase.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
struct SceneSetupData;

//----------------------------------------------------------------------------
// Class SceneSetupDialog
//----------------------------------------------------------------------------
class SceneSetupDialog : public SceneSetupDialogBase
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	SceneSetupDialog( wxWindow* parent,
					  SceneSetupData& i_Data);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~SceneSetupDialog();

private:
	//------------------------------------------------------------------------
	// private functions
	//------------------------------------------------------------------------
	void build_project_list();
	void set_project_controls();
	void set_scene_setup_data();
	void update_project_index();
	void build_scene_list();
	void tabPage_scene_setup();
	void set_scene_controls();
	void update_scene_index();
	void set_project_data();
	void set_scene_data();
	void set_data();
	void tabPage_summary_setup();
	void finalize_data();

	//--------------------------------------------------------------------
	// event handling
	//--------------------------------------------------------------------
	virtual void OnNotebookPageChanged( wxNotebookEvent& i_Event);
	virtual void listBox_Projects_SelIndexChanged( wxCommandEvent& i_Event);
	virtual void listBox_Scenes_SelIndexChanged( wxCommandEvent& i_Event);
	virtual void button_RemoveScene_Click( wxCommandEvent& i_Event);
	virtual void button_ImportSceneList_Click( wxCommandEvent& i_Event);
	virtual void button_CreateDirs_Click( wxCommandEvent& i_Event);
	virtual void button_ProjectNext_Click( wxCommandEvent& i_Event);
	virtual void button_ScenePrev_Click( wxCommandEvent& i_Event);
	virtual void button_SceneNext_Click( wxCommandEvent& i_Event);
	virtual void button_SummaryPrev_Click( wxCommandEvent& i_Event);
	virtual void button_SummaryFinish_Click( wxCommandEvent& i_Event);

	SceneSetupData& m_SceneSetupData;
};

#endif // USE_WXWIDGETS
