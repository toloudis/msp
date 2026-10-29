//****************************************************************************
///	captRenderOutputTagsUtil.hpp
///
///		Interface to RenderOutput Tags
//
//	StudioGPU
//	Copyright(C) 2010 - All Rights Reserved
//****************************************************************************
#include "Support/capt/captRenderOutputTagsUtil.hpp"

#include "Support/capt/captRenderOutputData.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/capt/captStereoUtil.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"

#include <iomanip>
#include <queue>
#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif


//============================================================================
//	The tag structure is of the form:
//	
//	Tag Name		Tag Value
//		.				.
//		.				.
//		.				.
//	
// NOTE: I don't think we need the 2nd column because we can just get the
//	data from the data structure.  This will avoid having duplicate data
//============================================================================
namespace
{
	std::vector<std::vector<std::string>>	l_RenderOutputTags(2, std::vector<std::string>(0));

	struct ParsedNode
	{
		bool bIsTag;
		std::string StringData;
	};
	std::queue<ParsedNode> ParsedData;


//------------------------------------------------------------------------
/// @param i_TagNum
//------------------------------------------------------------------------
std::string get_tag(int i_TagNum)
{
	if ((i_TagNum >= 0) && (i_TagNum < captRenderOutputTags::e_SceneEnvTagsCount))
	{
		return l_RenderOutputTags[0][i_TagNum];
	}
	return std::string();
}

//------------------------------------------------------------------------
/// @param i_Data render output data structure
/// @param i_TagNum index of the tag
//------------------------------------------------------------------------
std::string get_tagstring( captRenderOutputData& i_Data, int i_TagNum )
{
	if ((i_TagNum >= 0) && (i_TagNum < captRenderOutputTags::e_SceneEnvTagsCount))
	{
		switch (i_TagNum)
		{
			case captRenderOutputTags::e_SceneFilename:
				return i_Data.m_CurrentScene.GetValue();
			case captRenderOutputTags::e_CameraName:
				return i_Data.m_CurrentCamera.GetValue();
			case captRenderOutputTags::e_RenderLayerName:
				return i_Data.m_CurrentLayer.GetValue();
			case captRenderOutputTags::e_CompressionType:
			{
				// remove front + back spaces (front won't happen because of "_" before
				std::string::size_type const first = i_Data.m_CompressCode.GetValue().find_first_not_of(" \t");
				std::string temp = i_Data.m_CompressCode.GetValue().substr(first, i_Data.m_CompressCode.GetValue().find_last_not_of(" \t")-first+1).c_str();
				return temp;
			}
			case captRenderOutputTags::e_RenderPassName:
			{
				return i_Data.m_CurrentRenderPassNoSpaces.GetValue();
				// remove front + back spaces (front won't happen because of "_" before
				//std::string::size_type const first = i_Data.m_CompressCode.GetValue().find_first_not_of(" \t");
				//std::string temp = i_Data.m_CompressCode.GetValue().substr(first, i_Data.m_CompressCode.GetValue().find_last_not_of(" \t")-first+1).c_str();
				//o_OutputString += itString(temp.c_str());
				//return i_Data.m_CurrentCamera.GetValue();

				//return itString(L"");
			}
			case captRenderOutputTags::e_Counters:
			{
				std::ostringstream oss;
				oss << std::setw(i_Data.m_nCounterDigits.GetValue());
				oss << i_Data.m_nCurrentFrame.GetValue();
				return oss.str();
			}
			case captRenderOutputTags::e_CountersZeros:
			{
				std::ostringstream oss;
				oss << std::setfill('0') << std::setw(i_Data.m_nCounterDigits.GetValue());
				oss << i_Data.m_nCurrentFrame.GetValue();
				return oss.str();
			}
			case captRenderOutputTags::e_Resolution:
			{
				std::string resolution;
				std::ostringstream oss;
				oss << (i_Data.m_nWidth.GetValue()) << "x" << (i_Data.m_nHeight.GetValue());
				resolution = oss.str().c_str();
				return resolution;
			}
			case captRenderOutputTags::e_ResolutionWidth:
			{
				std::string resolution;
				std::ostringstream oss;
				oss << i_Data.m_nWidth.GetValue();
				resolution = oss.str().c_str();
				return resolution;
			}
			case captRenderOutputTags::e_ResolutionHeight:
			{
				std::string resolution;
				std::ostringstream oss;
				//oss.setf(0, std::ios::floatfield);
				//oss.setf(std::ios::fixed, std::ios::floatfield);
				oss << i_Data.m_nHeight.GetValue();
				std::string res(oss.str());
				resolution = res.c_str();
				return resolution;
			}
			case captRenderOutputTags::e_OutputBaseDir:
			{
				fsLocator dir = i_Data.m_OutputDirectoryRoot.GetValue();
				std::string path;
				fsFileUtil::LocatorToUnicodeString( dir, path );
				return path;
			}
			case captRenderOutputTags::e_CaptureUnique:
			{
				//	need camera "letter"
				std::string resolution;
				return resolution;
			}
			case captRenderOutputTags::e_FPS:
			{
				std::string value;
				std::ostringstream oss;
				oss.setf(0, std::ios::floatfield);
				oss.setf(std::ios::fixed, std::ios::floatfield);
				oss << i_Data.m_fCaptureFPS.GetValue();
				std::string str(oss.str());
				value = str.c_str();
				return value;
			}
			case captRenderOutputTags::e_Time:
			{
				std::string value;
				std::string timestr;
				tmlnTimeUtil::GetTimeStringInHMSM( tmlnTimeLine::GetValue(), timestr );
				value = timestr.c_str();
				return value;
			}
			case captRenderOutputTags::e_Minutes:
			{
				std::string value;
				std::string timestr;
				int Hours, Minutes, Seconds;
				float Milliseconds;
				tmlnTimeUtil::GetTimeInHMSM( tmlnTimeLine::GetValue(), Hours, Minutes, Seconds, Milliseconds);
				std::ostringstream oss;
				oss.setf(0, std::ios::floatfield);
				oss.setf(std::ios::fixed, std::ios::floatfield);
				oss << Minutes;
				std::string str(oss.str());
				value = str.c_str();
				return value;
			}
			case captRenderOutputTags::e_Seconds:
			{
				std::string value;
				std::string timestr;
				int Hours, Minutes, Seconds;
				float Milliseconds;
				tmlnTimeUtil::GetTimeInHMSM( tmlnTimeLine::GetValue(), Hours, Minutes, Seconds, Milliseconds);
				std::ostringstream oss;
				oss.setf(0, std::ios::floatfield);
				oss.setf(std::ios::fixed, std::ios::floatfield);
				oss << Seconds;
				std::string str(oss.str());
				value = str.c_str();
				return value;
			}
			case captRenderOutputTags::e_Milliseconds:
			{
				std::string value;
				std::string timestr;
				int Hours, Minutes, Seconds;
				float Milliseconds;
				tmlnTimeUtil::GetTimeInHMSM( tmlnTimeLine::GetValue(), Hours, Minutes, Seconds, Milliseconds);
				std::ostringstream oss;
				oss.setf(0, std::ios::floatfield);
				oss.setf(std::ios::fixed, std::ios::floatfield);
				oss << Milliseconds;
				std::string str(oss.str());
				value = str.c_str();
				return value;
			}
			case captRenderOutputTags::e_Frame:
			{
				std::string value;
				std::ostringstream oss;
				oss.setf(0, std::ios::floatfield);
				oss.setf(std::ios::fixed, std::ios::floatfield);
				oss << i_Data.m_nCurrentFrame.GetValue();
				std::string str(oss.str());
				value = str.c_str();
				return value;
			}
			case captRenderOutputTags::e_RenderType:
			{
				std::string value;
				/// \todo fill in this value for RenderType
				return value;
			}
			case captRenderOutputTags::e_StereoEye:
			{
				std::string value;
				//	Stereo
				if (i_Data.m_CurrentStereoMode.GetValue() == STEREO_LEFTCAM )
				{
					value = std::string("Left");
				}
				else if (i_Data.m_CurrentStereoMode.GetValue() == STEREO_RIGHTCAM )
				{
					value = std::string("Right");
				}
				return value;
			}
			default:
				return std::string();
		}
	}
	return std::string();
}

}


