/********************************************************************************************\
**  prtclDataParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Particles/Data/prtclDataParser.hpp"

#include "Support/tmln/tmlnParser.hpp"

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
#include "Core/prty/prtyFilePath.hpp"
#include "Support/tmln/tmlnBaseDataParser.hpp"
#include "Support/tmln/tmlnChannelInfoParser.hpp"



namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_PRTCLL = chDefs::MakeName('P', 'R', 'T', 'L');	// prtcl list
const chDefs::Name c_PRTCLC = chDefs::MakeName('P', 'R', 'T', 'C');	// +-prtcl chunk
const chDefs::Name c_PRTCLD = chDefs::MakeName('P', 'R', 'T', 'D');	//	 +-prtcl data (part of chunk)
const chDefs::Name c_PDRVS  = chDefs::MakeName('D', 'R', 'V', 'S');	//	 +-prtcl drivers (part of chunk)
const chDefs::Name c_PPBSE  = chDefs::MakeName('P', 'B', 'S', 'E');	//	 +-prtcl base data
const chDefs::Name c_PSPRT  = chDefs::MakeName('S', 'P', 'R', 'T');	//	 +-prtcl spirte data
const chDefs::Name c_PCONE  = chDefs::MakeName('C', 'O', 'N', 'E');	//	 +-prtcl cone data
const chDefs::Name c_PSPRL  = chDefs::MakeName('S', 'P', 'R', 'L');	//	 +-prtcl spiral data
const chDefs::Name c_PTXTR  = chDefs::MakeName('T', 'X', 'T', 'R');	//	 +-prtcl texture data
const chDefs::Name c_PEMIT  = chDefs::MakeName('E', 'M', 'I', 't');	//	 +-prtcl emitter data
const chDefs::Name c_PSTRK  = chDefs::MakeName('S', 'T', 'R', 'K');	//	 +-prtcl streak data
const chDefs::Name c_PRNDR  = chDefs::MakeName('R', 'N', 'D', 'R');	//	 +-prtcl rendering data


//------------------------------------------------------------------------
// Used to support old file formats of channel info. This list
// does not need to be updated as channels are added, it just
// represents the order of the channels when the file format
// for channel info was changed from indexing to name mapping.
//------------------------------------------------------------------------
const char* c_ConeChannelNameStrings[] = 
{ "Emit", "Animation", "Position", "Orientation", "Rate", "Max Particles",
 "Lifetime Min", "Lifetime Max", "Scale Start","Scale Coefficient","Start Angle Min", "Start Angle Max", 
"Angular Velocity Min", "Angular Velocity Max", "Angular Acceleration Min", "Angular Acceleration Max", 
"Emitter Scale", "TextureAlphaStart", "TextureAlphaMiddle", "TextureAlphaEnd", 
"TextureAlphaMiddlePercentStart", "TextureAlphaMiddlePercentEnd", 
"ConeAngle", "MinSpeed","MaxSpeed", "AccelerationX", "AccelerationY", "AccelerationZ", 
};
const int c_NumConeChannelStrings = 28;

const char* c_SpiralChannelNameStrings[] = 
{ "Emit", "Animation", "Position", "Orientation", "Rate", "Max Particles",
 "Lifetime Min", "Lifetime Max", "Scale Start","Scale Coefficient","Start Angle Min", "Start Angle Max", 
"Angular Velocity Min", "Angular Velocity Max", "Angular Acceleration Min", "Angular Acceleration Max", 
"Emitter Scale", "TextureAlphaStart", "TextureAlphaMiddle", "TextureAlphaEnd", 
"TextureAlphaMiddlePercentStart", "TextureAlphaMiddlePercentEnd", 
"MinEmitSpeed", "MaxEmitSpeed", "EmitDirectionX", "EmitDirectionY", "EmitDirectionZ", 
"MinRotStartAngle", "MaxRotStartAngle","MinRotAngularVel", "MaxRotAngularVel", "RotRadius", 
"RotRadiusScaleRate", "AccelerationX", "AccelerationY", "AccelerationZ"
};
const int c_NumSpiralChannelStrings = 36;

}


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  prtclDataParser::GetChunkName()
{
	return c_PRTCLL;
}

//------------------------------------------------------------------------
//   ReadParticleChunk
//------------------------------------------------------------------------
void prtclDataParser::ReadParticleChunk(chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										prtclScriptData& o_Data )
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
							 prtclDataParser::GetChunkName(), 
							 version, size, bdata);
			o_Data.m_BaseData.m_Name			= bdata.m_Name;
			o_Data.m_BaseData.m_Filename		= bdata.m_Filename;
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
							 prtclDataParser::GetChunkName(), 
							 version, size, tmln_data);

			envSTLHelpers::DeleteContainer(o_Data.m_Drivers);

			int num_drivers = tmln_data.m_Drivers.size();
			for (int i=0; i<num_drivers; i++)
			{
				o_Data.m_Drivers.push_back(tmln_data.m_Drivers[i]->Clone());
			}
		}
		else if ( name == c_PRTCLD )
		{
			if ( version < 2 )
			{
				if ( version >= 0 )
				{
					std::string name;
					chChunkParserUtil::Read(i_Reader, name);
					o_Data.m_BaseData.m_Name.SetValue( name );

					if (version >= 1)
					{
						nameUID uid;
						chChunkParserUtil::Read(i_Reader, uid);
						o_Data.m_BaseData.m_Name.SetUID( uid );
					}
				}
				o_Data.m_BaseData.m_Filename.Read(i_Reader);
				o_Data.m_BaseData.m_Position.Read(i_Reader);
				o_Data.m_BaseData.m_Orientation.Read(i_Reader);
			}
			else
			{
				o_Data.m_BaseData.m_Name.Read(i_Reader);
				o_Data.m_BaseData.m_Filename.Read(i_Reader);
				o_Data.m_BaseData.m_bEditorVisible.Read(i_Reader);
				o_Data.m_BaseData.m_Position.Read(i_Reader);
				o_Data.m_BaseData.m_Orientation.Read(i_Reader);

				// HACK: [rjk] oh this hurts.  because some files wrote out invalid IDs
				//	this hack will find an ID "up higher" so they don't get assigned an
				//	ID of something that hasn't been loaded yet (but has that ID).
				//	This is temporary until the "old" scenes are re-saved.
				if ( version < 3 )
				{
					if ( o_Data.m_BaseData.m_Name.GetUID() == nameString::e_InvalidUID )
					{
						o_Data.m_BaseData.m_Name.RegisterName();
						if (o_Data.m_BaseData.m_Name.GetUID() < 150)		// if it generated a low UID, force it higher.  all others will be above this one
						{
							o_Data.m_BaseData.m_Name.SetUID(150);
							o_Data.m_BaseData.m_Name.RegisterName();
						}
					}
				}
			}
		}
		else if ( name == c_PPBSE )
		{
			o_Data.m_BaseData.m_Rate.Read(i_Reader);					// Base Generator
			o_Data.m_BaseData.m_MaxParticles.Read(i_Reader);
			o_Data.m_BaseData.m_LifetimeMin.Read(i_Reader);
			o_Data.m_BaseData.m_LifetimeMax.Read(i_Reader);
			if (version < 1)
			{
				// bug fix: version zero had a field added without incrementing the version.
				// check the chunk size to know whether to read the new field.
				if (size > 16)
				{
					o_Data.m_BaseData.m_PreSimTime.Read(i_Reader);
				}
			}
			else
			{
				o_Data.m_BaseData.m_PreSimTime.Read(i_Reader);
			}
		}
		else if ( name == c_PSPRT )
		{
			o_Data.m_BaseData.m_ScaleStart.Read(i_Reader);			// Sprite Particles
			o_Data.m_BaseData.m_ScaleCoefficient.Read(i_Reader);
			o_Data.m_BaseData.m_StartAngleMin.Read(i_Reader);
			o_Data.m_BaseData.m_StartAngleMax.Read(i_Reader);
			o_Data.m_BaseData.m_AngularVelocityMin.Read(i_Reader);
			o_Data.m_BaseData.m_AngularVelocityMax.Read(i_Reader);
			o_Data.m_BaseData.m_AngularAccelerationMin.Read(i_Reader);
			o_Data.m_BaseData.m_AngularAccelerationMax.Read(i_Reader);
			if (version > 0)
			{
				o_Data.m_BaseData.m_ScaleMode.Read(i_Reader);
			}
		}
		else if ( name == c_PSTRK )
		{
			o_Data.m_BaseData.m_bRenderStreaks.Read(i_Reader);			// Render Streaking
			o_Data.m_BaseData.m_StreakLength.Read(i_Reader);
			o_Data.m_BaseData.m_StreakTaper.Read(i_Reader);
			o_Data.m_BaseData.m_StreakFade.Read(i_Reader);

			//DBG_WARNING4("Read Streaks: %d %f %f %f", o_Data.m_BaseData.m_bRenderStreaks.GetValue(),
			//	o_Data.m_BaseData.m_StreakLength.GetValue(), 
			//	o_Data.m_BaseData.m_StreakTaper.GetValue(), 
			//	o_Data.m_BaseData.m_StreakFade.GetValue());
		}
		else if ( name == c_PRNDR )
		{
			o_Data.m_BaseData.m_bShowInCubeReflections.Read(i_Reader);			// Particle Rendering
			o_Data.m_BaseData.m_bShowInPlanarReflections.Read(i_Reader);
			o_Data.m_BaseData.m_bCastShadows.Read(i_Reader);
			o_Data.m_BaseData.m_bUseDitheredShadows.Read(i_Reader);
			o_Data.m_BaseData.m_ShadowDitherBias.Read(i_Reader);
			o_Data.m_BaseData.m_bAdditive.Read(i_Reader);
		}
		else if ( name == c_PEMIT )
		{
			o_Data.m_BaseData.m_EmitterScale.Read(i_Reader);			// Emitter
		}
		else if ( name == c_PCONE )
		{
			o_Data.m_BaseData.m_ConeAngle.Read(i_Reader);		// prtConeParticleGenerator
			o_Data.m_BaseData.m_MinSpeed.Read(i_Reader);
			o_Data.m_BaseData.m_MaxSpeed.Read(i_Reader);
			o_Data.m_BaseData.m_AccelerationX.Read(i_Reader);
			o_Data.m_BaseData.m_AccelerationY.Read(i_Reader);
			o_Data.m_BaseData.m_AccelerationZ.Read(i_Reader);
		}
		else if ( name == c_PSPRL )
		{
			o_Data.m_BaseData.m_MinEmitSpeed.Read(i_Reader);		// prtSpiralParticleGenerator
			o_Data.m_BaseData.m_MaxEmitSpeed.Read(i_Reader);
			o_Data.m_BaseData.m_EmitDirectionX.Read(i_Reader);
			o_Data.m_BaseData.m_EmitDirectionY.Read(i_Reader);
			o_Data.m_BaseData.m_EmitDirectionZ.Read(i_Reader);
			o_Data.m_BaseData.m_MinRotStartAngle.Read(i_Reader);
			o_Data.m_BaseData.m_MaxRotStartAngle.Read(i_Reader);
			o_Data.m_BaseData.m_MinRotAngularVel.Read(i_Reader);
			o_Data.m_BaseData.m_MaxRotAngularVel.Read(i_Reader);
			o_Data.m_BaseData.m_RotRadius.Read(i_Reader);
			o_Data.m_BaseData.m_RotRadiusScaleRate.Read(i_Reader);
			//prtyFloat		m_AccelerationX;
			//prtyFloat		m_AccelerationY;
			//prtyFloat		m_AccelerationZ;
		}
		else if ( name == c_PTXTR )
		{
			o_Data.m_BaseData.m_TextureAlphaStart.Read(i_Reader);		// Texture
			o_Data.m_BaseData.m_TextureAlphaMiddle.Read(i_Reader);
			o_Data.m_BaseData.m_TextureAlphaEnd.Read(i_Reader);
			o_Data.m_BaseData.m_TextureAlphaMiddlePercentStart.Read(i_Reader);
			o_Data.m_BaseData.m_TextureAlphaMiddlePercentEnd.Read(i_Reader);

			// the old version stored the whole path, the new version stores on the filename
			if (version < 3)
			{
				prtyFilePath txtfull;
				txtfull.Read(i_Reader);
				o_Data.m_BaseData.m_TextureFilename.SetValue( txtfull.GetValue().GetLastName() );
			}
			else
			{
				o_Data.m_BaseData.m_TextureFilename.Read(i_Reader);
			}

			if (version > 0)
			{
				o_Data.m_BaseData.m_TextureRows.Read(i_Reader);
				o_Data.m_BaseData.m_TextureCols.Read(i_Reader);
				o_Data.m_BaseData.m_bTextureLooping.Read(i_Reader);
				o_Data.m_BaseData.m_bTextureReverse.Read(i_Reader);
				o_Data.m_BaseData.m_TextureRate.Read(i_Reader);
				if (version > 1)
				{
					o_Data.m_BaseData.m_TextureUVAMode.Read(i_Reader);
				}
			}
		}
		else if ( name == c_PDRVS )
		{
			tmlnParser::ReadDrivers(i_Reader, 
									prtclDataParser::GetChunkName(), 
									(o_Data.m_Drivers));
		}
		else if ( name == tmlnChannelInfoParser::GetChunkName() )
		{
			// To truly support the old file format, we would need to know which 
			// type of particle generator we are using. But, it would be too hard
			// to try to get this info here to support an old file format for channels
			// that probably weren't ever animated or locked. 
			//
			//if (gen_type == prtParticleGeneratorTemplate::e_Spiral)
			//{
			//	tmlnChannelInfoParser::ReadChannels(i_Reader, o_Data.m_ChannelInfo,
			//			c_SpiralChannelNameStrings, c_NumSpiralChannelStrings);
			//}
			//else 
			//{
				tmlnChannelInfoParser::ReadChannels(i_Reader, o_Data.m_ChannelInfo,
						c_ConeChannelNameStrings, c_NumConeChannelStrings);
			//}
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteParticleChunk
//------------------------------------------------------------------------
void prtclDataParser::WriteParticleChunk(chWriter& o_Writer,
						const prtclScriptData& i_Data )
{
	const int l_PRTCLCHUNK_VERSION = 4;
	o_Writer.WriteChunkHeader( c_PRTCLC, l_PRTCLCHUNK_VERSION, true );

	const int l_PRTCLD_VERSION = 3;
	o_Writer.WriteChunkHeader( c_PRTCLD, l_PRTCLD_VERSION, false );
	i_Data.m_BaseData.m_Name.Write(o_Writer);
	i_Data.m_BaseData.m_Filename.Write(o_Writer);
	i_Data.m_BaseData.m_bEditorVisible.Write(o_Writer);
	i_Data.m_BaseData.m_Position.Write(o_Writer);
	i_Data.m_BaseData.m_Orientation.Write(o_Writer);
	o_Writer.FinishChunk();

	const int l_PPBSE_VERSION = 1;
	o_Writer.WriteChunkHeader( c_PPBSE, l_PPBSE_VERSION, false );
	i_Data.m_BaseData.m_Rate.Write(o_Writer);					// Base Generator
	i_Data.m_BaseData.m_MaxParticles.Write(o_Writer);
	i_Data.m_BaseData.m_LifetimeMin.Write(o_Writer);
	i_Data.m_BaseData.m_LifetimeMax.Write(o_Writer);
	i_Data.m_BaseData.m_PreSimTime.Write(o_Writer);
	o_Writer.FinishChunk();

	const int l_PSPRT_VERSION = 1;
	o_Writer.WriteChunkHeader( c_PSPRT, l_PSPRT_VERSION, false );
	i_Data.m_BaseData.m_ScaleStart.Write(o_Writer);			// Sprite Particles
	i_Data.m_BaseData.m_ScaleCoefficient.Write(o_Writer);
	i_Data.m_BaseData.m_StartAngleMin.Write(o_Writer);
	i_Data.m_BaseData.m_StartAngleMax.Write(o_Writer);
	i_Data.m_BaseData.m_AngularVelocityMin.Write(o_Writer);
	i_Data.m_BaseData.m_AngularVelocityMax.Write(o_Writer);
	i_Data.m_BaseData.m_AngularAccelerationMin.Write(o_Writer);
	i_Data.m_BaseData.m_AngularAccelerationMax.Write(o_Writer);
	// added in version 1
	i_Data.m_BaseData.m_ScaleMode.Write(o_Writer);
	o_Writer.FinishChunk();
	
	const int l_PSTRK_VERSION = 0;
	o_Writer.WriteChunkHeader( c_PSTRK, l_PSTRK_VERSION, false );
	i_Data.m_BaseData.m_bRenderStreaks.Write(o_Writer);			// Render Streaking
	i_Data.m_BaseData.m_StreakLength.Write(o_Writer);
	i_Data.m_BaseData.m_StreakTaper.Write(o_Writer);
	i_Data.m_BaseData.m_StreakFade.Write(o_Writer);

	DBG_WARNING4("Write Streaks: %d %f %f %f", i_Data.m_BaseData.m_bRenderStreaks.GetValue(),
		i_Data.m_BaseData.m_StreakLength.GetValue(), 
		i_Data.m_BaseData.m_StreakTaper.GetValue(), 
		i_Data.m_BaseData.m_StreakFade.GetValue());

	o_Writer.FinishChunk();

	const int l_PRNDR_VERSION = 0;
	o_Writer.WriteChunkHeader( c_PRNDR, l_PRNDR_VERSION, false );
	i_Data.m_BaseData.m_bShowInCubeReflections.Write(o_Writer);			// Render Streaking
	i_Data.m_BaseData.m_bShowInPlanarReflections.Write(o_Writer);
	i_Data.m_BaseData.m_bCastShadows.Write(o_Writer);
	i_Data.m_BaseData.m_bUseDitheredShadows.Write(o_Writer);
	i_Data.m_BaseData.m_ShadowDitherBias.Write(o_Writer);
	i_Data.m_BaseData.m_bAdditive.Write(o_Writer);
	o_Writer.FinishChunk();

	const int l_PEMIT_VERSION = 0;
	o_Writer.WriteChunkHeader( c_PEMIT, l_PEMIT_VERSION, false );
	i_Data.m_BaseData.m_EmitterScale.Write(o_Writer);			// Emitter
	o_Writer.FinishChunk();

	const int l_PCONE_VERSION = 0;
	o_Writer.WriteChunkHeader( c_PCONE, l_PCONE_VERSION, false );
	i_Data.m_BaseData.m_ConeAngle.Write(o_Writer);		// prtConeParticleGenerator
	i_Data.m_BaseData.m_MinSpeed.Write(o_Writer);
	i_Data.m_BaseData.m_MaxSpeed.Write(o_Writer);
	i_Data.m_BaseData.m_AccelerationX.Write(o_Writer);
	i_Data.m_BaseData.m_AccelerationY.Write(o_Writer);
	i_Data.m_BaseData.m_AccelerationZ.Write(o_Writer);
	o_Writer.FinishChunk();

	const int l_PSPRL_VERSION = 0;
	o_Writer.WriteChunkHeader( c_PSPRL, l_PSPRL_VERSION, false );
	i_Data.m_BaseData.m_MinEmitSpeed.Write(o_Writer);		// prtSpiralParticleGenerator
	i_Data.m_BaseData.m_MaxEmitSpeed.Write(o_Writer);
	i_Data.m_BaseData.m_EmitDirectionX.Write(o_Writer);
	i_Data.m_BaseData.m_EmitDirectionY.Write(o_Writer);
	i_Data.m_BaseData.m_EmitDirectionZ.Write(o_Writer);
	i_Data.m_BaseData.m_MinRotStartAngle.Write(o_Writer);
	i_Data.m_BaseData.m_MaxRotStartAngle.Write(o_Writer);
	i_Data.m_BaseData.m_MinRotAngularVel.Write(o_Writer);
	i_Data.m_BaseData.m_MaxRotAngularVel.Write(o_Writer);
	i_Data.m_BaseData.m_RotRadius.Write(o_Writer);
	i_Data.m_BaseData.m_RotRadiusScaleRate.Write(o_Writer);
	//prtyFloat		m_AccelerationX;
	//prtyFloat		m_AccelerationY;
	//prtyFloat		m_AccelerationZ;
	o_Writer.FinishChunk();

	const int l_PTXTR_VERSION = 3;
	o_Writer.WriteChunkHeader( c_PTXTR, l_PTXTR_VERSION, false );
	i_Data.m_BaseData.m_TextureAlphaStart.Write(o_Writer);		// Texture
	i_Data.m_BaseData.m_TextureAlphaMiddle.Write(o_Writer);
	i_Data.m_BaseData.m_TextureAlphaEnd.Write(o_Writer);
	i_Data.m_BaseData.m_TextureAlphaMiddlePercentStart.Write(o_Writer);
	i_Data.m_BaseData.m_TextureAlphaMiddlePercentEnd.Write(o_Writer);
	i_Data.m_BaseData.m_TextureFilename.Write(o_Writer);
	i_Data.m_BaseData.m_TextureRows.Write(o_Writer);
	i_Data.m_BaseData.m_TextureCols.Write(o_Writer);
	i_Data.m_BaseData.m_bTextureLooping.Write(o_Writer);
	i_Data.m_BaseData.m_bTextureReverse.Write(o_Writer);
	i_Data.m_BaseData.m_TextureRate.Write(o_Writer);
	// added in v2:
	i_Data.m_BaseData.m_TextureUVAMode.Write(o_Writer);
	o_Writer.FinishChunk();

	const int l_PDRVS_VERSION = 2;
	const bool c_bDRVS_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_PDRVS, l_PDRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );
	tmlnParser::WriteDrivers(o_Writer, 
							 prtclDataParser::GetChunkName(), 
							 i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, true );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void prtclDataParser::ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				prtclParticlesData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_PRTCLC )
		{
			prtclScriptData data;
			ReadParticleChunk(i_Reader, version, size, data);
			o_Data.m_Items.push_back(data);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void prtclDataParser::WriteData(	chWriter& o_Writer,
				const prtclParticlesData& i_Data )
{
	const int l_PRTCLLIST_VERSION		= 1;
	o_Writer.WriteChunkHeader( c_PRTCLL, l_PRTCLLIST_VERSION, true );

	int num_prtcls = i_Data.m_Items.size();
	for (int i=0; i < num_prtcls ; i++)
	{
		WriteParticleChunk(o_Writer, i_Data.m_Items[i]);
	}

	o_Writer.FinishChunk();
}


