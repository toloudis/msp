/*****************************************************************************
**	cptrRenderStatsDialog.hpp
**
**		Render log, in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERSTATSDIALOG_HPP
#error cptrRenderStatsDialog.hpp multiply included
#endif
#define CPTR_RENDERSTATSDIALOG_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <string>

#ifdef USE_QT

class QPlainTextEdit;

//============================================================================
// Class cptrRenderStatsDialog
//============================================================================
class cptrRenderStatsDialog : public QDialog
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void UpdateStatString( std::string& i_pString );
		void AddStatString( std::string& i_pString );
		void Clear();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrRenderStatsDialog( QWidget* parent );

	private:
		void append_text( const std::string& i_String );

		QPlainTextEdit* m_richText_stats;
};

#endif
