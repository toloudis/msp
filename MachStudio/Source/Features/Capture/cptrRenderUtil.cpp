/****************************************************************************\
**	cptrRenderUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderUtil.hpp"

#include "MainApp/mnmApp.hpp"

#include "Features/Capture/cptrModeRenderBake.hpp"
#include "Features/Capture/cptrWriteAVI.hpp"
#include "Features/Capture/cptrWriteQuickTime.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Features/RenderPrefs/rndrPrefsUtil.hpp"

#include "Support/capt/captRenderOutputData.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/capt/captRenderOutputTagsUtil.hpp"
#include "Support/capt/captStereoUtil.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/pfx/pfxPostEffectMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Graphics/g2d/g2dImageCreate.hpp"
#include "Graphics/g2d/g2dImageSave.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"

#include "Tool/gui/guiMenuMgr.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Ma/maSampling.hpp"

//============================================================================
//============================================================================
namespace
{
	//
	//	variables
	//
	bool l_bScreenCapEnabled	= false;
	bool l_bRenderInProgress	= false;
	maTime l_fSimTime;
	maTime l_fSimTimeInc;
	maTime l_fSimTimeMax;
	bool m_bIsPhotoshop = false;
	bool l_bCaptureToneMapped = false;

	itString	l_OutputFilename;
	fsLocator	l_OutputFilenameFull;

	g2dWindow*			l_pWindow = NULL;		// the capture window
	g3dSceneRenderer*	l_pRenderer = NULL;		// the capture renderer
	g2dWindow*			l_pAppWindow = NULL;	// the application window
	g2dImage*			l_pQuadImage = NULL;	// large resolution filled in by quadrants

	std::vector<maVector3d> l_SamplePoints;

	//------------------------------------------------------------------------
	// compute how may divisions on a side we need to make to render the large frame
	// in manageable chunks. If quadrant_div is 2, then 2*2=4 quadrants 
	// will be rendered.
	//------------------------------------------------------------------------
	int get_quadrant_div()
	{
		//return 2;

		captRenderOutputData& data = captRenderOutputDataUtil::Data();
		int width	= data.m_nWidth.GetValue() * (data.m_bJitteredSampling.GetValue() ? 1 : data.m_nCaptureSampling.GetValue());
		
		// Determine a power of 2 that will keep the quadrants under 
		// the maximum quadrant size
		const int c_MaximumQuadrantSize = 1024;
		int quadrants = width / c_MaximumQuadrantSize;
		if (quadrants <= 1)
			return 1;
		else if (quadrants <= 2)
			return 2;
		else if (quadrants <= 4)
			return 4;
		else if (quadrants <= 8)
			return 8;
		else if (quadrants <= 16)
			return 16;
		else if (quadrants <= 32)
			return 32;
		else
			return 64;
	}

	//------------------------------------------------------------------------
	//	when a image/movie is completed being written to, do these things.
	//------------------------------------------------------------------------
	void capture_file_finished()
	{
		captRenderOutputData& data = captRenderOutputDataUtil::Data();
		
		//	store the filename
		int index = data.m_OutputFiles.GetValue().size();

		std::string path;
		fsFileUtil::LocatorToANSIFilename( l_OutputFilenameFull, path );
		data.m_OutputFiles.SetValueText(index, path);
		data.m_OutputFiles.SetValueFlag(index, true);
	}

	maFilter* CreateFilter(captRenderOutputData::eFilterFunc i_FilterType)
	{
		maFilter* filter = NULL;
		switch (i_FilterType)
		{
		case captRenderOutputData::eFilterBox:
			filter = new maBoxFilter;
			break;
		case captRenderOutputData::eFilterGaussian:
			filter = new maGaussianFilter;
			break;
		case captRenderOutputData::eFilterMitchell:
			filter = new maMitchellFilter;
			break;
		case captRenderOutputData::eFilterTriangle:
			filter = new maTriangleFilter;
			break;
		case captRenderOutputData::eFilterSinc:
			filter = new maSincFilter;
			break;
		case captRenderOutputData::eFilterLanczos:
			filter = new maLanczosFilter;
			break;
		case captRenderOutputData::eFilterBlackmanHarris:
			filter = new maBlackmanHarrisFilter;
			break;
		case captRenderOutputData::eFilterCatmullRom:
			filter = new maCatmullRomFilter;
			break;
		default:
			//DBG_WARNING("unknown filter type.");
			break;
		}
		return filter;
	}

//
//
// TODO: [rjk] make this capture even more flexible by having formats register themselves
//	instead of the hardcoded "BMP","AVI",...
// TODO: [rjk] clean up the code
//

} // end of anonymous namespace




//============================================================================
//
//	cptrRenderUtil
//
//============================================================================


//----------------------------------------------------------------------------
//	SetCapture() - turn on/off capturing
//----------------------------------------------------------------------------
void cptrRenderUtil::SetCapture( bool i_bEnable )
{
	l_bScreenCapEnabled = i_bEnable;
}

//----------------------------------------------------------------------------
//	GetCapture() - if capturign on/off
//----------------------------------------------------------------------------
bool cptrRenderUtil::GetCapture()
{
	return l_bScreenCapEnabled;
}

//------------------------------------------------------------------------
//	SetCaptureFinished() - make capturing finish early
//------------------------------------------------------------------------
void cptrRenderUtil::SetCaptureFinished()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	l_fSimTime = data.m_fEndTime.GetValue();
}

//------------------------------------------------------------------------
//	GetCaptureFinished() - if captured
//		enough frames, return true
//------------------------------------------------------------------------
bool cptrRenderUtil::GetCaptureFinished()
{
	if ( l_fSimTime >= l_fSimTimeMax)
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
//	set to the MAX time
//------------------------------------------------------------------------
void cptrRenderUtil::SetCaptureTimeMax( const maTime& i_fMaxTime )
{
	l_fSimTimeMax = i_fMaxTime;
}

//------------------------------------------------------------------------
//	SetCaptureWindow() - set the window to capture from
//------------------------------------------------------------------------
void cptrRenderUtil::SetCaptureWindow( g2dWindow* i_pWindow )
{
	DBG_ASSERT( i_pWindow != 0, "cannot capture from a null window" );

	l_pWindow = (i_pWindow);
}
void cptrRenderUtil::SetCaptureRenderer( g3dSceneRenderer* i_pRenderer )
{
//	DBG_ASSERT( i_pRenderer != 0, "cannot capture with a null renderer" );
	l_pRenderer = (i_pRenderer);
}

//------------------------------------------------------------------------
//	SetAppWindow() - store the main window for the application
//	so that it can be used as the capture window in default cases.
//------------------------------------------------------------------------
void cptrRenderUtil::SetAppWindow( g2dWindow* i_pWindow )
{
	l_pAppWindow = i_pWindow;
}
g2dWindow* cptrRenderUtil::GetAppWindow()
{
	return l_pAppWindow;
}

//------------------------------------------------------------------------
//	GetSimTimeIncrement() - get the simTime increment that is used by
//	UpdateSimTime().
//------------------------------------------------------------------------
float cptrRenderUtil::GetSimTimeIncrement()
{
	return l_fSimTimeInc.AsSeconds();
}

//------------------------------------------------------------------------
// Return the full file path of the render
//------------------------------------------------------------------------
fsLocator& cptrRenderUtil::GetFullOutputFile()
{
	return l_OutputFilenameFull;
}

//----------------------------------------------------------------------------
//	Initialize()
//----------------------------------------------------------------------------
void cptrRenderUtil::Initialize()
{
	captRenderOutputTagsUtil::Init();
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	ResetTime();

	l_pWindow = 0;
	l_pRenderer = 0;

	if (data.m_bJitteredSampling.GetValue())
	{
		maSampling::GetSamples2D_Repeatable(data.m_nCaptureSampling.GetValue(), data.m_nCaptureSampling.GetValue(),
			l_SamplePoints, 1, data.m_FilterWidth.GetValue(), data.m_FilterWidth.GetValue());
		
		maFilter* filter = CreateFilter((captRenderOutputData::eFilterFunc)data.m_FilterFunc.GetValue());

		if (filter != NULL)
		{
			maSampling::WeightSamples(l_SamplePoints, filter, data.m_FilterWidth.GetValue(), data.m_FilterWidth.GetValue());
			delete filter;
		}
	}
}

//------------------------------------------------------------------------
//	Reset the time to Initialize values
//------------------------------------------------------------------------
void cptrRenderUtil::ResetTime()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	//l_fSimTimeInc = 1.0f / (tmlnTimeLine::GetFPS() /* --motion blur disabled-- * data.m_nMotionSamplesPerFrame.GetValue() */);
	//l_fSimTime = data.m_fStartTime.GetValue() - l_fSimTimeInc; //0.0f;
	l_fSimTimeInc = tmlnTimeLine::GetFrameIncrement();
	l_fSimTime = data.m_fStartTime.GetValue() - l_fSimTimeInc; //0.0f;
}

