/****************************************************************************\
**  cptrRenderUtil.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderUtil.hpp"

#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Capture/cptrWriteAVI.hpp"
#include "Features/Capture/cptrWriteQuickTime.hpp"
#include "Features/Capture/cptrWriteUtil.hpp"
#include "Features/Capture/orthoAvatarDataUtil.hpp"
#include "Features/ObjectManip/orthoModeObjectManip.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Features/RenderPrefs/rndrPrefsUtil.hpp"
#include "Features/Requests/orthoRemoteCommandMgr.hpp"
#include "MainApp/LightWaitMessage.hpp"
#include "MainApp/mnmApp.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"

#include <windows.h>

#undef CreateFile
#undef DeleteFile

#include "Core/App/appSimTime.hpp"
#include "Core/App/appTime.hpp"
#include "Core/Ch/chReader.hpp"
#include "Core/Ch/chWriter.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Fs/fsFileStream.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Core/Ma/maFunctions.hpp"
#include "Graphics/Cam/camCamera.hpp"
#include "Graphics/G2d/g2dImageCreate.hpp"
#include "Graphics/G2d/g2dImageSave.hpp"
#include "Graphics/G2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/G2d/g2dSystem.hpp"
#include "Graphics/G2d/g2dWindow.hpp"
#include "Graphics/G3d/g3dSingleLightRendering.hpp"

// #pragma comment( lib "rpcrt4")
#include "rpcdce.h"


//============================================================================
//============================================================================
namespace LightWaitMessaging 
{
	extern LightWaitMessageProducer *l_Producer;
};


//============================================================================
//external access to shared remote rendering parameters
//============================================================================
namespace mnpPackage
{
	extern orthoModeObjectManip* l_pModeObjectManip;
}

//============================================================================
//============================================================================
namespace
{
	//
	//	variables
	//
	bool l_bScreenCapEnabled	= false;

	float l_fSimTime;
	float l_fSimTimeInc;
	float l_fSimTimeMax;
	float l_fLeadInTime;
	float l_fLeadOutTime;

	int	l_Counter		= 0;
	int l_CounterMax	= 0;


	std::string l_OutputFilename;
	fsLocator	l_OutputFilenameFull;

	g2dWindow* l_pWindow = NULL;	// the capture window
	g2dWindow* l_pAppWindow = NULL;	// the application window
	g2dImage* l_pQuadImage = NULL;	// large resolution filled in by quadrants

	std::vector<maVector3d> l_SamplePoints;

	//------------------------------------------------------------------------
	// compute how may divisions on a side we need to make to render the large frame
	// in manageable chunks. If quadrant_div is 2, then 2*2=4 quadrants 
	// will be rendered.
	//------------------------------------------------------------------------
	int get_quadrant_div()
	{
		//return 2;

		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
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
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
		
		//	store the filename
		int index = data.m_OutputFiles.GetValue().size();

		std::string path;
		fsFileUtil::LocatorToANSIFilename( l_OutputFilenameFull, path );
		data.m_OutputFiles.SetValueText(index, path);
		data.m_OutputFiles.SetValueFlag(index, true);
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
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	l_fSimTime = data.m_fEndTime.GetValue();
	l_Counter = l_CounterMax;
}

//------------------------------------------------------------------------
//	GetCaptureFinished() - if captured
//		enough frames, return true
//------------------------------------------------------------------------
bool cptrRenderUtil::GetCaptureFinished()
{
	//DBG_LOG2( "Counter %d of %d", l_Counter, l_CounterMax );

	//if ( l_CounterMax > 0 && l_Counter >= l_CounterMax)
	if ( l_fSimTime >= l_fSimTimeMax)
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
//	set to the MAX number of frames for this scene
//	in batch mode this would set this every time.
//------------------------------------------------------------------------
void cptrRenderUtil::SetCaptureMax( int i_nCaptureMax )
{
	l_CounterMax = i_nCaptureMax;
}

//------------------------------------------------------------------------
//	set to the MAX time
//------------------------------------------------------------------------
void cptrRenderUtil::SetCaptureTimeMax( float i_fMaxTime )
{
	l_fSimTimeMax = i_fMaxTime;
}

//------------------------------------------------------------------------
//	SetCaptureWindow() - set the window to capture from
//------------------------------------------------------------------------
void cptrRenderUtil::SetCaptureWindow( g2dWindow* i_pWindow )
{
	DBG_ASSERT0( i_pWindow != 0, "cannot capture from a null window" );

	l_pWindow = (i_pWindow);
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
	return l_fSimTimeInc;
}

//----------------------------------------------------------------------------
//	Initialize()
//----------------------------------------------------------------------------
void cptrRenderUtil::Initialize()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	ResetTime();

	l_Counter = 0;
	l_pWindow = 0;

	if (data.m_bJitteredSampling.GetValue())
	{
		maFunctions::GetSamples2D(data.m_nCaptureSampling.GetValue(), l_SamplePoints);
	}
}

//------------------------------------------------------------------------
//	Reset the time to Initialize values
//------------------------------------------------------------------------
void cptrRenderUtil::ResetTime()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	l_fSimTimeInc = 1.0f / (data.m_fCaptureFPS.GetValue() * data.m_nMotionSamplesPerFrame.GetValue());
	l_fSimTime = data.m_fStartTime.GetValue() - l_fSimTimeInc; //0.0f;
}

//----------------------------------------------------------------------------
//	UpdateSimTime() - change the sim time
//----------------------------------------------------------------------------
void cptrRenderUtil::UpdateSimTime()
{
	appSimTime::SetTime( l_fSimTime, l_fSimTimeInc );

	l_fSimTime += l_fSimTimeInc;

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	float fps = data.m_fCaptureFPS.GetValue() * data.m_nMotionSamplesPerFrame.GetValue();
	l_fSimTime = floor(l_fSimTime * fps + 0.5f) / fps;	//round nearest, align to frames to prevent accumulation error
}

//------------------------------------------------------------------------
//	SetSimTime() - set the sim time
//------------------------------------------------------------------------
void cptrRenderUtil::SetSimTime(float i_fSimTime)
{
	l_fSimTime = i_fSimTime;
	appSimTime::SetTime( l_fSimTime, l_fSimTimeInc );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool cptrRenderUtil::ReadyToCapture()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	return (l_fSimTime >= data.m_fStartTime.GetValue());
}



// =========================================================================================
// =========================================================================================

char *MakeUniqueFileName(void);

char *MakeUniqueFileName(void)
{
   char *sUUID = NULL;
   RPC_CSTR sTemp;
   BOOL bAllocated = FALSE;

   UUID *pUUID = new UUID;
   bAllocated = TRUE;

   if (pUUID != NULL)
   {
      HRESULT hr;
      hr = UuidCreateSequential(pUUID);
      if (hr == RPC_S_OK)
      {
         hr = UuidToString(pUUID, &sTemp);
#if 1
		 if (hr == RPC_S_OK)
         {
            // sTemp.MakeUpper();
			int n = strlen((char *) sTemp) + 5;
			sUUID = new char[n];
			strcpy(sUUID,(char *) sTemp);
			// strcat(sUUID,".PNG");
//            sUUID.MakeUpper();
            RpcStringFree(&sTemp);
			// delete sTemp;
         }
#endif
	  }
      if (bAllocated)
      {
         delete pUUID;
         pUUID = NULL;
      }
   }
   return sUUID;
}


// =========================================================================================
// =========================================================================================

//----------------------------------------------------------------------------
//	CaptureFrame() - capture the frame
//----------------------------------------------------------------------------
void cptrRenderUtil::CaptureFrame()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
   	PrefsData& prefsData = PrefsMgr::Data();

	// wait for start time before capturing in batch mode
	if (l_fSimTime >= data.m_fStartTime.GetValue())
	{
		// Screen capture
		//
		float begin_time = appTime::GetTime();

		// convert to surface representation
		g2dImage* pImg = NULL;
		g2dScreenCaptureUtil::CaptureWindowToImage(*l_pWindow, pImg);

		if (   ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"BMP",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"TGA",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"JPG",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"PNG",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"DDS",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"PPM",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"DIB",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"HDR",3)==0 )
			|| ( strncmp(data.m_CaptureFormat.GetValue().c_str(),"PFM",3)==0 )
			)
		{
			// Write bitmap to file
			cptrRenderOutputDataUtil::SetCurrentFrame( l_Counter );
			cptrRenderOutputDataUtil::GenerateFilename( l_OutputFilename );
			//DBG_LOG1("capture frame (%s)", l_OutputFilename.c_str() );

			l_OutputFilename += ".";
			l_OutputFilename += data.m_CaptureFormat.GetValue().c_str();

			// =========================================================================================
			// =========================================================================================
			orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];

			char *uuidString = MakeUniqueFileName();
			std::string testName = uuidString;
			delete[] uuidString;
			testName.append(rr.renderName);
			testName.append(".");
			testName.append(data.m_CaptureFormat.GetValue().c_str());

			fsLocator dir;
			dir.Clear();
			dir.Push(prefsData.m_Lightwait_MessagingLocalOutputLocation.GetValue().c_str());
			dir.Push(testName.c_str());

			l_OutputFilenameFull = dir;

			data.m_OutputDirectory.SetValue(dir);
			data.m_OutputFileName.SetValue( itString(l_OutputFilename.c_str()) );

			std::string sdir;
			fsFileUtil::LocatorToANSIFilename( data.m_OutputDirectory.GetValue(), sdir );
			//DBG_LOG1("cRU:CF: directory= (%s)", sdir.c_str() );
			//DBG_LOG1("   writing image (%s)", l_OutputFilename.c_str() );

			g2dImageSave::Save(dir, pImg);

			dir = data.m_OutputDirectory.GetValue();
			dir.Pop();
			data.m_OutputDirectory.SetValue(dir);

			data.m_bCaptureMovie.SetValue(false);

			if (rr.responseFormat != "XML") 
			{
				DBG_WARNING( "unsupported Render Response Format " << rr.responseFormat.c_str() << ", only XML supported" );
			}
			if (rr.GetRenderResponse() != NULL) 
			{
				int animIndex = -1;

				// find which animation this frame belongs to
				//
				for (int k = 0; k < orthoRemoteCommandMgr::m_AnimFrameData.size(); ++k) 
				{
					if ((l_fSimTime >= orthoRemoteCommandMgr::m_AnimFrameData[k].m_fAnimStartTime) &&
						(l_fSimTime < orthoRemoteCommandMgr::m_AnimFrameData[k].m_fAnimStartTime +
						orthoRemoteCommandMgr::m_AnimFrameData[k].m_fAnimLength)) 
					{
						animIndex = k;
						break;
					}
				}

				//	add the image
				//
				if ( animIndex >= 0 )
				{
					rr.GetRenderResponse()->addImage((char *) testName.c_str(),
						(char *) data.m_CaptureFormat.GetValue().substr(data.m_CaptureFormat.GetValue().length()-3,3).c_str(),
						rr.renderWidth,
						rr.renderHeight,
						rr.renderFPS,
						orthoRemoteCommandMgr::m_AnimFrameData[animIndex].m_AnimFile,
						orthoRemoteCommandMgr::m_AnimFrameData[animIndex].m_fAnimStartTime,
						orthoRemoteCommandMgr::m_AnimFrameData[animIndex].m_fAnimLength,
						orthoRemoteCommandMgr::m_AnimFrameData[animIndex].m_OrientationY,
						(char *) rr.renderCamera.c_str(),
						l_fSimTime,
						data.m_bMaskFrame.GetValue());

					DBG_WARNING4("--Frame %d, Time %6.3f (%d deg) - %s", l_Counter+1, l_fSimTime, (int)(orthoRemoteCommandMgr::m_AnimFrameData[animIndex].m_OrientationY * maConstants::c_dRadToAngle), testName.c_str() );
				}
				else if (animIndex == -1) 
				{
					rr.GetRenderResponse()->addImage((char *) testName.c_str(),
						(char *) data.m_CaptureFormat.GetValue().substr(data.m_CaptureFormat.GetValue().length()-3,3).c_str(),
						rr.renderWidth,
						rr.renderHeight,
						rr.renderFPS,
						"NONE",
						0.0,
						0.0,
						0,
						(char *) rr.renderCamera.c_str(),
						l_fSimTime,
						data.m_bMaskFrame.GetValue());

					DBG_WARNING3("--Frame %d, Time %6.3f Unknown Anim - %s", l_Counter+1, l_fSimTime, testName.c_str() );
				}
			}

			//	store the filename
			capture_file_finished();
		}
		else if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"AVI")==0 )
		{
			cptrWriteAVI::Write( pImg, data.m_OutputDirectory.GetValue(), (data.m_bJitteredSampling.GetValue() ? 1 : data.m_nCaptureSampling.GetValue()) );
			data.m_bCaptureMovie.SetValue(true);
		}
		else if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"MOV")==0 )
		{
			cptrWriteQuickTime::Write( pImg, data.m_OutputDirectory.GetValue(), l_fSimTime, (data.m_bJitteredSampling.GetValue() ? 1 : data.m_nCaptureSampling.GetValue()) );
			data.m_bCaptureMovie.SetValue(true);
		}
		else if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"NONE")==0 )
		{
			DBG_LOG0("image format is NONE; skipping.");
		}
		else
		{
			DBG_ASSERT1(false,"Invalid file format selected (%s)", data.m_CaptureFormat.GetValue().c_str());
		}
		
		delete pImg;

		l_Counter++;
		//if (l_Counter == 1) // debug output frame time on first write
		//{
		//	float end_time = appTime::GetTime();
		//	DBG_WARNING1("Screen capture time: %f", end_time - begin_time);
		//}
	}
}

//------------------------------------------------------------------------
//	BeginCapture/EndCapture() - notify start and stop capturing so that
//		AVI files can be begun and ended
//------------------------------------------------------------------------
void cptrRenderUtil::BeginCapture()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

//	g3dSingleLightRendering::SetDoSingleLightRendering(g3dPrefs::CurrentPrefs().m_bShadowsOn);
	rndrPrefsUtil::SetMultipassRendering(g3dPrefs::CurrentPrefs().m_bShadowsOn);
	mnmApp::SetRenderer(g3dPrefs::CurrentPrefs().m_RendererType);
	mtrlScriptObject::RecreateRenderTargets();

	OpenOutput();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderUtil::EndCapture()
{
	CloseOutput();

	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	rndrPrefsMgr::ApplyPrefs(rndrPrefsMgr::e_ViewportPrefs);
//	g3dSingleLightRendering::SetDoSingleLightRendering(g3dPrefs::CurrentPrefs().m_bShadowsOn);
	rndrPrefsUtil::SetMultipassRendering(g3dPrefs::CurrentPrefs().m_bShadowsOn);
	mnmApp::SetRenderer(g3dPrefs::CurrentPrefs().m_RendererType);
	mtrlScriptObject::RecreateRenderTargets();

	// reset the values
	//
	l_fSimTime = 0.0f;
	l_fSimTimeInc = 1.0f / (data.m_fCaptureFPS.GetValue() * data.m_nMotionSamplesPerFrame.GetValue());
	appSimTime::SetTime( l_fSimTime, l_fSimTimeInc );
}

//------------------------------------------------------------------------
//	Open/CloseOutput - can be called to open/close an output file
//	(usually a movie).
//------------------------------------------------------------------------
void cptrRenderUtil::OpenOutput()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	// Open movie file
	//
	if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"AVI")==0 )
	{
		cptrWriteAVI::BeginCapture(data);

		cptrRenderOutputDataUtil::GenerateFilename( l_OutputFilename );
		
		l_OutputFilenameFull = data.m_OutputDirectory.GetValue();
		std::string fname( l_OutputFilename );
		fname += ".avi";
		l_OutputFilenameFull.Push(fname.c_str());
	}
	else if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"MOV")==0 )
	{
		cptrWriteQuickTime::BeginCapture(l_pWindow, data);

		cptrRenderOutputDataUtil::GenerateFilename( l_OutputFilename );
		
		l_OutputFilenameFull = data.m_OutputDirectory.GetValue();
		std::string fname( l_OutputFilename );
		fname += ".mov";
		l_OutputFilenameFull.Push(fname.c_str());
	}
}

void cptrRenderUtil::CloseOutput()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

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
void cptrRenderUtil::CaptureTimeCode(const float i_fStartTime, const float i_fEndTime)
{
	//	if the current time is valid, but the last frame wasn't then write out a time code
	//
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
	if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"MOV")==0 )
	{
		cptrWriteQuickTime::WriteTimeCode( i_fStartTime, i_fEndTime );
	}
}


//------------------------------------------------------------------------
// These functions are used to do a high resolution capture by 
//	splitting it into 4 separate renders of the 4 quadrants.
//------------------------------------------------------------------------
bool cptrRenderUtil::GetCaptureQuadrants()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

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
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	if (l_fSimTime < data.m_fStartTime.GetValue()) return;

	int width, height;
	l_pWindow->GetDimensions(width, height);

	// make in d3dpool_default
	int quadrant_div = get_quadrant_div();
	l_pQuadImage = g2dImageCreate::Make(quadrant_div*width, 
										quadrant_div*height, 
										l_pWindow->GetBackBufferPixelFormat(), 
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
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	// wait for start time before capturing in batch mode
	if (l_fSimTime < data.m_fStartTime.GetValue()) return;

	int width, height;
	l_pWindow->GetDimensions(width, height);

	POINT dest_point;
	int quad_div = get_quadrant_div();
	dest_point.x = (i_Index % quad_div) * width;
	dest_point.y = (i_Index / quad_div) * height;
	
	g2dImage* qimage = NULL;
	g2dScreenCaptureUtil::CaptureWindowToImage(*(g2dWindow*)l_pWindow, qimage);
	DBG_ASSERT0(qimage != NULL, "CaptureWindowToImage failed");

	// blit qimage to l_pQuadImage
	l_pQuadImage->DrawImage(dest_point.x, dest_point.y, *qimage);
	delete qimage;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cptrRenderUtil::EndQuadrants()
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	if (l_fSimTime < data.m_fStartTime.GetValue()) return;

	CaptureImage(l_pQuadImage);

	delete l_pQuadImage;
	l_pQuadImage = NULL;

}

//------------------------------------------------------------------------
// just grab the buffer, don't update any state yet!
//------------------------------------------------------------------------
void cptrRenderUtil::EndQuadrants(g2dPixelR8G8B8** o_pBuffer, int& o_w, int& o_h)
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	*o_pBuffer = NULL;

	// wait for start time before capturing in batch mode
	if (l_fSimTime < data.m_fStartTime.GetValue()) return;
	
	// Write out large resolution buffer to bmp
	int w=0, h=0;
	
	l_pQuadImage->GetPixels(o_pBuffer, &w, &h);
//	cptrWriteUtil::GetSurfacePixels(l_pQuadSurface, o_pBuffer, &w, &h);

	o_w = w;
	o_h = h;

//	l_pQuadSurface->Release();
//	l_pQuadSurface = NULL;

	delete l_pQuadImage;
	l_pQuadImage = NULL;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// just grab the buffer, don't update any state yet!
void cptrRenderUtil::CaptureFrame(g2dPixelR8G8B8** o_pBuffer, int& o_w, int& o_h)
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	*o_pBuffer = NULL;
	// wait for start time before capturing in batch mode
	if (l_fSimTime >= data.m_fStartTime.GetValue())
	{
		g2dImage* pImg = NULL;
		g2dScreenCaptureUtil::CaptureWindowToImage(*l_pWindow, pImg);
		int w=0,h=0;
		pImg->GetPixels(o_pBuffer, &w, &h);
		o_w = w;
		o_h = h;
		delete pImg;
	}
}

//----------------------------------------------------------------------------
//	CaptureSurface() - capture the frame
//----------------------------------------------------------------------------
void cptrRenderUtil::CaptureImage(g2dImage* i_pImage)
{
	cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

	// wait for start time before capturing in batch mode
	if (l_fSimTime < data.m_fStartTime.GetValue()) return;

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
		)
	{
		// Write bitmap to file
		cptrRenderOutputDataUtil::SetCurrentFrame( l_Counter );
		cptrRenderOutputDataUtil::GenerateFilename( l_OutputFilename );
		//DBG_LOG1("capture frame (%s)", l_OutputFilename.c_str() );

		l_OutputFilename += ".";
		l_OutputFilename += data.m_CaptureFormat.GetValue().c_str();
		fsLocator dir = data.m_OutputDirectory.GetValue();
		dir.Push(itString(l_OutputFilename.c_str()));
		data.m_OutputDirectory.SetValue(dir);

		g2dImageSave::Save(dir, i_pImage);

		dir = data.m_OutputDirectory.GetValue();
		dir.Pop();
		data.m_OutputDirectory.SetValue(dir);

		data.m_bCaptureMovie.SetValue(false);
	}
	else if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"AVI")==0 )
	{
		cptrWriteAVI::Write( i_pImage, data.m_OutputDirectory.GetValue(), (data.m_bJitteredSampling.GetValue() ? 1 : data.m_nCaptureSampling.GetValue()) );

		data.m_bCaptureMovie.SetValue(true);
	}
	else if ( strcmp(data.m_CaptureFormat.GetValue().c_str(),"MOV")==0 )
	{
		cptrWriteQuickTime::Write( i_pImage, data.m_OutputDirectory.GetValue(), l_fSimTime, (data.m_bJitteredSampling.GetValue() ? 1 : data.m_nCaptureSampling.GetValue()) );

		data.m_bCaptureMovie.SetValue(true);
	}
	else
	{
		DBG_ASSERT1(false,"Invalid file format selected (%s)", data.m_CaptureFormat.GetValue().c_str());
	}

	l_Counter++;
	//if (l_Counter == 1) // debug output frame time on first write
	//{
	//	float end_time = appTime::GetTime();
	//	DBG_WARNING1("Screen capture time: %f", end_time - begin_time);
	//}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
maVector3d cptrRenderUtil::GetSamplePos(int i)
{
	DBG_ASSERT0(i < l_SamplePoints.size(), "Bad sampling index");
	return l_SamplePoints[i];
}

