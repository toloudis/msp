/*****************************************************************************
**	cptrRenderOptionsDialog.hpp
**
**		Render output options and render layers, in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDEROPTIONSDIALOG_HPP
#error cptrRenderOptionsDialog.hpp multiply included
#endif
#define CPTR_RENDEROPTIONSDIALOG_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

//============================================================================
// Class cptrRenderOptionsDialog
//============================================================================
class cptrRenderOptionsDialog : public QDialog
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrRenderOptionsDialog( QWidget* parent );

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void closeEvent( QCloseEvent* i_pEvent ) override;

	private:
		void button_Render_OnButtonClick();
};

#endif