//----------------------------------------------------------------------------
//	UpdateSimTime() - change the sim time
//----------------------------------------------------------------------------
maTime cptrRenderUtil::UpdateSimTime()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	appSimTime::SetTime( l_fSimTime.AsSeconds(), l_fSimTimeInc.AsSeconds() );

	l_fSimTime += l_fSimTimeInc;

	//TIME - rounding to frame is not needed if using maTime?
	//float fps = tmlnTimeLine::GetFPS() /* --motion blur disabled-- * data.m_nMotionSamplesPerFrame.GetValue() */;
	//l_fSimTime = floor(l_fSimTime * fps + 0.5f) / fps;	//round nearest, align to frames to prevent accumulation error
	
	return l_fSimTime;
}

//------------------------------------------------------------------------
//	SetSimTime() - set the sim time
//------------------------------------------------------------------------
void cptrRenderUtil::SetSimTime(const maTime& i_fSimTime)
{
	l_fSimTime = i_fSimTime;
	appSimTime::SetTime( l_fSimTime.AsSeconds(), l_fSimTimeInc.AsSeconds() );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool cptrRenderUtil::ReadyToCapture()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	return (l_fSimTime >= data.m_fStartTime.GetValue());
}

//----------------------------------------------------------------------------
//	SetCaptureToneMapped - set the flag for the capture tonemapped pixel render
//----------------------------------------------------------------------------
void cptrRenderUtil::SetCaptureToneMapped( bool i_bCaptureToneMapped )
{	
	l_bCaptureToneMapped = i_bCaptureToneMapped;
}

