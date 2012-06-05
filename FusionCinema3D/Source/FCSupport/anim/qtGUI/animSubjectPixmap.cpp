/*****************************************************************************
**	animSubjectPixmapItem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/anim/qtGUI/animSubjectPixmap.hpp"

#include "FCSupport/fcui/qtGUI/fcuiPanel.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include <QtGui/QtGui>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
animSubjectPixmap::animSubjectPixmap(fcuiPanel* i_pParentPanel, int i_ModeLevel, double i_TileWidth, double i_TileHeight)
:  animPixmapItem(i_pParentPanel, i_ModeLevel, i_TileWidth, i_TileHeight)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
animSubjectPixmap::~animSubjectPixmap()
{}

//------------------------------------------------------------------------
/// Checks if the item should be enabled, if not, it will disable it
/// and grey it out.  The item will not respond to button clicks
//------------------------------------------------------------------------
void animSubjectPixmap::PerformEnableCheck()
{}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animSubjectPixmap::hoverEnterEvent(QGraphicsSceneHoverEvent   *event)
{
	if(!IsSelected())
		HighlightImage(200, 0.6f);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animSubjectPixmap::hoverLeaveEvent(QGraphicsSceneHoverEvent  *event)
{
	if(!IsSelected())
		AnimRemoveHighlightImage(500, 0.6f);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animSubjectPixmap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	animPixmapItem::mousePressEvent(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animSubjectPixmap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	animPixmapItem::mouseMoveEvent(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animSubjectPixmap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	animPixmapItem::mouseReleaseEvent(event);
}

//------------------------------------------------------------------------
/// paint overrides parent paint function and currently sets a border 
/// around the pixmap image
//------------------------------------------------------------------------
void animSubjectPixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	animPixmapItem::paint(painter, option, widget);
}