/****************************************************************************\
**  cptrWriteQuickTime.hpp
**
**      Supplies routines for writing the QuickTime format
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_WRITEQUICKTIME_HPP
#error cptrWriteQuickTime.hpp multiply included
#endif
#define CPTR_WRITEQUICKTIME_HPP


//============================================================================
//	forward references
//============================================================================
class fsLocator;
class g2dImage;
class g2dWindow;
class cptrRenderOutputData;


//============================================================================
//============================================================================
namespace cptrWriteQuickTime
{
	//------------------------------------------------------------------------
	//	write a frame to the movie
	//------------------------------------------------------------------------
	void Write( g2dWindow* i_pWindow, const fsLocator& i_Locator, float i_fCurrentTime, const int i_Sampling = 1 );

	//------------------------------------------------------------------------
	//	write a frame to the movie
	//------------------------------------------------------------------------
	void Write( g2dImage* i_pSurface, const fsLocator& i_Locator, float i_fCurrentTime, const int i_Sampling = 1 );

	//------------------------------------------------------------------------
	//	BeginCapture/EndCapture() - notify start and stop capturing so that
	//		movie files can be begun and ended
	//------------------------------------------------------------------------
	void BeginCapture(g2dWindow* i_pWindow, cptrRenderOutputData& i_Data);
	void EndCapture();

	//------------------------------------------------------------------------
	//	WriteAudio() - write the audio file to the movie.  This must get
	//	called after the frames are written right before EndCapture().
	//------------------------------------------------------------------------
	void WriteAudio(const fsLocator& i_AudioFile);

	//------------------------------------------------------------------------
	//	WriteTimeCode() - write the timecode into the movie.
	//------------------------------------------------------------------------
	void WriteTimeCode( const float i_fStartTime, const float i_fEndTime );
}
