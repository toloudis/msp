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

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

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
		void UpdateStatString( itString& i_pString );
		void AddStatString( itString& i_pString );
		void Clear();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cptrRenderStatsDialog( QWidget* parent );

	private:
		void append_text( const itString& i_String );

		QPlainTextEdit* m_richText_stats;
};

#endif
