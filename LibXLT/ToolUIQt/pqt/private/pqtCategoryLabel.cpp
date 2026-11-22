/****************************************************************************\
**	pqtCategoryLabel.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/private/pqtCategoryLabel.hpp"

#include <map>
#include <set>

#ifdef USE_QT

namespace
{
	// This map keeps track of which category names have been collpased
	// by the user in order to create them in the same state the next time.
	// The primary mapping is from DialogName to a Set of Category Names that
	// have been collapsed.
	std::map< std::string, std::set< QString > > l_CollapsedStates;

	void set_collapsed_state(const std::string& i_DialogName,
							 const QString &i_CategoryName,
							 bool i_bCollapsed)
	{
		if (i_bCollapsed)
			l_CollapsedStates[i_DialogName].insert(i_CategoryName);
		else
			l_CollapsedStates[i_DialogName].erase(i_CategoryName);
	}

	bool get_collapsed_state(const std::string& i_DialogName,
							 const QString &i_CategoryName)
	{
		std::set< QString > &collapsed_states = l_CollapsedStates[i_DialogName];
		return (collapsed_states.find(i_CategoryName) != collapsed_states.end());
	}

	// Size of the expanded/collapsed box
	const int c_IconSize = 7;
	// Where the text starts
	const int c_TextOffset = 12;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pqtCategoryLabel::pqtCategoryLabel(QWidget* i_pParent,
						 const std::string& i_DialogName,
						 const QString& i_LabelText)
:	QWidget(i_pParent),
	m_DialogName(i_DialogName),
	m_LabelText(i_LabelText),
	m_TextColor(palette().color(QPalette::HighlightedText)),
	m_BackgroundColor(palette().color(QPalette::Highlight))
{
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	this->setCursor(Qt::PointingHandCursor);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtCategoryLabel::SetGroup(QWidget* i_pGroup)
{
	m_pGroup = i_pGroup;
	update();
}

//----------------------------------------------------------------------------
// Return true if this category label should be initially collapsed
// based on what the user has done previously.
//----------------------------------------------------------------------------
bool pqtCategoryLabel::GetInitialCollapsedState() const
{
	return get_collapsed_state(m_DialogName, m_LabelText);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtCategoryLabel::SetColors(const QColor& i_Text, const QColor& i_Background)
{
	m_TextColor = i_Text;
	m_BackgroundColor = i_Background;
	update();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
QSize pqtCategoryLabel::sizeHint() const
{
	QFontMetrics metrics(this->font());
	int height = metrics.height() + 2;
	// icon needs at least 9 in height
	if (height < 9)
		height = 9;
	return QSize(c_TextOffset + metrics.horizontalAdvance(m_LabelText) + 4, height);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtCategoryLabel::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), m_BackgroundColor);

	// Expanded, compressed state
	int y = (this->height() - c_IconSize) / 2;
	painter.setPen(QPen(m_TextColor, 1));
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(2, y, c_IconSize, c_IconSize);
	int half_size = c_IconSize / 2;
	painter.drawLine(2, y+half_size, 2+c_IconSize, y+half_size);
	const bool bIsShown = m_pGroup ? !m_pGroup->isHidden() : true;
	if (!bIsShown)
		painter.drawLine(2+half_size, y, 2+half_size, y+c_IconSize);

	// Category Text
	painter.setFont(this->font());
	painter.drawText(QRect(c_TextOffset, 0, this->width() - c_TextOffset, this->height()),
					 Qt::AlignLeft | Qt::AlignVCenter, m_LabelText);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void pqtCategoryLabel::mousePressEvent(QMouseEvent* i_pEvent)
{
	if (m_pGroup && i_pEvent->button() == Qt::LeftButton)
	{
		// Toggle visiblity of the group
		bool bIsShown = !m_pGroup->isHidden();
		m_pGroup->setVisible(!bIsShown);
		set_collapsed_state(m_DialogName, m_LabelText, bIsShown); // Collapsed is true, when bIsShown was true
		update();
	}
}

#endif // USE_QT