//------------------------------------------------------------------------
//	Init
//------------------------------------------------------------------------
void captRenderOutputTagsUtil::Init()
{
	SetToDefault();
}

//------------------------------------------------------------------------
//	DeInitialize
//------------------------------------------------------------------------
void captRenderOutputTagsUtil::DeInitialize()
{
}

//------------------------------------------------------------------------
// When a new scene is loaded, set all the tags to default
//------------------------------------------------------------------------
void captRenderOutputTagsUtil::SetToDefault()
{
	l_RenderOutputTags[0].resize( captRenderOutputTags::e_SceneEnvTagsCount );
	l_RenderOutputTags[1].resize( captRenderOutputTags::e_SceneEnvTagsCount );
	l_RenderOutputTags[0][captRenderOutputTags::e_SceneFilename] = std::string("SceneFilename");
	l_RenderOutputTags[1][captRenderOutputTags::e_SceneFilename] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_CameraName] = std::string("CameraName");
	l_RenderOutputTags[1][captRenderOutputTags::e_CameraName] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_RenderLayerName] = std::string("RenderLayerName");
	l_RenderOutputTags[1][captRenderOutputTags::e_RenderLayerName] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_CompressionType] = std::string("CompressionType");
	l_RenderOutputTags[1][captRenderOutputTags::e_CompressionType] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_RenderPassName] = std::string("RenderPassName");
	l_RenderOutputTags[1][captRenderOutputTags::e_RenderPassName] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_Counters] = std::string("#");
	l_RenderOutputTags[1][captRenderOutputTags::e_Counters] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_CountersZeros] = std::string("0");
	l_RenderOutputTags[1][captRenderOutputTags::e_CountersZeros] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_OutputBaseDir] = std::string("OutputBaseDir");
	l_RenderOutputTags[1][captRenderOutputTags::e_OutputBaseDir] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_CaptureUnique] = std::string("CaptureUnique");
	l_RenderOutputTags[1][captRenderOutputTags::e_CaptureUnique] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_Resolution] = std::string("Resolution");
	l_RenderOutputTags[1][captRenderOutputTags::e_Resolution] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_ResolutionWidth] = std::string("ResolutionWidth");
	l_RenderOutputTags[1][captRenderOutputTags::e_ResolutionWidth] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_ResolutionHeight] = std::string("ResolutionHeight");
	l_RenderOutputTags[1][captRenderOutputTags::e_ResolutionHeight] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_FPS] = std::string("FPS");
	l_RenderOutputTags[1][captRenderOutputTags::e_FPS] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_Time] = std::string("Time");
	l_RenderOutputTags[1][captRenderOutputTags::e_Time] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_Minutes] = std::string("Minutes");
	l_RenderOutputTags[1][captRenderOutputTags::e_Minutes] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_Seconds] = std::string("Seconds");
	l_RenderOutputTags[1][captRenderOutputTags::e_Seconds] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_Milliseconds] = std::string("Milliseconds");
	l_RenderOutputTags[1][captRenderOutputTags::e_Milliseconds] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_Frame] = std::string("Frame");
	l_RenderOutputTags[1][captRenderOutputTags::e_Frame] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_RenderType] = std::string("RenderType");
	l_RenderOutputTags[1][captRenderOutputTags::e_RenderType] = std::string();
	l_RenderOutputTags[0][captRenderOutputTags::e_StereoEye] = std::string("StereoEye");
	l_RenderOutputTags[1][captRenderOutputTags::e_StereoEye] = std::string();
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
int captRenderOutputTagsUtil::GetNumTags()
{
	//return captRenderOutputTags::e_SceneEnvTagsCount;
	return l_RenderOutputTags[1].size();
}

