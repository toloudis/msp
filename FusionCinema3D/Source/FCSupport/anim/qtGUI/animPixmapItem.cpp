/*****************************************************************************
**	animPixmapItem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/anim/qtGUI/animPixmapItem.hpp"

#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/qtGUI/fcuiPanel.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"

#include <QtGui/QtGui>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
animPixmapItem::animPixmapItem(fcuiPanel* i_pParentPanel, int i_ModeLevel, double i_TileWidth, double i_TileHeight)
:	QGraphicsPixmapItem(), 
	m_pTimer(NULL),
	m_pParentPanel(i_pParentPanel),
	m_pRotateAnimation(NULL),
	m_pScaleAnimation(NULL),
	m_ModeLevel(i_ModeLevel),
	m_TileWidth(i_TileWidth),
	m_TileHeight(i_TileHeight),
	m_Locator(fsLocator()),
	m_pCheckbox(NULL),
	m_pLabel(NULL),
	m_ItemID(-1),
	m_bSelected(false),
	m_bHasLabel(false)
{
	m_myScale = 1.0;
	m_HighlightColor = QColor(0, 255, 144, 70);
	m_axis = Qt::YAxis;
	setAcceptHoverEvents(true);
	setTransformationMode(Qt::SmoothTransformation);
	setFlag(QGraphicsItem::ItemIsFocusable);
}

animPixmapItem::~animPixmapItem()
{
	delete m_pCheckbox;
	m_pCheckbox = NULL;

	delete m_pLabel;
	m_pLabel = NULL;
}

//----------------------------------------------------------------------------
// Perform rotate transform manipulation on the object
//----------------------------------------------------------------------------
void animPixmapItem::anim_rotate()
{
	QTransform transform ;

	//rotate the object by changing the transform
	transform = transform.translate(m_TileWidth, m_TileHeight);
	transform = transform.rotate(m_myRotate, Qt::ZAxis);
	transform = transform.translate(-m_TileWidth, -m_TileHeight);

	setTransform(transform);
	update();
}

//----------------------------------------------------------------------------
// Clean up the rotation animation pointer
//----------------------------------------------------------------------------
void animPixmapItem::clean_rotate_anim()
{
	if (m_pRotateAnimation)
	{
		m_pRotateAnimation->stop();
		delete m_pRotateAnimation;
		m_pRotateAnimation = NULL;
	}
}

//----------------------------------------------------------------------------
// Perform rotate transform manipulation on the object
//----------------------------------------------------------------------------
void animPixmapItem::anim_scale()
{
	QTransform transform ;

	//scale the object by changing the transform
	transform = transform.translate(m_TileWidth, m_TileHeight);
	transform = transform.scale(m_myScale, m_myScale);
	transform = transform.translate(-m_TileWidth, -m_TileHeight);

	setTransform(transform);
	update();
}

//----------------------------------------------------------------------------
// Setup the animation to do scale up
//----------------------------------------------------------------------------
void animPixmapItem::StartAnimScale()
{
	//cleanup the scale animation if necessary
	clean_scale_anim();

	//pause any rotation animation for the object until scale is done
	if (m_pRotateAnimation)
		m_pRotateAnimation->pause();

	//Define the necessary paramaters for the m_ScaleAmount animation
	m_myScale = 1.0;		// start scale of the image
	m_ScaleAmount = 1.5;	// final scale angle
	m_ScaleLoopCount = 1;	// Number of loops for the animation
	m_ScaleTime = 300;		// Amount of time for each flip

	// Do something
	m_pScaleAnimation = new QPropertyAnimation(this, "scale");
	m_pScaleAnimation->setDuration(m_ScaleTime);
	m_pScaleAnimation->setLoopCount(m_ScaleLoopCount);
	m_pScaleAnimation->setStartValue(m_myScale);
	m_pScaleAnimation->setEndValue(m_ScaleAmount);

	m_pScaleAnimation->start();

	//clean up the animation pointer when it produces a "finished" signal
	QObject::connect(m_pScaleAnimation, SIGNAL(finished()), this, SLOT(StartAnimUnScale()));
}

//----------------------------------------------------------------------------
// Setup the animation to do scale up
//----------------------------------------------------------------------------
void animPixmapItem::StartAnimUnScale()
{
	//cleanup the scale animation if necessary
	clean_scale_anim();

	//pause any rotation animation for the object until scale is done
	if (m_pRotateAnimation)
		m_pRotateAnimation->pause();
	
	//Define the necessary paramaters for the m_ScaleAmount animation
	m_ScaleAmount = 1.0;    // final scale angle
	m_ScaleLoopCount = 1;   // Number of loops for the animation
	m_ScaleTime = 300;     // Amount of time for each flip

	// Do something
	m_pScaleAnimation = new QPropertyAnimation(this, "scale");
	m_pScaleAnimation->setDuration(m_ScaleTime);
	m_pScaleAnimation->setLoopCount(m_ScaleLoopCount);
	m_pScaleAnimation->setStartValue(m_myScale);
	m_pScaleAnimation->setEndValue(m_ScaleAmount);
		
	m_pScaleAnimation->start();

	//clean up the animation pointer when it produces a "finished" signal
	QObject::connect(m_pScaleAnimation, SIGNAL(finished()), this, SLOT(clean_scale_anim()));
}

//----------------------------------------------------------------------------
// Setup the animation to do scale up
//----------------------------------------------------------------------------
void animPixmapItem::Shake()
{
	//ShowCheckbox();
	qreal cur_rotate = rotation();
	qreal new_rotate = cur_rotate + 5;
	qreal new_rotate2 = cur_rotate - 5;
	int duration = 45;

	QPropertyAnimation* animation1 = new QPropertyAnimation(this, "rotation");
	//set the necessary animation parameters
	animation1->setDuration(duration);     
	animation1->setStartValue(cur_rotate);
	animation1->setEndValue(new_rotate);

	QPropertyAnimation* animation2 = new QPropertyAnimation(this, "rotation");
	//set the necessary animation parameters
	animation2->setDuration(duration);     
	animation2->setStartValue(new_rotate);
	animation2->setEndValue(new_rotate2);

	QPropertyAnimation* animation3 = new QPropertyAnimation(this, "rotation");
	//set the necessary animation parameters
	animation3->setDuration(duration);     
	animation3->setStartValue(new_rotate2);
	animation3->setEndValue(cur_rotate);

	QSequentialAnimationGroup *group = new QSequentialAnimationGroup();
	group->addAnimation(animation1);
	group->addAnimation(animation2);
	group->addAnimation(animation3);
	group->setLoopCount(2);
	//start the animation
	group->start(QPropertyAnimation::DeleteWhenStopped);
}

//------------------------------------------------------------------------
/// Do the scale animation 
//------------------------------------------------------------------------
void animPixmapItem::Scale(float i_ScaleAmount, int i_Duration)
{
	QPropertyAnimation* animation = new QPropertyAnimation(this, "scale");
	animation->setDuration(i_Duration);
	animation->setStartValue(m_myScale);
	animation->setEndValue(i_ScaleAmount);

	animation->start( QPropertyAnimation::DeleteWhenStopped );
}

//----------------------------------------------------------------------------
// Clean up the rotation animation pointer
//----------------------------------------------------------------------------
void animPixmapItem::clean_scale_anim()
{
	//if any rotation animation was paused, resume it
	if (m_pRotateAnimation && (m_pRotateAnimation->state() == QAbstractAnimation::Paused))
		m_pRotateAnimation->resume();

	//clean up the animation pointer
	if (m_pScaleAnimation)
	{
		m_pScaleAnimation->stop();
		delete m_pScaleAnimation;
		m_pScaleAnimation = NULL;
	}
}

//----------------------------------------------------------------------------
// Get the current value of the object's rotation angle; used by animation pointer
//----------------------------------------------------------------------------
qreal animPixmapItem::getRotate()
{
	return m_myRotate;
}

//----------------------------------------------------------------------------
// Set the current value of the object's rotation angle.
// used by animation pointer
//----------------------------------------------------------------------------
void animPixmapItem::setRotate(qreal r)
{
	m_myRotate = r;
	prepareGeometryChange();
	anim_rotate();
}

//----------------------------------------------------------------------------
// Get the current value of the object's scale; used by animation pointer
//----------------------------------------------------------------------------
qreal animPixmapItem::GetScale()
{
	return m_myScale;
}

//----------------------------------------------------------------------------
// Set the current value of the object's scale.
// used by animation pointer
//----------------------------------------------------------------------------
void animPixmapItem::SetScale(qreal s)
{
	m_myScale = s;
	prepareGeometryChange();
	anim_scale();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animPixmapItem::SetHasLabel(bool i_bHasLabel)
{
	m_bHasLabel = i_bHasLabel;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool animPixmapItem::GetHasLabel()
{
	return m_bHasLabel;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animPixmapItem::flip(QGraphicsSceneHoverEvent  *event)
{
	if (m_pRotateAnimation && (m_pRotateAnimation->state() == QAbstractAnimation::Paused))
		return;

	//Define the necessary paramaters for the flip animation
	m_myRotate = 0.0;  //starting rotation angle
	m_myScale = 1.0;   // scale of the image
	m_rotateAmount = 360.0;   //final rotation angle
	m_RotateLoopCount = 1;    //Number of flips for the animation
	m_RotationTime = 2000;     // Amount of time for each flip

	//If a flip was already in progress, stop it and clean up after it.
	clean_rotate_anim();

	// create a new rotation animation
	m_pRotateAnimation = new QPropertyAnimation(this, "rotation");

	//set the necessary animation parameters
	m_pRotateAnimation->setDuration(m_RotationTime);     
	m_pRotateAnimation->setLoopCount(m_RotateLoopCount);
	m_pRotateAnimation->setStartValue(m_myRotate);
	m_pRotateAnimation->setEndValue(m_rotateAmount);

	//start the animation
	m_pRotateAnimation->start();

	//clean up the animation pointer when it produces a "finished" signal
	QObject::connect(m_pRotateAnimation, SIGNAL(finished()), this, SLOT(clean_rotate_anim()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animPixmapItem::hoverEnterEvent(QGraphicsSceneHoverEvent   *event)
{
	QGraphicsPixmapItem::hoverEnterEvent(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animPixmapItem::hoverLeaveEvent(QGraphicsSceneHoverEvent  *event)
{
	QGraphicsPixmapItem::hoverLeaveEvent(event);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animPixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	event->accept();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animPixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	event->accept();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void animPixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QRectF b = boundingRect();
	if (b.contains(event->pos()))
	{
	     if(m_ModeLevel != (fcmdModeMgr::ModeLevel::e_CustomIcons))
			m_pParentPanel->UpdateSelection(this);
	     if( fcmdModeMgr::Instance != NULL )
			fcmdModeMgr::Instance->ProcessModeEvent((fcmdModeMgr::ModeLevel)m_ModeLevel, m_Locator, m_ItemID);
	}
}

///---------------------------------------------------------------------------
/// Set the locator that is associated with this anim pixmap
///---------------------------------------------------------------------------
void animPixmapItem::SetLocator(const fsLocator& i_Locator)
{
	m_Locator = i_Locator;
}

///-----------------------------------------------------------------------
/// Get the locator that is associated with this anim pixmap
///-----------------------------------------------------------------------
const fsLocator& animPixmapItem::GetLocator()
{
	return m_Locator;
}

///-----------------------------------------------------------------------
/// Set the ID for this pixmap if necessary
///-----------------------------------------------------------------------
void animPixmapItem::SetItemID(const int& i_ItemID)
{
	m_ItemID = i_ItemID;
}

///-----------------------------------------------------------------------
/// Get the ID for this pixmap
///-----------------------------------------------------------------------
const int& animPixmapItem::GetItemID()
{
	return m_ItemID;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
QWidget* animPixmapItem::GetParent()
{
	return m_pParentPanel->parentWidget();
}

//------------------------------------------------------------------------
/// Get the tile width
//------------------------------------------------------------------------
qreal animPixmapItem::GetTileWidth()
{
	return m_TileWidth;
}

//------------------------------------------------------------------------
/// Get the tile height
//------------------------------------------------------------------------
qreal animPixmapItem::GetTileHeight()
{
	return m_TileHeight;
}

//------------------------------------------------------------------------
/// will call the base class setPixmap function, but also allow child classes
/// to perform custom steps before the pixmap is set
//------------------------------------------------------------------------
void animPixmapItem::SetPixmap(const QPixmap &i_Pixmap)
{
	QGraphicsPixmapItem::setPixmap(i_Pixmap);
}

//------------------------------------------------------------------------
/// paint overrides parent paint function and currently sets a border 
/// around the pixmap image
//------------------------------------------------------------------------
void animPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QGraphicsPixmapItem::paint(painter, option, widget);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void animPixmapItem::SetHighlightColor(const QColor& i_Color)
{
	m_HighlightColor = i_Color;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
const QColor& animPixmapItem::GetHighlightColor()
{
	return m_HighlightColor;
}

//----------------------------------------------------------------------------
/// Highlight the image
//----------------------------------------------------------------------------
void animPixmapItem::HighlightImage(int i_Duration, float i_EndStrength)
{
	QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect();
	effect->setColor(m_HighlightColor);
	effect->setStrength(0.0);
	setGraphicsEffect(effect);

	m_pColorAnimation= new QPropertyAnimation(effect, "strength");

	//set the necessary animation parameters
	m_pColorAnimation->setDuration(i_Duration);     
	m_pColorAnimation->setStartValue(0);
	m_pColorAnimation->setEndValue(i_EndStrength);
	m_pColorAnimation->start(QPropertyAnimation::DeleteWhenStopped);
}

//----------------------------------------------------------------------------
/// Use an animation to remove the highlight
//----------------------------------------------------------------------------
void animPixmapItem::AnimRemoveHighlightImage(int i_Duration, float i_StartStrength)
{
	QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect();
	effect->setColor(m_HighlightColor);
	effect->setStrength(i_StartStrength);
	setGraphicsEffect(effect);

	m_pColorAnimation= new QPropertyAnimation(effect, "strength");

	//set the necessary animation parameters
	m_pColorAnimation->setDuration(i_Duration);     
	m_pColorAnimation->setStartValue(i_StartStrength);
	m_pColorAnimation->setEndValue(0.0);
	m_pColorAnimation->start(QPropertyAnimation::DeleteWhenStopped);
	QObject::connect(m_pColorAnimation, SIGNAL(finished()), this, SLOT(clean_highlight()));
}

//----------------------------------------------------------------------------
/// Remove any color effect placed on the image
//----------------------------------------------------------------------------
void animPixmapItem::RemoveHighlight()
{
	setGraphicsEffect(NULL);
	if(m_bSelected)
		m_bSelected = false;
}

//----------------------------------------------------------------------------
/// Slot that is called on the animation event of removing the highlight of an item
//----------------------------------------------------------------------------
void animPixmapItem::clean_highlight()
{
	RemoveHighlight();
}

//----------------------------------------------------------------------------
/// If there is some special operation to do when the pixmap is selected,
/// do that here
//----------------------------------------------------------------------------
void animPixmapItem::DoSelection()
{
	SetSelected(true);
}

//------------------------------------------------------------------------
/// Get the selected state of the pixmap
//------------------------------------------------------------------------
bool animPixmapItem::IsSelected()
{
	return m_bSelected;
}

//------------------------------------------------------------------------
/// Set the selected state of the pixmap
//------------------------------------------------------------------------
void animPixmapItem::SetSelected(bool i_bSelected)
{
	m_bSelected = i_bSelected;
}

//----------------------------------------------------------------------------
/// Enable the checkbox of the pixmap if it requires one
//----------------------------------------------------------------------------
void animPixmapItem::ShowCheckbox()
{
	if( m_pCheckbox == NULL && this->GetParent())
	{
		m_pCheckbox = new QCheckBox(this->GetParent());
		QPointF pixPos = this->pos();
		m_pCheckbox->setGeometry(pixPos.x() + 5, pixPos.y(), m_pCheckbox->width(), m_pCheckbox->height());
		m_pCheckbox->setText("");
		QObject::connect(m_pCheckbox, SIGNAL(stateChanged(int)), this, SLOT(ProcessCheck(int)));
	}
	
	if(m_pCheckbox)
		m_pCheckbox->show();
}

//----------------------------------------------------------------------------
/// When a checkbox is clicked, we need to notify the parent panel
//----------------------------------------------------------------------------
void animPixmapItem::ProcessCheck(int i_NewState)
{
	bool bChecked = true;
	if(i_NewState == Qt::Checked)
		bChecked = true;
	else
		bChecked = false;

	//process the checked item through the parent panel
	m_pParentPanel->UpdateCheckedItem(this, bChecked);
}

//----------------------------------------------------------------------------
/// Hide/remove the checkbox from the animpixmap
//----------------------------------------------------------------------------
void animPixmapItem::HideCheckbox()
{
	if( m_pCheckbox != NULL )
		m_pCheckbox->hide();
}

//----------------------------------------------------------------------------
/// Hide the pixmap's label
//----------------------------------------------------------------------------
void animPixmapItem::HideLabel()
{
	if(m_pLabel != NULL)
		m_pLabel->hide();
}

//----------------------------------------------------------------------------
/// show the checkbox check selection from the animpixmap
//----------------------------------------------------------------------------
void animPixmapItem::ShowCheckboxCheck()
{
	QObject::disconnect(m_pCheckbox, SIGNAL(stateChanged(int)), this, SLOT(ProcessCheck(int)));
	if( m_pCheckbox != NULL )
		m_pCheckbox->setCheckState(Qt::Checked);
	QObject::connect(m_pCheckbox, SIGNAL(stateChanged(int)), this, SLOT(ProcessCheck(int)));
}

//----------------------------------------------------------------------------
/// remove the checkbox check selection from the animpixmap
//----------------------------------------------------------------------------
void animPixmapItem::RemoveCheckboxCheck()
{
	QObject::disconnect(m_pCheckbox, SIGNAL(stateChanged(int)), this, SLOT(ProcessCheck(int)));
	if( m_pCheckbox != NULL )
		m_pCheckbox->setCheckState(Qt::Unchecked);
	QObject::connect(m_pCheckbox, SIGNAL(stateChanged(int)), this, SLOT(ProcessCheck(int)));
}