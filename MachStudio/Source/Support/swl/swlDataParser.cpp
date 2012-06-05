/********************************************************************************************\
**  swlDataParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#include "Support/swl/swlDataParser.hpp"

#include "Support/swl/swlData.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace swlDataParser
{

namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_SWLD = chDefs::MakeName('S', 'W', 'L', 'D');	// fragment data

	//========================================================================
	//   ReadFragmentData
	//========================================================================
	void ReadSwlData(	chReader& i_Reader,
					chDefs::Version i_Version,
					swlData& o_Data )
	{
		if (i_Version >= 0)
		{
			o_Data.m_bEnable.Read(i_Reader);
		}
	}

	//========================================================================
	//   WriteFragmentData
	//========================================================================
	void WriteSwlData(	chWriter& o_Writer,
						const swlData& i_Data )
	{
		static const int s_Version = 0;
		o_Writer.WriteChunkHeader( c_SWLD, s_Version, false );

		i_Data.m_bEnable.Write(o_Writer);

		o_Writer.FinishChunk();
	}
}



//========================================================================
//   ReadFragmentData
//========================================================================
void ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				swlData& o_data)
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if (name == c_SWLD)
		{
			swlData data;
			ReadSwlData(i_Reader, version, data);
			o_data = data;
		}
		i_Reader.FinishChunk();
	}
}

//========================================================================
//   WriteFragmentData
//========================================================================
void WriteData(	chWriter& o_Writer,
				const swlData& i_data)
{
	WriteSwlData( o_Writer, i_data );
}


}	// end of namespace
