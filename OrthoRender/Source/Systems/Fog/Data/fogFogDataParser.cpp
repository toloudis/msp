/********************************************************************************************\
**  fogFogDataParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Fog/Data/fogFogDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ch/chChunkParserUtil.hpp"


namespace
{
//========================================================================
//========================================================================
const chDefs::Name c_FOGD = chDefs::MakeName('F', 'O', 'G', 'D');

// fog base data
const chDefs::Name c_FOGB = chDefs::MakeName('F', 'O', 'G', 'B');

}	// local namespace

//========================================================================
//  returns chunk name for this data type
//========================================================================
chDefs::Name  fogFogDataParser::GetChunkName()
{
	return c_FOGD;
}

//========================================================================
//   ReadLegacyFogData
//========================================================================
void fogFogDataParser::ReadLegacyFogData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					fogFogData& o_Data )
{
	if (i_Version == 0)
	{
		envType::Int32 mode;
		i_Reader.Read(mode);
		o_Data.m_Mode.SetValue(fogFogData::FogMode(mode));
	}
	else
	{
		envType::Int8 mode;
		i_Reader.Read(mode);
		o_Data.m_Mode.SetValue(fogFogData::FogMode(mode));
	}

	o_Data.m_Color.Read(i_Reader);
	o_Data.m_Density.Read(i_Reader);
	o_Data.m_Start.Read(i_Reader);
	o_Data.m_End.Read(i_Reader);
}

//========================================================================
//   ReadFogData
//========================================================================
void fogFogDataParser::ReadFogData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					fogFogData& o_Data )
{
	// FOGB chunk
	o_Data.m_Mode.Read(i_Reader);
	o_Data.m_Color.Read(i_Reader);
	o_Data.m_Density.Read(i_Reader);
	o_Data.m_Start.Read(i_Reader);
	o_Data.m_End.Read(i_Reader);
}

//========================================================================
//   WriteFogData
//========================================================================
void fogFogDataParser::WriteFogData(	chWriter& o_Writer,
					const fogFogData& i_Data )
{
	static int s_Version = 0;
	o_Writer.WriteChunkHeader( c_FOGB, s_Version, false );

	i_Data.m_Mode.Write(o_Writer);
	i_Data.m_Color.Write(o_Writer);
	i_Data.m_Density.Write(o_Writer);
	i_Data.m_Start.Write(o_Writer);
	i_Data.m_End.Write(o_Writer);

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void fogFogDataParser::ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				fogScriptData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	if (i_Version < 2)
	{
		ReadLegacyFogData(i_Reader, i_Version, i_Size, o_Data.m_BaseData);
		return;
	}

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_FOGB )
		{
			ReadFogData(i_Reader, version, size, o_Data.m_BaseData);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void fogFogDataParser::WriteData( chWriter& o_Writer,
					   const fogScriptData& i_Data )
{
	static int s_Version = 2;
	o_Writer.WriteChunkHeader( c_FOGD, s_Version, true );

	WriteFogData(o_Writer, i_Data.m_BaseData);
	
	o_Writer.FinishChunk();
}

