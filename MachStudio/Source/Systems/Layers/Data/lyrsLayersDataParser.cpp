/********************************************************************************************\
**  lyrsLayersDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Layers/Data/lyrsLayersDataParser.hpp"
#include "Support/lyer/lyerLayerStyle.hpp"

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
	const chDefs::Name c_LYRS = chDefs::MakeName('L', 'Y', 'R', 'S');
	const chDefs::Name c_LYRD = chDefs::MakeName('L', 'Y', 'R', 'D');
	

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
chDefs::Name  lyrsLayersDataParser::GetChunkName()
{
	return c_LYRS;
}

//------------------------------------------------------------------------
//   ReadLayerData
//------------------------------------------------------------------------
void lyrsLayersDataParser::ReadLayerData(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						lyrsData& o_Data )
{
	// Version 2 switched to properties, but the file format is
	// compatible with version 1
	o_Data.m_Name.Read(i_Reader);

	if (i_Version >= 1)
	{
		// Version 1 writes separate flags for each aspect
		o_Data.m_bVisible.Read(i_Reader);
		o_Data.m_bPickable.Read(i_Reader);
		o_Data.m_bWireframe.Read(i_Reader);
		o_Data.m_bLowRes.Read(i_Reader);
	}
	else
	{
		// Version 0 wrote an enumeration
		envType::UInt8 style;
		i_Reader.Read(style);
		//o_Data.m_Style = (lyerLayerStyle)style;

		switch((lyerLayerStyle)style)
		{
		case e_Normal:
			o_Data.m_bVisible = true;
			o_Data.m_bPickable = true;
			o_Data.m_bWireframe = false;
			o_Data.m_bLowRes = false;
			break;
		case e_Wireframe:
			o_Data.m_bVisible = true;
			o_Data.m_bPickable = false;
			o_Data.m_bWireframe = true;
			o_Data.m_bLowRes = false;
			break;
		case e_Invisible:
			o_Data.m_bVisible = false;
			o_Data.m_bPickable = false;
			o_Data.m_bWireframe = false;
			o_Data.m_bLowRes = false;
			break;
		}
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
//   WriteLayerData
//------------------------------------------------------------------------
void lyrsLayersDataParser::WriteLayerData(	chWriter& o_Writer,
						const lyrsData& i_Data )
{
	const int c_LYRD_VERSION = 2;
	o_Writer.WriteChunkHeader( c_LYRD, c_LYRD_VERSION, false );

	// Version 2 switched to properties
	i_Data.m_Name.Write(o_Writer);
	i_Data.m_bVisible.Write(o_Writer);
	i_Data.m_bPickable.Write(o_Writer);
	i_Data.m_bWireframe.Write(o_Writer);
	i_Data.m_bLowRes.Write(o_Writer);

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
void lyrsLayersDataParser::ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				lyrsLayersData& o_Data)
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_LYRD )
		{
			lyrsData data;
			ReadLayerData(i_Reader, version, size, data);
			o_Data.m_Items.push_back(data);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void lyrsLayersDataParser::WriteData(	chWriter& o_Writer,
				const lyrsLayersData& i_Data  )
{
	o_Writer.WriteChunkHeader( c_LYRS, 0, true );

	const int num_sets = i_Data.m_Items.size();
	for (int i=0; i < num_sets ; i++)
	{
		WriteLayerData(o_Writer, i_Data.m_Items[i]);
	}

	o_Writer.FinishChunk();
}

