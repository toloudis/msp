/****************************************************************************\
**	tqcFloatEdit.hpp
**
**		Custom text control for floating point values.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_FLOATEDIT_HPP
#error tqcFloatEdit.hpp multiply included
#endif
#define TQC_FLOATEDIT_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT
#include <QtGui/QLineEdit>


//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed after ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class tqcFloatEdit : public QLineEdit
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcFloatEdit(QWidget* i_pParent);
#ifdef QT_FINISH_PORT
			, 
					 QWidgetID i_Id = wxID_ANY,
					 const wxPoint& i_Pos = wxDefaultPosition, 
					 const wxSize& i_Size = wxDefaultSize);
#endif;

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
#ifdef QT_FINISH_PORT
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange(wxCommandEvent& i_Event);
		void OnTextLeave(wxFocusEvent& i_Event);
#endif

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void text_changed();

		float m_FloatValue;
		short m_DecimalPlaces;

#ifdef QT_FINISH_PORT
DECLARE_EVENT_TABLE()
#endif
};

#endif // USE_QT
