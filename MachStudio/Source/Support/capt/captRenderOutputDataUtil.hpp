/*****************************************************************************
**	captRenderOutputDataUtil.hpp
**
**		Interface to RenderOutput data
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CAPT_RENDEROUTPUTDATAUTIL_HPP
#error captRenderOutputDataUtil.hpp multiply included
#endif
#define CAPT_RENDEROUTPUTDATAUTIL_HPP

#ifndef CAPT_RENDEROUTPUTDATA_HPP
#include "Support/Capt/captRenderOutputData.hpp"
#endif

#ifndef RLYR_PASSESOBJECT_HPP
#include "Support/rlyr/rlyrPassesObject.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================
class itString;
class nameString;
class prtyObject;


//============================================================================
//============================================================================
namespace captRenderOutputDataUtil
{
	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	captRenderOutputData& Data();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetData(captRenderOutputData& i_Data);

	//------------------------------------------------------------------------
	// Set format and compress code for the render
	//------------------------------------------------------------------------
	void SetCaptureFormat(const itString& i_CapFormat);
	void SetCompressCode(const itString& i_CompressCode);

	//------------------------------------------------------------------------
	// Set the given camera as an active camera
	//------------------------------------------------------------------------
	void SetActiveCamera(const itString& i_Camera);

	//------------------------------------------------------------------------
	// Set the render resolution width and height
	//------------------------------------------------------------------------
	void SetCaptureWidth(int i_Width);
	void SetCaptureHeight(int i_Height);
	
	//------------------------------------------------------------------------
	// Set the render output file and path
	//------------------------------------------------------------------------
	void SetCaptureOutputFile(itString& i_OutputFile);
	void SetCaptureOutputDirectory(itString& i_OutputPath);
	void SetCaptureOutputDirectoryRoot(itString& i_OutputPath);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetLayerData(captRenderOutputData& i_LayerData);

	//--------------------------------------------------------------------
	//	Write to a config file
	//--------------------------------------------------------------------
	void WriteData(const captRenderOutputData& i_Data);
	void WriteData(const fsLocator& i_ConfigFile, const captRenderOutputData& i_Data);

	//------------------------------------------------------------------------
	//	Read from a config file
	//------------------------------------------------------------------------
	void ReadData(captRenderOutputData& o_Data);
	void ReadData(const fsLocator& i_ConfigFile, captRenderOutputData& o_Data);

	//------------------------------------------------------------------------
	//	called right before showing the dialog
	//------------------------------------------------------------------------
	void SetupControls( bool i_bBatchMode );

	//------------------------------------------------------------------------
	//	set the capture data to default settings
	//------------------------------------------------------------------------
	void SetToDefault(fsLocator& i_CurrentScene);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void BuildCameraList(captRenderOutputData& o_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateCameraList(captRenderOutputData& i_NewData);
	void UpdateData(captRenderOutputData& i_NewData);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentScene( itString& i_SceneName );
	void SetCurrentScene( nameString& i_SceneName );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentCamera( itString& i_CamName );
	void SetCurrentCamera( nameString& i_CamName );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentLayer( itString& i_LayerName );
	void SetCurrentLayer( nameString& i_LayerName );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentRenderPass( itString& i_RenderPass );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentSceneFilename( itString& i_SceneFilename );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetDirectory( fsLocator& i_Dir );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetCurrentFrame( int i_FrameNumber );

	//------------------------------------------------------------------------
	//	Generate the filename based on the current data.
	//
	//	Note: if the "use scene" or "use camera" flags are set, it is up
	//	to the code to set the current scene and/or current camera before
	//	generating the name.
	//
	//	Note: if the output is frames then the counter should be set as well.
	//
	//	the format will be "scenename_cameraname_counter" or a variation of
	//	this depending on the flags set.
	//------------------------------------------------------------------------
	void GenerateFilename( itString& o_Filename, bool i_bCapturingRenderman = false );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void GenerateDirectoryName();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetPrefix( itString& i_Prefix );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetMaxTime( int i_nMaxTime );

	//------------------------------------------------------------------------
	//	Turn On/off the shadows for capture.  This will also store what the
	//	current shadow state is.	Restore will put the shadow back to it's
	//	state before Enable was called.
	//------------------------------------------------------------------------
	void EnableShadows( bool i_bOn );
	void RestoreShadows();

	//--------------------------------------------------------------------
	//	EnableWireframe - enable/disable wireframe render
	//--------------------------------------------------------------------
	void EnableWireframe( bool i_bRenderWireframe );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject();

	//------------------------------------------------------------------------
	//	output the struct values at anytime
	//------------------------------------------------------------------------
	void Debug();

	
	//------------------------------------------------------------------------
	// When a new scene is loaded, set all the capture options to default
	//------------------------------------------------------------------------
	void SetToDefault();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	itString GetRenderPassName(rlyrPassesObject::ePassType i_PassType);

}	// end of namespace
