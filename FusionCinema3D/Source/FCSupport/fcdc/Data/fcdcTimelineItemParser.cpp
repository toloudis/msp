/********************************************************************************************\
**	mnmTitleCardParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#include "FCSupport/fcdc/data/fcdcTimelineItemParser.hpp"

//#include "FCSupport/fcdc/data/fcdcTimelineItemData.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
//#include "Core/ch/chWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"


//============================================================================
//============================================================================
namespace fcdcTimelineItemParser
{
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_FC3D = chDefs::MakeName('F', 'C', '3', 'D');	
	const chDefs::Name c_MPCK = chDefs::MakeName('M', 'P', 'C', 'K');
	const chDefs::Name c_CTLL = chDefs::MakeName('C', 'T', 'L', 'L');
	const chDefs::Name c_CTLD = chDefs::MakeName('C', 'T', 'L', 'D');
	const chDefs::Name c_HLHT = chDefs::MakeName('H', 'L', 'H', 'T');
	const chDefs::Name c_HLHD = chDefs::MakeName('H', 'L', 'H', 'D');

	///------------------------------------------------------------------------
	///Read Movie pack name and movie title
	///------------------------------------------------------------------------
	void ReadProjectData(chReader& io_Reader,
						chDefs::Version i_Version,
						fcdcProjectData& o_Data )
	{
		o_Data.m_Title.Read(io_Reader);
		o_Data.m_MoviePackName.Read(io_Reader);
	}

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------
	void WriteProjectData(chWriter& o_Writer,
						const fcdcProjectData& i_Data)
	{
		const int l_c_MPCK_VERSION = 0;
		o_Writer.WriteChunkHeader(c_MPCK, l_c_MPCK_VERSION, false);

		i_Data.m_Title.Write(o_Writer);
		i_Data.m_MoviePackName.Write(o_Writer);

		o_Writer.FinishChunk();
	}

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------
	void ReadTimelineItemData(chReader& io_Reader,
						chDefs::Version i_Version,
						fcdcTimelineItem& o_TimelineItemData )
	{
		o_TimelineItemData.m_ItemLocator.Read(io_Reader);
		o_TimelineItemData.m_TimelineCategory.Read(io_Reader);
		o_TimelineItemData.m_CategoryData.Read(io_Reader);
		o_TimelineItemData.m_StartTime.Read(io_Reader);
		o_TimelineItemData.m_EndTime.Read(io_Reader);
		o_TimelineItemData.m_TitleText.Read(io_Reader);
		o_TimelineItemData.m_BodyText.Read(io_Reader);
		o_TimelineItemData.m_BillboardPosX.Read(io_Reader);
		o_TimelineItemData.m_BillboardName.Read(io_Reader);
		o_TimelineItemData.m_TitlecardColor.Read(io_Reader);
	}

	////------------------------------------------------------------------------
	////------------------------------------------------------------------------
	void ReadTimelineItemList(chReader& io_Reader,
						chDefs::Version i_Version,
						std::vector<fcdcTimelineItem>& o_TimelineItems )
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;
	
		int	index = 0;

		while( io_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_CTLD )
			{
				if (index >= o_TimelineItems.size())
					o_TimelineItems.resize( index+1 );

				ReadTimelineItemData(io_Reader, version, o_TimelineItems[index++]);
			}
			else
			{
				//DBG_ASSERT(false, "invalid chunk header");
				DBG_TRACE("invalid chunk header" << name);
			}
			io_Reader.FinishChunk();
		}
	}
	
	////------------------------------------------------------------------------
	////------------------------------------------------------------------------
	void WriteTimelineItemData(chWriter& o_Writer,
						 fcdcTimelineItem& i_TimelineItemData)
	{
		const int l_cCTLD_VERSION = 0;
		o_Writer.WriteChunkHeader( c_CTLD, l_cCTLD_VERSION, false );

		i_TimelineItemData.m_ItemLocator.Write(o_Writer);
		i_TimelineItemData.m_TimelineCategory.Write(o_Writer);
		i_TimelineItemData.m_CategoryData.Write(o_Writer);
		i_TimelineItemData.m_StartTime.Write(o_Writer);
		i_TimelineItemData.m_EndTime.Write(o_Writer);
		i_TimelineItemData.m_TitleText.Write(o_Writer);
		i_TimelineItemData.m_BodyText.Write(o_Writer);
		i_TimelineItemData.m_BillboardPosX.Write(o_Writer);
		i_TimelineItemData.m_BillboardName.Write(o_Writer);
		i_TimelineItemData.m_TitlecardColor.Write(o_Writer);

		o_Writer.FinishChunk();
	}

	////------------------------------------------------------------------------
	////------------------------------------------------------------------------
	void WriteTimelineData(chWriter& o_Writer,
							fcdcTimelineData i_TimelineData)
	{ 
		const int l_cCTLL_VERSION = 0;
		o_Writer.WriteChunkHeader( c_CTLL, l_cCTLL_VERSION, true );

		std::vector<fcdcTimelineItem>::iterator it, end = i_TimelineData.m_TimelineItems.end();
		for (it = i_TimelineData.m_TimelineItems.begin(); it != end; ++it)
		{
			WriteTimelineItemData(o_Writer, (*it));
		}

		o_Writer.FinishChunk();
	}


	///------------------------------------------------------------------------
	///------------------------------------------------------------------------
	void ReadHighlightData(chReader& io_Reader,
						chDefs::Version i_Version,
						fcdcHighlightItem& o_HighlightItem )
	{
		o_HighlightItem.m_categorypath.Read(io_Reader);
		o_HighlightItem.m_elementpath.Read(io_Reader);
	}

	////------------------------------------------------------------------------
	////------------------------------------------------------------------------
	void ReadHighlightItemList(chReader& io_Reader,
						chDefs::Version i_Version,
						std::vector<fcdcHighlightItem>& o_HighlightItems )
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;
	
		int	index = 0;

		while( io_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_HLHD )
			{
				if (index >= o_HighlightItems.size())
					o_HighlightItems.resize( index+1 );

				ReadHighlightData(io_Reader, version, o_HighlightItems[index++]);
			}
			else
			{
				//DBG_ASSERT(false, "invalid chunk header");
				DBG_TRACE("invalid chunk header" << name);
			}
			io_Reader.FinishChunk();
		}
	}
	
	////------------------------------------------------------------------------
	////------------------------------------------------------------------------
	void WriteHighlightItemData(chWriter& o_Writer,
						 fcdcHighlightItem& i_HighlightItemData)
	{
		const int l_cHLHD_VERSION = 0;
		o_Writer.WriteChunkHeader( c_HLHD, l_cHLHD_VERSION, false);

		i_HighlightItemData.m_categorypath.Write(o_Writer);
		i_HighlightItemData.m_elementpath.Write(o_Writer);
		
		o_Writer.FinishChunk();
	}

	////------------------------------------------------------------------------
	////------------------------------------------------------------------------
	void WriteHighlightData(chWriter& o_Writer,
							fcdcHighlightData i_HighlightData)
	{
		const int l_cHLHT_VERSION = 0;
		o_Writer.WriteChunkHeader( c_HLHT, l_cHLHT_VERSION,true);

		std::vector<fcdcHighlightItem>::iterator it, end = i_HighlightData.m_HighlightItems.end();
		for (it = i_HighlightData.m_HighlightItems.begin(); it != end; ++it)
		{
			WriteHighlightItemData(o_Writer, (*it));
		}

		o_Writer.FinishChunk();
	}
}	// local namespace


//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name GetChunkName()
{
	return c_FC3D;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void ReadData(	chReader& io_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								fcdcTimelineItemData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( io_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_CTLL )
		{
			ReadTimelineItemList(io_Reader, version, o_Data.m_TimelineData.m_TimelineItems);
		}
		else if ( name == c_MPCK )
		{
			ReadProjectData(io_Reader, version, o_Data.m_ProjectData);
		}
		else if ( name == c_HLHT )
		{
			ReadHighlightItemList(io_Reader, version, o_Data.m_HighlightData.m_HighlightItems);
		}
		else
		{
			//DBG_ASSERT(false, "invalid chunk header");
			DBG_TRACE("invalid chunk header" << name);
		}
		io_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void WriteData(chWriter& io_Writer,
			const fcdcTimelineItemData& i_Data )
{
	const int l_cFC3D_VERSION = 0;
	io_Writer.WriteChunkHeader( c_FC3D, l_cFC3D_VERSION, true );

	WriteProjectData(io_Writer, i_Data.m_ProjectData);		// MPCK

	WriteTimelineData(io_Writer, i_Data.m_TimelineData);	// CTLL

	WriteHighlightData(io_Writer, i_Data.m_HighlightData);	// HLHT

	io_Writer.FinishChunk();
}

}// end of namespace