//------------------------------------------------------------------------
/// @param i_TagNum index of the tag
/// @param i_Tag string to set at the tag index
//------------------------------------------------------------------------
void captRenderOutputTagsUtil::SetTag(int i_TagNum, std::string& i_Tag)
{
	if ((i_TagNum >= 0) && (i_TagNum < captRenderOutputTags::e_SceneEnvTagsCount))
	{
		l_RenderOutputTags[1][i_TagNum] = i_Tag;
	}
}

//------------------------------------------------------------------------
/// @param i_TagNum index of the tag
/// @return the tag string associated with this index
//------------------------------------------------------------------------
std::string captRenderOutputTagsUtil::GetTag(int i_TagNum)
{
	if ((i_TagNum >= 0) && (i_TagNum < captRenderOutputTags::e_SceneEnvTagsCount))
	{
		return l_RenderOutputTags[0][i_TagNum];
	}
	return std::string();
}

//------------------------------------------------------------------------
///	Generate the final text output for the filename
///
/// @param i_Data render output data structure
/// @param o_OutputString the generated file name based on the tags
//------------------------------------------------------------------------
void captRenderOutputTagsUtil::GenerateOutputFilename(captRenderOutputData& i_Data, std::string& o_OutputString, bool i_bCapturingRenderman /*false*/)
{
	if (i_Data.m_bOutputFileCustomize.GetValue())
	{
		o_OutputString.clear();

		//	break apart the string into tags and non-tags
		bool bTagParse = false;
		int cindex = 0;
		char ch;
		std::string accum;
		std::string tags( i_Data.m_OutputFileTags.GetValue() );

		//DBG_LOG( "Tagged String = " << tags );

		for (int i=0; i < tags.size(); ++i)
		{
			ch = tags[i];
			if (ch == '<')
			{
				if (accum.size() > 0)
				{
					ParsedNode node;
					node.bIsTag = false;
					node.StringData = accum;
					ParsedData.push(node);
					//DBG_LOG("\t\tnon-tag = " << accum);
					accum.clear();
				}
				bTagParse = true;
			}
			else if (ch == '>')
			{
				bTagParse = false;
				if (accum.size() > 0)
				{
					ParsedNode node;
					node.bIsTag = true;
					node.StringData = accum;
					ParsedData.push(node);
					//DBG_LOG("\t\ttag = " << accum);
					accum.clear();
				}
			}
			else
			{
				accum += ch;
			}
		}

		//	if there are characters left over, add them as a non-tag
		if (accum.size() > 0)
		{
			ParsedNode node;
			node.bIsTag = false;
			node.StringData = accum;
			ParsedData.push(node);
			//DBG_LOG("\t\tnon-tag = " << accum);
			accum.clear();
		}

		//	go through the list and build the text string
		while (!ParsedData.empty())
		{
			std::string nodestr = ParsedData.front().StringData;

			if (ParsedData.front().bIsTag)
			{
				if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_SceneFilename] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_SceneFilename);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_CameraName] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CameraName);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_RenderLayerName] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_RenderLayerName);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_CompressionType] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CompressionType);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_RenderPassName] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_RenderPassName);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Counters] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Counters);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_CountersZeros] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CountersZeros);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_OutputBaseDir] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_OutputBaseDir);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_CaptureUnique] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CaptureUnique);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_ResolutionWidth] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_ResolutionWidth);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_ResolutionHeight] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_ResolutionHeight);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Resolution] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Resolution);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_FPS] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_FPS);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Time] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Time);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Minutes] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Minutes);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Seconds] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Seconds);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Milliseconds] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Milliseconds);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Frame] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Frame);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_RenderType] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_RenderType);
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_StereoEye] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_StereoEye);
				}
			}
			else
			{
				o_OutputString += nodestr;
			}
			ParsedData.pop();
		}
	}
	else
	{
		//	parse the input string, check tags, create output string
		o_OutputString.clear();

		//	Scene
		if ( i_Data.m_bUseSceneFilenameInFilename.GetValue() )
		{
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_SceneFilename);
		}

		//	camera
		if ( i_Data.m_bUseCameraNameInFilename.GetValue() )
		{
			o_OutputString += std::string("_");
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CameraName);
		}

		//	render layer
		if ( i_Data.m_bUseLayerNameInFilename.GetValue() )
		{
			o_OutputString += std::string("_");
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_RenderLayerName);
		}

		//	Render Pass
		if ( i_Data.m_bUseRenderPassInFilename.GetValue())
		{
			o_OutputString += std::string("_");
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_RenderPassName);
		}

		//	compression
		if ( i_Data.m_bUseCompressionInFilename.GetValue() && i_Data.m_bCaptureMovie.GetValue())
		{
			o_OutputString += std::string("_");
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CompressionType);
		}

		//	Stereo
		if (   (i_Data.m_CurrentStereoMode.GetValue() == STEREO_LEFTCAM )
			|| (i_Data.m_CurrentStereoMode.GetValue() == STEREO_RIGHTCAM ))
		{
			o_OutputString += std::string("_");
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_StereoEye);
		}

		//	digits
		if ( !i_Data.m_bCaptureMovie.GetValue() || i_bCapturingRenderman )
		{
			o_OutputString += std::string("_");
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CountersZeros);
		}
	}

	//DBG_LOG("Built Filename = " << o_OutputString);
}

