/****************************************************************************\
**	twcVector3Edit.hpp
**
**		Custom control with 3 float edit controls
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_VECTOR3EDIT_HPP
#error twcVector3Edit.hpp multiply included
#endif
#define TWC_VECTOR3EDIT_HPP

#ifndef TWC_FLOATEDIT_HPP
#include "ToolUIWx/twc/twcFloatEdit.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
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
class twcVector3Edit : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcVector3Edit(wxWindow* i_pParent);

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

		twcFloatEdit* m_pFloatEditX;
		twcFloatEdit* m_pFloatEditY;
		twcFloatEdit* m_pFloatEditZ;
		
		maVector3d m_Value;
};

#endif // USE_WXWIDGETS
