/*****************************************************************************
**	qtfc3d.hpp
**
**		custom code for main interface
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef QTFC3D_H
#define QTFC3D_H

#include <QCloseEvent>
#include <QKeyEvent>
#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QFileInfo> 
#include <QDateTime>
#include "GeneratedFiles/ui_qtfc3d.h"
#include "fcuiWidget.h"


//============================================================================
//	Forward References
//============================================================================
class itString;
class fsLocator;


//============================================================================
//============================================================================
class QtFC3D : public QMainWindow
{
	Q_OBJECT

public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	QtFC3D(QWidget *parent = 0);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~QtFC3D();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	enum Buttons
	{
		new_movie = 0,
		watch_movie = 1,
		edit_movie = 2
	};

	enum Output_type
	{
		standard = 0,
		anaglyph = 1
	};
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	HWND GetWinId();
	void closeEvent(QCloseEvent*);
	void keyPressEvent(QKeyEvent* keyevent);
	bool event(QEvent *event);
	void SetWinId(HWND i_winId);
	void LoadMoviePack(const itString& i_MovieName);
	void EditList(const fsLocator& i_DirectoryName);
	void Sort_by_time(std::vector<QString>& files_qstr);
		
public:
	HWND winid;
	Ui::QtFC3DClass ui;
	int prev_button, no_items;

private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	public slots:
		void ShowMovieList();
		//void ShowEditPage();
		void DancingBrideButton();
		void CubeSphereButton();
		void exitApp();
		void HomeButton();
		void PromptToSave();
		void SaveButton_clicked();
		void LoadProject();
		void MakeButton_clicked();
		void PlayButton_clicked();
		void Action_BeginButton_clicked();
		void Action_EndButton_clicked();
		void Action_PlayButton_clicked();
		void Action_PauseButton_clicked();
		void Action_RewButton_clicked();
		void Action_FwdButton_clicked();
		void watchMovieButton_clicked();
		void SelectMovieButton_clicked();
		void MakeMovieButton_clicked();
		void editListClicked(QListWidgetItem* item);
		void Action_MuteButton_clicked();
		void AbortButton_clicked();
		void link_clicked();
		void AppendCountdown_checked();
		void Stereoscope_checked();
		void Standard_checked();
		void Web_checked();
		void TV_checked();
		void HD_checked();
		void movieTitleEdited(const QString&);
		void TimelineSliderMoved(int);
		void VolumeSliderMoved(int);

private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void load_moviepack(const char* i_MoviePackName);
	void load_moviepack(const itString& i_MoviePackName);
	void DoMute( bool i_bMute );

	fcuiWidget* Edit_MainWidget;
	bool m_IsMuted;
	int m_VolumeLevel;
	bool m_bDoVolume;
	//QWidget* Action_CameraPickerWidget;
	
};

#endif // QTFC3D_H
