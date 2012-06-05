/*****************************************************************************
**	effBakeDataParser.cpp
**
**		effBakeDataParser handles the creation of different types shader 
**		data objects.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effBakeDataParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"
#include "Graphics/eff/effBakeData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_BAKE = chDefs::MakeName('B', 'A', 'K', 'E');
}


//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effBakeDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effBakeData& data = dynamic_cast<effBakeData &>(o_Shader);

	chChunkParserUtil::Read(i_Reader, data.m_ColorEmissive);
	i_Reader.Read(data.m_NameEmissive);
	i_Reader.Read(data.m_NameNormalMap);

}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effBakeDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								mdlMaterialInfo& o_MaterialInfo,
								effShaderParams& o_Shader) const
{
	effBakeData data;
	Read(i_Reader, i_Version, i_Size, data);
	data.AddToParams(o_Shader);
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effBakeDataParser::Write( chWriter& o_Writer,
								const effShaderData& i_Shader ) const
{
	const effBakeData& data = dynamic_cast<const effBakeData &>(i_Shader);

	static const chDefs::Version s_Version = 0;
	o_Writer.WriteChunkHeader( GetChunkName(), s_Version, false );

	chChunkParserUtil::Write(o_Writer, data.m_ColorEmissive);
	o_Writer.Write(data.m_NameEmissive);
	o_Writer.Write(data.m_NameNormalMap);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effBakeDataParser::Create() const
{
	return new effBakeData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effBakeDataParser::GetChunkName()
{
	return c_BAKE;
}
