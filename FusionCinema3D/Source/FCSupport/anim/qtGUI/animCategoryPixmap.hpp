/*****************************************************************************
**	animCategoryPixmapItem.hpp
**
**		a pixmap item that will only be used in the element panel
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef ANIM_CATEGORYPIXMAP_HPP
#error animCategoryPixmap.hpp multiply included
#endif
#define ANIM_CATEGORYPIXMAP_HPP

#ifndef ANIM_PIXMAPITEM_HPP
#include "FCSupport/anim/qtGUI/animPixmapItem.hpp"
#endif


//============================================================================
//============================================================================
class animCategoryPixmap : public animPixmapItem
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	animCategoryPixmap(fcuiPanel* i_pParentPanel, int i_ModeLevel, double i_TileWidth, double i_TileHeight);
	~animCategoryPixmap();

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

	void SetLocator(const fsLocator& i_Locator);

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
};  //end class animCategoryPixmap