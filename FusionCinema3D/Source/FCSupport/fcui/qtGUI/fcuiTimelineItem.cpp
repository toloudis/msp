/*****************************************************************************
**	fcuiTimelineItem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcui/qtGUI/fcuiTimelineItem.hpp"

#include "Core/it/itStringUtil.hpp"

//----------------------------------------------------------------------------
/// Constructor
//----------------------------------------------------------------------------
fcuiTimelineItem::fcuiTimelineItem(QListWidget* i_pParentView)
:   QListWidgetItem(i_pParentView, QListWidgetItem::UserType)
{

}

//----------------------------------------------------------------------------
/// Constructor
//----------------------------------------------------------------------------
fcuiTimelineItem::fcuiTimelineItem(const fcuiTimelineItem& i_Copy)
:   QListWidgetItem((QListWidgetItem)i_Copy)
{
	this->m_Data = i_Copy.m_Data;
	//update the tooltip
	itString name("");
	if(m_Data.m_ItemLocator.GetNumNames() > 0)
		name = m_Data.m_ItemLocator.GetLastName();
	QString tip(itStringUtil::GetStdString(name).c_str());
	setToolTip(tip);
}

//----------------------------------------------------------------------------
/// Destructor
//----------------------------------------------------------------------------
fcuiTimelineItem::~fcuiTimelineItem()
{

}

//----------------------------------------------------------------------------
/// Creates an exact copy of the item.  Overrides QListWidgetItem::clone()
//----------------------------------------------------------------------------
QListWidgetItem *fcuiTimelineItem::clone() const
{
    return new fcuiTimelineItem(*this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fcuiTimelineItem &fcuiTimelineItem::operator=(const fcuiTimelineItem &other)
{
	(QListWidgetItem)*this = (QListWidgetItem)other;
	m_Data = other.m_Data;
	//update the tooltip
	itString name("");
	if(m_Data.m_ItemLocator.GetNumNames() > 0)
		name = m_Data.m_ItemLocator.GetLastName();
	QString tip(itStringUtil::GetStdString(name).c_str());
	setToolTip(tip);
    return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiTimelineItem::SetItemData(const fcuiTimelineItemData& i_Data)
{
	m_Data = i_Data;
	//update the tooltip
	itString name("");
	if(m_Data.m_ItemLocator.GetNumNames() > 0)
		name = m_Data.m_ItemLocator.GetLastName();
	QString tip(itStringUtil::GetStdString(name).c_str());
	setToolTip(tip);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const fcuiTimelineItemData& fcuiTimelineItem::GetItemData()
{
	return m_Data;
}
