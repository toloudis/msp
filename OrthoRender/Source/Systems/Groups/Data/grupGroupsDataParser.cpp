/********************************************************************************************\
**  grupGroupsDataParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Groups/Data/grupGroupsDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_GRUP = chDefs::MakeName('G', 'R', 'U', 'P');
	const chDefs::Name c_GRPD = chDefs::MakeName('G', 'R', 'P', 'D');
	

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
chDefs::Name  grupGroupsDataParser::GetChunkName()
{
	return c_GRUP;
}

//------------------------------------------------------------------------
//   ReadGroupData
//------------------------------------------------------------------------
void grupGroupsDataParser::ReadGroupData(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						grupData& o_Data )
{
	// Version 2 switched to properties, but the file format is
	// compatible with version 1
	o_Data.m_Name.Read(i_Reader);

	// Version 1 removes unused style
	if (i_Version < 1)
	{
		envType::UInt8 style;
		i_Reader.Read(style);
		//o_Data.m_Style = (grpsGroupStyle)style;
	}

	envType::Int32 num_objects;
	i_Reader.Read(num_objects);
	o_Data.m_Objects.resize(num_objects);
	for (int i=0; i<num_objects; i++)
	{
		read_name_string(i_Reader, o_Data.m_Objects[i]);
	}
}

//------------------------------------------------------------------------
//   WriteGroupData
//------------------------------------------------------------------------
void grupGroupsDataParser::WriteGroupData(	chWriter& o_Writer,
						const grupData& i_Data )
{

	const int c_GRPD_VERSION = 2;
	o_Writer.WriteChunkHeader( c_GRPD, c_GRPD_VERSION, false );
	// Version 2 switched to properties
	i_Data.m_Name.Write(o_Writer);

	// version 1 removes unused style
	//envType::UInt8 style = (envType::Int8)i_Data.m_Style;
	//o_Writer.Write(style);

	const int num_objects = i_Data.m_Objects.size();
	o_Writer.Write(envType::Int32(num_objects));
	for (int i=0; i<num_objects; i++)
	{
		write_name_string(o_Writer,i_Data.m_Objects[i]);
	}

	o_Writer.FinishChunk();
}


//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void grupGroupsDataParser::ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				grupGroupsData& o_Data)
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_GRPD )
		{
			grupData data;
			ReadGroupData(i_Reader, version, size, data);
			o_Data.m_Items.push_back(data);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void grupGroupsDataParser::WriteData(	chWriter& o_Writer,
				const grupGroupsData& i_Data  )
{
	o_Writer.WriteChunkHeader( c_GRUP, 0, true );

	const int num_sets = i_Data.m_Items.size();
	for (int i=0; i < num_sets ; i++)
	{
		WriteGroupData(o_Writer, i_Data.m_Items[i]);
	}

	o_Writer.FinishChunk();
}

