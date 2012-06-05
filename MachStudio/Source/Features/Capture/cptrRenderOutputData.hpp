/********************************************************************************************\
**  cptrRenderOutputData.hpp
**
**		RenderOutput data.  This is data relating to rendering of scenes that IS NOT related
**	to render specifics.
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef CPTR_RENDEROUTPUTDATA_HPP
#error cptrRenderOutputData.hpp multiply included
#endif
#define CPTR_RENDEROUTPUTDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_DIRECTORY_HPP
#include "Core/prty/prtyDirectory.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_LISTCHECKED_HPP
#include "Core/prty/prtyListChecked.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif
#ifndef PRTY_TRIGGER_HPP
#include "Core/prty/prtyTrigger.hpp"
#endif 

#ifndef TMLN_TIMEINOUTDATA_HPP
#include "Support/tmln/tmlnTimeInOutData.hpp"
#endif

#include <list>
#include <string>
#include <vector>


//============================================================================
//============================================================================
class cptrRenderOutputCameraDriverData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	cptrRenderOutputCameraDriverData()
	:	m_DriverNumber("Driver Number"),
		m_bCapture("Driver Capture Flag")
	{
	};

public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	prtyInt32		m_DriverNumber;			// not saved
	prtyBoolean		m_bCapture;				// not saved
};

//============================================================================
//============================================================================
class cptrRenderOutputCamerasData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	cptrRenderOutputCamerasData()
	:	m_CameraName("Camera Name"),
		m_bCapture("Camera Capture Flag")
	{
	};

public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	prtyText		m_CameraName;			// not saved
	prtyBoolean		m_bCapture;				// not saved

	std::list<cptrRenderOutputCameraDriverData> m_CameraCaptureData;		// not saved
};

//============================================================================
//============================================================================
class cptrRenderOutputData
{
public:
	cptrRenderOutputData()
	:	m_fCaptureFPS("Capture FPS"),
		m_nMotionSamplesPerFrame("Motion Samples Per Frame"),
		m_nCaptureSampling("Capture Sampling"),
		m_bJitteredSampling("Jittered Sampling"),
		m_nCaptureMax("Capture Max"),
		m_OutputDirectory("Output Directory"),
		m_OutputDirectoryRoot("Output Directory Root"),
		m_SoundFile("Sound File"),
		m_Prefix("Prefix"),
		m_bCaptureMovie("Capture Movie"),
		m_CaptureFormat("Capture Format"),
		m_CompressCode("Compress Code"),
		m_nWidth("Width"),
		m_nHeight("Height"),
		m_bUseSceneFilenameInFilename("Use Scene Filename In Filename"),
		m_bUseCameraNameInFilename("Use Camera Name In Filename"),
		m_bUseCompressionInFilename("Use Compression Type In Filename"),
		m_bUseSceneFilenameAsDirectory("Use Scene Filename As Directory"),
		m_bUseCameraNameAsDirectory("Use Camera Name As Directory"),
		m_bUseResolutionAsDirectory("Use Resolution As Directory"),
		m_bUseRenderDriversAsUniqueCameras("Treat capture drivers as unique cameras"),
		m_bCaptureAllCameras("Capture All Cameras"),
		m_bDisplayTimeCode("Display Time Code"),
		m_bOutputTitleCard("Output Title Card"),
		m_bKeepFrameOpenAfterRender("Keep Frame Open After Render"),
		m_bShowRenderProgressDialog("Show Render Progress Dialog"),
		m_fLeadIn("Lead-In (seconds)"),
		m_fLeadOut("Lead-Out (seconds)"),
		m_fStartTime("Start Time"),
		m_fEndTime("End Time"),
		m_nCounterDigits("Counter Digits"),
		//m_nSubdivLevel("Subdiv Level"),
		m_bSmoothing("Smoothing"),			// Subdivision smoothing
		m_fMarkerInTime("Marker In Time"),
		m_fMarkerOutTime("Marker Out Time"),
		m_bUseMarkerTimes("Use Marker Times"),
		m_bSendPostEmailAddress("Post Render Email Send"),
		m_PostEmailAddress("Post Render Email Send Addresses"),
		m_bExecutePostCommand("Post Render Command or Python File to Execute"),
		m_PostCommand("Post Render Command To Execute"),
		m_bBatchMode("Batch Mode"),
		m_nCurrentFrame("Current Frame"),
		m_CurrentScene("Current Scene"),
		m_CurrentCamera("Current Camera"),
		m_CurrentSceneFilename("Current Scene Filename"),
		m_NumberOfScenes("Number Of Scenes"),
		m_CurrentSceneNumber("Current Scene Number"),
		m_RenderPosTime("Render Pos Time"),
		m_RenderPosScene("Render Pos Scene"),
		m_RenderPosCamera("Render Pos Camera"),
		m_RenderPosSaveFile("Render Pos Save File"),
		m_RenderOutputDataFile("Render Output Data File"),
		m_OutputFileName("Output File Name"),
		m_OutputMovieDirectory("Output Movie Directory"),
		m_Cameras("Cameras"),
		m_bRenderPosUse("Resume"),
		m_tRenderPosDelete("Delete Resume Point"),
		m_Resolution("Resolutions"),
		m_OutputFiles("Rendered Files"),
		m_bMoviePerDriver("Movie Per Capture Driver"),
		m_bUseLayerVisibility("Use Layer Visibility")
	{
	};

public:
	//---------------------------------------------------------------------------
	//	data
	//---------------------------------------------------------------------------

	// audio
	prtyFilePath	m_SoundFile;
	// cameras
	prtyBoolean		m_bCaptureAllCameras;
	prtyListChecked	m_Cameras;
	prtyBoolean		m_bUseRenderDriversAsUniqueCameras;
	// dimensions
	prtyText		m_Resolution;					// not saved to file
	prtyInt32		m_nWidth;
	prtyInt32		m_nHeight;
	prtyInt32		m_nCaptureSampling;
	prtyBoolean		m_bJitteredSampling;
	// filename
	prtyText		m_Prefix;
	prtyBoolean		m_bUseSceneFilenameInFilename;
	prtyBoolean		m_bUseCameraNameInFilename;
	prtyBoolean		m_bUseCompressionInFilename;
	prtyInt32		m_nCounterDigits;
	// frames
	prtyFloat		m_fCaptureFPS;
	prtyInt32		m_nMotionSamplesPerFrame;
	// location
	prtyDirectory	m_OutputDirectory;				// not saved to file
	prtyDirectory	m_OutputDirectoryRoot;
	prtyBoolean		m_bUseSceneFilenameAsDirectory;
	prtyBoolean		m_bUseCameraNameAsDirectory;
	prtyBoolean		m_bUseResolutionAsDirectory;
	// other
	prtyBoolean		m_bShowRenderProgressDialog;
	// padding
	prtyFloat		m_fLeadIn;
	prtyFloat		m_fLeadOut;
	// post
	prtyBoolean		m_bSendPostEmailAddress;		// not saved to file, but is saved
	prtyText		m_PostEmailAddress;				// not saved to file, but is saved
	prtyBoolean		m_bExecutePostCommand;			// not saved to file, but is saved
	prtyText		m_PostCommand;					// not saved to file, but is saved
	// save-resume point
	prtyBoolean		m_bRenderPosUse;				// not saved (except for save render state)
	prtyFloat		m_RenderPosTime;				// not saved (except for save render state)
	prtyFileName	m_RenderPosScene;				// not saved (except for save render state)
	prtyFileName	m_RenderPosCamera;				// not saved (except for save render state)
	prtyFilePath	m_RenderPosSaveFile;			// not saved (except for save render state)
	prtyTrigger		m_tRenderPosDelete;				// not saved (except for save render state)
	// time
	prtyFloat		m_fStartTime;
	prtyFloat		m_fEndTime;
	prtyBoolean		m_bUseMarkerTimes;
	prtyFloat		m_fMarkerInTime;
	prtyFloat		m_fMarkerOutTime;
	// video output
	prtyBoolean		m_bCaptureMovie;
	prtyBoolean		m_bMoviePerDriver;
	prtyText		m_CaptureFormat;				// frame or movie format
	prtyText		m_CompressCode;
	// visual output
	prtyBoolean		m_bDisplayTimeCode;
	prtyBoolean		m_bOutputTitleCard;
	//prtyInt32		m_nSubdivLevel;
	prtyBoolean		m_bSmoothing;
	prtyBoolean		m_bUseLayerVisibility;

	//
	prtyInt32		m_nCaptureMax;					// max # of frames to capture
	prtyBoolean		m_bKeepFrameOpenAfterRender;
	prtyBoolean		m_bBatchMode;					// not saved
	prtyInt32		m_nCurrentFrame;				// not saved
	prtyFileName	m_CurrentScene;					// not saved
	prtyFileName	m_CurrentCamera;				// not saved
	prtyFileName	m_CurrentSceneFilename;			// not saved
	prtyInt32		m_NumberOfScenes;				// not saved
	prtyInt32		m_CurrentSceneNumber;			// not saved
	prtyFilePath	m_RenderOutputDataFile;			// not saved -- data configuration file
	prtyFilePath	m_RenderOutputDataDefaultFile;	// not saved -- default data configuration file
	prtyFileName	m_OutputFileName;				// not saved -- last written output file
	prtyDirectory	m_OutputMovieDirectory;			// not saved -- last written output movie directory

	//	post capture data (not stored)
	prtyListChecked	m_OutputFiles;					// all files rendered out (images + movies)

	std::vector<cptrRenderOutputCamerasData>	m_CameraList;		// not saved
};
