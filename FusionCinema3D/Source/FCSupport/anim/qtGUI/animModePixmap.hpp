/*****************************************************************************
**	animModePixmapItem.hpp
**
**		a pixmap item that will only be used in the mode panel
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef ANIM_MODEPIXMAP_HPP
#error animModePixmap.hpp multiply included
#endif
#define ANIM_MODEPIXMAP_HPP

#ifndef ANIM_PIXMAPITEM_HPP
#include "FCSupport/anim/qtGUI/animPixmapItem.hpp"
#endif


//============================================================================
//============================================================================
class animModePixmap : public animPixmapItem
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	animModePixmap(fcuiPanel* i_pParentPanel, int i_ModeLevel, double i_TileWidth, double i_TileHeight);
	~animModePixmap();

	//------------------------------------------------------------------------
	/// Checks if the item should be enabled, if not, it will disable it
	/// and grey it out.  The item will not respond to button clicks
	//------------------------------------------------------------------------
	void PerformEnableCheck();

	//------------------------------------------------------------------------
	/// paint overrides parent paint function and currently sets a border 
	/// around the pixmap image
	//------------------------------------------------------------------------
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
	// Virtual method overrides
 	void hoverEnterEvent(QGraphicsSceneHoverEvent  *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent  *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	int m_zBackup;
	qreal m_xPosDown;
	qreal m_yPosDown;
	bool m_bDragDrop;
};  //end class animModePixmap