/****************************************************************************\
**  g2dScanlineOpenEXRUtil.hpp
**
**		OpenEXR file format support
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_SCANLINEEXRUTIL_HPP
#error g2dScanlineOpenEXRUtil.hpp multiply included
#endif
#define CPTR_SCANLINEEXRUTIL_HPP


//============================================================================
//============================================================================
class fsLocator;
class g2dImage;
class itString;


//============================================================================
//============================================================================
namespace g2dScanlineOpenEXRUtil
{
	//--------------------------------------------------------------------
	// Function to invoke writing of Scanline EXR file
	//--------------------------------------------------------------------
	void WriteToScanlineEXR( const fsLocator& i_Locator, const g2dImage* pImg );
}
