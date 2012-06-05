/****************************************************************************\
**  cptrWriteQuickTime.cpp
**
**      see .hpp
**
**		quicktime constants reference
**		http://developer.apple.com/documentation/QuickTime/Reference/QTRef_Constants/Reference/reference.html#//apple_ref/doc/uid/TP40003455-GlobalQuickTimeAPIConstants-CodecIdentifiers
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrWriteQuickTime.hpp"

#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Capture/cptrWriteUtil.hpp"

#undef STRICT
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
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/g2d/g2dImage.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
//#include "Graphics/g2d/g2dWindow.hpp"
#include "GraphicsDX9/g2d/g2dWindowDX9.hpp"


//
// TODO: [rjk] clean up the code by removing duplicate BMP code from here.
//


//============================================================================
//============================================================================
namespace
{
	//
	//	variables
	//
	cptrRenderOutputData l_Data;

	int l_MovieFileCounter	= 0;
	long double l_MovieSize	= 0;
	const long double c_MovieMaxSize = (2.0L * 1073.741824L) - 0.2f; // size in megabytes (- header size)
	double l_FrameSize = 0.0f;	// in megabytes
	bool	l_bFirstFrame = false;
	int l_QuicktimeCount = 0;
	std::string l_OutputFilename;

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
	char fnm[128];
	char fnmTemp[128];
	cptrRenderOutputDataUtil::GenerateFilename( l_OutputFilename );

	
	if ( l_MovieFileCounter == 0 )
	{
		sprintf(fnm, "%s.mov", l_OutputFilename.c_str());
		sprintf(fnmTemp, "%s", l_OutputFilename.c_str());
	}
	else
	{
		sprintf(fnm, "%s_part%d.mov", l_OutputFilename.c_str(), (l_MovieFileCounter+1));
		sprintf(fnmTemp, "%s_part%d", l_OutputFilename.c_str(), (l_MovieFileCounter+1));
	}
	// Make sure directory exists
	//
	fsFileUtil::CreateDirectory(i_OutputDirectory);
	
	
	//make sure the file doesn't already exist
	//if it does exist increment the section count
	fsLocator Temp_loc = i_OutputDirectory;
	int fileCounter = 0;
	Temp_loc.Push(itString(fnm));

	if(i_isResuming)
	{
		while( fsFileUtil::FileExists(Temp_loc) ) 
		{	
			//remove the previous file from the directory list because it already exists
			Temp_loc.Pop();

			//append a new section count to check if this file exists
			sprintf(fnm, "%s_Section%d.mov", fnmTemp, (fileCounter+1));
			Temp_loc.Push(itString(fnm));

			fileCounter++;		
		}
	}

	DBG_WARNING1("The new filename is %s", fnm);


	// set this value so other functions know what the last QuickTime written was
	//
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	data.m_OutputFileName.SetValue( itString(fnm) );
	data.m_OutputMovieDirectory.SetValue( i_OutputDirectory );

	//	create the QuickTime
	//


	fsLocator QuickTime_loc = i_OutputDirectory;
	QuickTime_loc.Push(itString(fnm));
	std::string path;
	fsFileUtil::LocatorToANSIFilename(QuickTime_loc, path);

	//
	HDC	hdc;
	HWND hwnd;
	g2dWindowDX9* pWinD3D = dynamic_cast<g2dWindowDX9 *>(i_pWindow);
	if (pWinD3D != 0)
	{
		hwnd = (HWND)pWinD3D->GetHandle();
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

	DBG_WARNING3("Creating Quicktime %dx%d - %s", l_Data.m_nWidth.GetValue(), l_Data.m_nHeight.GetValue(), path.c_str());

	//	Initialize Quicktime GraphicsWorld
	HRESULT res = mnmQuickTimeUtil::InitGraphicsWorld( backHDC, hbm, path.c_str(), i_pWindow );
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


//------------------------------------------------------------------------
//	BeginCapture/EndCapture() - notify start and stop capturing so that
//		QuickTime files can be begun and ended
//------------------------------------------------------------------------
void cptrWriteQuickTime::BeginCapture(g2dWindow* i_pWindow, cptrRenderOutputData& i_Data)
{
	l_Data = i_Data;

	//debug_OutputDataValues();

	l_MovieFileCounter = 0;
	l_bFirstFrame = true;

	fsLocator dir = l_Data.m_OutputDirectory.GetValue();

	mnmQuickTimeUtil::SetFramesPerSecond( i_Data.m_fCaptureFPS.GetValue() );
	mnmQuickTimeUtil::SetCodec( i_Data.m_CompressCode.GetValue() );

	
	//
	create_QuickTime_file( i_pWindow, dir, i_Data.m_bRenderPosUse.GetValue() );
}

void cptrWriteQuickTime::EndCapture()
{
	close_QuickTime_file();
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

		DBG_LOG1("QuickTime Compression Code: %s", l_Data.m_CompressCode.GetValue().c_str());

//		QuickTimeCOMPRESSOPTIONS opts; ZeroMemory(&opts,sizeof(opts));
//		opts.fccHandler=mmioFOURCC( l_Data.m_CompressCode.GetValue()[0], l_Data.m_CompressCode.GetValue()[1], l_Data.m_CompressCode.GetValue()[2], l_Data.m_CompressCode.GetValue()[3] );
//		bool dialog = (l_Data.m_CompressCode.GetValue().length() > 4); // say "dialog" to get dialog
//		mnmQuickTimeUtil::SetQuickTimeVideoCompression(l_hQuickTime, hbm, &opts, dialog, (HWND) appApplication::GetMainWindowHandle());
	}

	// Add Frame to QuickTime
	//DBG_LOG0("Adding frame to QuickTime");
	mnmQuickTimeUtil::AppendNewFrame(i_fCurrentTime, NULL);
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
	// Set up compression on first frame
	//
	if (l_bFirstFrame && l_Data.m_CompressCode.GetValue().length() >= 4)
	{
		l_bFirstFrame = false;

		//DBG_LOG1("QuickTime Compression Code: %s", l_Data.m_CompressCode.c_str());

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
}


//------------------------------------------------------------------------
//	WriteAudio() - write the audio file to the QuickTime.  This must get
//	called after the frames are written right before EndCapture().
//------------------------------------------------------------------------
void cptrWriteQuickTime::WriteAudio(const fsLocator& i_AudioFile)
{
	//	don't remove this until writing audio is fixed
	//
//	return;

	std::string snd;
	fsFileUtil::LocatorToANSIFilename(i_AudioFile, snd);

	HRESULT	op_result;
	op_result = mnmQuickTimeUtil::WriteWavAudio(snd.c_str());
	if ( op_result != S_OK )
	{
		DBG_ASSERT0(op_result == S_OK, "wav write in QuickTime failed");
	}
}

//------------------------------------------------------------------------
//	WriteTimeCode() - write the timecode into the movie.
//------------------------------------------------------------------------
void cptrWriteQuickTime::WriteTimeCode( const float i_fStartTime, const float i_fEndTime )
{
	mnmQuickTimeUtil::WriteTimeCode( i_fStartTime, i_fEndTime );
}
