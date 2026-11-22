/****************************************************************************\
**	tqcFloatEdit.hpp
**
**	Text box for editing a floating point value. The value changed
**	callback is called after ENTER is pressed or the control is left
**	with a different value.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_FLOATEDIT_HPP
#error tqcFloatEdit.hpp multiply included
#endif
#define TQC_FLOATEDIT_HPP

#ifndef TQC_EVENT_HPP
#include "ToolUIQt/tqc/tqcEvent.hpp"
#endif

#ifdef USE_QT

//============================================================================
//============================================================================
class tqcFloatEdit : public QLineEdit, public tqcValueChangedNotifier
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcFloatEdit(QWidget* i_pParent);

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const float GetFloatValue() const;
		void SetFloatValue(const float i_Value);

		//--------------------------------------------------------------------
		//	DecimalPlaces, -1 for the natural format with 6 digits
		//--------------------------------------------------------------------
		const short GetDecimalPlaces() const;
		void SetDecimalPlaces(const short i_DecimalPlaces);

		//--------------------------------------------------------------------
		// Change the text without notifying (like wxTextCtrl::ChangeValue)
		//--------------------------------------------------------------------
		void ChangeValue(const QString& i_Text);

	private:
		void text_changed();

		float m_FloatValue;
		short m_DecimalPlaces;
};

#endif // USE_QT
