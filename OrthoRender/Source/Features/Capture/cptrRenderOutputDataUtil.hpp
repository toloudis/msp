/*****************************************************************************
**	cptrRenderOutputDataUtil.hpp
**
**		Interface to RenderOutput data
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDEROUTPUTDATAUTIL_HPP
#error cptrRenderOutputDataUtil.hpp multiply included
#endif
#define CPTR_RENDEROUTPUTDATAUTIL_HPP

#ifndef CPTR_RENDEROUTPUTDATA_HPP
#include "Features/Capture/cptrRenderOutputData.hpp"
#endif

#ifndef LIGHTWAIT_RESPONSE_HPP
#include "MainApp/LightWaitResponse.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class itString;
class nameString;
class prtyObject;


//============================================================================
//============================================================================
namespace cptrRenderOutputDataUtil
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CleanUp();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void WriteData(const cptrRenderOutputData& i_Data);
	void WriteData(const fsLocator& i_ConfigFile, const cptrRenderOutputData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadData(cptrRenderOutputData& o_Data);
	void ReadData(const fsLocator& i_ConfigFile, cptrRenderOutputData& o_Data);

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
	void BuildCameraList(cptrRenderOutputData& o_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AppendXMLDescription(LightWaitResponse *response) ;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateCameraList(cptrRenderOutputData& i_NewData);
	void UpdateData(cptrRenderOutputData& i_NewData);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cptrRenderOutputData& Data();

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
	void SetCurrentSceneFilename( itString& i_SceneFilename );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetSceneAndDirectory( fsLocator& i_DirAndFilename );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetDirectory( fsLocator& i_Dir );

	//--------------------------------------------------------------------
	//	Sets the (final) width and height of the renderer
	//--------------------------------------------------------------------
	void SetWidth(int width);
	void SetHeight(int height);

	//--------------------------------------------------------------------
	//	Sets whether we are doing progressively larger renders
	//--------------------------------------------------------------------
	void SetProgressive(bool progressive) ;

	//--------------------------------------------------------------------
	//	Sets how many progressive steps remain
	//--------------------------------------------------------------------
	void SetProgressiveCount(int progressiveStepCount) ;

	//--------------------------------------------------------------------
	//	Sets the (starting progressive) width and height of the renderer
	//--------------------------------------------------------------------
	void SetProgressiveStartWidth(int width);
	void SetProgressiveStartHeight(int height);

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
	void GenerateFilename( std::string& o_Filename );

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
	//	SetOneCameraToRender() - set a single camera to render
	//--------------------------------------------------------------------
	void SetOneCameraToRender(const std::string& i_CamName);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject();

	//------------------------------------------------------------------------
	//	output the struct values at anytime
	//------------------------------------------------------------------------
	void Debug();

}	// end of namespace
