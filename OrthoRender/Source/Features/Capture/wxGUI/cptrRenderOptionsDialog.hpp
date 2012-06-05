/*****************************************************************************
**	cptrRenderOptionsDialog.hpp
**
**		implementation of the RenderOptions dialog
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDEROPTIONSDIALOG_HPP
#error cptrRenderOptionsDialog.hpp multiply included
#endif
#define CPTR_RENDEROPTIONSDIALOG_HPP

//	needed before App so USE_WXWIDGETS is set
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//	base dialog
#ifdef USE_WXWIDGETS
#include "Features/Capture/wxGUI/cptrRenderOptionsDialogBase.h"
#endif


#ifdef USE_WXWIDGETS

//============================================================================
// Class RenderOptionsDialog
//============================================================================
class cptrRenderOptionsDialog : public cptrRenderOptionsDialogBase
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//static cptrRenderOptionsDialog* DialogInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrRenderOptionsDialog( wxWindow* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//~cptrRenderOptionsDialog();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void cptrRenderOptionsDialog_OnClose( wxCloseEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void button_Render_OnButtonClick( wxCommandEvent& event );
};

#endif