/*****************************************************************************
**	animMoviePixmap.hpp
**
**		a pixmap item that will only be used in the movie pack panel
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef ANIM_MOVIEPIXMAP_HPP
#error animMoviePixmap.hpp multiply included
#endif
#define ANIM_MOVIEPIXMAP_HPP

#ifndef ANIM_PIXMAPITEM_HPP
#include "FCSupport/anim/qtGUI/animPixmapItem.hpp"
#endif


//============================================================================
//============================================================================
class animMoviePixmap : public animPixmapItem
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	animMoviePixmap(fcuiPanel* i_pParentPanel, int i_ModeLevel, double i_TileWidth, double i_TileHeight);
	~animMoviePixmap();

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
	
	//------------------------------------------------------------------------
	/// Overrides the set pixmap function to set up a composite of the image and
	/// the image border
	//------------------------------------------------------------------------
	void SetPixmap(const QPixmap &pixmap);

private:
	// Virtual method overrides
 	void hoverEnterEvent(QGraphicsSceneHoverEvent  *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent  *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool m_bEnabled;
	QGraphicsPixmapItem* m_pBorderImage;

};  //end class animMoviePixmap