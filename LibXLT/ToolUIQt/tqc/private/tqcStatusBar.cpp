/*****************************************************************************
**	tqcStatusBar.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcStatusBar.hpp"

#ifdef USE_QT

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqcStatusBar::tqcStatusBar(QWidget* i_pParent)
: QStatusBar(i_pParent)
{
	this->setSizeGripEnabled(true);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcStatusBar::SetFields(int i_NumFields, const int* i_pWidths)
{
	for (size_t i = 0; i < m_Fields.size(); ++i)
	{
		this->removeWidget(m_Fields[i]);
		delete m_Fields[i];
	}
	m_Fields.clear();

	for (int i = 0; i < i_NumFields; ++i)
	{
		QLabel* pField = new QLabel(this);
		pField->setFrameShape(QFrame::StyledPanel);
		pField->setFrameShadow(QFrame::Sunken);
		pField->setTextFormat(Qt::PlainText);

		int width = (i_pWidths) ? i_pWidths[i] : -1;
		if (width > 0)
		{
			pField->setFixedWidth(width);
			this->addPermanentWidget(pField, 0);
		}
		else
		{
			pField->setMinimumWidth(20);
			this->addPermanentWidget(pField, 1);
		}
		m_Fields.push_back(pField);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int tqcStatusBar::GetFieldsCount() const
{
	return (int)m_Fields.size();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcStatusBar::SetStatusText(const QString& i_Text, int i_Field)
{
	if (QLabel* pField = GetField(i_Field))
	{
		if (pField->text() != i_Text)
			pField->setText(i_Text);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
QString tqcStatusBar::GetStatusText(int i_Field) const
{
	QLabel* pField = GetField(i_Field);
	return pField ? pField->text() : QString();
}

//--------------------------------------------------------------------
// Set the tool tip for a given panel by index
//--------------------------------------------------------------------
void tqcStatusBar::SetPanelToolTip(int i_Panel, const std::string& i_String)
{
	if (QLabel* pField = GetField(i_Panel))
		pField->setToolTip(tqtQString(i_String));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
QLabel* tqcStatusBar::GetField(int i_Field) const
{
	if (i_Field >= 0 && i_Field < (int)m_Fields.size())
		return m_Fields[i_Field];
	return NULL;
}

#endif // USE_QT
