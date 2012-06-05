/*****************************************************************************
**  wxMainDropTarget.hpp
**
**     Drop target for receiving drag and drop into render panel
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef WX_MAINDROPTARGET_HPP
#error wxMainDropTarget.hpp multiply included
#endif
#define WX_MAINDROPTARGET_HPP


#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//============================================================================
//============================================================================
class tma3dRenderView;


#ifdef USE_WXWIDGETS
#include <wx/dnd.h>


//============================================================================
// Class wxMainDropTarget
//============================================================================
class wxMainDropTarget : public wxFileDropTarget
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		wxMainDropTarget(tma3dRenderView *i_pRenderView);
		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~wxMainDropTarget();

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual bool OnDropFiles(wxCoord x, wxCoord y, 
								 const wxArrayString& filenames);

		tma3dRenderView *m_pRenderView;
	
};

#endif // USE_WXWIDGETS

