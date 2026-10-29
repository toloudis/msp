/****************************************************************************\
**  cptrWriteAVI.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrWriteAVI.hpp"

#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Core/app/appApplication.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/fs/fsAviFile.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/g2d/g2dImage.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
#include "Graphics/g2d/g2dWindow.hpp"


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

	std::string l_OutputFilename;

	fsAviFile::Handle l_hAvi;				// AVI file handle


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
			l_OutputFilename += std::string(".avi");
		}
		else
		{
			l_OutputFilename += std::string("_part");
			std::ostringstream o;
			if (o << (l_MovieFileCounter+1))
				l_OutputFilename += o.str();
			l_OutputFilename += std::string(".avi");
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

	l_hAvi = fsAviFile::Create(avi_loc, (int)(1000.0f / tmlnTimeLine::GetFPS()));
	
	DBG_LOG( "saving AVI " << avi_loc );
	DBG_ASSERT( l_hAvi != NULL, "could not create AVI " << avi_loc );

	l_MovieSize = 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void append_avi_file(const std::string i_origmovie, const std::string i_countdown, const std::string i_tempPath)
{
	fsLocator countdown_loc, origmovie_loc, temp_loc;
	fsFileUtil::UnicodeStringToLocator(i_countdown, countdown_loc);
	fsFileUtil::UnicodeStringToLocator(i_origmovie, origmovie_loc);
	fsFileUtil::UnicodeStringToLocator(i_tempPath, temp_loc);

	fsAviFile::Handle l_hAvi1 = fsAviFile::Open(countdown_loc);
	fsAviFile::Handle l_hAvi2 = fsAviFile::Open(origmovie_loc);
	
	bool ok = fsAviFile::Append(l_hAvi1,l_hAvi2, temp_loc);
	DBG_ASSERT(ok, "Error Appending AVI frame: " << fsAviFile::GetLastErrorMessage());

	bool ok1 = fsAviFile::Close(l_hAvi1);
	DBG_ASSERT(ok1, "Error closing AVI1: " << fsAviFile::GetLastErrorMessage());
	bool ok2 = fsAviFile::Close(l_hAvi2);
	DBG_ASSERT(ok2, "Error closing AVI2: " << fsAviFile::GetLastErrorMessage());
	
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void close_avi_file()
{
	fsAviFile::Close(l_hAvi);
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
void cptrWriteAVI::AppendAVI(const std::string i_origmovie, const std::string i_countdown, const std::string i_tempPath)
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
	int width = 0, height = 0, bpp = 0, size = 0;
	uint8_t* pixels = NULL;
	pImg->GetPixels(&pixels, &size, &width, &height, &bpp, l_Data.m_nCaptureSampling.GetValue());
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

		bool dialog = (l_Data.m_CompressCode.GetValue().length() > 4); // say "dialog" to get dialog
		fsAviFile::SetVideoCompression(l_hAvi, l_Data.m_CompressCode.GetValue().c_str(), dialog,
			appApplication::GetMainWindowHandle(), width, height, bpp, size);
	}

	// Add Frame to AVI
	//DBG_LOG("Adding frame to AVI");
	bool ok = fsAviFile::AddFrame(l_hAvi, pixels, size, width, height, bpp);
	DBG_ASSERT(ok, "Error Adding AVI frame: " << fsAviFile::GetLastErrorMessage());
	delete [] pixels;

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
	int width = 0, height = 0, bpp = 0, size = 0;
	uint8_t* pixels = NULL;
	i_pSurface->GetPixels(&pixels, &size, &width, &height, &bpp, i_Sampling);
	//int s =	i_pSurface->GetStride();
	//int w = i_pSurface->GetWidth();
	//int h = i_pSurface->GetHeight();

	// Set up compression on first frame
	//
	if (l_bFirstFrame && l_Data.m_CompressCode.GetValue().length() >= 4)
	{
		l_bFirstFrame = false;

		//DBG_LOG("AVI Compression Code: " << l_Data.m_CompressCode.c_str());

		bool dialog = (l_Data.m_CompressCode.GetValue().length() > 4); // say "dialog" to get dialog
		fsAviFile::SetVideoCompression(l_hAvi, l_Data.m_CompressCode.GetValue().c_str(), dialog,
			appApplication::GetMainWindowHandle(), width, height, bpp, size);
	}

	// Add Frame to AVI
	fsAviFile::AddFrame(l_hAvi, pixels, size, width, height, bpp);
	delete [] pixels;
}


//------------------------------------------------------------------------
//	WriteAudio() - write the audio file to the AVI.  This must get
//	called after the frames are written right before EndCapture().
//------------------------------------------------------------------------
void cptrWriteAVI::WriteAudio(const fsLocator& i_AudioFile)
{
	bool ok = fsAviFile::AddWav(l_hAvi, i_AudioFile);
	DBG_ASSERT(ok, "wav write in AVI failed: " << fsAviFile::GetLastErrorMessage());
}
