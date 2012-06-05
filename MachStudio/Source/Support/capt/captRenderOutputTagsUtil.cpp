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
	std::vector<std::vector<itString>>	l_RenderOutputTags(2, std::vector<itString>(0));

	struct ParsedNode
	{
		bool bIsTag;
		itString StringData;
	};
	std::queue<ParsedNode> ParsedData;


//------------------------------------------------------------------------
/// @param i_TagNum
//------------------------------------------------------------------------
itString get_tag(int i_TagNum)
{
	if ((i_TagNum >= 0) && (i_TagNum < captRenderOutputTags::e_SceneEnvTagsCount))
	{
		return l_RenderOutputTags[0][i_TagNum];
	}
	return itString();
}

//------------------------------------------------------------------------
/// @param i_Data render output data structure
/// @param i_TagNum index of the tag
//------------------------------------------------------------------------
itString get_tagstring( captRenderOutputData& i_Data, int i_TagNum )
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
				return itString(temp.c_str());
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
				return itString( oss.str().c_str());
			}
			case captRenderOutputTags::e_CountersZeros:
			{
				std::ostringstream oss;
				oss << std::setfill('0') << std::setw(i_Data.m_nCounterDigits.GetValue());
				oss << i_Data.m_nCurrentFrame.GetValue();
				return itString( oss.str().c_str() );
			}
			case captRenderOutputTags::e_Resolution:
			{
				itString resolution;
				std::ostringstream oss;
				oss << (i_Data.m_nWidth.GetValue()) << "x" << (i_Data.m_nHeight.GetValue());
				resolution = oss.str().c_str();
				return resolution;
			}
			case captRenderOutputTags::e_ResolutionWidth:
			{
				itString resolution;
				std::ostringstream oss;
				oss << i_Data.m_nWidth.GetValue();
				resolution = oss.str().c_str();
				return resolution;
			}
			case captRenderOutputTags::e_ResolutionHeight:
			{
				itString resolution;
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
				itString path;
				fsFileUtil::LocatorToUnicodeString( dir, path );
				return path;
			}
			case captRenderOutputTags::e_CaptureUnique:
			{
				//	need camera "letter"
				itString resolution;
				return resolution;
			}
			case captRenderOutputTags::e_FPS:
			{
				itString value;
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
				itString value;
				std::string timestr;
				tmlnTimeUtil::GetTimeStringInHMSM( tmlnTimeLine::GetValue(), timestr );
				value = timestr.c_str();
				return value;
			}
			case captRenderOutputTags::e_Minutes:
			{
				itString value;
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
				itString value;
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
				itString value;
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
				itString value;
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
				itString value;
				/// \todo fill in this value for RenderType
				return value;
			}
			case captRenderOutputTags::e_StereoEye:
			{
				itString value;
				//	Stereo
				if (i_Data.m_CurrentStereoMode.GetValue() == STEREO_LEFTCAM )
				{
					value = itString(L"Left");
				}
				else if (i_Data.m_CurrentStereoMode.GetValue() == STEREO_RIGHTCAM )
				{
					value = itString(L"Right");
				}
				return value;
			}
			default:
				return itString(L"");
		}
	}
	return itString(L"");
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
	l_RenderOutputTags[0][captRenderOutputTags::e_SceneFilename] = itString(L"SceneFilename");
	l_RenderOutputTags[1][captRenderOutputTags::e_SceneFilename] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_CameraName] = itString(L"CameraName");
	l_RenderOutputTags[1][captRenderOutputTags::e_CameraName] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_RenderLayerName] = itString(L"RenderLayerName");
	l_RenderOutputTags[1][captRenderOutputTags::e_RenderLayerName] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_CompressionType] = itString(L"CompressionType");
	l_RenderOutputTags[1][captRenderOutputTags::e_CompressionType] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_RenderPassName] = itString(L"RenderPassName");
	l_RenderOutputTags[1][captRenderOutputTags::e_RenderPassName] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_Counters] = itString(L"#");
	l_RenderOutputTags[1][captRenderOutputTags::e_Counters] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_CountersZeros] = itString(L"0");
	l_RenderOutputTags[1][captRenderOutputTags::e_CountersZeros] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_OutputBaseDir] = itString(L"OutputBaseDir");
	l_RenderOutputTags[1][captRenderOutputTags::e_OutputBaseDir] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_CaptureUnique] = itString(L"CaptureUnique");
	l_RenderOutputTags[1][captRenderOutputTags::e_CaptureUnique] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_Resolution] = itString(L"Resolution");
	l_RenderOutputTags[1][captRenderOutputTags::e_Resolution] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_ResolutionWidth] = itString(L"ResolutionWidth");
	l_RenderOutputTags[1][captRenderOutputTags::e_ResolutionWidth] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_ResolutionHeight] = itString(L"ResolutionHeight");
	l_RenderOutputTags[1][captRenderOutputTags::e_ResolutionHeight] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_FPS] = itString(L"FPS");
	l_RenderOutputTags[1][captRenderOutputTags::e_FPS] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_Time] = itString(L"Time");
	l_RenderOutputTags[1][captRenderOutputTags::e_Time] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_Minutes] = itString(L"Minutes");
	l_RenderOutputTags[1][captRenderOutputTags::e_Minutes] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_Seconds] = itString(L"Seconds");
	l_RenderOutputTags[1][captRenderOutputTags::e_Seconds] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_Milliseconds] = itString(L"Milliseconds");
	l_RenderOutputTags[1][captRenderOutputTags::e_Milliseconds] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_Frame] = itString(L"Frame");
	l_RenderOutputTags[1][captRenderOutputTags::e_Frame] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_RenderType] = itString(L"RenderType");
	l_RenderOutputTags[1][captRenderOutputTags::e_RenderType] = itString(L"");
	l_RenderOutputTags[0][captRenderOutputTags::e_StereoEye] = itString(L"StereoEye");
	l_RenderOutputTags[1][captRenderOutputTags::e_StereoEye] = itString(L"");
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
void captRenderOutputTagsUtil::SetTag(int i_TagNum, itString& i_Tag)
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
itString captRenderOutputTagsUtil::GetTag(int i_TagNum)
{
	if ((i_TagNum >= 0) && (i_TagNum < captRenderOutputTags::e_SceneEnvTagsCount))
	{
		return l_RenderOutputTags[0][i_TagNum];
	}
	return itString();
}

