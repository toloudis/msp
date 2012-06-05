/****************************************************************************\
**	twcTextBox.hpp
**
**		Textbox that handles value changed events like we like it.
**	ValueChanged fires when the text box's value is different
**	and either ENTER has been pressed, or the focus leaves the 
**	text box.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_TEXTBOX_HPP
#error twcTextBox.hpp multiply included
#endif
#define TWC_TEXTBOX_HPP

#ifndef TWC_EVENT_HPP
#include "ToolUIWx/twc/twcEvent.hpp"
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
class twcTextBox : public wxTextCtrl
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcTextBox(wxWindow* i_pParent, 
				   wxWindowID i_Id = wxID_ANY, 
				   const wxString& i_Value = wxT(""),
				   bool i_bMultiline = false,
				   bool i_bMultilineFixed = false,
				   const wxSize& i_Size = wxDefaultSize);

		//--------------------------------------------------------------------
		// Override SetValue so that we can track the current value
		//	in order to know when it changes.
		//--------------------------------------------------------------------
		virtual void SetValue(const wxString& value);

		//--------------------------------------------------------------------
		// This function is used in twcFilePicker in order to restore the old 
		// value when a user partially deletes the file.
		//--------------------------------------------------------------------
		wxString last_value();

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange(wxCommandEvent& i_Event);
		void OnTextLeave(wxFocusEvent& i_Event);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void text_changed();

		bool m_bMultiline;
		

		wxString m_LastValue;
		wxString m_PrevValue;

    DECLARE_EVENT_TABLE()
};

//============================================================================
// Convenience class name in order to get a multiline text box without
// needing to pass in extra arguments.
//============================================================================
class twcMultilineTextBox : public twcTextBox
{
	public:
		//--------------------------------------------------------------------
		// Passes true for i_bMultiline to base class constructor.
		//--------------------------------------------------------------------
		twcMultilineTextBox(wxWindow* i_pParent, 
						   wxWindowID i_Id = wxID_ANY)
			:	twcTextBox(i_pParent, i_Id, wxT(""), true, false) {}
}
;

//============================================================================
// Convenience class name in order to get a multiline text box without
// needing to pass in extra arguments.
//============================================================================
class twcMultilineFixedTextBox : public twcTextBox
{
	public:
		//--------------------------------------------------------------------
		// Passes true for i_bMultiline to base class constructor.
		//--------------------------------------------------------------------
		twcMultilineFixedTextBox(wxWindow* i_pParent, 
								wxWindowID i_Id = wxID_ANY)
			:	twcTextBox(i_pParent, i_Id, wxT(""), true, true) {}
}
;

#endif // USE_WXWIDGETS
