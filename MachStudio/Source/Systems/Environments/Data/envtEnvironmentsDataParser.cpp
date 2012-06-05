/********************************************************************************************\
**  envtEnvironmentsDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Environments/Data/envtEnvironmentsDataParser.hpp"

#include "Support/tmln/tmlnChannelInfoParser.hpp"
#include "Support/tmln/tmlnParser.hpp"
#include "Support/xtra/xtraPropertyDataParser.hpp"
#include "Systems/Environments/GUI/envtTextureList.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/name/nameMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_ENVS = chDefs::MakeName('E', 'N', 'V', 'S');
	const chDefs::Name c_ENVD = chDefs::MakeName('E', 'N', 'V', 'D');
	const chDefs::Name c_ENVT = chDefs::MakeName('E', 'N', 'V', 'T');
	const chDefs::Name c_ENVB = chDefs::MakeName('E', 'N', 'V', 'B');
	const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');
	const chDefs::Name c_XTRP = chDefs::MakeName('X', 'T', 'R', 'P');
	const chDefs::Name c_EDTX = chDefs::MakeName('E', 'D', 'T', 'X');  // diffuse texture
	const chDefs::Name c_ESTX = chDefs::MakeName('E', 'S', 'T', 'X');  // specular texture
	const chDefs::Name c_ENVM = chDefs::MakeName('E', 'N', 'V', 'M');  // software lighting


	//------------------------------------------------------------------------
	// Used to support old file formats of channel info. This list
	// does not need to be updates as channels are added, it just
	// represents the order of the channels when the file format
	// for channel info was changed from indexing to name mapping.
	//------------------------------------------------------------------------
	const char* c_ChannelNameStrings[] = {""};
	const int c_NumChannelStrings = 0;

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
				envtTextureList::BuildFileList(file_list);
				itString tex_fname = tex_loc.GetLastName();
				if (file_list.FindFilePath(tex_fname, tex_loc))
					tex_loc.Push(tex_fname);
			}

			if (fsAbsolutePathMgr::ResolvePath(tex_loc, "Textures"))
			{
				prtyTextureFileData val = io_FilePath.GetFullValue();
				val.m_TextureLocator = tex_loc;
				
				io_FilePath.SetValue(val);
			}
		}
	}

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

		//DBG_LOG2("envt READ : %d %s", uid, name.c_str() );
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void write_name_string(chWriter& o_Writer, const nameString& i_Name)
	{
		std::string name = i_Name.GetString();
		nameUID uid = i_Name.GetUID();

		chChunkParserUtil::Write(o_Writer, uid);
		chChunkParserUtil::Write(o_Writer, name);
	}

	class evmtEnvironmentData
	{
	public:
		nameString m_Name;

		itString m_DiffuseMapName;
		float m_DiffuseFactor;
		itString m_SpecularMapName;
		float m_SpecularFactor;
		float m_DiffuseAngle;
		float m_SpecularAngle;

		std::vector<nameString> m_Objects;
	};
	//------------------------------------------------------------------------
	//   ReadLegacyEnvironmentData
	//------------------------------------------------------------------------
	void ReadLegacyEnvironmentData(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							evmtEnvironmentData& o_Data )
	{
		read_name_string(i_Reader, o_Data.m_Name);

		chChunkParserUtil::Read(i_Reader, o_Data.m_DiffuseMapName);
		chChunkParserUtil::Read(i_Reader, o_Data.m_DiffuseFactor);
		chChunkParserUtil::Read(i_Reader, o_Data.m_SpecularMapName);
		chChunkParserUtil::Read(i_Reader, o_Data.m_SpecularFactor);
		chChunkParserUtil::Read(i_Reader, o_Data.m_DiffuseAngle);
		envType::Int32 num_objects;
		i_Reader.Read(num_objects);
		o_Data.m_Objects.resize(num_objects);
		//DBG_LOG("Objects------------" << num_objects);
		for (int i=0; i<num_objects; i++)
		{
			read_name_string(i_Reader, o_Data.m_Objects[i]);
			//if (i_Version == 1)
			{
				//	for this version of light sets, set the UID to invalid
				//	because it was saving UIDs that didn't match
				//
				//	TODO: [rjk] remove the nameMgr include + this code.
				//
				nameUID uid = o_Data.m_Objects[i].GetUID();
				//DBG_LOG2("- %d (%s)", uid, o_Data.m_Objects[i].GetString().c_str() );
				uid = nameMgr::GetNameUIDFromString(o_Data.m_Objects[i]);
				o_Data.m_Objects[i].SetUID(uid);
				//DBG_LOG2("  %d (%s)", o_Data.m_Objects[i].GetUID(), o_Data.m_Objects[i].GetString().c_str() );
			}
		}

		if (i_Version >= 1)
			chChunkParserUtil::Read(i_Reader, o_Data.m_SpecularAngle);

	}


	//------------------------------------------------------------------------
	//   read_environment_data
	//------------------------------------------------------------------------
	void read_environment_data(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								envtData& o_Data )
	{
		// version 0!
		o_Data.m_DiffuseAngle.Read(i_Reader);
		o_Data.m_DiffuseFactor.Read(i_Reader);
		o_Data.m_DiffuseMapName.ReadTexture(i_Reader);
		resolve_fullpath(o_Data.m_DiffuseMapName);
		o_Data.m_SpecularAngle.Read(i_Reader);
		o_Data.m_SpecularFactor.Read(i_Reader);
		o_Data.m_SpecularMapName.ReadTexture(i_Reader);
		resolve_fullpath(o_Data.m_SpecularMapName);

		// two fields inserted for v1 here:
		if (i_Version >= 1)
		{
			o_Data.m_DiffuseColor.Read(i_Reader);
			o_Data.m_SpecularColor.Read(i_Reader);

			//added ramp data 
			if (i_Version >= 2)
			{
				o_Data.m_RampData.m_Gradient.Read(i_Reader);
				o_Data.m_RampData.m_Shape.Read(i_Reader);
				o_Data.m_RampData.m_Interpolation.Read(i_Reader);
				o_Data.m_RampData.m_TexSize.Read(i_Reader);
				o_Data.m_RampData.m_UWave.Read(i_Reader);
				o_Data.m_RampData.m_UWaveFreq.Read(i_Reader);
				o_Data.m_RampData.m_VWave.Read(i_Reader);
				o_Data.m_RampData.m_VWaveFreq.Read(i_Reader);
				o_Data.m_RampData.m_Noise.Read(i_Reader);
				o_Data.m_RampData.m_NoiseFreq.Read(i_Reader);
			}
		}

		envType::Int32 num_objects;
		i_Reader.Read(num_objects);
		o_Data.m_Objects.resize(num_objects);
		for (int i = 0; i < num_objects; i++)
		{
			read_name_string(i_Reader, o_Data.m_Objects[i]);
		}

		if (i_Version >= 3)
		{
			o_Data.m_SwlData.m_bEnable.Read(i_Reader);
			o_Data.m_SwlData.m_bEnableBG.Read(i_Reader);
		}
	}


	//------------------------------------------------------------------------
	//   read_base_data
	//------------------------------------------------------------------------
	void read_base_data(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							envtData& o_Data )
	{
		// version 0!
		o_Data.m_Name.Read(i_Reader);
		//DBG_LOG2( "Reading envt %d %s", uid, name.c_str() );
	}

	//------------------------------------------------------------------------
	//   read_diffuse_data
	//------------------------------------------------------------------------
	void read_diffuse_data(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							envtData& o_Data )
	{
		// version 0!
		o_Data.m_DiffuseMapName.Read(i_Reader);
		resolve_fullpath(o_Data.m_DiffuseMapName);
	}

	//------------------------------------------------------------------------
	//   read_specular_data
	//------------------------------------------------------------------------
	void read_specular_data( chReader& i_Reader,
							 chDefs::Version i_Version,
							 chDefs::Size i_Size,
							 envtData& o_Data )
	{
		// version 0!
		o_Data.m_SpecularMapName.Read(i_Reader);
		resolve_fullpath(o_Data.m_SpecularMapName);
	}

}	// local namespace

//============================================================================
//============================================================================

//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  envtEnvironmentsDataParser::GetChunkName()
{
	return c_ENVS;
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void envtEnvironmentsDataParser::ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				envtEnvironmentsData& o_Data)
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	if (i_Version >= 1)
	{
		// parse one default environment first.
		i_Reader.ReadChunkHeader(name, version, size);
		DBG_ASSERT( name == c_ENVD, "invalid chunk in environments data");
		ReadEnvironmentData(i_Reader, version, size, o_Data.m_DefaultEnv);
		i_Reader.FinishChunk();
	}

	// now read all the (non-default) environments
	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_ENVD )
		{
			envtScriptData data;
			ReadEnvironmentData(i_Reader, version, size, data);
			o_Data.m_Items.push_back(data);
		}
		else if (name == c_ENVM)
		{
			i_Reader.ReadChunkHeader(name, version, size);
			DBG_ASSERT( name == c_ENVD, "invalid chunk in environments data");
			ReadEnvironmentData(i_Reader, version, size, o_Data.m_SwlEnv);
			i_Reader.FinishChunk();
		}
		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void envtEnvironmentsDataParser::WriteData(	chWriter& o_Writer,
				const envtEnvironmentsData& i_Data  )
{
	const int l_ENVT_VERSION = 2;
	o_Writer.WriteChunkHeader( c_ENVS, l_ENVT_VERSION, true );

	// v1 added default env.
	WriteEnvironmentData(o_Writer, i_Data.m_DefaultEnv);

	const int num_sets = i_Data.m_Items.size();
	for (int i=0; i < num_sets ; i++)
	{
		WriteEnvironmentData(o_Writer, i_Data.m_Items[i]);
	}

	// v2 added swl env
	const int l_ENVM_VERSION = 1;
	o_Writer.WriteChunkHeader( c_ENVM, l_ENVM_VERSION, true );
	WriteEnvironmentData(o_Writer, i_Data.m_SwlEnv);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}



//------------------------------------------------------------------------
//   ReadEnvironmentData
//------------------------------------------------------------------------
void envtEnvironmentsDataParser::ReadEnvironmentData(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						envtScriptData& o_Data )
{
	if (i_Version < 2)
	{
		evmtEnvironmentData data;
		ReadLegacyEnvironmentData(i_Reader, i_Version, i_Size, data);

		o_Data.m_BaseData.m_Name.SetValue(data.m_Name);
		o_Data.m_BaseData.m_DiffuseAngle.SetValue(data.m_DiffuseAngle);
		o_Data.m_BaseData.m_DiffuseFactor.SetValue(data.m_DiffuseFactor);
		if (data.m_DiffuseMapName.GetLength() > 0)
		{
			o_Data.m_BaseData.m_DiffuseMapName.SetValue(fsLocator(data.m_DiffuseMapName));
			resolve_fullpath(o_Data.m_BaseData.m_DiffuseMapName);
		}
		o_Data.m_BaseData.m_SpecularAngle.SetValue(data.m_SpecularAngle);
		o_Data.m_BaseData.m_SpecularFactor.SetValue(data.m_SpecularFactor);
		if (data.m_SpecularMapName.GetLength() > 0)
		{
			o_Data.m_BaseData.m_SpecularMapName.SetValue(fsLocator(data.m_SpecularMapName));
			resolve_fullpath(o_Data.m_BaseData.m_SpecularMapName);
		}

		o_Data.m_BaseData.m_Objects = data.m_Objects;
	}
	else
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;

		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			// Info divided into base chunk and driver list
			if ( name == c_ENVT )
			{
				read_environment_data(i_Reader, version, size, o_Data.m_BaseData);
			}
			else if ( name == c_ENVB )
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
										envtEnvironmentsDataParser::GetChunkName(), 
										o_Data.m_Drivers);
			}
			else if ( name == tmlnChannelInfoParser::GetChunkName() )
			{
				tmlnChannelInfoParser::ReadChannels(i_Reader, o_Data.m_ChannelInfo,
						c_ChannelNameStrings, c_NumChannelStrings);
			}
			else if (name == c_EDTX)
			{	
				read_diffuse_data(i_Reader, version, size, o_Data.m_BaseData);
			}
			else if (name == c_ESTX)
			{	
				read_specular_data(i_Reader, version, size, o_Data.m_BaseData);
			}

			i_Reader.FinishChunk();
		}
	}
}

//------------------------------------------------------------------------
//   WriteEnvironmentData
//------------------------------------------------------------------------
void envtEnvironmentsDataParser::WriteEnvironmentData(	chWriter& o_Writer,
						const envtScriptData& i_Data )
{
	// version 2 moved everything to properties and added channels and drivers
	const int c_ENVD_VERSION = 3;
	o_Writer.WriteChunkHeader( c_ENVD, c_ENVD_VERSION, true );
	
	// Write base info
	const int l_ENVB_VERSION = 0;
	o_Writer.WriteChunkHeader( c_ENVB, l_ENVB_VERSION, false );
	i_Data.m_BaseData.m_Name.Write(o_Writer);
	o_Writer.FinishChunk();

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
							envtEnvironmentsDataParser::GetChunkName(), 
							i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write channels info
	const int l_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), l_CHNI_VERSION, false );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();


	// Write environment info
	//
	//
	const int l_ENVTITEM_VERSION = 3;
	o_Writer.WriteChunkHeader( c_ENVT, l_ENVTITEM_VERSION, false );
	i_Data.m_BaseData.m_DiffuseAngle.Write(o_Writer);
	i_Data.m_BaseData.m_DiffuseFactor.Write(o_Writer);
	i_Data.m_BaseData.m_DiffuseMapName.WriteTexture(o_Writer);
	i_Data.m_BaseData.m_SpecularAngle.Write(o_Writer);
	i_Data.m_BaseData.m_SpecularFactor.Write(o_Writer);
	i_Data.m_BaseData.m_SpecularMapName.WriteTexture(o_Writer);

	// added in v1:
	i_Data.m_BaseData.m_DiffuseColor.Write(o_Writer);
	i_Data.m_BaseData.m_SpecularColor.Write(o_Writer);

	//added in v2:
	i_Data.m_BaseData.m_RampData.m_Gradient.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_Shape.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_Interpolation.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_TexSize.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_UWave.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_UWaveFreq.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_VWave.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_VWaveFreq.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_Noise.Write(o_Writer);
	i_Data.m_BaseData.m_RampData.m_NoiseFreq.Write(o_Writer);

	const int num_objects = i_Data.m_BaseData.m_Objects.size();
	o_Writer.Write(envType::Int32(num_objects));
	for (int i=0; i<num_objects; i++)
	{
		write_name_string(o_Writer, i_Data.m_BaseData.m_Objects[i]);
	}
	// added in v3
	i_Data.m_BaseData.m_SwlData.m_bEnable.Write(o_Writer);
	i_Data.m_BaseData.m_SwlData.m_bEnableBG.Write(o_Writer);

	o_Writer.FinishChunk();

	// version 3 - added new texture control
	// Write diffuse texture data
	const int l_EDTX_VERSION = 0;
	const bool c_bEDTX_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_EDTX, l_EDTX_VERSION, c_bEDTX_CONTAINER_CHUNK );
	i_Data.m_BaseData.m_DiffuseMapName.Write(o_Writer);
	o_Writer.FinishChunk();

	// Write specular texture data
	const int l_ESTX_VERSION = 0;
	const bool c_bESTX_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_ESTX, l_ESTX_VERSION, c_bESTX_CONTAINER_CHUNK );
	i_Data.m_BaseData.m_SpecularMapName.Write(o_Writer);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();
}

/*
	//--------------------------------------------------------------------
	//  Append this data to given environments, merging into existing
	//	sets of the same name
	//--------------------------------------------------------------------
	void MergeData(const evmtEnvironmentsData &i_Data, fsysFileList& i_FileList)
	{
		l_bDisableNotify = true;

		std::vector<evmtEnvironmentData>::const_iterator it, end = i_Data.m_Environments.end();
		for (it = i_Data.m_Environments.begin(); it != end; ++it)
		{
			const evmtEnvironmentData &data = (*it);

			// Only create environment if name does not already exist.
			std::vector<evmtEnvironment*>::iterator layer_it = 
				std::find_if(l_Environments.begin(), l_Environments.end(), name_search<evmtEnvironment>(data.m_Name));
			if  (layer_it == l_Environments.end())
			{
				CreateEnvironment(data.m_Name);
				SetDiffuseMap(data.m_Name, data.m_DiffuseMapName, i_FileList);
				SetSpecularMap(data.m_Name, data.m_SpecularMapName, i_FileList);
				SetDiffuseFactor(data.m_Name, data.m_DiffuseFactor);
				SetSpecularFactor(data.m_Name, data.m_SpecularFactor);
				SetDiffuseAngle(data.m_Name, data.m_DiffuseAngle);
				SetSpecularAngle(data.m_Name, data.m_SpecularAngle);
			}

			const int num_objects = data.m_Objects.size();
			for (int i=0; i<num_objects; ++i)
			{
				AddObjectToEnvironment(data.m_Name, data.m_Objects[i]);
			}
		}
		l_bDisableNotify = false;
		notify_interests_changed();
	}
*/
