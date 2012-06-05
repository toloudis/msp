/********************************************************************************************\
**  prjltDataParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/PrjLt/Data/prjltDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Support/mnm/mnmBaseDataParser.hpp"
#include "Support/tmln/tmlnBaseDataParser.hpp"
#include "Support/tmln/tmlnChannelInfoParser.hpp"
#include "Support/tmln/tmlnParser.hpp"


namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_PRJD = chDefs::MakeName('P', 'R', 'J', 'D');
const chDefs::Name c_PRJT = chDefs::MakeName('P', 'R', 'J', 'T');
const chDefs::Name c_PRJC = chDefs::MakeName('P', 'R', 'J', 'C');
const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');


//------------------------------------------------------------------------
// Used to support old file formats of channel info. This list
// does not need to be updates as channels are added, it just
// represents the order of the channels when the file format
// for channel info was changed from indexing to name mapping.
//------------------------------------------------------------------------
const char* c_ChannelNameStrings[] = 
{ "Position", "Target", "Enabled", "Color", "Tilt", "Range", "Angle", "Scale",
  "Shadow Source", "Aspect", "Shadow Intensity", "Shaft Visible", "Shaft Alpha", "Edge Softness", 
  "Enable Diffuse", "Enable Specular", "Affects Fur", "Affects Glow", "Falloff", 
  "Shaft Falloff Start", "Shaft Falloff End", "Intensity" };

const int c_NumChannelStrings = 22;

//------------------------------------------------------------------------
//   read_base_data
//------------------------------------------------------------------------
void read_base_data(chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					prjltData& o_Data )
{
	if ( i_Version <= 5 )
	{
		//	older versions (1 to 5)
		o_Data.m_Color.Read(i_Reader);
		o_Data.m_Enabled.Read(i_Reader);
		o_Data.m_ShadowSource.Read(i_Reader);

		// all previous versions of projected lights are really shadow casters
		o_Data.m_ShadowSource = true;

		o_Data.m_Position.Read(i_Reader);
		o_Data.m_Falloff.Read(i_Reader);
		o_Data.m_Range.Read(i_Reader);
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

				if (i_Version >= 3)
				{
					if (i_Version < 5)
					{
						maVector3d direction;
						chChunkParserUtil::Read(i_Reader, direction);
						o_Data.m_Target.SetValue( (o_Data.m_Position.GetValue() + direction * 10.0f) );
					}
					else 
					{
						// Version 5 starts writing "Target" instead of
						// "Direction"
						o_Data.m_Target.Read(i_Reader);
					}
					o_Data.m_Angle.Read(i_Reader);
					o_Data.m_Scale.Read(i_Reader);
					o_Data.m_Aspect.Read(i_Reader);

					if (i_Version >=4)
					{
						o_Data.m_TextureFilename.Read(i_Reader);
						o_Data.m_DepthMapSize.Read(i_Reader);
					}
				}
			}
		}
	}
	else
	{
		//	version 6
		o_Data.m_Enabled.Read(i_Reader);
		o_Data.m_Color.Read(i_Reader);
		o_Data.m_ShadowSource.Read(i_Reader);

		if (i_Version < 10)
		{
			// all previous versions of projected lights are really shadow casters
			o_Data.m_ShadowSource.SetValue(true);
		}

		o_Data.m_Falloff.Read(i_Reader);
		o_Data.m_Range.Read(i_Reader);
		o_Data.m_Target.Read(i_Reader);
		o_Data.m_Angle.Read(i_Reader);
		o_Data.m_Scale.Read(i_Reader);
		o_Data.m_Aspect.Read(i_Reader);
		o_Data.m_TextureFilename.Read(i_Reader);
		o_Data.m_DepthMapSize.Read(i_Reader);

		if ( i_Version > 6 )
		{
			// data added in version 7
			o_Data.m_LightSize.Read(i_Reader);
			o_Data.m_SceneScale.Read(i_Reader);
			o_Data.m_ShadowQuality.Read(i_Reader);

			if ( i_Version > 7 )
			{
				o_Data.m_Name.Read(i_Reader);
				
				//DBG_LOG2( "Reading prjlt %d %s", o_Data.m_Name.GetUID(), o_Data.m_Name.GetString().c_str() );

				o_Data.m_bEditorVisible.Read(i_Reader);
				o_Data.m_Position.Read(i_Reader);
				o_Data.m_Orientation.Read(i_Reader);

				if ( i_Version >= 9 )
				{
					o_Data.m_bDiffuseEnabled.Read(i_Reader);
					o_Data.m_bSpecularEnabled.Read(i_Reader);

					if (i_Version >= 11)
					{
						o_Data.m_bShaftVisible.Read(i_Reader);

						if (i_Version >= 12)
						{
							o_Data.m_ShaftAlpha.Read(i_Reader);
							o_Data.m_ShaftDensity.Read(i_Reader);
							o_Data.m_ShaftDistFalloffStart.Read(i_Reader);
							o_Data.m_ShaftDistFalloffEnd.Read(i_Reader);
							
							if (i_Version >= 13)
							{
								o_Data.m_Tilt.Read(i_Reader);

								if (i_Version >= 14)
								{
									o_Data.m_bAffectsFur.Read(i_Reader);

									if (i_Version >= 15)
									{
										o_Data.m_ShadowIntensity.Read(i_Reader);

										if (i_Version >= 16)
										{
											o_Data.m_bAffectsGlow.Read(i_Reader);

											if (i_Version >= 17)
											{
												o_Data.m_Intensity.Read(i_Reader);

												if (i_Version >= 18)
												{
													o_Data.m_DepthBias.Read(i_Reader);
													if (i_Version >= 19)
													{
														o_Data.m_ShaftTextureFilename.Read(i_Reader);
														if (i_Version >= 20)
														{
															o_Data.m_ShadowColor.Read(i_Reader);
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

}


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  prjltDataParser::GetChunkName()
{
	return c_PRJD;
}

//------------------------------------------------------------------------
//   ReadProjectedLightData
//------------------------------------------------------------------------
void prjltDataParser::ReadProjectedLightData(	chReader& i_Reader,
												chDefs::Version i_Version,
												chDefs::Size i_Size,
												prjltScriptData& o_Data )
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
				prjltDataParser::GetChunkName(),
				version, size, bdata);
			o_Data.m_BaseData.m_Name.SetValue(bdata.m_Name);
			//DBG_LOG2( "Reading prjlt+%d %s", o_Data.m_BaseData.m_Name.GetUID(), o_Data.m_BaseData.m_Name.GetString().c_str() );
			o_Data.m_BaseData.m_bEditorVisible.SetValue(bdata.m_bEditorVisible);
			o_Data.m_BaseData.m_Position.SetValue(bdata.m_Position);
			o_Data.m_BaseData.m_Orientation.SetQuaternion(bdata.m_Orientation);

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
				prjltDataParser::GetChunkName(),
				version, size, tmln_data);

			envSTLHelpers::DeleteContainer(o_Data.m_Drivers);

			int num_drivers = tmln_data.m_Drivers.size();
			for (int i=0; i<num_drivers; i++)
			{
				o_Data.m_Drivers.push_back(tmln_data.m_Drivers[i]->Clone());
			}
		}
		else if ( name == c_PRJT )
		{
			read_base_data(i_Reader, version, size, o_Data.m_BaseData);
		}
		else if ( name == c_DRVS )
		{
			tmlnParser::ReadDrivers(i_Reader, 
				prjltDataParser::GetChunkName(),
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
//   WriteProjectedLightData
//------------------------------------------------------------------------
void prjltDataParser::WriteProjectedLightData(	chWriter& o_Writer,
												const prjltScriptData& i_Data )
{
	const int l_PRJCCHNK_VERSION = 4;
	o_Writer.WriteChunkHeader( c_PRJC, l_PRJCCHNK_VERSION, true );

	// Write base info
	const int l_PRJTITEM_VERSION = 20;
	o_Writer.WriteChunkHeader( c_PRJT, l_PRJTITEM_VERSION, false );

	i_Data.m_BaseData.m_Enabled.Write(o_Writer);
	i_Data.m_BaseData.m_Color.Write(o_Writer);
	i_Data.m_BaseData.m_ShadowSource.Write(o_Writer);
	i_Data.m_BaseData.m_Falloff.Write(o_Writer);
	i_Data.m_BaseData.m_Range.Write(o_Writer);

	// version 3
	i_Data.m_BaseData.m_Target.Write(o_Writer);
	//i_Data.m_BaseData.m_Direction.Write(o_Writer); // version 5 replaces Target for Direction
	i_Data.m_BaseData.m_Angle.Write(o_Writer);
	i_Data.m_BaseData.m_Scale.Write(o_Writer);
	i_Data.m_BaseData.m_Aspect.Write(o_Writer);

	// version 4
	i_Data.m_BaseData.m_TextureFilename.Write(o_Writer);
	i_Data.m_BaseData.m_DepthMapSize.Write(o_Writer);

	// version 7
	i_Data.m_BaseData.m_LightSize.Write(o_Writer);
	i_Data.m_BaseData.m_SceneScale.Write(o_Writer);
	i_Data.m_BaseData.m_ShadowQuality.Write(o_Writer);

	// version 8
	i_Data.m_BaseData.m_Name.Write(o_Writer);
	//DBG_LOG2( "Writing prjlt %d %s", i_Data.m_BaseData.m_Name.GetUID(), i_Data.m_BaseData.m_Name.GetString().c_str() );
	i_Data.m_BaseData.m_bEditorVisible.Write(o_Writer);
	i_Data.m_BaseData.m_Position.Write(o_Writer);
	i_Data.m_BaseData.m_Orientation.Write(o_Writer);

	// version 9
	i_Data.m_BaseData.m_bDiffuseEnabled.Write(o_Writer);
	i_Data.m_BaseData.m_bSpecularEnabled.Write(o_Writer);

	// version 11
	i_Data.m_BaseData.m_bShaftVisible.Write(o_Writer);

	// version 12
	i_Data.m_BaseData.m_ShaftAlpha.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftDensity.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftDistFalloffStart.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftDistFalloffEnd.Write(o_Writer);

	// version 13
	i_Data.m_BaseData.m_Tilt.Write(o_Writer);

	// version 14
	i_Data.m_BaseData.m_bAffectsFur.Write(o_Writer);

	// version 15
	i_Data.m_BaseData.m_ShadowIntensity.Write(o_Writer);

	// version 16
	i_Data.m_BaseData.m_bAffectsGlow.Write(o_Writer);

	// version 17
	i_Data.m_BaseData.m_Intensity.Write(o_Writer);

	// version 18
	i_Data.m_BaseData.m_DepthBias.Write(o_Writer);

	// version 19
	i_Data.m_BaseData.m_ShaftTextureFilename.Write(o_Writer);

	// version 20
	i_Data.m_BaseData.m_ShadowColor.Write(o_Writer);

	o_Writer.FinishChunk();	// c_PRJT

	// Write drivers
	const int l_DRVS_VERSION = 0;
	const bool c_bDRVS_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_DRVS, l_DRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );
	tmlnParser::WriteDrivers(o_Writer, 
				prjltDataParser::GetChunkName(),
				i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, false );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();	// c_PRJC
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void prjltDataParser::ReadData(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								prjltProjectLightsData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_PRJC )
		{
			// New chunk format, contains base info chunk and drivers
			prjltScriptData data;
			ReadProjectedLightData(i_Reader, version, size, data);
			o_Data.m_Items.push_back(data);
		}
		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void prjltDataParser::WriteData(chWriter& o_Writer,
								const prjltProjectLightsData& i_Data )
{
	const int l_PRJTLIST_VERSION = 1;
	o_Writer.WriteChunkHeader( c_PRJD, l_PRJTLIST_VERSION, true );

	for (int i=0; i<i_Data.m_Items.size(); i++)
	{
		WriteProjectedLightData(o_Writer, i_Data.m_Items[i]);
	}
	o_Writer.FinishChunk();
}
