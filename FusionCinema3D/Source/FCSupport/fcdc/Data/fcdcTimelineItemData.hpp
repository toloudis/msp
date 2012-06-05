/********************************************************************************************\
**	fcdcTimelineItemData.hpp.hpp
**
**		Data structure for the Timeline chunk
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#ifdef FCDC_TIMELINEITEMDATA_HPP
#error fcdcTimelineItemData.hpp multiply included
#endif
#define FCDC_TIMELINEITEMDATA_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif 
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_TIME_HPP
#include "Core/prty/prtyTime.hpp"
#endif 
#ifndef PRTY_BOOL_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif

#ifndef FCDC_TITLEITEMDATA_HPP
#include "FCSupport/fcdc/Data/fcdcTitleItemData.hpp"
#endif



//============================================================================
//============================================================================
struct fcdcHighlightItem
{
	prtyFilePath m_categorypath;
	prtyFilePath m_elementpath;
};


//============================================================================
//============================================================================
struct fcdcHighlightData
{
	std::vector<fcdcHighlightItem> m_HighlightItems;
};

//============================================================================
//============================================================================
struct fcdcTimelineItem
{
	prtyFilePath m_ItemLocator;
	prtyFileName m_TimelineCategory;
	prtyFileName m_CategoryData;
	prtyTime m_StartTime;
	prtyTime m_EndTime;
	prtyText m_TitleText;
	prtyText m_BodyText;
	prtyFloat m_BillboardPosX;
	prtyFileName m_BillboardName;
	prtyColor m_TitlecardColor;
	prtyBoolean m_colorChanged;

};

//============================================================================
//============================================================================
struct fcdcTimelineData
{
	std::vector<fcdcTimelineItem> m_TimelineItems;
};

//============================================================================
//============================================================================
struct fcdcProjectData
{
	prtyText m_Title;  // The movie title
	prtyText m_MoviePackName; // The movie pack name
};

//============================================================================
//============================================================================
struct fcdcTimelineItemData
{
	//	Project Data
	fcdcProjectData m_ProjectData;

	//	Timeline data
	fcdcTimelineData m_TimelineData;

	// Items to highlight
	fcdcHighlightData m_HighlightData;
};

