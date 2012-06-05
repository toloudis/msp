/*****************************************************************************
**	fcuiTimelineItem.hpp
**
**		The items that are created after a drag event into the action timeline
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCUI_TIMELINEITEM_HPP
#error fcuiTimelineItem.hpp multiply included
#endif
#define FCUI_TIMELINEITEM_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef MA_TIME_HPP
#include "Core/Ma/maTime.hpp"
#endif
#include <QtGui>
#include <QGraphicsPixmapItem>


//============================================================================
//============================================================================
class fcuiTimelineItemData
{
public:
	fcuiTimelineItemData(){m_colorChanged = false;}
	fsLocator m_ItemLocator;
	itString m_TimelineCategory;
	itString m_CategoryData;  //camera name for sequences, texture driver name for title cards, etc.
	maTime m_StartTime;			//TIME - switch to maTime
	maTime m_EndTime;			//TIME - switch to maTime
	itString m_TitleText;
	itString m_BodyText;
	float m_BillboardPosX;
	itString m_BillboardName;
	maFloatRGBA m_TitlecardColor;
	bool m_colorChanged;


	bool operator ==(const fcuiTimelineItemData& i_Value) const
	{
		return  (m_ItemLocator == i_Value.m_ItemLocator) && 
				(m_TimelineCategory == i_Value.m_TimelineCategory) &&
				(m_CategoryData == i_Value.m_CategoryData) &&
				(m_StartTime == i_Value.m_StartTime) &&
				(m_EndTime == i_Value.m_EndTime) &&
				(m_TitleText == i_Value.m_TitleText) &&
				(m_BodyText == i_Value.m_BodyText) &&
				(m_BillboardPosX == i_Value.m_BillboardPosX)&&
				(m_TitlecardColor == i_Value.m_TitlecardColor)&&
				(m_colorChanged == i_Value.m_colorChanged);
	}
};

//============================================================================
//============================================================================
class fcuiTimelineItem :  public QListWidgetItem
{
public:
	//------------------------------------------------------------------------
	/// Constructor
	//------------------------------------------------------------------------
	fcuiTimelineItem(QListWidget* i_pParentView = NULL);
	fcuiTimelineItem::fcuiTimelineItem(const fcuiTimelineItem& i_Copy);

	//------------------------------------------------------------------------
	/// Destructor
	//------------------------------------------------------------------------
	~fcuiTimelineItem();

	//------------------------------------------------------------------------
	/// Creates an exact copy of the item.
	//------------------------------------------------------------------------
	QListWidgetItem *clone() const;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fcuiTimelineItem &operator=(const fcuiTimelineItem &other);
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetItemData(const fcuiTimelineItemData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const fcuiTimelineItemData& GetItemData();

private:
	fcuiTimelineItemData m_Data;
};

