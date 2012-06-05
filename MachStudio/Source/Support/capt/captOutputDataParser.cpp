/********************************************************************************************\
**	captOutputDataParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\********************************************************************************************/
#include "Support/capt/captOutputDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/ch/chChunkParserUtil.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Support/rlyr/data/rlyrLayersDataParser.hpp"


//============================================================================
//============================================================================
namespace captOutputDataParser
{

//============================================================================
//============================================================================
namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_COPT = chDefs::MakeName('C', 'O', 'P', 'T');	// main data - capture output
const chDefs::Name c_CAPD = chDefs::MakeName('C', 'A', 'P', 'D');	// +-- global capture data
const chDefs::Name c_LYRS = chDefs::MakeName('L', 'Y', 'R', 'S');	//   +-- render layer list data
const chDefs::Name c_RLYR = chDefs::MakeName('R', 'L', 'Y', 'R');	//   +-- render layer data


////------------------------------------------------------------------------
////	Read global capture data
////------------------------------------------------------------------------
void ReadGlobalCaptureData(chReader& io_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						captRenderOutputData& o_CaptureData )
{
	ReadCaptureData(io_Reader, i_Version, i_Size, o_CaptureData);
}

////------------------------------------------------------------------------
////	Write global capture data
////------------------------------------------------------------------------
void WriteGlobalCaptureData( chWriter& o_Writer,
						 const captRenderOutputData& i_CaptureData)
{
	//const int l_cCAPD_VERSION = 1;
	//o_Writer.WriteChunkHeader( c_CAPD, l_cCAPD_VERSION, true );

	WriteCaptureData(o_Writer, i_CaptureData);

	//o_Writer.FinishChunk();
}


////------------------------------------------------------------------------
////	ReadLayerData
////------------------------------------------------------------------------
void ReadLayerListData(chReader& io_Reader,
						chDefs::Version i_Version,
						rlyrLayersData& o_LayerData )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;
	int index = 0;

	rlyrLayerDataItem cur_layer;
	while( io_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_RLYR )
		{
			rlyrLayersDataParser::ReadAllLayerData(io_Reader, i_Version, size, cur_layer);
			o_LayerData.m_Layers.push_back(cur_layer);

			io_Reader.FinishChunk();
		}
		else
		{
			//DBG_ASSERT(false, "invalid chunk header");
			DBG_TRACE("invalid chunk header" << name);
		}
	}
}

////------------------------------------------------------------------------
////	WriteLayerListData
////------------------------------------------------------------------------
//void WriteLayerListData( chWriter& o_Writer,
//						 const rlyrLayersData& i_LayerData)
//{
//	const int l_cLYRS_VERSION = 1;
//	o_Writer.WriteChunkHeader( c_LYRS, l_cLYRS_VERSION, true );
//
//	for(int i = 0; i < i_LayerData.m_Layers.size(); ++i)
//		rlyrLayersDataParser::WriteAllLayerData(o_Writer, i_LayerData.m_Layers[i]);
//
//	o_Writer.FinishChunk();
//}

}	// local namespace


//------------------------------------------------------------------------
//	returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_COPT;
}

