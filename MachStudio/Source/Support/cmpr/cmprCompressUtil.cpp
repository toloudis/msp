/*****************************************************************************
**	cmprCompressUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/cmpr/cmprCompressUtil.hpp"

#include "Support/cmpr/private/cmprZipUtil.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/it/itStringUtil.hpp"


//============================================================================
//============================================================================
namespace cmprCompressUtil
{
	namespace
	{
		//default compression type to zip
		int l_CompressionType = e_Zip;
	}

	//------------------------------------------------------------------------
	// Set the Compression type that the Util should use
	//------------------------------------------------------------------------
	void SetCompressionType( int i_CompressionType )
	{
		l_CompressionType = i_CompressionType;
	}

	//------------------------------------------------------------------------
	// Get the file extension associated with the current compression type
	//------------------------------------------------------------------------
	void GetCompressionEXT(std::string& o_CompressionEXT)
	{
		switch(l_CompressionType)
		{
		case e_Zip:
			o_CompressionEXT = "zip";
			break;
		default:
			DBG_ERROR("Compression Type not recognized");
			break;
		}
	}

	//------------------------------------------------------------------------
	// Do the necessary preperations before compressing a file.  
	// Based on current compression type
	//------------------------------------------------------------------------
	void PrepareCompression()
	{
		switch(l_CompressionType)
		{
		case e_Zip:
			cmprZipUtil::PrepareZip();
			break;
		default:
			DBG_ERROR("Compression Type not recognized");
			break;
		}
	}

	//------------------------------------------------------------------------
	// Do the file compression
	//------------------------------------------------------------------------
	bool CompressFile( const fsLocator& i_SourceFile, fsLocator& io_DestPath )
	{
		bool bCompressSuccess = false;
		switch(l_CompressionType)
		{
		case e_Zip:
			bCompressSuccess = cmprZipUtil::ZipFile(i_SourceFile, io_DestPath);
			break;
		default:
			DBG_ERROR("Compression Type not recognized");
			break;
		}
		return bCompressSuccess;
	}

}  // end cmprCompressUtil