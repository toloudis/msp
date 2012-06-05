/*****************************************************************************
**	fcuiTimeline.hpp
**
**		This is the timeline panel for the Action mode.  
**		Receives drag/drop events.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCUI_TIMELINE_HPP
#error fcuiTimeline.hpp multiply included
#endif
#define FCUI_TIMELINE_HPP

#include <QtGui>


//============================================================================
//============================================================================
class fcuiTimelineItem;
class fcuiTimelineItemData;
class maTime;

//============================================================================
//============================================================================
class fcuiTimeline : public QListWidget
{
	Q_OBJECT

public:
	//------------------------------------------------------------------------
	/// Constructor
	//------------------------------------------------------------------------
	fcuiTimeline(QWidget* i_pParent);
	
	//------------------------------------------------------------------------
	/// Destructor
	//------------------------------------------------------------------------
	~fcuiTimeline();

	///---------------------------------------------------------------------------
	/// Process timeline events when they need to happen on a subsequent frame
	///---------------------------------------------------------------------------
	void Think();

	//------------------------------------------------------------------------
	/// Populate the timeline given a vector of timeline item properties
	//------------------------------------------------------------------------
	void PopulateTimeline(const std::vector<fcuiTimelineItemData>& i_TimelineItems);

	//------------------------------------------------------------------------
	/// Process the event when a drag enter occurs
	/// overloaded Qt function
	//------------------------------------------------------------------------
	void dragEnterEvent(QDragEnterEvent* event);

	//------------------------------------------------------------------------
	/// Process the event when an item is moved within the widget
	/// overloaded Qt function
	//------------------------------------------------------------------------
	void dragMoveEvent(QDragMoveEvent* event);

	//------------------------------------------------------------------------
	/// Process the event when an item is dropped into the widget
	/// overloaded Qt function
	//------------------------------------------------------------------------
	void dropEvent(QDropEvent* event);

	//------------------------------------------------------------------------
	/// Do the drop in animation for the timeline
	//------------------------------------------------------------------------
	void AnimateDropIn();

	//------------------------------------------------------------------------
	/// Do the drop out animation for the timeline
	//------------------------------------------------------------------------
	void AnimateDropOut();

	//------------------------------------------------------------------------
	/// Select the timeline item at the given time
	//------------------------------------------------------------------------
	void SelectItemAtTime(const maTime& i_CurrentTime);
	
	//----------------------------------------------------------------------------
	/// Write Timeline items into the chunk
	//----------------------------------------------------------------------------
	void WriteChunk();

	//----------------------------------------------------------------------------
	/// Read Timeline items from the chunk
	//----------------------------------------------------------------------------
	void ReadChunk(std::vector<fcuiTimelineItemData>& o_TimelineItems);

	//------------------------------------------------------------------------
	/// find the timeline item that matches the old data and update the item
	//------------------------------------------------------------------------
	void UpdateItemData(const fcuiTimelineItemData& i_OldData, const fcuiTimelineItemData& i_NewData);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void AdjustItemPosition(int i_AdjustAmount);

	///-----------------------------------------------------------------------
	/// Set 2 scroll buttons from the ui as this timeline's scroll buttons
	///-----------------------------------------------------------------------
	void SetTimelineScrollButtons( QPushButton* i_pToHome, QPushButton* i_pToEnd );

	///-----------------------------------------------------------------------
	/// make the scroll buttons belonging to the timeline visible
	///-----------------------------------------------------------------------
	void ShowScrollButtons(bool i_bShow);

	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	void SetIsEditing(bool i_bEditing);

private slots:
	//------------------------------------------------------------------------
	/// Slots for item interactions within the fcuiTimeline
	//------------------------------------------------------------------------
	void TimelineItemClicked( QListWidgetItem* i_Item );
	void TimelineItemPressed( QListWidgetItem* i_Item );
	void TimelineItemChanged( QListWidgetItem* i_Item );
	void ScrollBarMoved(int i_ScrollValue );
	void DeleteButton_Clicked();
	void EditButton_Clicked();
	void FinishDropOut();

	//------------------------------------------------------------------------
	/// Scroll the view towards the proper direction
	//------------------------------------------------------------------------
	void ScrollHome();
	void ScrollEnd();

private:
	//----------------------------------------------------------------------------
	/// Given the duration of the item, return the width the icon should take in
	/// the timeline
	//----------------------------------------------------------------------------
	int GetTimelineItemSize( const maTime& i_ItemDuration );

	//------------------------------------------------------------------------
	/// Shift a single item's drivers to the time matching their new index in 
	/// the fc3d timeline
	//------------------------------------------------------------------------
	void ShiftItem(int i_NewReorderIndex);

	//------------------------------------------------------------------------
	/// Shift all item drivers that are affected by the shift of an item
	/// in the fc3d timeline
	//------------------------------------------------------------------------
	void ShiftItems(int i_NewReorderIndex, const maTime& i_Duration);

	//------------------------------------------------------------------------
	/// Remove an item's drivers from the timeline
	//------------------------------------------------------------------------
	void RemoveItem(fcuiTimelineItem* i_pItem);

	//------------------------------------------------------------------------
	/// Re-position the additional buttons that appear when an item is clicked
	//------------------------------------------------------------------------
	void PositionButtons(const QRect& i_ItemRect);

	//------------------------------------------------------------------------
	/// If the timeline is scrolled, we need to update the position of our buttons
	//------------------------------------------------------------------------
	void UpdateButtonPosition();
	
	//------------------------------------------------------------------------
	/// show the timeline buttons
	//------------------------------------------------------------------------
	void ShowButtons();

	//------------------------------------------------------------------------
	/// Hide the timeline buttons
	//------------------------------------------------------------------------
	void HideButtons();

private:
	int m_IconWidth;
	int m_IconHeight;
	int m_NumIcons;
	int m_MidDropRow;
	bool m_bEditing;
	bool m_bItemShift;
	bool m_bMiddleDrop;
	bool m_bMidDropStart;
	bool m_ProcessItem;
	QRect m_StartPosition;
	QRect m_HidePosition;
	fcuiTimelineItem* m_UpdateData;
	fcuiTimelineItem* m_pReorderOriginalItem;
	fcuiTimelineItem* m_pClickedItem;
	fcuiTimelineItem* m_pShiftedItem;
	fcuiTimelineItem* m_MidDropItem;
	fcuiTimelineItem* m_CurrentPlaybackItem;
	int m_pReorderOriginalRow;
	QPushButton* m_pDeleteButton;
	QPushButton* m_pEditButton;
	QPushButton* m_pScrollButtonHome;
	QPushButton* m_pScrollButtonEnd;
};