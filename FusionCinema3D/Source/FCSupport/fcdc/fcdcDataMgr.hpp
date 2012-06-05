/****************************************************************************\
**	mnmTitleCardMgr.hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCDC_DATAMGR_HPP
#error fcdcDataMgr.hpp multiply included
#endif
#define FCDC_DATAMGR_HPP


//============================================================================
//============================================================================
class itString;
struct fcdcTimelineItemData;


//============================================================================
//============================================================================
namespace fcdcDataMgr
{
	
	//------------------------------------------------------------------------
	//	Get/Set TitleCard Data
	//		This is the running application's info (name, version, etc)
	//------------------------------------------------------------------------
	void SetData(fcdcTimelineItemData& i_Data );
	fcdcTimelineItemData& GetData();

	//------------------------------------------------------------------------
	//	Set the titlecard specific data
	//------------------------------------------------------------------------
	//void SetTitleCardData( itString i_TitleText, itString i_BodyText );
};

