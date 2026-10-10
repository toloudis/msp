/*****************************************************************************
**	SceneSetupDialog.hpp
**
**	Project and scene setup wizard, written in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef SCENESETUPDIALOG_HPP
#error SceneSetupDialog.hpp multiply included
#endif
#define SCENESETUPDIALOG_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

struct SceneSetupData;
class QCheckBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QTabWidget;
class tqcDirPicker;

//----------------------------------------------------------------------------
// Class SceneSetupDialog
//----------------------------------------------------------------------------
class SceneSetupDialog : public QDialog
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	SceneSetupDialog( QWidget* parent,
					  SceneSetupData& i_Data);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~SceneSetupDialog();

private:
	//--------------------------------------------------------------------
	// private functions
	//--------------------------------------------------------------------
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
	void OnNotebookPageChanged( int i_Page );
	void button_RemoveScene_Click();
	void button_ImportSceneList_Click();
	void button_SummaryFinish_Click();

	//--------------------------------------------------------------------
	// Change the tab page without the page change handling
	//--------------------------------------------------------------------
	void change_page( int i_Page );

	QTabWidget* m_notebook1;
	QListWidget* m_listBox_Projects;
	QLineEdit* m_textCtrl_ProjectName;
	QLineEdit* m_textCtrl_ProjectDescription;
	tqcDirPicker* m_dirPicker_ProjectDir;
	QListWidget* m_listBox_Scenes;
	QLineEdit* m_textCtrl_SceneName;
	QLineEdit* m_textCtrl_SceneDescription;
	QLabel* m_staticText_SummaryProject;
	QLabel* m_staticText_SummaryDirectory;
	QLabel* m_staticText_SummaryScene;
	QCheckBox* m_checkBox_SetDefault;

	SceneSetupData& m_SceneSetupData;
	bool m_bChangingPage;
};

#endif // USE_QT
