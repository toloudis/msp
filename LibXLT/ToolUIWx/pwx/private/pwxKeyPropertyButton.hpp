/****************************************************************************\
**	pwxKeyPropertyButton.hpp
**
**		Label that expands and compresses the properties beneath it.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_KEYPROPERTYBUTTON_HPP
#error pwxKeyPropertyButton.hpp multiply included
#endif
#define PWX_KEYPROPERTYBUTTON_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class pwxKeyPropertyButton : public wxBitmapButton
{
	public:
		//--------------------------------------------------------------------
		// Function signature as callback when the "Key" button
		// next to a property is pressed.
		//--------------------------------------------------------------------
		typedef void (*KeyPropertyFunction)(const std::string& /*i_PropertyName*/);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pwxKeyPropertyButton(wxWindow* i_pParent, 
							 const std::string& i_PropertyName,
							 KeyPropertyFunction i_KeyPropertyFunction);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void DoKeyProperty(wxCommandEvent &i_Event);

		std::string m_PropertyName;
		KeyPropertyFunction m_KeyPropertyFunction;

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
