/****************************************************************************\
**	pwxBoolean_CheckBox.hpp
**
**		Intermediate class between the property (prtyBoolean) and 
**	the control (CheckBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_BOOLEAN_CHECKBOX_HPP
#error pwxBoolean_CheckBox.hpp multiply included
#endif
#define PWX_BOOLEAN_CHECKBOX_HPP

#ifndef PWX_CONTROL_HPP
#include "ToolUIWx/pwx/pwxControl.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class pwxBoolean_CheckBox : public pwxControl
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pwxBoolean_CheckBox(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							wxWindow* i_pParent);

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		~pwxBoolean_CheckBox();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual wxWindow* GetControl();

		//----------------------------------------------------------------------------
		//	Note: right now, PropertyChanged will not create a circular update because
		//	of the m_bLocalChangeNoUpdate flag.  This IS NOT true the other way
		//	around.  If a control calls its "ValueChanged" then the property will
		//	call all of its callback controls and one of them could have been the one
		//	that originally updated the property value(s).  By checking the diff of 
		//	the values we can avoid a repetitive setting of the control's values.
		//----------------------------------------------------------------------------
		virtual void PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty);

		//----------------------------------------------------------------------------
		// Update control to match the changes in the UI Info
		//----------------------------------------------------------------------------
		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo);

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void CheckBox_ValueChanged(wxCommandEvent& i_Event);

		wxCheckBox* m_pActualControl;
};

#endif // USE_WXWIDGETS