//----------------------------------------------------------------------------
//	GetIsCaptureRaw - need to know if capturing raw buffer or rgba color
//----------------------------------------------------------------------------
bool cptrRenderUtil::GetIsCaptureRaw()
{
	// special handling for some renders. we want the raw float depth buffer.
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	bool captureFloatFormat = ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"HDR",3)==0 )
		|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"PFM",3)==0 )
		|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"EXR",3)==0 );

	captureFloatFormat = captureFloatFormat && !l_bCaptureToneMapped;
	if (l_pRenderer == NULL)
		return captureFloatFormat;
	else
		return captureFloatFormat && (l_pRenderer->GetRawBufferPFD() != g2dPFD::e_Color);
}

//----------------------------------------------------------------------------
//	GetCapturePFD - determine what pfd will actually be captured.
//----------------------------------------------------------------------------
g2dPFD cptrRenderUtil::GetCapturePFD()
{
	if (GetIsCaptureRaw() && l_pRenderer)
	{
		g2dPFD::PixelFormat rawBufferPFD = l_pRenderer->GetRawBufferPFD();
		int numBytesPerPixel = captStereoUtil::GetNumBytesPerPixel( rawBufferPFD );
		// expand pfd to a 4-channel format:
		switch(rawBufferPFD)
		{
		case g2dPFD::e_Float16:
			return g2dPFD(g2dPFD::e_RGBA16f, 16*4);
			break;
		case g2dPFD::e_Float32:
			return g2dPFD(g2dPFD::e_RGBA32f, 32*4);
			break;
		case g2dPFD::e_GR32f:
			return g2dPFD(g2dPFD::e_RGBA32f, 32*4);
			break;
		default:
			return g2dPFD(rawBufferPFD, numBytesPerPixel*8);
		};
	}
	else
	{
		g2dPFD pfd(g2dPFD::e_Color, 32);
		pfd.Set(0,8, 8,8, 16,8, 24,8, 32);
		return pfd;
	}
}

