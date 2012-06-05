/*****************************************************************************
**  wxPropertyPanel.hpp
**
**    Panel for properties of generated material in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef WX_PROPERTYPANEL_HPP
#error wxPropertyPanel.hpp multiply included
#endif
#define WX_PROPERTYPANEL_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

class prtyPropertyUIInfoContainer;

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class wxPropertyPanel : public wxScrolledWindow
{
public:
	static wxPropertyPanel* DialogInstance;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
    wxPropertyPanel(wxWindow* parent);
	
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~wxPropertyPanel();

	//--------------------------------------------------------------------
	// Clear display
	//--------------------------------------------------------------------
	void ClearProperties();

	//--------------------------------------------------------------------
	// Set property object to display
	//--------------------------------------------------------------------
	void SetProperties(const prtyPropertyUIInfoContainer& i_PropertyContainer);

private:
    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
