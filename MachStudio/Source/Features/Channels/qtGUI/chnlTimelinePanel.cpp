/****************************************************************************\
**	chnlTimelinePanel.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/qtGUI/chnlTimelinePanel.hpp"

#include "Features/Channels/qtGUI/chnlTimelineGuideUtil.hpp"

#ifdef USE_QT

#include <QMouseEvent>
#include <QPainter>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chnlTimelinePanel::chnlTimelinePanel(QWidget* i_pParent)
:	QWidget(i_pParent)
{
	// Light grey behind the channels
	this->setAutoFillBackground(true);
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor(192, 192, 192));
	this->setPalette(palette);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimelinePanel::SetMouseCallbacks(MouseFunction i_Down, MouseFunction i_Move, MouseFunction i_Up)
{
	m_MouseDown = i_Down;
	m_MouseMove = i_Move;
	m_MouseUp = i_Up;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimelinePanel::paintEvent(QPaintEvent*)
{
	QPainter dc(this);
	chnlTimelineGuideUtil::PaintGuides(dc, this->height());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimelinePanel::mousePressEvent(QMouseEvent* i_pEvent)
{
	if (m_MouseDown && i_pEvent->button() == Qt::LeftButton)
		m_MouseDown(i_pEvent);
	else
		QWidget::mousePressEvent(i_pEvent);
}

void chnlTimelinePanel::mouseMoveEvent(QMouseEvent* i_pEvent)
{
	if (m_MouseMove)
		m_MouseMove(i_pEvent);
	else
		QWidget::mouseMoveEvent(i_pEvent);
}

void chnlTimelinePanel::mouseReleaseEvent(QMouseEvent* i_pEvent)
{
	if (m_MouseUp && i_pEvent->button() == Qt::LeftButton)
		m_MouseUp(i_pEvent);
	else
		QWidget::mouseReleaseEvent(i_pEvent);
}

#endif // USE_QT
