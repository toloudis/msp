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

//	needed before App so USE_QT is set
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#include <string>


#ifdef USE_QT

//============================================================================
// Class brshColorDialog - pane with the paint brush properties
//============================================================================
class brshColorDialog : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static brshColorDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		brshColorDialog( QWidget* parent,
						  const char* i_Title = "Paint",
						  const char* i_Caption = "Paint" );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~brshColorDialog();

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void closeEvent( QCloseEvent* event ) override;
};

#endif
