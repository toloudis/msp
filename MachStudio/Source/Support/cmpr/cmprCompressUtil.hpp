/*****************************************************************************
**	cmprCompressUtil.hpp
**
**		API for Compressing files
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef CMPR_COMPRESS_UTIL_HPP
#error cmprCompressUtil.hpp multiply included
#endif
#define CMPR_COMPRESS_UTIL_HPP

#include<string>

//----------------------------------------------------------------------------
//Forward Declarations
//----------------------------------------------------------------------------
class fsLocator;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace cmprCompressUtil
{
	//------------------------------------------------------------------------
	//  Implemented Compression types
	//------------------------------------------------------------------------
	enum CompressionTypes
	{
		e_Zip = 0,
		e_BZip2,
		e_Tar_BZip2,
		e_7Zip
	};

	//------------------------------------------------------------------------
	// Set the Compression type that the Util should use
	//------------------------------------------------------------------------
	void SetCompressionType( int i_CompressionType );

	//------------------------------------------------------------------------
	// Get the file extension associated with the current compression type
	//------------------------------------------------------------------------
	void GetCompressionEXT(std::string& o_CompressionEXT);

	//------------------------------------------------------------------------
	// Do the necessary preperations before compressing a file.  
	// Based on current compression type
	//------------------------------------------------------------------------
	void PrepareCompression();

	//------------------------------------------------------------------------
	// Do the file compression
	//------------------------------------------------------------------------
	bool CompressFile( const fsLocator& i_SourceFile, fsLocator& io_DestPath );


} // end cmprCompressUtil namespace