//------------------------------------------------------------------------
//	ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				captRenderOutputData& o_CaptureData,
				rlyrLayersData& o_LayerData)
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_CAPD )
		{
			ReadGlobalCaptureData(i_Reader, version, size, o_CaptureData);
		}
		else
		{
			if ( version == 1)
			{
				if ( name == c_LYRS )
				{
					ReadLayerListData(i_Reader, version, o_LayerData);
				}
				else
				{
					DBG_TRACE("invalid chunk header " << name);
				}
			}
			else
			{
				DBG_TRACE("invalid chunk header " << name);
			}
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const captRenderOutputData& i_CaptureData)
{
	const int l_cCOPT_VERSION = 2;
	o_Writer.WriteChunkHeader( c_COPT, l_cCOPT_VERSION, true );

	WriteGlobalCaptureData( o_Writer, i_CaptureData);
	
	//version 1
	//Layers will be written in its own chunk
	//WriteLayerListData( o_Writer, i_LayerData );

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadCaptureData
//------------------------------------------------------------------------
void ReadCaptureData(	chReader& io_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						captRenderOutputData& o_Data)
{
	o_Data.m_bChunkData.Read( io_Reader );
	o_Data.m_bCaptureMovie.Read( io_Reader );
	o_Data.m_bMoviePerDriver.Read( io_Reader );
	o_Data.m_bShowRenderProgressDialog.Read( io_Reader );
	
	o_Data.m_bUseMarkerTimes.Read( io_Reader );
	o_Data.m_fMarkerInTime.Read( io_Reader );
	o_Data.m_fMarkerOutTime.Read( io_Reader );
	
	o_Data.m_fCaptureFPS.Read( io_Reader );
	o_Data.m_nMotionSamplesPerFrame.Read( io_Reader );
	o_Data.m_nCaptureMax.Read( io_Reader );
	o_Data.m_nCaptureSampling.Read( io_Reader );
	o_Data.m_bJitteredSampling.Read( io_Reader );
	o_Data.m_CaptureFormat.Read( io_Reader );
	o_Data.m_CompressCode.Read( io_Reader );
	o_Data.m_OutputDirectoryRoot.Read( io_Reader );
	o_Data.m_bDisplayTimeCode.Read( io_Reader );
	o_Data.m_fEndTime.Read( io_Reader );
	o_Data.m_fLeadIn.Read( io_Reader );
	o_Data.m_fLeadOut.Read( io_Reader );
	o_Data.m_Prefix.Read( io_Reader );
	o_Data.m_fStartTime.Read( io_Reader );
	o_Data.m_bCaptureAllCameras.Read( io_Reader );
	o_Data.m_bUseCameraNameInFilename.Read( io_Reader );
	o_Data.m_bUseSceneFilenameInFilename.Read( io_Reader );
	o_Data.m_bUseCompressionInFilename.Read( io_Reader );
	o_Data.m_bUseRenderDriversAsUniqueCameras.Read( io_Reader );
	o_Data.m_bUseCameraNameAsDirectory.Read( io_Reader );
	o_Data.m_bUseResolutionAsDirectory.Read( io_Reader );
	o_Data.m_bUseSceneFilenameAsDirectory.Read( io_Reader );
	o_Data.m_nWidth.Read( io_Reader );
	o_Data.m_nHeight.Read( io_Reader );
	o_Data.m_bOutputTitleCard.Read( io_Reader );
	o_Data.m_nCounterDigits.Read( io_Reader );
	//o_Data.m_nSubdivLevel.Read( io_Reader );
	o_Data.m_bSmoothing.Read( io_Reader );
	o_Data.m_bKeepFrameOpenAfterRender.Read( io_Reader );
	o_Data.m_bSendPostEmailAddress.Read( io_Reader );
	o_Data.m_PostEmailAddress.Read( io_Reader );
	o_Data.m_PostCommand.Read( io_Reader );
	o_Data.m_bExecutePostCommand.Read( io_Reader );
	o_Data.m_bUseLayerVisibility.Read( io_Reader );

#ifdef DEMO_VERSION
	//	force the width and height
	o_Data.m_nWidth.SetValue( 852 );
	o_Data.m_nHeight.SetValue( 480 );
#endif

	bool bUseCaptureDrivers = false;

	if (i_Version > 1)
	{
		o_Data.m_bUseLayerNameAsDirectory.Read( io_Reader );
		o_Data.m_bUseLayerNameInFilename.Read( io_Reader );

		if (i_Version > 2)
		{
			o_Data.m_FilterWidth.Read(io_Reader);
			o_Data.m_FilterFunc.Read(io_Reader);
		}
		if (i_Version > 3)
		{
			o_Data.m_Cameras.Read(io_Reader);
		}
		if (i_Version > 4)
		{
			// No longer using UseCaptureDrivers boolean, replaced with Render Frame Range enum.
			// Store value in local boolean to use when reading older file formats.
			//o_Data.m_bUseCaptureDrivers.Read(io_Reader);
			io_Reader.Read(bUseCaptureDrivers);
		}
		if (i_Version > 5)
		{
			o_Data.m_PixelAspect.Read(io_Reader);
		}
		if (i_Version > 6)
		{
			o_Data.m_OutputDirectoryTags.Read(io_Reader);
			o_Data.m_bOutputDirectoryCustomize.Read(io_Reader);
			o_Data.m_OutputFileTags.Read(io_Reader);
			o_Data.m_bOutputFileCustomize.Read(io_Reader);
		}
		if (i_Version > 7)
		{
			o_Data.m_bUseRenderPassInFilename.Read(io_Reader);
		}
		if (i_Version > 8)
		{
			o_Data.m_bUseRenderPassAsDirectory.Read(io_Reader);
		}
		if (i_Version > 9)
		{
			o_Data.m_bHardwareAA.Read(io_Reader);
		}
		if (i_Version > 10)
		{
			o_Data.m_RenderFrameRange.Read(io_Reader);
			// If value is above the range we understand, then it must be coming from
			// a newer file format. Clamp down to a format we do understand.
			if (o_Data.m_RenderFrameRange.GetValue() >= captRenderOutputData::eNumRenderFrameRangeOptions)
				o_Data.m_RenderFrameRange.SetValue(captRenderOutputData::eTimeline);
		}
		else
		{
			// Older file format was read. Try to deduce the correct value for m_RenderFrameRange
			// from the other values...
			if (bUseCaptureDrivers)
				o_Data.m_RenderFrameRange.SetValue(captRenderOutputData::eCaptureDrivers);
			else if ((o_Data.m_fStartTime.GetValue() > maTime::c_ZeroTime) ||
					 (o_Data.m_fEndTime.GetValue() >= maTime::c_ZeroTime))
				o_Data.m_RenderFrameRange.SetValue(captRenderOutputData::eSpecifyRange);
			else
				o_Data.m_RenderFrameRange.SetValue(captRenderOutputData::eTimeline);
		}
	}
}

//------------------------------------------------------------------------
//   WriteCaptureData - write each value of the capture data to the document
//------------------------------------------------------------------------
void WriteCaptureData(	chWriter& o_Writer,
						const captRenderOutputData& i_Data)
{
	const int l_cCAPD_VERSION = 11;
	o_Writer.WriteChunkHeader( c_CAPD, l_cCAPD_VERSION, true );

	i_Data.m_bChunkData.Write( o_Writer );
	i_Data.m_bCaptureMovie.Write( o_Writer );
	i_Data.m_bMoviePerDriver.Write( o_Writer );
	i_Data.m_bShowRenderProgressDialog.Write( o_Writer );
	
	i_Data.m_bUseMarkerTimes.Write( o_Writer );
	i_Data.m_fMarkerInTime.Write( o_Writer );
	i_Data.m_fMarkerOutTime.Write( o_Writer );
	
	i_Data.m_fCaptureFPS.Write( o_Writer );
	i_Data.m_nMotionSamplesPerFrame.Write( o_Writer );
	i_Data.m_nCaptureMax.Write( o_Writer );
	i_Data.m_nCaptureSampling.Write( o_Writer );
	i_Data.m_bJitteredSampling.Write( o_Writer );
	i_Data.m_CaptureFormat.Write( o_Writer );
	i_Data.m_CompressCode.Write( o_Writer );
	i_Data.m_OutputDirectoryRoot.Write( o_Writer );
	i_Data.m_bDisplayTimeCode.Write( o_Writer );
	i_Data.m_fEndTime.Write( o_Writer );
	i_Data.m_fLeadIn.Write( o_Writer );
	i_Data.m_fLeadOut.Write( o_Writer );
	i_Data.m_Prefix.Write( o_Writer );
	i_Data.m_fStartTime.Write( o_Writer );
	i_Data.m_bCaptureAllCameras.Write( o_Writer );
	i_Data.m_bUseCameraNameInFilename.Write( o_Writer );
	i_Data.m_bUseSceneFilenameInFilename.Write( o_Writer );
	i_Data.m_bUseCompressionInFilename.Write( o_Writer );
	i_Data.m_bUseRenderDriversAsUniqueCameras.Write( o_Writer );
	i_Data.m_bUseCameraNameAsDirectory.Write( o_Writer );
	i_Data.m_bUseResolutionAsDirectory.Write( o_Writer );
	i_Data.m_bUseSceneFilenameAsDirectory.Write( o_Writer );
	i_Data.m_nWidth.Write( o_Writer );
	i_Data.m_nHeight.Write( o_Writer );
	i_Data.m_bOutputTitleCard.Write( o_Writer );
	i_Data.m_nCounterDigits.Write( o_Writer );
	//i_Data.m_nSubdivLevel.Write( o_Writer );
	i_Data.m_bSmoothing.Write( o_Writer );
	i_Data.m_bKeepFrameOpenAfterRender.Write( o_Writer );
	i_Data.m_bSendPostEmailAddress.Write( o_Writer );
	i_Data.m_PostEmailAddress.Write( o_Writer );
	i_Data.m_PostCommand.Write( o_Writer );
	i_Data.m_bExecutePostCommand.Write( o_Writer );
	i_Data.m_bUseLayerVisibility.Write( o_Writer );

	//version 2
	i_Data.m_bUseLayerNameAsDirectory.Write( o_Writer );
	i_Data.m_bUseLayerNameInFilename.Write( o_Writer );

	// version 3
	i_Data.m_FilterWidth.Write(o_Writer);
	i_Data.m_FilterFunc.Write(o_Writer);

	//version 4
	i_Data.m_Cameras.Write( o_Writer );

	//version 5
	//i_Data.m_bUseCaptureDrivers.Write( o_Writer );
	//version 11 - replaces m_bUseCaptureDrivers with m_RenderFrameRange below; need to put
	// a boolean in its place in order to keep file format the same.
	bool bUseCaptureDrivers = (i_Data.m_RenderFrameRange.GetValue() == captRenderOutputData::eCaptureDrivers);
	o_Writer.Write(bUseCaptureDrivers);

	//version 6
	i_Data.m_PixelAspect.Write(o_Writer);

	//version 7
	i_Data.m_OutputDirectoryTags.Write(o_Writer);
	i_Data.m_bOutputDirectoryCustomize.Write(o_Writer);
	i_Data.m_OutputFileTags.Write(o_Writer);
	i_Data.m_bOutputFileCustomize.Write(o_Writer);

	//version 8
	i_Data.m_bUseRenderPassInFilename.Write(o_Writer);

	// version 9
	i_Data.m_bUseRenderPassAsDirectory.Write( o_Writer );

	// version 10 
	i_Data.m_bHardwareAA.Write(o_Writer);

	// version 11
	i_Data.m_RenderFrameRange.Write(o_Writer);

	// finish the chunk
	o_Writer.FinishChunk();
}

}	// end of namespace

