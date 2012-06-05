/****************************************************************************\
**	pwxColor_ColorRGBAEdit.hpp
**
**		Intermediate class between the property (prtyColor) and 
**	the control (ColorRGBAEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_COLOR_COLORRGBAEDIT_HPP
#error pwxColor_ColorRGBAEdit.hpp multiply included
#endif
#define PWX_COLOR_COLORRGBAEDIT_HPP

#ifndef PWX_CONTROL_HPP
#include "ToolUIWx/pwx/pwxControl.hpp"
#endif
#ifndef TWC_COLORRGBAEDIT_HPP
#include "ToolUIWx/twc/twcColorRGBAEdit.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class pwxColor_ColorRGBAEdit : public pwxControl
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pwxColor_ColorRGBAEdit(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							wxWindow* i_pParent);

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		~pwxColor_ColorRGBAEdit();

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
		void ColorRGBAEdit_ValueChanged(wxCommandEvent& i_Event);

		twcColorRGBAEdit* m_pActualControl;
};

#endif // USE_WXWIDGETS
