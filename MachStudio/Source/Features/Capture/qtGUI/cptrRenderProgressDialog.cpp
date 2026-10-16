/*****************************************************************************
**	cptrRenderProgressDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/qtGUI/cptrRenderProgressDialog.hpp"

//	App
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrModeRenderBatch.hpp"
#include "Features/Capture/cptrRenderStateUtil.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

//	Library
#include "Tool/gpx/gpxRenderControl.hpp"

#include <sstream>

#ifdef USE_QT

#include <QCloseEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

namespace
{
	QString to_qstring(const itString& i_String)
	{
		return QString::fromWCharArray(i_String.GetString());
	}

	QProgressBar* create_gauge(QWidget* i_pParent)
	{
		QProgressBar* pGauge = new QProgressBar(i_pParent);
		pGauge->setRange(0, 100);
		pGauge->setTextVisible(false);
		return pGauge;
	}
}

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
cptrRenderProgressDialog* cptrRenderProgressDialog::DialogInstance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrRenderProgressDialog::cptrRenderProgressDialog( QWidget* parent )
:	QDialog( parent ),
	m_RenderBatchID( 0 ),
	m_fLastScenesPercentage( 0.0f )
{
	this->setWindowTitle( "Render Progress" );
	this->move( 10, 10 );

	QVBoxLayout* bSizer3 = new QVBoxLayout( this );

	m_staticText_SceneNumber = new QLabel( "Scene X of Y", this );
	bSizer3->addWidget( m_staticText_SceneNumber );
	m_gauge_Scenes = create_gauge( this );
	bSizer3->addWidget( m_gauge_Scenes );
	m_staticText_SceneName = new QLabel( "Scene Name", this );
	bSizer3->addWidget( m_staticText_SceneName );
	m_staticText_CameraNumber = new QLabel( "Camera X of Y", this );
	bSizer3->addWidget( m_staticText_CameraNumber );
	m_staticText_LayerName = new QLabel( "Render Layer Name", this );
	bSizer3->addWidget( m_staticText_LayerName );
	m_gauge_Cameras = create_gauge( this );
	bSizer3->addWidget( m_gauge_Cameras );
	m_staticText_CameraName = new QLabel( "Camera Name", this );
	bSizer3->addWidget( m_staticText_CameraName );
	m_gauge_Camera = create_gauge( this );
	bSizer3->addWidget( m_gauge_Camera );
	m_staticText_EstimatedTimeLeft = new QLabel( "Est. Time Left  00:00:00.00", this );
	bSizer3->addWidget( m_staticText_EstimatedTimeLeft );

	QFrame* pLine = new QFrame( this );
	pLine->setFrameShape( QFrame::HLine );
	pLine->setFrameShadow( QFrame::Sunken );
	bSizer3->addWidget( pLine );

	QHBoxLayout* bSizer4 = new QHBoxLayout();
	m_button_Pause = new QPushButton( "Pause", this );
	m_button_Pause->setAutoDefault( false );
	bSizer4->addWidget( m_button_Pause );
	m_button_Save = new QPushButton( "Save", this );
	m_button_Save->setAutoDefault( false );
	bSizer4->addWidget( m_button_Save );
	m_button_Abort = new QPushButton( "Abort", this );
	m_button_Abort->setAutoDefault( false );
	bSizer4->addWidget( m_button_Abort );
	bSizer4->addStretch( 1 );
	bSizer3->addLayout( bSizer4 );

	this->setMinimumWidth( 477 );

	QObject::connect( m_button_Pause, &QPushButton::clicked, [this]() { toggle_pause_rendering(); } );
	QObject::connect( m_button_Save, &QPushButton::clicked, [this]() { Button_Save_OnButtonClick(); } );
	QObject::connect( m_button_Abort, &QPushButton::clicked, [this]() { AbortProgress(); } );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrRenderProgressDialog::~cptrRenderProgressDialog()
{
	if (cptrRenderProgressDialog::DialogInstance == this)
		cptrRenderProgressDialog::DialogInstance = NULL;
}

//--------------------------------------------------------------------
//	set the percentage for scenes
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetScenesRenderPercentage( float i_percentage, const itString& i_label )
{
	m_fLastScenesPercentage = i_percentage;
	m_gauge_Scenes->setValue( (int)i_percentage );
	if (i_label.GetLength() > 0)
		m_staticText_SceneNumber->setText( to_qstring(i_label) );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderProgressDialog::AbortProgress()
{
	modeMode* pMode = modeModeMgr::GetCurrentMode();
	cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(pMode);
	if ( pModeRender != 0 )
	{
		pModeRender->AbortRender();
		m_button_Abort->setEnabled( false );
		pModeRender->PauseRender( false );
	}

	cptrModeRenderBatch* pModeRenderBatch = dynamic_cast<cptrModeRenderBatch*>( modeModeMgr::GetMode( m_RenderBatchID ) );
	if ( pModeRenderBatch != 0 )
	{
		if ( modeModeMgr::IsInStack( m_RenderBatchID ) )
		{
			pModeRenderBatch->AbortRenders();
		}
	}
}

//--------------------------------------------------------------------
//	set the label for scene
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetSceneLabel( const itString& i_label )
{
	if (i_label.GetLength() > 0)
		m_staticText_SceneName->setText( to_qstring(i_label) );
}

//--------------------------------------------------------------------
//	set the label for the current render layer
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetRenderLayerLabel( const itString& i_label )
{
	if (i_label.GetLength() > 0)
		m_staticText_LayerName->setText( to_qstring(i_label) );
}

//--------------------------------------------------------------------
//	set the percentage for cameras
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetCamerasRenderPercentage( float i_percentage, const itString& i_label )
{
	m_gauge_Cameras->setValue( (int)i_percentage );
	if (i_label.GetLength() > 0)
		m_staticText_CameraName->setText( to_qstring(i_label) );
}

//--------------------------------------------------------------------
//	set the percentage for camera
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetCameraRenderPercentage( float i_percentage, const itString& i_label )
{
	m_gauge_Camera->setValue( (int)i_percentage );
	if (i_label.GetLength() > 0)
		m_staticText_CameraNumber->setText( to_qstring(i_label) );
}

//--------------------------------------------------------------------
//	set the time that has elapsed
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetTimeElapsed( float i_fTimeElapsed )
{
	float timeleft = calculate_time_left( i_fTimeElapsed );
	m_staticText_EstimatedTimeLeft->setText( tqtQString(format_time_left( timeleft )) );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderProgressDialog::EnablePauseButton()
{
	m_button_Pause->setEnabled( true );
}

void cptrRenderProgressDialog::DisablePauseButton()
{
	m_button_Pause->setEnabled( false );
}

void cptrRenderProgressDialog::Reset()
{
	m_button_Abort->setEnabled( true );
	m_button_Save->setEnabled( true );
}

void cptrRenderProgressDialog::SetRenderBatchID( int i_ID )
{
	m_RenderBatchID = i_ID;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool cptrRenderProgressDialog::event( QEvent* i_pEvent )
{
	if (i_pEvent->type() == QEvent::WindowActivate)
		m_button_Abort->setEnabled( true );
	return QDialog::event( i_pEvent );
}

//--------------------------------------------------------------------
// The dialog stays up until the render finishes, closing it aborts
//--------------------------------------------------------------------
void cptrRenderProgressDialog::closeEvent( QCloseEvent* i_pEvent )
{
	AbortProgress();
	i_pEvent->ignore();
}

void cptrRenderProgressDialog::reject()
{
	AbortProgress();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderProgressDialog::Button_Save_OnButtonClick()
{
	pause_rendering(true);
	m_button_Save->setEnabled( false );

	// Abort the rendering before saving the render position
	gpxRenderControl::ConfirmSingleThread();

	// now, save the time of the render.
	cptrRenderStateUtil::SaveRenderPosition();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderProgressDialog::pause_rendering(bool i_bPause)
{
	modeMode* pMode = modeModeMgr::GetCurrentMode();
	cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(pMode);
	if ( pModeRender != 0 )
	{
		//	if the mode is about to unpause then enable the save again.
		if (pModeRender->IsRenderPaused())
			m_button_Save->setEnabled( true );

		pModeRender->PauseRender( i_bPause );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderProgressDialog::toggle_pause_rendering()
{
	modeMode* pMode = modeModeMgr::GetCurrentMode();
	cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(pMode);
	if ( pModeRender != 0 )
	{
		//	if the mode is about to unpause then enable the save again.
		if (pModeRender->IsRenderPaused())
		{
			m_button_Save->setEnabled( true );
			m_button_Pause->setText("Pause");
		}
		else
		{
			m_button_Pause->setText("Resume");
		}

		pModeRender->PauseRender( !pModeRender->IsRenderPaused() );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
std::string cptrRenderProgressDialog::format_time_left(float i_fTimeLeft)
{
	int hrs, mins, secs;
	float msecs;
	tmlnTimeUtil::GetTimeInHMSM( maTime::FromSeconds(i_fTimeLeft), hrs, mins, secs, msecs );

	std::ostringstream oss;
	oss <<hrs<<":"<<mins<<":"<<secs<<":"<<(int)msecs;
	return oss.str();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float cptrRenderProgressDialog::calculate_time_left(float i_fTimeElapsed)
{
	if (m_fLastScenesPercentage != 0.0f)
		return i_fTimeElapsed * ( (100.0f - m_fLastScenesPercentage) / m_fLastScenesPercentage );
	return 0.0f;
}

#endif
