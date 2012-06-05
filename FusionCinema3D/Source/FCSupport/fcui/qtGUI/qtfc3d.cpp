/*****************************************************************************
**	qtfc3d.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "MainApp/stdafx.h"
#include "MainApp/mainConstants.hpp"

#include "qtfc3d.h"
//#include "fcuiWidget.h"

#include "FCSupport/actn/actnTitleCardMgr.hpp"
#include "FCSupport/fcdc/data/fcdcTimelineItemData.hpp"
#include "FCSupport/fcdc/fcdcDataMgr.hpp"
#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "FCSupport/fcui/qtGUI/fcuiPanel.hpp"
#include "FCSupport/fcui/qtGUI/fcuiSplashScreen.hpp"
#include "FCSupport/path/pathDirectoryParser.hpp"

#include "Features/ObjectManip/mnpPackage.hpp"
#include "Features/Playback/plbkModePlayback.hpp"
#include "Features/RenderPanels/rpnOperations.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/pfx/pfxPostEffectObject.hpp"
#include "Support/pfx/pfxPostEffectMgr.hpp"
#include "Support/pyth/pythUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"


#include "AudioDS/sn/snSoundManager.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"

#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"

#include <QtGui/QWidget>
#include <QtGUI/QListWidget>
#include <QtGUI/QTextEdit>

#include <Phonon/MediaObject>


//============================================================================
//============================================================================
namespace
{
	const int max_movie_items = 100;
	QListWidgetItem* MovieListItems[max_movie_items] ;  
	QString l_moviename;
	int m_buttonID = 0;
	
	std::string left_icon_file("Media/GUI/left_arrow.png");
	std::string right_icon_file("Media/GUI/right_arrow.png");
	std::string mute_icon_file("Media/GUI/Mute_active.png");
	std::string unmute_icon_file("Media/GUI/Volume.png");
	
	const QSize scrollbar_icon(95,95);
	const QSize mute_icon(16,17);

	QString button_stylesheet(" QToolButton{border: 0px solid #8f8f91;border-radius: 0px;} QToolButton:pressed {color: green }	QToolButton:checked {color:green} QToolButton:hover {color:yellow} 		");
	QString listview_stylesheet(" QListWidget:Item{border: 0px solid #8f8f91;border-radius: 0px;} QListWidget::Item:selected {color: green }	QListWidget::Item:hover {color:yellow} QScrollBar:vertical { border:0px solid grey; background: #32CC99; width: 15px; margin: 20px 5 20px 5;} QScrollBar::handle:vertical { background: black; min-height: 18px;} ");
									
					

}


//------------------------------------------------------------------
// Set up the UI and then fill the background
//------------------------------------------------------------------
QtFC3D::QtFC3D(QWidget *parent)
:	QMainWindow(parent), Edit_MainWidget(NULL)
{
	for(int i = 0 ; i < max_movie_items ; i++)
		MovieListItems[i] = NULL;

	prev_button = 0;
	no_items = max_movie_items; 
	ui.setupUi(this);

	
	m_IsMuted = false;
	// This is the main widget in which we show the MSP window. 
	Edit_MainWidget = new fcuiWidget(ui.widget_3);
	Edit_MainWidget->setObjectName(QString::fromUtf8("Edit_MainWidget"));
	Edit_MainWidget->setGeometry(QRect(0,0,ui.Home_CenterWidget->width(),ui.Home_CenterWidget->height()));
	SetWinId(Edit_MainWidget->winId());

	//custom messagebox
	
	/*QDialog* shoot_dialog = new QDialog(ui.Make_Widget);
	QRadioButton* standard_button = new QRadioButton(shoot_dialog);
	standard_button->setGeometry(50,97,107,20);
	QRadioButton* stereo_button = new QRadioButton(shoot_dialog);
	stereo_button->setGeometry(160,97,107,20);*/

	ui.zAction_CameraPickerWidget = new QWidget(Edit_MainWidget);
    ui.zAction_CameraPickerWidget->setObjectName(QString::fromUtf8("zAction_CameraPickerWidget"));
    ui.zAction_CameraPickerWidget->setGeometry(QRect(140, 280, 531, 171));
	
   // ui.Home_SGPULink->setText(tr("<a style='color: green;' href='http://www.studiogpu.com'>HELP</a>"));

	QObject::connect(ui.Home_NewMovieButton, SIGNAL(clicked()), this, SLOT(ShowMovieList()));
	QObject::connect(ui.Home_ExitButton, SIGNAL(clicked()), this, SLOT(exitApp()));
	QObject::connect(ui.Home_EditMovieButton, SIGNAL(clicked()), this, SLOT(LoadProject()));
	QObject::connect(ui.Home_WatchMovieButton, SIGNAL(clicked()), this, SLOT(watchMovieButton_clicked()));
	QObject::connect(ui.Edit_ExitButton, SIGNAL(clicked()), this, SLOT(exitApp()));
	QObject::connect(ui.Edit_HomeButton, SIGNAL(clicked()), this, SLOT(PromptToSave()));
	QObject::connect(ui.Edit_SaveButton, SIGNAL(clicked()), this, SLOT(SaveButton_clicked()));
	QObject::connect(ui.Action_PlayButton, SIGNAL(clicked()), this, SLOT(Action_PlayButton_clicked()));
	QObject::connect(ui.Action_BeginButton, SIGNAL(clicked()), this, SLOT(Action_BeginButton_clicked()));
	QObject::connect(ui.Action_EndButton, SIGNAL(clicked()), this, SLOT(Action_EndButton_clicked()));
	QObject::connect(ui.Action_PauseButton, SIGNAL(clicked()), this, SLOT(Action_PauseButton_clicked()));
	QObject::connect(ui.Action_RewButton, SIGNAL(clicked()), this, SLOT(Action_RewButton_clicked()));
	QObject::connect(ui.Action_FwdButton, SIGNAL(clicked()), this, SLOT(Action_FwdButton_clicked()));
	QObject::connect(ui.Action_MuteButton, SIGNAL(clicked()), this, SLOT(Action_MuteButton_clicked()));
	//QObject::connect(ui.Theater_VideoPlayer, SIGNAL(finished()), ui.Theater_VideoPlayer, SLOT(deleteLater()));
	QObject::connect(ui.Home_ListWidget, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(editListClicked(QListWidgetItem*)));
	//QObject::connect(ui.Make_AbortButton, SIGNAL(clicked()), this, SLOT(AbortButton_clicked()));
	QObject::connect(ui.Make_CancelButton, SIGNAL(clicked()), this, SLOT(AbortButton_clicked()));
	QObject::connect(ui.Shoot_AddCountdownCB, SIGNAL(clicked()), this, SLOT(AppendCountdown_checked()));
	QObject::connect(ui.Shoot_op_stereo, SIGNAL(clicked()), this, SLOT(Stereoscope_checked()));
	QObject::connect(ui.Shoot_op_Standard, SIGNAL(clicked()), this, SLOT(Standard_checked()));
	QObject::connect(ui.shoot_size_web, SIGNAL(clicked()), this, SLOT(Web_checked()));
	QObject::connect(ui.Shoot_size_TV, SIGNAL(clicked()), this, SLOT(TV_checked()));
	QObject::connect(ui.shoot_size_HD, SIGNAL(clicked()), this, SLOT(HD_checked()));
	QObject::connect(ui.MakeMovieButton, SIGNAL(clicked()), this, SLOT(MakeButton_clicked()));
	QObject::connect(ui.Edit_MovieTitle, SIGNAL(textEdited ( const QString&)), this, SLOT(movieTitleEdited( const QString&)));
	QObject::connect(ui.Edit_TimelineSlider, SIGNAL(valueChanged(int)), this, SLOT(TimelineSliderMoved(int)));
	QObject::connect(ui.Edit_VolumeSlider, SIGNAL(valueChanged(int)), this, SLOT(VolumeSliderMoved(int)));

	//QObject::connect(ui.Home_SGPULink, SIGNAL((clicked())), this, SLOT(link_clicked()));
	Edit_MainWidget->setMouseTracking(true);
	ui.Home_ScrollRightWidget->setVisible(false);
	ui.Home_ScrollLeftWidget->setVisible(false);
	ui.widget_2->setVisible(false);
	ui.zMake_Widget->setVisible(false);
	ui.Action_TimelineWidgetOverlay->setVisible(false);
	ui.Home_MovieClipBGWidget->setVisible(false);

	ui.Edit_ActionScrollLeftButton->setIconSize(scrollbar_icon);
	ui.Edit_ActionScrollLeftButton->setIcon(QIcon(left_icon_file.c_str()));
	ui.Edit_ActionScrollRightButton->setIconSize(scrollbar_icon);
	ui.Edit_ActionScrollRightButton->setIcon(QIcon(right_icon_file.c_str()));

	ui.zAction_CameraPickerWidget->setVisible(false);
	ui.Home_NewMovieButton->setStyleSheet(button_stylesheet);
	ui.Home_EditMovieButton->setStyleSheet(button_stylesheet);
	ui.Home_WatchMovieButton->setStyleSheet(button_stylesheet);

	ui.Edit_HomeButton->setStyleSheet(button_stylesheet);
	ui.Edit_SaveButton->setStyleSheet(button_stylesheet);
	ui.Edit_ExitButton->setStyleSheet(button_stylesheet);
	ui.Home_ExitButton->setStyleSheet(button_stylesheet);

	fsLocator fs_countdownmovie_path = gfPaths::GetPath(mnmPaths::e_Data);
	fs_countdownmovie_path.Push("countdown_tv.avi");
	fcuiUtils::SetCountdownMoviepath(fs_countdownmovie_path);

	std::string VersionNumber = mainConstants::mc_ExecutableVersion;
	ui.Home_VersionNumberLabel->setText(VersionNumber.c_str());

	m_bDoVolume = true;
	ui.Edit_VolumeSlider->setSingleStep(5);
	ui.Edit_VolumeSlider->setPageStep(20);
	ui.Edit_VolumeSlider->setValue(50);
	float volume = (float)ui.Edit_VolumeSlider->value() / (float)ui.Edit_VolumeSlider->maximum();
	snSoundManager::SetSoundTypeVolumeGlobalFactor(volume);
	snSoundManager::Pause();
	m_VolumeLevel = ui.Edit_VolumeSlider->value();

	plbkModePlayback pMode = plbkModePlayback();
	pMode.set_app_fusion(true);

	ui.Make_ProgressBarWidget->setVisible(false);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