//----------------------------------------------------------------------------
//	CaptureFrameToImage() - creates a g2dImage in system mem. CALLER MUST DELETE!
//----------------------------------------------------------------------------
void cptrRenderUtil::CaptureFrameToImage(g2dImage*& o_pImage)
{
	// convert to surface representation
	o_pImage = NULL;

	// wait for start time before capturing in batch mode
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	if (l_fSimTime < data.m_fStartTime.GetValue()) return;

	bool captureFloatFormat = GetIsCaptureRaw();

	g2dRenderTarget* buffer = NULL;
	if (l_pRenderer)
		buffer = l_pRenderer->GetRawBuffer();
	if (buffer && captureFloatFormat)
	{
		g2dScreenCaptureUtil::CaptureRenderTargetToImage(*buffer, o_pImage);
	}
	else
	{
		g2dScreenCaptureUtil::CaptureWindowToImage(*l_pWindow, o_pImage);
	}
}

//----------------------------------------------------------------------------
//	GetRibFilename() - get renderman filename
//----------------------------------------------------------------------------
fsLocator cptrRenderUtil::GetRibFilename()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	// Write bitmap to file
	int current_frame;
	itString pureFileName;
	tmlnTimeUtil::GetTimeInFrames( l_fSimTime, current_frame );
	captRenderOutputDataUtil::SetCurrentFrame( current_frame );
	captRenderOutputDataUtil::GenerateFilename( pureFileName, true );
	pureFileName += '.';
	pureFileName += 'r';
	pureFileName += 'i';
	pureFileName += 'b';

	fsLocator finalPath = data.m_OutputDirectory.GetValue();
	finalPath.Push( pureFileName );

	return finalPath;
}

//----------------------------------------------------------------------------
//	GetMiFilename() - get mental ray filename
//----------------------------------------------------------------------------
fsLocator cptrRenderUtil::GetMiFilename()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	// Write bitmap to file
	int current_frame;
	itString pureFileName;
	tmlnTimeUtil::GetTimeInFrames( l_fSimTime, current_frame );
	captRenderOutputDataUtil::SetCurrentFrame( current_frame );
	captRenderOutputDataUtil::GenerateFilename( pureFileName, true );
	pureFileName += '.';
	pureFileName += 'm';
	pureFileName += 'i';

	fsLocator finalPath = data.m_OutputDirectory.GetValue();
	finalPath.Push( pureFileName );

	return finalPath;
}

