/*****************************************************************************
**	mtrThumbnailData.hpp
**
**	mtrThumbnailData represents the thumbnail icon for a material file.
**	It contains the material applied to a stylized sphere to be used to
**	display material properties on a standardized geometry.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MTR_THUMBNAILDATA_HPP
#error mtrThumbnailData.hpp multiply included
#endif
#define MTR_THUMBNAILDATA_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif


//============================================================================
//============================================================================
class mtrThumbnailData
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mtrThumbnailData();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~mtrThumbnailData();

		//--------------------------------------------------------------------
		//	Give thumbnail info for this data structrue to manage,
		//  The i_ThumbData should have been allocated using new 
		// uint8_t []. This structure will take ownership of the
		// data and delete it in the destructor
		//--------------------------------------------------------------------
		void SetThumbData(uint8_t* i_ThumbData, 
						  int i_BufferSize,
						  int i_BitmapSize);

		//--------------------------------------------------------------------
		//	Thumbnail Info
		//--------------------------------------------------------------------
		bool GetHasThumb() const;
		//void SetHasThumb(bool i_bHasThumb);
		uint8_t* GetThumbParams() const;
		int GetBufferSize() const;
		int GetBitmapSize() const;
		//uint8_t* ThumbParams();

	private:
		//--------------------------------------------------------------------
		// copy constructor and assignment are not allowed. This would have
		// to copy the thumbnail data and is not the purpose of this class.
		//--------------------------------------------------------------------
		mtrThumbnailData& operator = (const mtrThumbnailData& i_CopyFrom);
		mtrThumbnailData(const mtrThumbnailData& i_CopyFrom);

	private:
		// Thumbnail data.  If pointer is null, then no thumbnail for this material
		//bool m_bHasThumb;
		uint8_t* m_ThumbData;
		int m_BufferSize;
		int m_BitmapSize;

};


