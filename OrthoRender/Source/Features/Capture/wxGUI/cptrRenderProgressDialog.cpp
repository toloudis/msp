/*****************************************************************************
**	cptrRenderProgressDialog.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/wxGUI/cptrRenderProgressDialog.hpp"

//	App
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrModeRenderBatch.hpp"
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

//	Library
//#include "ToolUIWx/Twx/twxPaneMgr.hpp"


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
void cptrRenderProgressDialog::SetScenesRenderPercentage( float i_percentage, std::string& i_label )
{
	m_fLastScenesPercentage = i_percentage;
	m_gauge_Scenes->SetValue( (int)i_percentage );

	if (i_label.length() > 0)
	{
		this->Freeze();
		m_staticText_SceneNumber->SetLabel( i_label.c_str() );
		this->Thaw();
	}
}

//--------------------------------------------------------------------
//	set the percentage for scene
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetSceneLabel( std::string& i_label )
{
	if (i_label.length() > 0)
	{
		this->Freeze();
		m_staticText_SceneName->SetLabel( i_label.c_str() );
		this->Thaw();
	}
}

//--------------------------------------------------------------------
//	set the percentage for cameras
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetCamerasRenderPercentage( float i_percentage, std::string& i_label )
{
	m_gauge_Cameras->SetValue(  (int)i_percentage );

	if (i_label.length() > 0)
	{
		this->Freeze();
		if (i_label.size() > 0)
			m_staticText_CameraName->SetLabel( i_label.c_str() );
		this->Thaw();
	}
}

//--------------------------------------------------------------------
//	set the percentage for camera
//--------------------------------------------------------------------
void cptrRenderProgressDialog::SetCameraRenderPercentage( float i_percentage, std::string& i_label )
{
	m_gauge_Camera->SetValue( (int)i_percentage );

	if (i_label.length() > 0)
	{
		this->Freeze();
		m_staticText_CameraNumber->SetLabel( i_label.c_str() );
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
	m_staticText_EstimatedTimeLeft->SetLabel( format_time_left( timeleft ).c_str() );
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
	event.Skip(); 
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void cptrRenderProgressDialog::OnClose( wxCloseEvent& event )
{ 
	event.Skip(); 
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

	// now, save the time of the render.
	cptrRenderProgressDialogUtil::SaveRenderPosition();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void cptrRenderProgressDialog::Button_Abort_OnButtonClick( wxCommandEvent& event )
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
			m_button_Pause->SetLabel("Pause");
		}
		else
		{
			m_button_Pause->SetLabel("Resume");
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
	tmlnTimeUtil::GetTimeInHMSM( i_fTimeLeft, hrs, mins, secs, msecs );

	char buffer[64];
	sprintf(buffer, "%0d:%0d:%0d.%0d", hrs, mins, secs, (int)msecs);
	return std::string(buffer);
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