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

#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
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
		// envType::UInt8 []. This structure will take ownership of the
		// data and delete it in the destructor
		//--------------------------------------------------------------------
		void SetThumbData(envType::UInt8* i_ThumbData, 
						  int i_BufferSize,
						  int i_BitmapSize);

		//--------------------------------------------------------------------
		//	Thumbnail Info
		//--------------------------------------------------------------------
		bool GetHasThumb() const;
		//void SetHasThumb(bool i_bHasThumb);
		envType::UInt8* GetThumbParams() const;
		int GetBufferSize() const;
		int GetBitmapSize() const;
		//envType::UInt8* ThumbParams();

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
		envType::UInt8* m_ThumbData;
		int m_BufferSize;
		int m_BitmapSize;

};


