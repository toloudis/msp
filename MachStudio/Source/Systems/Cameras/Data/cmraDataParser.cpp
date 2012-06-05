/********************************************************************************************\
**  cmraCamerasDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Cameras/Data/cmraDataParser.hpp"

#include "Support/mnm/mnmBaseDataParser.hpp"
#include "Support/tmln/tmlnBaseDataParser.hpp"
#include "Support/tmln/tmlnChannelInfoParser.hpp"
#include "Support/tmln/tmlnParser.hpp"
#include "Support/xtra/xtraPropertyDataParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/it/itStringUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	//
	//	constants
	//
	const chDefs::Name c_CAMD = chDefs::MakeName('C', 'A', 'M', 'D');
	const chDefs::Name c_ECAM = chDefs::MakeName('E', 'C', 'A', 'M');	// editor camera, replaces INIT
	const chDefs::Name c_INIT = chDefs::MakeName('I', 'N', 'I', 'T');
	const chDefs::Name c_CAMR = chDefs::MakeName('C', 'A', 'M', 'R');
	const chDefs::Name c_CAMI = chDefs::MakeName('C', 'A', 'M', 'I');
	const chDefs::Name c_CABD = chDefs::MakeName('C', 'A', 'B', 'D');		// camera base data
	const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');
	const chDefs::Name c_CCFD = chDefs::MakeName('C', 'C', 'F', 'D');
	const chDefs::Name c_XTRP = chDefs::MakeName('X', 'T', 'R', 'P');


	//------------------------------------------------------------------------
	// Used to support old file formats of channel info. This list
	// does not need to be updated as channels are added, it just
	// represents the order of the channels when the file format
	// for channel info was changed from indexing to name mapping.
	//------------------------------------------------------------------------
	const char* c_ChannelNameStrings[] = 
	{ "Position", "Target", "FOV", "Tilt", "Capture", "NearFocusDist", "FarFocusDist",
	  "NearBlurDist", "FarBlurDist", "NearClip", "FarClip", "DOFMaxFarBlur", "HDRMiddleGray", "HDRBloomScale",
	  "HDRStarScale", "HDRBrightPassThresh", "HDRBrightPassOffset", "HDRWhiteCutoff", "HDRSceneLuminance", "AO", "GI", "Reflections"
	};
	const int c_NumChannelStrings = 22;


	//------------------------------------------------------------------------
	//   read_CABD - read camera base data
	//------------------------------------------------------------------------
	void read_CABD(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					cmraCameraData& o_Data )
	{
		o_Data.m_Name.Read(i_Reader);
		o_Data.m_bEditorVisible.Read(i_Reader);
		o_Data.m_Position.Read(i_Reader);
		o_Data.m_Orientation.Read(i_Reader);

		if (i_Version > 1)
		{
			o_Data.m_Description.Read(i_Reader);
		}
		else if (i_Version == 1)
		{
			std::string new_desc;
			itString desc;
			chChunkParserUtil::Read(i_Reader,desc);
			new_desc = itStringUtil::GetStdString(desc);
			o_Data.m_Description.SetValue(new_desc);
		}
	}

	//------------------------------------------------------------------------
	//   read_CAMI - read camera base information
	//------------------------------------------------------------------------
	void read_CAMI(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					cmraCameraData& o_Data )
	{
		if ( i_Version < 3 )
		{
			if ( i_Version >= 1 )
			{
				std::string name;
				chChunkParserUtil::Read(i_Reader, name);
				o_Data.m_Name.SetValue( name );

				if (i_Version >= 2)
				{
					nameUID uid;
					chChunkParserUtil::Read(i_Reader, uid);
					o_Data.m_Name.SetUID( uid );
				}
			}

			o_Data.m_Position.Read(i_Reader);
			o_Data.m_Target.Read(i_Reader);

			if (i_Version >= 1)
			{
				o_Data.m_FOV.Read(i_Reader);
				o_Data.m_Tilt.Read(i_Reader);
				o_Data.m_Near.Read(i_Reader);
				o_Data.m_Far.Read(i_Reader);
			}

			//DBG_LOG( "read camera (" << o_Data.m_Name.c_str() << ")"  );
		}
		else
		{
			o_Data.m_Target.Read(i_Reader);
			o_Data.m_FOV.Read(i_Reader);
			o_Data.m_Tilt.Read(i_Reader);
			o_Data.m_Near.Read(i_Reader);
			o_Data.m_Far.Read(i_Reader);
			if (i_Version == 4)
			{
				o_Data.m_bEnableDOF.Read(i_Reader);
				o_Data.m_NearBlurDistance.Read(i_Reader);
				o_Data.m_FarBlurDistance.Read(i_Reader);
				o_Data.m_NearFocalDistance.Read(i_Reader);
				o_Data.m_FarFocalDistance = o_Data.m_NearFocalDistance;
				o_Data.m_MaxFarBlur.Read(i_Reader);
			}
			else if (i_Version >= 5)
			{
				o_Data.m_bEnableDOF.Read(i_Reader);
				o_Data.m_NearBlurDistance.Read(i_Reader);
				o_Data.m_NearFocalDistance.Read(i_Reader);
				o_Data.m_FarFocalDistance.Read(i_Reader);
				o_Data.m_FarBlurDistance.Read(i_Reader);
				o_Data.m_MaxFarBlur.Read(i_Reader);
			}
			if (i_Version >= 6)
			{
				o_Data.m_HDRMiddleGray.Read(i_Reader);
				o_Data.m_HDRBloomScale.Read(i_Reader);
				o_Data.m_HDRStarScale.Read(i_Reader);
				o_Data.m_HDRBrightPassThresh.Read(i_Reader);
				o_Data.m_HDRBrightPassOffset.Read(i_Reader);
				o_Data.m_HDRWhiteCutoff.Read(i_Reader);
				o_Data.m_HDRStarType.Read(i_Reader);
			}
			if (i_Version >= 7)
			{
				if (i_Version < 9)
				{
					// removed v9
					prtyBoolean bHDRAdaptiveLuminance;
					bHDRAdaptiveLuminance.Read(i_Reader);
				}
				o_Data.m_HDRSceneLuminance.Read(i_Reader);
			}
			if (i_Version >= 8)
			{
				o_Data.m_bOrthographic.Read(i_Reader);
				o_Data.m_OrthoWidth.Read(i_Reader);
			}
			if (i_Version >= 10)
			{
				o_Data.m_MaxCoC.Read(i_Reader);
			}

			if (i_Version >= 11)
			{				
				//o_Data.m_bEnableStereo.Read(i_Reader);
				prtyFloat dummy;			//deprecated (replaced with IOD in version 14)
				dummy.Read(i_Reader);

				if ( i_Version == 11 )
				{
					if ( dummy != 0 )
					{
						o_Data.m_StereoType.SetValue(0);
					}
				}
				o_Data.m_StereoFD.Read(i_Reader);
			}

			if (i_Version >= 12)
			{
				o_Data.m_StereoFilterColor.Read(i_Reader);
			}
			if (i_Version >= 13)
			{
				o_Data.m_StereoFilterColor.Read(i_Reader);
			}
			if (i_Version >= 14)
			{
				o_Data.m_StereoIOD.Read(i_Reader);
			}
			if (i_Version >= 15)
			{
				o_Data.m_StereoFilterColor.Read(i_Reader);
				o_Data.m_StereoType.Read(i_Reader);
			}

			// Making sure that toe-in gets assigned to cameras that dont know about off-axis
			if ( i_Version < 16 )
			{
				o_Data.m_StereoProjection.SetValue(0);
			} 

			if (i_Version >= 16)
			{				
				o_Data.m_StereoProjection.Read(i_Reader);				
			}

			// version 17 - start of real world camera properties
			if (i_Version >= 17)
			{				
				o_Data.m_bEnableALP.Read(i_Reader);	
				o_Data.m_FocalLength.Read(i_Reader);	
				o_Data.m_HorizontalAperture.Read(i_Reader);	
			}
			else
			{
				// Cameras written before version 17 need to have ALP false,
				// even if the default for ALP for new cameras is set to true
				o_Data.m_bEnableALP = false;
			}

			if (i_Version >= 18)
			{				
				o_Data.m_FocalDistance.Read(i_Reader);	
				o_Data.m_Fstop.Read(i_Reader);	
			}

			if(i_Version >=19)
			{
				o_Data.m_CoC.Read(i_Reader);
			}

		}
	}

	//------------------------------------------------------------------------
	//   read_ECAM - read editor camera chunk
	//------------------------------------------------------------------------
	void read_ECAM(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					cmraCameraData& o_Data )
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;

		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_CABD )
			{
				read_CABD(i_Reader, version, size, o_Data);
			}
			else if ( name == c_CAMI )
			{
				read_CAMI(i_Reader, version, size, o_Data);
			}

			i_Reader.FinishChunk();
		}
	}
	//------------------------------------------------------------------------
	//   WriteCameraBaseData - write just the chunks related to
	//	the base camera data. Used to write scripted data and to write the
	//	editor camera data.
	//------------------------------------------------------------------------
	void WriteCameraBaseData(	chWriter& o_Writer,
							const cmraCameraData& i_Data )
	{
		//	write base info
		const int l_CABD_VERSION = 2;
		o_Writer.WriteChunkHeader( c_CABD, l_CABD_VERSION, false );
		i_Data.m_Name.Write(o_Writer);
		i_Data.m_bEditorVisible.Write(o_Writer);
		i_Data.m_Position.Write(o_Writer);
		i_Data.m_Orientation.Write(o_Writer);
		i_Data.m_Description.Write(o_Writer);
		o_Writer.FinishChunk();

		// Write cam info
		const int l_CAMI_VERSION = 19;
		o_Writer.WriteChunkHeader( c_CAMI, l_CAMI_VERSION, false );
		i_Data.m_Target.Write(o_Writer);
		i_Data.m_FOV.Write(o_Writer);
		i_Data.m_Tilt.Write(o_Writer);
		i_Data.m_Near.Write(o_Writer);
		i_Data.m_Far.Write(o_Writer);
		// added in version 4 and modified in 5:
		i_Data.m_bEnableDOF.Write(o_Writer);
		i_Data.m_NearBlurDistance.Write(o_Writer);
		i_Data.m_NearFocalDistance.Write(o_Writer);
		i_Data.m_FarFocalDistance.Write(o_Writer);
		i_Data.m_FarBlurDistance.Write(o_Writer);
		i_Data.m_MaxFarBlur.Write(o_Writer);
		// added in version 6:
		i_Data.m_HDRMiddleGray.Write(o_Writer);
		i_Data.m_HDRBloomScale.Write(o_Writer);
		i_Data.m_HDRStarScale.Write(o_Writer);
		i_Data.m_HDRBrightPassThresh.Write(o_Writer);
		i_Data.m_HDRBrightPassOffset.Write(o_Writer);
		i_Data.m_HDRWhiteCutoff.Write(o_Writer);
		i_Data.m_HDRStarType.Write(o_Writer);
		// added in version 7:
		//i_Data.m_bHDRAdaptiveLuminance.Write(o_Writer);// removed in v9
		i_Data.m_HDRSceneLuminance.Write(o_Writer);
		// added in version 8:
		i_Data.m_bOrthographic.Write(o_Writer);
		i_Data.m_OrthoWidth.Write(o_Writer);
		// version 9 removed m_bHDRAdaptiveLuminance
		// added in version 10:
		i_Data.m_MaxCoC.Write(o_Writer);

		// version 11 for stereoscopy
		//i_Data.m_bEnableStereo.Write(o_Writer);
		prtyFloat dummy;
		dummy.Write(o_Writer);	//deprecated 		
		i_Data.m_StereoFD.Write(o_Writer);

		// version 12
		i_Data.m_StereoFilterColor.Write(o_Writer);

		// version 13
		i_Data.m_StereoType.Write(o_Writer);

		// version 14
		i_Data.m_StereoIOD.Write(o_Writer);

		// version 15
		i_Data.m_StereoFilterColor.Write(o_Writer);
		i_Data.m_StereoType.Write(o_Writer);

		// version 16
		i_Data.m_StereoProjection.Write(o_Writer);

		// version 17 - start of real world camera properties
		i_Data.m_bEnableALP.Write(o_Writer);
		i_Data.m_FocalLength.Write(o_Writer);
		i_Data.m_HorizontalAperture.Write(o_Writer);

		// version 18
		i_Data.m_FocalDistance.Write(o_Writer);
		i_Data.m_Fstop.Write(o_Writer);

		//version 19
		i_Data.m_CoC.Write(o_Writer);


		o_Writer.FinishChunk();
	}

	//------------------------------------------------------------------------
	//   ReadCameraData
	//------------------------------------------------------------------------
	void ReadCameraData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							cmraScriptData& o_Data )
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;

		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			// Info divided into base chunk and driver list
			if ( name == mnmBaseDataParser::GetChunkName() )
			{
				mnmBaseData bdata;
				mnmBaseDataParser::ReadData(i_Reader, 
								 cmraCamerasDataParser::GetChunkName(), 
								 version, size, bdata);
				o_Data.m_BaseData.m_Name			= bdata.m_Name;
				o_Data.m_BaseData.m_bEditorVisible	= bdata.m_bEditorVisible;
				o_Data.m_BaseData.m_Position		= bdata.m_Position;
				o_Data.m_BaseData.m_Orientation		= bdata.m_Orientation;

				if ( bdata.m_Drivers.size() > 0 )
				{
					envSTLHelpers::DeleteContainer(o_Data.m_Drivers);

					int num_drivers = bdata.m_Drivers.size();
					for (int i=0; i<num_drivers; i++)
					{
						o_Data.m_Drivers.push_back(bdata.m_Drivers[i]->Clone());
					}
				}
			}
			else if ( name == tmlnBaseDataParser::GetChunkName() )
			{
				tmlnBaseData tmln_data;
				tmlnBaseDataParser::ReadData(i_Reader, 
								 cmraCamerasDataParser::GetChunkName(), 
								 version, size, tmln_data);

				envSTLHelpers::DeleteContainer(o_Data.m_Drivers);

				int num_drivers = tmln_data.m_Drivers.size();
				for (int i=0; i<num_drivers; i++)
				{
					o_Data.m_Drivers.push_back(tmln_data.m_Drivers[i]->Clone());
				}
			}
			else if ( name == c_CABD )
			{
				read_CABD(i_Reader, version, size, o_Data.m_BaseData);
			}
			else if ( name == c_CAMI )
			{
				read_CAMI(i_Reader, version, size, o_Data.m_BaseData);
			}
			else if ( name == c_XTRP )
			{
				// Read custom property info
				xtraPropertyDataParser::ReadCustomPropertyData(i_Reader, version, size, o_Data.m_CustomProperties);
			}
			else if ( name == c_DRVS )
			{
				tmlnParser::ReadDrivers(i_Reader, 
										cmraCamerasDataParser::GetChunkName(), 
										(o_Data.m_Drivers));
			}
			else if ( name == tmlnChannelInfoParser::GetChunkName() )
			{
				tmlnChannelInfoParser::ReadChannels(i_Reader, o_Data.m_ChannelInfo,
						c_ChannelNameStrings, c_NumChannelStrings);
			}
			i_Reader.FinishChunk();
		}
	}

	//------------------------------------------------------------------------
	//   WriteCameraData
	//------------------------------------------------------------------------
	void WriteCameraData(	chWriter& o_Writer,
							const cmraScriptData& i_Data )
	{
		const int l_CAMR_VERSION = 4;
		o_Writer.WriteChunkHeader( c_CAMR, l_CAMR_VERSION, true );

		// Writes c_CABD and c_CAMI chunks
		WriteCameraBaseData(o_Writer, i_Data.m_BaseData);

		// Write custom property info
		const int l_XTRP_VERSION = 0;
		const bool c_bXTRP_CONTAINER_CHUNK = true;
		o_Writer.WriteChunkHeader( c_XTRP, l_XTRP_VERSION, c_bXTRP_CONTAINER_CHUNK );
		xtraPropertyDataParser::WriteCustomPropertyData(o_Writer, i_Data.m_CustomProperties);
		o_Writer.FinishChunk();

		const int l_DRVS_VERSION = 1;
		const bool c_bDRVS_CONTAINER_CHUNK = true;
		o_Writer.WriteChunkHeader( c_DRVS, l_DRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );
		tmlnParser::WriteDrivers(o_Writer, 
								 cmraCamerasDataParser::GetChunkName(), 
								 i_Data.m_Drivers);
		o_Writer.FinishChunk();

		// Write channels info
		const int l_CHNI_VERSION = 0;
		o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, false );
		tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
		o_Writer.FinishChunk();

		o_Writer.FinishChunk();
	}

	//------------------------------------------------------------------------
	//   ReadCameraCueFormData
	//------------------------------------------------------------------------
	//void ReadCameraCueFormData(	chReader& i_Reader,
	//							chDefs::Version i_Version,
	//							chDefs::Size i_Size,
	//							cmraCueFormData& o_Data )
	//{
	//	// Not reading the camera names in this version
	//	o_Data.m_CameraNames.clear();
	//
	//	chChunkParserUtil::Read(i_Reader, o_Data.m_Index[0]);
	//	chChunkParserUtil::Read(i_Reader, o_Data.m_Index[1]);
	//	chChunkParserUtil::Read(i_Reader, o_Data.m_Index[2]);
	//	chChunkParserUtil::Read(i_Reader, o_Data.m_Index[3]);
	//
	//	if (i_Version >= 1)
	//	{
	//		int num_keys = 0;
	//		chChunkParserUtil::Read(i_Reader, num_keys);
	//		float time = 0;
	//		int value = 0;
	//		for (int i=0; i<num_keys; i++)
	//		{
	//			chChunkParserUtil::Read(i_Reader, time);
	//			chChunkParserUtil::Read(i_Reader, value);
	//			o_Data.m_Cues.AddKey(time, value);
	//		}
	//	}
	//}


	//------------------------------------------------------------------------
	//   WriteCameraCueFormData
	//------------------------------------------------------------------------
	//void WriteCameraCueFormData(	chWriter& o_Writer,
	//								const cmraCueFormData& i_Data )
	//{
	//	const l_CCFD_VERSION = 1;
	//	o_Writer.WriteChunkHeader( c_CCFD, l_CCFD_VERSION, false );
	//
	//	// I'm not going to read the camera names for now.
	//
	//	chChunkParserUtil::Write(o_Writer, i_Data.m_Index[0]);
	//	chChunkParserUtil::Write(o_Writer, i_Data.m_Index[1]);
	//	chChunkParserUtil::Write(o_Writer, i_Data.m_Index[2]);
	//	chChunkParserUtil::Write(o_Writer, i_Data.m_Index[3]);
	//
	//	int num_keys = i_Data.m_Cues.GetNumKeys();
	//	chChunkParserUtil::Write(o_Writer, num_keys);
	//	float time = 0;
	//	int value = 0;
	//	for (int i=0; i<num_keys; i++)
	//	{
	//		i_Data.m_Cues.GetKeyData(i, time, value);
	//		chChunkParserUtil::Write(o_Writer, time);
	//		chChunkParserUtil::Write(o_Writer, value);
	//	}
	//
	//	o_Writer.FinishChunk();
	//}

}	// local namespace

//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  cmraCamerasDataParser::GetChunkName()
{
	return c_CAMD;
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void cmraCamerasDataParser::ReadData(	chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										cmraCamerasData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_CAMR )
		{
			// Scripted Camera chunk format, contains base info chunk and drivers
			cmraScriptData data;
			ReadCameraData(i_Reader, version, size, data);

			//	add it to the list
			o_Data.m_Items.push_back(data);

			//DBG_LOG("Read camera " << data.m_Name.c_str() );
		}
		else if ( name == c_ECAM )
		{
			// Read camera information for editor camera
			read_ECAM(i_Reader, version, size, o_Data.m_EditorCamera);
		}
		else if ( name == c_INIT )
		{
			// Initial position of camera for editor (last position when saving doc)
			// Old format, replaced with ECAM chunk now to write full camera information
			//	including HDR settings.
			maVector3d target;
			float pitch, yaw, radius;
			chChunkParserUtil::Read(i_Reader, target);
			chChunkParserUtil::Read(i_Reader, pitch);
			chChunkParserUtil::Read(i_Reader, yaw);
			chChunkParserUtil::Read(i_Reader, radius);

			// Set this info into the new full editor camera data structure
			float x = float(sin(yaw) * cos(pitch));
			float y = float(sin(pitch));
			float z = float(cos(yaw) * cos(pitch));
			maVector3d dir(-x, -y, -z);
			maPoint3d pos = target - dir * radius;

			o_Data.m_EditorCamera.m_Position = pos;
			o_Data.m_EditorCamera.m_Target = target;

		}
		//bga -  No longer storing cue form data in SystemCameras, see SystemDirectorsCut
		//else if (name == c_CCFD)
		//{
		//	ReadCameraCueFormData(i_Reader, version, size, o_Data.m_CueForm);
		//}
		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void cmraCamerasDataParser::WriteData(	chWriter& o_Writer,
										const cmraCamerasData& i_Data )
{
	const int c_CAMD_VERSION = 1;
	o_Writer.WriteChunkHeader( c_CAMD, c_CAMD_VERSION, true );

	// Init camera position - writing full editor camera data now in ECAM chunk below
	//const int c_INIT_VERSION = 0;
	//o_Writer.WriteChunkHeader( c_INIT, c_INIT_VERSION, false );
	//i_Data.m_Target.Write(o_Writer);
	//i_Data.m_Pitch.Write(o_Writer);
	//i_Data.m_Yaw.Write(o_Writer);
	//i_Data.m_Radius.Write(o_Writer);
	//o_Writer.FinishChunk();

	// Editor camera data
	const int c_ECAM_VERSION = 0;
	o_Writer.WriteChunkHeader( c_ECAM, c_ECAM_VERSION, false );
	WriteCameraBaseData(o_Writer, i_Data.m_EditorCamera);
	o_Writer.FinishChunk();

	// Write scripted cameras
	for (int i=0; i<i_Data.m_Items.size(); i++)
	{
		WriteCameraData(o_Writer, i_Data.m_Items[i]);
	}

	//bga - No longer storing this data in SystemCameras, see SystemDirectorsCut
	// Cue Form Data
	//WriteCameraCueFormData(o_Writer, i_Data.m_CueForm);

	o_Writer.FinishChunk();
}


