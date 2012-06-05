/****************************************************************************\
**	twcRangedFloat.hpp
**
**		Custom control with slider and float edit controls
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_RANGEDFLOAT_HPP
#error twcRangedFloat.hpp multiply included
#endif
#define TWC_RANGEDFLOAT_HPP

#ifndef TWC_FLOATEDIT_HPP
#include "ToolUIWx/twc/twcFloatEdit.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed aftere ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class twcRangedFloat : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcRangedFloat(wxWindow* i_pParent);

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const float GetValue() const;
		void SetValue(const float i_Value);

		//--------------------------------------------------------------------
		// Set minimum and maximum in one step.
		//--------------------------------------------------------------------
		void SetRange(float i_Minimum, float i_Maximum);

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
		//	NumTicks
		//--------------------------------------------------------------------
		const short GetNumTicks() const;
		void SetNumTicks(const short i_NumTicks);

		//--------------------------------------------------------------------
		//	Exponent
		//--------------------------------------------------------------------
		const short GetExponent() const;
		void SetExponent(const short i_Exponent);

		//--------------------------------------------------------------------
		// ShowValue turns on and off the display of the float edit control
		//--------------------------------------------------------------------
		void ShowValue(bool i_bShow);

		//--------------------------------------------------------------------
		// If true, this flag allows the slider to expand its range to 
		// accomodate values outside of its minimum and maximum.
		//--------------------------------------------------------------------
		void AllowExpandRange(bool i_bExpand);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange(wxCommandEvent& i_Event);
		void OnScrollChanged(wxScrollEvent& i_Event);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_slider();
		void notify_callback();

		wxSlider* m_pSlider;
		twcFloatEdit* m_pFloatEdit;
		
		float m_Value;
		float m_Minimum, m_Maximum;
		short m_NumTicks;
		short m_Exponent;
		bool m_bExpandRange;
		bool m_bRestrictValue;

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
