/*****************************************************************************
**	cmprZipUtil.hpp
**
**		API for Compressing files into a Zip archive
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef CMPR_ZIP_UTIL_HPP
#error cmprZipUtil.hpp multiply included
#endif
#define CMPR_ZIP_UTIL_HPP

#include <string>

//----------------------------------------------------------------------------
//Forward Declarations
//----------------------------------------------------------------------------
class fsLocator;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace cmprZipUtil
{
	//------------------------------------------------------------------------
	// Do the necessary preperations before compressing a file.  
	// Based on current compression type
	//------------------------------------------------------------------------
	void PrepareZip();

	//------------------------------------------------------------------------
	// Do the file compression
	//------------------------------------------------------------------------
	bool ZipFile( const fsLocator& i_SourceFile, fsLocator& io_DestPath );


} // end cmprCompressUtil namespace