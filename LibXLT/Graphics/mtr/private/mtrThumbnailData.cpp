/****************************************************************************\
**	mtrThumbnailData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mtr/mtrThumbnailData.hpp"

#include "Core/dbg/dbgMsg.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mtrThumbnailData::mtrThumbnailData()
:	m_ThumbData(NULL), m_BufferSize(0), m_BitmapSize(0)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mtrThumbnailData::~mtrThumbnailData()
{
	if (m_ThumbData)
		delete [] m_ThumbData;
}

//--------------------------------------------------------------------
//	Give thumbnail info for this data structrue to manage,
//  The i_ThumbData should have been allocated using new 
// envType::UInt8 []. This structure will take ownership of the
// data and delete it in the destructor
//--------------------------------------------------------------------
void mtrThumbnailData::SetThumbData(envType::UInt8* i_ThumbData, 
									int i_BufferSize,
									int i_BitmapSize)
{
	// delete old data that we own
	if (m_ThumbData)
		delete [] m_ThumbData;

	// ASsign new data
	m_ThumbData = i_ThumbData;
	m_BufferSize = i_BufferSize;
	m_BitmapSize = i_BitmapSize;
}

//--------------------------------------------------------------------
//	Thumbnail Image Info
//--------------------------------------------------------------------
bool mtrThumbnailData::GetHasThumb() const
{
	// Return true if we have thumbnail data and it is enabled
	//return (m_bHasThumb && m_ThumbData);
	return (m_ThumbData != NULL);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//void mtrThumbnailData::SetHasThumb(bool i_bHasThumb)
//{
//	m_bHasThumb = i_bHasThumb;
//
//	// Create the thumbnail parameters if needed, but keep old values
//	// if turning thumbnail off (values remain same when thumbnail is turned back on).
//	if (i_bHasThumb && !m_ThumbData)
//	{
//		m_ThumbData = new envType::UInt8();
//	}
//}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envType::UInt8* mtrThumbnailData::GetThumbParams() const
{
	DBG_ASSERT(m_ThumbData, "Thumbnail Data is NULL, call GetHasThumb() first");
	return m_ThumbData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int mtrThumbnailData::GetBufferSize() const
{
	DBG_ASSERT(m_BufferSize != 0, "Thumbnail Image Size is 0");
	return m_BufferSize;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int mtrThumbnailData::GetBitmapSize() const
{
	DBG_ASSERT(m_BitmapSize != 0, "Thumbnail Image Size is 0");
	return m_BitmapSize;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//envType::UInt8* mtrThumbnailData::ThumbParams()
//{
//	// This function could also be designed to create the data when needed...
//	DBG_ASSERT(m_ThumbData, "Thumbnail Data is NULL, call GetHasThumb() first");
//	return m_ThumbData;
//}
