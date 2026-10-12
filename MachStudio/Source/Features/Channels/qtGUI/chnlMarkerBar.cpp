/****************************************************************************\
**	chnlMarkerBar.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/qtGUI/chnlMarkerBar.hpp"

#include "Features/Channels/Markers/chnlMarkerOperations.hpp"
#include "Features/Channels/Notes/chnlNotesOperations.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/Ma/maTime.hpp"

#include <algorithm>

#ifdef USE_QT

#include <QContextMenuEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>

namespace
{
	//--------------------------------------------------------------------
	// Predicate for picking icons
	//--------------------------------------------------------------------
	template <class Icon>
	struct icon_pick
	{
		icon_pick(int i_X, int i_Y) : m_X(i_X), m_Y(i_Y) {}
		bool operator()(const Icon& i_Icon) const
		{
			return i_Icon.Pick(m_X, m_Y);
		}
		int m_X, m_Y;
	};
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chnlMarkerBar::chnlMarkerBar(QWidget* i_pParent)
:	QWidget(i_pParent),
	m_pContextIcon(NULL)
{
	this->setMouseTracking(true);
	this->compute_size();
}

//----------------------------------------------------------------------------
// Add marker to display
//----------------------------------------------------------------------------
void chnlMarkerBar::AddMarker(float i_Time, chnlMarkerIcon::MarkerType i_Type, const std::string& i_Note)
{
	m_Markers.push_back( chnlMarkerIcon(i_Time, i_Type, i_Note, m_MinTime, m_TimeScale) );
	m_Markers.sort();
	this->update();
}

//----------------------------------------------------------------------------
// Clear markers from display
//----------------------------------------------------------------------------
void chnlMarkerBar::ClearMarkers()
{
	m_Markers.clear();
	m_pContextIcon = NULL;
	this->update();
}

//----------------------------------------------------------------------------
// Add note to display
//----------------------------------------------------------------------------
void chnlMarkerBar::AddNote(float i_Time, chnlNoteIcon::NoteStatus i_Status, const std::string& i_Note)
{
	m_Notes.push_back( chnlNoteIcon(i_Time, i_Status, i_Note, m_MinTime, m_TimeScale) );
	m_Notes.sort();
	this->update();
}

//----------------------------------------------------------------------------
// Clear notes from display
//----------------------------------------------------------------------------
void chnlMarkerBar::ClearNotes()
{
	m_Notes.clear();
	m_pContextIcon = NULL;
	this->update();
}

//----------------------------------------------------------------------------
// Virtual function called when time range or scale has changed
//----------------------------------------------------------------------------
void chnlMarkerBar::update_size()
{
	this->compute_size();

	// Scale markers
	std::list<chnlMarkerIcon>::iterator mit;
	for (mit = m_Markers.begin(); mit != m_Markers.end(); ++mit)
	{
		mit->AlterTimeScale(m_MinTime, m_TimeScale);
	}

	// Scale notes
	std::list<chnlNoteIcon>::iterator nit;
	for (nit = m_Notes.begin(); nit != m_Notes.end(); ++nit)
	{
		nit->AlterTimeScale(m_MinTime, m_TimeScale);
	}

	this->update();
}

//----------------------------------------------------------------------------
// The bar is as wide as the timeline
//----------------------------------------------------------------------------
void chnlMarkerBar::compute_size()
{
	this->setFixedSize(this->get_full_width(), 16);
}

//----------------------------------------------------------------------------
// Pick marker or note under given position
//----------------------------------------------------------------------------
const chnlTimeIcon* chnlMarkerBar::pick_icon(int i_X, int i_Y) const
{
	// The notes are lower than the markers, so just decide
	// which list to search based on the Y value
	if (i_Y >= 8)
	{
		std::list<chnlNoteIcon>::const_iterator nit =
			std::find_if(m_Notes.begin(), m_Notes.end(), icon_pick<chnlNoteIcon>(i_X, i_Y));
		if (nit != m_Notes.end())
			return &(*nit);
	}
	else
	{
		std::list<chnlMarkerIcon>::const_iterator mit =
			std::find_if(m_Markers.begin(), m_Markers.end(), icon_pick<chnlMarkerIcon>(i_X, i_Y));
		if (mit != m_Markers.end())
			return &(*mit);
	}
	return NULL;
}

//----------------------------------------------------------------------------
// Display properties for marker or note icon.
//----------------------------------------------------------------------------
void chnlMarkerBar::show_properties(const chnlTimeIcon* i_pIcon)
{
	if (dynamic_cast<const chnlMarkerIcon*>(i_pIcon))
	{
		chnlMarkerOperations::ShowProperties(maTime::FromSeconds(i_pIcon->GetTime()));
	}
	else if (dynamic_cast<const chnlNoteIcon*>(i_pIcon))
	{
		chnlNotesOperations::ShowProperties(maTime::FromSeconds(i_pIcon->GetTime()));
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlMarkerBar::paintEvent(QPaintEvent*)
{
	QPainter dc(this);
	dc.fillRect(this->rect(), this->palette().window());

	// Outline the client rectangle (instead of using a windows border)
	dc.setPen( QPen( Qt::lightGray, 1 ) );
	dc.setBrush( Qt::NoBrush );
	dc.drawRect( 0, 0, this->width()-1, this->height()-1 );

	// Draw markers
	std::list<chnlMarkerIcon>::const_iterator mit;
	for (mit = m_Markers.begin(); mit != m_Markers.end(); ++mit)
	{
		mit->Paint(dc);
	}

	// Draw notes
	std::list<chnlNoteIcon>::const_iterator nit;
	for (nit = m_Notes.begin(); nit != m_Notes.end(); ++nit)
	{
		nit->Paint(dc);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlMarkerBar::contextMenuEvent(QContextMenuEvent* i_pEvent)
{
	// Skip, if from keyboard
	if (i_pEvent->reason() == QContextMenuEvent::Keyboard)
		return;

	m_pContextIcon = pick_icon(i_pEvent->pos().x(), i_pEvent->pos().y());
	if (m_pContextIcon)
	{
		QMenu menu(this);
		QAction* pShow = menu.addAction("Show &Properties");
		if (menu.exec(i_pEvent->globalPos()) == pShow && m_pContextIcon)
			show_properties(m_pContextIcon);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlMarkerBar::mouseDoubleClickEvent(QMouseEvent* i_pEvent)
{
	const QPoint pos = i_pEvent->position().toPoint();
	const chnlTimeIcon* pIcon = pick_icon(pos.x(), pos.y());
	if (pIcon)
	{
		m_pContextIcon = pIcon;
		show_properties(m_pContextIcon);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlMarkerBar::mouseMoveEvent(QMouseEvent* i_pEvent)
{
	const QPoint pos = i_pEvent->position().toPoint();
	const chnlTimeIcon* pIcon = pick_icon(pos.x(), pos.y());
	this->setToolTip( (pIcon) ? tqtQString(pIcon->GetHoverDescription()) : QString() );
	QWidget::mouseMoveEvent(i_pEvent);
}

#endif // USE_QT
