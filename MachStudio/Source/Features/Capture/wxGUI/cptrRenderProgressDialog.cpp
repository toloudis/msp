/*****************************************************************************
**	cptrRenderProgressDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/wxGUI/cptrRenderProgressDialog.hpp"

//	App
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrModeRenderBatch.hpp"
#include "Features/Capture/cptrRenderStateUtil.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

//	Library
#include "Tool/gpx/gpxRenderControl.hpp"
//#include "ToolUIWx/Twx/twxPaneMgr.hpp"

#include <sstream>
#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
cptrRenderProgressDialog* cptrRenderProgressDialog::DialogInstance = NULL;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrRenderProgressDialog::cptrRenderProgressDialog( wxWindow* parent, wxWindowID id, 
													const wxString& title, 
													const wxPoint& pos, 
													const wxSize& size, 
													long style )
:	cptrRenderProgressDialogBase( parent, id, title, pos, size, style )
{
	//twxPaneMgr::AddPane( this, wxAuiPaneInfo().Name(title).Caption(title).Show().Layer(2).Float());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrRenderProgressDialog::~cptrRenderProgressDialog()
{
	if (cptrRenderProgressDialog::DialogInstance == this)
		cptrRenderProgressDialog::DialogInstance = NULL;
}

//----------------------------------------------------------------------------
//	set the percentage for scenes
//----------------------------------------------------------------------------
void cptrRenderProgressDialog::SetScenesRenderPercentage( float i_percentage, const itString& i_label )
{
	m_fLastScenesPercentage = i_percentage;
	m_gauge_Scenes->SetValue( (int)i_percentage );

	if (i_label.GetLength() > 0)
	{
		this->Freeze();
		m_staticText_SceneNumber->SetLabel( i_label.GetString() );
		this->Thaw();
	}
}

//----------------------------------------------------------------------------
//	set the percentage for scenes
//----------------------------------------------------------------------------
void cptrRenderProgressDialog::AbortProgress()
{
	//
	modeMode* pMode = modeModeMgr::GetCurrentMode();
	cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(pMode);
	cptrModeRenderBatch* pModeRenderBatch = 0;
	if ( pModeRender != 0 )
	{
		pModeRender->AbortRender();
		m_button_Abort->Enable( false );

		pModeRender->PauseRender( false );
	}

	pModeRenderBatch = dynamic_cast<cptrModeRenderBatch*>( modeModeMgr::GetMode( m_RenderBatchID ) );
	if ( pModeRenderBatch != 0 )
	{
		if ( modeModeMgr::IsInStack( m_RenderBatchID ) )
		{
			pModeRenderBatch->AbortRenders();
		}
	}
}

//--------------------------------------------------------------------
//	set the percentage for scene
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetSceneLabel( const itString& i_label )
{
	if (i_label.GetLength() > 0)
	{
		this->Freeze();
		m_staticText_SceneName->SetLabel( i_label.GetString() );
		this->Thaw();
	}
}

//--------------------------------------------------------------------
//	set the label for the current render layer
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetRenderLayerLabel( const itString& i_label )
{
	if (i_label.GetLength() > 0)
	{
		this->Freeze();
		m_staticText_LayerName->SetLabel( i_label.GetString() );
		this->Thaw();
	}
}

//--------------------------------------------------------------------
//	set the percentage for cameras
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetCamerasRenderPercentage( float i_percentage, const itString& i_label )
{
	m_gauge_Cameras->SetValue(  (int)i_percentage );

	if (i_label.GetLength() > 0)
	{
		this->Freeze();
		if (i_label.GetLength() > 0)
			m_staticText_CameraName->SetLabel( i_label.GetString() );
		this->Thaw();
	}
}

//--------------------------------------------------------------------
//	set the percentage for camera
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetCameraRenderPercentage( float i_percentage, const itString& i_label )
{
	m_gauge_Camera->SetValue( (int)i_percentage );

	if (i_label.GetLength() > 0)
	{
		this->Freeze();
		m_staticText_CameraNumber->SetLabel( i_label.GetString() );
		this->Thaw();
	}
}

//--------------------------------------------------------------------
//	set the time that has elapsed
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetTimeElapsed( float i_fTimeElapsed )
{
	float timeleft = calculate_time_left( i_fTimeElapsed );
	this->Freeze();
	m_staticText_EstimatedTimeLeft->SetLabel( wxString(format_time_left( timeleft ).c_str(), wxConvUTF8) );
	this->Thaw();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderProgressDialog::EnablePauseButton()
{
	m_button_Pause->Enable(true);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderProgressDialog::DisablePauseButton()
{
	m_button_Pause->Enable( false );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderProgressDialog::Reset()
{
	m_button_Abort->Enable( true );
	m_button_Save->Enable( true );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetRenderBatchID( int i_ID )
{
	m_RenderBatchID = i_ID;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void cptrRenderProgressDialog::OnActivate( wxActivateEvent& event )
{ 
	m_button_Abort->Enable( true );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void cptrRenderProgressDialog::OnClose( wxCloseEvent& event )
{ 
	AbortProgress();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void cptrRenderProgressDialog::Button_Pause_OnButtonClick( wxCommandEvent& event )
{ 
	toggle_pause_rendering();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void cptrRenderProgressDialog::Button_Save_OnButtonClick( wxCommandEvent& event )
{ 
	pause_rendering(true);

	m_button_Save->Enable( false );

	// Abort the rendering before saving the render position
	gpxRenderControl::ConfirmSingleThread();

	// now, save the time of the render.
	cptrRenderStateUtil::SaveRenderPosition();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void cptrRenderProgressDialog::Button_Abort_OnButtonClick( wxCommandEvent& event )
{ 
	AbortProgress();
}

//--------------------------------------------------------------------
// way to combine pause + toggle_pause?
//--------------------------------------------------------------------
void cptrRenderProgressDialog::pause_rendering(bool i_bPause)
{
	modeMode* pMode = modeModeMgr::GetCurrentMode();
	cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(pMode);
	cptrModeRenderBatch* pModeRenderBatch = 0;
	if ( pModeRender != 0 )
	{
		//	if the mode is about to unpause then enable the save again.
		//
		if (pModeRender->IsRenderPaused())
			m_button_Save->Enable( true );

		pModeRender->PauseRender( i_bPause );
	}
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void cptrRenderProgressDialog::toggle_pause_rendering()
{
	modeMode* pMode = modeModeMgr::GetCurrentMode();
	cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(pMode);
	cptrModeRenderBatch* pModeRenderBatch = 0;
	if ( pModeRender != 0 )
	{
		//	if the mode is about to unpause then enable the save again.
		//
		if (pModeRender->IsRenderPaused())
			m_button_Save->Enable( true );

		if (pModeRender->IsRenderPaused())
		{
			m_button_Pause->SetLabel(L"Pause");
		}
		else
		{
			m_button_Pause->SetLabel(L"Resume");
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

	//char buffer[64];
	//sprintf(buffer, "%0d:%0d:%0d.%0d", hrs, mins, secs, (int)msecs);
	std::ostringstream oss;
	oss <<hrs<<":"<<mins<<":"<<secs<<":"<<(int)msecs;
	std::string buffer(oss.str());
	return buffer;
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