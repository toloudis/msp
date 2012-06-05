/*****************************************************************************
**	cptrRenderStatsDialog.hpp
**
**		implementation of the RenderStats dialog
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERSTATSDIALOG_HPP
#error cptrRenderStatsDialog.hpp multiply included
#endif
#define CPTR_RENDERSTATSDIALOG_HPP

//	needed before App so USE_WXWIDGETS is set
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//	base dialog
#ifdef USE_WXWIDGETS
#include "Features/Capture/wxGUI/cptrRenderStatsDialogBase.h"
#endif

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif



#ifdef USE_WXWIDGETS

//============================================================================
// Class RenderStatsDialog
//============================================================================
class cptrRenderStatsDialog : public cptrRenderStatsDialogBase
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void UpdateStatString( itString& i_pString );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AddStatString( itString& i_pString );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Clear();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrRenderStatsDialog( wxWindow* parent );

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void RenderStatsDialog_OnActivate( wxActivateEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//~cptrRenderStatsDialog();
};

#endif