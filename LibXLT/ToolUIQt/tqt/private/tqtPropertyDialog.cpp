/*****************************************************************************
**	tqtPropertyDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtPropertyDialog.hpp"
#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"

#ifdef USE_QT

namespace
{
	// The property panel grows to show this much before it scrolls
	const int c_MaxPropertyPanelHeight = 400;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqtPropertyDialog::tqtPropertyDialog( QWidget* parent,
							const QString& i_Title,
							const prtyPropertyUIInfoContainer& i_PropertyContainer,
							const char* i_Message )
:	tqtPropertyDialogBase( parent, i_Title )
{
	if (i_Message && *i_Message)
	{
		m_staticText_Message->setText(tqtQString(i_Message));
		m_staticText_Message->show();
	}

	std::string dialog_name("PropertyDialog"); // not needed, not using categories
	pqtFormControlBuilder::BuildForm(m_panel_Properties, dialog_name, i_PropertyContainer );

	fit_to_properties();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqtPropertyDialog::tqtPropertyDialog( QWidget* parent,
					const QString& i_Title,
					std::vector<std::string>& i_RowNames,
					std::vector<std::string>& i_ColumnNames,
					std::vector<prtyObject*>& i_Rows)
: tqtPropertyDialogBase( parent, i_Title )
{
	pqtFormControlBuilder::BuildGridForm(m_panel_Properties, i_RowNames, i_ColumnNames, i_Rows );

	fit_to_properties();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqtPropertyDialog::~tqtPropertyDialog()
{
	pqtFormControlBuilder::ClearForm(m_panel_Properties);
}

//--------------------------------------------------------------------
// Make sure the properties panel is big enough to see some properties,
// or all of them if just a few.
//--------------------------------------------------------------------
void tqtPropertyDialog::fit_to_properties()
{
	QSize hint = m_panel_Properties->sizeHint();
	int height = (hint.height() < c_MaxPropertyPanelHeight) ? hint.height() : c_MaxPropertyPanelHeight;
	m_scroll_Properties->setMinimumHeight(height + 2*m_scroll_Properties->frameWidth());
	m_scroll_Properties->setMinimumWidth(hint.width() +
		m_scroll_Properties->verticalScrollBar()->sizeHint().width() +
		2*m_scroll_Properties->frameWidth());
	this->adjustSize();
}

#endif // USE_QT