//----------------------------------------------------------------------------
//	CaptureFrame() - capture the frame
//----------------------------------------------------------------------------
void cptrRenderUtil::CaptureFrame()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	// wait for start time before capturing in batch mode
	if (l_fSimTime >= data.m_fStartTime.GetValue())
	{
		// Screen capture
		//
		float begin_time = appTime::GetTime();

		// convert to surface representation
		g2dImage* pImg = NULL;
		CaptureFrameToImage(pImg);

		g2dImage* pDownSampledImg = NULL;

		//	check if jittered or not
		if ( !data.m_bJitteredSampling.GetValue() )
		{
			pDownSampledImg = g2dImageCreate::Make(data.m_nWidth.GetValue(),
											 data.m_nHeight.GetValue(),
											 pImg->GetPixelFormat(),
											 g2dImage::e_SystemMemory);

			maFilter* filter = CreateFilter((captRenderOutputData::eFilterFunc)data.m_FilterFunc.GetValue());
			pDownSampledImg->CopyImage(*pImg, filter, 
				data.m_FilterWidth.GetValue(), data.m_FilterWidth.GetValue(),
				data.m_nCaptureSampling.GetValue(), data.m_nCaptureSampling.GetValue() );
			delete filter;
		}
		else
		{
			pDownSampledImg = pImg;
		}

		//	check if image or movie
		//
		if (   ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"BMP",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"TGA",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"JPG",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"PNG",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"DDS",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"PPM",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"DIB",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"HDR",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"PFM",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"EXR",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"TIF",3)==0 )
			)
		{
			// Write bitmap to file
			int current_frame;
			tmlnTimeUtil::GetTimeInFrames( l_fSimTime, current_frame );
			captRenderOutputDataUtil::SetCurrentFrame( current_frame );
			captRenderOutputDataUtil::GenerateFilename( l_OutputFilename );
			//DBG_LOG("capture frame (" << l_OutputFilename.c_str() << ")" );

			l_OutputFilename += itString(L".");

			l_OutputFilename += itString(data.m_CaptureFormat.GetValue().c_str());
			
			fsLocator dir = data.m_OutputDirectory.GetValue();
			dir.Push(l_OutputFilename);
			l_OutputFilenameFull = dir;

			data.m_OutputDirectory.SetValue(dir);
			data.m_OutputFileName.SetValue( l_OutputFilename );

			//std::string dir;
			//fsFileUtil::LocatorToANSIFilename( data.m_OutputDirectory, dir );
			//DBG_LOG( "SCU: directory= (" << dir.c_str() << ")" );
			DBG_LOG("writing image (" << l_OutputFilename << ")" );

			//	if the file exists, write it in the log
			//
			if (fsFileUtil::FileExists( dir ))
			{
				DBG_WARNING("Overwriting file during render: " << dir);
			}

			g2dImageSave::Save(dir, pDownSampledImg);

			dir = data.m_OutputDirectory.GetValue();
			dir.Pop();
			data.m_OutputDirectory.SetValue(dir);

			data.m_bCaptureMovie.SetValue(false);

			//	store the filename
			capture_file_finished();
		}
		else if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"AVI")==0 )
		{			
			cptrWriteAVI::Write( pDownSampledImg, data.m_OutputDirectory.GetValue(), 1 );
			data.m_bCaptureMovie.SetValue(true);
		}
		else if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"MOV")==0 )
		{
			cptrWriteQuickTime::Write( pDownSampledImg, data.m_OutputDirectory.GetValue(), l_fSimTime.AsSeconds(), 1 );
			data.m_bCaptureMovie.SetValue(true);
		}
		else
		{
			DBG_ASSERT(false,"Invalid file format selected - " << data.m_CaptureFormat.GetValue().c_str());
		}

		if ( !data.m_bJitteredSampling.GetValue() )
		{				
			delete pDownSampledImg;
			pDownSampledImg = NULL;
		}
		
		delete pImg;
	}
}

//------------------------------------------------------------------------
//	BeginCapture/EndCapture() - notify start and stop capturing so that
//		AVI files can be begun and ended
//------------------------------------------------------------------------
void cptrRenderUtil::BeginCapture()
{
//	captRenderOutputData& data = captRenderOutputDataUtil::Data();

//	rndrPrefsUtil::SetMultipassRendering(g3dPrefs::CurrentPrefs().m_bMultipassOn);
//	mnmApp::SetRenderer(g3dPrefs::CurrentPrefs().m_RendererType);
//	mtrlScriptObject::RecreateRenderTargets();

	OpenOutput();
	l_bRenderInProgress = true;
	ToggleMenuItems(false);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderUtil::EndCapture()
{
	CloseOutput();

	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	rndrPrefsMgr::ApplyPrefs(rndrPrefsMgr::e_ViewportPrefs);
	pfxPostEffectMgr::ApplyPostEffect(pfxPostEffectMgr::e_ViewportPfx);

	captRenderOutputDataUtil::EnableWireframe( g3dPrefs::CurrentPrefs().m_bRenderWireframe );
	rndrPrefsUtil::SetMultipassRendering(g3dPrefs::CurrentPrefs().m_bMultipassOn);
	mnmApp::SetRenderer(g3dPrefs::CurrentPrefs().m_RendererType);
	
	mtrlScriptObject::RecreateRenderTargets();

	// reset the values
	//
	l_fSimTime = maTime::c_ZeroTime;
	l_fSimTimeInc = tmlnTimeLine::GetFrameIncrement();
	appSimTime::SetTime( l_fSimTime.AsSeconds(), l_fSimTimeInc.AsSeconds() );
	l_bRenderInProgress = false;
	ToggleMenuItems(true);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool cptrRenderUtil::GetCaptureProgress()
{
	return l_bRenderInProgress;
}	

//------------------------------------------------------------------------
//	Open/CloseOutput - can be called to open/close an output file
//	(usually a movie).
//------------------------------------------------------------------------
void cptrRenderUtil::OpenOutput()
{
	// Do not try to open anything if doing renderman capture
	if ( g3dPrefs::CurrentPrefs().m_RendererEngine != g3dSceneRenderEngineCreate::e_Default )
		return;

	// Do not try to open anything if doing baking
	if (modeMode* pMode = modeModeMgr::GetCurrentMode())
	{
		if (dynamic_cast<cptrModeRenderBake*>(pMode))
		{
			return;
		}
	}

	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	// Open movie file
	//
	if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"AVI")==0 )
	{
		cptrWriteAVI::BeginCapture(data);

		captRenderOutputDataUtil::GenerateFilename( l_OutputFilename );
		
		l_OutputFilenameFull = data.m_OutputDirectory.GetValue();
		l_OutputFilenameFull.Push(l_OutputFilename);
		l_OutputFilenameFull.Push(itString(L".avi"));
	}
	else if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"MOV")==0 )
	{
		cptrWriteQuickTime::BeginCapture(l_pWindow, data);

		captRenderOutputDataUtil::GenerateFilename( l_OutputFilename );
		
		l_OutputFilenameFull = data.m_OutputDirectory.GetValue();
		l_OutputFilenameFull.Push(l_OutputFilename);
		l_OutputFilenameFull.Push(itString(L".mov"));
	}
}

