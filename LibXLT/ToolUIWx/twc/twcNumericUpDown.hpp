/****************************************************************************\
**	twcNumericUpDown.hpp
**
**		Custom control with float edit and spin button controls
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_NUMERICUPDOWN_HPP
#error twcNumericUpDown.hpp multiply included
#endif
#define TWC_NUMERICUPDOWN_HPP

#ifndef TWC_FLOATEDIT_HPP
#include "ToolUIWx/twc/twcFloatEdit.hpp"
#endif

#ifdef USE_WXWIDGETS

#include <wx/spinbutt.h>


//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed after ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class twcNumericUpDown : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcNumericUpDown(wxWindow* i_pParent);
		~twcNumericUpDown();

		static twcNumericUpDown* Instance;

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const float GetValue() const;
		void SetValue(const float i_Value);

		//--------------------------------------------------------------------
		//	Minimum
		//--------------------------------------------------------------------
		const float GetMinimum() const;
		void SetMinimum(const float i_Minimum);

		//--------------------------------------------------------------------
		//	Maximum
		//--------------------------------------------------------------------
		const float GetMaximum() const;
		void SetMaximum(const float i_Maximum);

		//--------------------------------------------------------------------
		//  Functions to check if the flags restricting ranges is set
		//--------------------------------------------------------------------

		const bool GetRestrictFlag() const;
		void SetRestrictFlag( const bool i_bRestrictValue);	

		//--------------------------------------------------------------------
		//	DecimalPlaces
		//--------------------------------------------------------------------
		const short GetDecimalPlaces() const;
		void SetDecimalPlaces(const short i_DecimalPlaces);

		//--------------------------------------------------------------------
		//	Increment
		//--------------------------------------------------------------------
		const float GetIncrement() const;
		void SetIncrement(const float i_Increment);

		//--------------------------------------------------------------------
		// Calls the callback and appropriate event handlers
		//--------------------------------------------------------------------
		bool ProcessCommand(wxCommandEvent& i_Event);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange(wxCommandEvent& i_Event);
		void OnScrollUp(wxSpinEvent& i_Event);
		void OnScrollDown(wxSpinEvent& i_Event);
		void OnMouseClick(wxMouseEvent& i_Event);
		void OnMouseDrag(wxMouseEvent& i_Event);
		void OnMouseUp(wxMouseEvent& i_Event);
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_slider();
		void notify_callback();

		wxSpinButton* m_pSpinner;
		twcFloatEdit* m_pFloatEdit;
		
		float m_fValue;
		float m_Minimum, m_Maximum;
		float m_Increment;
		float m_yInitial, m_yOffset, new_value;

		bool m_bRestrictValue;
		bool m_bspinButtonClicked;
		
    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
