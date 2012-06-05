/*****************************************************************************
**  wxTimeSlider.hpp
**
**     Time slider in main form when using wxWidgets
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef WX_TIMESLIDER_HPP
#error wxMainForm.hpp multiply included
#endif
#define WX_TIMESLIDER_HPP

#ifndef TWC_RANGEDFLOAT_HPP
#include "ToolUIWx/twc/twcRangedFloat.hpp"
#endif 
#ifndef TMLN_TIMEINTEREST_HPP
#include "Support/tmln/tmlnTimeInterest.hpp"
#endif 

#ifdef USE_WXWIDGETS

#include <wx/slider.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/panel.h>

//============================================================================
//============================================================================
class ptmTimeEdit;

//============================================================================
/// Class wxTimeSlider
//============================================================================
class wxTimeSlider : public wxPanel, public tmlnTimeInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		wxTimeSlider( wxWindow* parent, 
					  wxWindowID id = wxID_ANY, 
					  const wxPoint& pos = wxDefaultPosition, 
					  const wxSize& size = wxSize( 500,55 ), 
					  long style = wxTAB_TRAVERSAL );
		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~wxTimeSlider();

//============================================================================
// TimeInterest callbacks
//============================================================================

		//--------------------------------------------------------------------
		//	TimeChanged - timeline current time has changed
		//--------------------------------------------------------------------
		virtual void TimeChanged( float i_Time );

		//--------------------------------------------------------------------
		//	TimeRangeChanged - timeline maximum time has changed
		//--------------------------------------------------------------------
		virtual void TimeRangeChanged( float i_MinTime, float i_MaxTime );

		//--------------------------------------------------------------------
		//	TimeFormatChanged - timeline time display format has changed
		//--------------------------------------------------------------------
		virtual void TimeFormatChanged( int i_TimeFormat );
	
	private:

		//----------------------------------------------------------------------------
		// event callbacks
		//----------------------------------------------------------------------------
		void SliderThumbDown(wxCommandEvent &i_Event);
		void SliderThumbUp(wxCommandEvent &i_Event);
		void SliderChanged(wxCommandEvent &i_Event);
		void CurrentTimeChanged(wxCommandEvent &i_Event);
		void MaxTimeChanged(wxCommandEvent &i_Event);
		void MinTimeChanged(wxCommandEvent &i_Event);

		twcRangedFloat* m_slider1;
		ptmTimeEdit* m_floatEdit_CurrentTime;
		wxStaticText* m_staticText1;
		ptmTimeEdit* m_floatEdit_MaxTime;
		wxStaticText* m_staticText2;
		ptmTimeEdit* m_floatEdit_MinTime;
		wxStaticText* m_staticText3;
	
};

#endif // USE_WXWIDGETS