//------------------------------------------------------------------------
///	Generate the final text output for the directory
///
/// @param i_Data render output data structure
//------------------------------------------------------------------------
void captRenderOutputTagsUtil::GenerateOutputDirectory(captRenderOutputData& i_Data)
{
	if (i_Data.m_bOutputDirectoryCustomize.GetValue())
	{
		fsLocator dir;

		//	break apart the string into tags and non-tags
		bool bTagParse = false;
		int cindex = 0;
		char ch;
		std::string accum;
		std::string tags( i_Data.m_OutputDirectoryTags.GetValue() );

		//DBG_LOG( "Tagged String = " << tags );

		for (int i=0; i < tags.size(); ++i)
		{
			ch = tags[i];
			if (ch == '<')
			{
				if (accum.size() > 0)
				{
					ParsedNode node;
					node.bIsTag = false;
					node.StringData = accum;
					ParsedData.push(node);
					//DBG_LOG("\t\tnon-tag = " << accum);
					accum.clear();
				}
				bTagParse = true;
			}
			else if (ch == '>')
			{
				bTagParse = false;
				if (accum.size() > 0)
				{
					ParsedNode node;
					node.bIsTag = true;
					node.StringData = accum;
					ParsedData.push(node);
					//DBG_LOG("\t\ttag = " << accum);
					accum.clear();
				}
			}
			else if (ch == '\\')
			{
				//	add what has accumulated before the slash
				if (accum.size() > 0)
				{
					ParsedNode node;
					node.bIsTag = false;
					node.StringData = accum;
					ParsedData.push(node);
					//DBG_LOG("\t\tnon-tag = " << accum);
					accum.clear();
				}

				//	if last node was a backslash, then this is part
				//	of a UNC path most likely, so add to the slash node
				if (   (ParsedData.size() > 0)
					&& (ParsedData.back().StringData == "\\"))
				{
					ParsedData.back().StringData += ch;
				}
				else
				{
					// now add the slash
					accum += ch;
					ParsedNode node;
					node.bIsTag = false;
					node.StringData = accum;
					ParsedData.push(node);
					//DBG_LOG("\t\tnon-tag = " << accum);
					accum.clear();
				}
			}
			else
			{
				accum += ch;
			}
		}

		//	if there are left over chars, add them.
		if (accum.size() > 0)
		{
			ParsedNode node;
			node.bIsTag = false;
			node.StringData = accum;
			ParsedData.push(node);
			//DBG_LOG("\t\tnon-tag = " << accum);
			accum.clear();
		}

		//	go through the list and build the text string
		std::string singledir;
		accum.clear();
		while (!ParsedData.empty())
		{
			std::string nodestr = ParsedData.front().StringData;
			
			//DBG_LOG("Node -> " << nodestr );

			if (ParsedData.front().bIsTag)
			{
				if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_SceneFilename] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_SceneFilename) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_CameraName] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_CameraName) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_RenderLayerName] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_RenderLayerName) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_CompressionType] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_CompressionType) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_RenderPassName] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_RenderPassName) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Counters] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Counters) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_CountersZeros] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_CountersZeros) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_OutputBaseDir] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_OutputBaseDir) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_CaptureUnique] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_CaptureUnique) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_ResolutionWidth] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_ResolutionWidth) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_ResolutionHeight] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_ResolutionHeight) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Resolution] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Resolution) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_FPS] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_FPS) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Time] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Time) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Minutes] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Minutes) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Seconds] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Seconds) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Milliseconds] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Milliseconds) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_Frame] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Frame) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_RenderType] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_RenderType) );
				}
				else if (envString::StartsWith(nodestr,  l_RenderOutputTags[0][captRenderOutputTags::e_StereoEye] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_StereoEye) );
				}
			}
			else
			{
				if (nodestr == "\\")
				{
					//	ignore nodes that are just the "\"
					fsLocator subdir;
					fsFileUtil::UnicodeStringToLocator( singledir, subdir );
					dir.Push( subdir );
					//DBG_LOG("\t\t->dir node = " << singledir);
					singledir.clear();
				}
				//else if (nodestr.StartsWith("\\")
				else
				{
					//UNC?????
					if (envString::StartsWith(nodestr, std::string("\\\\")))
					{
						dir.SetUNCPath(true);
					}
					else if (envString::StartsWith(nodestr, std::string("\\")))
					{
						nodestr.erase(0, 1);
					}

					if (envString::EndsWith(nodestr, std::string("\\")))
					{
						nodestr.pop_back();
						singledir += nodestr;
						fsLocator subdir;
						fsFileUtil::UnicodeStringToLocator( singledir, subdir );
						dir.Push( subdir );
						//DBG_LOG("\t\t->dir node - " << singledir);
						singledir.clear();
					}
					else
						singledir += nodestr;
				}
			}
			ParsedData.pop();
		}

		//	push anything left over on the directory
		if (singledir.size() > 0)
		{
			fsLocator subdir;
			fsFileUtil::UnicodeStringToLocator( singledir, subdir );
			dir.Push( subdir );
			//DBG_LOG("\t\t->dir node - " << singledir);
		}

		//DBG_LOG("Final Dir = " << dir);
		i_Data.m_OutputDirectory.SetValue(dir);
	}
	else
	{
		//	parse the input string, check tags, create output string
		i_Data.m_OutputDirectory.SetValue( i_Data.m_OutputDirectoryRoot.GetValue() );
		fsLocator dir = i_Data.m_OutputDirectory.GetValue();

		if ( i_Data.m_bUseSceneFilenameAsDirectory.GetValue() )
		{
			dir.Push( get_tagstring(i_Data, captRenderOutputTags::e_SceneFilename) );
		}

		//	is the output movie or image?
		if ( i_Data.m_bCaptureMovie.GetValue() )
		{
			//	output is a movie, so determine whether there is a movie
			//	per driver or all in one.  If seperate, set the camera name
			//
			if (i_Data.m_bMoviePerDriver.GetValue())
			{
				//set the current render layer as a directory for the output
				if (i_Data.m_bUseLayerNameAsDirectory.GetValue())
				{
					dir.Push( get_tagstring(i_Data, captRenderOutputTags::e_RenderLayerName) );

					captRenderOutputDataUtil::SetCurrentLayer( get_tagstring(i_Data, captRenderOutputTags::e_RenderLayerName) );
				}

				//set the current render layer as a directory for the output
				if (i_Data.m_bUseRenderPassAsDirectory.GetValue())
				{
					dir.Push( get_tagstring(i_Data, captRenderOutputTags::e_RenderPassName) );

					captRenderOutputDataUtil::SetCurrentRenderPass( get_tagstring(i_Data, captRenderOutputTags::e_RenderPassName) );
				}
			}
		}
		else
		{
			//
			if ( i_Data.m_bUseCameraNameAsDirectory.GetValue() )
			{
				dir.Push( get_tagstring(i_Data, captRenderOutputTags::e_CameraName) );
			}

			//set the current render layer as a directory for the output
			if (i_Data.m_bUseLayerNameAsDirectory.GetValue())
			{
				dir.Push( get_tagstring(i_Data, captRenderOutputTags::e_RenderLayerName) );

				captRenderOutputDataUtil::SetCurrentLayer( get_tagstring(i_Data, captRenderOutputTags::e_RenderLayerName) );
			}

			//set the current render layer as a directory for the output
			if (i_Data.m_bUseRenderPassAsDirectory.GetValue())
			{
				dir.Push( get_tagstring(i_Data, captRenderOutputTags::e_RenderPassName) );

				captRenderOutputDataUtil::SetCurrentRenderPass( get_tagstring(i_Data, captRenderOutputTags::e_RenderPassName) );
			}

			//
			if (i_Data.m_bUseResolutionAsDirectory.GetValue())
			{
				dir.Push( get_tagstring(i_Data, captRenderOutputTags::e_Resolution) );
			}
		}

		i_Data.m_OutputDirectory.SetValue(dir);
	}

	//DBG_LOG("OUTPUT DIR = " << i_Data.m_OutputDirectory.GetValue());
}