void cptrRenderUtil::CloseOutput()
{
	// Do not try to close anything if doing renderman capture
	if ( g3dPrefs::CurrentPrefs().m_RendererEngine != g3dSceneRenderEngineCreate::e_Default )
		return;

	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	//	close movie file
	//
	if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"AVI")==0 )
	{
		if (data.m_SoundFile.GetValue().GetNumNames() > 0)
		{
			cptrWriteAVI::WriteAudio( data.m_SoundFile.GetValue() );
		}
		cptrWriteAVI::EndCapture();

		capture_file_finished();
	}
	else if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"MOV")==0 )
	{
		if (data.m_SoundFile.GetValue().GetNumNames() > 0)
		{
			cptrWriteQuickTime::WriteAudio( data.m_SoundFile.GetValue() );
		}
		cptrWriteQuickTime::EndCapture();

		capture_file_finished();
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderUtil::CaptureTimeCode(const maTime& i_fStartTime, const maTime& i_fEndTime)
{
	//	if the current time is valid, but the last frame wasn't then write out a time code
	//
	captRenderOutputData& data = captRenderOutputDataUtil::Data();
	if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"MOV")==0 )
	{
		cptrWriteQuickTime::WriteTimeCode( i_fStartTime.AsSeconds(), i_fEndTime.AsSeconds() );
	}
}


