/****************************************************************************\
**  cptrWriteQuickTime.cpp
**
**      see .hpp
**
**		quicktime constants reference
**		http://developer.apple.com/documentation/QuickTime/Reference/QTRef_Constants/Reference/reference.html#//apple_ref/doc/uid/TP40003455-GlobalQuickTimeAPIConstants-CodecIdentifiers
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrWriteQuickTime.hpp"

#include "Features/Capture/cptrWriteUtil.hpp"

#undef STRICT
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mnm/mnmQuickTimeUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include <windows.h>

#undef CreateDirectory
#undef CreateFile
#undef DeleteFile

#include "Core/app/appApplication.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/g2d/g2dImage.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
#include "Graphics/g2d/g2dWindow.hpp"


//
// TODO: [rjk] clean up the code by removing duplicate BMP code from here.
//


#ifdef USE_QUICKTIME
//============================================================================
//============================================================================
namespace
{
	//
	//	variables
	//
	captRenderOutputData l_Data;

	int l_MovieFileCounter	= 0;
	long double l_MovieSize	= 0;
	const long double c_MovieMaxSize = (2.0L * 1073.741824L) - 0.2f; // size in megabytes (- header size)
	double l_FrameSize = 0.0f;	// in megabytes
	bool l_bFirstFrame = false;
	int l_QuicktimeCount = 0;
	itString l_OutputFilename;

	HDC l_backHDC = 0;
	HBITMAP l_backBitmap = 0;

	bool l_bTempColorFlag = false;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void create_QuickTime_file(g2dWindow* i_pWindow, const fsLocator& i_OutputDirectory, bool i_isResuming)
{
	//	after a filename is generated, if the output size is over the maximum
	//	limit for QuickTimes, then break apart the movie and append a number to
	//	the filename.
	//
	itString full_filename;
	itString base_filename;
	captRenderOutputDataUtil::GenerateFilename( l_OutputFilename );

	if ( l_MovieFileCounter == 0 )
	{
		full_filename = l_OutputFilename;
		base_filename = full_filename;		// before adding the extension
		full_filename += itString(L".mov");
	}
	else
	{
		full_filename = l_OutputFilename;
		full_filename += itString(L"_part");
		std::ostringstream o;
		if (o << (l_MovieFileCounter+1))
			full_filename += itString(o.str().c_str());
		base_filename = full_filename;		// before adding the extension
		full_filename += itString(L".mov");
	}

	// Make sure directory exists
	//
	fsFileUtil::CreateDirectory(i_OutputDirectory);

	//	make sure the file doesn't already exist
	//	if it does exist increment the section count
	//
	fsLocator Temp_loc = i_OutputDirectory;
	int fileCounter = 0;
	Temp_loc.Push(full_filename);

	if (i_isResuming)
	{
		while ( fsFileUtil::FileExists(Temp_loc) ) 
		{	
			//remove the previous file from the directory list because it already exists
			Temp_loc.Pop();

			//append a new section count to check if this file exists
			full_filename = base_filename;
			full_filename += itString(L"_Section");
			std::ostringstream o;
			if (o << (fileCounter+1))
				full_filename += itString(o.str().c_str());
			full_filename += itString(L".mov");
			Temp_loc.Push(full_filename);

			fileCounter++;		
		}
	}

	//DBG_LOG("The new filename is %s" << fnm);

	// set this value so other functions know what the last QuickTime written was
	//
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	data.m_OutputFileName.SetValue( full_filename );
	data.m_OutputMovieDirectory.SetValue( i_OutputDirectory );

	//	create the QuickTime
	//
	fsLocator QuickTime_loc = i_OutputDirectory;
	QuickTime_loc.Push(full_filename);
	
	//	if the file exists, write it in the log
	//
	if (fsFileUtil::FileExists( QuickTime_loc ))
	{
		DBG_WARNING("Overwritting file during render: " << QuickTime_loc);
	}
	
	itString path;
	fsFileUtil::LocatorToUnicodeString(QuickTime_loc, path);

	//
	HDC	hdc;
	HWND hwnd;
	if (i_pWindow != 0)
	{
		hwnd = (HWND)i_pWindow->GetHandle();
		hdc=GetDC(hwnd);
	}

	int bitdepth;
	i_pWindow->GetBitDepth(bitdepth);
	HDC backHDC=CreateCompatibleDC(hdc);
	l_backHDC = backHDC;
	BITMAPINFO	bmpInfo;
	ZeroMemory(&bmpInfo,sizeof(bmpInfo));
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biBitCount=bitdepth;
	bmpInfo.bmiHeader.biCompression=BI_RGB;
	bmpInfo.bmiHeader.biSizeImage=l_Data.m_nWidth.GetValue()*l_Data.m_nHeight.GetValue()*(bitdepth/8);
	bmpInfo.bmiHeader.biPlanes=1;
	bmpInfo.bmiHeader.biHeight=-l_Data.m_nHeight.GetValue();
	bmpInfo.bmiHeader.biWidth=l_Data.m_nWidth.GetValue();
	void *pBits=NULL;
	HBITMAP hbm=CreateDIBSection(backHDC,&bmpInfo,DIB_RGB_COLORS,&pBits,NULL,NULL);
	SelectObject( backHDC, hbm );
	l_backBitmap = hbm;

	DBG_LOG("Creating Quicktime " << l_Data.m_nWidth.GetValue() << "x" << l_Data.m_nHeight.GetValue() << " - " << path.GetString());

	//	Initialize Quicktime GraphicsWorld
	HRESULT res = mnmQuickTimeUtil::InitGraphicsWorld( backHDC, hbm, path.GetString(), i_pWindow );
	if (res == E_FAIL)
	{
		// do error!
	}

	//	Clean-up
	ReleaseDC(hwnd, hdc);
	::DeleteDC(hdc);
	
	l_MovieSize = 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void close_QuickTime_file()
{
	mnmQuickTimeUtil::Close();
}

} // end of anonymous namespace

#endif // USE_QUICKTIME

//------------------------------------------------------------------------
//	BeginCapture/EndCapture() - notify start and stop capturing so that
//		QuickTime files can be begun and ended
//------------------------------------------------------------------------
void cptrWriteQuickTime::BeginCapture(g2dWindow* i_pWindow, captRenderOutputData& i_Data)
{
#ifdef USE_QUICKTIME
	l_Data = i_Data;

	//debug_OutputDataValues();

	l_MovieFileCounter = 0;
	l_bFirstFrame = true;

	fsLocator dir = l_Data.m_OutputDirectory.GetValue();

	mnmQuickTimeUtil::SetFramesPerSecond( tmlnTimeLine::GetFPS() );
	mnmQuickTimeUtil::SetCodec( i_Data.m_CompressCode.GetValue() );

	
	//
	create_QuickTime_file( i_pWindow, dir, i_Data.m_bRenderPosUse.GetValue() );
#endif // USE_QUICKTIME
}

void cptrWriteQuickTime::EndCapture()
{
#ifdef USE_QUICKTIME
	close_QuickTime_file();
#endif // USE_QUICKTIME
}

//------------------------------------------------------------------------
//	Write writes screen in 24-bit BMP format to given filename.
//  This should be used for screen captures for PR shots.
//
//	the sample rate is how many input pixels per output pixel
//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
//------------------------------------------------------------------------
void cptrWriteQuickTime::Write( g2dWindow* i_pWindow, const fsLocator& i_Locator, float i_fCurrentTime, const int i_Sampling )
{
#ifdef USE_QUICKTIME
	//	test if the compression should be set.
	//
	bool bSetCompression;
	bSetCompression = (l_bFirstFrame && l_Data.m_CompressCode.GetValue().length() >= 4);

	l_MovieSize += l_FrameSize;
	//DBG_LOG3("building QuickTime %6.3fMB (max %6.3f) framesize(%6.3f)", l_MovieSize, c_MovieMaxSize, l_FrameSize);

	if ( l_MovieSize >= c_MovieMaxSize )
	{
		close_QuickTime_file();
	
		l_MovieFileCounter++;
		create_QuickTime_file(i_pWindow, i_Locator, false);
	
		bSetCompression = (l_Data.m_CompressCode.GetValue().length() >= 4);
	}

	// Set up compression on first frame
	//
	if ( bSetCompression )
	{
		l_bFirstFrame = false;

		//DBG_LOG("QuickTime Compression Code: " << l_Data.m_CompressCode.GetValue().c_str());

//		QuickTimeCOMPRESSOPTIONS opts; ZeroMemory(&opts,sizeof(opts));
//		opts.fccHandler=mmioFOURCC( l_Data.m_CompressCode.GetValue()[0], l_Data.m_CompressCode.GetValue()[1], l_Data.m_CompressCode.GetValue()[2], l_Data.m_CompressCode.GetValue()[3] );
//		bool dialog = (l_Data.m_CompressCode.GetValue().length() > 4); // say "dialog" to get dialog
//		mnmQuickTimeUtil::SetQuickTimeVideoCompression(l_hQuickTime, hbm, &opts, dialog, (HWND) appApplication::GetMainWindowHandle());
	}

	// Add Frame to QuickTime
	//DBG_LOG("Adding frame to QuickTime");
	mnmQuickTimeUtil::AppendNewFrame(i_fCurrentTime, NULL);
#endif // USE_QUICKTIME
}


//------------------------------------------------------------------------
//	Write writes screen in 24-bit BMP format to given filename.
//  This should be used for screen captures for PR shots.
//
//	the sample rate is how many input pixels per output pixel
//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
//------------------------------------------------------------------------
void cptrWriteQuickTime::Write( g2dImage* i_pSurface, const fsLocator& i_Locator, float i_fCurrentTime, const int i_Sampling )
{
#ifdef USE_QUICKTIME
	// Set up compression on first frame
	//
	if (l_bFirstFrame && l_Data.m_CompressCode.GetValue().length() >= 4)
	{
		l_bFirstFrame = false;

		//DBG_LOG("QuickTime Compression Code: " << l_Data.m_CompressCode.c_str());

//		QuickTimeCOMPRESSOPTIONS opts; ZeroMemory(&opts,sizeof(opts));
//		opts.fccHandler=mmioFOURCC( l_Data.m_CompressCode.GetValue()[0], l_Data.m_CompressCode.GetValue()[1], l_Data.m_CompressCode.GetValue()[2], l_Data.m_CompressCode.GetValue()[3] );
//		bool dialog = (l_Data.m_CompressCode.GetValue().length() > 4); // say "dialog" to get dialog
//		mnmQuickTimeUtil::SetQuickTimeVideoCompression(l_hQuickTime, hbm, &opts, dialog, (HWND) appApplication::GetMainWindowHandle());
	}
	else
	{
	}

	// Add Frame to QuickTime
	mnmQuickTimeUtil::AppendNewFrame(i_fCurrentTime, i_pSurface);
#endif // USE_QUICKTIME
}


//------------------------------------------------------------------------
//	WriteAudio() - write the audio file to the QuickTime.  This must get
//	called after the frames are written right before EndCapture().
//------------------------------------------------------------------------
void cptrWriteQuickTime::WriteAudio(const fsLocator& i_AudioFile)
{
#ifdef USE_QUICKTIME
	//	don't remove this until writing audio is fixed
	//
//	return;

	std::string snd;
	fsFileUtil::LocatorToANSIFilename(i_AudioFile, snd);

	HRESULT	op_result;
	op_result = mnmQuickTimeUtil::WriteWavAudio(snd.c_str());
	if ( op_result != S_OK )
	{
		DBG_ASSERT(op_result == S_OK, "wav write in QuickTime failed");
	}
#endif // USE_QUICKTIME
}

//------------------------------------------------------------------------
//	WriteTimeCode() - write the timecode into the movie.
//------------------------------------------------------------------------
void cptrWriteQuickTime::WriteTimeCode( const float i_fStartTime, const float i_fEndTime )
{
#ifdef USE_QUICKTIME
	mnmQuickTimeUtil::WriteTimeCode( i_fStartTime, i_fEndTime );
#endif // USE_QUICKTIME
}
