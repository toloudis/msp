/*****************************************************************************
**	animElementPixmapItem.hpp
**
**		a pixmap item that will only be used in the element panel
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef ANIM_ELEMENTPIXMAP_HPP
#error animElementPixmap.hpp multiply included
#endif
#define ANIM_ELEMENTPIXMAP_HPP

#ifndef ANIM_PIXMAPITEM_HPP
#include "FCSupport/anim/qtGUI/animPixmapItem.hpp"
#endif


//============================================================================
//============================================================================
class animElementPixmap : public animPixmapItem
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	animElementPixmap(fcuiPanel* i_pParentPanel, int i_ModeLevel, double i_TileWidth, double i_TileHeight);
	~animElementPixmap();

	//------------------------------------------------------------------------
	/// Highlight the image
	//------------------------------------------------------------------------
	void HighlightImage(int i_Duration = 100, float i_EndStrength = 1.0f);
	void HighlightBorder(int i_Duration = 100, float i_EndStrength = 1.0f);

	//------------------------------------------------------------------------
	/// Use an animation to remove the highlight
	//------------------------------------------------------------------------
	void AnimRemoveHighlightImage(int i_Duration = 100, float i_StartStrength = 1.0f);

	//------------------------------------------------------------------------
	/// Remove any color effect placed on the image
	//------------------------------------------------------------------------
	void RemoveHighlight();

	//------------------------------------------------------------------------
	/// If there is some special operation to do when the pixmap is selected,
	/// do that here
	//------------------------------------------------------------------------
	void DoSelection();

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

	//------------------------------------------------------------------------
	/// Update the tooltip for the element when the locator is updated
	//------------------------------------------------------------------------
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
	QGraphicsPixmapItem* m_pBorderImage;
};  //end class animElementPixmap