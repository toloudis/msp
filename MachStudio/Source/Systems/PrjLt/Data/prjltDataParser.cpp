/********************************************************************************************\
**  prjltDataParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/PrjLt/Data/prjltDataParser.hpp"
#include "Systems/PrjLt/GUI/prjltTextureList.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Support/mnm/mnmBaseDataParser.hpp"
#include "Support/tmln/tmlnBaseDataParser.hpp"
#include "Support/tmln/tmlnChannelInfoParser.hpp"
#include "Support/tmln/tmlnParser.hpp"
#include "Support/xtra/xtraPropertyDataParser.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_PRJD = chDefs::MakeName('P', 'R', 'J', 'D');
	const chDefs::Name c_PRJT = chDefs::MakeName('P', 'R', 'J', 'T');
	const chDefs::Name c_PRJC = chDefs::MakeName('P', 'R', 'J', 'C');
	const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');
	const chDefs::Name c_XTRP = chDefs::MakeName('X', 'T', 'R', 'P');
	const chDefs::Name c_PRTX = chDefs::MakeName('P', 'R', 'T', 'X'); //projection texture
	const chDefs::Name c_PSTX = chDefs::MakeName('P', 'S', 'T', 'X'); //projection shaft texture


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

	const int c_NumChannelStrings = 21;


	//--------------------------------------------------------------------
	// Check absolute path and resolve single filenames into fullpaths
	//--------------------------------------------------------------------
	void resolve_fullpath(prtyTextureFileName &io_FilePath)
	{
		fsLocator tex_loc = io_FilePath.GetValue();
		if (tex_loc.GetNumNames() > 0)
		{
			// This comparison is only needed to support old file formats
			// and could be removed in product
			if (tex_loc.GetNumNames() == 1)
			{
				//	get the file path
				//
				fsysFileList file_list;
				prjltTextureList::BuildFileList(file_list);
				itString tex_fname = tex_loc.GetLastName();

				//bga - Old file formats relied on the texture manager
				// promoting "bmp" files to "dds" files. I am just 
				// going to hack in this particular switch that was
				// common because it was the default value in really
				// old files.
				if (tex_fname == itString("Projection.bmp"))
					tex_fname = itString("Projection.dds");

				if (file_list.FindFilePath(tex_fname, tex_loc))
					tex_loc.Push(tex_fname);
			}

			if (fsAbsolutePathMgr::ResolvePath(tex_loc, "Textures"))
			{
				//preserve the callback state
				prtyTextureFileData tex;
				tex.m_TextureLocator = tex_loc;
				tex.m_CurrentCallback = io_FilePath.GetFullValue().m_CurrentCallback;  //preserve callback
				io_FilePath.SetValue(tex);
			}
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void check_falloff(chReader& i_Reader,
					   chDefs::Version i_Version)
	{
		//bga - in version 25, there was a mistake in the file format that expanded
		// the falloff property to a vector4. This was never released in a version
		// to users, but handle it here to help out with programmers for the few days 
		// that the code was at version 8
		if (i_Version == 25)
		{
			envType::Float32 cubic_falloff;
			i_Reader.Read(cubic_falloff);
		}
	}

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
							o_Data.m_TextureFilename.ReadTexture(i_Reader);
							resolve_fullpath(o_Data.m_TextureFilename);	
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

			check_falloff(i_Reader, i_Version); // check for version 25 problems
			o_Data.m_Falloff.Read(i_Reader);
			o_Data.m_Range.Read(i_Reader);
			o_Data.m_Target.Read(i_Reader);
			o_Data.m_Angle.Read(i_Reader);
			o_Data.m_Scale.Read(i_Reader);
			o_Data.m_Aspect.Read(i_Reader);
			o_Data.m_TextureFilename.ReadTexture(i_Reader);
			resolve_fullpath(o_Data.m_TextureFilename);
			o_Data.m_DepthMapSize.Read(i_Reader);

			if ( i_Version > 6 )
			{
				// data added in version 7
				o_Data.m_LightSize.Read(i_Reader);
				if (i_Version < 21)
				{
					// removed in v21.
					o_Data.m_SceneScale.Read(i_Reader);

					// was bool, now is enum.
					// false -> low
					// true -> medium
					prtyBoolean oldShadowQuality;
					oldShadowQuality.Read(i_Reader);
					o_Data.m_ShadowQuality.SetValue(oldShadowQuality.GetValue() ? 1 : 0);
				}
				else
				{
					o_Data.m_ShadowQuality.Read(i_Reader);
				}

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
															o_Data.m_ShaftTextureFilename.ReadTexture(i_Reader);
															resolve_fullpath(o_Data.m_ShaftTextureFilename);
															if (i_Version >= 20)
															{
																o_Data.m_ShadowColor.Read(i_Reader);
																if (i_Version > 21)
																{
																	o_Data.m_PCSSAdjust.Read(i_Reader);

																	if (i_Version > 22)
																	{
																		o_Data.m_bEnabledRamp.Read(i_Reader);
																		o_Data.m_RampData.m_Gradient.Read(i_Reader);
																		o_Data.m_RampData.m_Shape.Read(i_Reader);
																		o_Data.m_RampData.m_Interpolation.Read(i_Reader);

																		if (i_Version > 23)
																		{
																			o_Data.m_RampData.m_TexSize.Read(i_Reader);

																			if (i_Version > 25)
																			{
																				o_Data.m_RampData.m_UWave.Read(i_Reader);
																				o_Data.m_RampData.m_UWaveFreq.Read(i_Reader);
																				o_Data.m_RampData.m_VWave.Read(i_Reader);
																				o_Data.m_RampData.m_VWaveFreq.Read(i_Reader);
																				o_Data.m_RampData.m_Noise.Read(i_Reader);
																				o_Data.m_RampData.m_NoiseFreq.Read(i_Reader);
																				
																				if (i_Version > 26)
																				{
																					o_Data.m_bEnabledShaftRamp.Read(i_Reader);
																					o_Data.m_ShaftRampData.m_Gradient.Read(i_Reader);
																					o_Data.m_ShaftRampData.m_Shape.Read(i_Reader);
																					o_Data.m_ShaftRampData.m_Interpolation.Read(i_Reader);
																					o_Data.m_ShaftRampData.m_TexSize.Read(i_Reader);
																					o_Data.m_ShaftRampData.m_UWave.Read(i_Reader);
																					o_Data.m_ShaftRampData.m_UWaveFreq.Read(i_Reader);
																					o_Data.m_ShaftRampData.m_VWave.Read(i_Reader);
																					o_Data.m_ShaftRampData.m_VWaveFreq.Read(i_Reader);
																					o_Data.m_ShaftRampData.m_Noise.Read(i_Reader);
																					o_Data.m_ShaftRampData.m_NoiseFreq.Read(i_Reader);
																				

																					if (i_Version > 27)
																					{
																						// Properties related to spot and directional lighting
																						envType::Int8 light_type = 0;
																						i_Reader.Read(light_type);
																						if (light_type >= 0 && light_type < prjltData::e_NumLightTypes)
																							o_Data.m_LightType = prjltData::LightType(light_type);
																						
																						o_Data.m_bDirectional.Read(i_Reader);
																						o_Data.m_bConeLighting.Read(i_Reader);
																						o_Data.m_Penumbra.Read(i_Reader);

																						if (i_Version > 28)
																						{
																							o_Data.m_bMRayAreaLight.Read(i_Reader);
																							o_Data.m_MRayAreaLightType.Read(i_Reader);
																							o_Data.m_bMRayAreaLightVisible.Read(i_Reader);
																							if (i_Version > 29)
																							{
																								o_Data.m_MRayAreaLightSampling.Read(i_Reader);

																								if (i_Version > 30) 
																								{
																									o_Data.m_GISource.Read(i_Reader);
																									o_Data.m_HairShadowEnable.Read(i_Reader);
																									o_Data.m_HairShadowSize.Read(i_Reader);
																									o_Data.m_HairShadowType.Read(i_Reader);
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
								}
							}
						}
					}
				}
			}
		}
	}
	
	//------------------------------------------------------------------------
	//   read_texture_data
	//------------------------------------------------------------------------
	void read_texture_data( chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							prjltData& o_Data )
	{
		//get projection map
		o_Data.m_TextureFilename.Read(i_Reader);
		resolve_fullpath(o_Data.m_TextureFilename);
	}

	//------------------------------------------------------------------------
	//   read_shaft_data
	//------------------------------------------------------------------------
	void read_shaft_data( chReader& i_Reader,
						  chDefs::Version i_Version,
						  chDefs::Size i_Size,
						  prjltData& o_Data )
	{
		//get shaft map
		o_Data.m_ShaftTextureFilename.Read(i_Reader);
		resolve_fullpath(o_Data.m_ShaftTextureFilename);
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
		else if ( name == c_XTRP )
		{
			// Read custom property info
			xtraPropertyDataParser::ReadCustomPropertyData(i_Reader, version, size, o_Data.m_CustomProperties);
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
		else if ( name == c_PRTX )
		{
			read_texture_data(i_Reader, version, size, o_Data.m_BaseData);
		}
		else if ( name == c_PSTX )
		{
			read_shaft_data(i_Reader, version, size, o_Data.m_BaseData);
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
	const int l_PRJTITEM_VERSION = 31; 
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
	i_Data.m_BaseData.m_TextureFilename.WriteTexture(o_Writer);
	i_Data.m_BaseData.m_DepthMapSize.Write(o_Writer);

	// version 7
	i_Data.m_BaseData.m_LightSize.Write(o_Writer);
	// v21 removed scenescale and changed the type of shadowquality.
	//i_Data.m_BaseData.m_SceneScale.Write(o_Writer);
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
	i_Data.m_BaseData.m_ShaftTextureFilename.WriteTexture(o_Writer);

	// version 20
	i_Data.m_BaseData.m_ShadowColor.Write(o_Writer);

	// version 21
	i_Data.m_BaseData.m_PCSSAdjust.Write(o_Writer);

	// version 23
	i_Data.m_BaseData.m_bEnabledRamp.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_Gradient.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_Shape.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_Interpolation.Write(o_Writer);

	// version 24
	i_Data.m_BaseData.m_RampData.m_TexSize.Write(o_Writer);

	// version 25 was skipped on purpose

	// version 26
	i_Data.m_BaseData.m_RampData.m_UWave.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_UWaveFreq.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_VWave.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_VWaveFreq.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_Noise.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_NoiseFreq.Write(o_Writer);

	// version 27
	i_Data.m_BaseData.m_bEnabledShaftRamp.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftRampData.m_Gradient.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftRampData.m_Shape.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftRampData.m_Interpolation.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftRampData.m_TexSize.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftRampData.m_UWave.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftRampData.m_UWaveFreq.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftRampData.m_VWave.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftRampData.m_VWaveFreq.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftRampData.m_Noise.Write(o_Writer);
	i_Data.m_BaseData.m_ShaftRampData.m_NoiseFreq.Write(o_Writer);

	// version 28 adds properties related to spot and directional light variations
	envType::Int8 light_type = i_Data.m_BaseData.m_LightType;
	o_Writer.Write(light_type);
	i_Data.m_BaseData.m_bDirectional.Write(o_Writer);
	i_Data.m_BaseData.m_bConeLighting.Write(o_Writer);
	i_Data.m_BaseData.m_Penumbra.Write(o_Writer);

	// version 29 adds mental ray area light params
	i_Data.m_BaseData.m_bMRayAreaLight.Write(o_Writer);
	i_Data.m_BaseData.m_MRayAreaLightType.Write(o_Writer);
	i_Data.m_BaseData.m_bMRayAreaLightVisible.Write(o_Writer);

	// version 30
	i_Data.m_BaseData.m_MRayAreaLightSampling.Write(o_Writer);

	// version 31
	i_Data.m_BaseData.m_GISource.Write(o_Writer);
	i_Data.m_BaseData.m_HairShadowEnable.Write(o_Writer);
	i_Data.m_BaseData.m_HairShadowSize.Write(o_Writer);
	i_Data.m_BaseData.m_HairShadowType.Write(o_Writer);

	o_Writer.FinishChunk();	// c_PRJT

	// Write custom property info
	const int l_XTRP_VERSION = 0;
	const bool c_bXTRP_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_XTRP, l_XTRP_VERSION, c_bXTRP_CONTAINER_CHUNK );
	xtraPropertyDataParser::WriteCustomPropertyData(o_Writer, i_Data.m_CustomProperties);
	o_Writer.FinishChunk();

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

	const int l_PRTX_VERSION = 0;
	const bool c_bPRTX_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_PRTX, l_PRTX_VERSION, c_bPRTX_CONTAINER_CHUNK );

	//write the texture files of the light using the new control methods
	i_Data.m_BaseData.m_TextureFilename.Write(o_Writer);
	o_Writer.FinishChunk(); // c_PRTX

	const int l_PSTX_VERSION = 0;
	const bool c_bPSTX_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_PSTX, l_PSTX_VERSION, c_bPSTX_CONTAINER_CHUNK );

	//write the shaft texture files of the light using the new control methods
	i_Data.m_BaseData.m_ShaftTextureFilename.Write(o_Writer);
	o_Writer.FinishChunk(); // c_PSTX
	
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
