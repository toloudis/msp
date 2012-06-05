/********************************************************************************************\
**  aoAODataParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Systems/AmbientOcclusion/Data/aoAODataParser.hpp"

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
const chDefs::Name c_AOCD = chDefs::MakeName('A', 'O', 'C', 'D');

// ao base data
const chDefs::Name c_AOCB = chDefs::MakeName('A', 'O', 'C', 'B');

}	// local namespace

//========================================================================
//  returns chunk name for this data type
//========================================================================
chDefs::Name  aoAODataParser::GetChunkName()
{
	return c_AOCD;
}

//========================================================================
//   ReadFogData
//========================================================================
void aoAODataParser::ReadAOData(	chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					aoAOData& o_Data )
{
	// AOCB chunk
	o_Data.m_AngleBias.Read(i_Reader);
	o_Data.m_AORadius.Read(i_Reader);
	o_Data.m_Attenuation.Read(i_Reader);
	o_Data.m_BlurSharpness.Read(i_Reader);
	o_Data.m_BlurWidth.Read(i_Reader);

	// contrast was removed in v1
	if (i_Version < 1)
	{
		prtyFloat contrast;
		contrast.Read(i_Reader);
	}
	else if (i_Version > 1)
	{
		// reinserted for v2.
		o_Data.m_Contrast.Read(i_Reader);
	}
}

//========================================================================
//   WriteAOData
//========================================================================
void aoAODataParser::WriteAOData(	chWriter& o_Writer,
					const aoAOData& i_Data )
{
	static int s_Version = 2;
	o_Writer.WriteChunkHeader( c_AOCB, s_Version, false );

	i_Data.m_AngleBias.Write(o_Writer);
	i_Data.m_AORadius.Write(o_Writer);
	i_Data.m_Attenuation.Write(o_Writer);
	i_Data.m_BlurSharpness.Write(o_Writer);
	i_Data.m_BlurWidth.Write(o_Writer);
//	i_Data.m_Contrast.Write(o_Writer); // removed in v1
	i_Data.m_Contrast.Write(o_Writer); // added in v2

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void aoAODataParser::ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				aoScriptData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_AOCB )
		{
			ReadAOData(i_Reader, version, size, o_Data.m_BaseData);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void aoAODataParser::WriteData( chWriter& o_Writer,
					   const aoScriptData& i_Data )
{
	static int s_Version = 0;
	o_Writer.WriteChunkHeader( c_AOCD, s_Version, true );

	WriteAOData(o_Writer, i_Data.m_BaseData);
	
	o_Writer.FinishChunk();
}

