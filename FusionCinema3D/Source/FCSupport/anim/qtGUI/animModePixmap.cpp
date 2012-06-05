/*****************************************************************************
**	animModePixmapItem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/anim/qtGUI/animModePixmap.hpp"

#include "FCSupport/fcui/qtGUI/fcuiPanel.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include <QtGui/QtGui>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
animModePixmap::animModePixmap(fcuiPanel* i_pParentPanel, int i_ModeLevel, double i_TileWidth, double i_TileHeight)
:  animPixmapItem(i_pParentPanel, i_ModeLevel, i_TileWidth, i_TileHeight)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
animModePixmap::~animModePixmap()
{}

//------------------------------------------------------------------------
/// Checks if the item should be enabled, if not, it will disable it
/// and grey it out.  The item will not respond to button clicks
//------------------------------------------------------------------------
void animModePixmap::PerformEnableCheck()
{}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animModePixmap::hoverEnterEvent(QGraphicsSceneHoverEvent   *event)
{
	if(!IsSelected())
		HighlightImage(200, 0.6f);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animModePixmap::hoverLeaveEvent(QGraphicsSceneHoverEvent  *event)
{
	if(!IsSelected())
		AnimRemoveHighlightImage(500, 0.6f);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animModePixmap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	animPixmapItem::mousePressEvent(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animModePixmap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	animPixmapItem::mouseMoveEvent(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animModePixmap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	animPixmapItem::mouseReleaseEvent(event);
}

//------------------------------------------------------------------------
/// paint overrides parent paint function and currently sets a border 
/// around the pixmap image
//------------------------------------------------------------------------
void animModePixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	animPixmapItem::paint(painter, option, widget);
}