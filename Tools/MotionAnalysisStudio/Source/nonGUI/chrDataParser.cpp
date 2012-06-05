/********************************************************************************************\
**  chrDataParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#include "chrDataParser.hpp"

#include "chBinReader.hpp"
#include "chBinWriter.hpp"
#include "chExceptionX.hpp"
#include "dbgLog.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "gfFileBin.hpp"
#include "chChunkParserUtil.hpp"

namespace chrDataParser
{

namespace
{
//========================================================================
//========================================================================
const chDefs::Name c_CHRD = chDefs::MakeName('C', 'H', 'R', 'D');
const chDefs::Name c_CHRB = chDefs::MakeName('C', 'H', 'R', 'B');
const chDefs::Name c_EXPR = chDefs::MakeName('E', 'X', 'P', 'R');

}


//========================================================================
//  returns chunk name for this data type
//========================================================================
chDefs::Name  GetChunkName()
{
	return c_CHRD;
}


//========================================================================
//   ReadExpression
//========================================================================
void ReadExpression(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				mcpExpression& o_Data )
{
	chChunkParserUtil::Read(i_Reader, o_Data.m_Name);
	chChunkParserUtil::Read(i_Reader, o_Data.m_Filename);
}

//========================================================================
//   WriteExpression
//========================================================================
void WriteExpression(	chWriter& o_Writer,
				const mcpExpression& i_Data )
{

	o_Writer.WriteChunkHeader( c_EXPR, 0, false );
	chChunkParserUtil::Write(o_Writer, i_Data.m_Name);
	chChunkParserUtil::Write(o_Writer, i_Data.m_Filename);
	o_Writer.FinishChunk();
}

//========================================================================
//   ReadData
//========================================================================
void ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				chrData& o_Data )
{

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_CHRB )
		{
			// base data
			chChunkParserUtil::Read(i_Reader, o_Data.m_ModelFilename);
			chChunkParserUtil::Read(i_Reader, o_Data.m_RestAnimFilename);
		}
		else if ( name == c_EXPR )
		{
			// expression
			mcpExpression data;
			ReadExpression(i_Reader, version, size, data);
			o_Data.m_Expressions.push_back(data);
		}
		i_Reader.FinishChunk();
	}
}

//========================================================================
// ReadData
//========================================================================
void ReadData(	const fsLocator &i_Locator,
				chrData& o_Data )
{
	gfFileBin ifile(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
	ifile.ReadHeader();
	chBinReader reader(ifile);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while ( reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_CHRD )
		{
			ReadData(reader, version, size, o_Data);
		}
	}
}

//========================================================================
//   WriteData
//========================================================================
void WriteData(	chWriter& o_Writer,
				const chrData& i_Data )
{
	o_Writer.WriteChunkHeader( c_CHRD, 0, true );

	o_Writer.WriteChunkHeader( c_CHRB, 0, false );
	chChunkParserUtil::Write(o_Writer, i_Data.m_ModelFilename);
	chChunkParserUtil::Write(o_Writer, i_Data.m_RestAnimFilename);
	o_Writer.FinishChunk();

	for (int i=0; i<i_Data.m_Expressions.size(); i++)
		WriteExpression(o_Writer, i_Data.m_Expressions[i]);
	o_Writer.FinishChunk();
}



//========================================================================
// WriteData
//========================================================================
void WriteData( const fsLocator &i_Locator,
				const chrData& i_Data )
{
	// Create new file
	if( fsFileUtil::FileExists(i_Locator) )
		fsFileUtil::DeleteFile(i_Locator);
	fsFileUtil::CreateFile(i_Locator);

	gfFileBin ofile(i_Locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	ofile.WriteHeader();
	chBinWriter writer(ofile);

	WriteData(writer, i_Data);
}


}	// end of namespace

