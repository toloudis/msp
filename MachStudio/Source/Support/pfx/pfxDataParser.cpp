/********************************************************************************************\
**  pfxDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#include "Support/pfx/pfxDataParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"

#include "Graphics/mat/matShaderParser.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//============================================================================
//============================================================================
namespace pfxDataParser
{

namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_PFXD = chDefs::MakeName('P', 'F', 'X', 'D');	// pfx data
const chDefs::Name c_PFXB = chDefs::MakeName('P', 'F', 'X', 'B');	// pfx data
const chDefs::Name c_PFXS = chDefs::MakeName('P', 'F', 'X', 'S');	// pfx data
const chDefs::Name c_SHDT = chDefs::MakeName('S', 'H', 'D', 'T');

void read_base_data(chReader& i_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					pfxData& o_Data )
{
	// version 0
	o_Data.m_bActive.Read(i_Reader);
	o_Data.m_Name.Read(i_Reader);
}

}

//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_PFXD;
}

//------------------------------------------------------------------------
//   ReadPrefsData
//------------------------------------------------------------------------
void ReadPfxData(	chReader& io_Reader, chDefs::Version i_Version,
					pfxData& o_PfxData)
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( io_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_PFXB )
		{
			// read base data
			read_base_data(io_Reader, version, size, o_PfxData);
		}
		else if (name == c_PFXS)
		{
			// read shader data
			while (io_Reader.ReadChunkHeader(name, version, size))
			{
				if (name == c_SHDT)
				{
					mdlMaterialInfo matInfo;
					effShaderParams* pParam;
					pParam = matShaderParser::ReadShaderParams(io_Reader, name, version, size, matInfo);
					o_PfxData.m_pShaderParams.reset(pParam);
					io_Reader.FinishChunk();
				}
			}
		}

		io_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WritePrefsData
//------------------------------------------------------------------------
void WritePfxData(chWriter& o_Writer,
					const pfxData& i_PfxData)
{
	const int l_cPFXD_VERSION = 0;
	o_Writer.WriteChunkHeader( c_PFXD, l_cPFXD_VERSION, true );

	// Write base info
	const int l_cPFXB_VERSION = 0; 
	o_Writer.WriteChunkHeader( c_PFXB, l_cPFXB_VERSION, false );

	// added v0
	i_PfxData.m_bActive.Write(o_Writer);
	i_PfxData.m_Name.Write(o_Writer);
	o_Writer.FinishChunk();	// cPFXB

	// Write shader info
	const int l_cPFXS_VERSION = 0;
	o_Writer.WriteChunkHeader( c_PFXS, l_cPFXS_VERSION, true );
	matShaderParser::WriteShader(o_Writer, i_PfxData.m_pShaderParams);
	o_Writer.FinishChunk();	// cPFXS
	

	o_Writer.FinishChunk();	// c_PFXD
}

}