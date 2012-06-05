/*****************************************************************************
**	effWaterDataParser.cpp
**
**		effWaterDataParser handles the creation of different types shader 
**		data objects.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"
#include "Graphics/eff/effShaderParams.hpp"
#include "Graphics/eff/effWaterData.hpp"
#include "Graphics/eff/effWaterDataParser.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_WATR = chDefs::MakeName('W', 'A', 'T', 'R');
}


//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void effWaterDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effWaterData& data = dynamic_cast<effWaterData &>(o_Shader);

	i_Reader.Read(data.m_FadeBias);
	i_Reader.Read(data.m_FadeExp);
	i_Reader.Read(data.m_NoiseBumpFactor);
	i_Reader.Read(data.m_NoiseSpeed);
	i_Reader.Read(data.m_RingBumpFactor);
	i_Reader.Read(data.m_RingFreq);
	i_Reader.Read(data.m_RingSpeed);
	i_Reader.Read(data.m_TimeOffset);
	i_Reader.Read(data.m_WaveSpeed);
	chChunkParserUtil::Read(i_Reader, data.m_RingCenter);
	chChunkParserUtil::Read(i_Reader, data.m_WaterColor);
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effWaterDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								mdlMaterialInfo& o_MaterialInfo,
								effShaderParams& o_Shader) const
{
	effWaterData data;
	Read(i_Reader, i_Version, i_Size, data);
	data.AddToParams(o_Shader);
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effWaterDataParser::Write( chWriter& o_Writer,
								const effShaderData& i_Shader ) const
{
	const effWaterData& data = dynamic_cast<const effWaterData &>(i_Shader);

	o_Writer.WriteChunkHeader( GetChunkName(), 0, false );

	o_Writer.Write(data.m_FadeBias);
	o_Writer.Write(data.m_FadeExp);
	o_Writer.Write(data.m_NoiseBumpFactor);
	o_Writer.Write(data.m_NoiseSpeed);
	o_Writer.Write(data.m_RingBumpFactor);
	o_Writer.Write(data.m_RingFreq);
	o_Writer.Write(data.m_RingSpeed);
	o_Writer.Write(data.m_TimeOffset);
	o_Writer.Write(data.m_WaveSpeed);
	chChunkParserUtil::Write(o_Writer, data.m_RingCenter);
	chChunkParserUtil::Write(o_Writer, data.m_WaterColor);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effWaterDataParser::Create() const
{
	return new effWaterData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effWaterDataParser::GetChunkName()
{
	return c_WATR;
}
