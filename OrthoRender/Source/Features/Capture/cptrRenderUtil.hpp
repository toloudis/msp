/****************************************************************************\
**  cptrRenderUtil.hpp
**
**      Supplies routines for saving the current scene into files or a movie
**
**	Extra Large Technology
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERUTIL_HPP
#error cptrRenderUtil.hpp multiply included
#endif
#define CPTR_RENDERUTIL_HPP

#ifndef CPTR_RENDEROUTPUTDATA_HPP
#include "Features/Capture/cptrRenderOutputData.hpp"
#endif

#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
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
struct g2dPixelR8G8B8;


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
	//	set to the MAX number of frames for this scene
	//	in batch mode this would set this every time.
	//------------------------------------------------------------------------
	void SetCaptureMax( int i_nCaptureMax );

	//------------------------------------------------------------------------
	//	set to the MAX time
	//------------------------------------------------------------------------
	void SetCaptureTimeMax( float i_fMaxTime );

	//------------------------------------------------------------------------
	//	SetCaptureWindow() - set the window to capture from
	//------------------------------------------------------------------------
	void SetCaptureWindow( g2dWindow* i_pWindow );

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
	void UpdateSimTime();

	//------------------------------------------------------------------------
	//	SetSimTime() - set the sim time
	//------------------------------------------------------------------------
	void SetSimTime(float i_fSimTime);

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
	//	Open/CloseOutput - can be called to open/close an output file
	//	(usually a movie).
	//------------------------------------------------------------------------
	void OpenOutput();
	void CloseOutput();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CaptureTimeCode(const float i_fStartTime, const float i_fEndTime);

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

	void EndQuadrants(g2dPixelR8G8B8** o_pBuffer, int& o_w, int& o_h);
	void CaptureFrame(g2dPixelR8G8B8** o_pBuffer, int& o_w, int& o_h);

	//------------------------------------------------------------------------
	// GetSamplePos() - for jittered sampling, return the sample point for 
	//	the given sampling pass
	//------------------------------------------------------------------------
	maVector3d GetSamplePos(int i);
}

