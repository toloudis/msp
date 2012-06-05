/*****************************************************************************
**	fcuiTimeline.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcui/qtGUI/fcuiTimeline.hpp"

#include "FCSupport/actn/actnSceneMgr.hpp"
#include "FCSupport/actn/actnTitleCardMgr.hpp"
#include "FCSupport/fcdc/fcdcDataMgr.hpp"
#include "FCSupport/fcdc/data/fcdcTimelineItemData.hpp"
#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "FCSupport/fcui/qtGUI/fcuiTimelineItem.hpp"
#include "FCSupport/path/pathDirectoryParser.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/name/nameString.hpp"

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
namespace
{
	//qt data format type used to grab the format of the icons
	const char* c_IMAGEFORMAT = "application/x-dnditemdata";
	const char* c_LOCATORFORMAT ="application/x-fslocatordata";

	const char* c_DELETEICON = "Media/GUI/Action_element_deleteimage.png";
	const char* c_CAMERAICON = "Media/Gui/Action_catagory_camerachioce.png";
	const char* c_CAMERASELECTEDICON = "Media/Gui/Action_catagory_cameraselected.png";
	QString button_stylesheet1(" QPushButton{border: 0px solid #8f8f91;border-radius: 0px; background-image: url(Media/Gui/Action_element_deleteimage.png); background-color: rgba(0,0,0,0);} QPushButton:pressed {color: rgba(0,0,0,0) }	QPushButton:checked {color:rgba(0,0,0,0)} QPushButton:hover {color:rgba(0,0,0,0)}");
	QString button_stylesheet2(" QPushButton{border: 0px solid #8f8f91;border-radius: 0px; background-image: url(Media/Gui/Action_catagory_camerachioce.png); background-color: rgba(0,0,0,0);} QPushButton:pressed {color: rgba(0,0,0,0) }	QPushButton:checked {color:rgba(0,0,0,0)} QPushButton:hover {color:rgba(0,0,0,0)}");
	QString button_stylesheet3(" QPushButton{border: 0px solid #8f8f91;border-radius: 0px; background-image: url(Media/Gui/BLUEpencil_titlecard_element.png); background-color: rgba(0,0,0,0);} QPushButton:pressed {color: rgba(0,0,0,0) }	QPushButton:checked {color:rgba(0,0,0,0)} QPushButton:hover {color:rgba(0,0,0,0)}");
	int l_IconSize;
}

///---------------------------------------------------------------------------
/// Constructor
///---------------------------------------------------------------------------
fcuiTimeline::fcuiTimeline(QWidget* i_pParent)
: QListWidget(i_pParent),
  m_pReorderOriginalItem(NULL),
  m_UpdateData(NULL),
  m_pClickedItem(NULL), 
  m_pShiftedItem(NULL),
  m_MidDropItem(NULL),
  m_CurrentPlaybackItem(NULL),
  m_pScrollButtonHome(NULL),
  m_pScrollButtonEnd(NULL),
  m_NumIcons(0),
  m_bEditing(false),
  m_bItemShift(false),
  m_bMiddleDrop(false),
  m_bMidDropStart(false),
  m_ProcessItem(false)
{
	setAcceptDrops(true);
	setGeometry(0, 0, i_pParent->width(), i_pParent->height());
	m_StartPosition = geometry();
	m_HidePosition = m_StartPosition;
	m_HidePosition.setY((m_StartPosition.y() - 100));

	setFrameStyle(QFrame::NoFrame);
	setFrameShape(QFrame::NoFrame);
	setLineWidth(0);
	
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
	
	QString qs_Style("background-image: url(Media/panel_back.png)");
	this->setStyleSheet(qs_Style);
	this->setUniformItemSizes(false);

	viewport()->setAcceptDrops(true);
	setFlow(QListView::LeftToRight);
	setAutoScroll(true);
	setDragEnabled(true);
	
	m_IconHeight = height() - 10;
	m_IconWidth = fcuiUtils::GetWidescreenWidth(m_IconHeight);
	setAutoScrollMargin(m_IconWidth*2);  //set the auto scroll amount to the width of a single item

	this->setSpacing(2);
	setIconSize(QSize(m_IconWidth,m_IconHeight));
	setMovement(QListView::Free);
	setDropIndicatorShown(true);
	setResizeMode(QListView::Adjust);
	QListWidget::setDragDropMode(QAbstractItemView::InternalMove);
	
	//signals when a timeline item is clicked
	QObject::connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(TimelineItemClicked(QListWidgetItem*)));
	QObject::connect(this, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(TimelineItemPressed(QListWidgetItem*)));
	QObject::connect(this, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(TimelineItemChanged(QListWidgetItem*)));
	QObject::connect(this->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(ScrollBarMoved(int)));
	//set up the buttons for that appear when an item is clicked
	m_pDeleteButton = new QPushButton(this);
	m_pEditButton = new QPushButton(this);
	
	//m_pDeleteButton->setIcon(QIcon(c_DELETEICON));
	m_pDeleteButton->setGeometry(m_pDeleteButton->x(), m_pDeleteButton->y(), 20,20);
	//m_pDeleteButton->setIconSize(m_pDeleteButton->size());
	m_pDeleteButton->setFlat(true);
	m_pDeleteButton->setStyleSheet(button_stylesheet1);
	m_pDeleteButton->hide();

	//m_pEditButton->setIcon(QIcon(c_CAMERAICON));
	m_pEditButton->setGeometry(m_pEditButton->x(), m_pEditButton->y(), 20,20);
	//m_pEditButton->setIconSize(m_pEditButton->size());
	m_pEditButton->setFlat(true);
	m_pEditButton->setStyleSheet(button_stylesheet2);
	m_pEditButton->hide();
	
	QObject::connect(m_pDeleteButton, SIGNAL(clicked()), this, SLOT(DeleteButton_Clicked()));
	QObject::connect(m_pEditButton, SIGNAL(clicked()), this, SLOT(EditButton_Clicked()));

	fcuiFormMgr::EnableTimelineSlider(false);
}

///---------------------------------------------------------------------------
/// Destructor
///---------------------------------------------------------------------------
fcuiTimeline::~fcuiTimeline()
{
}

///---------------------------------------------------------------------------
/// Process timeline events when they need to happen on a subsequent frame
///---------------------------------------------------------------------------
void fcuiTimeline::Think()
{
	if( m_bItemShift )
	{
		m_bItemShift = false;
		if( m_pShiftedItem != NULL )
		{
			this->setCurrentItem(m_pShiftedItem);
			if(m_bEditing)
			{
				emit itemClicked(m_pShiftedItem);
			}
			else
			{
				if(fcmdModeMgr::Instance != NULL)
					fcmdModeMgr::Instance->ProcessTimelineItem(m_pShiftedItem->GetItemData());
				//save the item for later
				m_pClickedItem = m_pShiftedItem;
				//re-position our buttons and make sure they are visible
				PositionButtons(visualItemRect(m_pShiftedItem));
			}
			m_pShiftedItem = NULL;
		}
	}	
	if( m_bMiddleDrop )
	{
		if( !m_bMidDropStart )
		{
			QListWidgetItem* decoy = m_MidDropItem->QListWidgetItem::clone();
			insertItem(m_MidDropRow, decoy);
			emit itemChanged(decoy);
		}
		else
		{
			QListWidgetItem* old = this->takeItem(m_pReorderOriginalRow + 1);
			delete old;
			m_MidDropItem = NULL;
			m_bMiddleDrop = false;
			m_bMidDropStart = false;
		}
	}
	if( m_ProcessItem )
	{
		if(fcmdModeMgr::Instance != NULL)
			fcmdModeMgr::Instance->ProcessTimelineItem(m_UpdateData->GetItemData());
		m_UpdateData = NULL;
		m_ProcessItem = false;
	}
}


//----------------------------------------------------------------------------
/// Populate the timeline given a vector of timeline item properties
//----------------------------------------------------------------------------
void fcuiTimeline::PopulateTimeline(const std::vector<fcuiTimelineItemData>& i_TimelineItems)
{
	fcuiTimelineItem* time_item; 
	fsLocator iconDir;
	fcuiTimelineItemData temp_data;
	for( int i = 0; i < i_TimelineItems.size(); ++i )
	{
		temp_data = i_TimelineItems[i];
		time_item = new fcuiTimelineItem();
		this->addItem(time_item);
	
		if(i_TimelineItems[i].m_TimelineCategory == itString(fcuiConstants::c_ACTION_STATE_TITLECARD))
		{
			if(actnTitleCardMgr::Instance != NULL)
				actnTitleCardMgr::Instance->CheckTitleCardData(temp_data);
		}
		time_item->SetItemData( i_TimelineItems[i] );

		//get the icon for this item
		iconDir = i_TimelineItems[i].m_ItemLocator;
		iconDir.Push(fcuiConstants::c_FILE_ICON);
		std::string iconStr;
		QString q_IconStr;
		if(fsFileUtil::FileExists(iconDir))
		{
			fsFileUtil::LocatorToANSIFilename(iconDir, iconStr);
			q_IconStr = QString(iconStr.c_str());
			time_item->setIcon(QIcon(q_IconStr));
		}

		if( fcuiTimelineMgr::Instance != NULL )
		{
			if( i_TimelineItems[i].m_EndTime > fcuiTimelineMgr::Instance->GetEndTime() )
				fcuiTimelineMgr::Instance->SetEndTime(i_TimelineItems[i].m_EndTime);
		}
		//set the size and background color
		time_item->setBackgroundColor(QColor(0,0,0));
		QSize item_size = QSize(GetTimelineItemSize(i_TimelineItems[i].m_EndTime - i_TimelineItems[i].m_StartTime), m_IconHeight);
		time_item->setSizeHint(item_size);

		//increment the number of timeline items
		m_NumIcons++;
	}
	fcuiFormMgr::EnableTimelineSlider(m_NumIcons > 0);

}

///---------------------------------------------------------------------------
/// Process the event when a drag enter occurs
/// overloaded Qt function
///---------------------------------------------------------------------------
void fcuiTimeline::dragEnterEvent(QDragEnterEvent* event)
{
	if( fcmdModeMgr::Instance != NULL && fcmdModeMgr::Instance->GetTimelineLocked())
		return;

	if( m_bMiddleDrop )
		return;

	event->acceptProposedAction();
	//QListWidget::dragEnterEvent(event);
}

///---------------------------------------------------------------------------
/// Process the event when an item is moved within the widget
/// overloaded Qt function
///---------------------------------------------------------------------------
void fcuiTimeline::dragMoveEvent(QDragMoveEvent* event)
{
	if( fcmdModeMgr::Instance != NULL && fcmdModeMgr::Instance->GetTimelineLocked())
		return;

	if( m_bMiddleDrop )
		return;

	event->acceptProposedAction();
	QListWidget::dragMoveEvent(event);
}

///---------------------------------------------------------------------------
/// Process the event when an item is dropped into the widget
/// overloaded Qt function
///---------------------------------------------------------------------------
void fcuiTimeline::dropEvent(QDropEvent* event)
{
	if( fcmdModeMgr::Instance != NULL && fcmdModeMgr::Instance->GetTimelineLocked())
		return;

	if( m_bMiddleDrop )
		return;

	if( event->mimeData()->hasFormat(QString(c_IMAGEFORMAT)) &&
		event->mimeData()->hasFormat(QString(c_LOCATORFORMAT)) )
	{
		//first check if we are dropping this item onto a pre-existing item
		QListWidgetItem* pShiftItem = this->itemAt(event->pos());

		QByteArray image_data = event->mimeData()->data(QString(c_IMAGEFORMAT));
		QDataStream data_stream(&image_data, QIODevice::ReadOnly);
		QPixmap pixmap;
		data_stream >> pixmap;
		fcuiTimelineItem* time_item = new fcuiTimelineItem();
		time_item->setIcon(QIcon(pixmap));
		this->addItem(time_item);

		// Retrieve the custom data holding the locator string
		QByteArray locator_data = event->mimeData()->data(QString(c_LOCATORFORMAT));
		QDataStream locator_stream(&locator_data, QIODevice::ReadOnly);
		QString qs_locator;
		locator_stream >> qs_locator;

		fsLocator locator;
		itString locator_string;
		locator_string = ((const itString::CharType*)qs_locator.data());
		fsFileUtil::UnicodeStringToLocator(locator_string, locator);
		
		//now get the data associated with this timeline item
		fcuiTimelineItemData time_data;
		time_data.m_ItemLocator = locator;

		if(fcmdModeMgr::Instance != NULL)
			time_data.m_TimelineCategory = fcmdModeMgr::Instance->GetTimelineSubject();

		if(fcuiTimelineMgr::Instance != NULL)
			time_data.m_StartTime = fcuiTimelineMgr::Instance->GetEndTime();
		
		//process the element's timeline operation
		if(fcmdModeMgr::Instance != NULL)
			fcmdModeMgr::Instance->ProcessElementOperation(locator);

		if(fcuiTimelineMgr::Instance != NULL)
			time_data.m_EndTime = fcuiTimelineMgr::Instance->GetEndTime();

		//if this is a title card, save its driver name
		if(actnTitleCardMgr::Instance != NULL && 
			time_data.m_TimelineCategory == itString(fcuiConstants::c_ACTION_STATE_TITLECARD))
		{
			m_pEditButton->setStyleSheet(button_stylesheet3);
			time_data.m_CategoryData = actnTitleCardMgr::Instance->GetMostRecentDriverName();
		}

		else if(time_data.m_TimelineCategory == itString(fcuiConstants::c_ACTION_STATE_SCENE))
		{
			m_pEditButton->setStyleSheet(button_stylesheet2);
			std::vector<fsLocator> directories;
			pathDirectoryParser parser;
			parser.GetSubDirectories(locator, directories);
			if( directories.size() >= 1 )
			{
				time_data.m_CategoryData = directories[0].GetLastName();
			}
		}

		time_item->SetItemData(time_data);
		time_item->setBackgroundColor(QColor(0,0,0));
		QSize item_size = QSize(GetTimelineItemSize(time_data.m_EndTime - time_data.m_StartTime), m_IconHeight);
		time_item->setSizeHint(item_size);

		m_NumIcons++;
		QRect item_pos = visualItemRect(time_item);
		QSize view_size = this->maximumViewportSize();
		int item_bounds = item_pos.x() + item_pos.width();
		int view_bounds = view_size.width() + 40;

		//if the new item is outside of our view bounds, we need to increase the view
		/*if( item_bounds > view_bounds)
		{
			
		}*/

		if( pShiftItem != NULL )
		{
			time_item->setHidden(true);
			m_bMiddleDrop = true;
			m_bMidDropStart = false;
			m_pReorderOriginalItem = time_item;
			m_pReorderOriginalRow = this->row(time_item);
			m_MidDropRow = this->row(pShiftItem);
			m_MidDropItem = time_item;
		}
		else
		{
			this->setCurrentItem(time_item);
			if(m_bEditing)
			{
				emit itemClicked(time_item);
			}
			else
			{
				if(fcmdModeMgr::Instance != NULL)
					fcmdModeMgr::Instance->ProcessTimelineItem(time_item->GetItemData());
				//save the item for later
				m_pClickedItem = time_item;
				//re-position our buttons and make sure they are visible
				PositionButtons(visualItemRect(time_item));
			}
			//UpdateButtonPosition();
		
		}
		fcuiFormMgr::EnableTimelineSlider(m_NumIcons > 0);
		event->acceptProposedAction();
	}
	else
	{
		QListWidget::dropEvent(event);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int fcuiTimeline::GetTimelineItemSize( const maTime& i_ItemDuration )
{
	int itemWidth = 0;
	float item_dur_sec = i_ItemDuration.AsSeconds();

	if( item_dur_sec <= 2.0f )
	{
		itemWidth = m_IconWidth;
	}	else if( item_dur_sec <= 2.5f )
	{
		itemWidth = m_IconWidth * 1.2;
	}
	else if( item_dur_sec <= 3.0f )
	{
		itemWidth = m_IconWidth * 1.4;
	}
	else if( item_dur_sec <= 3.5f )
	{
		itemWidth = m_IconWidth * 1.6;
	}
	else if( item_dur_sec <= 4.0f )
	{
		itemWidth = m_IconWidth * 1.8;
	}
	else if( item_dur_sec > 4.0f )
	{
		itemWidth = m_IconWidth * 2;
	}
	return itemWidth;
}

//----------------------------------------------------------------------------
/// When a timeline item is pressed we need to execute its operations depending
/// its timeline data.
//----------------------------------------------------------------------------
void fcuiTimeline::TimelineItemClicked( QListWidgetItem* i_Item )
{
	if( m_bMiddleDrop )
		return;

	//now get our TimelineItem
	fcuiTimelineItem* time_item = dynamic_cast<fcuiTimelineItem*>(i_Item);
	if(time_item != NULL)
	{
		if(actnTitleCardMgr::Instance != NULL && 
			time_item->GetItemData().m_TimelineCategory == itString(fcuiConstants::c_ACTION_STATE_TITLECARD))
		{
			m_pEditButton->setStyleSheet(button_stylesheet3);
		}

		else if(time_item->GetItemData().m_TimelineCategory == itString(fcuiConstants::c_ACTION_STATE_SCENE))
		{
			m_pEditButton->setStyleSheet(button_stylesheet2);
		}

		if(fcmdModeMgr::Instance != NULL && !fcmdModeMgr::Instance->GetTimelineLocked())
			fcmdModeMgr::Instance->ProcessTimelineItem(time_item->GetItemData());
		
		//save the item for later
		m_pClickedItem = time_item;
		//re-position our buttons and make sure they are visible
		PositionButtons(visualItemRect(i_Item));
		ShowButtons();
		if(m_bEditing)
		{
			fcuiFormMgr::ResetTimelineEdit();	//clear the current edit window if any
			EditButton_Clicked();  // now update the edit mode
		}
	}
}

//----------------------------------------------------------------------------
/// We need to prepare for a possible move operation by saving the data of the
/// most recently pressed item
//----------------------------------------------------------------------------
void fcuiTimeline::TimelineItemPressed( QListWidgetItem* i_Item )
{
	if( fcmdModeMgr::Instance != NULL && fcmdModeMgr::Instance->GetTimelineLocked())
		return;

	if( m_bMiddleDrop )
		return;

	fcuiTimelineItem* time_item = dynamic_cast<fcuiTimelineItem*>(i_Item);
	if(time_item != NULL)
	{
		m_pReorderOriginalItem = time_item;
		m_pReorderOriginalRow = this->row(i_Item);
	}
}

//----------------------------------------------------------------------------
/// The reorder operation has just copied the original item without our
/// custom data, now we need to replace the data in the new copied data
/// with our original data.
//----------------------------------------------------------------------------
void fcuiTimeline::TimelineItemChanged( QListWidgetItem* i_Item )
{
	if( fcmdModeMgr::Instance != NULL && fcmdModeMgr::Instance->GetTimelineLocked())
		return;
	fcuiTimelineItem* time_item = dynamic_cast<fcuiTimelineItem*>(i_Item);
	if(time_item == NULL)
	{
		//if the item has no TimelineItem data, we need to remove it and replace it
		int item_row = row(i_Item);
		QListWidgetItem* old = takeItem(item_row);
		fcuiTimelineItem* new_item = new fcuiTimelineItem(*m_pReorderOriginalItem);
		insertItem(item_row, new_item);
		viewport()->repaint();

		//items removed by takeItem() are no longer handeled by Qt so we need to 
		//delete it ourselves.
		delete old;

		//if trying to reorder to the original row, don't shift any items
		if(item_row == m_pReorderOriginalRow)
			return;

		maTime duration = new_item->GetItemData().m_EndTime - new_item->GetItemData().m_StartTime;

		ShiftItems(item_row, duration);
		ShiftItem(item_row);

		m_pShiftedItem = new_item;
		m_bItemShift = true;
		m_bMidDropStart = true;
	}
}

//----------------------------------------------------------------------------
/// callback when delete button is clicked
//----------------------------------------------------------------------------
void fcuiTimeline::ScrollBarMoved(int i_ScrollValue )
{
	UpdateButtonPosition();
}

//----------------------------------------------------------------------------
/// callback when delete button is clicked
//----------------------------------------------------------------------------
void fcuiTimeline::DeleteButton_Clicked()
{
	if( fcmdModeMgr::Instance != NULL && fcmdModeMgr::Instance->GetTimelineLocked())
		return;
	if(m_pClickedItem == NULL)
		return;

	int item_row = row(m_pClickedItem);
	
	//shift all items affected by the deletion
	m_pReorderOriginalRow = item_row;
	maTime duration = m_pClickedItem->GetItemData().m_EndTime - m_pClickedItem->GetItemData().m_StartTime;
	ShiftItems(m_NumIcons, duration);
	RemoveItem(m_pClickedItem);

	//remove the item from our view
	QListWidgetItem* removedItem = takeItem(item_row);
	delete removedItem;  //qt doesn't delete the object once we remove it from our view
	m_pClickedItem = NULL;

	//decrement the number of icons in the view and hide the edit/delete buttons
	m_NumIcons--;
	HideButtons();
	fcuiFormMgr::HideCameraPicker();
	fcuiFormMgr::EnableTimelineSlider(m_NumIcons > 0);
}

//----------------------------------------------------------------------------
/// callback when the edit button is clicked
//----------------------------------------------------------------------------
void fcuiTimeline::EditButton_Clicked()
{
	if( fcmdModeMgr::Instance != NULL && fcmdModeMgr::Instance->GetTimelineLocked())
		return;
	if(m_pClickedItem == NULL)
		return;

	if(fcmdModeMgr::Instance != NULL)
	{
		
		fcuiTimelineItemData edit_data = m_pClickedItem->GetItemData();
		fcmdModeMgr::Instance->ProcessTimelineItem(edit_data);
		fcmdModeMgr::Instance->EditTimelineItem(edit_data);
		m_pClickedItem->SetItemData(edit_data);
	}
	//HideButtons();
}

//----------------------------------------------------------------------------
/// Set whether or not the timeline is in editing mode.  Should not be set
/// by the timeline class itself.  External managers should tell the instance
/// when they are editing or when they no longer need the editing services.
//----------------------------------------------------------------------------
void fcuiTimeline::SetIsEditing(bool i_bEditing)
{
	m_bEditing = i_bEditing;
}

//----------------------------------------------------------------------------
/// Do the drop in animation for the timeline
//----------------------------------------------------------------------------
void fcuiTimeline::AnimateDropIn()
{
	setVisible(true);
	setEnabled(true);
	HideButtons();
	if( (m_pScrollButtonHome != NULL) && (m_pScrollButtonEnd != NULL) )
	{
		m_pScrollButtonHome->show();
		m_pScrollButtonEnd->show();
	}
	QPoint sPoint(m_StartPosition.x(), m_StartPosition.y());
	QPoint hPoint(m_HidePosition.x(), m_HidePosition.y());
	QPropertyAnimation* animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(600);
    animation->setStartValue(hPoint);
	animation->setEndValue(sPoint);
	animation->setEasingCurve(QEasingCurve::OutBounce);
	
	animation->start(QPropertyAnimation::DeleteWhenStopped);
}

//----------------------------------------------------------------------------
/// Do the drop out animation for the timeline
//----------------------------------------------------------------------------
void fcuiTimeline::AnimateDropOut()
{
	if( (m_pScrollButtonHome != NULL) && (m_pScrollButtonEnd != NULL) )
	{
		m_pScrollButtonHome->hide();
		m_pScrollButtonEnd->hide();
	}
	QPoint sPoint(m_StartPosition.x(), m_StartPosition.y());;
	QPoint hPoint(m_HidePosition.x(), m_HidePosition.y());
	QPropertyAnimation* animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(600);
    animation->setStartValue(sPoint);
	animation->setEndValue(hPoint);
	animation->setEasingCurve(QEasingCurve::InBack);
	
	animation->start(QPropertyAnimation::DeleteWhenStopped);
	QObject::connect(animation, SIGNAL(finished()), this, SLOT(FinishDropOut()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiTimeline::FinishDropOut()
{
	setVisible(false);
	setEnabled(false);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiTimeline::ShiftItem(int i_NewReorderIndex)
{
	int neighborIndex;
	bool bMoveForward;
	if(i_NewReorderIndex > m_pReorderOriginalRow)
	{
		bMoveForward = true;
		neighborIndex = i_NewReorderIndex - 1;
	}
	else
	{
		bMoveForward = false;
		neighborIndex = i_NewReorderIndex + 1;
	}

	QListWidgetItem* neighborItem = NULL;
	QListWidgetItem* movedItem = NULL;
	fcuiTimelineItemData update_data;

	neighborItem = this->item(neighborIndex);
	movedItem = this->item(i_NewReorderIndex);

	fcuiTimelineItem* neighbor_item = dynamic_cast<fcuiTimelineItem*>(neighborItem);
	fcuiTimelineItem* time_item = dynamic_cast<fcuiTimelineItem*>(movedItem);
	if(time_item != NULL && neighbor_item != NULL)
	{
		if(fcmdModeMgr::Instance != NULL)
		{
			maTime duration;
			//start and end time may be changed during the shift, so we need to 
			update_data = time_item->GetItemData();
			if(bMoveForward)
			{
				duration = neighbor_item->GetItemData().m_EndTime - update_data.m_StartTime;
			}
			else
			{
				duration = neighbor_item->GetItemData().m_StartTime - update_data.m_EndTime;
			}

			//perform the shift operation depending on the item's category
			fcmdModeMgr::Instance->ShiftTimelineItem(update_data, duration, false);
			//any changes to the item data should be updated in the timeline object
			time_item->SetItemData(update_data);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiTimeline::ShiftItems(int i_NewReorderIndex, const maTime& i_Duration)
{
	int startIndex, endIndex;
	maTime duration = i_Duration;

	//depending on where the initial item was moved, we may need to move the affected
	//items forwards or backwards in time
	if(i_NewReorderIndex > m_pReorderOriginalRow)
	{
		startIndex = m_pReorderOriginalRow + 1;  // the items haven't shifted in the fcuiTimeline yet
		endIndex = i_NewReorderIndex;
		//duration = i_Duration
		duration.Negate(); // shift the items back in time
	}
	else
	{
		//if we moved an item backwards in time, we want to make sure to include that
		//proper items in our shift.  We need to add 1 to the start to make sure we 
		//aren't including our moved item in the shift
		startIndex = i_NewReorderIndex + 1;
		endIndex = m_pReorderOriginalRow + 1;
		//duration = i_Duration;  // shift the items forward in time
	}

	QListWidgetItem* widgetItem = NULL;
	fcuiTimelineItemData update_data;
	// in the timeline, all items that previously followed the moved item have shifted
	// back in the timeline.  We need to shift the drivers for each of these objects starting
	// with the object currently at the moved item's old spot up to the item right before the moved
	// item's new spot.
	for( int i = startIndex; i < endIndex; ++i )
	{
		widgetItem = this->item(i);
		fcuiTimelineItem* time_item = dynamic_cast<fcuiTimelineItem*>(widgetItem);
		if(time_item != NULL)
		{
			if(fcmdModeMgr::Instance != NULL)
			{
				//start and end time may be changed during the shift, so we need to 
				update_data = time_item->GetItemData();
				//perform the shift operation depending on the item's category
				fcmdModeMgr::Instance->ShiftTimelineItem(update_data, duration, false);
				//any changes to the item data should be updated in the timeline object
				time_item->SetItemData(update_data);
			}
		}
	}

}

//------------------------------------------------------------------------
/// Remove an item's drivers from the timeline
//------------------------------------------------------------------------
void fcuiTimeline::RemoveItem(fcuiTimelineItem* i_pItem)
{
	if( i_pItem == NULL )
		return;
	
	if(fcmdModeMgr::Instance != NULL)
	{
		fcmdModeMgr::Instance->RemoveTimelineItem(i_pItem->GetItemData());
	}
}

//------------------------------------------------------------------------
/// Re-position the additional buttons that appear when an item is clicked
//------------------------------------------------------------------------
void fcuiTimeline::PositionButtons(const QRect& i_ItemRect)
{
	int editOffset = 3;
	int deleteOffset = -23;
	int yPos = 3;
	int xPos = i_ItemRect.x() + editOffset;
	m_pEditButton->setGeometry(QRect(xPos, yPos, m_pEditButton->iconSize().width(), 
		                                         m_pEditButton->iconSize().height()));

	xPos = (i_ItemRect.x() + i_ItemRect.width()) + deleteOffset;
	m_pDeleteButton->setGeometry(QRect(xPos, yPos, m_pDeleteButton->iconSize().width(), 
												   m_pDeleteButton->iconSize().height()));

}

//----------------------------------------------------------------------------
/// If the timeline is scrolled, we need to update the position of our buttons
//----------------------------------------------------------------------------
void fcuiTimeline::UpdateButtonPosition()
{
	if(m_pClickedItem)
	{
		//re-position our buttons
		PositionButtons(visualItemRect(m_pClickedItem));
	}
}

//----------------------------------------------------------------------------
/// Select the timeline item at the given time
//----------------------------------------------------------------------------
void fcuiTimeline::SelectItemAtTime(const maTime& i_CurrentTime)
{
	QListWidgetItem* widgetItem = NULL;
	int endIndex = m_NumIcons;
	for( int i = 0; i < endIndex; ++i )
	{
		widgetItem = this->item(i);
		fcuiTimelineItem* time_item = dynamic_cast<fcuiTimelineItem*>(widgetItem);
		if(time_item != NULL)
		{
			if(time_item->GetItemData().m_StartTime <= i_CurrentTime &&
				time_item->GetItemData().m_EndTime >= i_CurrentTime)
			{
				if( time_item != m_CurrentPlaybackItem )
				{
					m_CurrentPlaybackItem = time_item;
					if(time_item->GetItemData().m_TimelineCategory == itString(fcuiConstants::c_ACTION_STATE_SCENE)) 
					{
						if( actnSceneMgr::Instance != NULL )
							actnSceneMgr::Instance->ShiftCameraKeys(m_CurrentPlaybackItem->GetItemData());
					}
					this->setCurrentItem(time_item);
					this->setItemSelected(time_item, true);
					break;
					
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiTimeline::WriteChunk()
{
	QListWidgetItem* widgetItem = NULL;
	int endIndex = m_NumIcons;
	fcdcTimelineItemData temp = fcdcDataMgr::GetData();
	temp.m_TimelineData.m_TimelineItems.clear();
	fcdcTimelineItem new_item;
	for( int i = 0; i < endIndex; ++i )
	{
		widgetItem = this->item(i);
		fcuiTimelineItem* time_item = dynamic_cast<fcuiTimelineItem*>(widgetItem);
		if(time_item != NULL)
		{
			new_item.m_BillboardName = time_item->GetItemData().m_BillboardName;
			new_item.m_BillboardPosX = time_item->GetItemData().m_BillboardPosX;
			new_item.m_BodyText.SetValue(itStringUtil::GetStdString(time_item->GetItemData().m_BodyText));
			new_item.m_CategoryData = time_item->GetItemData().m_CategoryData;
			new_item.m_EndTime = time_item->GetItemData().m_EndTime;
			new_item.m_ItemLocator = time_item->GetItemData().m_ItemLocator;
			new_item.m_StartTime = time_item->GetItemData().m_StartTime;
			new_item.m_TimelineCategory = time_item->GetItemData().m_TimelineCategory;
			new_item.m_TitleText.SetValue(itStringUtil::GetStdString(time_item->GetItemData().m_TitleText));
			new_item.m_TitlecardColor = time_item->GetItemData().m_TitlecardColor;
			new_item.m_colorChanged = time_item->GetItemData().m_colorChanged;
			temp.m_TimelineData.m_TimelineItems.push_back(new_item);

		}
	}
	fcdcDataMgr::SetData(temp);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiTimeline::ReadChunk(std::vector<fcuiTimelineItemData>& o_TimelineItems)
{
	fcdcTimelineItemData temp = fcdcDataMgr::GetData();
	
	for(int i = 0; i < temp.m_TimelineData.m_TimelineItems.size(); i++)
	{
		fcuiTimelineItemData temp_timelineitem;
		temp_timelineitem.m_BillboardName = temp.m_TimelineData.m_TimelineItems[i].m_BillboardName.GetValue();
		temp_timelineitem.m_BillboardPosX = temp.m_TimelineData.m_TimelineItems[i].m_BillboardPosX.GetValue();
		temp_timelineitem.m_BodyText = itString(temp.m_TimelineData.m_TimelineItems[i].m_BodyText.GetValue().c_str());
		temp_timelineitem.m_CategoryData = temp.m_TimelineData.m_TimelineItems[i].m_CategoryData.GetValue();
		temp_timelineitem.m_EndTime = temp.m_TimelineData.m_TimelineItems[i].m_EndTime.GetValue();
		temp_timelineitem.m_ItemLocator = temp.m_TimelineData.m_TimelineItems[i].m_ItemLocator.GetValue();
		temp_timelineitem.m_StartTime = temp.m_TimelineData.m_TimelineItems[i].m_StartTime.GetValue();
		temp_timelineitem.m_TimelineCategory = temp.m_TimelineData.m_TimelineItems[i].m_TimelineCategory.GetValue();
		temp_timelineitem.m_TitleText = itString(temp.m_TimelineData.m_TimelineItems[i].m_TitleText.GetValue().c_str());
		temp_timelineitem.m_TitlecardColor = temp.m_TimelineData.m_TimelineItems[i].m_TitlecardColor.GetValue();
		temp_timelineitem.m_colorChanged = temp.m_TimelineData.m_TimelineItems[i].m_colorChanged.GetValue();
		o_TimelineItems.push_back(temp_timelineitem);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiTimeline::UpdateItemData(const fcuiTimelineItemData& i_OldData, const fcuiTimelineItemData& i_NewData)
{
	QListWidgetItem* widgetItem = NULL;
	int endIndex = m_NumIcons;
	for( int i = 0; i < endIndex; ++i )
	{
		widgetItem = this->item(i);
		fcuiTimelineItem* time_item = dynamic_cast<fcuiTimelineItem*>(widgetItem);
		if(time_item != NULL)
		{
			if(time_item->GetItemData() == i_OldData)
			{
				time_item->SetItemData(i_NewData);
				m_UpdateData = time_item;
				m_ProcessItem = true;
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiTimeline::AdjustItemPosition(int i_AdjustAmount)
{
	QListWidgetItem* widgetItem = NULL;
	int endIndex = m_NumIcons;
	for( int i = 0; i < endIndex; ++i )
	{
		widgetItem = this->item(i);
	}

}

//----------------------------------------------------------------------------
/// Set 2 scroll buttons from the ui as this timeline's scroll buttons
//----------------------------------------------------------------------------
void fcuiTimeline::SetTimelineScrollButtons( QPushButton* i_pToHome, QPushButton* i_pToEnd )
{
	m_pScrollButtonHome = i_pToHome;
	m_pScrollButtonEnd = i_pToEnd;

	QObject::connect(m_pScrollButtonHome, SIGNAL(pressed()), this, SLOT(ScrollHome()));
	QObject::connect(m_pScrollButtonEnd, SIGNAL(pressed()), this, SLOT(ScrollEnd()));
}

//----------------------------------------------------------------------------
/// make the scroll buttons belonging to the timeline visible
//----------------------------------------------------------------------------
void fcuiTimeline::ShowScrollButtons(bool i_bShow)
{
	if( (m_pScrollButtonHome != NULL) && (m_pScrollButtonEnd != NULL) )
	{
		if(i_bShow)
		{
			m_pScrollButtonHome->show();
			m_pScrollButtonEnd->show();
		}
		else
		{
			m_pScrollButtonHome->hide();
			m_pScrollButtonEnd->hide();
		}
	}
}

//----------------------------------------------------------------------------
/// show the timeline buttons
//----------------------------------------------------------------------------
void fcuiTimeline::ShowButtons()
{
	m_pEditButton->show();
	m_pDeleteButton->show();
}

//----------------------------------------------------------------------------
/// Hide the timeline buttons
//----------------------------------------------------------------------------
void fcuiTimeline::HideButtons()
{
	m_pEditButton->hide();
	m_pDeleteButton->hide();
}

//----------------------------------------------------------------------------
/// Scroll to the top of the panel
//----------------------------------------------------------------------------
void fcuiTimeline::ScrollHome()
{
	QScrollBar* scrollBar;
	scrollBar = horizontalScrollBar();

	int val = scrollBar->value();
	int newVal = val - 1;
	scrollBar->setSliderPosition(newVal);
	//UpdateButtonPosition();
}

//----------------------------------------------------------------------------
/// Scroll to the end of the panel
//----------------------------------------------------------------------------
void fcuiTimeline::ScrollEnd()
{
	QScrollBar* scrollBar;
	scrollBar = horizontalScrollBar();

	int val = scrollBar->value();
	int newVal = val + 1;
	scrollBar->setSliderPosition(newVal);
	//UpdateButtonPosition();
}
