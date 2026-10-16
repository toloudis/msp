/****************************************************************************\
**	tqcNumericUpDown.hpp
**
**	Float edit box with up/down arrows. Clicking an arrow steps the value
**	by the increment (repeating while held); dragging up or down from an
**	arrow changes the value continuously.
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

#ifdef USE_QT

class tqcSpinArrows;

//============================================================================
//============================================================================
class tqcNumericUpDown : public QWidget, public tqcValueChangedNotifier
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcNumericUpDown(QWidget* i_pParent);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~tqcNumericUpDown();

		//--------------------------------------------------------------------
		//	Static pointer to the instance of the control
		//--------------------------------------------------------------------
		static tqcNumericUpDown* Instance;

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const float GetValue() const;
		void SetValue(const float i_Value);

		//--------------------------------------------------------------------
		//	Minimum / Maximum, used when the restrict flag is set
		//--------------------------------------------------------------------
		const float GetMinimum() const;
		void SetMinimum(const float i_Minimum);
		const float GetMaximum() const;
		void SetMaximum(const float i_Maximum);
		const bool GetRestrictFlag() const;
		void SetRestrictFlag( const bool i_bRestrictValue);

		//--------------------------------------------------------------------
		//	DecimalPlaces - passed on to FloatEdit control
		//--------------------------------------------------------------------
		const short GetDecimalPlaces() const;
		void SetDecimalPlaces(const short i_DecimalPlaces);

		//--------------------------------------------------------------------
		//	Increment
		//--------------------------------------------------------------------
		const float GetIncrement() const;
		void SetIncrement(const float i_Increment);

	private:
		friend class tqcSpinArrows;

		void OnTextChange();
		void step(float i_Steps);
		float restrict_value(float i_Value) const;

		tqcSpinArrows* m_pSpinner;
		tqcFloatEdit* m_pFloatEdit;
		float m_fValue;
		float m_Minimum, m_Maximum;
		float m_Increment;
		bool m_bRestrictValue;
};

#endif // USE_QT