//------------------------------------------------------------------------
// These functions are used to do a high resolution capture by 
//	splitting it into 4 separate renders of the 4 quadrants.
//------------------------------------------------------------------------
bool cptrRenderUtil::GetCaptureQuadrants()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	// this could eventually be a separate flag on the dialog, 
	// but for now, just do quadrant rendering when sampling is used.
	//return (data.m_nCaptureSampling > 1);

	// Adding a check to see if the resolution is too big, even 
	// when not using capture sampling.
	int quad_div = get_quadrant_div();
	if ((data.m_bJitteredSampling.GetValue() ? 1 : data.m_nCaptureSampling.GetValue()) > 1)
		return (quad_div > 1);
	else 
		return (quad_div > 2);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderUtil::BeginQuadrants()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	if (l_fSimTime < data.m_fStartTime.GetValue()) return;

	int width, height;
	l_pWindow->GetDimensions(width, height);

	// make in d3dpool_default
	int quadrant_div = get_quadrant_div();
	l_pQuadImage = g2dImageCreate::Make(quadrant_div*width, 
										quadrant_div*height, 
										GetCapturePFD(),//l_pWindow->GetBackBufferPixelFormat(), 
										g2dImage::e_SystemMemory);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int	 cptrRenderUtil::GetNumQuadrants()
{
	int quadrant_div = get_quadrant_div();
	return quadrant_div*quadrant_div;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int	 cptrRenderUtil::GetQuadrantDivision()
{
	return get_quadrant_div();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderUtil::ConfigureViewport(int i_Index, camCamera &o_Camera)
{
	// viewport is (top,bottom, left,right) in normalized range from -1 to 1

	int quadrant_div = get_quadrant_div();
	float grid_size = 2.0f / (float) quadrant_div;

	int x_cell = i_Index % quadrant_div;
	int y_cell = i_Index / quadrant_div;
	
	float top = -1.0f + y_cell * grid_size;
	float bottom = top + grid_size;
	float left = -1.0f + x_cell * grid_size;
	float right = left + grid_size;

	//float top = (i_Index / 2) ? 0 : -1;
	//float bottom = (i_Index / 2) ? 1 : 0;
	//float left = (i_Index % 2) ? 0 : -1;
	//float right = (i_Index % 2) ? 1 : 0;

	o_Camera.SetSubViewport(top,bottom, left,right);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderUtil::CaptureQuadrant(int i_Index)
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	// wait for start time before capturing in batch mode
	if (l_fSimTime < data.m_fStartTime.GetValue()) return;

	int width, height;
	l_pWindow->GetDimensions(width, height);

	POINT dest_point;
	int quad_div = get_quadrant_div();
	dest_point.x = (i_Index % quad_div) * width;
	dest_point.y = (i_Index / quad_div) * height;
	
	g2dImage* qimage = NULL;
	CaptureFrameToImage(qimage);
	DBG_ASSERT(qimage != NULL, "CaptureWindowToImage failed");

	// blit qimage to l_pQuadImage
	l_pQuadImage->DrawImage(dest_point.x, dest_point.y, *qimage);
	delete qimage;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderUtil::EndQuadrants()
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	if (l_fSimTime < data.m_fStartTime.GetValue()) return;

	CaptureImage(l_pQuadImage);

	delete l_pQuadImage;
	l_pQuadImage = NULL;

}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderUtil::EndAnaglyphQuadrants(g2dImage* i_pImg,
										  const maFloatRGBA& color_filter, 
										  g2dPFD::PixelFormat rendererPFD)
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	if (l_fSimTime < data.m_fStartTime.GetValue()) return;

	maFilter* filter = CreateFilter((captRenderOutputData::eFilterFunc)data.m_FilterFunc.GetValue());

	i_pImg->CopyImage(*l_pQuadImage, filter, 
					data.m_FilterWidth.GetValue(), data.m_FilterWidth.GetValue(),
					data.m_nCaptureSampling.GetValue(), data.m_nCaptureSampling.GetValue() );

	captStereoUtil::ApplyAnaglyphColorFilter(i_pImg, color_filter, rendererPFD);
	
	delete filter;
	filter = NULL;
}

//------------------------------------------------------------------------
// just grab the buffer, don't update any state yet!
//------------------------------------------------------------------------
void cptrRenderUtil::GetQuadrants(g2dImage** o_pBuffer, int& o_w, int& o_h)
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	*o_pBuffer = NULL;

	// wait for start time before capturing in batch mode
	if (l_fSimTime < data.m_fStartTime.GetValue()) return;
	
	// Write out large resolution buffer to bmp
	int w=0, h=0;
	
	*o_pBuffer = l_pQuadImage;
	o_w = l_pQuadImage->GetWidth();
	o_h = l_pQuadImage->GetHeight();
//	cptrWriteUtil::GetSurfacePixels(l_pQuadSurface, o_pBuffer, &w, &h);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderUtil::ClearQuadrants()
{
	delete l_pQuadImage;
	l_pQuadImage = NULL;
}

//----------------------------------------------------------------------------
//	CaptureSurface() - capture the frame
//----------------------------------------------------------------------------
void cptrRenderUtil::CaptureImage(g2dImage* i_pImage)
{
	captRenderOutputData& data = captRenderOutputDataUtil::Data();

	// wait for start time before capturing in batch mode
	//
	if (l_fSimTime < data.m_fStartTime.GetValue()) 
		return;

	g2dImage* pDownSampledImg = NULL;

	//
	if ( !data.m_bJitteredSampling.GetValue() )
	{
		pDownSampledImg = g2dImageCreate::Make(data.m_nWidth.GetValue(),
										 data.m_nHeight.GetValue(),
										 i_pImage->GetPixelFormat(),
										 g2dImage::e_SystemMemory);
		maFilter* filter = CreateFilter((captRenderOutputData::eFilterFunc)data.m_FilterFunc.GetValue());
		pDownSampledImg->CopyImage(*i_pImage, filter, 
			data.m_FilterWidth.GetValue(), data.m_FilterWidth.GetValue(),
			data.m_nCaptureSampling.GetValue(), data.m_nCaptureSampling.GetValue() );
		delete filter;
	}
	else
	{
		pDownSampledImg = i_pImage;
	}

	// Screen capture
	//
	float begin_time = appTime::GetTime();

	if (   ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"BMP",3)==0 )
		|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"TGA",3)==0 )
		|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"JPG",3)==0 )
		|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"PNG",3)==0 )
		|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"DDS",3)==0 )
		|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"PPM",3)==0 )
		|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"DIB",3)==0 )
		|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"HDR",3)==0 )
		|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"PFM",3)==0 )
		|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"EXR",3)==0 )
		|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"TIF",3)==0 )
		)
	{
		// Write bitmap to file
		int current_frame;
		tmlnTimeUtil::GetTimeInFrames( l_fSimTime, current_frame );
		captRenderOutputDataUtil::SetCurrentFrame( current_frame );
		captRenderOutputDataUtil::GenerateFilename( l_OutputFilename );
		//DBG_LOG("capture frame (" << l_OutputFilename.c_str() << ")" );

		l_OutputFilename += itString(L".");
		l_OutputFilename += itString(data.m_CaptureFormat.GetValue().c_str());
		fsLocator dir = data.m_OutputDirectory.GetValue();
		dir.Push(l_OutputFilename);
		data.m_OutputDirectory.SetValue(dir);
		
		l_OutputFilenameFull = dir;
		//	if the file exists, write it in the log
		//
		if (fsFileUtil::FileExists( dir ))
		{
			DBG_WARNING("Overwritting file during render: " << dir);
		}

		g2dImageSave::Save(dir, pDownSampledImg);

		dir = data.m_OutputDirectory.GetValue();
		dir.Pop();
		data.m_OutputDirectory.SetValue(dir);

		data.m_bCaptureMovie.SetValue(false);
	}	
	else if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"AVI")==0 )
	{
		cptrWriteAVI::Write( pDownSampledImg, data.m_OutputDirectory.GetValue(), 1 );

		data.m_bCaptureMovie.SetValue(true);
	}
	else if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"MOV")==0 )
	{
		cptrWriteQuickTime::Write( pDownSampledImg, data.m_OutputDirectory.GetValue(), l_fSimTime.AsSeconds(), 1 );

		data.m_bCaptureMovie.SetValue(true);
	}
	else
	{
		DBG_ASSERT(false,"Invalid file format selected - " << data.m_CaptureFormat.GetValue().c_str());
	}

	if ( !data.m_bJitteredSampling.GetValue() )
	{				
		delete pDownSampledImg;
		pDownSampledImg = NULL;
	}

}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
maVector3d cptrRenderUtil::GetSamplePos(int i)
{
	DBG_ASSERT(i < l_SamplePoints.size(), "Bad sampling index");
	return l_SamplePoints[i];
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderUtil::ToggleMenuItems(bool i_bToggle)
{
	int mid;
	mid = guiMenuMgr::GetMenuItemID("File","New");
	if (mid != -1)
		guiMenuMgr::MenuObjectsEnable(mid,i_bToggle);
	mid = guiMenuMgr::GetMenuItemID("File","Open");
	if (mid != -1)
		guiMenuMgr::MenuObjectsEnable(mid,i_bToggle);
	mid = guiMenuMgr::GetMenuItemID("File","Revert");
	if (mid != -1)
		guiMenuMgr::MenuObjectsEnable(mid,i_bToggle);

#ifdef USE_WXWIDGETS
	DBG_ASSERT((twxSystem::g_pMainMenu != NULL), "MainMenu not yet initialized.");
	mid = twxSystem::g_pMainMenu->FindMenuItem(wxT("File"), wxT("Recent Files") );
	if (mid != -1)
		twxSystem::g_pMainMenu->Enable(mid, i_bToggle);
#endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrRenderUtil::SetPSflag(bool i_PS)
{
	m_bIsPhotoshop = i_PS;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool cptrRenderUtil::GetPSflag()
{
	return m_bIsPhotoshop;
}
