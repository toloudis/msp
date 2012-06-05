/****************************************************************************\
**	tqcNumericUpDown.hpp
**
**		Custom control with float edit and spin button controls
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_NUMERICUPDOWN_HPP
#error tqcNumericUpDown.hpp multiply included
#endif
#define TQC_NUMERICUPDOWN_HPP

#ifndef TQC_FLOATEDIT_HPP
#include "ToolUIQt/tqc/tqcFloatEdit.hpp"
#endif
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT
#include <QtGui/QSpinBox>


//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed after ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class tqcNumericUpDown : public QSpinBox
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcNumericUpDown(QWidget* i_pParent);
		~tqcNumericUpDown();

		static tqcNumericUpDown* Instance;

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
#ifdef QT_FINISH_PORT
		bool ProcessCommand(wxCommandEvent& i_Event);
#endif

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
#ifdef QT_FINISH_PORT
		void OnTextChange(wxCommandEvent& i_Event);
		void OnScrollUp(wxSpinEvent& i_Event);
		void OnScrollDown(wxSpinEvent& i_Event);
		void OnMouseClick(wxMouseEvent& i_Event);
		void OnMouseDrag(wxMouseEvent& i_Event);
		void OnMouseUp(wxMouseEvent& i_Event);
#endif
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_slider();
		void notify_callback();

		//QSpinBox* m_pSpinner;
		//tqcFloatEdit* m_pFloatEdit;
		
		float m_fValue;
		float m_Minimum, m_Maximum;
		float m_Increment;
		float m_yInitial, m_yOffset, new_value;

		bool m_bRestrictValue;
		bool m_bspinButtonClicked;

#ifdef QT_FINISH_PORT
	DECLARE_EVENT_TABLE()
#endif
};

#endif // USE_QT
