//****************************************************************************
///	captRenderOutputTagsUtil.hpp
///
///		Interface to RenderOutput Tags
//
//	StudioGPU
//	Copyright(C) 2010 - All Rights Reserved
//****************************************************************************
#ifdef CAPT_RENDEROUTPUTTAGSUTIL_HPP
#error captRenderOutputTagsUtil.hpp multiply included
#endif
#define CAPT_RENDEROUTPUTTAGSUTIL_HPP


//============================================================================
//	Forward References
//============================================================================
class itString;
class captRenderOutputData;


//============================================================================
//============================================================================
namespace captRenderOutputTags
{
	enum SceneEnvTags
	{
		e_SceneFilename = 0,
		e_CameraName,
		e_RenderLayerName,
		e_CompressionType,
		e_RenderPassName,
		e_Counters,
		e_CountersZeros,
		e_OutputBaseDir,
		e_CaptureUnique,
		e_ResolutionWidth,
		e_ResolutionHeight,
		e_Resolution,
		e_FPS,
		e_Time,
		e_Minutes,
		e_Seconds,
		e_Milliseconds,
		e_Frame,
		e_RenderType,
		e_StereoEye,
		e_SceneEnvTagsCount
	};
}


//============================================================================
//============================================================================
namespace captRenderOutputTagsUtil
{
	//------------------------------------------------------------------------
	//	Init
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	//	DeInitialize
	//------------------------------------------------------------------------
	void DeInitialize();

	//------------------------------------------------------------------------
	// When a new scene is loaded, set all the tags to default
	//------------------------------------------------------------------------
	void SetToDefault();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int GetNumTags();

	//------------------------------------------------------------------------
	/// @param i_TagNum index of the tag
	/// @param i_Tag string to set at the tag index
	//------------------------------------------------------------------------
	void SetTag(int i_TagNum, itString& i_Tag);

	//------------------------------------------------------------------------
	/// @param i_TagNum index of the tag
	/// @return the tag string associated with this index
	//------------------------------------------------------------------------
	itString GetTag(int i_TagNum);

	//------------------------------------------------------------------------
	///	Generate the final text output for the filename
	///
	/// @param i_Data render output data structure
	/// @param o_OutputString the generated file name based on the tags
	//------------------------------------------------------------------------
	void GenerateOutputFilename(captRenderOutputData& i_Data, itString& o_OutputString, bool i_bCapturingRenderman = false);

	//------------------------------------------------------------------------
	///	Generate the final text output for the directory
	///
	/// @param i_Data render output data structure
	//------------------------------------------------------------------------
	void GenerateOutputDirectory(captRenderOutputData& i_Data);

	//------------------------------------------------------------------------
	///	Build tags for file and directory
	///
	/// @param io_Data render output data structure
	//------------------------------------------------------------------------
	void BuildTags(captRenderOutputData& io_Data);

	//------------------------------------------------------------------------
	/// @param io_Data render output data structure
	//------------------------------------------------------------------------
	void BuildTagsForFile(captRenderOutputData& io_Data);

	//------------------------------------------------------------------------
	/// @param io_Data render output data structure
	//------------------------------------------------------------------------
	void BuildTagsForDirectory(captRenderOutputData& io_Data);
}

