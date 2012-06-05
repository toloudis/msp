/*****************************************************************************
**	fcmdModeMake.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcmd/fcmdModeMake.hpp"

#include "FCSupport/anim/qtGUI/animPixmapItem.hpp"
#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"
#include "FCSupport/fcui/qtGUI/fcuiLabel.hpp"
#include "FCSupport/path/pathDirectoryParser.hpp"
#include "Features/Capture/cptrWriteAVI.hpp"
#include "Features/Capture/cptrPackage.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Support/capt/captRenderProgressMgr.hpp"
#include "Support/capt/captRenderProgressData.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeInOutMgr.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"

#include <vector>


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------
fcmdModeMake::fcmdModeMake()
:	fcmdModeTemplate(fsLocator()),
	m_bRenderStarted(false),
	m_bContinueRender(false),
	m_value(0)
{
	DBG_TRACE("Created Make Mode");
}
fcmdModeMake::fcmdModeMake(const fsLocator& i_Directory)
:	fcmdModeTemplate(i_Directory),
	m_bRenderStarted(false),
	m_bContinueRender(false),
	m_value(0)
{
	DBG_TRACE("Created Make Mode");
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
fcmdModeMake::~fcmdModeMake()
{}

//----------------------------------------------------------------------------
// Activate the Make mode and any other systems associated with it
//----------------------------------------------------------------------------
void fcmdModeMake::Activate()
{
	DBG_TRACE("Make Mode Activated");
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	i_ui->ui.Action_TimelineWidgetOverlay->setVisible(false);
	i_ui->ui.MakeMovieButton->setText("Shoot");
	i_ui->ui.Action_ProgressBar->setValue(0);
	i_ui->ui.Shoot_AddCountdownCB->setChecked(false);
	m_bContinueRender = false;
	if (docSingleDocumentMgr::NeedsSave())
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle("Project Changed");
		msgBox.setText("The Project has been modified.");
		msgBox.setInformativeText("Do you want to save your changes?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Yes);
		int ret = msgBox.exec();

		switch(ret)
		{
			case QMessageBox::Yes:
				i_ui->SaveButton_clicked();
			case QMessageBox::No: 
				i_ui->ui.zMake_Widget->setVisible(true);
				break;
			case QMessageBox::Cancel:
				{
				fcmdModeMgr::Instance->ProcessModeEvent(fcmdModeMgr::e_Mode, 
													fcmdModeMgr::GetModeLocator(fcmdModeMgr::e_Action), 
													fcmdModeMgr::e_Action);
				}
				break;
			default:
				break;
		}
	}

	m_bRenderStarted = false;
	m_bStopRender = false;
}

//----------------------------------------------------------------------------
// Deactivate the Make mode and clean up any systems
//----------------------------------------------------------------------------
void fcmdModeMake::DeActivate()
{
	DBG_TRACE("Make Mode DeActivated");
}

///-----------------------------------------------------------------------
/// Do any mode thinking
///-----------------------------------------------------------------------
void fcmdModeMake::Think()
{
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	cptrWriteAVI::set_to_fusion(true);
	fsLocator fs_countdownmovie_path = fcuiUtils::GetCountdownMoviePath();
	fsLocator fs_templocation = gfPaths::GetPath(mnmPaths::e_Cache);
	fs_templocation.Push("temp.avi");
	fsLocator fs_Countdownappended_movie = gfPaths::GetPath(mnmPaths::e_UserMovies);
	fs_Countdownappended_movie.Push("CurrentMovie.avi");
	fsLocator fs_movie_path = fcuiUtils::GetMovieName();
	
	itString movie1, movie2, temp;
	fsFileUtil::LocatorToUnicodeString(fs_countdownmovie_path, movie1);
	fsFileUtil::LocatorToUnicodeString(fs_movie_path, movie2);
	fsFileUtil::LocatorToUnicodeString(fs_templocation, temp);

	//	if rendering starting, wait for it to finish
	//
	int percentage_multiplier = 100;
	maTime current_value = tmlnTimeLine::GetValue();
	maTime Maximum_value = fcuiTimelineMgr::Instance->GetEndTime();
	
	if( m_value < 0 )
		m_value = 0;
	if(m_bRenderStarted)
	{
		m_value = (int)((current_value/Maximum_value) * percentage_multiplier);
		
		if(fcuiUtils::GetChecked())
		{
			if(m_value >= 95)
			{
				m_value = 95;
			}
		}
		if(m_value > 94)
			m_bContinueRender = true;
		if(m_bContinueRender && current_value == maTime::c_ZeroTime )
		{
			if(fcuiUtils::GetChecked())
				m_value = 95;
			else
				m_value = 100;
		}
			
		if(m_value > 100 )
			m_value = 100;
 
		i_ui->ui.Action_ProgressBar->setValue(m_value);
		i_ui->ui.Make_RenderText->setText(QString::number(m_value));
	}

	if (((!cptrPackage::IsRenderModeActive()) && (m_bRenderStarted) )|| ( m_bStopRender))
	{
		// TODO - stop "rendering" dialog here (or render to make screen)

		m_bRenderStarted = false;
		//Reset director camera capture time 
		maTime DriverMaxTime = maTime::FromSeconds(9999.0f);
		fcuiUtils::ExtendDirectorDriver(DriverMaxTime);
		
		if (fcmdModeMgr::Instance != NULL)
		{
			//	fsFileUtil::LocatorToUnicodeString(appended_movie, temp);
			std::string str_file;
			if(fcuiUtils::GetChecked())
			{
				cptrWriteAVI::AppendAVI(movie2, movie1, temp);	
				fsFileUtil::CopyFile(fs_templocation,fs_Countdownappended_movie);
				fs_movie_path = fs_Countdownappended_movie;
				fsFileUtil::LocatorToANSIFilename(fs_Countdownappended_movie, str_file);
			}
			else
				fsFileUtil::LocatorToANSIFilename(fs_movie_path, str_file); // Pass the file path 
			QApplication::restoreOverrideCursor();
			i_ui->ui.Action_ProgressBar->setValue(100);
			i_ui->ui.Make_RenderText->setText(QString::number(100));
						
			std::string file_path("start wmplayer.exe "); // Call windows media player
			file_path += ("\"") + str_file + ("\"");
			if(fsFileUtil::FileExists(fs_movie_path))
				int ret_val = system(file_path.c_str());	
			
			i_ui->ui.Make_ProgressBarWidget->setVisible(false);
			i_ui->ui.zMake_Widget->setVisible(false);
			i_ui->ui.MakeMovieButton->setText("Shoot");
			

			fcmdModeMgr::Instance->ProcessModeEvent(fcmdModeMgr::e_Mode, 
												fcmdModeMgr::GetModeLocator(fcmdModeMgr::e_Action), 
												fcmdModeMgr::e_Action);
			
		}
	}

	//	wait for rendering to start
	//
	if ((cptrPackage::IsRenderModeActive()) && (!m_bRenderStarted))
	{
		try
		{
			/*if(fsFileUtil::FileExists(fs_movie_path))
				fsFileUtil::DeleteFile(fs_movie_path); */
			if(fsFileUtil::FileExists(fs_templocation))
				fsFileUtil::DeleteFile(fs_templocation);
			if(fsFileUtil::FileExists(fs_Countdownappended_movie))
				fsFileUtil::DeleteFile(fs_Countdownappended_movie);
			m_bRenderStarted = true;
			QApplication::setOverrideCursor(Qt::WaitCursor);
		}
		catch(fsFileInUseX& /*i_Ex*/)
		{
			DBG_ERROR("File is in Use");
		}		
	}
}

