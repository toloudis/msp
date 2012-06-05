/********************************************************************************************\
**  lsetLightSetsDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Systems/LightSets/Data/lsetLightSetsDataParser.hpp"

#include "Support/tmln/tmlnChannelInfoParser.hpp"
#include "Support/tmln/tmlnParser.hpp"
#include "Support/xtra/xtraPropertyDataParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/name/nameMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_LSTS = chDefs::MakeName('L', 'S', 'T', 'S');
	const chDefs::Name c_LSTD = chDefs::MakeName('L', 'S', 'T', 'D');
	const chDefs::Name c_LSTT = chDefs::MakeName('L', 'S', 'T', 'T');
	const chDefs::Name c_AMBT = chDefs::MakeName('A', 'M', 'B', 'T');
	const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');
	const chDefs::Name c_XTRP = chDefs::MakeName('X', 'T', 'R', 'P');
	

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void read_name_string(chReader& i_Reader, nameString& o_Name)
	{
		std::string name;
		nameUID uid;

		chChunkParserUtil::Read(i_Reader, uid);
		chChunkParserUtil::Read(i_Reader, name);

		o_Name.SetString( name );
		o_Name.SetUID( uid );

		//DBG_LOG2("lset READ : %d %s", uid, name.c_str() );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void write_name_string(chWriter& o_Writer, const nameString& i_Name)
	{
		std::string name = i_Name.GetString();
		nameUID uid = i_Name.GetUID();

		chChunkParserUtil::Write(o_Writer, uid);
		chChunkParserUtil::Write(o_Writer, name);

		//DBG_LOG2("lset WRITE: %d %s", uid, name.c_str() );
	}

	//------------------------------------------------------------------------
	//   read_legacy_lightset_data
	//------------------------------------------------------------------------
	void read_legacy_lightset_data(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									ltstLightSetData& o_Data )
	{
		read_name_string(i_Reader, o_Data.m_Name);

		// added ambient light in version 1
		if (i_Version > 0)
		{
			// ambient color dropped.
			maFloatRGBA c;
			chChunkParserUtil::Read(i_Reader, c);
		}

		envType::Int32 num_lights;
		i_Reader.Read(num_lights);
		o_Data.m_Lights.resize(num_lights);
		//DBG_LOG("Lights-------------" << num_lights);
		for (int i=0; i<num_lights; i++)
		{
			read_name_string(i_Reader, o_Data.m_Lights[i]);
			//if (i_Version == 1)
			{
				//	for this version of light sets, set the UID to invalid
				//	because it was saving UIDs that didn't match
				//
				//	TODO: [rjk] remove the nameMgr include + this code.
				//
				//DBG_LOG2("- %d (%s)", o_Data.m_Lights[i].GetUID(), o_Data.m_Lights[i].GetString().c_str() );
				nameUID uid = nameMgr::GetNameUIDFromString(o_Data.m_Lights[i]);
				o_Data.m_Lights[i].SetUID(uid);
				//DBG_LOG2("  %d (%s)", o_Data.m_Lights[i].GetUID(), o_Data.m_Lights[i].GetString().c_str() );
			}
		}

		envType::Int32 num_objects;
		i_Reader.Read(num_objects);
		o_Data.m_Objects.resize(num_objects);
		//DBG_LOG("Objects------------" << num_objects);
		for (int i=0; i<num_objects; i++)
		{
			read_name_string(i_Reader, o_Data.m_Objects[i].m_Name);
			//if (i_Version == 1)
			{
				//	for this version of light sets, set the UID to invalid
				//	because it was saving UIDs that didn't match
				//
				//	TODO: [rjk] remove the nameMgr include + this code.
				//
				nameUID uid = o_Data.m_Objects[i].m_Name.GetUID();
				//DBG_LOG2("- %d (%s)", uid, o_Data.m_Objects[i].m_Name.GetString().c_str() );
				uid = nameMgr::GetNameUIDFromString(o_Data.m_Objects[i].m_Name);
				o_Data.m_Objects[i].m_Name.SetUID(uid);
				//DBG_LOG2("  %d (%s)", o_Data.m_Objects[i].GetUID(), o_Data.m_Objects[i].m_Name.GetString().c_str() );
			}

			// Version 3 adds list of indices of fragments
			// that are individually lit
			if (i_Version >= 3)
			{
				envType::Int32 num_frags, ind;
				i_Reader.Read(num_frags);
				o_Data.m_Objects[i].m_LitFragmentIndices.resize(num_frags);
				for (int f=0; f<num_frags; f++)
				{
					i_Reader.Read(ind);
					o_Data.m_Objects[i].m_LitFragmentIndices[f] = ind;
				}
			}
		}
	}

	//------------------------------------------------------------------------
	//   read_lightset_data
	//------------------------------------------------------------------------
	void read_lightset_data(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								lsetData& o_Data )
	{
		// version 0!
		o_Data.m_Name.Read(i_Reader);
		if (i_Version < 1)
		{
			// ambient color dropped.
			prtyColor c;
			c.Read(i_Reader);
		}

		envType::Int32 num_lights;
		i_Reader.Read(num_lights);
		o_Data.m_Lights.resize(num_lights);
		//DBG_LOG("Lights-------------" << num_lights);
		for (int i=0; i<num_lights; i++)
		{
			read_name_string(i_Reader, o_Data.m_Lights[i]);
		}

		envType::Int32 num_objects;
		i_Reader.Read(num_objects);
		o_Data.m_Objects.resize(num_objects);
		//DBG_LOG("Objects------------" << num_objects);
		for (int i=0; i<num_objects; i++)
		{
			read_name_string(i_Reader, o_Data.m_Objects[i].m_Name);

			// list of indices of fragments that are individually lit
			envType::Int32 num_frags, ind;
			i_Reader.Read(num_frags);
			o_Data.m_Objects[i].m_LitFragmentIndices.resize(num_frags);
			for (int f=0; f<num_frags; f++)
			{
				i_Reader.Read(ind);
				o_Data.m_Objects[i].m_LitFragmentIndices[f] = ind;
			}
		}
	}

	//------------------------------------------------------------------------
	//   write_lightset_data
	//------------------------------------------------------------------------
	void write_lightset_data(chWriter& o_Writer,
							 const lsetData& i_Data)
	{
		const int l_LSTT_VERSION = 1;
		o_Writer.WriteChunkHeader( c_LSTT, l_LSTT_VERSION, false );
		i_Data.m_Name.Write(o_Writer);
		// ambient removed in v1.
		//i_Data.m_AmbientLight.Write(o_Writer);

		const int num_lights = i_Data.m_Lights.size();
		o_Writer.Write(envType::Int32(num_lights));
		for (int i=0; i<num_lights; i++)
		{
			write_name_string(o_Writer,i_Data.m_Lights[i]);
		}

		const int num_objects = i_Data.m_Objects.size();
		o_Writer.Write(envType::Int32(num_objects));
		for (int i=0; i<num_objects; i++)
		{
			const ltstLightSetObjectData &obj_data = i_Data.m_Objects[i];
			write_name_string(o_Writer, obj_data. m_Name);

			const int num_frags = obj_data.m_LitFragmentIndices.size();
			o_Writer.Write(envType::Int32(num_frags));
			for (int f=0; f<num_frags; f++)
			{
				o_Writer.Write(envType::Int32(obj_data.m_LitFragmentIndices[f]));
			}
		}
		o_Writer.FinishChunk();
	}


}	// local namespace

//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  lsetLightSetsDataParser::GetChunkName()
{
	return c_LSTS;
}

//------------------------------------------------------------------------
//   ReadLightSetData
//------------------------------------------------------------------------
void lsetLightSetsDataParser::ReadLightSetData(	chReader& i_Reader,
												chDefs::Version i_Version,
												chDefs::Size i_Size,
												lsetScriptData& o_Data )
{
	// version 4 moved everything to properties and added channels and drivers
	if (i_Version < 4)
	{
		ltstLightSetData data;
		read_legacy_lightset_data(i_Reader, i_Version, i_Size, data);

		o_Data.m_BaseData.m_Name.SetValue(data.m_Name);

		o_Data.m_BaseData.m_Objects = data.m_Objects;
		o_Data.m_BaseData.m_Lights = data.m_Lights;
	}
	else
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;

		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			// Info divided into base chunk and driver list
			if ( name == c_LSTT )
			{
				read_lightset_data(i_Reader, version, size, o_Data.m_BaseData);
			}
			else if ( name == c_XTRP )
			{
				// Read custom property info
				xtraPropertyDataParser::ReadCustomPropertyData(i_Reader, version, size, o_Data.m_CustomProperties);
			}
			else if ( name == c_DRVS )
			{
				tmlnParser::ReadDrivers(i_Reader, 
										lsetLightSetsDataParser::GetChunkName(), 
										o_Data.m_Drivers);
			}
			else if ( name == tmlnChannelInfoParser::GetChunkName() )
			{
				tmlnChannelInfoParser::ReadChannels(i_Reader, o_Data.m_ChannelInfo);
			}

			i_Reader.FinishChunk();
		}
	}
}

//------------------------------------------------------------------------
//   WriteLightSetData
//------------------------------------------------------------------------
void lsetLightSetsDataParser::WriteLightSetData(chWriter& o_Writer,
												const lsetScriptData& i_Data )
{
	// version 4 moved everything to properties and added channels and drivers
	const int c_LSTD_VERSION = 4;
	o_Writer.WriteChunkHeader( c_LSTD, c_LSTD_VERSION, false );

	// Write custom property info
	const int l_XTRP_VERSION = 0;
	const bool c_bXTRP_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_XTRP, l_XTRP_VERSION, c_bXTRP_CONTAINER_CHUNK );
	xtraPropertyDataParser::WriteCustomPropertyData(o_Writer, i_Data.m_CustomProperties);
	o_Writer.FinishChunk();

	// Write drivers info
	const int l_DRVS_VERSION = 0;
	const bool c_bDRVS_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_DRVS, l_DRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );
	tmlnParser::WriteDrivers(o_Writer, 
							lsetLightSetsDataParser::GetChunkName(), 
							i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, false );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();

	// Write light set info
	//
	write_lightset_data(o_Writer, i_Data.m_BaseData);

	o_Writer.FinishChunk();
}


//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void lsetLightSetsDataParser::ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				lsetLightSetsData& o_Data)
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_LSTD )
		{
			lsetScriptData data;
			ReadLightSetData(i_Reader, version, size, data);
			o_Data.m_Items.push_back(data);
		}
		else if ( name == c_AMBT )
		{
			// ambient color dropped.
			maFloatRGBA c;
			chChunkParserUtil::Read(i_Reader, c);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void lsetLightSetsDataParser::WriteData(	chWriter& o_Writer,
				const lsetLightSetsData& i_Data  )
{
	o_Writer.WriteChunkHeader( c_LSTS, 0, true );

	const int num_sets = i_Data.m_Items.size();
	for (int i=0; i < num_sets ; i++)
	{
		WriteLightSetData(o_Writer, i_Data.m_Items[i]);
	}

	o_Writer.FinishChunk();
}

