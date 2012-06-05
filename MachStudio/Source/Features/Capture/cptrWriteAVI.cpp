/****************************************************************************\
**  cptrWriteAVI.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrWriteAVI.hpp"

#include "Features/Capture/cptrWriteUtil.hpp"

#undef STRICT
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mnm/mnmAviUtil.hpp"
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
	bool l_bFusionApp = false;

	itString l_OutputFilename;

	mnmAviUtil::HAVI l_hAvi;				// AVI file handle


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void create_avi_file(const fsLocator& i_OutputDirectory)
{
	//	after a filename is generated, if the output size is over the maximum
	//	limit for AVIs, then break apart the movie and append a number to
	//	the filename.
	//
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	if(!l_bFusionApp)
	{
		captRenderOutputDataUtil::GenerateFilename( l_OutputFilename );
	
		if ( l_MovieFileCounter == 0 )
		{
			l_OutputFilename += itString(L".avi");
		}
		else
		{
			l_OutputFilename += itString(L"_part");
			std::ostringstream o;
			if (o << (l_MovieFileCounter+1))
				l_OutputFilename += itString(o.str().c_str());
			l_OutputFilename += itString(L".avi");
		}

		// Make sure directory exists
		//
		fsFileUtil::CreateDirectory(i_OutputDirectory);

		// set this value so other functions know what the last avi written was
		//
		data.m_OutputFileName.SetValue( l_OutputFilename );
	}
	else
		l_OutputFilename = data.m_OutputFileName.GetValue() ;

	data.m_OutputMovieDirectory.SetValue( i_OutputDirectory );

	//	create the AVI
	//
	fsLocator avi_loc = i_OutputDirectory;
	avi_loc.Push(l_OutputFilename);

	//	if the file exists, write it in the log
	//
	if (fsFileUtil::FileExists( avi_loc ))
	{
		DBG_WARNING("Overwritting file during render: " << avi_loc);
	}

	itString path;
	fsFileUtil::LocatorToUnicodeString(avi_loc, path);
	l_hAvi = mnmAviUtil::CreateAvi(path.GetString(), (int)(1000.0f / tmlnTimeLine::GetFPS()), NULL);
	
	DBG_LOG( "saving AVI " << avi_loc );
	DBG_ASSERT( l_hAvi != NULL, "could not create AVI " << avi_loc );

	l_MovieSize = 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void append_avi_file(const itString i_origmovie, const itString i_countdown, const itString i_tempPath)
{
	mnmAviUtil::HAVI l_hAvi2;
	mnmAviUtil::HAVI l_hAvi1;// AVI file handle
	
	l_hAvi1 = mnmAviUtil::OpenAVI(i_countdown.GetString());
	l_hAvi2 = mnmAviUtil::OpenAVI(i_origmovie.GetString());
	
	int ret_val = mnmAviUtil::AppendAvi(l_hAvi1,l_hAvi2, i_tempPath);
	DBG_ASSERT(ret_val == 0, "Error Appending AVI frame, code " << ret_val);

	int ret_val1 = mnmAviUtil::CloseAvi(l_hAvi1);
	DBG_ASSERT(ret_val1 == 0, "Error closing AVI1 , code " << ret_val1);
	int ret_val2 = mnmAviUtil::CloseAvi(l_hAvi2);
	DBG_ASSERT(ret_val2 == 0, "Error closing AVI2 , code " << ret_val2);
	
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void close_avi_file()
{
	mnmAviUtil::CloseAvi(l_hAvi);
}

} // end of anonymous namespace


//------------------------------------------------------------------------
//	BeginCapture/EndCapture() - notify start and stop capturing so that
//		AVI files can be begun and ended
//------------------------------------------------------------------------
void cptrWriteAVI::BeginCapture(captRenderOutputData& i_Data)
{
	l_Data = i_Data;

	//debug_OutputDataValues();

	l_MovieFileCounter = 0;
	l_bFirstFrame = true;

	fsLocator dir = l_Data.m_OutputDirectory.GetValue();
	create_avi_file( dir );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrWriteAVI::EndCapture()
{
	close_avi_file();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrWriteAVI::AppendAVI(const itString i_origmovie, const itString i_countdown, const itString i_tempPath)
{
	append_avi_file(i_origmovie,i_countdown,i_tempPath);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrWriteAVI::set_to_fusion(bool i_isFusionApp)
{
	l_bFusionApp = i_isFusionApp;
}

//------------------------------------------------------------------------
//	Write writes screen in 24-bit BMP format to given filename.
//  This should be used for screen captures for PR shots.
//
//	the sample rate is how many input pixels per output pixel
//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
//------------------------------------------------------------------------
void cptrWriteAVI::Write( g2dWindow* i_pWindow, const fsLocator& i_Locator, const int i_Sampling )
{
	// Add to AVI (Loads from disk for now)
	//char fns[128];
	//sprintf(fns, "Saves\\%s", l_OutputFilename);
	//HBITMAP hbm=(HBITMAP)LoadImage(NULL,fname.c_str(),IMAGE_BITMAP,
	//								0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);

	g2dImage* pImg = NULL;
	g2dScreenCaptureUtil::CaptureWindowToImage(*i_pWindow, pImg);
	HBITMAP hbm = pImg->GetHBMP(l_Data.m_nCaptureSampling.GetValue());
	delete pImg;

	//float save_time = appTime::GetTime();

	//	test if the compression should be set.
	//
	bool bSetCompression;
	bSetCompression = (l_bFirstFrame && l_Data.m_CompressCode.GetValue().length() >= 4);

	l_MovieSize += l_FrameSize;
	//DBG_LOG3("building AVI %6.3fMB (max %6.3f) framesize(%6.3f)", l_MovieSize, c_MovieMaxSize, l_FrameSize);

	if ( l_MovieSize >= c_MovieMaxSize )
	{
		close_avi_file();

		l_MovieFileCounter++;
		create_avi_file(i_Locator);

		bSetCompression = (l_Data.m_CompressCode.GetValue().length() >= 4);
	}

	// Set up compression on first frame
	//
	if ( bSetCompression )
	{
		l_bFirstFrame = false;

		//DBG_LOG("AVI Compression Code: " << l_Data.m_CompressCode.GetValue().c_str());

		AVICOMPRESSOPTIONS opts; ZeroMemory(&opts,sizeof(opts));
		opts.fccHandler=mmioFOURCC( l_Data.m_CompressCode.GetValue()[0], l_Data.m_CompressCode.GetValue()[1], l_Data.m_CompressCode.GetValue()[2], l_Data.m_CompressCode.GetValue()[3] );
		bool dialog = (l_Data.m_CompressCode.GetValue().length() > 4); // say "dialog" to get dialog
		mnmAviUtil::SetAviVideoCompression(l_hAvi, hbm, &opts, dialog, (HWND) appApplication::GetMainWindowHandle());
	}

	// Add Frame to AVI
	//DBG_LOG("Adding frame to AVI");
	int ret_val = mnmAviUtil::AddAviFrame(l_hAvi,hbm);
	DBG_ASSERT(ret_val == 0, "Error Adding AVI frame, code " << ret_val);
	::DeleteObject(hbm);

	//DBG_WARNING1("Save capture time: %f", save_time - begin_time);
}


//------------------------------------------------------------------------
//	Write writes screen in 24-bit BMP format to given filename.
//  This should be used for screen captures for PR shots.
//
//	the sample rate is how many input pixels per output pixel
//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
//------------------------------------------------------------------------
void cptrWriteAVI::Write( g2dImage* i_pSurface, const fsLocator& i_Locator, const int i_Sampling )
{
	HBITMAP hbm = i_pSurface->GetHBMP( i_Sampling );
	//int s =	i_pSurface->GetStride();
	//int w = i_pSurface->GetWidth();
	//int h = i_pSurface->GetHeight();

	// Set up compression on first frame
	//
	if (l_bFirstFrame && l_Data.m_CompressCode.GetValue().length() >= 4)
	{
		l_bFirstFrame = false;

		//DBG_LOG("AVI Compression Code: " << l_Data.m_CompressCode.c_str());

		AVICOMPRESSOPTIONS opts; ZeroMemory(&opts,sizeof(opts));
		opts.fccHandler=mmioFOURCC( l_Data.m_CompressCode.GetValue()[0], l_Data.m_CompressCode.GetValue()[1], l_Data.m_CompressCode.GetValue()[2], l_Data.m_CompressCode.GetValue()[3] );
		bool dialog = (l_Data.m_CompressCode.GetValue().length() > 4); // say "dialog" to get dialog
		mnmAviUtil::SetAviVideoCompression(l_hAvi, hbm, &opts, dialog, (HWND) appApplication::GetMainWindowHandle());
	}

	// Add Frame to AVI
	mnmAviUtil::AddAviFrame(l_hAvi,hbm);
	::DeleteObject(hbm);
}


//------------------------------------------------------------------------
//	WriteAudio() - write the audio file to the AVI.  This must get
//	called after the frames are written right before EndCapture().
//------------------------------------------------------------------------
void cptrWriteAVI::WriteAudio(const fsLocator& i_AudioFile)
{
	itString snd;
	fsFileUtil::LocatorToUnicodeString(i_AudioFile, snd);

	HRESULT	op_result;
	op_result = mnmAviUtil::AddAviWav(l_hAvi, snd.GetString(), SND_FILENAME);
	if ( op_result != S_OK )
	{
		DBG_ASSERT(op_result == S_OK, "wav write in AVI failed");
	}
}
