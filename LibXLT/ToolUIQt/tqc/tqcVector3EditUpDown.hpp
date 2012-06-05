/****************************************************************************\
**	tqcVector3EditUpDown.hpp
**
**		Custom control with 3 float edit controls
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_VECTOR3EDITUPDOWN_HPP
#error tqcVector3EditUpDown.hpp multiply included
#endif
#define TQC_VECTOR3EDITUPDOWN_HPP

#ifndef TQC_NUMERICUPDOWN_HPP
#include "ToolUIQt/tqc/tqcNumericUpDown.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif

#ifdef QT_FINISH_PORT

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed aftere ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class tqcVector3EditUpDown : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcVector3EditUpDown(QWidget* i_pParent);

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const maVector3d& GetValue() const;
		void SetValue(const maVector3d& i_Value);

		//--------------------------------------------------------------------
		//	Increment
		//--------------------------------------------------------------------
		maVector3d GetIncrement() const;
		void SetIncrement(const maVector3d& i_Increment);
		void SetIncrement(const float i_IncX, const float i_IncY, const float i_IncZ);

		//--------------------------------------------------------------------
		//	Minimum
		//--------------------------------------------------------------------
		maVector3d GetMinimum() const;
		void SetMinimum(const maVector3d& i_Minimum);
		void SetMinimum(const float i_ValX, const float i_ValY, const float i_ValZ);

		//--------------------------------------------------------------------
		//	Maximum
		//--------------------------------------------------------------------
		maVector3d GetMaximum() const;
		void SetMaximum(const maVector3d& i_Maximum);
		void SetMaximum(const float i_ValX, const float i_ValY, const float i_ValZ);

		//--------------------------------------------------------------------
		//	DecimalPlaces
		//--------------------------------------------------------------------
		const short GetDecimalPlaces() const;
		void SetDecimalPlaces(const short i_DecimalPlaces);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange(wxCommandEvent& i_Event);

		tqcNumericUpDown* m_pNumericUpDownX;
		tqcNumericUpDown* m_pNumericUpDownY;
		tqcNumericUpDown* m_pNumericUpDownZ;
		
		maVector3d m_Value;
};

#endif // USE_QT
