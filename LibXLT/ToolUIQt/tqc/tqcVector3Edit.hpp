/****************************************************************************\
**	tqcVector3Edit.hpp
**
**		Custom control with 3 float edit controls
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_VECTOR3EDIT_HPP
#error tqcVector3Edit.hpp multiply included
#endif
#define TQC_VECTOR3EDIT_HPP

#ifndef TQC_FLOATEDIT_HPP
#include "ToolUIQt/tqc/tqcFloatEdit.hpp"
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
class tqcVector3Edit : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcVector3Edit(QWidget* i_pParent);

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const maVector3d& GetValue() const;
		void SetValue(const maVector3d& i_Value);

		//--------------------------------------------------------------------
		//	DecimalPlaces
		//--------------------------------------------------------------------
		const short GetDecimalPlaces() const;
		void SetDecimalPlaces(const short i_DecimalPlaces);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange(wxCommandEvent& i_Event);

		tqcFloatEdit* m_pFloatEditX;
		tqcFloatEdit* m_pFloatEditY;
		tqcFloatEdit* m_pFloatEditZ;
		
		maVector3d m_Value;
};

#endif // USE_QT