QtFC3D::~QtFC3D()
{
	delete Edit_MainWidget;
	Edit_MainWidget = NULL;
	
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool QtFC3D::event(QEvent* event)
{
	if ((event->type() == QEvent::HoverMove) && (event->type() != QEvent::HoverLeave))		// Get only the Hover event
		
	{
		QBrush white_brush(QColor(255, 255, 255, 255));
		QBrush hover_brush(QColor(127,127,127,255));

		QPalette palette_hover, palette_white;
		palette_hover.setBrush(QPalette::Active, QPalette::ButtonText, hover_brush);
		palette_white.setBrush(QPalette::Active, QPalette::ButtonText, white_brush);
			
		if (Edit_MainWidget->underMouse())	// Make sure its tied inside the Main Widget
		{
			// Take Action
			tma3dCursorMgr::CursorPosChangedCallback( Edit_MainWidget->mapFromGlobal(QCursor::pos()).x(), Edit_MainWidget->mapFromGlobal(QCursor::pos()).y() );

			//DBG_TRACE("The x position of the mouse "<<Edit_MainWidget->mapFromGlobal(QCursor::pos()).x());
		}
		return true;
	}

	return QWidget::event(event);
}

//------------------------------------------------------------------
// Get the HWND for the window that acts as the MSP window
//------------------------------------------------------------------
HWND QtFC3D::GetWinId()
{
	return winid;
}

//------------------------------------------------------------------
// Set the HWND for the window that acts as the MSP window
//------------------------------------------------------------------
void QtFC3D::SetWinId(HWND i_winId)
{
	winid = i_winId; 
}

//------------------------------------------------------------------
// Shows the movie packs when new Movie is clicked
//------------------------------------------------------------------
void QtFC3D::ShowMovieList()
{
	//	Set the movie pack directory
	prev_button = new_movie;
	ui.Home_ScrollRightWidget->setVisible(true);
	ui.Home_ScrollLeftWidget->setVisible(true);
	ui.Home_MovieClipBGWidget->setVisible(true);

	fcuiFormMgr::HideWatchPanel();
	if (fcmdModeMgr::Instance != NULL)
	{
		fcmdModeMgr::Instance->ActivateMode(fcmdModeMgr::e_NewMovie);
		fcmdModeMgr::Instance->ProcessNewMovieRequest();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::LoadMoviePack(const itString& i_MoviePackName)
{
	load_moviepack(i_MoviePackName);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void QtFC3D::closeEvent(QCloseEvent* i_Event)
{
	fcuiUtils::ExitApplication();
	i_Event->accept();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void QtFC3D::keyPressEvent(QKeyEvent* keyevent)
{
	if ( keyevent == NULL  )
		return;

	switch (keyevent->key())
	{
		case Qt::Key_Up:
		{
			DBG_TRACE("Key-Up");
			break;
		}
		case Qt::Key_Down:
		{
			DBG_TRACE("Key-Down");
			break;
		}
		case Qt::Key_Right:
		{
			DBG_TRACE("Key-Right");
			if (fcuiTimelineMgr::Instance != NULL)
				fcuiTimelineMgr::Instance->IncrementTime(1.0f);
			break;
		}
		case Qt::Key_Left:
		{
			DBG_TRACE("Key-Left");
			if (fcuiTimelineMgr::Instance != NULL)
				fcuiTimelineMgr::Instance->DecrementTime(1.0f);
			break;
		}
		case Qt::Key_Home:
		{
			DBG_TRACE("Key-Home");
			keyevent->accept();
			break;
		}
		case Qt::Key_End:
		{
			DBG_TRACE("Key-End");
			break;
		}
		case Qt::Key_Space:
		{
			DBG_TRACE("Key-Space");
			if (fcuiTimelineMgr::Instance != NULL)
				fcuiTimelineMgr::Instance->Pause();
			break;
		}
		default:
		{
			QMainWindow::keyPressEvent(keyevent);
			break;
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::DancingBrideButton()
{
	load_moviepack( "Dancer" );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::CubeSphereButton()
{
	load_moviepack( "Cube+Sphere" );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::exitApp()
{
	QMessageBox msgBox;
	msgBox.setText("The Project has been modified.");
	msgBox.setInformativeText("Do you want to save your changes?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	int ret = msgBox.exec();

	switch(ret)
	{
		case QMessageBox::Save:
			SaveButton_clicked();
		case QMessageBox::Discard:
			fcuiUtils::ExitApplication();
			break;
		case QMessageBox::Cancel:
			break;
		default:
			break;
	}
	
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::HomeButton()
{
	ui.stackedWidget->setCurrentIndex(0);
}


//----------------------------------------------------------------------------
// Prompt to save the project.
//----------------------------------------------------------------------------
void QtFC3D::PromptToSave()
{
	QMessageBox msgBox;
	msgBox.setWindowTitle("Save Project");
	msgBox.setText("The Project has been modified.");
	msgBox.setInformativeText("Do you want to save your changes?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	int ret = msgBox.exec();

	switch(ret)
	{
		case QMessageBox::Save:
			SaveButton_clicked();
		case QMessageBox::Discard:
			HomeButton();
			break;
		case QMessageBox::Cancel:
			break;
		default:
			break;
	}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::link_clicked()
{
	QDesktopServices::openUrl(QUrl("www.studiogpu.com", QUrl::TolerantMode));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::SaveButton_clicked()
{
	fcuiUtils::UserSaveProject( this );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::LoadProject()
{
	fcuiFormMgr::HideMoviePacks();
	int current_button = edit_movie;
	fsLocator l_MoviePackDirectory = gfPaths::GetPath( mnmPaths::e_UserProjects );

	ui.Home_ListLabel->setText("Edit List");
	if (current_button != prev_button)
		fcuiFormMgr::ShowWatchPanel(l_MoviePackDirectory);
	prev_button = current_button;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::Sort_by_time(std::vector<QString>& files_qstr)
{
	for (int j = 2; j < files_qstr.size(); j++) 
	{
		for (int k = 0; k < j; k++) 
		{
			QFileInfo element1(files_qstr[j]);
			QFileInfo element2(files_qstr[k]);
			if (element1.lastModified() < element2.lastModified()) 
			{
				QString temp = files_qstr[k];
				files_qstr[k] = files_qstr[j];
				files_qstr[j] = temp;
			}
		}
	}
}

//----------------------------------------------------------------------------
// Edit the List View Widget
//----------------------------------------------------------------------------
void QtFC3D::EditList(const fsLocator& i_DirectoryName)
{
	std::vector<fsLocator> files;
	pathDirectoryParser path;
	path.GetDirectoryFiles(i_DirectoryName, files);
	std::vector<std::string> files_str;
	std::vector<QString> files_qstr;
	for(int i = 0; i < files.size(); i++)
	{
		std::string temp_string;
		fsFileUtil::LocatorToANSIFilename(files[i],temp_string);
		files_str.push_back(temp_string);
		files_qstr.push_back(QString(files_str[i].c_str()));
	}
	files.clear();

	Sort_by_time(files_qstr);
	for(int i = 0 ; i < no_items ; i ++)
	{
		if (MovieListItems[i] != NULL)
			delete MovieListItems[i];
		MovieListItems[i] = NULL;
	}

	ui.Home_ListWidget->clear();

	itString itstr;
	for(int i = 0; i < files_str.size(); i++)
	{
		
		itstr = ((const itString::CharType*)files_qstr[i].data());
		fsLocator temp_locator;
		fsFileUtil::UnicodeStringToLocator(itstr, temp_locator);
		files.push_back(temp_locator);
	}

	for(int i = files.size()-1 ; i >=0 ; i--)
	{
		itString it_filename = files[i].GetLastName();
		itString it_filenamewext = it_filename;
	//	it_filename.StripExtension();
		std::string filename = itStringUtil::GetStdString(it_filename);
		std::string filenamewext = itStringUtil::GetStdString(it_filenamewext);
		MovieListItems[i] = new QListWidgetItem(filename.c_str(), ui.Home_ListWidget);
		//ui.Home_ListWidget->setStyleSheet(listview_stylesheet);
		//MovieListItems[i]->setText(	filename.c_str());
		ui.Home_ListWidget->insertItem(i,MovieListItems[i]);
	}
	no_items = files.size();
	
	//fcuiUtils::UserLoadProject( this );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::MakeButton_clicked()
{
	if(ui.MakeMovieButton->text() == QString("Shoot"))
	{
		ui.MakeMovieButton->setText("Cancel");
		ui.Make_ProgressBarWidget->setVisible(true);
		fcuiUtils::MakeProject();
	}
	else if(ui.MakeMovieButton->text() == QString("Cancel"))
	{
		ui.MakeMovieButton->setText("Shoot");
		ui.Make_ProgressBarWidget->setVisible(false);
		ui.Edit_TitleCardWidget->show();
		AbortButton_clicked();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::PlayButton_clicked()
{
	fcuiUtils::PlayMovie( this );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::Action_BeginButton_clicked()
{

	// TODO: playback control - Pause
	if (fcuiTimelineMgr::Instance != NULL)
	{
		tmlnTimeLine::SetValue(maTime::c_ZeroTime);
		fcuiFormMgr::UpdateTimelineSelection( tmlnTimeLine::GetValue() );
		fcuiFormMgr::UpdateCurrentMovieTime( true );
	}
	if( fcmdModeMgr::Instance != NULL)
		fcmdModeMgr::Instance->SetTimelineLocked(false);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::Action_EndButton_clicked()
{
	// TODO: playback control - Pause
	if (fcuiTimelineMgr::Instance != NULL)
	{
		//TIME - why is there an epsilon here? What is the meaning of 0.001f? Is it needed with maTime?
		//tmlnTimeLine::SetValue( fcuiTimelineMgr::Instance->GetEndTime() - 0.001f);
		tmlnTimeLine::SetValue( fcuiTimelineMgr::Instance->GetEndTime() - maTime::FromFrame(1, 1000) );
		fcuiFormMgr::UpdateTimelineSelection( tmlnTimeLine::GetValue() );
		fcuiFormMgr::UpdateCurrentMovieTime( true );
	}
	if( fcmdModeMgr::Instance != NULL)
		fcmdModeMgr::Instance->SetTimelineLocked(false);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::Action_PlayButton_clicked()
{
	if((fcuiTimelineMgr::Instance != NULL) && (fcuiTimelineMgr::Instance->GetEndTime() > maTime::c_ZeroTime))
	{
		std::string camname = fcuiConstants::c_CAMERA_DIRECTOR;
		nameString camName(camname);
		int index = camsCameraMgr::GetIndexForName(camName);
		if(index >= 0 && index < camsCameraMgr::GetNumCameras())
		{
			rpnOperations::ChangeCamera(camsCameraMgr::GetCameraProxy(index), camName.GetString());
			camsCameraMgr::SelectCamera(index);
		}
		else
		{
			DBG_WARNING("Camera: " << camName << " was not found.");
		}
	}
	// TODO: playback control - Play
	if (fcuiTimelineMgr::Instance != NULL)
	{
		fcuiTimelineMgr::Instance->Play();
		snSoundManager::Resume(tmlnTimeLine::GetTimeInSeconds());  //make sure we start our sounds at the appropriate time
	}
	if( fcmdModeMgr::Instance != NULL)
		fcmdModeMgr::Instance->SetTimelineLocked(true);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::Action_PauseButton_clicked()
{
	// TODO: playback control - Pause
	if (fcuiTimelineMgr::Instance != NULL)
	{
		fcuiTimelineMgr::Instance->Pause();
		snSoundManager::Pause();
	}
	if( fcmdModeMgr::Instance != NULL)
		fcmdModeMgr::Instance->SetTimelineLocked(false);
		
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::Action_RewButton_clicked()
{
	// TODO: playback control - Rewind
	/*if (fcuiTimelineMgr::Instance != NULL)
		fcuiTimelineMgr::Instance->DecrementTime(1.0f);*/
	if (fcuiTimelineMgr::Instance != NULL)
		fcuiTimelineMgr::Instance->Rewind();
	if( fcmdModeMgr::Instance != NULL)
		fcmdModeMgr::Instance->SetTimelineLocked(true);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::Action_FwdButton_clicked()
{
	// TODO: playback control - Forward
	/*if (fcuiTimelineMgr::Instance != NULL)
		fcuiTimelineMgr::Instance->IncrementTime(1.0f);*/
	if (fcuiTimelineMgr::Instance != NULL)
		fcuiTimelineMgr::Instance->FastFwd();
	if( fcmdModeMgr::Instance != NULL)
		fcmdModeMgr::Instance->SetTimelineLocked(true);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::TimelineSliderMoved(int i_SliderVal)
{
	fcuiFormMgr::TimeSliderMoved(i_SliderVal);
}

//----------------------------------------------------------------------------
/// Mute or unmute sound
//----------------------------------------------------------------------------
void QtFC3D::DoMute( bool i_bMute )
{
	m_IsMuted = i_bMute;
	snSoundManager::Mute(i_bMute);

	//show the appropriate icon
	ui.Action_MuteButton->setIconSize(mute_icon);
	if( i_bMute )
		ui.Action_MuteButton->setIcon(QIcon(mute_icon_file.c_str()));
	else
		ui.Action_MuteButton->setIcon(QIcon(unmute_icon_file.c_str()));
	
	ui.Action_MuteButton->show();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::VolumeSliderMoved(int i_SliderVal)
{
	// this function is executed when the button needs to mute/unmute
	// so we need to make sure that the rest of the slider operations don't
	// execute on a mute button press
	if(!m_bDoVolume)
	{
		m_bDoVolume = true;
		return;
	}

	//Check which operation should be performed when the volume is hit
	if( i_SliderVal == 0 ) //mute
	{
		DoMute(true);
	}
	else
	{
		//if muted and we are making a increasing volume, remove the mute
		if( m_IsMuted )
			DoMute(false);		
		
		float volume = (float)ui.Edit_VolumeSlider->value() / (float)ui.Edit_VolumeSlider->maximum();
		snSoundManager::SetSoundTypeVolumeGlobalFactor(volume);
		m_VolumeLevel = i_SliderVal;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::Action_MuteButton_clicked()
{
	m_bDoVolume = false;
	
	//set the slider position based on the mute value
	if(m_IsMuted)
		ui.Edit_VolumeSlider->setSliderPosition(m_VolumeLevel);
	else
		ui.Edit_VolumeSlider->setSliderPosition(0);
	
	//adjust the mute button icon
	DoMute(!m_IsMuted);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::load_moviepack(const char* i_MoviePackName)
{
	load_moviepack(itString(i_MoviePackName));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::load_moviepack(const itString& i_MoviePackName)
{
	
	if ( fcuiTimelineMgr::Instance != NULL )
	{
		fcuiTimelineMgr::Instance->SetEndTime(maTime::c_ZeroTime);
	}
	
	ui.Action_CurrentTimeLabel->setText("00:00:00");
	ui.Action_EndTimeLabel->setText("00:00:00");
	ui.Edit_TimelineSlider->setValue(0);

	// Turn off PostFX
	pfxPostEffectObject* obj = dynamic_cast<pfxPostEffectObject*>(pfxPostEffectMgr::GetDataObject(pfxPostEffectMgr::e_ViewportPfx));
	pfxData& data= obj->GetData();
	data.m_bActive.SetValue(false);

	if (fcmdModeMgr::Instance != NULL)
	{
		fcmdModeMgr::Instance->SetMoviePackDirectory(i_MoviePackName);
	}

#ifndef _DEBUG
	//	Show the splash screen
	fsLocator l_LoadScreenPath = gfPaths::GetPath( mnmPaths::e_CurrentMoviePack );
	l_LoadScreenPath.Push("LoadMoviePack.png");
	std::string l_moviepacklogo;
	fsFileUtil::LocatorToANSIFilename(l_LoadScreenPath, l_moviepacklogo);
	QPixmap pixmap(l_moviepacklogo.c_str());
	fcuiSplashScreen* pSplash = new fcuiSplashScreen(pixmap, Qt::WindowStaysOnTopHint);
	pSplash->ShowScreen();
#endif

	//	Load the scene
	fsLocator l_CastDirectory = gfPaths::GetPath( mnmPaths::e_CurrentMoviePack );
	l_CastDirectory.Push( fcuiConstants::c_MEDIA );
	fsLocator scene( l_CastDirectory );
	scene.Push( fcuiConstants::c_MODE_LOCATION );
	scene.Push( fcuiConstants::c_FILE_MOVIEPACK_SCENE );

	fcuiFormMgr::ClearTimeline();
	fcuiUtils::LaunchMovie(scene);
	fcuiUtils::UserLoadProject();

	fcuiFormMgr::HighlightItems();
	fcdcTimelineItemData temp = fcdcDataMgr::GetData();
	temp.m_ProjectData.m_MoviePackName.SetValue(itStringUtil::GetStdString(i_MoviePackName));
	temp.m_ProjectData.m_Title.SetValue("My Movie");
	fcdcDataMgr::SetData(temp);

	//	Parse the directories
	std::vector<fsLocator> directories;
	pathDirectoryParser parser;
	parser.GetSubDirectories(l_CastDirectory, directories);

	//	create the labels
	fcuiFormMgr::CreateLabels(ui.Edit_ModeTextWidget, l_CastDirectory, 0);

#ifndef _DEBUG
	//	Hide the splash screen
	pSplash->HideScreen(this);
#endif

	fcuiFormMgr::HideMoviePacks();
	ui.stackedWidget->setCurrentIndex(1);

	//	Set the movie pack directory
	if (fcmdModeMgr::Instance != NULL)
	{
		fcmdModeMgr::Instance->InitCastAnims();
		fcmdModeMgr::Instance->ProcessModeEvent(fcmdModeMgr::e_Mode, 
												fcmdModeMgr::GetModeLocator(fcmdModeMgr::e_Cast), 
												fcmdModeMgr::e_Cast);
	}
	if (actnTitleCardMgr::Instance != NULL)
	{
		actnTitleCardMgr::Instance->CreateTitleCard();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::watchMovieButton_clicked()
{
	int current_button = watch_movie;
	
	fcuiFormMgr::HideMoviePacks();
	fsLocator l_MoviesDirectory = gfPaths::GetPath( mnmPaths::e_UserMovies );
	ui.Home_ListLabel->setText("Play List");
	if (prev_button != current_button)
		fcuiFormMgr::ShowWatchPanel(l_MoviesDirectory);
	prev_button = current_button;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::SelectMovieButton_clicked()
{
//	fcuiUtils::UserLoadProject(this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::MakeMovieButton_clicked()
{
	if(ui.MakeMovieButton->text() == QString("Shoot"))
	{
		ui.MakeMovieButton->setText("Cancel");
		ui.Make_ProgressBarWidget->setVisible(true);
		fcuiUtils::MakeProject();
	}
	else if(ui.MakeMovieButton->text() == QString("Cancel"))
	{
		ui.MakeMovieButton->setText("Shoot");
		ui.Make_ProgressBarWidget->setVisible(false);
		AbortButton_clicked();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::AbortButton_clicked()
{
	
	fcuiUtils::AbortMake();
	fcmdModeMgr::Instance->ProcessModeEvent(fcmdModeMgr::e_Mode, 
												fcmdModeMgr::GetModeLocator(fcmdModeMgr::e_Action), 
												fcmdModeMgr::e_Action);
	
	ui.zMake_Widget->hide();
	//ui.Make_ProgressBarWidget->setVisible(false);
	ui.MakeMovieButton->setText("Shoot");
	QApplication::restoreOverrideCursor();
	ui.page->show();
	QCoreApplication::processEvents();
	//QApplication::beep();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::AppendCountdown_checked()
{
	if(ui.Shoot_AddCountdownCB->isChecked())
		fcuiUtils::SetChecked(true);
	else 
		fcuiUtils::SetChecked(false);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::Stereoscope_checked()
{
	int num_cam = cmraObjectMgr::GetNumObjects();
	for(int i = 0 ; i < num_cam ; i++)
	{
		cmraScriptObject* pSO = cmraObjectMgr::GetObject(i);
		if (pSO != NULL)
		{
			cmraCameraObject* pCO = pSO->GetPickObject();
			if (pCO != NULL)
			{
				cmraCameraData cdata = pCO->GetData();
				cdata.m_StereoType.SetValue(anaglyph);
				pCO->SetData(cdata);
			}
		}
	}
	ui.zMake_Widget->show();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::Standard_checked()
{
	int num_cam = cmraObjectMgr::GetNumObjects();
	for(int i = 0 ; i < num_cam ; i++)
	{
		cmraScriptObject* pSO = cmraObjectMgr::GetObject(i);
		if (pSO != NULL)
		{
			cmraCameraObject* pCO = pSO->GetPickObject();
			if (pCO != NULL)
			{
				cmraCameraData cdata = pCO->GetData();
				cdata.m_StereoType.SetValue(standard);
				pCO->SetData(cdata);
			}
		}
	}
	ui.zMake_Widget->show();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::Web_checked()
{
	captRenderOutputData& m_OutputData = captRenderOutputDataUtil::Data();
	//m_OutputData.m_CompressCode.SetValueWithoutNotify("CVID");
	m_OutputData.m_Resolution.SetValue( "Custom Resolution" );
	m_OutputData.m_nWidth.SetValue(400);
	m_OutputData.m_nHeight.SetValue(225);
	fsLocator fs_countdownmovie_path = gfPaths::GetPath(mnmPaths::e_Data);
	fs_countdownmovie_path.Push("countdown_web.avi");
	fcuiUtils::SetCountdownMoviepath(fs_countdownmovie_path);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::TV_checked()
{
	captRenderOutputData& m_OutputData = captRenderOutputDataUtil::Data();
	//m_OutputData.m_CompressCode.SetValueWithoutNotify("CVID");
	m_OutputData.m_Resolution.SetValue( "640x360" );
	fsLocator fs_countdownmovie_path = gfPaths::GetPath(mnmPaths::e_Data);
	fs_countdownmovie_path.Push("countdown_tv.avi");
	fcuiUtils::SetCountdownMoviepath(fs_countdownmovie_path);
	
	
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::HD_checked()
{
	captRenderOutputData& m_OutputData = captRenderOutputDataUtil::Data();
	//m_OutputData.m_CompressCode.SetValueWithoutNotify("CVID");
	m_OutputData.m_Resolution.SetValue( "1280x720" );
	fsLocator fs_countdownmovie_path = gfPaths::GetPath(mnmPaths::e_Data);
	fs_countdownmovie_path.Push("countdown_HD.avi");
	fcuiUtils::SetCountdownMoviepath(fs_countdownmovie_path);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::movieTitleEdited(const QString& title)
{
	fsLocator fs_file;
	itString itstr;
	itstr = ((const itString::CharType*)title.data());	
	fsFileUtil::UnicodeStringToLocator(itstr, fs_file);
	fcuiUtils::SetMovieTitle(fs_file);
	fcdcTimelineItemData temp = fcdcDataMgr::GetData();
	temp.m_ProjectData.m_Title.SetValue(itStringUtil::GetStdString(itstr));

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void QtFC3D::editListClicked(QListWidgetItem* item)
{
	fsLocator movies_directory = gfPaths::GetPath( mnmPaths::e_UserMovies );
	fsLocator project_path = gfPaths::GetPath( mnmPaths::e_UserProjects );
	fsLocator fs_file;

	itString itstr;
	QString ItemString = item->text();
	itstr = ((const itString::CharType*)ItemString.data());
	fsFileUtil::UnicodeStringToLocator(itstr, fs_file);

	itString extension;
	fs_file.GetLastName().GetExtension(extension);
	
	
	// If its a movie, go to theater 
	if (extension == fcuiConstants::l_EXT_AVI)
	{
		movies_directory.Push(fs_file);
		if (fsFileUtil::FileExists(movies_directory))
			fcuiUtils::SetMovieName(movies_directory);
		std::string file_path("start wmplayer.exe "), str_file; // Call windows media player
		fsFileUtil::LocatorToANSIFilename(movies_directory, str_file); // Pass the file path 
		file_path += ("\"") + str_file + ("\"");
		int ret_val = system(file_path.c_str());
	}

	// if its a project, load it
	if (extension == fcuiConstants::l_EXT_MAB)
	{
		project_path.Push(fs_file);
		if (fsFileUtil::FileExists(project_path))
		{
			QPixmap pixmap("Media/GUI/LoadMoviePack.png");
			fcuiSplashScreen* splash = new fcuiSplashScreen(pixmap, Qt::WindowStaysOnTopHint);
			splash->ShowScreen();

			docSingleDocumentMgr::LoadDocument( project_path );

			fcdcTimelineItemData temp = fcdcDataMgr::GetData();
			movieTitleEdited(temp.m_ProjectData.m_Title.GetValue().c_str());
			ui.Edit_MovieTitle->setText(temp.m_ProjectData.m_Title.GetValue().c_str());
			fcuiFormMgr::HighlightItems();

			itString i_moviepackname = itString(temp.m_ProjectData.m_MoviePackName.GetValue().c_str());
			if (fcmdModeMgr::Instance != NULL)
			{
				fcmdModeMgr::Instance->SetMoviePackDirectory(i_moviepackname);
				fcmdModeMgr::Instance->ProcessModeEvent(fcmdModeMgr::e_Mode, 
													fcmdModeMgr::GetModeLocator(fcmdModeMgr::e_Action), 
													fcmdModeMgr::e_Action);
			}
			fcuiFormMgr::ClearTimeline();
			fcuiUtils::UserLoadProject();
			fcuiFormMgr::UpdateTimelineItems();
			fcuiFormMgr::UpdateMovieEndTime();
			if((fcuiTimelineMgr::Instance != NULL) && (fcuiTimelineMgr::Instance->GetEndTime() > maTime::c_ZeroTime))
			{

				std::string camname = fcuiConstants::c_CAMERA_DIRECTOR;
				nameString camName(camname);
				int index = camsCameraMgr::GetIndexForName(camName);
				if(index >= 0 && index < camsCameraMgr::GetNumCameras())
				{
					rpnOperations::ChangeCamera(camsCameraMgr::GetCameraProxy(index), camName.GetString());
					camsCameraMgr::SelectCamera(index);
				}
				else
				{
					DBG_WARNING("Camera: " << camName << " was not found.");
				}
			}
				
			fsLocator l_CastDirectory = gfPaths::GetPath( mnmPaths::e_CurrentMoviePack );
			l_CastDirectory.Push( fcuiConstants::c_MEDIA );
			std::vector<fsLocator> directories;
			pathDirectoryParser parser;
			parser.GetSubDirectories(l_CastDirectory, directories);

			//	create the labels
			fcuiFormMgr::CreateLabels(ui.Edit_ModeTextWidget, l_CastDirectory, 0);
			ui.Edit_MovieTitle->setText(temp.m_ProjectData.m_Title.GetValue().c_str());
			splash->HideScreen(this);

			ui.stackedWidget->setCurrentIndex(1);
	
		}
	}
}
