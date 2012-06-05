/*****************************************************************************
**	animMoviePixmapItem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/anim/qtGUI/animMoviePixmap.hpp"

#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcui/qtGUI/fcuiPanel.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include <QtGui/QtGui>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	QColor l_Highlight(0, 255, 255, 70);
	QColor l_DisabledHighlight(192, 192, 192, 70);
	const char* l_cBorderImage = "Media/GUI/MovieIconBorder.png";
	int l_Width, l_Height = 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
animMoviePixmap::animMoviePixmap(fcuiPanel* i_pParentPanel, int i_ModeLevel, double i_TileWidth, double i_TileHeight)
:  animPixmapItem(i_pParentPanel, i_ModeLevel, i_TileWidth, i_TileHeight),
   m_bEnabled(true)
{
	Scale(0.9f, 1);
	SetHighlightColor(l_Highlight);

	//m_pBorderImage = new animPixmapItem(i_pParentPanel, i_ModeLevel, i_TileWidth, i_TileHeight);
	m_pBorderImage = new QGraphicsPixmapItem(this);

	m_pBorderImage->setPos(m_pBorderImage->pos().x(), m_pBorderImage->pos().y() - 1);
	l_Width = i_pParentPanel->GetImageWidth() + 2;
	l_Height = i_pParentPanel->GetImageHeight() + 2;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
animMoviePixmap::~animMoviePixmap()
{
	if(m_pBorderImage != NULL)
	{
		delete m_pBorderImage;
		m_pBorderImage = NULL;
	}
}

//------------------------------------------------------------------------
/// Checks if the item should be enabled, if not, it will disable it
/// and grey it out.  The item will not respond to button clicks
//------------------------------------------------------------------------
void animMoviePixmap::PerformEnableCheck()
{
	fsLocator location = GetLocator();
	if( location.GetNumNames() == 0 )
		return;

	location.Push( fcuiConstants::c_MEDIA );
	location.Push( fcuiConstants::c_MODE_LOCATION );

	if( !fsFileUtil::DirectoryExists(location) )
	{
		location.Pop();
		location.Pop();
		DBG_ERROR("Movie Pack, " << location.GetLastName() << " does not have a scene file.");
		m_bEnabled = false;
		SetHighlightColor(l_DisabledHighlight);
		HighlightImage();
	}
	else
	{
		m_bEnabled = true;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animMoviePixmap::hoverEnterEvent(QGraphicsSceneHoverEvent   *event)
{
	if(!m_bEnabled)
		return;

	animPixmapItem::hoverEnterEvent(event);
	Scale(1.0f, 200);
	HighlightImage();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animMoviePixmap::hoverLeaveEvent(QGraphicsSceneHoverEvent  *event)
{
	if(!m_bEnabled)
		return;

	animPixmapItem::hoverLeaveEvent(event);
	Scale(0.9f, 200);
	RemoveHighlight();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animMoviePixmap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if(!m_bEnabled)
		return;

	animPixmapItem::mousePressEvent(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animMoviePixmap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if(!m_bEnabled)
		return;

	animPixmapItem::mouseMoveEvent(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animMoviePixmap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if(!m_bEnabled)
		return;

	animPixmapItem::mouseReleaseEvent(event);
}

//------------------------------------------------------------------------
/// paint overrides parent paint function and currently sets a border 
/// around the pixmap image
//------------------------------------------------------------------------
void animMoviePixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	animPixmapItem::paint(painter, option, widget);
}

//------------------------------------------------------------------------
/// Overrides the set pixmap function to set up a composite of the image and
/// the image border
//------------------------------------------------------------------------
void animMoviePixmap::SetPixmap(const QPixmap &pixmap)
{
	// check if this is an empty pixmap
	if( (pixmap.height() == 0) && (pixmap.width() == 0) )
	{
		//if it is, remove the border
		m_pBorderImage->setPixmap(pixmap);
	}
	else
	{
		//create the new pixmap
		QPixmap border(l_cBorderImage);
		border = border.scaled(QSize(l_Width, l_Height), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		m_pBorderImage->setPixmap(border);
	}
	
	QGraphicsPixmapItem::setPixmap(pixmap);
}
