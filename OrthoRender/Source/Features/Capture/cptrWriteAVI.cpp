/****************************************************************************\
**  cptrWriteAVI.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrWriteAVI.hpp"

#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Capture/cptrWriteUtil.hpp"

#undef STRICT
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
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/g2d/g2dImage.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
#include "Graphics/g2d/g2dWindow.hpp"


//
// TODO: [rjk] clean up the code by removing duplicate BMP code from here.
//

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

	std::string l_OutputFilename;

	mnmAviUtil::HAVI l_hAvi;				// AVI file handle

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void create_avi_file(const fsLocator& i_OutputDirectory)
{
	//	after a filename is generated, if the output size is over the maximum
	//	limit for AVIs, then break apart the movie and append a number to
	//	the filename.
	//
	char fnm[128];
	cptrRenderOutputDataUtil::GenerateFilename( l_OutputFilename );
	if ( l_MovieFileCounter == 0 )
	{
		sprintf(fnm, "%s.avi", l_OutputFilename.c_str());
	}
	else
	{
		sprintf(fnm, "%s_part%d.avi", l_OutputFilename.c_str(), (l_MovieFileCounter+1));
	}

	// Make sure directory exists
	//
	fsFileUtil::CreateDirectory(i_OutputDirectory);

	// set this value so other functions know what the last avi written was
	//
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	data.m_OutputFileName.SetValue( itString(fnm) );
	data.m_OutputMovieDirectory.SetValue( i_OutputDirectory );

	//	create the AVI
	//
	fsLocator avi_loc = i_OutputDirectory;
	avi_loc.Push(itString(fnm));
	std::string path;
	fsFileUtil::LocatorToANSIFilename(avi_loc, path);

	l_hAvi = mnmAviUtil::CreateAvi(path.c_str(), (int)(1000.0f / l_Data.m_fCaptureFPS.GetValue()), NULL);
	
	DBG_LOG1( "saving AVI %s", path.c_str() );
	DBG_ASSERT1( l_hAvi != NULL, "could not create AVI %s", path.c_str() );

	l_MovieSize = 0;
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
void cptrWriteAVI::BeginCapture(cptrRenderOutputData& i_Data)
{
	l_Data = i_Data;

	//debug_OutputDataValues();

	l_MovieFileCounter = 0;
	l_bFirstFrame = true;

	fsLocator dir = l_Data.m_OutputDirectory.GetValue();
	create_avi_file( dir );
}

void cptrWriteAVI::EndCapture()
{
	close_avi_file();
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

		DBG_LOG1("AVI Compression Code: %s", l_Data.m_CompressCode.GetValue().c_str());

		AVICOMPRESSOPTIONS opts; ZeroMemory(&opts,sizeof(opts));
		opts.fccHandler=mmioFOURCC( l_Data.m_CompressCode.GetValue()[0], l_Data.m_CompressCode.GetValue()[1], l_Data.m_CompressCode.GetValue()[2], l_Data.m_CompressCode.GetValue()[3] );
		bool dialog = (l_Data.m_CompressCode.GetValue().length() > 4); // say "dialog" to get dialog
		mnmAviUtil::SetAviVideoCompression(l_hAvi, hbm, &opts, dialog, (HWND) appApplication::GetMainWindowHandle());
	}

	// Add Frame to AVI
	//DBG_LOG0("Adding frame to AVI");
	int ret_val = mnmAviUtil::AddAviFrame(l_hAvi,hbm);
	DBG_ASSERT1(ret_val == 0, "Error Adding AVI frame, code %d", ret_val);
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
	HBITMAP hbm = i_pSurface->GetHBMP( l_Data.m_nCaptureSampling.GetValue() );

	// Set up compression on first frame
	//
	if (l_bFirstFrame && l_Data.m_CompressCode.GetValue().length() >= 4)
	{
		l_bFirstFrame = false;

		//DBG_LOG1("AVI Compression Code: %s", l_Data.m_CompressCode.c_str());

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
	std::string snd;
	fsFileUtil::LocatorToANSIFilename(i_AudioFile, snd);

	HRESULT	op_result;
	op_result = mnmAviUtil::AddAviWav(l_hAvi, snd.c_str(), SND_FILENAME);
	if ( op_result != S_OK )
	{
		DBG_ASSERT0(op_result == S_OK, "wav write in AVI failed");
	}
}
