/*****************************************************************************
**	brshColorDialog.hpp
**
**		implementation of the brushstroke color picking dialog
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef BRSH_COLORDIALOG_HPP
#error brshColorDialog.hpp multiply included
#endif
#define BRSH_COLORDIALOG_HPP

//	needed before App so USE_WXWIDGETS is set
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//	base dialog
#ifdef USE_WXWIDGETS
#include "Support/brsh/wxGUI/brshColorDialogBase.h"
#endif

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#include <string>


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// class forwards
//----------------------------------------------------------------------------
class fsLocator;

//============================================================================
// Class RenderStatsDialog
//============================================================================
class brshColorDialog : public brshColorDialogBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static brshColorDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		brshColorDialog( wxWindow* parent, 
						  const wxString& i_Title = L"Paint", 
						  const wxString& i_Caption = L"Paint" );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~brshColorDialog();

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void brshColorDialog_onClose( wxCloseEvent& event );
	
		// any class wishing to process wxWidgets events must use this macro
	    DECLARE_EVENT_TABLE()
};

#endif