//------------------------------------------------------------------------
///	Generate the final text output for the filename
///
/// @param i_Data render output data structure
/// @param o_OutputString the generated file name based on the tags
//------------------------------------------------------------------------
void captRenderOutputTagsUtil::GenerateOutputFilename(captRenderOutputData& i_Data, itString& o_OutputString, bool i_bCapturingRenderman /*false*/)
{
	if (i_Data.m_bOutputFileCustomize.GetValue())
	{
		o_OutputString.Clear();

		//	break apart the string into tags and non-tags
		bool bTagParse = false;
		int cindex = 0;
		itString::CharType ch;
		itString accum;
		itString tags( i_Data.m_OutputFileTags.GetValue() );

		//DBG_LOG( "Tagged String = " << tags );

		for (int i=0; i < tags.GetLength(); ++i)
		{
			ch = tags[i];
			if (ch == '<')
			{
				if (accum.GetLength() > 0)
				{
					ParsedNode node;
					node.bIsTag = false;
					node.StringData = accum;
					ParsedData.push(node);
					//DBG_LOG("\t\tnon-tag = " << accum);
					accum.Clear();
				}
				bTagParse = true;
			}
			else if (ch == '>')
			{
				bTagParse = false;
				if (accum.GetLength() > 0)
				{
					ParsedNode node;
					node.bIsTag = true;
					node.StringData = accum;
					ParsedData.push(node);
					//DBG_LOG("\t\ttag = " << accum);
					accum.Clear();
				}
			}
			else
			{
				accum += ch;
			}
		}

		//	if there are characters left over, add them as a non-tag
		if (accum.GetLength() > 0)
		{
			ParsedNode node;
			node.bIsTag = false;
			node.StringData = accum;
			ParsedData.push(node);
			//DBG_LOG("\t\tnon-tag = " << accum);
			accum.Clear();
		}

		//	go through the list and build the text string
		while (!ParsedData.empty())
		{
			itString nodestr = ParsedData.front().StringData;

			if (ParsedData.front().bIsTag)
			{
				if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_SceneFilename] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_SceneFilename);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_CameraName] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CameraName);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_RenderLayerName] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_RenderLayerName);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_CompressionType] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CompressionType);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_RenderPassName] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_RenderPassName);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Counters] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Counters);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_CountersZeros] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CountersZeros);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_OutputBaseDir] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_OutputBaseDir);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_CaptureUnique] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CaptureUnique);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_ResolutionWidth] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_ResolutionWidth);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_ResolutionHeight] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_ResolutionHeight);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Resolution] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Resolution);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_FPS] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_FPS);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Time] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Time);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Minutes] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Minutes);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Seconds] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Seconds);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Milliseconds] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Milliseconds);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Frame] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_Frame);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_RenderType] ))
				{
					o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_RenderType);
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_StereoEye] ))
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
		o_OutputString.Clear();

		//	Scene
		if ( i_Data.m_bUseSceneFilenameInFilename.GetValue() )
		{
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_SceneFilename);
		}

		//	camera
		if ( i_Data.m_bUseCameraNameInFilename.GetValue() )
		{
			o_OutputString += itString(L"_");
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CameraName);
		}

		//	render layer
		if ( i_Data.m_bUseLayerNameInFilename.GetValue() )
		{
			o_OutputString += itString(L"_");
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_RenderLayerName);
		}

		//	Render Pass
		if ( i_Data.m_bUseRenderPassInFilename.GetValue())
		{
			o_OutputString += itString(L"_");
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_RenderPassName);
		}

		//	compression
		if ( i_Data.m_bUseCompressionInFilename.GetValue() && i_Data.m_bCaptureMovie.GetValue())
		{
			o_OutputString += itString(L"_");
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_CompressionType);
		}

		//	Stereo
		if (   (i_Data.m_CurrentStereoMode.GetValue() == STEREO_LEFTCAM )
			|| (i_Data.m_CurrentStereoMode.GetValue() == STEREO_RIGHTCAM ))
		{
			o_OutputString += itString(L"_");
			o_OutputString += get_tagstring(i_Data, captRenderOutputTags::e_StereoEye);
		}

		//	digits
		if ( !i_Data.m_bCaptureMovie.GetValue() || i_bCapturingRenderman )
		{
			o_OutputString += itString(L"_");
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
		itString::CharType ch;
		itString accum;
		itString tags( i_Data.m_OutputDirectoryTags.GetValue() );

		//DBG_LOG( "Tagged String = " << tags );

		for (int i=0; i < tags.GetLength(); ++i)
		{
			ch = tags[i];
			if (ch == '<')
			{
				if (accum.GetLength() > 0)
				{
					ParsedNode node;
					node.bIsTag = false;
					node.StringData = accum;
					ParsedData.push(node);
					//DBG_LOG("\t\tnon-tag = " << accum);
					accum.Clear();
				}
				bTagParse = true;
			}
			else if (ch == '>')
			{
				bTagParse = false;
				if (accum.GetLength() > 0)
				{
					ParsedNode node;
					node.bIsTag = true;
					node.StringData = accum;
					ParsedData.push(node);
					//DBG_LOG("\t\ttag = " << accum);
					accum.Clear();
				}
			}
			else if (ch == '\\')
			{
				//	add what has accumulated before the slash
				if (accum.GetLength() > 0)
				{
					ParsedNode node;
					node.bIsTag = false;
					node.StringData = accum;
					ParsedData.push(node);
					//DBG_LOG("\t\tnon-tag = " << accum);
					accum.Clear();
				}

				//	if last node was a backslash, then this is part
				//	of a UNC path most likely, so add to the slash node
				if (   (ParsedData.size() > 0)
					&& (ParsedData.back().StringData == itString("\\")))
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
					accum.Clear();
				}
			}
			else
			{
				accum += ch;
			}
		}

		//	if there are left over chars, add them.
		if (accum.GetLength() > 0)
		{
			ParsedNode node;
			node.bIsTag = false;
			node.StringData = accum;
			ParsedData.push(node);
			//DBG_LOG("\t\tnon-tag = " << accum);
			accum.Clear();
		}

		//	go through the list and build the text string
		itString singledir;
		accum.Clear();
		while (!ParsedData.empty())
		{
			itString nodestr = ParsedData.front().StringData;
			
			//DBG_LOG("Node -> " << nodestr );

			if (ParsedData.front().bIsTag)
			{
				if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_SceneFilename] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_SceneFilename) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_CameraName] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_CameraName) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_RenderLayerName] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_RenderLayerName) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_CompressionType] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_CompressionType) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_RenderPassName] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_RenderPassName) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Counters] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Counters) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_CountersZeros] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_CountersZeros) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_OutputBaseDir] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_OutputBaseDir) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_CaptureUnique] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_CaptureUnique) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_ResolutionWidth] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_ResolutionWidth) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_ResolutionHeight] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_ResolutionHeight) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Resolution] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Resolution) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_FPS] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_FPS) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Time] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Time) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Minutes] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Minutes) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Seconds] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Seconds) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Milliseconds] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Milliseconds) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_Frame] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_Frame) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_RenderType] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_RenderType) );
				}
				else if (nodestr.StartsWith( l_RenderOutputTags[0][captRenderOutputTags::e_StereoEye] ))
				{
					singledir += ( get_tagstring(i_Data, captRenderOutputTags::e_StereoEye) );
				}
			}
			else
			{
				if (nodestr == itString("\\"))
				{
					//	ignore nodes that are just the "\"
					fsLocator subdir;
					fsFileUtil::UnicodeStringToLocator( singledir, subdir );
					dir.Push( subdir );
					//DBG_LOG("\t\t->dir node = " << singledir);
					singledir.Clear();
				}
				//else if (nodestr.StartsWith("\\")
				else
				{
					//UNC?????
					if (nodestr.StartsWith(itString("\\\\")))
					{
						dir.SetUNCPath(true);
					}
					else if (nodestr.StartsWith(itString("\\")))
					{
						nodestr.RemoveCharAt(0);
					}

					if (nodestr.EndsWith(itString("\\")))
					{
						nodestr.RemoveCharAt(nodestr.GetLength()-1);
						singledir += nodestr;
						fsLocator subdir;
						fsFileUtil::UnicodeStringToLocator( singledir, subdir );
						dir.Push( subdir );
						//DBG_LOG("\t\t->dir node - " << singledir);
						singledir.Clear();
					}
					else
						singledir += nodestr;
				}
			}
			ParsedData.pop();
		}

		//	push anything left over on the directory
		if (singledir.GetLength() > 0)
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

	itString FilenameTags;

	if (io_Data.m_bUseSceneFilenameInFilename == true)
	{
		FilenameTags += itString(L"<");
		FilenameTags += l_RenderOutputTags[0][captRenderOutputTags::e_SceneFilename];
		FilenameTags += itString(L">");
	}
	if (io_Data.m_bUseCameraNameInFilename == true)
	{
		if (FilenameTags.GetLength() > 0)
			FilenameTags += itString(L"_");
		FilenameTags += itString(L"<");
		FilenameTags += l_RenderOutputTags[0][captRenderOutputTags::e_CameraName];
		FilenameTags += itString(L">");
	}
	if (io_Data.m_bUseLayerNameInFilename == true)
	{
		if (FilenameTags.GetLength() > 0)
			FilenameTags += itString(L"_");
		FilenameTags += itString(L"<");
		FilenameTags += l_RenderOutputTags[0][captRenderOutputTags::e_RenderLayerName];
		FilenameTags += itString(L">");
	}
	if (io_Data.m_bUseCompressionInFilename == true)
	{
		if (FilenameTags.GetLength() > 0)
			FilenameTags += itString(L"_");
		FilenameTags += itString(L"<");
		FilenameTags += l_RenderOutputTags[0][captRenderOutputTags::e_CompressionType];
		FilenameTags += itString(L">");
	}
	if (io_Data.m_bUseRenderPassInFilename == true)
	{
		if (FilenameTags.GetLength() > 0)
			FilenameTags += itString(L"_");
		FilenameTags += itString(L"<");
		FilenameTags += l_RenderOutputTags[0][captRenderOutputTags::e_RenderPassName];
		FilenameTags += itString(L">");
	}
	//if (using stereo)
	{
		if (FilenameTags.GetLength() > 0)
			FilenameTags += itString(L"_");
		FilenameTags += itString(L"<");
		FilenameTags += l_RenderOutputTags[0][captRenderOutputTags::e_StereoEye];
		FilenameTags += itString(L">");
	}

	if (io_Data.m_nCounterDigits > 0)
	{
		if (FilenameTags.GetLength() > 0)
			FilenameTags += itString(L"_");
		FilenameTags += itString(L"<");
		for (int i=0; i < io_Data.m_nCounterDigits.GetValue(); ++i)
			FilenameTags += itString(L"0");
		FilenameTags += itString(L">");
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

	itString DirectoryTags;

	if (io_Data.m_OutputDirectoryRoot.GetValue().GetNumNames() > 0)
	{
		//fsFileUtil::LocatorToUnicodeString( io_Data.m_OutputDirectoryRoot.GetValue(), DirectoryTags );
		DirectoryTags += itString(L"<");
		DirectoryTags += l_RenderOutputTags[0][captRenderOutputTags::e_OutputBaseDir];
		DirectoryTags += itString(L">");
	}
	if (io_Data.m_bUseSceneFilenameAsDirectory == true)
	{
		if (DirectoryTags.GetLength() > 0)
			DirectoryTags += itString(L"\\");
		DirectoryTags += itString(L"<");
		DirectoryTags += l_RenderOutputTags[0][captRenderOutputTags::e_SceneFilename];
		DirectoryTags += itString(L">");
	}
	if (io_Data.m_bUseCameraNameAsDirectory == true)
	{
		if (DirectoryTags.GetLength() > 0)
			DirectoryTags += itString(L"\\");
		DirectoryTags += itString(L"<");
		DirectoryTags += l_RenderOutputTags[0][captRenderOutputTags::e_CameraName];
		DirectoryTags += itString(L">");
	}
	if (io_Data.m_bUseLayerNameAsDirectory == true)
	{
		if (DirectoryTags.GetLength() > 0)
			DirectoryTags += itString(L"\\");
		DirectoryTags += itString(L"<");
		DirectoryTags += l_RenderOutputTags[0][captRenderOutputTags::e_RenderLayerName];
		DirectoryTags += itString(L">");
	}
	if (io_Data.m_bUseRenderPassAsDirectory == true)
	{
		if (DirectoryTags.GetLength() > 0)
			DirectoryTags += itString(L"\\");
		DirectoryTags += itString(L"<");
		DirectoryTags += l_RenderOutputTags[0][captRenderOutputTags::e_RenderPassName];
		DirectoryTags += itString(L">");
	}
	if (io_Data.m_bUseResolutionAsDirectory == true)
	{
		if (DirectoryTags.GetLength() > 0)
			DirectoryTags += itString(L"\\");
		DirectoryTags += itString(L"<");
		DirectoryTags += l_RenderOutputTags[0][captRenderOutputTags::e_Resolution];
		DirectoryTags += itString(L">");
	}

	io_Data.m_OutputDirectoryTags.SetValue( DirectoryTags );
}

