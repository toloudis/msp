/*****************************************************************************
**	animCategoryPixmapItem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/anim/qtGUI/animCategoryPixmap.hpp"

#include "FCSupport/fcui/qtGUI/fcuiPanel.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"

#include <QtGui/QtGui>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
animCategoryPixmap::animCategoryPixmap(fcuiPanel* i_pParentPanel, int i_ModeLevel, double i_TileWidth, double i_TileHeight)
:  animPixmapItem(i_pParentPanel, i_ModeLevel, i_TileWidth, i_TileHeight)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
animCategoryPixmap::~animCategoryPixmap()
{}

//------------------------------------------------------------------------
/// Checks if the item should be enabled, if not, it will disable it
/// and grey it out.  The item will not respond to button clicks
//------------------------------------------------------------------------
void animCategoryPixmap::PerformEnableCheck()
{}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animCategoryPixmap::hoverEnterEvent(QGraphicsSceneHoverEvent   *event)
{
	if(!IsSelected())
		HighlightImage(200, 0.6f);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animCategoryPixmap::hoverLeaveEvent(QGraphicsSceneHoverEvent  *event)
{
	if(!IsSelected())
		AnimRemoveHighlightImage(500, 0.6f);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animCategoryPixmap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	animPixmapItem::mousePressEvent(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animCategoryPixmap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	animPixmapItem::mouseMoveEvent(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animCategoryPixmap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	animPixmapItem::mouseReleaseEvent(event);
}

//------------------------------------------------------------------------
/// paint overrides parent paint function and currently sets a border 
/// around the pixmap image
//------------------------------------------------------------------------
void animCategoryPixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	animPixmapItem::paint(painter, option, widget);
}

//------------------------------------------------------------------------
/// Update the tooltip for the element when the locator is updated
//------------------------------------------------------------------------
void animCategoryPixmap::SetLocator(const fsLocator& i_Locator)
{
	itString name("");
	itString ele_name("");
	QString tip;
	if(i_Locator.GetNumNames() > 0)
		name = i_Locator.GetLastName();
	name.GetExtension(ele_name);
	if(ele_name != itString(""))
		 tip = itStringUtil::GetStdString(ele_name).c_str();
	else 
		 tip = itStringUtil::GetStdString(name).c_str();
	setToolTip(tip);
	animPixmapItem::SetLocator(i_Locator);
}