/****************************************************************************\
**	mnmTitleCardMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcdc/fcdcDataMgr.hpp"

#include "FCSupport/fcdc/data/fcdcTimelineItemData.hpp"
#include "FCSupport/fcdc/fcdcDataInterest.hpp"

//============================================================================
//============================================================================
namespace
{
	//std::vector<fcdcDataInterest*> l_InterestList;
	fcdcTimelineItemData l_DataItem;

	//bool l_bDisableNotify = false;
	//void notify_interests_changed()
	//{
	//	if (l_bDisableNotify) return;
	//	std::vector<fcdcDataInterest*>::iterator it, end = l_InterestList.end();
	//	for (it  = l_InterestList.begin(); it != end; ++it)
	//	{
	//		(*it)->DataChanged();
	//	} 
	//}
	//void notify_interests_added()
	//{
	//	if (l_bDisableNotify) return;
	//	std::vector<fcdcDataInterest*>::iterator it, end = l_InterestList.end();
	//	for (it  = l_InterestList.begin(); it != end; ++it)
	//	{
	//		(*it)->TimelineItemAdded();
	//	}
	//}
	//void notify_interests_removed(const float& i_StartTime)
	//{
	//	if (l_bDisableNotify) return;
	//	std::vector<fcdcDataInterest*>::iterator it, end = l_InterestList.end();
	//	for (it  = l_InterestList.begin(); it != end; ++it)
	//	{
	//		(*it)->TimelineItemRemoved(i_StartTime);
	//	}
	//}
	//void notify_interests_renamed()
	//{
	//	if (l_bDisableNotify) return;
	//	std::vector<fcdcDataInterest*>::iterator it, end = l_InterestList.end();
	//	for (it  = l_InterestList.begin(); it != end; ++it)
	//	{
	//		(*it)->TitleRenamed();
	//	}
	//}
}


//============================================================================
//============================================================================
namespace fcdcDataMgr
{
	//------------------------------------------------------------------------
	//	Get/Set TitleCard Data
	//------------------------------------------------------------------------
	void fcdcDataMgr::SetData( fcdcTimelineItemData& i_Data )
	{
		l_DataItem = i_Data;
	}

	fcdcTimelineItemData& fcdcDataMgr::GetData()
	{
		return l_DataItem;
	}
}
//------------------------------------------------------------------------
//	Set the application specific data
//------------------------------------------------------------------------
//void fcdcTimelineItemMgr::SetTimelineItemData( itString i_TitleText, itString i_BodyText)
//{
//	l_AppData.m_TitleText = i_TitleText;
//	l_AppData.m_BodyText = i_BodyText;
//}