//------------------------------------------------------------------------
///	Build tags for file and directory
///
/// @param io_Data render output data structure
//------------------------------------------------------------------------
void captRenderOutputTagsUtil::BuildTags(captRenderOutputData& io_Data)
{
	BuildTagsForFile( io_Data );
	BuildTagsForDirectory( io_Data );
}

//------------------------------------------------------------------------
/// @param io_Data render output data structure
//------------------------------------------------------------------------
void captRenderOutputTagsUtil::BuildTagsForFile(captRenderOutputData& io_Data)
{
	//	don't update the tag field if the user customized it.
	if (io_Data.m_bOutputFileCustomize == true)
		return;

	std::string FilenameTags;

	if (io_Data.m_bUseSceneFilenameInFilename == true)
	{
		FilenameTags += std::string("<");
		FilenameTags += l_RenderOutputTags[0][captRenderOutputTags::e_SceneFilename];
		FilenameTags += std::string(">");
	}
	if (io_Data.m_bUseCameraNameInFilename == true)
	{
		if (FilenameTags.size() > 0)
			FilenameTags += std::string("_");
		FilenameTags += std::string("<");
		FilenameTags += l_RenderOutputTags[0][captRenderOutputTags::e_CameraName];
		FilenameTags += std::string(">");
	}
	if (io_Data.m_bUseLayerNameInFilename == true)
	{
		if (FilenameTags.size() > 0)
			FilenameTags += std::string("_");
		FilenameTags += std::string("<");
		FilenameTags += l_RenderOutputTags[0][captRenderOutputTags::e_RenderLayerName];
		FilenameTags += std::string(">");
	}
	if (io_Data.m_bUseCompressionInFilename == true)
	{
		if (FilenameTags.size() > 0)
			FilenameTags += std::string("_");
		FilenameTags += std::string("<");
		FilenameTags += l_RenderOutputTags[0][captRenderOutputTags::e_CompressionType];
		FilenameTags += std::string(">");
	}
	if (io_Data.m_bUseRenderPassInFilename == true)
	{
		if (FilenameTags.size() > 0)
			FilenameTags += std::string("_");
		FilenameTags += std::string("<");
		FilenameTags += l_RenderOutputTags[0][captRenderOutputTags::e_RenderPassName];
		FilenameTags += std::string(">");
	}
	//if (using stereo)
	{
		if (FilenameTags.size() > 0)
			FilenameTags += std::string("_");
		FilenameTags += std::string("<");
		FilenameTags += l_RenderOutputTags[0][captRenderOutputTags::e_StereoEye];
		FilenameTags += std::string(">");
	}

	if (io_Data.m_nCounterDigits > 0)
	{
		if (FilenameTags.size() > 0)
			FilenameTags += std::string("_");
		FilenameTags += std::string("<");
		for (int i=0; i < io_Data.m_nCounterDigits.GetValue(); ++i)
			FilenameTags += std::string("0");
		FilenameTags += std::string(">");
	}

	io_Data.m_OutputFileTags.SetValue( FilenameTags );
}


