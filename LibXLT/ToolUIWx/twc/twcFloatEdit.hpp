/****************************************************************************\
**	twcFloatEdit.hpp
**
**		Custom text control for floating point values.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_FLOATEDIT_HPP
#error twcFloatEdit.hpp multiply included
#endif
#define TWC_FLOATEDIT_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//#ifndef TWC_EVENT_HPP
//#include "ToolUIWx/twc/twcEvent.hpp"
//#endif

#ifdef USE_WXWIDGETS

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed aftere ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class twcFloatEdit : public wxTextCtrl
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcFloatEdit(wxWindow* i_pParent, 
					 wxWindowID i_Id = wxID_ANY,
					 const wxPoint& i_Pos = wxDefaultPosition, 
					 const wxSize& i_Size = wxDefaultSize);

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const float GetFloatValue() const;
		void SetFloatValue(const float i_Value);

		//--------------------------------------------------------------------
		//	DecimalPlaces
		//--------------------------------------------------------------------
		const short GetDecimalPlaces() const;
		void SetDecimalPlaces(const short i_DecimalPlaces);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange(wxCommandEvent& i_Event);
		void OnTextLeave(wxFocusEvent& i_Event);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void text_changed();

		float m_FloatValue;
		short m_DecimalPlaces;

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
