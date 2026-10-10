/*****************************************************************************
**	tqcStatusBar.hpp
**
**		Status bar divided into fields (like wxStatusBar), with a tool tip
**	for each field.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_STATUSBAR_HPP
#error tqcStatusBar.hpp multiply included
#endif
#define TQC_STATUSBAR_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <vector>
#include <string>

#ifdef USE_QT

//============================================================================
//============================================================================
class tqcStatusBar : public QStatusBar
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcStatusBar(QWidget* i_pParent);

		//--------------------------------------------------------------------
		// Create the fields. A width of -1 makes a field share the space
		//	left over by the fixed width fields.
		//--------------------------------------------------------------------
		void SetFields(int i_NumFields, const int* i_pWidths);
		int GetFieldsCount() const;

		//--------------------------------------------------------------------
		// Text and tool tip of a field
		//--------------------------------------------------------------------
		void SetStatusText(const QString& i_Text, int i_Field);
		QString GetStatusText(int i_Field) const;
		void SetPanelToolTip(int i_Panel, const std::string& i_String);

		//--------------------------------------------------------------------
		// The label widget showing a field
		//--------------------------------------------------------------------
		QLabel* GetField(int i_Field) const;

	private:
		std::vector<QLabel*> m_Fields;
};

#endif // USE_QT