//------------------------------------------------------------------------
/// @param io_Data render output data structure
//------------------------------------------------------------------------
void captRenderOutputTagsUtil::BuildTagsForDirectory(captRenderOutputData& io_Data)
{
	//	don't update the tag field if the user customized it.
	if (io_Data.m_bOutputDirectoryCustomize == true)
		return;

	std::string DirectoryTags;

	if (io_Data.m_OutputDirectoryRoot.GetValue().GetNumNames() > 0)
	{
		//fsFileUtil::LocatorToUnicodeString( io_Data.m_OutputDirectoryRoot.GetValue(), DirectoryTags );
		DirectoryTags += std::string("<");
		DirectoryTags += l_RenderOutputTags[0][captRenderOutputTags::e_OutputBaseDir];
		DirectoryTags += std::string(">");
	}
	if (io_Data.m_bUseSceneFilenameAsDirectory == true)
	{
		if (DirectoryTags.size() > 0)
			DirectoryTags += std::string("\\");
		DirectoryTags += std::string("<");
		DirectoryTags += l_RenderOutputTags[0][captRenderOutputTags::e_SceneFilename];
		DirectoryTags += std::string(">");
	}
	if (io_Data.m_bUseCameraNameAsDirectory == true)
	{
		if (DirectoryTags.size() > 0)
			DirectoryTags += std::string("\\");
		DirectoryTags += std::string("<");
		DirectoryTags += l_RenderOutputTags[0][captRenderOutputTags::e_CameraName];
		DirectoryTags += std::string(">");
	}
	if (io_Data.m_bUseLayerNameAsDirectory == true)
	{
		if (DirectoryTags.size() > 0)
			DirectoryTags += std::string("\\");
		DirectoryTags += std::string("<");
		DirectoryTags += l_RenderOutputTags[0][captRenderOutputTags::e_RenderLayerName];
		DirectoryTags += std::string(">");
	}
	if (io_Data.m_bUseRenderPassAsDirectory == true)
	{
		if (DirectoryTags.size() > 0)
			DirectoryTags += std::string("\\");
		DirectoryTags += std::string("<");
		DirectoryTags += l_RenderOutputTags[0][captRenderOutputTags::e_RenderPassName];
		DirectoryTags += std::string(">");
	}
	if (io_Data.m_bUseResolutionAsDirectory == true)
	{
		if (DirectoryTags.size() > 0)
			DirectoryTags += std::string("\\");
		DirectoryTags += std::string("<");
		DirectoryTags += l_RenderOutputTags[0][captRenderOutputTags::e_Resolution];
		DirectoryTags += std::string(">");
	}

	io_Data.m_OutputDirectoryTags.SetValue( DirectoryTags );
}

