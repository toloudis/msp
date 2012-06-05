/*****************************************************************************
**	animElementPixmapItem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/anim/qtGUI/animElementPixmap.hpp"

#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/qtGUI/fcuiPanel.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"

#include <QtGui>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	const char* l_cBorderImage = "Media/GUI/IconBorder.png";
	QColor l_SelectHighlight(200, 15, 15, 70);
	QColor l_HoverHighlight(0, 255, 144, 70);

	//QPixmap l_BorderPixmap = QPixmap(QString("Media/GUI/IconBorder.png"));
	int l_Width, l_Height = 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
animElementPixmap::animElementPixmap(fcuiPanel* i_pParentPanel, int i_ModeLevel, double i_TileWidth, double i_TileHeight)
:  animPixmapItem(i_pParentPanel, i_ModeLevel, i_TileWidth, i_TileHeight)
{
	//SetHighlightColor(l_Highlight);
	SetHasLabel(true);
	m_pBorderImage = new QGraphicsPixmapItem(this);
	l_Width = i_pParentPanel->GetImageWidth();
	l_Height = i_pParentPanel->GetImageHeight();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
animElementPixmap::~animElementPixmap()
{
	if(m_pBorderImage != NULL)
	{
		delete m_pBorderImage;
		m_pBorderImage = NULL;
	}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animElementPixmap::HighlightImage(int i_Duration, float i_EndStrength)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animElementPixmap::HighlightBorder(int i_Duration, float i_EndStrength)
{
	QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect();
	effect->setColor(GetHighlightColor());
	effect->setStrength(0.0);
	m_pBorderImage->setGraphicsEffect(effect);

	QPropertyAnimation *animation = new QPropertyAnimation(effect, "strength");

	//set the necessary animation parameters
	animation->setDuration(i_Duration);     
	animation->setStartValue(0);
	animation->setEndValue(i_EndStrength);
	animation->start(QPropertyAnimation::DeleteWhenStopped);
}

//------------------------------------------------------------------------
/// Use an animation to remove the highlight
//------------------------------------------------------------------------
void animElementPixmap::AnimRemoveHighlightImage(int i_Duration, float i_StartStrength)
{
	QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect();
	effect->setColor(GetHighlightColor());
	effect->setStrength(i_StartStrength);
	m_pBorderImage->setGraphicsEffect(effect);

	QPropertyAnimation *animation = new QPropertyAnimation(effect, "strength");

	//set the necessary animation parameters
	animation->setDuration(i_Duration);     
	animation->setStartValue(i_StartStrength);
	animation->setEndValue(0.0);
	animation->start(QPropertyAnimation::DeleteWhenStopped);
	QObject::connect(animation, SIGNAL(finished()), this, SLOT(clean_highlight()));
}

//------------------------------------------------------------------------
/// Remove any color effect placed on the image
//------------------------------------------------------------------------
void animElementPixmap::RemoveHighlight()
{
	//in case we are removing a selected highlight, reset the hover highlight color
	if(IsSelected())
	{
		AnimRemoveHighlightImage(500, 0.7f);
		SetSelected(false);
		SetHighlightColor(l_HoverHighlight);

	}

	m_pBorderImage->setGraphicsEffect(NULL);
}

//----------------------------------------------------------------------------
/// If there is some special operation to do when the pixmap is selected,
/// do that here
//----------------------------------------------------------------------------
void animElementPixmap::DoSelection()
{
	SetSelected(true);
	SetHighlightColor(l_SelectHighlight);
	HighlightBorder(200, 0.7f);
}

//------------------------------------------------------------------------
/// Checks if the item should be enabled, if not, it will disable it
/// and grey it out.  The item will not respond to button clicks
//------------------------------------------------------------------------
void animElementPixmap::PerformEnableCheck()
{}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animElementPixmap::hoverEnterEvent(QGraphicsSceneHoverEvent   *event)
{
	if(!IsSelected())
	{
		HighlightBorder(200, 0.8f);
	}
	animPixmapItem::hoverEnterEvent(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animElementPixmap::hoverLeaveEvent(QGraphicsSceneHoverEvent  *event)
{
	if(!IsSelected())
	{
		AnimRemoveHighlightImage(500, 0.8f);
	}
	animPixmapItem::hoverLeaveEvent(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animElementPixmap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	event->accept();
	m_zBackup = zValue();
	setZValue(100);

	m_xPosDown = event->pos().x();
	m_yPosDown = event->pos().y();
	m_bDragDrop = false;

	if(fcmdModeMgr::Instance != NULL)
		if(fcmdModeMgr::Instance->GetCanDrag() || fcmdModeMgr::Instance->GetCanCheck())
			fcmdModeMgr::Instance->SelectElement(GetLocator());
}


#define DRAGDROP_DELTA 10.0

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animElementPixmap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if(fcmdModeMgr::Instance != NULL)
	{
		if(!fcmdModeMgr::Instance->GetCanDrag())
			return;
	}

	event->accept();

	if (fabs(m_xPosDown-event->pos().x()) > DRAGDROP_DELTA || fabs(m_yPosDown-event->pos().y()) > DRAGDROP_DELTA)
	{
		setZValue(m_zBackup);
		SetScale(1.0);
		QTransform transform;
		transform = transform.scale(GetScale(), GetScale());
		setTransform(transform);
		setScale(1.0);
		update();

		m_bDragDrop = true;

		// Drag
		QDrag *drag = new QDrag(GetParent());
		QMimeData *mimeData = new QMimeData();
		QList<QUrl> list;
		
		QByteArray itemData;
		QByteArray locatorData;

		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		dataStream << pixmap();
		mimeData->setData("application/x-dnditemdata", itemData);
		
		QDataStream locatorStream(&locatorData, QIODevice::WriteOnly);
		itString locator_string;
		fsFileUtil::LocatorToUnicodeString(GetLocator(), locator_string);
		
		//std::wstring w_loc = locator_string.GetString();
		QString qs_locator((const QChar *)(locator_string.GetString()), locator_string.GetLength());
		locatorStream << qs_locator;
		mimeData->setData("application/x-fslocatordata", locatorData);
		
		drag->setMimeData(mimeData);

		drag->setPixmap(pixmap());
		drag->setHotSpot(QPoint(drag->pixmap().width()/2, drag->pixmap().height()/2));
		// start drag - do this at the end
		drag->exec(Qt::CopyAction);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animElementPixmap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (!m_bDragDrop)
	{
		event->accept();
		QRectF b = boundingRect();
		if (b.contains(event->pos()))
		{
			//StartAnimScale();
			Shake();
		}
		animPixmapItem::mouseReleaseEvent(event);
	}
}

//------------------------------------------------------------------------
/// paint overrides parent paint function and currently sets a border 
/// around the pixmap image
//------------------------------------------------------------------------
void animElementPixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	animPixmapItem::paint(painter, option, widget);
	//m_pBorderImage->paint(painter, option, widget); 
	/*QPen saved = painter->pen();
	painter->setPen(QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::BevelJoin));
	painter->drawRoundedRect( boundingRect(), 5, 5);
	painter->setPen(saved);*/
}

//------------------------------------------------------------------------
/// Overrides the set pixmap function to set up a composite of the image and
/// the image border
//------------------------------------------------------------------------
void animElementPixmap::SetPixmap(const QPixmap &pixmap)
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

//------------------------------------------------------------------------
/// Update the tooltip for the element when the locator is updated
//------------------------------------------------------------------------
void animElementPixmap::SetLocator(const fsLocator& i_Locator)
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