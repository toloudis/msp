/****************************************************************************\
**  mnmQuickTimeUtil.hpp
**
**      Supplies routines for saving QuickTime movie files.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef MNM_QUICKTIMEUTIL_HPP
#error mnmQuickTimeUtil.hpp multiply included
#endif
#define MNM_QUICKTIMEUTIL_HPP


#include <windows.h>
#include <string>

// compiler flag to turn off quicktime
#ifndef WIN64 
#define USE_QUICKTIME 1
#endif

#ifdef USE_QUICKTIME

//============================================================================
//	Forward References
//============================================================================
class g2dWindow;
class g2dImage;


//============================================================================
//============================================================================
namespace mnmQuickTimeUtil
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetFramesPerSecond(const float i_FramesPerSecond);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void SetCodec(const std::string& i_CodeType);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	HRESULT InitGraphicsWorld(HDC hBackDC,HBITMAP hBackBitmap,LPCTSTR lpszFileName,g2dWindow* i_pWindow);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	HRESULT	AppendNewFrame(float i_fCurrentTime, g2dImage* i_pSurface);

	//------------------------------------------------------------------------
	//	WriteTimeCode() - write the timecode into the movie.
	//------------------------------------------------------------------------
	void WriteTimeCode( const float i_fStartTime, const float i_fEndTime );

	//------------------------------------------------------------------------
	//	WriteWavAudio() - write an audio track into the movie
	//------------------------------------------------------------------------
	HRESULT WriteWavAudio(const char* i_AudioFile);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	HRESULT Close();
}


#endif // USE_QUICKTIME
