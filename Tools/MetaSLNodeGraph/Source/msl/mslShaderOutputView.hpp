/*****************************************************************************
**  mslShaderOutputView.hpp
**
**    Read only textbox in wxWidgets for displaying the output of the 
**	compiler shader.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_SHADEROUTPUTVIEW_HPP
#error mslShaderOutputView.hpp multiply included
#endif
#define MSL_SHADEROUTPUTVIEW_HPP

#ifndef MSL_DEFS_HPP
#include "mslDefs.hpp"
#endif
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#ifdef USE_WXWIDGETS

#include <wx/richtext/richtextctrl.h>

//============================================================================
//============================================================================
class mslShaderOutputView : public wxPanel
{
public:
    // ctor(s)
    mslShaderOutputView(wxWindow* parent);
	~mslShaderOutputView();

	//--------------------------------------------------------------------
	// Clear the text box 
	//--------------------------------------------------------------------
	void ClearShaderString();

	//--------------------------------------------------------------------
	// Set the text box to contain the given shader string
	//--------------------------------------------------------------------
	void UpdateShaderString(const std::string& i_String );

	//--------------------------------------------------------------------
	// Move cursor to the given line number
	//--------------------------------------------------------------------
	void MoveToLine(int i_LineNum);

private:
	wxRichTextCtrl* m_richText_Console;
	

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
