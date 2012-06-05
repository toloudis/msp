/*****************************************************************************
**  prefsLayoutChoice.hpp
**
**     Choice box displaying current and available cameras and
**	director's cuts
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef PREFS_LAYOUTCHOICE_HPP
#error prefsLayoutChoice.hpp multiply included
#endif
#define PREFS_LAYOUTCHOICE_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS


//============================================================================
// Derive a new class in order to handle callbacks when layout
// choice changes.
//============================================================================
class prefsLayoutChoice : public wxChoice
{
public:
	//--------------------------------------------------------------------
	//	Static pointer to the instance of the form, will be cleared
	//	when the object dialog is deleted.
	//--------------------------------------------------------------------
	static prefsLayoutChoice* Instance;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
    prefsLayoutChoice(wxWindow* parent);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
    ~prefsLayoutChoice();
		
	//------------------------------------------------------------------------
	// Set list of camera names
	//------------------------------------------------------------------------
	void Update();

private:
	//------------------------------------------------------------------------
	// event callbacks
	//------------------------------------------------------------------------
	void SelectedIndexChanged(wxCommandEvent& i_Event);

	bool m_bDisableNotify;
};

#endif // USE_WXWIDGETS
