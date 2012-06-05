/********************************************************************************************\
**  trfnTransformsDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Transforms/Data/trfnTransformsDataParser.hpp"

#include "Support/tmln/tmlnChannelInfoParser.hpp"
#include "Support/tmln/tmlnParser.hpp"
#include "Support/xtra/xtraPropertyDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_XFMS = chDefs::MakeName('X', 'F', 'M', 'S');
	const chDefs::Name c_XFMC = chDefs::MakeName('X', 'F', 'M', 'C');
	const chDefs::Name c_XFMD = chDefs::MakeName('X', 'F', 'M', 'D');
	const chDefs::Name c_XFMO = chDefs::MakeName('X', 'F', 'M', 'O');
	const chDefs::Name c_DRVS = chDefs::MakeName('D', 'R', 'V', 'S');	//	drivers (part of chunk)
	const chDefs::Name c_XTRP = chDefs::MakeName('X', 'T', 'R', 'P');	//	extra properties (part of chunk)


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

}	// local namespace

//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  trfnTransformsDataParser::GetChunkName()
{
	return c_XFMS;
}

//------------------------------------------------------------------------
//   ReadTransformData
//------------------------------------------------------------------------
void trfnTransformsDataParser::ReadTransformData(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						trfnScriptData& o_Data )
{	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_XFMD )
		{
			o_Data.m_BaseData.m_Name.Read(i_Reader);
			o_Data.m_BaseData.m_Position.Read(i_Reader);
			o_Data.m_BaseData.m_Orientation.Read(i_Reader);
			o_Data.m_BaseData.m_bVisible.Read(i_Reader);
			o_Data.m_BaseData.m_Scale.Read(i_Reader);
			o_Data.m_BaseData.m_PivotPoint.Read(i_Reader);
			o_Data.m_BaseData.m_PivotCompensation.Read(i_Reader);

			if (version >= 1)
			{
				// version 1 adds pickable and wireframe flags
				o_Data.m_BaseData.m_bPickable.Read(i_Reader);
				o_Data.m_BaseData.m_bWireframe.Read(i_Reader);
			}

			if (version >= 2)
			{
				// version 1 adds "inherits transform" flag
				o_Data.m_BaseData.m_bInheritsTransform.Read(i_Reader);
			}
		}
		else if ( name == c_XFMO )
		{
			envType::Int32 num_objects;
			i_Reader.Read(num_objects);
			o_Data.m_BaseData.m_Objects.resize(num_objects);
			for (int i=0; i<num_objects; i++)
			{
				read_name_string(i_Reader, o_Data.m_BaseData.m_Objects[i]);
			}		
		}
		else if ( name == c_XTRP )
		{
			// Read custom property info
			xtraPropertyDataParser::ReadCustomPropertyData(i_Reader, version, size, o_Data.m_CustomProperties);
		}
		else if ( name == c_DRVS )
		{
			//DBG_TRACE("chtr Drivers");
			tmlnParser::ReadDrivers(i_Reader, 
									trfnTransformsDataParser::GetChunkName(),  
									(o_Data.m_Drivers) );
		}
		else if ( name == tmlnChannelInfoParser::GetChunkName() )
		{
			//DBG_TRACE("chtr Channel Info");
			tmlnChannelInfoParser::ReadChannels(i_Reader, 
												o_Data.m_ChannelInfo);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteTransformData
//------------------------------------------------------------------------
void trfnTransformsDataParser::WriteTransformData(	chWriter& o_Writer,
						const trfnScriptData& i_Data )
{
	// Container chunk
	const int c_XFMC_VERSION = 0;
	const bool c_bXFMC_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_XFMC, c_XFMC_VERSION, c_bXFMC_CONTAINER_CHUNK );

	// Basic data
	const int c_XFMD_VERSION = 2;
	o_Writer.WriteChunkHeader( c_XFMD, c_XFMD_VERSION, false );
	i_Data.m_BaseData.m_Name.Write(o_Writer);
	i_Data.m_BaseData.m_Position.Write(o_Writer);
	i_Data.m_BaseData.m_Orientation.Write(o_Writer);
	i_Data.m_BaseData.m_bVisible.Write(o_Writer);
	i_Data.m_BaseData.m_Scale.Write(o_Writer);
	i_Data.m_BaseData.m_PivotPoint.Write(o_Writer);
	i_Data.m_BaseData.m_PivotCompensation.Write(o_Writer);

	// version 1 adds pickable and wireframe flags
	i_Data.m_BaseData.m_bPickable.Write(o_Writer);
	i_Data.m_BaseData.m_bWireframe.Write(o_Writer);

	// version 2 adds "inherits transform" flag
	i_Data.m_BaseData.m_bInheritsTransform.Write(o_Writer);

	o_Writer.FinishChunk();

	// List of objects
	const int c_XFMO_VERSION = 0;
	o_Writer.WriteChunkHeader( c_XFMO, c_XFMO_VERSION, false );
	const int num_objects = i_Data.m_BaseData.m_Objects.size();
	o_Writer.Write(envType::Int32(num_objects));
	for (int i=0; i<num_objects; i++)
	{
		write_name_string(o_Writer,i_Data.m_BaseData.m_Objects[i]);
	}
	o_Writer.FinishChunk();
	
	// Write custom property info
	const int c_XTRP_VERSION = 0;
	const bool c_bXTRP_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_XTRP, c_XTRP_VERSION, c_bXTRP_CONTAINER_CHUNK );
	xtraPropertyDataParser::WriteCustomPropertyData(o_Writer, i_Data.m_CustomProperties);
	o_Writer.FinishChunk();

	const int c_DRVS_VERSION = 4;
	const bool c_bDRVS_CONTAINER_CHUNK = true;
	o_Writer.WriteChunkHeader( c_DRVS, c_DRVS_VERSION, c_bDRVS_CONTAINER_CHUNK );
	tmlnParser::WriteDrivers(o_Writer, 
							trfnTransformsDataParser::GetChunkName(),  
							i_Data.m_Drivers);
	o_Writer.FinishChunk();

	// Write channels info
	const int c_CHNI_VERSION = 0;
	o_Writer.WriteChunkHeader( tmlnChannelInfoParser::GetChunkName(), c_CHNI_VERSION, true );
	tmlnChannelInfoParser::WriteChannels(o_Writer, i_Data.m_ChannelInfo);
	o_Writer.FinishChunk();

	o_Writer.FinishChunk();	// XMFC Container Chunk
}


//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void trfnTransformsDataParser::ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				trfnTransformsData& o_Data)
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_XFMC )
		{
			trfnScriptData data;
			ReadTransformData(i_Reader, version, size, data);
			o_Data.m_Items.push_back(data);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void trfnTransformsDataParser::WriteData(	chWriter& o_Writer,
				const trfnTransformsData& i_Data  )
{
	o_Writer.WriteChunkHeader( c_XFMS, 0, true );

	const int num_sets = i_Data.m_Items.size();
	for (int i=0; i < num_sets ; i++)
	{
		WriteTransformData(o_Writer, i_Data.m_Items[i]);
	}

	o_Writer.FinishChunk();
}

