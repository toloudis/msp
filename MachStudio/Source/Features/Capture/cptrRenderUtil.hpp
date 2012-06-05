/****************************************************************************\
**	cptrRenderUtil.hpp
**
**		Supplies routines for saving the current scene into files or a movie
**
**	StudioGPU
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERUTIL_HPP
#error cptrRenderUtil.hpp multiply included
#endif
#define CPTR_RENDERUTIL_HPP

#ifndef CAPT_RENDEROUTPUTDATA_HPP
#include "Support/capt/captRenderOutputData.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class camCamera;
class chBinWriter;
class fsFileStream;
class fsLocator;
class g2dImage;
class g2dWindow;
struct g2dPixelR8G8B8A8;
class g3dSceneRenderer;
class maFloatRGBA;


//============================================================================
//============================================================================
namespace cptrRenderUtil
{
	//------------------------------------------------------------------------
	//	SetCapture() - turn on/off capturing
	//------------------------------------------------------------------------
	void SetCapture( bool i_bEnable );

	//------------------------------------------------------------------------
	//	GetCapture() - if capturing on/off
	//------------------------------------------------------------------------
	bool GetCapture();

	//------------------------------------------------------------------------
	//	SetCaptureFinished() - make capturing finish early
	//------------------------------------------------------------------------
	void SetCaptureFinished();

	//------------------------------------------------------------------------
	//	GetCaptureFinished() - if captured
	//		enough frames, return true
	//------------------------------------------------------------------------
	bool GetCaptureFinished();

	//------------------------------------------------------------------------
	//	set to the MAX time
	//------------------------------------------------------------------------
	void SetCaptureTimeMax( const maTime& i_fMaxTime );

	//------------------------------------------------------------------------
	//	SetCaptureWindow() - set the window to capture from
	//------------------------------------------------------------------------
	void SetCaptureWindow( g2dWindow* i_pWindow );
	void SetCaptureRenderer( g3dSceneRenderer* i_pRenderer );

	//------------------------------------------------------------------------
	//	SetAppWindow() - store the main window for the application
	//	so that it can be used as the capture window in default cases.
	//------------------------------------------------------------------------
	void SetAppWindow( g2dWindow* i_pWindow );
	g2dWindow* GetAppWindow();

	//------------------------------------------------------------------------
	//	GetSimTimeIncrement() - get the simTime increment that is used by
	//	UpdateSimTime().
	//------------------------------------------------------------------------
	float GetSimTimeIncrement();

	//------------------------------------------------------------------------
	// Return the full file path of the render
	//------------------------------------------------------------------------
	fsLocator& GetFullOutputFile();

	//------------------------------------------------------------------------
	//	Initialize()
	//------------------------------------------------------------------------
	void Initialize();

	//------------------------------------------------------------------------
	//	Reset the time to Initialize values
	//------------------------------------------------------------------------
	void ResetTime();

	//------------------------------------------------------------------------
	//	UpdateSimTime() - change the sim time
	//------------------------------------------------------------------------
	maTime UpdateSimTime();

	//------------------------------------------------------------------------
	//	SetSimTime() - set the sim time
	//------------------------------------------------------------------------
	void SetSimTime(const maTime& i_fSimTime);

	//------------------------------------------------------------------------
	// is the sim time past the start time?
	//------------------------------------------------------------------------
	bool ReadyToCapture();

	//------------------------------------------------------------------------
	//	CaptureSurface() - capture the frame from the given surface
	//------------------------------------------------------------------------
	void CaptureImage(g2dImage* i_pImage);

	//------------------------------------------------------------------------
	//	CaptureFrame() - capture the frame
	//------------------------------------------------------------------------
	void CaptureFrame();

	//------------------------------------------------------------------------
	//	BeginCapture/EndCapture() - notify start and stop capturing so that
	//		AVI files can be begun and ended
	//------------------------------------------------------------------------
	void BeginCapture();
	void EndCapture();

	//------------------------------------------------------------------------
	// GetCaptureProgess returns the progress state of the capture
	//------------------------------------------------------------------------
	bool GetCaptureProgress();

	//------------------------------------------------------------------------
	//	Open/CloseOutput - can be called to open/close an output file
	//	(usually a movie).
	//------------------------------------------------------------------------
	void OpenOutput();
	void CloseOutput();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CaptureTimeCode(const maTime& i_fStartTime, const maTime& i_fEndTime);

	//------------------------------------------------------------------------
	// These functions are used to do a high resolution capture by 
	//	splitting it into 4 separate renders of the 4 quadrants.
	// New: Although still called "quadrants", this code can do more
	//	than 4 sections of the frame at a time. Use GetNumQuadrants()
	//	to find how many times to call CaptureQuadrant(). Call
	//	ConfigureViewport with the camera before rendering each quadrant.
	//------------------------------------------------------------------------
	bool GetCaptureQuadrants();
	void BeginQuadrants();
	int	 GetNumQuadrants();
	int	 GetQuadrantDivision();
	void ConfigureViewport(int i_Index, camCamera &o_Camera);
	void CaptureQuadrant(int i_Index);
	void EndQuadrants();
	void EndAnaglyphQuadrants(g2dImage* ana,
							  const maFloatRGBA& color_filter,
							  g2dPFD::PixelFormat rendererPFD);
	
	void ClearQuadrants();
	void GetQuadrants(g2dImage** o_pBuffer, int& o_w, int& o_h);

	//----------------------------------------------------------------------------
	//	SetCaptureToneMapped - set the flag for the capture tonemapped pixel render
	//----------------------------------------------------------------------------
	void SetCaptureToneMapped( bool i_bCaptureToneMapped );

	//----------------------------------------------------------------------------
	//	GetIsCaptureRaw - need to know if capturing raw buffer or rgba color
	//----------------------------------------------------------------------------
	bool GetIsCaptureRaw();

	//----------------------------------------------------------------------------
	//	GetCapturePFD - determine what pfd will actually be captured.
	//----------------------------------------------------------------------------
	g2dPFD GetCapturePFD();

	//----------------------------------------------------------------------------
	//	CaptureFrameToImage() - creates a g2dImage in system mem. CALLER MUST DELETE!
	//----------------------------------------------------------------------------
	void CaptureFrameToImage(g2dImage*& o_pImage);

	//------------------------------------------------------------------------
	// GetSamplePos() - for jittered sampling, return the sample point for 
	//	the given sampling pass
	//------------------------------------------------------------------------
	maVector3d GetSamplePos(int i);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ToggleMenuItems(bool i_bToggle);

	//--------------------------------------------------------------------
	//	Functions to check if the frame will be exported to Photoshop
	//--------------------------------------------------------------------
	void SetPSflag(bool i_PS);
	bool GetPSflag();

	//----------------------------------------------------------------------------
	//	GetRibFilename()
	//----------------------------------------------------------------------------
	fsLocator GetRibFilename();

	//----------------------------------------------------------------------------
	//	GetMiFilename()
	//----------------------------------------------------------------------------
	fsLocator GetMiFilename();


}


