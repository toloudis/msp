/*****************************************************************************
**  wxSliderPane.hpp
**
**     Window using wxWidgets for doing Terawatt rendering
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "wxSliderPane.hpp"
#include "mspViewSettings.hpp"

#include "ToolUIWx/twc/twcRangedFloat.hpp"
#include "ToolUIWx/twc/twcEvent.hpp"

#ifdef USE_WXWIDGETS

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(wxSliderPane, wxPanel)
END_EVENT_TABLE()


//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
wxSliderPane::wxSliderPane(wxWindow* parent)
: wxPanel(parent, wxID_ANY)
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

    m_pSlider = new twcRangedFloat( this );
	m_pSlider->Enable( false );
	m_pSlider->SetValue( mspViewSettings::sm_CurrentFrame.GetValue() );
	m_pSlider->SetMaximum( mspViewSettings::sm_ModelAnimNumFrames.GetValue() );
	m_pSlider->SetNumTicks( mspViewSettings::sm_ModelAnimNumFrames.GetValue() );
	m_pSlider->SetDecimalPlaces(0);
	bSizer1->Add( m_pSlider, 1, wxEXPAND | wxALL, 0 );

	//	hook up events
	//
	this->Connect( m_pSlider->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(wxSliderPane::Slider_ValueChanged) );

	this->SetSizer( bSizer1 );
	this->Layout();

	// Register callbacks on the settings we care about
	m_pCurrentFrameCallback.reset(new prtyCallbackWrapper<wxSliderPane>(this, &wxSliderPane::CurrentFrameChanged));
	mspViewSettings::sm_CurrentFrame.AddCallback(m_pCurrentFrameCallback);
	m_pPlaybackBeginCallback.reset(new prtyCallbackWrapper<wxSliderPane>(this, &wxSliderPane::PlaybackRangeChanged));
	mspViewSettings::sm_PlaybackBegin.AddCallback(m_pPlaybackBeginCallback);
	m_pPlaybackEndCallback.reset(new prtyCallbackWrapper<wxSliderPane>(this, &wxSliderPane::PlaybackRangeChanged));
	mspViewSettings::sm_PlaybackEnd.AddCallback(m_pPlaybackEndCallback);
	m_pPauseAnimationCallback.reset(new prtyCallbackWrapper<wxSliderPane>(this, &wxSliderPane::PauseAnimationChanged));
	mspViewSettings::sm_PauseAnimation.AddCallback(m_pPauseAnimationCallback);
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
wxSliderPane::~wxSliderPane()
{
	// Remove callbacks
	mspViewSettings::sm_CurrentFrame.RemoveCallback(m_pCurrentFrameCallback);
	mspViewSettings::sm_PlaybackBegin.RemoveCallback(m_pPlaybackBeginCallback);
	mspViewSettings::sm_PlaybackEnd.RemoveCallback(m_pPlaybackEndCallback);
	mspViewSettings::sm_ModelAnimNumFrames.RemoveCallback(m_pPauseAnimationCallback);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void wxSliderPane::Slider_ValueChanged(wxCommandEvent& i_Event)
{
	// Do we need to test for circular, infinite updates?
	mspViewSettings::sm_CurrentFrame.SetValue( m_pSlider->GetValue() );
}

//----------------------------------------------------------------------------
// property callbacks
//----------------------------------------------------------------------------
void wxSliderPane::CurrentFrameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_pSlider->SetValue( mspViewSettings::sm_CurrentFrame.GetValue() );
}
void wxSliderPane::PlaybackRangeChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	float begin = mspViewSettings::sm_PlaybackBegin.GetValue();
	float end = mspViewSettings::sm_PlaybackEnd.GetValue();

	m_pSlider->SetMinimum( begin );
	m_pSlider->SetMaximum( end );

	int num_ticks = (end - begin);
	if (num_ticks < 0) num_ticks = 0;
	m_pSlider->SetNumTicks( num_ticks );
}
void wxSliderPane::PauseAnimationChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_pSlider->Enable( mspViewSettings::sm_PauseAnimation.GetValue() );
}


#endif // USE_WXWIDGETS