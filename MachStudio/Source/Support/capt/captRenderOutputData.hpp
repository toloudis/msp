/********************************************************************************************\
**	captRenderOutputData.hpp
**
**		RenderOutput data.  This is data relating to rendering of scenes that IS NOT related
**	to render specifics.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef CAPT_RENDEROUTPUTDATA_HPP
#error captRenderOutputData.hpp multiply included
#endif
#define CAPT_RENDEROUTPUTDATA_HPP

#ifndef TMLN_TIMEINOUTDATA_HPP
#include "Support/tmln/tmlnTimeInOutData.hpp"
#endif

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_DIRECTORY_HPP
#include "Core/prty/prtyDirectory.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
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
#ifndef PRTY_TIME_HPP
#include "Core/prty/prtyTime.hpp"
#endif 
#ifndef PRTY_TRIGGER_HPP
#include "Core/prty/prtyTrigger.hpp"
#endif 

#include <list>
#include <string>
#include <vector>


//============================================================================
//============================================================================
class captRenderOutputCameraDriverData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	captRenderOutputCameraDriverData()
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
class captRenderOutputCamerasData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	captRenderOutputCamerasData()
	:	m_CameraName("Camera Name"),
		m_bCapture("Camera Capture Flag")
	{
	};

public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	prtyText		m_CameraName;			// not saved
	prtyBoolean		m_bCapture;				// not saved

	std::list<captRenderOutputCameraDriverData> m_CameraCaptureData;		// not saved
};

//============================================================================
//============================================================================
class captRenderOutputData
{
public:
	captRenderOutputData();

public:
	enum eFilterFunc
	{
		eFilterBox,
		eFilterGaussian,
		eFilterMitchell,
		eFilterTriangle,
		eFilterSinc,
		eFilterLanczos,
		eFilterBlackmanHarris,
		eFilterCatmullRom,
	};
	enum eRenderFrameRange
	{
		eCaptureDrivers = 0,
		eTimeline,
		eSpecifyRange,
		eNumRenderFrameRangeOptions
	};

	//---------------------------------------------------------------------------
	// Constants for initial value
	//---------------------------------------------------------------------------
	static const maTime c_InitRenderPosTime;			// -1 in seconds
	static const maTime c_InitEndTime;					// -1 in seconds

	//---------------------------------------------------------------------------
	//	data
	//---------------------------------------------------------------------------
	// read from chunk flag
	prtyBoolean		m_bChunkData;
	// audio
	prtyFilePath	m_SoundFile;
	// cameras
	prtyBoolean		m_bCaptureAllCameras;
	prtyListChecked	m_Cameras;
	prtyBoolean		m_bUseRenderDriversAsUniqueCameras;
	// dimensions
	prtyText		m_Resolution;					// not saved to file
	prtyEnum		m_PixelAspect;
	prtyInt32		m_nWidth;
	prtyInt32		m_nHeight;
	
	// sampling
	prtyInt32		m_nCaptureSampling;
	prtyBoolean		m_bJitteredSampling;
	prtyFloat		m_FilterWidth;					// jittered only
	prtyEnum		m_FilterFunc;					// jittered only
	prtyBoolean		m_bHardwareAA;

	// Shadows
	prtyEnum		m_ShadowQuality;

	// filename
	prtyText		m_Prefix;
	prtyBoolean		m_bUseSceneFilenameInFilename;
	prtyBoolean		m_bUseCameraNameInFilename;
	prtyBoolean		m_bUseLayerNameInFilename;
	prtyBoolean		m_bUseCompressionInFilename;
	prtyBoolean		m_bUseRenderPassInFilename;
	prtyInt32		m_nCounterDigits;
	prtyFileName	m_OutputFileTags;
	prtyBoolean		m_bOutputFileCustomize;

	// frames
	prtyFloat		m_fCaptureFPS;
	prtyInt32		m_nMotionSamplesPerFrame;

	// location
	prtyDirectory	m_OutputDirectory;				// not saved to file
	prtyDirectory	m_OutputDirectoryRoot;
	prtyBoolean		m_bUseSceneFilenameAsDirectory;
	prtyBoolean		m_bUseCameraNameAsDirectory;
	prtyBoolean		m_bUseLayerNameAsDirectory;
	prtyBoolean		m_bUseRenderPassAsDirectory;
	prtyBoolean		m_bUseResolutionAsDirectory;
	prtyFileName	m_OutputDirectoryTags;
	prtyBoolean		m_bOutputDirectoryCustomize;

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
	//TIME - switched to prtyTime
	prtyTime		m_RenderPosTime;				// not saved (except for save render state)
	prtyFileName	m_RenderPosScene;				// not saved (except for save render state)
	prtyFileName	m_RenderPosCamera;				// not saved (except for save render state)
	prtyFileName	m_RenderPosLayer;				// not saved (except for save render state)
	prtyFilePath	m_RenderPosSaveFile;			// not saved (except for save render state)
	prtyTrigger		m_tRenderPosDelete;				// not saved (except for save render state)
	// time
	prtyEnum		m_RenderFrameRange;				// Replaces m_bUseCaptureDriver with 3-way enum option
	prtyTime		m_fStartTime;					//TIME - switched to prtyTime
	prtyTime		m_fEndTime;						//TIME - switched to prtyTime
	//prtyBoolean		m_bUseCaptureDrivers;
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

	// user definable name tags for passes (common to entire render output)
	prtyFileName	m_BeautyPassName;
	prtyFileName	m_AOOnlyPassName;
	prtyFileName	m_DepthPassName;
	prtyFileName	m_ShadowMaskPassName;
	prtyFileName	m_IlluminationOnlyPassName;
	prtyFileName	m_NormalsPassName;
	prtyFileName	m_DirtyMattePassName;
	prtyFileName	m_WireframePassName;
	prtyFileName	m_MaterialsPassName;
	prtyFileName	m_ReflectionsOnlyPassName;
	prtyFileName	m_VelocityPassName;
	prtyFileName	m_DiffusePassName;
	prtyFileName	m_SpecularPassName;
	prtyFileName	m_BloomPassName;
	prtyFileName	m_StarPassName;
	prtyFileName	m_CameraDOFPassName;
	prtyFileName	m_EmissivePassName;
	prtyFileName	m_SpecEnvPassName;
	prtyFileName	m_SpecLitPassName;
	prtyFileName	m_DiffEnvPassName;
	prtyFileName	m_DiffLitPassName;
	prtyFileName	m_PreviewPassName;
	prtyFileName	m_GIPassName;
	prtyFileName	m_GlowPassName;
	prtyFileName	m_MrayFinalGatherPassName;
	prtyFileName	m_RmanColorBleedPassName;

	//
	prtyInt32		m_nCaptureMax;					// max # of frames to capture
	prtyBoolean		m_bKeepFrameOpenAfterRender;
	prtyBoolean		m_bBatchMode;					// not saved
	prtyBoolean		m_bBatchSkipDialog;				// not saved
	prtyBoolean		m_bCannotOpen;					// not saved
	prtyInt32		m_nCurrentFrame;				// not saved
	prtyFileName	m_CurrentScene;					// not saved
	prtyFileName	m_CurrentCamera;				// not saved
	prtyFileName	m_CurrentLayer;					// not saved
	prtyFileName	m_CurrentSceneFilename;			// not saved
	prtyInt32		m_NumberOfScenes;				// not saved
	prtyInt32		m_CurrentSceneNumber;			// not saved
	prtyFilePath	m_RenderOutputDataFile;			// not saved -- data configuration file
	prtyFilePath	m_RenderOutputDataDefaultFile;	// not saved -- default data configuration file
	prtyFileName	m_OutputFileName;				// not saved -- last written output file
	prtyDirectory	m_OutputMovieDirectory;			// not saved -- last written output movie directory
	prtyFileName	m_CurrentRenderPass;			// not saved
	prtyFileName	m_CurrentRenderPassNoSpaces;	// not saved
	prtyInt8		m_CurrentStereoMode;			// not saved

	//	post capture data (not stored)
	prtyListChecked	m_OutputFiles;					// all files rendered out (images + movies)

	std::vector<captRenderOutputCamerasData>	m_CameraList;		// not saved
};
